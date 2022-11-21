#include "Socket.h"
#include "DebugMacros.h"
#include <memory.h>
#ifndef WIN32
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#endif

///<summary>
///This method will initialize the endpoint information
///to the loopback address, port zero (autoselect),
///AF_INET family
///</summary>
void Canberra::Utility::Network::EndPoint::Initialize() {
	_Address = inet_addr("127.0.0.1");
	_Port= 0;
	_Family = AF_INET;
}

///<summary>
///An instance of this class represents the endpoint information
///for a socket.  This method will initialize the endpoint information
///to the loopback address, port zero (autoselect),
///AF_INET family.
///</summary>
Canberra::Utility::Network::EndPoint::EndPoint() : _Family(AF_INET), _Port(0) {Initialize();}


///<summary>
///An instance of this class represents the endpoint information
///for a socket.
///</summary>
///<param name="Address">The network address</param>
///<param name="Port">The network port non-network format</param>
///<param name="Family">The network family</param>
Canberra::Utility::Network::EndPoint::EndPoint(LONG Address, USHORT Port, SHORT Family) :
_Family(Family), _Port(Port), _Address(Address) {}

///<summary>
///An instance of this class represents the endpoint information
///for a socket.
///</summary>
///<param name="Address">The network address</param>
///<param name="Port">The network port non-network format</param>
///<param name="Family">The network family</param>
Canberra::Utility::Network::EndPoint::EndPoint(const Canberra::Utility::Core::String &Address, USHORT Port, SHORT Family) :
 _Family(Family), _Port(Port) {PutAddressString(const_cast<Canberra::Utility::Core::String &>(Address));}
Canberra::Utility::Network::EndPoint::~EndPoint() {}


///<summary>
///This method compares
///</summary>
///<param name="Rhs">The right hand side</param>
///<returns>Equals state</returns>
bool Canberra::Utility::Network::EndPoint::operator==(const Canberra::Utility::Network::EndPoint& Rhs) const {
	return ((Rhs._Port == _Port) && (Rhs._Address == _Address));
}


///<summary>
///This method compares
///</summary>
///<param name="Rhs">The right hand side</param>
///<returns>Not-Equals state</returns>
bool Canberra::Utility::Network::EndPoint::operator!=(const Canberra::Utility::Network::EndPoint& Rhs) const {
	return !((Rhs._Port == _Port) && (Rhs._Address == _Address));
}

///<summary>
///This method will return the endpoint address information
///</summary>
///<returns>The socket address info</returns>
SOCKADDR_IN Canberra::Utility::Network::EndPoint::GetAddressInfo()
{
	SOCKADDR_IN sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = _Family;
	sin.sin_port = htons(_Port);
#ifdef WIN32
	sin.sin_addr.S_un.S_addr = _Address;
#else
	sin.sin_addr.s_addr = _Address;
#endif
	return sin;
}

///<summary>
///This method will set the endpoint address information
///</summary>
///<param name="Info">The socket info</param>
void Canberra::Utility::Network::EndPoint::PutAddressInfo(SOCKADDR_IN &Info)
{
	_Family = Info.sin_family;
	_Port = ntohs(Info.sin_port);
#ifdef WIN32
	_Address = Info.sin_addr.S_un.S_addr;
#else
	_Address = Info.sin_addr.s_addr;
#endif
	return;
}

///<summary>
///This method will get the network family
///</summary>
///<returns>The value</returns>
SHORT Canberra::Utility::Network::EndPoint::GetFamily() {return _Family;}

///<summary>
///This method will set the network family
///</summary>
///<param name="v">The value</param>
void Canberra::Utility::Network::EndPoint::PutFamily(SHORT v) {_Family = v;}

///<summary>
///This method will get the port
///</summary>
///<returns>The value</returns>
USHORT Canberra::Utility::Network::EndPoint::GetPort() {return _Port;}

///<summary>
///This method will set the port
///</summary>
///<param name="v">The value</param>
void Canberra::Utility::Network::EndPoint::PutPort(USHORT v) {_Port = v;}

