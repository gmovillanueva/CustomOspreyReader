#include "Spectrum.h"
#include "DebugMacros.h"
#include "endianUtils.h"
#include "SerializableObjectCodes.h"

//Legacy spectral compression routines
extern "C" LONG ndl_diffdata_compress(LONG *input,
                           ULONG channels,
                           char *output,
                           ULONG *bytes);
extern "C" LONG ndl_diffdata_decomp(unsigned char *input,
                         ULONG bytes,
                         LONG *output,
                         LONG *channels);

///<summary>
///An instance of this class is used to define a spectrum
///</summary>
///<param name="NumChan">The number of channels in the spectrum to allocate</param>
///<param name="Encoding">The encoding method</param>
///<exception cref="ULONG">The Win32 error code</exception>
Canberra::DataTypes::Spectroscopy::Spectrum::Spectrum(ULONG NumChan, EncodingType Encoding) :
	Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::SpectralData), _Spectrum(NULL),
	_HasSerializationMetaData(false)
{
	memset(&_Data, 0, sizeof(_Data));
	_Data.NumChannels = NumChan;
	_Data.EncodingSize = NumChan*sizeof(LONG);
	_Data.Encoding = (USHORT) Encoding;
	Allocate(NumChan);
}

///<summary>
///An instance of this class is used to define a spectrum
///</summary>
///<param name="HasMeta">State indicating that serialization meta data is to be included</param>
Canberra::DataTypes::Spectroscopy::Spectrum::Spectrum(bool HasMeta) :
	Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::SpectralData), _Spectrum(NULL),
	_HasSerializationMetaData(HasMeta)
{
	memset(&_Data, 0, sizeof(_Data));
	_Data.NumChannels = 0;
	_Data.EncodingSize = 0;
	_Data.Encoding = (USHORT) None;
}

///<summary>
///This will create an instance of this class using the
///data in the argument.  A deep copy is performed
///</summary>
///<param name="Src">Spectrum to copy</param>
///<exception cref="ULONG">The Win32 error code</exception>
Canberra::DataTypes::Spectroscopy::Spectrum::Spectrum(const Spectrum &v) : Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::SpectralData), _Spectrum(NULL)  {
	memset(&_Data, 0, sizeof(_Data));
	Copy(v);
}

///<summary>
///This will release resources associated with this instance
///</summary>
Canberra::DataTypes::Spectroscopy::Spectrum::~Spectrum(void)
{
	Free();
}

///<summary>
///This will perform a deep copy
///</summary>
///<param name="Src">Spectrum to copy</param>
///<exception cref="ULONG">The Win32 error code</exception>
void Canberra::DataTypes::Spectroscopy::Spectrum::Copy(const Spectrum &Src)
{
	Free();
	memcpy(&_Data, &(Src._Data), sizeof(_Data));
	if (_Data.NumChannels > 0) {
		Allocate(_Data.NumChannels);
		memcpy(_Spectrum, Src._Spectrum, sizeof(LONG)*_Data.NumChannels);
	}
	_HasSerializationMetaData = Src._HasSerializationMetaData;
}


///<summary>
///This will perform a deep copy for the equal operator
///</summary>
///<param name="Src">Spectrum to copy</param>
Canberra::DataTypes::Spectroscopy::Spectrum& Canberra::DataTypes::Spectroscopy::Spectrum::operator=( const Spectrum &Rhs) {Copy(Rhs); return *this;}

///<summary>
///This method will release the spectrum memory
///</summary>
void Canberra::DataTypes::Spectroscopy::Spectrum::Free()
{
	if (!_Spectrum) return;
	delete [] _Spectrum;
	_Spectrum = NULL;
	_Data.NumChannels = 0;
	_Data.EncodingSize = 0;
}

///<summary>
///This method will allocate the memory for the spectrum
///</summary>
///<param name="NumChan">The number of channels in the spectrum to allocate</param>
///<exception cref="ULONG">The Win32 error code</exception>
void Canberra::DataTypes::Spectroscopy::Spectrum::Allocate(ULONG NumChan)
{
	if (_Spectrum) {
		if (NumChan != _Data.NumChannels) {Free();}
		else {
			memset(_Spectrum, 0, sizeof(LONG)*_Data.NumChannels);
			return;
		}
	}
	if (!NumChan) return;

	_Spectrum = new LONG[NumChan];
	if (NULL == _Spectrum) {
		ULONG Stat = (ULONG) E_OUTOFMEMORY;
		DEBUG_ERROR_MESSAGE(L"Spectrum", L"Allocate", L"Spectrum too large", Stat);
		throw Stat;
	}
	_Data.NumChannels = NumChan;

	memset(_Spectrum, 0, sizeof(LONG)*_Data.NumChannels);
}

void Canberra::DataTypes::Spectroscopy::Spectrum::HandleEndianness(bool spec)
{
	if (spec) {
		for(ULONG i=0; i<_Data.NumChannels; i++) {
			SWAPNBYTES(&(_Spectrum[i]), sizeof(_Spectrum[i]));
		}
	}
	else {
		SWAPNBYTES(&(_Data.Encoding), sizeof(_Data.Encoding));
		SWAPNBYTES(&(_Data.EncodingSize), sizeof(_Data.EncodingSize));
		SWAPNBYTES(&(_Data.NumChannels), sizeof(_Data.NumChannels));
	}
}

