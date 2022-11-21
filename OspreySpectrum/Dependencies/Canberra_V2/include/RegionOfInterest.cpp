#include "DebugMacros.h"
#include "RegionOfInterest.h"
#include "MetaData.h"
#include "endianUtils.h"
#include <memory.h>
///<summary>
///This method will perform a deep copy
///</summary>
///<param name="v">The src</param>
void Canberra::DataTypes::Spectroscopy::RegionOfInterest::Copy(const Canberra::DataTypes::Spectroscopy::RegionOfInterest &v) {
	memcpy(&_Data, &(v._Data), sizeof(_Data));
}
Canberra::DataTypes::Spectroscopy::RegionOfInterest& Canberra::DataTypes::Spectroscopy::RegionOfInterest::operator=( const RegionOfInterest &Rhs) {Copy(Rhs); return *this;}


///<summary>
///This method will initialize an instance of this class
///</summary>
///<param name="v">The src</param>
Canberra::DataTypes::Spectroscopy::RegionOfInterest::RegionOfInterest(const RegionOfInterest &v) : Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::RegionOfInterestData)  {Copy(v);}


///<summary>
///This method will initialize an instance of this class
///</summary>
Canberra::DataTypes::Spectroscopy::RegionOfInterest::RegionOfInterest() :
	Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::RegionOfInterestData)
{
	memset(&_Data, 0, sizeof(_Data));
}

///<summary>
///This method will initialize an instance of this class
///</summary>
///<param name="Start">The start channel</param>
///<param name="End">The end channel</param>
///<param name="Type">The type of ROI</param>
Canberra::DataTypes::Spectroscopy::RegionOfInterest::RegionOfInterest(ULONG Start, ULONG End, ULONG Type) :
	Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::RegionOfInterestData)
{
	memset(&_Data, 0, sizeof(_Data));
	_Data.Start = Start;
	_Data.End = End;
	_Data.Type = Type;
}

Canberra::DataTypes::Spectroscopy::RegionOfInterest::~RegionOfInterest() {}

//Accessors
ULONG Canberra::DataTypes::Spectroscopy::RegionOfInterest::GetStartChannel() {return _Data.Start;}
void Canberra::DataTypes::Spectroscopy::RegionOfInterest::PutStartChannel(ULONG v) {_Data.Start=v;}
ULONG Canberra::DataTypes::Spectroscopy::RegionOfInterest::GetEndChannel() {return _Data.End;}
void Canberra::DataTypes::Spectroscopy::RegionOfInterest::PutEndChannel(ULONG v) {_Data.End=v;}
ULONG Canberra::DataTypes::Spectroscopy::RegionOfInterest::GetRegionType() {return _Data.Type;}
void Canberra::DataTypes::Spectroscopy::RegionOfInterest::PutRegionType(ULONG v) {_Data.Type=v;}

///<summary>
///This method will return the size of the data to serialize in bytes
///</summary>
///<returns>The number of bytes</returns>
ULONG Canberra::DataTypes::Spectroscopy::RegionOfInterest::GetDataSize()
{
	return sizeof(_Data);
}
///<summary>
///This method will format the contained data for the
///processor
///</summary>
void Canberra::DataTypes::Spectroscopy::RegionOfInterest::HandleEndianness()
{
	SWAPNBYTES(&(_Data.End), sizeof(_Data.End));
	SWAPNBYTES(&(_Data.Start), sizeof(_Data.Start));
	SWAPNBYTES(&(_Data.Type), sizeof(_Data.Type));
}
///<summary>
///This method will serialize this instance into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="MD">The metadata</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::RegionOfInterest::SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &, const LONG Offset)
{
	LONG Pos=Offset;
	LONG NumCpy=0;
	HandleEndianness();
	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_Data, NumCpy=sizeof(_Data)); Pos+=NumCpy;
	HandleEndianness();
	return Pos-Offset;
}

///<summary>
///This method will deserialize info from the byte stream into this instance
///</summary>
///<param name="Str">The stream</param>
///<param name="MD">The metadata</param>
///<param name="Offset">The byte position to start the read in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::RegionOfInterest::DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &, const LONG Offset)
{
	LONG Pos=Offset;
	LONG NumCpy=0;
	memcpy(&_Data, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_Data)); Pos+=NumCpy;
	HandleEndianness();
	return Pos-Offset;
}
