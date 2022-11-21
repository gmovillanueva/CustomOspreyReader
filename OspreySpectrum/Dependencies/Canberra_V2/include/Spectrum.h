#pragma once

#include "DataTypesImpExp.h"
#include "SerializableType.h"

//Data description for a spectrum
struct SpectrumMetaDataS {
	ULONG Encoding;				//The encoding type
	ULONG NumChannels;			//The number of channels before encoding
	ULONG EncodingSize;			//The number of channels after encoding
};

namespace Canberra {
	namespace DataTypes {
		namespace Spectroscopy {

			//
			//An instance of this class is used to serialize
			//a spectrum
			//
			class DATATYPES_DLLIMPORT Spectrum : public Canberra::Serialization::SerializableObject
			{
			protected:
				struct SpectrumMetaDataS _Data;		//Metadata describing the spectrum
				LONG *_Spectrum;					//The spectral memory
				bool _HasSerializationMetaData;		//Indicates serialization meta data exists
				void Copy(const Spectrum &);		//Deep copy

				//Allocates the memory
				void Allocate(ULONG);

				//Frees the memory
				void Free();

				//Gets the data size
				ULONG GetDataSize();

				//Serializes the data
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &, const Canberra::Serialization::MetaData &, const LONG =0);

				//Deserializes the data
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &, const Canberra::Serialization::MetaData &, const LONG =0);

				void HandleEndianness(bool);

			public:
				typedef enum EncodingTypeTag {
					None=0,						//No encoding is used
					DifferenceCompression=1,	//2nd difference encoding is used
				} EncodingType;

				Spectrum(const Spectrum&);
				Spectrum& operator=(const Spectrum&);
				Spectrum(ULONG NumCh, EncodingType T=None);
				Spectrum(bool HasMeta=false);
				virtual ~Spectrum(void);

				//Get/set the encoding
				EncodingType GetEncoding();
				void PutEncoding(EncodingType v);

				//Get/set accessor to enable/disabling writing of data type meta data
				bool GetSerializeWithMeta();
				void PutSerializeWithMeta(bool v);

				//Serializes this instance to a stream
				ULONG Serialize(Canberra::Utility::Core::ByteStream &, const LONG Offset=0);

				//Deserializes this instance from a stream
				ULONG Deserialize(Canberra::Utility::Core::ByteStream &, const LONG Offset=0);

				//Size of this info in bytes
				ULONG GetSize();

				//Get spectrum memory pointer
				const LONG * const GetCounts();

				//Get/set the number of channels
				ULONG GetNumChannels();
				void PutNumChannels(ULONG N);

			};
		}
	}
}
