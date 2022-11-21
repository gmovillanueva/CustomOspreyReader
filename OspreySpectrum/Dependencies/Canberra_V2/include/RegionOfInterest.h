#pragma once
#include "DataTypesImpExp.h"
#include "ParameterCodes.h"
#include "SerializableObjectCodes.h"
#include "UtilityImpExp.h"
#include "Variant.h"
#include "UtilityString.h"
#include "SerializableObject.h"
#include "VariantSerializer.h"

namespace Canberra {
	namespace DataTypes {
		namespace Spectroscopy {
#pragma pack(push, 1)
			struct RoiDataS {
				ULONG Start;			//The start channel
				ULONG End;				//The end channel
				ULONG Type;				//The type of
			};
#pragma pack(pop)

			//
			//An instance of this class is used to serialize
			//a Region of Interest
			//
			class DATATYPES_DLLIMPORT RegionOfInterest : public Canberra::Serialization::SerializableObject {
			protected:
				struct RoiDataS _Data;											//The ROI data
				void Copy(const RegionOfInterest &v);							//Deep copy
				void HandleEndianness();

			public:
				RegionOfInterest(const RegionOfInterest &v);
				RegionOfInterest& operator=( const RegionOfInterest &Rhs);
				RegionOfInterest();
				RegionOfInterest(ULONG Start, ULONG End, ULONG Type);
				virtual ~RegionOfInterest();

				//Serialization stuff
				ULONG GetDataSize();
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0);
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0);

				//Accessor to the start channel
				ULONG GetStartChannel();
				void PutStartChannel(ULONG);

				//Accessor to the end channel
				ULONG GetEndChannel();
				void PutEndChannel(ULONG);

				//Accessor to the type
				ULONG GetRegionType();
				void PutRegionType(ULONG);

			};
		}
	}
}
