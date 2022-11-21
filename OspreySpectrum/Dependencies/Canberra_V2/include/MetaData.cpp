#include "MetaData.h"
#include "DebugMacros.h"
#include "endianUtils.h"
#include <memory.h>

///<summary>
///This method will initialize this instance.  An instance of this
///class is used to define all of the metadata used to define a native
///type for serialization
///</summary>
Canberra::Serialization::MetaData::MetaData() {
	memset(&_Header, 0, sizeof(_Header));
}
Canberra::Serialization::MetaData::MetaData(ULONG Type, ULONG Size) {
	memset(&_Header, 0, sizeof(_Header));
	_Header.DataSize = Size;
	_Header.DataType = Type;
}
Canberra::Serialization::MetaData::~MetaData() {}

///<summary>
///This method will serialize this instance into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::Serialization::MetaData::Serialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	if (Offset < 0) {
		ULONG Stat = (ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(L"MetaData", L"Serialize", L"Invalid Offset size", Stat);
		throw Stat;
	}
	ULONG Size=sizeof(_Header);
	if ((Offset+Size) > Str.GetSize()) Str.Resize(Offset+Size);

	LONG Pos=Offset;
	LONG NumCpy=0;
	LONG Buf=0;
	Buf = _Header.DataType;
	SWAPNBYTES(&Buf, sizeof(Buf));
	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &Buf, NumCpy=sizeof(Buf)); Pos+=NumCpy;
	Buf = _Header.DataSize;
	SWAPNBYTES(&Buf, sizeof(Buf));
	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &Buf, NumCpy=sizeof(Buf)); Pos+=NumCpy;

	//memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_Header, NumCpy=sizeof(_Header)); Pos+=NumCpy;
	return Pos-Offset;
}

///<summary>
///This method will deserialize info from the byte stream into this instance
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the read in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
ULONG Canberra::Serialization::MetaData::Deserialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	ULONG Size=sizeof(_Header);
	if (((Offset+Size) > Str.GetSize()) || (Offset < 0)) {
		ULONG Stat = (ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(L"NativeTypeHeader", L"Deserialize", L"Invalid Offset size", Stat);
		throw Stat;
	}
	LONG Pos=Offset;
	LONG NumCpy=0;
	LONG Buf=0;
	memcpy(&Buf, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(Buf)); Pos+=NumCpy;
	SWAPNBYTES(&Buf, sizeof(Buf));
	_Header.DataType = Buf;
	memcpy(&Buf, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(Buf)); Pos+=NumCpy;
	SWAPNBYTES(&Buf, sizeof(Buf));
	_Header.DataSize = Buf;

	//memcpy(&_Header, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_Header)); Pos+=NumCpy;
	return Pos-Offset;
}

ULONG Canberra::Serialization::MetaData::GetSize() {return sizeof(_Header); }

//Returns the size of the data defined by the meta data (bytes)
ULONG Canberra::Serialization::MetaData::GetDataSize() {return _Header.DataSize; }

//Returns the type code
ULONG Canberra::Serialization::MetaData::GetDataType() {return _Header.DataType; }
