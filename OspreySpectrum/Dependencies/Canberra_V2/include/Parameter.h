#pragma once
#include "DataTypesImpExp.h"
#include "SerializableObjectCodes.h"
#include "UtilityImpExp.h"
#include "Variant.h"
#include "UtilityString.h"
#include "SerializableObject.h"
#include "VariantSerializer.h"

namespace Canberra {
	namespace DataTypes {
		namespace Application {

			//
			//An instance of this class is used to serialize
			//a single parameter value
			//
			class DATATYPES_DLLIMPORT Parameter : public Canberra::Serialization::SerializableObject {
			protected:
				ULONG _ParameterCode;										//The parameter code
				Canberra::Utility::Core::Variant _Value;					//The value
				void Copy(const Parameter &v);								//Deep copy

			public:
				Parameter(const Parameter &v);
				Parameter& operator=( const Parameter &Rhs);
				Parameter();
				Parameter(LONG C, const Canberra::Utility::Core::Variant &Val);
				virtual ~Parameter();

				//Serialization stuff
				ULONG GetDataSize();
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0);
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0);

				//Accessor to the parameter code
				LONG GetParameterCode();
				void PutParameterCode(LONG v);

				//Accessor to the value
				const Canberra::Utility::Core::Variant &GetValue();
				void PutValue(const Canberra::Utility::Core::Variant &v);

			};
		}
	}
}
