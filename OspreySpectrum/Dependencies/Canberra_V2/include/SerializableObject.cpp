#include "SerializableObject.h"
#include "DebugMacros.h"

Canberra::Serialization::SerializableObject::SerializableObject(ULONG Type) {_Type=Type;_UseMetaData=true;}
Canberra::Serialization::SerializableObject::~SerializableObject(void) {}

//Returns the type code
ULONG Canberra::Serialization::SerializableObject::GetType() {return _Type;}

//Returns the size (bytes) of the data (this includes meta data)
ULONG Canberra::Serialization::SerializableObject::GetSize() { MetaData MD; return (_UseMetaData ? MD.GetSize() : 0) + GetDataSize();}

///<summary>
///This method will serialize this instances data into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::Serialization::SerializableObject::Serialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	MetaData MD(GetType(), GetDataSize());
	ULONG StreamSize=GetSize();
	if (Offset < 0) {
		ULONG Stat = (ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(L"SerializableObject", L"Serialize", L"Invalid Offset size", Stat);
		throw Stat;
	}
	if ((Offset+StreamSize) > Str.GetSize()) Str.Resize(Offset+StreamSize);

	LONG Pos=Offset;
	LONG NumCpy=0;
	if (_UseMetaData) {NumCpy=MD.Serialize(Str, Offset); Pos+=NumCpy;}
	NumCpy = SerializeData(Str, MD, Pos); Pos+=NumCpy;
	return Pos-Offset;
}

///<summary>
///This method will deserialize info from the byte stream into this instance
///data member
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the read in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
ULONG Canberra::Serialization::SerializableObject::Deserialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	MetaData MD;

	LONG Pos = Offset;
	LONG NumCpy = 0;
	if (_UseMetaData) {
		NumCpy=MD.Deserialize(Str, Offset); Pos+=NumCpy;

		if (Str.GetSize() < (Offset+MD.GetDataSize())) {
			ULONG Stat = (ULONG) E_INVALIDARG;
			DEBUG_ERROR_MESSAGE(L"SerializableObject", L"Deserialize", L"Stream to deserialize is < Header size or Offset argument is invalid", Stat);
			throw Stat;
		}

		if (((ULONG) GetType()) != MD.GetDataType()) {
			ULONG Stat = (ULONG) GEN_INVALID_MSGCONTENT;
			DEBUG_ERROR_MESSAGE(L"SerializableObject", L"Deserialize", L"Data in stream does not match this instances data type", Stat);
			throw Stat;
		}
	}

	NumCpy = DeserializeData(Str, MD, Pos); Pos+=NumCpy;
	return Pos-Offset;
}
bool Canberra::Serialization::SerializableObject::GetUseMetaData() {return _UseMetaData;}
void Canberra::Serialization::SerializableObject::PutUseMetaData(bool v) {_UseMetaData=v;}
