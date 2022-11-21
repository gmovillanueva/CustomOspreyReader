#include "DigitalSignature.h"
#include "ApplicationSerializer.h"
#include "SerializableType.h"
#include "DebugMacros.h"
#include "endianUtils.h"
#include <memory.h>
#ifdef WIN32
#include <wincrypt.h>
#endif

///<summary>
///An instance of this class is used to create and verify a digital
///signature
///</summary>
Canberra::DataTypes::Serialization::DigitalSignature::DigitalSignature(void) :
#if (defined(WIN32) && defined(SECURE))
_HashAlgorithm(CALG_SHA1), _Provider(NULL), _UserKey(NULL),
#endif
_ReverseBytes(true), _IncludeRandomData(true), _EnableSigning(true)
{
	PutEnableSigning(_EnableSigning);
}

///<summary>
///An instance of this class is used to create and verify a digital
///signature
///</summary>
///<param name="EnableSigning">Enable/disable signing data</param>
Canberra::DataTypes::Serialization::DigitalSignature::DigitalSignature(bool EnableSigning) :
	_ReverseBytes(true), _IncludeRandomData(true),
#if (defined(WIN32) && defined(SECURE))
	_HashAlgorithm(CALG_SHA1), _Provider(NULL), _UserKey(NULL),
#endif
	_EnableSigning(EnableSigning)
{
	PutEnableSigning(EnableSigning);
}
Canberra::DataTypes::Serialization::DigitalSignature::~DigitalSignature(void) {
	Release();
}


///<summary>
///An releases the security provider
///</summary>
///<exceptions cref="ULONG">Win32 error code </exceptions>
void Canberra::DataTypes::Serialization::DigitalSignature::Release()
{
#if (defined(WIN32) && defined(SECURE))
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//ORDER OF DESTRUCTION BELOW
	//IS EXTREMELY IMPORTANT ELSE
	//WILL LEAK
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!

	if (NULL != _UserKey) {
		CryptDestroyKey(_UserKey);
		_UserKey=NULL;
	}

	if (NULL != _Provider) {
		CryptReleaseContext(_Provider,0);
		_Provider=NULL;
	}
#endif
}

///<summary>
///An initializes the security provider
///</summary>
///<exceptions cref="ULONG">Win32 error code </exceptions>
void Canberra::DataTypes::Serialization::DigitalSignature::Allocate()
{
	Release();
#if (defined(WIN32) && defined(SECURE))
	wchar_t *Container=NULL;
	if (!CryptAcquireContext(&_Provider, Container, NULL, PROV_RSA_FULL, 0)) {
		ULONG Stat = (ULONG) GetLastError();
		if (NTE_BAD_KEYSET != Stat) {
			DEBUG_ERROR_MESSAGE(L"DigitalSignature", L"Sign", L"Error CryptAcquireContext. ", Stat);
			throw Stat;
		}
		if(!CryptAcquireContext(&_Provider, Container, NULL, PROV_RSA_FULL, CRYPT_DELETEKEYSET)) {
			Stat = (ULONG) GetLastError();
			if (NTE_BAD_KEYSET != Stat) {
				DEBUG_ERROR_MESSAGE(L"DigitalSignature", L"Sign", L"Error CryptAcquireContext. with delete option ", Stat);
			}
		}
		if(!CryptAcquireContext(&_Provider, Container, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET)) {
			Stat = (ULONG) GetLastError();
			DEBUG_ERROR_MESSAGE(L"DigitalSignature", L"Sign", L"Error CryptAcquireContext. with create new option", Stat);
			throw Stat;
		}
	}

	if(!CryptGetUserKey(_Provider, AT_SIGNATURE, &_UserKey)) {
		ULONG Stat = (ULONG) GetLastError();
		if (NTE_NO_KEY != Stat) {
			DEBUG_ERROR_MESSAGE(L"DigitalSignature", L"Sign", L"Error CryptGetUserKey. ", Stat);
			throw Stat;
		}
		if (!CryptGenKey(_Provider, AT_SIGNATURE, 0, &_UserKey)) {
			DEBUG_ERROR_MESSAGE(L"DigitalSignature", L"Sign", L"Error CryptGenKey. ", Stat);
			throw Stat;
		}
	}
#endif
}


