#pragma once

namespace Canberra {
	namespace Protocols {
		namespace Lynx {

			///<summary>
			///This is a container for all of the socket settings used
			///by the stream or configuration channels.
			///</summary>
			class ChannelSettings {
			public:
				ChannelSettings(USHORT P) :
					SendTimeOut(60000),
					RecvTimeOut(60000),
					SendBufferSize(32768),
					RecvBufferSize(32768),
					KeepAlive(TRUE),
					NoDelay(TRUE),
					Port(P) {}
				LONG SendTimeOut;											//Send timout (ms)
				LONG RecvTimeOut;											//Receive timout (ms)
				int SendBufferSize;											//Send buffer size
				int RecvBufferSize;											//Receive buffer size
				BOOL KeepAlive;												//Keep alive state
				BOOL NoDelay;												//Nagles alg state
				USHORT Port;												//The socket port
			};
		}
	}
}
