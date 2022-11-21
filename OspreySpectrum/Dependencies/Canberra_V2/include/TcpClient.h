#pragma once
#include "TcpSocket.h"

namespace Canberra {
	namespace Utility {
		namespace Network {

			//
			//This is a class used to define a
			//TCP client.
			//
			class NETWORK_DLLIMPORT TcpClient : public TcpSocket
			{
				TcpClient(const TcpClient&);
				TcpClient& operator=(const TcpClient&);

			protected:
				EndPoint _ServerEP;								//The network address of the server

			public:
				TcpClient(const Canberra::Utility::Core::String &Name);
				virtual ~TcpClient(void);

				//Connect to a specific end point
				ULONG Connect(EndPoint Server, bool Async=false);

				//The address information of the server for this client
				EndPoint GetServerAddress();
				void PutServerAddress(EndPoint v);
			};
		}
	}
}
