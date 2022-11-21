#include "DebugMacros.h"
#include "ScaData.h"
#include "MetaData.h"
#include "endianUtils.h"
#include <memory.h>
///<summary>
///This method will perform a deep copy
///</summary>
///<param name="v">The src</param>
void Canberra::DataTypes::Spectroscopy::SCAdefinitions::Copy(const Canberra::DataTypes::Spectroscopy::SCAdefinitions &v) {_Data = v._Data;}

///<summary>
///This method will create an instance of this class based on the supplied
///instance.  A deep copy of the argument is performed
///</summary>
///<param name="v">The src</param>
///<exception cref="ULONG">Error code</exception>
Canberra::DataTypes::Spectroscopy::SCAdefinitions::SCAdefinitions(const Canberra::DataTypes::Spectroscopy::SCAdefinitions &v) : Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::SCAdefinitionData) {Copy(v);}

///<summary>
///This method will create an instance of this class with no
///SCA definitions
///</summary>
Canberra::DataTypes::Spectroscopy::SCAdefinitions::SCAdefinitions() : Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::SCAdefinitionData)  {}

///<summary>
///This method will create an instance of this class with no
///SCA definitions
///</summary>
Canberra::DataTypes::Spectroscopy::SCAdefinitions::~SCAdefinitions() {}

///<summary>
///This method will perform a deep copy
///</summary>
///<param name="v">The src</param>
///<exception cref="ULONG">Error code</exception>
Canberra::DataTypes::Spectroscopy::SCAdefinitions& Canberra::DataTypes::Spectroscopy::SCAdefinitions::operator=( const Canberra::DataTypes::Spectroscopy::SCAdefinitions &Rhs) {Copy(Rhs); return *this;}

///<summary>
///This method will return the number of bytes
///the data contained within uses
///</summary>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::SCAdefinitions::GetDataSize() {return _Data.GetDataSize();}

void Canberra::DataTypes::Spectroscopy::SCAdefinitions::HandleEndianness()
{
	for(ULONG i=0; i<_Data.GetNumberOfItems(); i++) {
		SWAPNBYTES(&(_Data.GetItem(i).LLD), sizeof((_Data.GetItem(i).LLD)));
		SWAPNBYTES(&(_Data.GetItem(i).LLD), sizeof((_Data.GetItem(i).ULD)));
	}
}

///<summary>
///This method will serialize this instance into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::SCAdefinitions::SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData & /* MD */, const LONG Offset) {
	HandleEndianness();
	ULONG Size=_Data.SerializeData(Str, Offset);
	HandleEndianness();
	return Size;
}

///<summary>
///This method will deserialize this instance from the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::SCAdefinitions::DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData & /* MD */, const LONG Offset) {
	ULONG Size=_Data.DeserializeData(Str, Offset);
	HandleEndianness();
	return Size;
}

///<summary>
///This method will add a definition to the list
///</summary>
///<param name="v">The definition</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::SCAdefinitions::Add(const Canberra::DataTypes::Spectroscopy::SCAdefinitions::Definition &v) {return _Data.Add(v);}

///<summary>
///This method will clear all defs
///</summary>
void Canberra::DataTypes::Spectroscopy::SCAdefinitions::Clear() {_Data.Clear();}

///<summary>
///This method will return the # defs
///</summary>
///<returns>The number of definitions</returns>
ULONG Canberra::DataTypes::Spectroscopy::SCAdefinitions::GetNumberOfDefinitions() {return _Data.GetNumberOfItems();}

///<summary>
///This method will get a reference to the definition
///</summary>
///<param name="v">The index</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
Canberra::DataTypes::Spectroscopy::SCAdefinitions::Definition &Canberra::DataTypes::Spectroscopy::SCAdefinitions::GetDefinition(ULONG i) {return _Data.GetItem(i);}


///<summary>
///This method will perform a deep copy
///</summary>
///<param name="v">The src</param>
void Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry::Copy(const Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry &v)
{
	_Header = v._Header;
	_Data = v._Data;
}

///<summary>
///This method will create an instance of this class based on the supplied
///instance.  A deep copy of the argument is performed
///</summary>
///<param name="v">The src</param>
///<exception cref="ULONG">Error code</exception>
Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry::BufferEntry(const Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry &v) {Copy(v);}

