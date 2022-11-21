#pragma once
#include "CommunicationsImpExp.h"
#include "MessageFactory.h"
#include "TcpClient.h"
#include "SpectralData.h"
#include "Variant.h"
#include "ChannelSettings.h"
#include "CriticalSection.h"

namespace Canberra {
	namespace Protocols {
		namespace Lynx {

			///<summary>
			///An instance of this class contains Lynx version 1.1
			///modifications.  This instance specifically contains
			///all information necessary to implement data streaming
			///</summary>
			class StreamChannel {
			protected:
				StreamChannel(const StreamChannel&);
				StreamChannel& operator=(const StreamChannel&);

				Canberra::DataTypes::Serialization::MessageFactory &_MessageFactory;//The message factory
				Canberra::Utility::Network::TcpClient _Channel;				//The stream channel
				Canberra::Utility::Core::CriticalSection _Lock;				//A lock for thread safety
				Canberra::Utility::Network::EndPoint _ClientEP;				//The client endpoint info
				Canberra::Utility::Network::EndPoint _DevEP;				//The device endpoint info
				bool _Abort;												//The abort state
				bool _Enable;												//Whether to use streaming
				ChannelSettings _Settings;									//The settings for the channel

				//Custom TCP/IP receive call
				ULONG Receive(Canberra::Utility::Core::ByteStream &Stream, ULONG *Recv=NULL, LONG Flags=0);

			public:
				StreamChannel(Canberra::DataTypes::Serialization::MessageFactory &MF);
				virtual ~StreamChannel();

				//Returns the streamed command
				ULONG ReceiveCommand(Canberra::DataTypes::Application::CommandData **Resp);

				//Establishes a connection between the client and the device
				ULONG Open(const Canberra::Utility::Core::String &Client, const Canberra::Utility::Core::String &Dest);

				//Closes the connection
				ULONG Close();

				//The enable state
				bool GetEnable() const;
				void PutEnable(const bool v);

				//The active state
				bool GetActive();

				//Accessor to properties
				ULONG GetProperty(const Canberra::Utility::Core::String &Name, Canberra::Utility::Core::Variant &Val);
				ULONG PutProperty(const Canberra::Utility::Core::String &Name, const Canberra::Utility::Core::Variant &Val);
			};
		}
	}
}
