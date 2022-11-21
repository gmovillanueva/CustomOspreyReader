#pragma once
#include "DataTypesImpExp.h"
#include "ParameterCodes.h"
#include "SerializableObjectCodes.h"
#include "UtilityImpExp.h"
#include "Variant.h"
#include "UtilityString.h"
#include "SerializableObject.h"
#include "VariantSerializer.h"
#include "VariantArray.h"
#include "ParameterAttributes.h"

namespace Canberra {
	namespace DataTypes {
		namespace Application {

			//
			//An instance of this class is used to serialize
			//a single ParameterMetaData value
			//
			class DATATYPES_DLLIMPORT ParameterMetaData : public Canberra::Serialization::SerializableObject {
			protected:
				Canberra::Utility::Core::Variant _Desc;						//A description of the parameter
				Canberra::Utility::Core::Variant _Name;						//The parameter name
				ULONG _Code;												//The parameter code
				Canberra::Utility::Core::Variant _Min;						//The min
				Canberra::Utility::Core::Variant _Max;						//The max
				Canberra::Utility::Core::Variant _Def;						//The default
				Canberra::Utility::Core::Variant _Step;						//The step size
				ULONG _Attr;												//The attributes
				Canberra::Utility::Core::VariantArray _Enum;				//The enumeration of values
				ULONG _ParameterDataType;									//The parameter data type
				Canberra::Utility::Core::VariantArray _EnumStrings;			//The enumeration strings
				Canberra::Utility::Core::VariantArray _EnumStringIds;		//The enumeration strings resource ids

			public:
				ParameterMetaData();
				ParameterMetaData(const ParameterMetaData &v);
				ParameterMetaData& operator=( const ParameterMetaData &Rhs);
				ParameterMetaData(LONG C,
								  const Canberra::Utility::Core::Variant &Desc,
								  const Canberra::Utility::Core::Variant &Nam,
								  const Canberra::Utility::Core::Variant &Min,
								  const Canberra::Utility::Core::Variant &Max,
								  const Canberra::Utility::Core::Variant &Def,
								  const Canberra::Utility::Core::Variant &Step,
								  Canberra::Utility::Core::VariantArray &Enum,
								  Canberra::Utility::Core::VariantArray &EnumStrings,
								  Canberra::Utility::Core::VariantArray &EnumStringIds,
								  const LONG Attr, const LONG PType);
				virtual ~ParameterMetaData();

				//Deep copy
				void Copy(const ParameterMetaData &v);

				//Serialization stuff
				ULONG GetDataSize();
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0);
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0);

				//Accessor to the Parameter code
				LONG GetParameterCode();

				//Accessor to the description
				Canberra::Utility::Core::String GetDescription();

				//Accessor to the name
				Canberra::Utility::Core::String GetName();

				//Accessor to the parameter data type
				Canberra::DataTypes::Application::ParameterAttributes::ValueType GetParameterDataType();

				//Accessor to the min
				Canberra::Utility::Core::Variant GetMinimum();

				//Accessor to the max
				Canberra::Utility::Core::Variant GetMaximum();

				//Accessor to the default
				Canberra::Utility::Core::Variant GetDefault();

				//Accessor to the step size
				Canberra::Utility::Core::Variant GetStepSize();

				//Accessor to the enumeration
				Canberra::Utility::Core::VariantArray GetEnumeration();

				//Accessor to the enumeration strings
				Canberra::Utility::Core::VariantArray GetEnumerationStrings();

				//Accessor to the enumeration string id's
				Canberra::Utility::Core::VariantArray GetEnumerationStringIds();

				//Accessor to the attributes
				Canberra::DataTypes::Application::ParameterAttributes::Attributes GetProperties();

				//Validates the value is in range
				bool GetIsValid(const Canberra::Utility::Core::Variant &v);
			};
		}
	}
}