///<summary>
///This method will compute a digital signature for the stream
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The offset to start computing the signature at in the stream</param>
///<returns>0 == Success</returns>
ULONG Canberra::DataTypes::Serialization::DigitalSignature::Sign(const Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	//No signing so exit
	if (!_EnableSigning) return 0;

	ULONG Stat = 0;
#if (defined(WIN32) && defined(SECURE))
	Canberra::Utility::Core::ByteStream Data;
	HCRYPTHASH Hash=NULL;
	DWORD Size=0;

	if (_IncludeRandomData) {
		LARGE_INTEGER Random = {0};
		_RandomData.Allocate(sizeof(Random));
		if (!QueryPerformanceCounter(&Random)) {
			ULONG Stat=GetLastError();
			DEBUG_ERROR_MESSAGE(L"DigitalSignature", L"Sign", L"Error QueryPerformanceCounter. ", Stat);
			Random.QuadPart = GetTickCount();
		}
		srand(5467);
		Random.QuadPart += rand();
		memcpy(const_cast<char *>(_RandomData.GetBuffer()), &Random, _RandomData.GetSize());
		Data = _RandomData;
		Data.Copy(const_cast<Canberra::Utility::Core::ByteStream &>(Str), (DWORD) Offset, _RandomData.GetSize());
	}
	else {
		Data.Copy(const_cast<Canberra::Utility::Core::ByteStream &>(Str), (DWORD) Offset, 0);
	}

	_Key.Clear();
	_Signature.Clear();

	//Create the key to send with the data
	if(!CryptExportKey(_UserKey, NULL, PUBLICKEYBLOB, 0, NULL, &Size)) {
		Stat = (ULONG) GetLastError();
		DEBUG_ERROR_MESSAGE(L"DigitalSignature", L"Sign", L"Error CryptExportKey. ", Stat);
		goto EXIT;
	}
	_Key.Allocate(Size);
	if(!CryptExportKey(_UserKey, NULL, PUBLICKEYBLOB, 0, reinterpret_cast<BYTE *>(const_cast<char *>(_Key.GetBuffer())), &Size)) {
		Stat = (ULONG) GetLastError();
		DEBUG_ERROR_MESSAGE(L"DigitalSignature", L"Sign", L"Error CryptExportKey. ", Stat);
		goto EXIT;

	}

	if (!CryptCreateHash((HCRYPTPROV) _Provider, _HashAlgorithm, NULL, 0, &Hash)) {
		Stat = (ULONG) GetLastError();
		DEBUG_ERROR_MESSAGE(L"DigitalSignature", L"Sign", L"Error CryptCreateHash. ", Stat);
		goto EXIT;
	}

	if (!CryptHashData(Hash, reinterpret_cast<const BYTE *>(Data.GetBuffer()), Data.GetSize(), 0)) {
		Stat = (ULONG) GetLastError();
		DEBUG_ERROR_MESSAGE(L"DigitalSignature", L"Sign", L"Error CryptHashData. ", Stat);
		goto EXIT;
	}

	//Determine signature size
	Size = 0;
	if(!CryptSignHash(Hash, AT_SIGNATURE, NULL, 0, NULL, &Size)) {
		Stat = (ULONG) GetLastError();
		DEBUG_ERROR_MESSAGE(L"DigitalSignature", L"Sign", L"Error CryptSignHash. ", Stat);
		goto EXIT;
	}
	_Signature.Clear();
	_Signature.Allocate(Size);

	if(!CryptSignHash(Hash, AT_SIGNATURE, NULL, 0, reinterpret_cast<BYTE *>(const_cast<char *>(_Signature.GetBuffer())), &Size)) {
		Stat = (ULONG) GetLastError();
		DEBUG_ERROR_MESSAGE(L"DigitalSignature", L"Sign", L"Error CryptSignHash. ", Stat);
		goto EXIT;
	}

EXIT:
	if (NULL != Hash) {CryptDestroyHash(Hash); Hash=NULL;}
#endif
	return Stat;
}

