#pragma once
#include "SerializationImpExp.h"
#include "Serializable.h"
#include "MetaData.h"
#include "DebugMacros.h"

namespace Canberra {
	namespace Serialization {

		//
		//This is an abstract class for any serializable
		//object instance
		//
		class SERIALIZATION_IMPORT_USAGE SerializableObject : public Serializable
		{
		protected:
			ULONG _Type;				//The data type
			bool _UseMetaData;			//Use meta data in serialization

			//Returns the size of the derived data (bytes)
			virtual ULONG GetDataSize()=0;

			//Serializes the derived data
			virtual ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0)=0;

			//Deserializes the derived data
			virtual ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0)=0;

		public:
			SerializableObject(ULONG Type);
			virtual ~SerializableObject(void);

			//
			//All known type codes.  Custom type codes are defined
			//by starting at CustomTypeOffset
			//
			typedef enum TypeCodeTag {
				Unknown=0,
				Byte=1,
				Short=2,
				Long=3,
				Ushort=4,
				Ubyte=5,
				Ulong=6,
				Float=7,
				Double=8,
				String=9,
				Int64=10,
				Uint64=11,
				Bool=12,
				Null=13,
				DateTime=14,
				Blob=15,
				Array=16,
				CustomTypeOffset=20,
			} TypeCode;

			//Returns the type code
			virtual ULONG GetType();

			//Returns the size (bytes) of the data (this includes meta data)
			virtual ULONG GetSize();

			virtual ULONG Serialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0);
			virtual ULONG Deserialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0);

			bool GetUseMetaData();
			void PutUseMetaData(bool v);
		};
	}
}
