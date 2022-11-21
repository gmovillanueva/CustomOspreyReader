#pragma once
#include "DataTypesImpExp.h"
#include "Serializable.h"

//Data description for message header
#pragma pack(push, 1)
struct MessageHeaderS {
	ULONG Size;				//Size of the entire message
	ULONG Version;			//Version of this message
	ULONG SequenceNumber;	//The sequence number for this message
	ULONG CheckSum;			//Check sum
};
#pragma pack(pop)

namespace Canberra {
	namespace DataTypes {
		namespace Serialization {

			//
			//An instance of this class is used to serialize/deserialize
			//a message header to/from a stream
			//
			class DATATYPES_DLLIMPORT MessageHeader : public Canberra::Serialization::Serializable
			{
			protected:
				struct MessageHeaderS _Data;

			public:
				MessageHeader(ULONG PayloadSize=0, ULONG Version=1, ULONG Seq=0);
				virtual ~MessageHeader(void);

				//Serializes the classes information into this stream
				ULONG Serialize(Canberra::Utility::Core::ByteStream &, const LONG Offset=0);

				//Deserializes the information in the stream into the class instance
				ULONG Deserialize(Canberra::Utility::Core::ByteStream &,const  LONG Offset=0);

				//The size of the message (header+payload)
				ULONG GetSize();

				//The size of the header only
				ULONG GetHeaderSize();

				//The version of the message
				ULONG GetVersion();

				//The sequence number
				ULONG GetSequenceNumber();

				//The checksum
				ULONG GetCheckSum();

			};
		}
	}
}
