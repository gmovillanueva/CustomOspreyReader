#pragma once
#include "ByteStream.h"
#include "SerializationImpExp.h"

namespace Canberra {
	namespace Serialization {

		//
		//This is an abstract class for all types that
		//wish to support serialization
		//
		class SERIALIZATION_IMPORT_USAGE Serializable
		{
			public:
				Serializable(void) {}
				virtual ~Serializable(void) {}

				//Serializes the derived classes information into this stream
				virtual ULONG Serialize(Canberra::Utility::Core::ByteStream &, const LONG Offset=0)=0;

				//Deserializes the information in the stream into the derived class instance
				virtual ULONG Deserialize(Canberra::Utility::Core::ByteStream &, const LONG Offset=0)=0;
		};
	}
}