///<summary>
///This method will verify the data matches this digital
///signature information.  Note: The random data is included
///int the verification.
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The offset to start verification of the signature in the stream</param>
///<returns>0 == Success</returns>
ULONG Canberra::DataTypes::Serialization::DigitalSignature::Verify(const Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	Canberra::Utility::Core::ByteStream Data(_RandomData);
	ULONG Stat=0;

	//No signature data so exit
	if ((0 == _Key.GetSize()) || (0 == _Signature.GetSize())) goto EXIT;

	if (_IncludeRandomData) {
		Data.Copy(const_cast<Canberra::Utility::Core::ByteStream &>(Str), (DWORD) Offset, _RandomData.GetSize());
	}
	else {
		Data.Copy(const_cast<Canberra::Utility::Core::ByteStream &>(Str), (DWORD) Offset, 0);
	}

#if (defined(WIN32) && defined(SECURE))
	HCRYPTKEY Key=NULL;
	HCRYPTHASH Hash=NULL;

	//Import the key to send with the data
	if(!CryptImportKey((HCRYPTPROV) _Provider, reinterpret_cast<const BYTE *>(_Key.GetBuffer()), _Key.GetSize(), 0, 0, &Key)) {
		Stat = (ULONG) GetLastError();
		DEBUG_ERROR_MESSAGE(L"DigitalSignature", L"Verify", L"Error CryptImportKey. ", Stat);
		goto EXIT;
	}

	if (!CryptCreateHash((HCRYPTPROV) _Provider, _HashAlgorithm, NULL, 0, &Hash)) {
		Stat = (ULONG) GetLastError();
		DEBUG_ERROR_MESSAGE(L"DigitalSignature", L"Verify", L"Error CryptCreateHash. ", Stat);
		goto EXIT;
	}

	if (!CryptHashData(Hash, reinterpret_cast<const BYTE *>(Data.GetBuffer()), Data.GetSize(), 0)) {
		Stat = (ULONG) GetLastError();
		DEBUG_ERROR_MESSAGE(L"DigitalSignature", L"Verify", L"Error CryptHashData. ", Stat);
		goto EXIT;
	}

	if (!CryptVerifySignature(Hash, reinterpret_cast<const BYTE *>(_Signature.GetBuffer()), _Signature.GetSize(), Key, NULL, 0)) {
		Stat = (ULONG) GetLastError();
		DEBUG_ERROR_MESSAGE(L"DigitalSignature", L"Verify", L"Error CryptVerifySignature. ", Stat);
		goto EXIT;
	}

EXIT:

	if (NULL != Hash) {CryptDestroyHash(Hash); Hash=NULL;}
	if (NULL != Key) {CryptDestroyKey(Key); Key=NULL;}
#else
EXIT:
	return Stat;
#endif

}

///<summary>
///This method will serialize this instance into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Serialization::DigitalSignature::Serialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	ApplicationSerializer Serializer;
	ULONG Size=GetSize();
	if (Offset < 0) {
		ULONG Stat = (ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(L"DigitalSignature", L"Serialize", L"Invalid Offset size", Stat);
		throw Stat;
	}
	if ((Offset+Size) > Str.GetSize()) Str.Resize(Offset+Size);

	LONG Pos=Offset;
	LONG NumCpy=0;

	//No signing so write zeros and exit
	if (!_EnableSigning) {
		Size = 0;
		NumCpy=Serializer.Serialize(Canberra::Serialization::Ulong(Size), Str, Pos); Pos+=NumCpy;
		NumCpy=Serializer.Serialize(Canberra::Serialization::Ulong(Size), Str, Pos); Pos+=NumCpy;
		NumCpy=Serializer.Serialize(Canberra::Serialization::Ulong(Size), Str, Pos); Pos+=NumCpy;
		return Pos-Offset;
	}

	//Write the public key
	Size = _Key.GetSize();
	NumCpy=Serializer.Serialize(Canberra::Serialization::Ulong(Size), Str, Pos); Pos+=NumCpy;
	memcpy(const_cast<char *>(Str.GetBuffer())+Pos, const_cast<char *>(_Key.GetBuffer()), NumCpy=_Key.GetSize()); Pos+=NumCpy;

	//Write the digital signature
	std::auto_ptr<char> Buf(new char[_Signature.GetSize()]);
	if (_ReverseBytes) {
		for(int i=_Signature.GetSize()-1, j=0; i>-1; i--, j++) {
			Buf.get()[j] = _Signature.GetBuffer()[i];
		}
	}
	else {memcpy(Buf.get(), const_cast<char *>(_Signature.GetBuffer()), _Signature.GetSize());}

	Size = _Signature.GetSize();
	NumCpy=Serializer.Serialize(Canberra::Serialization::Ulong(Size), Str, Pos); Pos+=NumCpy;
	memcpy(const_cast<char *>(Str.GetBuffer())+Pos, Buf.get(), NumCpy=_Signature.GetSize()); Pos+=NumCpy;

	//Write the random data
	if (_IncludeRandomData) {
		Size = _RandomData.GetSize();
		NumCpy=Serializer.Serialize(Canberra::Serialization::Ulong(Size), Str, Pos); Pos+=NumCpy;
		memcpy(const_cast<char *>(Str.GetBuffer())+Pos, const_cast<char *>(_RandomData.GetBuffer()), NumCpy=_RandomData.GetSize()); Pos+=NumCpy;
	}

	//Write the hash alg id
	//memcpy(const_cast<char *>(Str.GetBuffer())+Pos, &_HashAlgorithm, NumCpy=sizeof(_HashAlgorithm)); Pos+=NumCpy;
	return Pos-Offset;
}


