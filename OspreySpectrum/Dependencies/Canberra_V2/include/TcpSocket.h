#pragma once
#include "NetworkImpExp.h"
#include "Socket.h"


namespace Canberra {
	namespace Utility {
		namespace Network {

			//
			//This is a base used to access a stream socket
			//
			class NETWORK_DLLIMPORT TcpSocket : public Socket
			{
			protected:
					friend class TcpServer;
					bool _Interrupt;

					void Attach(SOCKET);
					TcpSocket(const Canberra::Utility::Core::String &Name);
					virtual ~TcpSocket(void);

			public:
					//Traditional block/nonblocking send
					ULONG Send(Canberra::Utility::Core::ByteStream &Stream, ULONG *Sent=NULL, LONG Flags=0);

					//Traditional block/nonblocking recv
					ULONG Receive(Canberra::Utility::Core::ByteStream &Stream, ULONG *Recv=NULL, LONG Flags=0);

					//Sets the interrupt event
					ULONG Interrupt();

					//Resets the interrupt event
					ULONG ResetInterrupt();

					//This method will get the state of the I/O interrutp
					bool GetIsInterrupted();
			};
		}
	}
}
