#pragma once
#ifndef UTILITY_DLLIMPORT
#define UTILITY_DLLIMPORT
#endif
#include "SerializationImpExp.h"
#include "SerializableObject.h"
#include "Variant.h"

namespace Canberra {
	namespace Serialization {

		//
		//This is a class that is used to serialize the
		//data contained within a variant type.  NOTE: The
		//serialization method will place the native type
		//into the stream not the actual variant.  Similarly,
		//when deserialize the deserialize routine assumes the
		//data is of native type format.
		//
		class SERIALIZATION_IMPORT_USAGE VariantSerializer
		{
		protected:
			//Utility class so prevent creation and destruction
			VariantSerializer(void);
			virtual ~VariantSerializer(void);

		public:

			//Returns the size of this type (bytes)
			static ULONG GetSize(Canberra::Utility::Core::Variant &V);
				
			//Serialize the supplied variant into the stream
			static ULONG Serialize(Canberra::Utility::Core::ByteStream &Str, Canberra::Utility::Core::Variant &V, const LONG Offset=0);

			//Deserialize the stream and place contents into the variant
			static ULONG Deserialize(Canberra::Utility::Core::ByteStream &Str, Canberra::Utility::Core::Variant &V, const LONG Offset=0);
		};
	}
}
