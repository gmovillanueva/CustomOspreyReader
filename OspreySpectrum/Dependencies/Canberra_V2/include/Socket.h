#pragma once
#include "NetworkImpExp.h"
#include "UtilityString.h"
#include "ByteStream.h"
#ifdef WIN32
//#include <winsock.h>
#include <Winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#define SOCKADDR_IN sockaddr_in
#define SD_BOTH SHUT_RDWR
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(a) close(a)
#define WSAGetLastError() errno
#endif
namespace Canberra {
	namespace Utility {
		namespace Network {

			//
			//An instance of this class is used to define an
			//endpoint of a socket
			//
			class NETWORK_DLLIMPORT EndPoint {
				protected:
					SHORT _Family;				//Socket family
					USHORT _Port;				//Socket port (0==autoselect)
					ULONG _Address;				//IP address

				public:
					EndPoint();
					EndPoint(LONG Address, USHORT Port, SHORT Family=AF_INET);
					EndPoint(const Canberra::Utility::Core::String &Address, USHORT Port, SHORT Family=AF_INET);
					virtual ~EndPoint();

					//Sets the instance to default values
					void Initialize();

					//Returns the socket address info to be used with socket APIs
					SOCKADDR_IN GetAddressInfo();
					void PutAddressInfo(SOCKADDR_IN &v);

					//Accessors to family
					void PutFamily(SHORT v);
					SHORT GetFamily();

					//Accessors to port
					void PutPort(USHORT v);
					USHORT GetPort();

					//Accessors to IP address
					void PutAddress(ULONG v);
					ULONG GetAddress();

					//String representation of IP address
					void PutAddressString(Canberra::Utility::Core::String v);
					Canberra::Utility::Core::String GetAddressString();

					//Comparison operator
					bool operator==(const EndPoint& Rhs) const;

					//Comparison operator
					bool operator!=(const EndPoint& Rhs) const;
			};

			//
			//This is a base used to access a socket
			//
			class NETWORK_DLLIMPORT Socket
			{
				Socket(const Socket&);
				Socket& operator=(const Socket&);

			protected:
				Canberra::Utility::Core::String _Name;			//Name associated with the socket
				SOCKET _Socket;									//The socket handle
				bool _Init;										//The init state of WSA
				bool _IsBlocking;								//The blocking state of the socket
				LONG _Type;										//The type
				LONG _Protocol;									//The protocol
				EndPoint _EndPoint;								//The current endpoint int use
				bool _IsBound;									//The bind state

				Socket(const Canberra::Utility::Core::String &Name);
				virtual ~Socket(void);

			public:
				//Opens the socket and binds to the specified end point
				ULONG Open(EndPoint);

				//Closes the socket
				ULONG Close();

				//Issues an IO control command
				ULONG IOControl(LONG Cmd, ULONG *Args=NULL);

				//Sets the socket option
				ULONG SetOption(LONG Level, LONG Name, const char *Value, LONG ValLength);

				//Gets the socket option
				ULONG GetOption(LONG Level, LONG Name, char *Value, LONG *ValLength);

				//Shutdown the socket
				ULONG Shutdown(LONG Options=SD_BOTH);

				//Returns the endpoint in use
				EndPoint &GetAddress();
				void PutAddress(EndPoint &EP);

				//Returns the open state
				bool GetIsOpen();

				//Returns the name of the file
				Canberra::Utility::Core::String GetName();

				//Returns the name of the file
				SOCKET GetHandle();

				//Returns the unmanaged socket handle
				operator SOCKET();

				//State to indicate sending data
				bool GetIsSending();

				//State to indicate recving data
				bool GetIsReceiving();

				//State to indicate blocking mode
				bool GetIsBlocking();
				void PutIsBlocking(bool v);

				//The no delay state
				bool GetNodelay();
				void PutNodelay(bool);

				//The keep alive state
				bool GetKeepAlive();
				void PutKeepAlive(bool);

				//The receive buffer size
				LONG GetReceiveBufferSize();
				void PutReceiveBufferSize(LONG);

				//The send buffer size
				LONG GetSendBufferSize();
				void PutSendBufferSize(LONG);

				//State to indicate bind state
				bool GetIsBound();

			};
		}
	}
}
