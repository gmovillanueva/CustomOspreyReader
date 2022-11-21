#include "Crc32.h"
#include "DebugMacros.h"

bool Canberra::Utility::Core::Crc32::_TableComputed = false;			//State indicating whether the table has been computed yet
ULONG Canberra::Utility::Core::Crc32::_Table[256] = {0};				//The table


///<summary>
///This is the constructor for this instance.  It will
///compute the table on the very first object instantiation
///</summary>
Canberra::Utility::Core::Crc32::Crc32(void) : _Crc(0) {MakeTable();}
Canberra::Utility::Core::Crc32::~Crc32(void) {}


///<summary>
///This method will compute the table of CRC values
///</summary>
void Canberra::Utility::Core::Crc32::MakeTable(void) {
	if (_TableComputed) return;

	unsigned long c;
    int n,k;

    for(n=0; n<256; n++) {
		c=(unsigned long)n;
        for(k=0; k<8; k++) {
			if(c&1) c=0xedb88320L^(c>>1);
			else c=c>>1;
		}
		_Table[n]=c;
	}
	_TableComputed=true;
}


///<summary>
///This method will return the computed CRC value
///that is based on the input stream.  Note: The
///previous CRC value included into the computed
///value
///</summary>
///<param name="BS">The byte stream to compute the CRC of </param>
///<param name="Offset">The offset in the stream to start the CRC calc </param>
///<exception cref="ULONG">Error code</exception>
///<returns>The computed CRC value</returns>
ULONG Canberra::Utility::Core::Crc32::Update(const Canberra::Utility::Core::ByteStream &BS, LONG Offset) {
	Canberra::Utility::Core::ByteStream &Str = const_cast<Canberra::Utility::Core::ByteStream &>(BS);
	if ((Offset < 0) || ((DWORD) Offset > Str.GetSize()))
	{
		DEBUG_ERROR_MESSAGE(L"Crc32", L"Update", L"Invalid Offset argument", E_INVALIDARG);
		throw (ULONG) E_INVALIDARG;
	}
	return Update(Str.GetBuffer() + Offset, Str.GetSize()-Offset);
}


///<summary>
///This method will return the computed CRC value
///that is based on the input stream.  Note: The
///previous CRC value included into the computed
///value
///</summary>
///<param name="BS">The byte stream to compute the CRC of </param>
///<param name="Offset">The offset in the stream to start the CRC calc </param>
///<exception cref="ULONG">Error code</exception>
///<returns>The computed CRC value</returns>
ULONG Canberra::Utility::Core::Crc32::Compute(const Canberra::Utility::Core::ByteStream &BS, LONG Offset) {
	Crc32 Crc;
	return Crc.Update(BS, Offset);
}



///<summary>
///This method will return the computed CRC value
///that is based on the input stream.  Note: The
///previous CRC value included into the computed
///value
///</summary>
///<param name="Buf">The byte stream to compute the CRC of </param>
///<param name="len">The number of bytes in the stream </param>
///<returns>The computed CRC value</returns>
ULONG Canberra::Utility::Core::Crc32::Update(const char * const Buf, LONG len) {
	ULONG c=_Crc^0xffffffffL;
	unsigned char *buf = const_cast<unsigned char *>(reinterpret_cast<const unsigned char * const>(Buf));
	ULONG index=0;

	for(LONG n=0; n<len; n++) {
		index=(c^buf[n])&0xff;
		if (index > 255) {
			DEBUG_TYPED_MESSAGE(L"Crc32", L"Update", L"Computed index value exceeds table dimensions.  Defaulting to max. index", Canberra::Utility::Core::Diagnostics::Warning);
			index = 255;
		}
		c=_Table[index]^(c>>8);
	}

	return _Crc=c^0xffffffffL;
}
///<summary>
///This method will clear the CRC value
///</summary>
void Canberra::Utility::Core::Crc32::Reset() { _Crc = 0;}