///<summary>
///This method will serialize this instance into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::Spectrum::SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &, const LONG Offset)
{
	ULONG Size=GetSize();
	if (Offset < 0) {
		ULONG Stat = (ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(L"Spectrum", L"Serialize", L"Invalid Offset size", Stat);
		throw Stat;
	}
	if ((Offset+Size) > Str.GetSize()) Str.Resize(Offset+Size);

	LONG Pos=Offset;
	LONG NumCpy=0;
	if (_Spectrum) {
		if (DifferenceCompression == _Data.Encoding) {
			ULONG CompSize=5*_Data.NumChannels;			//5 bytes as specified by ndl_diffdata_compress
			std::auto_ptr<char> Comp(new char[CompSize]);
			memset(Comp.get(), 0, CompSize);
			ULONG Stat = ndl_diffdata_compress(_Spectrum, _Data.NumChannels, Comp.get(), &_Data.EncodingSize);
			if (Stat) {
				DEBUG_ERROR_MESSAGE(L"Spectrum", L"Serialize", L"Error during spectral compression", Stat);
				_Data.EncodingSize = _Data.NumChannels*sizeof(LONG);
				_Data.Encoding = 0;
				HandleEndianness(false);
				memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_Data, NumCpy=sizeof(_Data)); Pos+=NumCpy;
				HandleEndianness(false);
				HandleEndianness(true);
				memcpy(const_cast<char *>(Str.GetBuffer()+Pos), _Spectrum, NumCpy=_Data.NumChannels*sizeof(LONG)); Pos+=NumCpy;
				HandleEndianness(true);
			}
			else {
				HandleEndianness(false);
				memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_Data, NumCpy=sizeof(_Data)); Pos+=NumCpy;
				HandleEndianness(false);
				HandleEndianness(true);
				memcpy(const_cast<char *>(Str.GetBuffer()+Pos), Comp.get(), NumCpy=_Data.EncodingSize); Pos+=NumCpy;
				HandleEndianness(true);
			}
		}
		else if (None == _Data.Encoding) {
			_Data.EncodingSize = _Data.NumChannels*sizeof(LONG);
			HandleEndianness(false);
			memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_Data, NumCpy=sizeof(_Data)); Pos+=NumCpy;
			HandleEndianness(false);
			HandleEndianness(true);
			memcpy(const_cast<char *>(Str.GetBuffer()+Pos), _Spectrum, NumCpy=(_Data.NumChannels*sizeof(LONG))); Pos+=NumCpy;
			HandleEndianness(true);
		}
		else {
			ULONG Stat = (ULONG) GEN_COMPRESSION_UNSUPPORT;
			DEBUG_ERROR_MESSAGE(L"Spectrum", L"Serialize",
								Canberra::Utility::Core::String(L"Unsupported compression method. Specified compression method is: ") +
								Canberra::Utility::Core::String(_Data.Encoding) , Stat);
			throw Stat;
		}
	}
	return Pos-Offset;
}

///<summary>
///This method will serialize this instance into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::Spectrum::Serialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	if (!_HasSerializationMetaData) {
		Canberra::Serialization::MetaData MD(this->_Type, GetSize());
		return SerializeData(Str, MD, Offset);
	}
	else return SerializableObject::Serialize(Str, Offset);
}

///<summary>
///This method will deserialize info from the byte stream into this instance
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the read in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::Spectrum::DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &, const LONG Offset)
{
	Free();
	ULONG Size=sizeof(_Data);
	if (((Offset+Size) > Str.GetSize()) || (Offset < 0)) {
		ULONG Stat = (ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(L"Spectrum", L"DeserializeData", L"Invalid Offset size", Stat);
		throw Stat;
	}
	LONG Pos=Offset;
	LONG NumCpy=0;
	memcpy(&_Data, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_Data)); Pos+=NumCpy;
	HandleEndianness(false);
	Allocate(_Data.NumChannels);

	if (DifferenceCompression == _Data.Encoding) {
		//Verify data contained within stream fits within stream
		if ((Pos+_Data.EncodingSize) > Str.GetSize()) {
			ULONG Stat = (ULONG) GEN_INVALID_MSGCONTENT;
			DEBUG_ERROR_MESSAGE(L"Spectrum", L"DeserializeData", L"Invalid stream size", Stat);
			throw Stat;
		}

		LONG NumChan=_Data.NumChannels;
		std::auto_ptr<unsigned char> Comp(new unsigned char[_Data.EncodingSize]);
		memset(Comp.get(), 0, _Data.EncodingSize);
		memcpy(Comp.get(), const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=_Data.EncodingSize); Pos+=NumCpy;
		ULONG Stat = ndl_diffdata_decomp(Comp.get(), _Data.EncodingSize, _Spectrum, &NumChan);
		if (Stat) {
			DEBUG_ERROR_MESSAGE(L"Spectrum", L"DeserializeData", L"Error during spectral decompression", Stat);
			throw Stat;
		}
	}
	else if (None == _Data.Encoding) {
		LONG Num2Copy = (sizeof(LONG)*_Data.NumChannels);

		//Verify data contained within stream fits within stream
		if ((DWORD) (Pos+Num2Copy) > Str.GetSize()) {
			ULONG Stat = (ULONG) GEN_INVALID_MSGCONTENT;
			DEBUG_ERROR_MESSAGE(L"Spectrum", L"DeserializeData", L"Invalid stream size", Stat);
			throw Stat;
		}

		memcpy(_Spectrum, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=(sizeof(LONG)*_Data.NumChannels)); Pos+=NumCpy;
	}
	else {
		ULONG Stat = (ULONG) GEN_COMPRESSION_UNSUPPORT;
		DEBUG_ERROR_MESSAGE(L"Spectrum", L"DeserializeData",
							Canberra::Utility::Core::String(L"Unsupported compression method. Specified compression method is: ") +
							Canberra::Utility::Core::String(_Data.Encoding) , Stat);
		throw Stat;
	}
	HandleEndianness(true);
	return Pos-Offset;
}


