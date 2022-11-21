#pragma once
#include "DataTypesImpExp.h"
#include "SerializableObject.h"
#include "DigitalSignature.h"

namespace Canberra {
	namespace DataTypes {
		namespace Serialization {

			//
			//This is a utility class used to construct
			//messages for transmitting to other machines
			//or processes.  It will also deserialize a message
			//into a specific class instance.
			//
			class DATATYPES_DLLIMPORT MessageFactory
			{
				MessageFactory(const MessageFactory&);
				MessageFactory& operator=(const MessageFactory&);

			protected:
				DigitalSignature _Signature;				//The digital signature
				static LONG MSG_VERSION_1;					//Supported message version

			public:
				MessageFactory(void);
				virtual ~MessageFactory(void);

				///<summary>
				///Accessor to enable signing
				///<exceptions cref="ULONG">Win32 error code </exceptions>
				///</summary>
				bool GetDigitallySign();
				void PutDigitallySign(bool v);

				//Serialize the data into a message that is serialized into the stream
				void SerializeToMessage(Canberra::Serialization::SerializableObject *, Canberra::Utility::Core::ByteStream &);

				//Deserializes the data from a serialized message into the defined data type.  Caller
				//is responsible for deleting the memory of the data type
				Canberra::Serialization::SerializableObject *DeserializeFromMessage(Canberra::Utility::Core::ByteStream &);

				//Indicates that the specified message version is supported by this factory
				static bool SupportsMsgVersion(const LONG Version);
			};
		}
	}
}
