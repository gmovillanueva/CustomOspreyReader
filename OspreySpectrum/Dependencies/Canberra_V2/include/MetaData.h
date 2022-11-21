#pragma once
#include "SerializationImpExp.h"
#include "Serializable.h"

namespace Canberra {
	namespace Serialization {

#pragma pack(push, 1)
		struct MetaHeader {
			ULONG DataType;			//The type of data type
			ULONG DataSize;			//The data size
		};
#pragma pack(pop)

		//
		//An instance of this class is used to describe data
		//that has been serialized into the stream
		//
		class SERIALIZATION_IMPORT_USAGE MetaData : public Serializable
		{
		protected:
			struct MetaHeader _Header;				//The meta data header

		public:
			MetaData(void);
			MetaData(ULONG Type, ULONG Size);
			virtual ~MetaData(void);

			//Returns the size of the meta data (bytes)
			ULONG GetSize();

			//Returns the size of the data defined by the meta data (bytes)
			ULONG GetDataSize();

			//Returns the type code
			ULONG GetDataType();

			ULONG Serialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0);
			ULONG Deserialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0);
		};
	}
}