///<summary>
///This method will create an instance of this class with an empty entry information
///</summary>
Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry::BufferEntry() {}

///<summary>
///This method will release all resources associated with this instance
///</summary>
Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry::~BufferEntry() {}

///<summary>
///This method will perform a deep copy
///</summary>
///<param name="v">The src</param>
///<exception cref="ULONG">Error code</exception>
Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry& Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry::operator=( const Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry &Rhs) {Copy(Rhs);return *this;}

///<summary>
///This method will return the number of bytes
///the data contained within uses
///</summary>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry::GetDataSize() {return _Data.GetDataSize()+sizeof(_Header);}
///<summary>
///This method will format the contained data for the
///processor
///</summary>
void Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry::HandleEndianness()
{
	SWAPNBYTES(&(_Header.ElapsedLive), sizeof(_Header.ElapsedLive));
	SWAPNBYTES(&(_Header.ElapsedReal), sizeof(_Header.ElapsedReal));
	SWAPNBYTES(&(_Header.Flags), sizeof(_Header.Flags));
	SWAPNBYTES(&(_Header.Spare), sizeof(_Header.Spare));
	SWAPNBYTES(&(_Header.StartTime), sizeof(_Header.StartTime));
}
///<summary>
///This method will serialize this instance into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry::SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData & /* MD */, const LONG Offset) {

	ULONG Size=GetDataSize();
	if (Offset < 0) {
		ULONG Stat = (ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(L"BufferEntry", L"Serialize", L"Invalid Offset size", Stat);
		throw Stat;
	}
	if ((Offset+Size) > Str.GetSize()) Str.Resize(Offset+Size);
	LONG Pos=Offset;
	LONG NumCpy=0;

	HandleEndianness();
	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_Header, NumCpy=sizeof(_Header)); Pos+=NumCpy;
	HandleEndianness();
	NumCpy=_Data.SerializeData(Str, Pos); Pos+=NumCpy;
	return Pos-Offset;
}

///<summary>
///This method will deserialize this instance from the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry::DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData & /* MD */, const LONG Offset) {
	LONG Pos=Offset;
	LONG NumCpy=0;
	_Data.Clear();
	memcpy(&_Header, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_Header)); Pos+=NumCpy;
	HandleEndianness();
	NumCpy=_Data.DeserializeData(Str, Pos); Pos+=NumCpy;
	return Pos-Offset;

}

///<summary>
///This method will clear all defs
///</summary>
void Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry::Clear() { memset(&_Header, 0, sizeof(_Header)); _Data.Clear();}

///<summary>
///This method will get a reference to the data associated with an SCA
///</summary>
///<param name="v">The index or SCA #</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The value</returns>
Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry::SCAdata &Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry::GetData(LONG i) {return _Data.GetItem(i);}

///<summary>
///This method will return the # data elements or SCAs contained in this instance
///</summary>
///<returns>The number of data elements in the buffer</returns>
ULONG Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry::GetNumberOfData() {return _Data.GetNumberOfItems();}

///<summary>
///This method will get a reference to the entry header
///</summary>
///<returns>The value</returns>
Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry::EntryHeader &Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry::GetHeader() {return _Header;}

///<summary>
///This method will perform a deep copy
///</summary>
///<param name="v">The src</param>
void Canberra::DataTypes::Spectroscopy::SCAbuffer::Copy(const Canberra::DataTypes::Spectroscopy::SCAbuffer &v) {
	_Header=v._Header; _Data = v._Data;
}

///<summary>
///This method will create an instance of this class based on the supplied
///instance.  A deep copy of the argument is performed
///</summary>
///<param name="v">The src</param>
///<exception cref="ULONG">Error code</exception>
Canberra::DataTypes::Spectroscopy::SCAbuffer::SCAbuffer(const Canberra::DataTypes::Spectroscopy::SCAbuffer &v) : Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::SCAbufferData) {Copy(v);}

///<summary>
///This method will create an instance of this class with an empty buffer
///</summary>
Canberra::DataTypes::Spectroscopy::SCAbuffer::SCAbuffer() : Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::SCAbufferData)  {}

