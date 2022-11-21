#pragma once
#include "DataTypesImpExp.h"
#include "Serializer.h"

namespace Canberra {
	namespace DataTypes {
		namespace Serialization {
			//
			//This is a utility class used to serialize any
			//DSA application type
			//
			class DATATYPES_DLLIMPORT ApplicationSerializer : public Canberra::Serialization::Serializer {
			public:
				ApplicationSerializer();
				virtual ~ApplicationSerializer();

				//Serializes an instance
				void Serialize(Canberra::Serialization::SerializableObject *Data, Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0);

				//Serializes the data into the stream
				ULONG Serialize(const Canberra::Serialization::SerializableObject &Data, Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0);

				//Deserializes an instance
				Canberra::Serialization::SerializableObject *Deserialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0);

				//Deserializes an instance
				ULONG Deserialize(Canberra::Serialization::SerializableObject &Obj, Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0);

			};
		}
	}
}
