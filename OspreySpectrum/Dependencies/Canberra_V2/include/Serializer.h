#pragma once
#include "SerializationImpExp.h"
#include "ByteStream.h"
#include "SerializableObject.h"

namespace Canberra {
	namespace Serialization {
			//
			//This is a utility class used to serialize data
			//native types (int, uint, double, etc..)
			//
			class SERIALIZATION_IMPORT_USAGE Serializer {
			public:
				Serializer();
				virtual ~Serializer();
				virtual void Serialize(SerializableObject *Data, Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0);
				virtual SerializableObject *Deserialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0);
			};
	}
}
