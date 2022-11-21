#include "MessageHeader.h"
#include "Crc32.h"
#include "DebugMacros.h"
#include <memory.h>
#ifdef WIN32
#include <wincrypt.h>
#endif

///<summary>
///An instance of this class is used to define a message header
///</summary>
///<param name="PayloadSize">The number of bytes for the payload</param>
///<param name="Version">The version of the messaging system</param>
///<param name="Seq">The seq number</param>
Canberra::DataTypes::Serialization::MessageHeader::MessageHeader(ULONG PayloadSize, ULONG Version, ULONG Seq){

	memset(&_Data, 0, sizeof(_Data));
	_Data.Size = sizeof(_Data)+PayloadSize;
	_Data.Version = Version;
	_Data.SequenceNumber = Seq;
}
Canberra::DataTypes::Serialization::MessageHeader::~MessageHeader(void) {}


///<summary>
///This method will serialize this instance into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Serialization::MessageHeader::Serialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	ULONG Size=sizeof(_Data);
	if (Offset < 0) {
		ULONG Stat = (ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(L"MessageHeader", L"Serialize", L"Invalid Offset size", Stat);
		throw Stat;
	}
	if ((Offset+Size) > Str.GetSize()) Str.Resize(Offset+Size);

	//Now all of the data is in the stream compute the check sum
	//Assumption is that the checksum is 16 bytes into the message stream
	_Data.Size = Str.GetSize();
	_Data.CheckSum=Canberra::Utility::Core::Crc32::Compute(Str, 4*sizeof(ULONG));

	LONG Pos=Offset;
	LONG NumCpy=0;
	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_Data, NumCpy=sizeof(_Data)); Pos+=NumCpy;

	return Pos-Offset;
}


///<summary>
///This method will deserialize info from the byte stream into this instance
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the read in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
ULONG Canberra::DataTypes::Serialization::MessageHeader::Deserialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	ULONG Size=sizeof(_Data);
	if (((Offset+Size) > Str.GetSize()) || (Offset < 0)) {
		ULONG Stat = (ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(L"MessageHeader", L"Deserialize", L"Invalid Offset size", Stat);
		throw Stat;
	}

	LONG Pos=Offset;
	LONG NumCpy=0;
	memcpy(&_Data, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy+=sizeof(_Data)); Pos+=NumCpy;

	//Test the check sum against the one in the header
	ULONG ComputedCheckSum=Canberra::Utility::Core::Crc32::Compute(Str, 4*sizeof(ULONG));
	if (ComputedCheckSum != _Data.CheckSum) {
		ULONG Stat = (ULONG) GEN_CHECKSUM_FAILURE;
		DEBUG_ERROR_MESSAGE(L"MessageHeader", L"Deserialize", Canberra::Utility::Core::String(L"Invalid checksum. ") +
							Canberra::Utility::Core::String(L" Header checksum value is: ") +
							Canberra::Utility::Core::String(_Data.CheckSum) +
							Canberra::Utility::Core::String(L" Computed checksum value is: ") +
							Canberra::Utility::Core::String(ComputedCheckSum), Stat);
		throw Stat;
	}

	return Pos-Offset;
}
//The size of the message (header+payload)
ULONG Canberra::DataTypes::Serialization::MessageHeader::GetSize() {return _Data.Size;}

//The size of the header only
ULONG Canberra::DataTypes::Serialization::MessageHeader::GetHeaderSize() {return sizeof(_Data);}

//The version of the message
ULONG Canberra::DataTypes::Serialization::MessageHeader::GetVersion() {return _Data.Version;}

//The sequence number
ULONG Canberra::DataTypes::Serialization::MessageHeader::GetSequenceNumber() {return _Data.SequenceNumber;}

//The checksum
ULONG Canberra::DataTypes::Serialization::MessageHeader::GetCheckSum() {return _Data.CheckSum;}
