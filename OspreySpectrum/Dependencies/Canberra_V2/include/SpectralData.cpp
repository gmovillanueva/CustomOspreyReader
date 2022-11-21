#include "Spectrum.h"
#include "DebugMacros.h"
#include "SpectralData.h"


Canberra::DataTypes::Spectroscopy::SpectralData::SpectralData(Canberra::DataTypes::Serialization::SerializableObjects::TypeCode Type, ULONG Chans) :
		Canberra::Serialization::SerializableObject(Type),
		_Spectrum(Chans) {
	memset(&_Source, 0, sizeof(_Source));
}
Canberra::DataTypes::Spectroscopy::SpectralData::~SpectralData(void) {}

//Get/set spectrum
Canberra::DataTypes::Spectroscopy::Spectrum &Canberra::DataTypes::Spectroscopy::SpectralData::GetSpectrum() {return _Spectrum;}

//Get/set start time
LONGLONG Canberra::DataTypes::Spectroscopy::SpectralData::GetStartTime() {return _Source.StartTime;};
void Canberra::DataTypes::Spectroscopy::SpectralData::PutStartTime(LONGLONG v) {_Source.StartTime=v;}

//Get/status
ULONG Canberra::DataTypes::Spectroscopy::SpectralData::GetStatus() {return _Source.Status;};
void Canberra::DataTypes::Spectroscopy::SpectralData::PutStatus(ULONG v) {_Source.Status=v;}

//Get/set input number
USHORT Canberra::DataTypes::Spectroscopy::SpectralData::GetInput() {return _Source.Input;}
void Canberra::DataTypes::Spectroscopy::SpectralData::PutInput(USHORT v) {_Source.Input=v;}

//Get/set group number
USHORT Canberra::DataTypes::Spectroscopy::SpectralData::GetGroup() {return _Source.Group;}
void Canberra::DataTypes::Spectroscopy::SpectralData::PutGroup(USHORT v) {_Source.Group=v;}

//Returns the size
ULONG Canberra::DataTypes::Spectroscopy::SpectralData::GetDataSize() {return sizeof(_Source) + _Spectrum.GetSize(); }

///<summary>
///This method will serialize this instance into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="MD">The metadata</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::SpectralData::SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &, const LONG Offset)
{
	LONG Pos=Offset;
	LONG NumCpy=0;
	struct SpectralDataS CopyOfData = _Source;
	SWAPNBYTES(&(_Source.Group), sizeof(_Source.Group));
	SWAPNBYTES(&(_Source.Input), sizeof(_Source.Input));
	SWAPNBYTES(&(_Source.StartTime), sizeof(_Source.StartTime));
	SWAPNBYTES(&(_Source.Status), sizeof(_Source.Status));
	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_Source, NumCpy=sizeof(_Source)); Pos+=NumCpy;
	_Source = CopyOfData;
	NumCpy=_Spectrum.Serialize(Str, Pos); Pos+=NumCpy;
	return Pos-Offset;
}

///<summary>
///This method will deserialize info from the byte stream into this instance
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the read in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::SpectralData::DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &, const LONG Offset)
{
	LONG Pos=Offset;
	LONG NumCpy=0;
	memcpy(&_Source, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_Source)); Pos+=NumCpy;
	SWAPNBYTES(&(_Source.Group), sizeof(_Source.Group));
	SWAPNBYTES(&(_Source.Input), sizeof(_Source.Input));
	SWAPNBYTES(&(_Source.StartTime), sizeof(_Source.StartTime));
	SWAPNBYTES(&(_Source.Status), sizeof(_Source.Status));
	NumCpy=_Spectrum.Deserialize(Str, Pos); Pos+=NumCpy;
	return Pos-Offset;
}


