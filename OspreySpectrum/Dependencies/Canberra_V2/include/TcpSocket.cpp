#include "TcpSocket.h"
#include "DebugMacros.h"
using namespace Canberra::Utility::Core;


///<summary>
///An instance of this class is used to manage
///a TCP socket connection
///</summary>
///<param name="Name">Name of the socket</param>
///<exception cref"ULONG">Error info</exception>
Canberra::Utility::Network::TcpSocket::TcpSocket(const String &Name) :
	Socket(Name), _Interrupt(false) {
	_Type = SOCK_STREAM;
	_Protocol=IPPROTO_TCP;
}
Canberra::Utility::Network::TcpSocket::~TcpSocket(void) {}

///<summary>
///This method will attach a socket handle to this
///instance
///</summary>
///<param name="S">The socket to attach</param>
void Canberra::Utility::Network::TcpSocket::Attach(SOCKET S) {if (GetIsOpen()) {Close();} _Socket = S;}

///<summary>
///This method will issue a blocking/nonblocking send.  The method will not
///return until all data has been sent.  You can interrupt the sending
///by invoking Interrupt().  However, you must invoke ResetInterrupt()
///make subsequent calls to this routine after invoking Interrupt()
///</summary>
///<param name="Stream">The stream send</param>
///<param name="NumReceived">The number of bytes sent</param>
///<param name="Flags">The flags</param>
///<returns>Win32 code.  0==Sucess</returns>
ULONG Canberra::Utility::Network::TcpSocket::Send(ByteStream &Stream, ULONG *Sent, LONG Flags)
{
	if (Sent) *Sent=0;
	if (!GetIsOpen()) {
		ULONG Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("TcpSocket"), _T("Send"), _T("Socket is not open."), Stat);
		return Stat;
	}
	if (NULL == Stream.GetBuffer()) {
		ULONG Stat=(ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(_T("TcpSocket"), _T("Send"), _T("Buffer is null."), Stat);
		return Stat;
	}

	int NSent=0, NBufBytes=0;
	int BufSize = Stream.GetSize();
	const char * const Buf = Stream.GetBuffer();
	do {
		if (_Interrupt) {
			ULONG Stat=(ULONG) E_ABORT;
			DEBUG_ERROR_MESSAGE(_T("TcpSocket"), _T("Send"), _T("Operation aborted because Interrupt signaled"), Stat);
			return Stat;
		}
		LONG Stat=send(_Socket, Buf+NBufBytes, BufSize-NBufBytes, Flags);
		if (SOCKET_ERROR == Stat) {
			ULONG Stat=0;
#ifdef WIN32
			Stat = WSAGetLastError();
#else
			if ((EWOULDBLOCK == errno) || (EAGAIN == errno)) {
				Stat = WSAEWOULDBLOCK;
			}
			else {
				DEBUG_ERROR_MESSAGE(_T("TcpSocket"), _T("Send"), _T("Error send."), errno);
				return errno;
			}
#endif
			if (WSAEWOULDBLOCK == Stat) {
				//
				//The GetIsSending() call has a 250 ms timeout so there is no need to
				//use a sleep in the while loop.  GetIsSending uses the select() API
				//to check the status of the socket.  Note, the GetIsSending API was added
				//to resolve an issue with RNDIS.  It appears that there may be a driver
				//bug in RNDIS where if you invoke a send/recv before the socket is ready
				//the driver locks up and all communications including those used by
				//the 'ping' utility fail
				//
				while(GetIsSending()) {
					if (_Interrupt) {
						ULONG Stat=(ULONG) E_ABORT;
						DEBUG_ERROR_MESSAGE(_T("TcpSocket"), _T("Send"), _T("Operation aborted because Interrupt signaled"), Stat);
						return Stat;
					}
				}
				continue;
			}
			DEBUG_ERROR_MESSAGE(_T("TcpSocket"), _T("Send"), _T("Error send."), Stat);
			return Stat;
		}
		else if (0 == Stat) break;


		NSent += Stat;
		NBufBytes += Stat;

	} while(BufSize != NBufBytes);

	if (Sent) *Sent=NSent;

	return 0;
}