///<summary>
///This method will create an instance of this class with no
///SCA definitions
///</summary>
Canberra::DataTypes::Spectroscopy::SCAbuffer::~SCAbuffer() {}

///<summary>
///This method will perform a deep copy
///</summary>
///<param name="v">The src</param>
///<exception cref="ULONG">Error code</exception>
Canberra::DataTypes::Spectroscopy::SCAbuffer& Canberra::DataTypes::Spectroscopy::SCAbuffer::operator=( const Canberra::DataTypes::Spectroscopy::SCAbuffer &Rhs) {Copy(Rhs);return *this;}

///<summary>
///This method will return the number of bytes
///the data contained within uses
///</summary>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::SCAbuffer::GetDataSize() {
	ULONG Cnt=sizeof(_Header)+sizeof(LONG);
	for(ULONG i=0; i<_Data.GetNumberOfItems(); i++) {Cnt+=_Data.GetItem(i).GetDataSize();}
	return Cnt;
}
///<summary>
///This method will format the contained data for the
///processor
///</summary>
void Canberra::DataTypes::Spectroscopy::SCAbuffer::HandleEndianness()
{
	SWAPNBYTES(&(_Header.BytesPerSample), sizeof(_Header.BytesPerSample));
	SWAPNBYTES(&(_Header.Flags), sizeof(_Header.Flags));
	SWAPNBYTES(&(_Header.StartTime), sizeof(_Header.StartTime));
}
///<summary>
///This method will serialize this instance into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::SCAbuffer::SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset) {

	ULONG Size=GetDataSize();
	if (Offset < 0) {
		ULONG Stat = (ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(L"SCAbuffer", L"Serialize", L"Invalid Offset size", Stat);
		throw Stat;
	}
	if ((Offset+Size) > Str.GetSize()) Str.Resize(Offset+Size);
	LONG Pos=Offset;
	LONG NumCpy=0;
	HandleEndianness();
	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_Header, NumCpy=sizeof(_Header)); Pos+=NumCpy;
	ULONG NumEntries=_Data.GetNumberOfItems();
	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &NumEntries, NumCpy=sizeof(NumEntries)); Pos+=NumCpy;
	for(ULONG i=0; i<_Data.GetNumberOfItems(); i++) {Pos+=_Data.GetItem(i).SerializeData(Str, MD, Pos);}
	HandleEndianness();
	return Pos-Offset;
}

///<summary>
///This method will deserialize this instance from the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::SCAbuffer::DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset) {
	LONG Pos=Offset;
	LONG NumCpy=0;
	_Data.Clear();
	memcpy(&_Header, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_Header)); Pos+=NumCpy;
	HandleEndianness();
	ULONG NumEntries=0;
	memcpy(&NumEntries, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(NumEntries)); Pos+=NumCpy;
	SWAPNBYTES(&NumEntries, sizeof(&NumEntries));
	for(ULONG i=0; i<NumEntries; i++) {
		Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry Entry;
		Pos+=Entry.DeserializeData(Str, MD, Pos);
		_Data.Add(Entry);
	}
	return Pos-Offset;
}

///<summary>
///This method will clear all defs
///</summary>
void Canberra::DataTypes::Spectroscopy::SCAbuffer::Clear() { memset(&_Header, 0, sizeof(_Header)); _Data.Clear();}

///<summary>
///This method will get a reference to the entry
///</summary>
///<param name="v">The index</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The value</returns>
Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferEntry &Canberra::DataTypes::Spectroscopy::SCAbuffer::GetEntry(ULONG i) {return _Data.GetItem(i);}

///<summary>
///This method will return the # of entries
///</summary>
///<returns>The number of data elements in the buffer</returns>
ULONG Canberra::DataTypes::Spectroscopy::SCAbuffer::GetNumberOfEntries() {return _Data.GetNumberOfItems();}

///<summary>
///This method will get a reference to the buffer header
///</summary>
///<returns>The value</returns>
Canberra::DataTypes::Spectroscopy::SCAbuffer::BufferHeader &Canberra::DataTypes::Spectroscopy::SCAbuffer::GetHeader() {return _Header;}