///<summary>
///This method will get the address
///</summary>
///<returns>The value</returns>
ULONG Canberra::Utility::Network::EndPoint::GetAddress() {return _Address;}

///<summary>
///This method will set the address
///</summary>
///<param name="v">The value</param>
void Canberra::Utility::Network::EndPoint::PutAddress(ULONG v) {_Address = v;}

///<summary>
///This method will get the address
///</summary>
///<returns>The value</returns>
Canberra::Utility::Core::String Canberra::Utility::Network::EndPoint::GetAddressString() {
	SOCKADDR_IN sin;
	memset(&sin, 0, sizeof(sin));
#ifdef WIN32
	sin.sin_addr.S_un.S_addr = _Address;
#else
	sin.sin_addr.s_addr = _Address;
#endif
	return Canberra::Utility::Core::String(inet_ntoa(sin.sin_addr));
}

///<summary>
///This method will get the address
///</summary>
///<param name="v">The value</param>
void Canberra::Utility::Network::EndPoint::PutAddressString(Canberra::Utility::Core::String v) {
	if (!v.GetLength()) {_Address = INADDR_ANY;}
	else {_Address = inet_addr(v);}
}

///<summary>
///This is the base class for all sockets.  It cannot be
///instantiated; it must be derived.  It will initialize
///the socket library during construction via WSAStartup call
///</summary>
///<param name="Name">The name to associated with this instance</param>
///<exception cref"ULONG">Error info</exception>
Canberra::Utility::Network::Socket::Socket(const Canberra::Utility::Core::String &Name) :
	_Name(Name), _Socket(INVALID_SOCKET),
	_IsBlocking(true), _Type(SOCK_STREAM), _Protocol(IPPROTO_TCP),
	_IsBound(false) {
#ifdef WIN32
	WSADATA WSAdata;
	WORD Version = MAKEWORD( 2, 2 );
	LONG Stat = WSAStartup(Version, &WSAdata);
	if (Stat) {
		DEBUG_ERROR_MESSAGE(_T("Socket"), _T("Socket"), _T("Error WSAStartup."), Stat);
		throw Stat;
	}
#endif
	_Init=true;
}

///<summary>
///This will shutdown, close, and cleanup the socket.
///it calls WSACleanup
///</summary>
Canberra::Utility::Network::Socket::~Socket(void) {
	if (GetIsOpen()) {
		Shutdown(SD_BOTH);
		Close();
	}
#ifdef WIN32
	if (_Init) {WSACleanup(); _Init=false;}
#else
	_Init=false;
#endif
}


///<summary>
///This method will get open state of the socket handle
///</summary>
///<returns>The value</returns>
bool Canberra::Utility::Network::Socket::GetIsOpen(void) { return INVALID_SOCKET !=_Socket;}

///<summary>
///This method will get the name associated with this instance
///</summary>
///<returns>The value</returns>
Canberra::Utility::Core::String Canberra::Utility::Network::Socket::GetName() { return _Name;}

///<summary>
///This method will get the socket address info
///</summary>
///<returns>The value</returns>
Canberra::Utility::Network::EndPoint &Canberra::Utility::Network::Socket::GetAddress() {
	return _EndPoint;
}

///<summary>
///This method will set the protocol
///</summary>
///<param name="v">The value</param>
void Canberra::Utility::Network::Socket::PutAddress(EndPoint &EP) { _EndPoint=EP;}