///<summary>
///This method will issue a blocking/nonblocking recv.   The method will not
///return until all data has been recv'd.  You can interrupt the receiving
///by invoking Interrupt().  However, you must invoke ResetInterrupt()
///make subsequent calls to this routine after invoking Interrupt()
///</summary>
///<param name="Stream">The stream recv'd</param>
///<param name="NumReceived">The number of bytes recv'd</param>
///<param name="Flags">The flags</param>
///<returns>Win32 code.  0==Sucess</returns>
ULONG Canberra::Utility::Network::TcpSocket::Receive(ByteStream &Stream, ULONG *Recv, LONG Flags)
{
	if (Recv) *Recv=0;
	if (!GetIsOpen()) {
		ULONG Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("TcpSocket"), _T("Receive"), _T("Socket is not open."), Stat);
		return Stat;
	}
	if (NULL == Stream.GetBuffer()) {
		ULONG Stat=(ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(_T("TcpSocket"), _T("Receive"), _T("Buffer is null."), Stat);
		return Stat;
	}

	int NBytesRecv=0, NBufBytes=0;
	int BufSize = Stream.GetSize();
	const char * const Buf = Stream.GetBuffer();
	while(NBufBytes < BufSize) {
		if (_Interrupt) {
			ULONG Stat=(ULONG) E_ABORT;
			DEBUG_ERROR_MESSAGE(_T("TcpSocket"), _T("Receive"), _T("Operation aborted because Interrupt signaled"), Stat);
			return Stat;
		}
		LONG Stat=recv(_Socket, const_cast<char *>(Buf+NBufBytes), BufSize-NBufBytes, Flags);
		if (SOCKET_ERROR == Stat) {
			ULONG Stat=0;
#ifdef WIN32
			Stat = WSAGetLastError();
#else
			if ((EWOULDBLOCK == errno) || (EAGAIN == errno)) {
				Stat = WSAEWOULDBLOCK;
			}
			else {
				DEBUG_ERROR_MESSAGE(_T("TcpSocket"), _T("Receive"), _T("Error recv."), errno);
				return errno;
			}
#endif
			if (WSAEWOULDBLOCK == Stat) {
				//
				//The GetIsReceiving() call has a 250 ms timeout so there is no need to
				//use a sleep in the while loop.  GetIsReceiving uses the select() API
				//to check the status of the socket.  Note, the GetIsReceiving API was added
				//to resolve an issue with RNDIS.  It appears that there may be a driver
				//bug in RNDIS where if you invoke a send/recv before the socket is ready
				//the driver locks up and all communications including those used by
				//the 'ping' utility fail
				//
				while(GetIsReceiving()) {
					if (_Interrupt) {
						ULONG Stat=(ULONG) E_ABORT;
						DEBUG_ERROR_MESSAGE(_T("TcpSocket"), _T("Receive"), _T("Operation aborted because Interrupt signaled"), Stat);
						return Stat;
					}
				}
				continue;
			}
			DEBUG_ERROR_MESSAGE(_T("TcpSocket"), _T("Receive"), _T("Error recv."), Stat);
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
///This method will signal the interrupt event
///</summary>
///<returns>0==Success. Otherwise error code</returns>
DWORD Canberra::Utility::Network::TcpSocket::Interrupt() { return _Interrupt = true;}

///<summary>
///This method will reset the interrupt event
///</summary>
///<returns>0==Success. Otherwise error code</returns>
DWORD Canberra::Utility::Network::TcpSocket::ResetInterrupt() { return _Interrupt = false;}

///<summary>
///This method will return the state of the interrupt event
///</summary>
///<returns>true==interrupt is signaled</returns>
bool Canberra::Utility::Network::TcpSocket::GetIsInterrupted() {return _Interrupt;}
