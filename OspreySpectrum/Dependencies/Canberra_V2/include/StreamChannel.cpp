#include "CommandData.h"
#include "SerializableObject.h"
#include "SerializableType.h"
#include "pbsmsgs_h.h"
#include "ApplicationSerializer.h"
#include "LockManager.h"
#include "StreamChannel.h"
#ifdef WIN32
#include <wininet.h>
#else
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#endif

using namespace Canberra::Utility::Network;
using namespace Canberra::Serialization;
using namespace Canberra::Utility::Core;
using namespace Canberra::DataTypes::Serialization;
using namespace Canberra::DataTypes::Application;

#ifdef WIN32
#define PLATFORM_INDEP_WOULDBLOCK WSAEWOULDBLOCK
#else
#define PLATFORM_INDEP_WOULDBLOCK EAGAIN
#endif

///<summary>
///This is the constructor for this class instance.  An instance
///of this class should be used to extract streamed measurements
///passed from the Lynx to the client application
///</summary>
Canberra::Protocols::Lynx::StreamChannel::StreamChannel(MessageFactory &MF) :
	_MessageFactory(MF), _Channel(String(L"StreamChannel")), _Lock(String(L"StreamLock")),
	_Abort(false), _Enable(false),  _Settings(16386)
{
}

///<summary>
///This is the destructor for this class instance.  This
///method will release all allocated resources
///</summary>
Canberra::Protocols::Lynx::StreamChannel::~StreamChannel() {
	Close();
}

///<summary>
///This method establishes a connection between
///the client and the lynx.
///</summary>
///<param name="Client">The IP address of the client. If "" then local address is discovered</param>
///<param name="Dest">The IP address of the device.</param>
///<returns>0==Success</returns>
ULONG Canberra::Protocols::Lynx::StreamChannel::Open(const String &Client, const String &Dest) {
	LockManager LM(&_Lock);
	if (!_Enable) return 0;

	//Create the local connection to the network adapter
	ULONG Stat = _Channel.Open(EndPoint(Client, 0));
	if (Stat) {
		DEBUG_ERROR_MESSAGE(L"StreamChannel", L"Open", L"Error opening streaming interface.", Stat);
		return Stat;
	}

	//Setup the channel (Note: It is not fatal if there are any errors with these calls.  Hence, not returned to caller)
	int Size=sizeof(_Settings.SendTimeOut);
#ifdef WIN32
	Stat=_Channel.SetOption(SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char *>(&_Settings.SendTimeOut), Size);
	if (Stat) {DEBUG_ERROR_MESSAGE(L"StreamChannel", L"Open", L"Error setting SO_SNDTIMEO", Stat);}
	Size=sizeof(_Settings.RecvTimeOut);
	Stat=_Channel.SetOption(SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&_Settings.RecvTimeOut), Size);
	if (Stat) {DEBUG_ERROR_MESSAGE(L"StreamChannel", L"Open", L"Error setting SO_RCVTIMEO", Stat);}
#endif
	Size=sizeof(_Settings.SendBufferSize);
	Stat=_Channel.SetOption(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char *>(&_Settings.SendBufferSize), Size);
	if (Stat) {DEBUG_ERROR_MESSAGE(L"StreamChannel", L"Open", L"Error setting SO_SNDBUF", Stat);}
	Size=sizeof(_Settings.RecvBufferSize);
	Stat=_Channel.SetOption(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&_Settings.RecvBufferSize), Size);
	if (Stat) {DEBUG_ERROR_MESSAGE(L"StreamChannel", L"Open", L"Error setting SO_RCVBUF", Stat);}
	Size=sizeof(_Settings.NoDelay);
	Stat=_Channel.SetOption(IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&_Settings.NoDelay), Size);
	if (Stat) {DEBUG_ERROR_MESSAGE(L"StreamChannel", L"Open", L"Error setting TCP_NODELAY", Stat);}
	Size=sizeof(_Settings.KeepAlive);
	Stat=_Channel.SetOption(SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char *>(&_Settings.KeepAlive), Size);
	if (Stat) {DEBUG_ERROR_MESSAGE(L"StreamChannel", L"Open", L"Error setting TCP_KEEPAlIVE", Stat);}

	//Init the client endpoint info
	_ClientEP = EndPoint(Client, 0);

	//Connect to the device
	Stat = _Channel.Connect(EndPoint(Dest, _Settings.Port));
	if (Stat) {
		_Channel.Close();
		DEBUG_ERROR_MESSAGE(L"StreamChannel", L"Open", L"Error connecting streaming interface to device.", Stat);
		return Stat;
	}
	_DevEP = EndPoint(Dest, 0);

	//Turn off blocking mode because streaming requires it to be off
	_Channel.PutIsBlocking(false);

	return Stat;
}