///<summary>
///This method will open the socket
///</summary>
///<param name="EP">The endpoint info</param>
///<returns>Win32 code.  0==Sucess</returns>
ULONG Canberra::Utility::Network::Socket::Open(Canberra::Utility::Network::EndPoint EP)
{
	if (GetIsOpen()) {
		ULONG Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("Socket"), _T("Open"), _T("Socket is already open."), Stat);
		return Stat;
	}

	//Create the socket
	_Socket = socket(EP.GetFamily(), _Type, _Protocol);
	if (INVALID_SOCKET == _Socket) {
		ULONG Stat = WSAGetLastError();
		DEBUG_ERROR_MESSAGE(_T("Socket"), _T("Open"), _T("Error socket."), Stat);
		return Stat;
	}

	_IsBound=false;
	SOCKADDR_IN sin = EP.GetAddressInfo();
	if (SOCKET_ERROR == bind(_Socket, (const sockaddr *) &sin, sizeof(sin))) {
		ULONG Stat = WSAGetLastError();
		DEBUG_ERROR_MESSAGE(_T("Socket"), _T("Open"), _T("Error bind."), Stat);
		_EndPoint = EP;
		return Stat;
	}
	_IsBound=true;
	_EndPoint = EP;

	return 0;
}

///<summary>
///This method will close the socket
///</summary>
///<returns>Win32 code.  0==Sucess</returns>
ULONG Canberra::Utility::Network::Socket::Close()
{
	if (!GetIsOpen()) {
		ULONG Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("Socket"), _T("Close"), _T("Socket not open."), Stat);
		return Stat;
	}

	if (SOCKET_ERROR == closesocket(_Socket)) {
		ULONG Stat = WSAGetLastError();
		DEBUG_ERROR_MESSAGE(_T("Socket"), _T("Close"), _T("Error closesocket."), Stat);
		return Stat;
	}
	_IsBound=false;
	_IsBlocking=false;
	_Socket=INVALID_SOCKET;
	return 0;
}



///<summary>
///This method will shutdown the socket
///</summary>
///<param name="Level">The level of shutdown</param>
///<returns>Win32 code.  0==Sucess</returns>
ULONG Canberra::Utility::Network::Socket::Shutdown(LONG Level)
{
	if (!GetIsOpen()) {
		ULONG Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("Socket"), _T("Shutdown"), _T("Socket not open."), Stat);
		return Stat;
	}
	_IsBound=false;
	if (SOCKET_ERROR == shutdown(_Socket, Level)) {
		ULONG Stat = WSAGetLastError();
		DEBUG_ERROR_MESSAGE(_T("Socket"), _T("Shutdown"), _T("Error shutdown."), Stat);
		return Stat;
	}
	return 0;
}


///<summary>
///This method will set the socket option
///</summary>
///<param name="Level">The level</param>
///<param name="Name">The option name</param>
///<param name="Value">The option value</param>
///<param name="ValLength">The option value length (bytes)</param>
///<returns>Win32 code.  0==Sucess</returns>
ULONG Canberra::Utility::Network::Socket::SetOption(LONG Level, LONG Name, const char *Value, LONG ValLength)
{
	if (!GetIsOpen()) {
		ULONG Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("Socket"), _T("SetOption"), _T("Socket not open."), Stat);
		return Stat;
	}

	if (SOCKET_ERROR == setsockopt(_Socket, Level, Name, Value, ValLength)) {
		ULONG Stat = WSAGetLastError();
		DEBUG_ERROR_MESSAGE(_T("Socket"), _T("SetOption"), _T("Error setsockopt."), Stat);
		return Stat;
	}
	return 0;
}

///<summary>
///This method will get the socket option
///</summary>
///<param name="Level">The level</param>
///<param name="Name">The option name</param>
///<param name="Value">The option value</param>
///<param name="ValLength">The option value length (bytes)</param>
///<returns>Win32 code.  0==Sucess</returns>
ULONG Canberra::Utility::Network::Socket::GetOption(LONG Level, LONG Name, char *Value, LONG *ValLength)
{
	if (!GetIsOpen()) {
		ULONG Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("Socket"), _T("GetOption"), _T("Socket not open."), Stat);
		return Stat;
	}
#ifdef WIN32
	int *len = reinterpret_cast<int *>(ValLength);
#else
	socklen_t *len = reinterpret_cast<socklen_t *>(ValLength);
#endif
	if (SOCKET_ERROR == getsockopt(_Socket, Level, Name, Value, len)) {
		ULONG Stat = WSAGetLastError();
		DEBUG_ERROR_MESSAGE(_T("Socket"), _T("GetOption"), _T("Error getsockopt."), Stat);
		return Stat;
	}
	return 0;
}

