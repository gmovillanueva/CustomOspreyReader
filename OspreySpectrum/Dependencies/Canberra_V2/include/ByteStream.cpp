#include "ByteStream.h"
#include "DebugMacros.h"
#include <math.h>
#include <memory.h>
using namespace std;

Canberra::Utility::Core::ByteStream::ByteStream(void) : _Stream(NULL), _Size(0) {Clear();}
Canberra::Utility::Core::ByteStream::ByteStream(const char * const V, DWORD Size): _Stream(NULL), _Size(0){Copy(V, Size);}
Canberra::Utility::Core::ByteStream::ByteStream(const ByteStream &V): _Stream(NULL), _Size(0) {Copy(V._Stream, V._Size);}
Canberra::Utility::Core::ByteStream::ByteStream(DWORD Size): _Stream(NULL), _Size(0) {Allocate(Size);}
Canberra::Utility::Core::ByteStream::~ByteStream(void) {Clear();}

///<summary>
///This method will clear the byte stream
///</summary>
void Canberra::Utility::Core::ByteStream::Clear(void)
{
	if (_Stream) {delete [] _Stream; _Stream=NULL;}
	_Size=0;
}

///<summary>
///This method will allocate and initialize the
///byte stream
///</summary>
///<param name="Size">The number of bytes</param>
///<returns>The number allocated</returns>
///<exception cref="ULONG">Error code</exception>
DWORD Canberra::Utility::Core::ByteStream::Allocate(DWORD Size)
{
	if (_Size == Size) {
		memset(_Stream, 0, Size);
		return Size;
	}

	Clear();
	_Stream = new char[Size];
	if (NULL == _Stream) {
		DEBUG_ERROR_MESSAGE(L"ByteStream", L"Allocate", L"Failed to allocate memory", E_OUTOFMEMORY);
		throw (ULONG) E_OUTOFMEMORY;
	}
	_Size=Size;
	memset(_Stream, 0, Size);
	return Size;
}


///<summary>
///This method will reallocate memory to meet
///the new specified size while preserving contents
///</summary>
///<param name="Size">The number of bytes</param>
///<returns>The number allocated</returns>
///<exception cref="ULONG">Error code</exception>
DWORD Canberra::Utility::Core::ByteStream::Resize(DWORD Rsize)
{
	if (_Stream) {
		auto_ptr<char> Tmp(new char[Rsize]);
		if (NULL == Tmp.get()) {
			DEBUG_ERROR_MESSAGE(L"ByteStream", L"Allocate", L"Failed to allocate memory", E_OUTOFMEMORY);
			throw (ULONG) E_OUTOFMEMORY;
		}
		memcpy(Tmp.get(), _Stream, min(Rsize, _Size));
		return Copy(Tmp.get(), Rsize);
	}
	else return Allocate(Rsize);
}


///<summary>
///This method will copy a byte stream
///</summary>
///<param name="Src">The source stream</param>
///<returns>The number allocated</returns>
///<exception cref="ULONG">Error code</exception>
DWORD Canberra::Utility::Core::ByteStream::Copy(ByteStream &Src)
{
	return Copy(Src.GetBuffer(), Src.GetSize());
}

///<summary>
///This method will copy a byte stream
///</summary>
///<param name="Size">The number of bytes</param>
///<returns>The number allocated</returns>
///<exception cref="ULONG">Error code</exception>
DWORD Canberra::Utility::Core::ByteStream::Copy(const char *const V, DWORD Sz)
{
	if ((NULL == V) && (Sz > 0)) {
		DEBUG_ERROR_MESSAGE(_T("ByteStream"), _T("Copy"), _T("NULL == V"), E_INVALIDARG);
		throw (ULONG) E_INVALIDARG;
	}
	if ((0 == Sz) && (NULL != V)) {
		DEBUG_ERROR_MESSAGE(_T("ByteStream"), _T("Copy"), _T("0 == Sz"), E_INVALIDARG);
		throw (ULONG) E_INVALIDARG;
	}

	//Let this thru
	if ((0 == Sz) && (NULL == V)) {
		Clear();
		return Sz;
	}


	Allocate(Sz);
	memcpy(_Stream, V, Sz);
	return Sz;
}

///<summary>
///This method will copy a source byte stream into
///this instance while preserving this ones contents.
///A value of 0 for Num implies copy all bytes
///</summary>
///<param name="Src">The source stream</param>
///<param name="SrcStart">Byte position to start the copy in the source stream</param>
///<param name="DestStart">Byte position to start the copy in the dest stream</param>
///<returns>The number copied</returns>
///<exception cref="ULONG">Error code</exception>
DWORD Canberra::Utility::Core::ByteStream::Copy(ByteStream &Src, DWORD SrcStart, DWORD DestStart, DWORD Num)
{
	//Check the source stream
	if ((SrcStart+Num) > Src.GetSize()) {
		DEBUG_ERROR_MESSAGE(_T("ByteStream"), _T("Copy"), _T("SrcStart+Num > Src.GetSize()"), E_INVALIDARG);
		throw (ULONG) E_INVALIDARG;
	}

	//Determine number to copy
	DWORD Num2Copy=Num < 1 ? Src.GetSize()-SrcStart : Num;

	//Resize preserving contents
	if (DestStart+Num2Copy > GetSize()) Resize(DestStart+Num2Copy);

	memcpy(_Stream+DestStart, const_cast<char *>(Src.GetBuffer())+SrcStart, Num2Copy);
	return Num2Copy;
}

///<summary>
///This method will return the pointer to the byte stream
///</summary>
///<returns>The pointer to the byte stream</returns>
const char *const Canberra::Utility::Core::ByteStream::GetBuffer() const {return _Stream;}


///<summary>
///This method will return the number of bytes
///allocated for the stream
///</summary>
///<returns>The number of bytes</returns>
DWORD Canberra::Utility::Core::ByteStream::GetSize() const {return _Size;}

///<summary>
///This method will return the pointer to the byte stream
///</summary>
///<returns>The pointer to the byte stream</returns>
Canberra::Utility::Core::ByteStream::operator const char* const() {return GetBuffer();}

///<summary>
///This method will perform a deep copy
///</summary>
///<param name="Rhs">Byte stream to copy</param>
///<returns>This instance</returns>
Canberra::Utility::Core::ByteStream& Canberra::Utility::Core::ByteStream::operator=( const Canberra::Utility::Core::ByteStream& Rhs) {
	if (this == &Rhs) return *this;
	Copy(Rhs._Stream, Rhs._Size);
	return *this;
}