///<summary>
///This method terminates the connection between
///the client and the lynx.
///</summary>
///<returns>0==Success</returns>
ULONG Canberra::Protocols::Lynx::StreamChannel::Close() {
	if (!_Channel.GetIsOpen()) return 0;

	LockManager LM(&_Lock);
	_Channel.Shutdown(SD_BOTH);
	_Channel.Close();

	return 0;
}


///<summary>
///This method will issue a recv socket call on a socket configured either for
///blocking or nonblocking.  This method will block until all of the requested
///information is received.  The size of the byte stream indicates what this
///method will block for.  This method was written to override the Receive
///implementation contained within the TcpClient class.  The reason for
///overriding it is to allow for aborting
///a non-blocking receive call.
///</summary>
///<param name="Stream">The stream recv'd</param>
///<param name="NumReceived">The number of bytes recv'd</param>
///<param name="Flags">The flags</param>
///<returns>Win32 code.  0==Sucess</returns>
ULONG Canberra::Protocols::Lynx::StreamChannel::Receive(ByteStream &Stream, ULONG *Recv, LONG Flags)
{
	LockManager LM(&_Lock);

	if (Recv) *Recv=0;
	if (!_Channel.GetIsOpen()) {
		ULONG Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("StreamChannel"), _T("Receive"), _T("Socket is not open."), Stat);
		return Stat;
	}
	if (NULL == Stream.GetBuffer()) {
		ULONG Stat=(ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(_T("StreamChannel"), _T("Receive"), _T("Buffer is null."), Stat);
		return Stat;
	}

	ULONG NBytesRecv=0, NBufBytes=0;
	ULONG BufSize = Stream.GetSize();
	const char * const Buf = Stream.GetBuffer();
	while(NBufBytes < BufSize) {
		if (_Abort) {
			//
			//We are closing the socket and opening a new one
			//because an abort will cause the data stream to
			//be out of sync.  Once out of sync it will become
			//very complicated to resync device with client
			//Therefore, simply close the connection and open
			//a new one.
			//

			//Close the socket
			LM.Unlock();
			Close();

			//Open a new one for fresh communications
			Open(_ClientEP.GetAddressString(), _DevEP.GetAddressString());
			return 0;
		}

		LONG Stat=recv(_Channel.GetHandle(), const_cast<char *>(Buf+NBufBytes), BufSize-NBufBytes, Flags);
		if (SOCKET_ERROR == Stat) {
			ULONG Stat = WSAGetLastError();
			if (PLATFORM_INDEP_WOULDBLOCK == Stat) {
#ifdef WIN32
				Sleep(0); //Yield
#else
				sched_yield();
#endif
				continue;
			}
			DEBUG_ERROR_MESSAGE(_T("StreamChannel"), _T("Receive"), _T("Error recv."), Stat);
			return Stat;
		}
		else if (Stat == 0) break;

		NBytesRecv += Stat;
		NBufBytes += Stat;
	}
	if (Recv) *Recv=NBytesRecv;
	return 0;
}