///<summary>
///This method will set the IO control command for the socket
///</summary>
///<param name="Cmd">The command</param>
///<param name="Args">The command args</param>
///<returns>Win32 code.  0==Sucess</returns>
ULONG Canberra::Utility::Network::Socket::IOControl(LONG Cmd, ULONG *Args)
{
	if (!GetIsOpen()) {
		ULONG Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("Socket"), _T("IOControl"), _T("Socket not open."), Stat);
		return Stat;
	}
#ifdef WIN32
	if (SOCKET_ERROR == ioctlsocket(_Socket, Cmd, Args)) {
		ULONG Stat = WSAGetLastError();
		DEBUG_ERROR_MESSAGE(_T("Socket"), _T("IOControl"), _T("Error ioctlsocket."), Stat);
		return Stat;
	}
#else
	if (SOCKET_ERROR == fcntl(_Socket, Cmd, *Args)) {
		ULONG Stat = WSAGetLastError();
		DEBUG_ERROR_MESSAGE(_T("Socket"), _T("IOControl"), _T("Error fcntl."), Stat);
		return Stat;
	}
#endif
	return 0;
}

///<summary>
///This method will return the socket handle
///</summary>
///<returns>The socket handle</returns>
SOCKET Canberra::Utility::Network::Socket::GetHandle() {return _Socket;}

///<summary>
///This method will return the socket handle
///</summary>
///<returns>The socket handle</returns>
Canberra::Utility::Network::Socket::operator SOCKET() {return GetHandle();}

///<summary>
///This method will return information indicating
///whether the socket is currently sending data
///</summary>
///<returns>Send state</returns>
bool Canberra::Utility::Network::Socket::GetIsSending() {

	if (!GetIsOpen()) {
		ULONG Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("Socket"), _T("GetIsSending"), _T("Socket not open."), Stat);
		return false;
	}

	fd_set wlist;
#ifdef WIN32
	struct timeval to = {0, 250};
	wlist.fd_count = 1;
	wlist.fd_array[0] = _Socket;
	LONG Stat = select(0, NULL, &wlist, NULL, &to);
	if ((SOCKET_ERROR == Stat) || (1 != Stat)) {
		if (SOCKET_ERROR == Stat) {
			ULONG Stat = WSAGetLastError();
			DEBUG_ERROR_MESSAGE(_T("Socket"), _T("GetIsSending"), _T("Error with select."), Stat);
		}
		return false;
	}
	return true;
#else

	struct timeval to;
	to.tv_sec = 0;
	to.tv_usec = 250;
	FD_ZERO(&wlist);
	FD_SET(_Socket, &wlist);
	LONG Stat = select(_Socket+1, NULL, &wlist, NULL, &to);
	if (SOCKET_ERROR != Stat) {
		if (FD_ISSET(_Socket, &wlist)) return true;
		return false;
	}
	return true;
#endif
}


///<summary>
///This method will return information indicating
///whether the socket is currently recv'ing data
///</summary>
///<returns>Recv state</returns>
bool Canberra::Utility::Network::Socket::GetIsReceiving() {

	if (!GetIsOpen()) {
		ULONG Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("Socket"), _T("GetIsReading"), _T("Socket not open."), Stat);
		return false;
	}

	fd_set rlist;
#ifdef WIN32
	struct timeval to = {0, 250};
	rlist.fd_count = 1;
	rlist.fd_array[0] = _Socket;
	LONG Stat = select(0, &rlist, NULL, NULL, &to);
	if ((SOCKET_ERROR == Stat) || (1 != Stat)) {
		if (SOCKET_ERROR == Stat) {
			ULONG Stat = WSAGetLastError();
			DEBUG_ERROR_MESSAGE(_T("Socket"), _T("GetIsReading"), _T("Error with select."), Stat);
		}
		return false;
	}
