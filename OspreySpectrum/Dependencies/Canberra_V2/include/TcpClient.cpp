#include "TcpClient.h"
#include "DebugMacros.h"
#ifndef WIN32
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#endif
using namespace Canberra::Utility::Core;

///<summary>
///An instance of this class is used to represent
///a TCP client.  A TCP client is a TCP socket that
///requires a server to connect to in order to make
///the connection complete
///</summary>
///<param name="Name">The name to associated with this socket</param>
Canberra::Utility::Network::TcpClient::TcpClient(const String &Name) : TcpSocket(Name) {}

Canberra::Utility::Network::TcpClient::~TcpClient(void)
{
}

///<summary>
///This method will connect to the server
///</summary>
///<param name="ServerEP">The server address</param>
///<param name="Async">Whether async connect is performed</param>
///<returns>Win32 code.  0==Sucess</returns>
ULONG Canberra::Utility::Network::TcpClient::Connect(EndPoint ServerEP, bool Async)
{
	_ServerEP.Initialize();
	if (!GetIsOpen()) {
		ULONG Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("TcpClient"), _T("Connect"), _T("Socket is not open."), Stat);
		return Stat;
	}

	SOCKADDR_IN In = ServerEP.GetAddressInfo();
	int Size=sizeof(In);

	if (!Async) {
#ifdef WIN32
		EndPoint EP(In.sin_addr.S_un.S_addr, ntohs(In.sin_port));
#else
		EndPoint EP(In.sin_addr.s_addr, ntohs(In.sin_port));
#endif
		if (SOCKET_ERROR == connect(_Socket, (struct sockaddr *) &In, Size)) {
			ULONG Stat = WSAGetLastError();
			DEBUG_ERROR_MESSAGE(_T("TcpClient"), _T("Connect"), _T("Error connect."), Stat);
			return Stat;
		}
		_ServerEP = ServerEP;

		//Try to determine the local address (incase INADDR_ANY was used)
		if (INADDR_ANY == _EndPoint.GetAddress()) {
			SOCKADDR_IN sin = {0};
#ifdef WIN32
			INT Size = sizeof(sin);
#else
			socklen_t Size = (socklen_t) sizeof(sin);
#endif
			if (SOCKET_ERROR != getsockname(_Socket, (struct sockaddr *) &sin, &Size)) {
#ifdef WIN32
				_EndPoint=EndPoint(sin.sin_addr.S_un.S_addr, ntohs(sin.sin_port));
#else
				_EndPoint=EndPoint(sin.sin_addr.s_addr, ntohs(sin.sin_port));
#endif
			}
		}

		return 0;
	}

	return E_NOTIMPL;

}

///<summary>
///This method returns the address information of
///the TCP server
///</summary>
///<returns>The server info</returns>
Canberra::Utility::Network::EndPoint Canberra::Utility::Network::TcpClient::GetServerAddress() { return _ServerEP;}

///<summary>
///This method sets the address information of
///the TCP server
///</summary>
///<param name="v">The server info</param>
void Canberra::Utility::Network::TcpClient::PutServerAddress(Canberra::Utility::Network::EndPoint v) { _ServerEP=v;}