///<summary>
///This method gets a command that has been streamed to this
///client from the Lynx.
///</summary>
///<param name="Resp">The command response that was received</param>
///<returns>0==Success</returns>
ULONG Canberra::Protocols::Lynx::StreamChannel::ReceiveCommand(CommandData **Resp) {
	if (NULL == Resp) {
		DEBUG_ERROR_MESSAGE(L"StreamChannel", L"ReceiveCommand", L"Argument passed was NULL", E_INVALIDARG);
		return (ULONG) E_INVALIDARG;
	}

	*Resp = NULL;
	if (!_Enable) {
		DEBUG_TYPED_MESSAGE(L"StreamChannel", L"ReceiveCommand", L"This method did not receive a command because streaming is disabled", Canberra::Utility::Core::Diagnostics::Warning);
		return 0;
	}

	//Send the command
	ByteStream Str;
	ULONG Size=0;
	ULONG MsgSize=0;
	Str.Clear();
	Str.Allocate(sizeof(MsgSize));

	//Get the size of the response message (1'st 4 bytes)
	ULONG Stat = Receive(Str, &Size);
	if (_Abort) {
		DEBUG_TYPED_MESSAGE(L"StreamChannel", L"ReceiveCommand", L"Abort flag set operation aborted", Canberra::Utility::Core::Diagnostics::Warning);
		return (ULONG) E_ABORT;
	}
	if (Stat || !Size) {
		if (!Stat) Stat = WSAECONNRESET;
		DEBUG_ERROR_MESSAGE(L"StreamChannel", L"ReceiveCommand", L"Error while receiving device response header.", Stat);
		return Stat;
	}

	//Allocate memory for the rest of the message and read
	memcpy(&MsgSize, Str.GetBuffer(), sizeof(MsgSize));
	Canberra::Utility::Core::ByteStream Frag;
	LONG PSize=MsgSize-sizeof(MsgSize);
	if (PSize < 0) {
		Stat = (ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(L"StreamChannel", L"ReceiveCommand", L"Error while receiving packet size.  Size is < 1", Stat);
		return Stat;
	}

	Frag.Allocate(PSize);
	Stat = Receive(Frag, &Size);
	if (_Abort) {
		DEBUG_TYPED_MESSAGE(L"StreamChannel", L"ReceiveCommand", L"Abort flag set operation aborted", Canberra::Utility::Core::Diagnostics::Warning);
		return (ULONG) E_ABORT;
	}
	if (Stat || !Size) {
		if (!Stat) Stat = WSAECONNRESET;
		DEBUG_ERROR_MESSAGE(L"StreamChannel", L"ReceiveCommand", L"Error while receiving device response body.", Stat);
		return Stat;
	}

	Stat=0;
	SerializableObject *Obj=NULL;
	Str.Copy(Frag, 0, sizeof(MsgSize));
	try {
		Obj = _MessageFactory.DeserializeFromMessage(Str);
	}
	CATCHTRACE(L"StreamChannel", L"ReceiveCommand", Stat);
	if (Stat) {
		delete Obj;
		return Stat;
	}

	*Resp = dynamic_cast<CommandData *>(Obj);
	if (!Resp) {
		delete Obj;
		Stat = GEN_INVALID_RESPONSE;
		DEBUG_ERROR_MESSAGE(L"StreamChannel", L"ReceiveCommand", L"Error device response is invalid. Argument is not a ListData nor TlistData type.", Stat);
		goto EXIT;
	}

EXIT:
	return Stat;
}


///<summary>
///This method gets the value of a named property.
///</summary>
///<param name="N">The property name.</param>
///<param name="V">The property value.</param>
///<returns>0==Success</returns>
ULONG Canberra::Protocols::Lynx::StreamChannel::GetProperty(const String &Name, Variant &V) {
	ULONG Stat=0;
	String &N = const_cast<String &>(Name);

	V.Clear();
	if (!N.CompareI(L"SendTimeout")) {
		V.Assign((LONG) _Settings.SendTimeOut);
	}
	else if (!N.CompareI(L"ReceiveTimeout")) {
		V.Assign((LONG) _Settings.RecvTimeOut);
	}
	else if (!N.CompareI(L"SendBufferSize")) {
		V.Assign((LONG) _Settings.SendBufferSize);
	}
	else if (!N.CompareI(L"ReceiveBufferSize")) {
		V.Assign((LONG) _Settings.RecvBufferSize);
	}
	else if (!N.CompareI(L"KeepAlive")) {
		V.Assign((LONG) _Settings.KeepAlive);
	}
	else if (!N.CompareI(L"NoDelay")) {
		V.Assign((LONG) _Settings.NoDelay);
	}
	else if (!N.CompareI(L"StreamingDataAvailable")) {
		LockManager LM(&_Lock);
		if (_Enable && _Channel.GetIsOpen()) V.Assign(_Channel.GetIsReceiving());
		else V.Assign((bool) false);
	}
	else if (!N.CompareI(L"StreamingPort")) {
		V.Assign(_Settings.Port);
	}
	else if (!N.CompareI(L"Enable") || !N.CompareI(L"UseStream") || !N.CompareI(L"EnableStreaming")) {
		V.Assign(_Enable);
	}
	else if (!N.CompareI(L"Aborted") || !N.CompareI(L"Abort") || !N.CompareI(L"AbortStreaming")) {
		V.Assign(_Abort);
	}
	else if (!N.CompareI(L"Active") || !N.CompareI(L"IsActive")) {
		V.Assign(GetActive());
	}
	return Stat;
}

///<summary>
///This method sets the value of a named property.
///</summary>
///<param name="Name">The property name.</param>
///<param name="Val">The property value.</param>
///<returns>0==Success</returns>
ULONG Canberra::Protocols::Lynx::StreamChannel::PutProperty(const String &Name, const Variant &Val) {
	ULONG Stat=0;
	String &N = const_cast<String &>(Name);
	Variant &V = const_cast<Variant &>(Val);

	if (!N.CompareI(L"SendTimeout")) {
		int Val=V.ToLong();

		LockManager LM(&_Lock);
		if (_Channel.GetIsOpen()) {
#ifdef WIN32
			int Size=sizeof(Val);
			Stat = _Channel.SetOption(SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char *>(&Val), Size);
#endif
			if (!Stat) _Settings.SendTimeOut=(Val);
		}
		else _Settings.SendTimeOut=(Val);
	}
	else if (!N.CompareI(L"ReceiveTimeout")) {
		int Val=V.ToLong();

		LockManager LM(&_Lock);
		if (_Channel.GetIsOpen()) {
#ifdef WIN32
			int Size=sizeof(Val);
			Stat = _Channel.SetOption(SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&Val), Size);
#endif
			if (!Stat) _Settings.RecvTimeOut=(Val);
		}
		else _Settings.RecvTimeOut=(Val);
	}
	else if (!N.CompareI(L"SendBufferSize")) {
		int Val=V.ToLong();
		int Size=sizeof(Val);

		LockManager LM(&_Lock);
		if (_Channel.GetIsOpen()) {
			Stat=_Channel.SetOption(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char *>(&Val), Size);
			if (!Stat) _Settings.SendBufferSize=(Val);
		}
		else _Settings.SendBufferSize=(Val);
	}
	else if (!N.CompareI(L"ReceiveBufferSize")) {
		int Val=V.ToLong();
		int Size=sizeof(Val);

		LockManager LM(&_Lock);
		if (_Channel.GetIsOpen()) {
			Stat=_Channel.SetOption(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&Val), Size);
			if (!Stat) _Settings.RecvBufferSize=(Val);
		}
		else _Settings.RecvBufferSize = Val;
	}
	else if (!N.CompareI(L"KeepAlive")) {
		BOOL Val=(V.ToLong()) ? TRUE : FALSE;
		int Size=sizeof(Val);

		LockManager LM(&_Lock);
		if (_Channel.GetIsOpen()) {
			Stat=_Channel.SetOption(SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char *>(&Val), Size);
			if (!Stat) _Settings.KeepAlive = Val;
		}
		else _Settings.KeepAlive=Val;
	}
	else if (!N.CompareI(L"NoDelay")) {
		BOOL Val=(V.ToLong()) ? TRUE : FALSE;
		int Size=sizeof(Val);

		LockManager LM(&_Lock);
		if (_Channel.GetIsOpen()) {
			Stat=_Channel.SetOption(IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&Val), Size);
			if (!Stat) _Settings.NoDelay=(Val);
		}
		else _Settings.NoDelay=(Val);
	}
	else if (!N.CompareI(L"StreamingPort")) {
		_Settings.Port = V.ToUshort();
	}
	else if (!N.CompareI(L"Enable") || !N.CompareI(L"UseStream") || !N.CompareI(L"EnableStreaming")) {
		_Enable = V.ToBool();
	}
	else if (!N.CompareI(L"Aborted") || !N.CompareI(L"Abort") || !N.CompareI(L"AbortStreaming")) {
		_Abort = V.ToBool();
	}
	return Stat;
}

///<summary>
///This method gets the value of the enable state
///</summary>
///<returns>The value</returns>
bool Canberra::Protocols::Lynx::StreamChannel::GetEnable() const {return _Enable;}

///<summary>
///This method sets the value of the enable state
///</summary>
///<param name="v">The value.</param>
///<returns>None</returns>
void Canberra::Protocols::Lynx::StreamChannel::PutEnable(const bool v) {_Enable=v;}


///<summary>
///This method gets the value of the enable active state
///Active state means that streaming is enabled and the
///streaming channel is open.
///</summary>
///<returns>The value</returns>
bool Canberra::Protocols::Lynx::StreamChannel::GetActive() {
	if (_Enable && _Channel.GetIsOpen()) return true;
	else return false;
}