#else
	struct timeval to;

	FD_ZERO(&rlist);
	FD_SET(_Socket, &rlist);
	to.tv_sec = 0;
	to.tv_usec = 250;
	LONG Stat = select(_Socket+1, &rlist, NULL, NULL, &to);
	if (SOCKET_ERROR != Stat) {
		if (FD_ISSET(_Socket, &rlist)) return true;
		return false;
	}
	return true;
#endif
	return true;
}

///<summary>
///This method will return the blocking state of the socket
///</summary>
///<returns>Send state</returns>
bool Canberra::Utility::Network::Socket::GetIsBlocking() {return _IsBlocking;}

///<summary>
///This method will set the blocking state of the socket
///</summary>
///<returns>Send state</returns>
void Canberra::Utility::Network::Socket::PutIsBlocking(bool v) {

#ifdef WIN32
	ULONG Mode = v ? 0 : 1;
	ULONG Stat = IOControl(FIONBIO, &Mode);
#else
	int flags=fcntl(_Socket, F_GETFL, 0);
	if (!v) flags |= O_NONBLOCK;
	else flags = flags&~O_NONBLOCK;
	ULONG Stat = fcntl(_Socket, F_SETFL, flags);
#endif
	if (!Stat) _IsBlocking=v;
}


///<summary>
///This method will return the no delay state.
///Nagle algorithm enabled state
///</summary>
///<returns>Send state</returns>
bool Canberra::Utility::Network::Socket::GetNodelay() {
	BOOL Value=0;
	LONG ValLength=sizeof(Value);
	GetOption(IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&Value), &ValLength);
	return (bool) (Value ? true : false);
}

///<summary>
///This method will set the no delay state.
///Nagle algorithm enabled state
///</summary>
///<returns>Send state</returns>
void Canberra::Utility::Network::Socket::PutNodelay(bool v) {
	BOOL Value=v;
	LONG ValLength=sizeof(Value);
	SetOption(IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&Value), ValLength);
}


///<summary>
///This method will return the keep alive state.
///</summary>
///<returns>Send state</returns>
bool Canberra::Utility::Network::Socket::GetKeepAlive() {
	BOOL Value=0;
	LONG ValLength=sizeof(Value);
	GetOption(SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char *>(&Value), &ValLength);
	return (bool) (Value ? true : false);
}

///<summary>
///This method will set the keep alive state.
///</summary>
///<returns>Send state</returns>
void Canberra::Utility::Network::Socket::PutKeepAlive(bool v) {
	BOOL Value=v;
	LONG ValLength=sizeof(Value);
	SetOption(SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char *>(&Value), ValLength);
}


///<summary>
///This method will return the receive buffer size
///</summary>
///<returns>Send state</returns>
LONG Canberra::Utility::Network::Socket::GetReceiveBufferSize() {
	INT Value=0;
	LONG ValLength=sizeof(Value);
	GetOption(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&Value), &ValLength);
	return Value;
}

///<summary>
///This method will set the receive buffer size
///</summary>
///<returns>Send state</returns>
void Canberra::Utility::Network::Socket::PutReceiveBufferSize(LONG v) {
	INT Value=(INT)v;
	LONG ValLength=sizeof(Value);
	SetOption(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&Value), ValLength);
}


///<summary>
///This method will return the Send buffer size
///</summary>
///<returns>Send state</returns>
LONG Canberra::Utility::Network::Socket::GetSendBufferSize() {
	INT Value=0;
	LONG ValLength=sizeof(Value);
	GetOption(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char *>(&Value), &ValLength);
	return Value;
}

///<summary>
///This method will set the Send buffer size
///</summary>
///<returns>Send state</returns>
void Canberra::Utility::Network::Socket::PutSendBufferSize(LONG v) {
	INT Value=(INT)v;
	LONG ValLength=sizeof(Value);
	SetOption(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char *>(&Value), ValLength);
}

///<summary>
///This method will return the bind state
///</summary>
///<returns>the state</returns>
bool Canberra::Utility::Network::Socket::GetIsBound() {return _IsBound;}