///<summary>
///This method will deserialize info from the byte stream into this instance
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the read in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::Spectrum::Deserialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	if (!_HasSerializationMetaData) {
		Canberra::Serialization::MetaData MD(this->_Type, sizeof(_Data));
		return DeserializeData(Str, MD, Offset);
	}
	else return SerializableObject::Deserialize(Str, Offset);
}

///<summary>
///This method will return the number of bytes needed to serialize
///this instances data
///</summary>
///<returns>The number of bytes</returns>
ULONG Canberra::DataTypes::Spectroscopy::Spectrum::GetDataSize()
{
	if (DifferenceCompression == _Data.Encoding) {
		ULONG CompSize=5*_Data.NumChannels;			//5 bytes as specified by ndl_diffdata_compress
		std::auto_ptr<char> Comp(new char[CompSize]);
		memset(Comp.get(), 0, CompSize);
		ULONG Stat = ndl_diffdata_compress(_Spectrum, _Data.NumChannels, Comp.get(), &_Data.EncodingSize);
		if (Stat) {
			DEBUG_ERROR_MESSAGE(L"Spectrum", L"Serialize", L"Error during spectral compression", Stat);
			return sizeof(LONG)*_Data.NumChannels + sizeof(_Data);
		}
		else {
			return sizeof(LONG)*_Data.EncodingSize + sizeof(_Data);
		}
	}
	else {
		return sizeof(LONG)*_Data.NumChannels + sizeof(_Data);
	}

}

///<summary>
///This method will get the state of the serialization option.  This
///option is used to indicate that serialization/deserialization data
///contains metadata.  If True then the serialization information
///contain metadata used to describe the data.  If Fase then the serialization
///information does not contain metadata
///</summary>
///<returns>The value</returns>
bool Canberra::DataTypes::Spectroscopy::Spectrum::GetSerializeWithMeta() {return _HasSerializationMetaData;}


///<summary>
///This method will set the state of the serialization option.  This
///option is used to indicate that serialization/deserialization data
///contains metadata.  If True then the serialization information
///contain metadata used to describe the data.  If Fase then the serialization
///information does not contain metadata
///</summary>
///<param name="v">The byte position to start the read in the stream</param>
void Canberra::DataTypes::Spectroscopy::Spectrum::PutSerializeWithMeta(bool v) {_HasSerializationMetaData=v;}

///<summary>
///This method will return the number of bytes needed to serialize
///this instance
///</summary>
///<returns>The number of bytes</returns>
ULONG Canberra::DataTypes::Spectroscopy::Spectrum::GetSize()
{
	if (!_HasSerializationMetaData) return GetDataSize();
	else {
		return SerializableObject::GetSize();
	}
}

///<summary>
///This method will get the encoding method being used
///</summary>
///<returns>The value</returns>
Canberra::DataTypes::Spectroscopy::Spectrum::EncodingType Canberra::DataTypes::Spectroscopy::Spectrum::GetEncoding() {return (EncodingType) _Data.Encoding;}

///<summary>
///This method will set the encoding method being used
///</summary>
///<returns>The value</returns>
void Canberra::DataTypes::Spectroscopy::Spectrum::PutEncoding(EncodingType v) {_Data.Encoding= (USHORT) v;}

///<summary>
///This method will get the spectrum counts
///</summary>
///<returns>The value</returns>
const LONG * const Canberra::DataTypes::Spectroscopy::Spectrum::GetCounts() {return _Spectrum;}

///<summary>
///This method will get the number of channels in the spectrum
///</summary>
///<returns>The value</returns>
ULONG Canberra::DataTypes::Spectroscopy::Spectrum::GetNumChannels() {return _Data.NumChannels;}


///<summary>
///This method will set the number of channels in the spectrum.  This method
///will allocate the necessary memory.
///</summary>
///<param name="N">The number of channels</param>
///<exception cref="ULONG">The Win32 error code</exception>
void Canberra::DataTypes::Spectroscopy::Spectrum::PutNumChannels(ULONG N) {Allocate(N);}