///<summary>
///This method will deserialize info from the byte stream into this instance
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the read in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
ULONG Canberra::DataTypes::Serialization::DigitalSignature::Deserialize(Canberra::Utility::Core::ByteStream &Str, const  LONG Offset)
{
	ApplicationSerializer Serializer;

	//Clear the old values
	_Key.Clear();
	_Signature.Clear();
	LONG Pos=Offset;
	LONG NumCpy=0;
	LONG Size=0;

	//Get the public key
	memcpy(&Size, Str.GetBuffer()+Pos, NumCpy=sizeof(ULONG)); Pos+=NumCpy;
	SWAPNBYTES(&Size, sizeof(Size));
	if (Size > 0) {
		_Key.Allocate(Size);
		memcpy(const_cast<char *>(_Key.GetBuffer()), Str.GetBuffer()+Pos, NumCpy=_Key.GetSize()); Pos+=NumCpy;
	}

	//Get the digital signature
	memcpy(&Size, Str.GetBuffer()+Pos, NumCpy=sizeof(ULONG)); Pos+=NumCpy;
	SWAPNBYTES(&Size, sizeof(Size));
	if (Size > 0) {
		std::auto_ptr<char> Buf(new char[Size]);
		_Signature.Allocate(Size);
		memcpy(Buf.get(), Str.GetBuffer()+Pos, NumCpy=Size); Pos+=NumCpy;

		//See if we need to reverse order
		if (_ReverseBytes) {
			for(int i=Size-1, j=0; i>-1; i--, j++) {
				const_cast<char *>(_Signature.GetBuffer())[j] = Buf.get()[i];
			}
		}
		else {memcpy(const_cast<char *>(_Signature.GetBuffer()), Buf.get(), _Signature.GetSize());}

	}

	//Get the random data
	memcpy(&Size, Str.GetBuffer()+Pos, NumCpy=sizeof(ULONG)); Pos+=NumCpy;
	SWAPNBYTES(&Size, sizeof(Size));
	if (Size > 0) {
		_IncludeRandomData = true;
		_RandomData.Allocate(Size);
		memcpy(const_cast<char *>(_RandomData.GetBuffer()), Str.GetBuffer()+Pos, NumCpy=_RandomData.GetSize()); Pos+=NumCpy;
	}
	else _IncludeRandomData=false;

	//Get the hash alg id
	//memcpy(&_HashAlgorithm, Str.GetBuffer()+Pos, NumCpy=sizeof(_HashAlgorithm)); Pos+=NumCpy;
	return Pos-Offset;
}


///<summary>
///This method will the number of bytes used to serialize
///this signature
///</summary>
///<returns>The number of bytes</returns>
ULONG Canberra::DataTypes::Serialization::DigitalSignature::GetSize() {
	if (_EnableSigning) {
		ULONG Val = 3*sizeof(ULONG) + _Signature.GetSize() + _Key.GetSize() + ((_IncludeRandomData) ? _RandomData.GetSize() : 0);
		return Val;
	}
	else {
		return 3*sizeof(ULONG);
	}
}

///<summary>
///Accessor to enable signing
///</summary>
///<exceptions cref="ULONG">Win32 error code </exceptions>
bool Canberra::DataTypes::Serialization::DigitalSignature::GetEnableSigning() { return _EnableSigning;}
void Canberra::DataTypes::Serialization::DigitalSignature::PutEnableSigning(bool v) { _EnableSigning=v; if (v) Allocate();}

//Accessor to the reverse bytes state.  This is req'd for .NET and Java clients
bool Canberra::DataTypes::Serialization::DigitalSignature::GetReverseByte() { return _ReverseBytes;}
void Canberra::DataTypes::Serialization::DigitalSignature::PutReverseByte(bool v) { _ReverseBytes=v;}

//Accessor to state indicating whether random data is added to the stream to improve security
bool Canberra::DataTypes::Serialization::DigitalSignature::GetIncludeRandomData() { return _IncludeRandomData;}
void Canberra::DataTypes::Serialization::DigitalSignature::PutIncludeRandomData(bool v) { _IncludeRandomData=v;}

#if (defined(WIN32) && defined(SECURE))
//The hash alg identifier
ULONG Canberra::DataTypes::Serialization::DigitalSignature::GetHashAlorithmId() {return _HashAlgorithm;}
void Canberra::DataTypes::Serialization::DigitalSignature::PutHashAlorithmId(ULONG v) {_HashAlgorithm=v;}
#endif

