#include "UtilityString.h"
#include "UtilityTime.h"
#include "DebugMacros.h"
#include <stdlib.h>
#include <memory>
#include <string>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <math.h>
#ifdef WIN32
#ifdef WINCE
#define swprintf _snwprintf
#else
#define swprintf _snwprintf_s
#endif
#endif

#define SIZEOF_WBUF(a) (sizeof(a)/sizeof(wchar_t)-1)

///<summary>
///This is a utility class used below for parsing
///strings
///</summary>
class CStringReplacer
{
	CStringReplacer(const CStringReplacer&);
	CStringReplacer& operator=(const CStringReplacer&);
private:
	const std::wstring _Old;
	const std::wstring _New;

public:
 CStringReplacer(const wchar_t* o,const wchar_t* n) : _Old(o), _New(n) {}

 void Replace(std::wstring& Target) const {
	 unsigned int p=0;

	 while (std::wstring::npos != (p=Target.find(_Old))) {
		Target.replace(p,_Old.size(),_New);
	 }
 }
};

///<summary>
///An instance of this class is used contain a string
///</summary>
Canberra::Utility::Core::String::String() : _Value(NULL), _ValueSZ(NULL) {}

///<summary>
///An instance of this class is used contain a string
///</summary>
///<param name="S">The string to assign</param>
Canberra::Utility::Core::String::String(const wchar_t * const S1, const wchar_t* const S2) :
	_Value(NULL), _ValueSZ(NULL)
{
	CopyFrom(S1, S2);
}


///<summary>
///An instance of this class is used contain a string
///</summary>
///<param name="S">The string to assign</param>
Canberra::Utility::Core::String::String(const char * const S)  : _Value(NULL), _ValueSZ(NULL) {CopyFrom(S);}


///<summary>
///An instance of this class is used contain a string
///</summary>
///<param name="V">The value to format into the string</param>
///<param name="Fmt">The format specifier</param>
void Canberra::Utility::Core::String::Initialize(long V, String Fmt) {
	const wchar_t * const FormatSpecifier = (Fmt.GetLength()) ?  (const wchar_t *) Fmt : L"%d";
	wchar_t Buf[4096]={0};
	swprintf(Buf, SIZEOF_WBUF(Buf), FormatSpecifier, V);
	CopyFrom(Buf);
}

///<summary>
///An instance of this class is used contain a string
///</summary>
///<param name="V">The value to format into the string</param>
///<param name="Fmt">The format specifier</param>
void Canberra::Utility::Core::String::Initialize(unsigned long V, String Fmt) {
	const wchar_t * const FormatSpecifier = (Fmt.GetLength()) ?  (const wchar_t *) Fmt : L"%d";
	wchar_t Buf[4096]={0};
	swprintf(Buf, SIZEOF_WBUF(Buf), FormatSpecifier, V);
	CopyFrom(Buf);
}


///<summary>
///An instance of this class is used contain a string
///</summary>
///<param name="V">The value to format into the string</param>
///<param name="Fmt">The format specifier</param>
void Canberra::Utility::Core::String::Initialize(int V, String Fmt) {
	const wchar_t * const FormatSpecifier = (Fmt.GetLength()) ?  (const wchar_t *) Fmt : L"%d";
	wchar_t Buf[4096]={0};
	swprintf(Buf, SIZEOF_WBUF(Buf), FormatSpecifier, V);
	CopyFrom(Buf);
}


///<summary>
///An instance of this class is used contain a string
///</summary>
///<param name="V">The value to format into the string</param>
///<param name="Fmt">The format specifier</param>
void Canberra::Utility::Core::String::Initialize(unsigned int V, String Fmt) {
	const wchar_t * const FormatSpecifier = (Fmt.GetLength()) ?  (const wchar_t *) Fmt : L"%d";
	wchar_t Buf[4096]={0};
	swprintf(Buf, SIZEOF_WBUF(Buf), FormatSpecifier, V);
	CopyFrom(Buf);
}


///<summary>
///An instance of this class is used contain a string
///</summary>
///<param name="V">The value to format into the string</param>
///<param name="Fmt">The format specifier</param>
void Canberra::Utility::Core::String::Initialize(short V, String Fmt) {
	const wchar_t * const FormatSpecifier = (Fmt.GetLength()) ?  (const wchar_t *) Fmt : L"%d";
	wchar_t Buf[4096]={0};
	swprintf(Buf, SIZEOF_WBUF(Buf), FormatSpecifier, V);
	CopyFrom(Buf);
}


///<summary>
///An instance of this class is used contain a string
///</summary>
///<param name="V">The value to format into the string</param>
///<param name="Fmt">The format specifier</param>
void Canberra::Utility::Core::String::Initialize(unsigned short V, String Fmt) {
	const wchar_t * const FormatSpecifier = (Fmt.GetLength()) ?  (const wchar_t *) Fmt : L"%d";
	wchar_t Buf[4096]={0};
	swprintf(Buf, SIZEOF_WBUF(Buf), FormatSpecifier, V);
	CopyFrom(Buf);
}


///<summary>
///An instance of this class is used contain a string
///</summary>
///<param name="V">The value to format into the string</param>
///<param name="Fmt">The format specifier</param>
void Canberra::Utility::Core::String::Initialize(double V, String Fmt) {
	const wchar_t * const FormatSpecifier = (Fmt.GetLength()) ?  (const wchar_t *) Fmt : L"%f";
	wchar_t Buf[4096]={0};
	swprintf(Buf, SIZEOF_WBUF(Buf), FormatSpecifier, V);
	CopyFrom(Buf);
}


///<summary>
///An instance of this class is used contain a string
///</summary>
///<param name="V">The value to format into the string</param>
///<param name="Fmt">The format specifier</param>
void Canberra::Utility::Core::String::Initialize(float V, String Fmt) {
	const wchar_t * const FormatSpecifier = (Fmt.GetLength()) ? (const wchar_t *) Fmt : L"%f";
	wchar_t Buf[4096]={0};
	swprintf(Buf, SIZEOF_WBUF(Buf), FormatSpecifier, V);
	CopyFrom(Buf);
}


///<summary>
///An instance of this class is used contain a string
///</summary>
///<param name="V">The value to format into the string</param>
Canberra::Utility::Core::String::String(bool V) : _Value(NULL), _ValueSZ(NULL) {
	wchar_t Buf[4096]={0};
	swprintf(Buf, SIZEOF_WBUF(Buf),  L"%s", V ? L"true" : L"false");
	CopyFrom(Buf);
}

///<summary>
///An instance of this class is used contain a string
///</summary>
///<param name="V">The value to format into the string</param>
Canberra::Utility::Core::String::String(const FILETIME &V) : _Value(NULL), _ValueSZ(NULL) {
#ifdef WIN32
	FILETIME f=V;
	SYSTEMTIME Time;
	FileTimeToSystemTime(&f, &Time);
	wchar_t Buf[4096]={0};
	swprintf(Buf, SIZEOF_WBUF(Buf), L"%04d-%02d-%02dT%02d:%02d:%02d", Time.wYear, Time.wMonth, Time.wDay, Time.wHour, Time.wMinute, Time.wSecond);
	CopyFrom(Buf);
#else
	time_t tm= Time::FileTimeToTime(V);
	struct tm *time=localtime(&tm);
	wchar_t Buf[4096]={0};
	swprintf(Buf, SIZEOF_WBUF(Buf), L"%04d-%02d-%02dT%02d:%02d:%02d", time->tm_year+1900, time->tm_mon+1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);
	CopyFrom(Buf);
#endif
}

#ifdef WIN32
///<summary>
///An instance of this class is used contain a string
///</summary>
///<param name="V">The value to format into the string</param>
Canberra::Utility::Core::String::String(const SYSTEMTIME &V) : _Value(NULL), _ValueSZ(NULL) {
	wchar_t Buf[4096]={0};
	swprintf(Buf, SIZEOF_WBUF(Buf), L"%-02d:%-02d:%-02d:%-02d", V.wHour, V.wMinute, V.wSecond, V.wMilliseconds);
	CopyFrom(Buf);
}
#else
///<summary>
///An instance of this class is used contain a string
///</summary>
///<param name="V">The value to format into the string</param>
Canberra::Utility::Core::String::String(const struct tm &Time) : _Value(NULL), _ValueSZ(NULL) {
	wchar_t Buf[4096]={0};
	swprintf(Buf, SIZEOF_WBUF(Buf), L"%-02d:%-02d:%-02d", Time.tm_hour, Time.tm_min, Time.tm_sec);
	CopyFrom(Buf);
}
#endif

///<summary>
///An instance of this class is used contain a string
///</summary>
///<param name="V">The string to copy</param>
Canberra::Utility::Core::String::String(const String& V) : _Value(NULL), _ValueSZ(NULL) {CopyFrom(V._Value);}
Canberra::Utility::Core::String::~String(void){Clear();}

///<summary>
///
///	CopyFrom clears any previous contents of this String and
///	sets the new String value to the concatenation of the
///	two wchar_t* arguments.  Note that one or both
///	of the arguments can be NULL, and that the second
///	argument is optional and defaults to NULL.
///
///</summary>
///<param name="S1">The first string to copy</param>
///<param name="S2">The optional second string to append to the first</param>
///<returns>0==Number of chars copied</returns>
///<exception cref="ULONG">Win32 error code</exception>
ULONG Canberra::Utility::Core::String::CopyFrom(const wchar_t *const S1, const wchar_t *const S2)
{
	Clear();

	//
	//	NULL parameters will make us return with allocating any space for a string
	//
	if (NULL == S1 && NULL == S2) return 0;

	size_t Len1 = 0, Len = 0;
	if (S1) Len1 = Len = wcslen(S1);
	if (S2) Len += wcslen(S2);

	_Value = new wchar_t[Len+1];
	if (NULL == _Value) throw (ULONG) ERROR_OUTOFMEMORY;
	*_Value = 0;
	if (S1) ::wcscpy(_Value, S1);
	if (S2) ::wcscpy(_Value + Len1, S2);
	return Len;
}

///<summary>
///This method will copy from the input char
///stream into this instance
///</summary>
///<param name="V">The string to copy</param>
///<returns>0==Number of chars copied</returns>
ULONG Canberra::Utility::Core::String::CopyFrom(const char *const V) {
	Clear();
	if (NULL == V) return 0;

	std::string str(V);
	std::wstring wstr(str.length(), L' ');
	std::copy(str.begin(), str.end(), wstr.begin());
	return CopyFrom(wstr.data());
}

///<summary>
///This method will release the
///memory
///</summary>
void Canberra::Utility::Core::String::Clear() {
	if (_Value) {delete [] _Value;_Value = NULL;}
	if (_ValueSZ) {delete [] _ValueSZ; _ValueSZ=NULL;}
}

///<summary>
///This method will return the length of the string
///</summary>
///<returns>Length</returns>
ULONG Canberra::Utility::Core::String::GetLength() const {
	if (_Value) return ((ULONG) wcslen(_Value));
	return 0;
}

///<summary>
///This method will return the unmanaged string
///</summary>
///<returns>The string</returns>
const wchar_t * const Canberra::Utility::Core::String::GetValue() const {return _Value;}

///<summary>
///This method perform a deep copy via the = operator
///</summary>
///<param name="Rhs">The string to be copied</param>
///<returns>The copied string</returns>
Canberra::Utility::Core::String& Canberra::Utility::Core::String::operator=( const Canberra::Utility::Core::String& Rhs) {
	if (this == &Rhs) return *this;
	if (NULL == _Value) {
		CopyFrom(Rhs._Value);
		return *this;
	}
	if (NULL == Rhs._Value) {
		Clear();
		return *this;
	}
	if (!wcscmp(_Value, Rhs._Value)) return *this;
	CopyFrom(Rhs._Value);
	return *this;
}


///<summary>
///This method append a string via the + operator
///</summary>
///<param name="Rhs">The source string</param>
///<returns>The appended string</returns>
///<exception cref="ULONG">Win32 error code</exception>
Canberra::Utility::Core::String Canberra::Utility::Core::String::operator+( const Canberra::Utility::Core::String& a) const
{
	return String(_Value, a._Value);
}


///<summary>
///This method append a string via the += operator
///</summary>
///<param name="Rhs">The source string</param>
///<returns>The appended string</returns>
///<exception cref="ULONG">Win32 error code</exception>
Canberra::Utility::Core::String& Canberra::Utility::Core::String::operator+=(const Canberra::Utility::Core::String& a)
{
	size_t i=((NULL !=_Value)? wcslen(_Value) : 0);
	i+= ((NULL != a._Value) ? wcslen(a._Value) : 0);
	i++;
	std::auto_ptr<wchar_t>Buf(new wchar_t[i]);
	if (NULL == Buf.get()) throw (ULONG) ERROR_OUTOFMEMORY;
	memset(Buf.get(), 0, sizeof(wchar_t)*i);
	if (_Value) wcscpy(Buf.get(), _Value);
	if (a._Value)wcscat(Buf.get(), a._Value);
	CopyFrom(Buf.get());
	return *this;
}


///<summary>
///This method return the unmanaged string via the cast operator
///</summary>
///<returns>The string</returns>
Canberra::Utility::Core::String::operator wchar_t*() {return _Value;}

#if (defined(WIN32) || defined(WINCE))
LONG sbcs_to_unicode(const char * const pSrc, WCHAR *pDest, LONG DestSize)
{
    long Ret = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pSrc,-1,pDest,DestSize);
	if (!Ret) return GetLastError();
    return 0;
}

LONG unicode_to_sbcs(const wchar_t * const pSrc, char *pDest, LONG DestSize)
{
	LONG Ret=0;
	LONG Len=0;

	memset(pDest, 0, DestSize);
	Ret = WideCharToMultiByte(CP_ACP, 0, pSrc, Len=(LONG) wcslen(pSrc),
							  pDest, DestSize, NULL, NULL);
	if (!Ret) return (GetLastError());

	/* NULL terminate */
	pDest[Len] = '\0';
	return 0;

}
#endif

///<summary>
///This method return the unmanaged string via the cast operator
///</summary>
///<returns>The string</returns>
Canberra::Utility::Core::String::operator char*() {
	if (!_Value) return NULL;
	if (_ValueSZ) {delete [] _ValueSZ; _ValueSZ=NULL;}
#if (defined(WIN32) || defined(WINCE))
	LONG Len = (LONG) wcslen(_Value)+1;
	if (_ValueSZ) {delete [] _ValueSZ; _ValueSZ=NULL;}
	_ValueSZ = new char[Len+1];
	memset(_ValueSZ, 0, sizeof(char)*(Len+1));
	ULONG Stat = unicode_to_sbcs(_Value, _ValueSZ, Len);
	if (Stat) {
		DEBUG_ERROR_MESSAGE(_T("String"), _T("operator char *"), _T("Error with unicode_to_sbcs."), Stat);
	}
	return _ValueSZ;
#else
	std::wstring s(_Value);
	std::string temp(s.length(), ' ');
	std::copy(s.begin(), s.end(), temp.begin());
	_ValueSZ = new char[strlen(temp.data())+1];
	strcpy(_ValueSZ, temp.data());
	return _ValueSZ;
#endif

}


///<summary>
///This method compare to strings using case sensitive
///comparison
///</summary>
///<param name="Rhs">The source string</param>
///<returns>Compare state</returns>
LONG Canberra::Utility::Core::String::Compare(const String& Rhs) const {
	if (_Value == Rhs._Value) return 0;
	else if (NULL == Rhs._Value) return 1;
	else if (NULL == _Value) return -1;
	else return wcscmp(_Value, Rhs._Value);
}


LONG Canberra::Utility::Core::String::Compare(const wchar_t* pRhs) const {
	if (_Value == pRhs) return 0;
	else if (NULL == pRhs) return 1;
	else if (NULL == _Value) return -1;
	else return wcscmp(_Value, pRhs);
}

bool caseInsCharCompareN(char a, char b) {return(toupper(a) == toupper(b));}
bool caseInsCharCompareW(wchar_t a, wchar_t b) {return(towupper(a) == towupper(b));}
bool caseInsCompare(const std::string& s1, const std::string& s2) {
	return((s1.size( ) == s2.size( )) &&
            equal(s1.begin( ), s1.end( ), s2.begin( ), caseInsCharCompareN));
}
bool caseInsCompare(const std::wstring& s1, const std::wstring& s2) {
     return((s1.size( ) == s2.size( )) &&
            equal(s1.begin( ), s1.end( ), s2.begin( ), caseInsCharCompareW));
}

///<summary>
///This method compare to strings using case insensitive
///comparison
///</summary>
///<param name="Rhs">The source string</param>
///<returns>Equals state</returns>
LONG Canberra::Utility::Core::String::CompareI(const String& Rhs) const {
	if (_Value == Rhs._Value) return 0;
	else if (NULL == Rhs._Value) return 1;
	else if (NULL == _Value) return -1;

	std::wstring rhs(Rhs._Value);
	std::wstring lhs(_Value);
	return !caseInsCompare(rhs, lhs);
}

LONG Canberra::Utility::Core::String::CompareI(const wchar_t* pRhs) const {
	if (_Value == pRhs) return 0;
	else if (NULL == pRhs) return 1;
	else if (NULL == _Value) return -1;
	std::wstring rhs(pRhs);
	std::wstring lhs(_Value);
	return !caseInsCompare(rhs, lhs);
}


///<summary>
///This method return an uppercase copy of this
///string
///</summary>
///<returns>The uppercase string</returns>
Canberra::Utility::Core::String Canberra::Utility::Core::String::ToUpper() const
{
	if (this->GetLength() < 1) return String(L"");
	String New(_Value);
	long Len=New.GetLength();
	for(int i=0; i<Len; i++) {
		((wchar_t *) New)[i] = towupper(((wchar_t *) New)[i]);
	}
	return New;
}


///<summary>
///This method return an lowercase copy of this
///string
///</summary>
///<returns>The lowercase string</returns>
Canberra::Utility::Core::String Canberra::Utility::Core::String::ToLower() const
{
	if (this->GetLength() < 1) return String(L"");
	String New(_Value);
	long Len=New.GetLength();
	for(int i=0; i<Len; i++) {
		((wchar_t *) New)[i] = towlower(((wchar_t *) New)[i]);
	}
	return New;
}


///<summary>
///This method replace all occurances of the substring
///with the new substring
///</summary>
///<param name="Old">The old substring</param>
///<param name="New">The new substring</param>
///<returns>The replaced string</returns>
Canberra::Utility::Core::String Canberra::Utility::Core::String::Replace(const String& Old, const String& New) const
{
	if ((NULL == _Value) || (NULL == Old._Value) || (NULL == New._Value)) return String(L"");

	//See if the old and new are the same
	if (!Old.Compare(New)) return String(_Value);

	CStringReplacer Replacer(Old._Value, New._Value);
	std::wstring Temp(_Value);
	Replacer.Replace(Temp);
	return String(Temp.data());
}


///<summary>
///This method extract a substring from the string
///</summary>
///<param name="Start">The starting position</param>
///<param name="Length">The number of chars to copy</param>
///<returns>The substring string</returns>
Canberra::Utility::Core::String Canberra::Utility::Core::String::Substring(unsigned long Start, unsigned long Length) const
{
	if ((NULL == _Value) || (Start >= this->GetLength()) || (Start < 0)) return String(L"");
	ULONG Num=this->GetLength()-Start;
	ULONG Len=Num+1;
	std::auto_ptr<wchar_t> Buf(new wchar_t[Len]);
	memset(Buf.get(), 0, sizeof(wchar_t)*(Len));

	if ((Length > 0) && (Length < Num)) Num=Length;
	wcsncpy(Buf.get(), this->_Value+Start, Num);
	return String(Buf.get());
}

///<summary>
///This method find the starting location of the
///substring in this string.  -1==Substring is not
///found
///</summary>
///<param name="Comp">The substring</param>
///<param name="Start">The starting index of the search</param>
///<returns>Index of the first substring</returns>
LONG Canberra::Utility::Core::String::IndexOf(const String& Comp, long Start) const
{
	if ((NULL == _Value) || (NULL == Comp._Value)) return -1;
	else if ((Start < 0) || (Start >= (long) (this->GetLength()))) return -1;

	wchar_t *Ptr = wcsstr(_Value+Start, Comp._Value);
	if (NULL == Ptr) return -1;
	else return abs(reinterpret_cast<long>(_Value)-reinterpret_cast<long>(Ptr))/sizeof(wchar_t);
}

///<summary>
///This method find the starting location of the
///character in this string.  -1==Character is not
///found
///</summary>
///<param name="Comp">The char </param>
///<param name="Start">The starting index of the search</param>
///<returns>Index of the first occurance of the char</returns>
LONG Canberra::Utility::Core::String::IndexOf(wchar_t Comp, long Start) const
{
	if (NULL == _Value) return -1;
	else if ((Start < 0) || (Start >= (long) (this->GetLength()))) return -1;

	wchar_t *Ptr = wcschr(_Value+Start, Comp);
	if (NULL == Ptr) return -1;
	else return abs(reinterpret_cast<long>(_Value)-reinterpret_cast<long>(Ptr))/sizeof(wchar_t);
}



///<summary>
///	Treats this string as a format pattern, and returns a new string
///	which is the result of calling FormatMessage with this string
///	and the supplied argument.
///</summary>
///<param name="args">An array of wchar_t* strings that will be substituted into the pattern</param>
///<returns>The final formatted result</returns>
Canberra::Utility::Core::String Canberra::Utility::Core::String::FormatString(const wchar_t* const args[]) const {
#ifdef WIN32
	static const Canberra::Utility::Core::String errMsg(L"String::FormatString failed with error code ");
	wchar_t* buff = NULL;
	DWORD size = ::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
						_Value, 0, 0, (LPWSTR)&buff, 1, (va_list*)args);

	Canberra::Utility::Core::String result;
	if (size)
	{
		result = buff;
	}

	else
	{
		result = (Canberra::Utility::Core::String)errMsg + Canberra::Utility::Core::String(::GetLastError());
	}

	::LocalFree(buff);
#else
	String result = String(L"");
#endif
	return result;
}


///<summary>
///This method compare to strings using case sensitive
///comparison
///</summary>
///<param name="Rhs">The source string</param>
///<returns>Equals state</returns>
bool Canberra::Utility::Core::String::operator==(const Canberra::Utility::Core::String& Rhs) const {return 0 == Compare(Rhs);}
bool Canberra::Utility::Core::String::operator==(const wchar_t* pRhs) const {return 0 == Compare(pRhs);}


///<summary>
///This method determine the none equalvalence of
///two strings via case sensitive
///comparison
///</summary>
///<param name="Rhs">The source string</param>
///<returns>Inequality state</returns>
bool Canberra::Utility::Core::String::operator!=(const Canberra::Utility::Core::String& Rhs) const {
	return Compare(Rhs) != 0;
}

bool Canberra::Utility::Core::String::operator!=(const wchar_t* pRhs) const
{
	return Compare(pRhs) != 0;
}

bool Canberra::Utility::Core::String::operator<(const Canberra::Utility::Core::String& Rhs) const {
	return Compare(Rhs) < 0;
}

bool Canberra::Utility::Core::String::operator>(const Canberra::Utility::Core::String& Rhs) const {
	return Compare(Rhs) > 0;
}
bool Canberra::Utility::Core::String::operator<=(const Canberra::Utility::Core::String& Rhs) const {
	return Compare(Rhs) <= 0;
}

bool Canberra::Utility::Core::String::operator>=(const Canberra::Utility::Core::String& Rhs) const {
	return Compare(Rhs) >= 0;
}

Canberra::Utility::Core::String& Canberra::Utility::Core::String::operator=( const wchar_t* Rhs) {CopyFrom(Rhs); return *this;}
Canberra::Utility::Core::String& Canberra::Utility::Core::String::operator=( const char * Rhs) {CopyFrom(Rhs); return *this;}

Canberra::Utility::Core::String::String(long V, String S) : _Value(NULL), _ValueSZ(NULL) {Initialize(V, S);}
Canberra::Utility::Core::String::String(unsigned long V, String S) : _Value(NULL), _ValueSZ(NULL) {Initialize(V, S);}
Canberra::Utility::Core::String::String(int V, String S) : _Value(NULL), _ValueSZ(NULL) {Initialize(V, S);}
Canberra::Utility::Core::String::String(unsigned int V, String S) : _Value(NULL), _ValueSZ(NULL) {Initialize(V, S);}
Canberra::Utility::Core::String::String(short V, String S) : _Value(NULL), _ValueSZ(NULL) {Initialize(V, S);}
Canberra::Utility::Core::String::String(unsigned short V, String S) : _Value(NULL), _ValueSZ(NULL) {Initialize(V, S);}
Canberra::Utility::Core::String::String(double V, String S) : _Value(NULL), _ValueSZ(NULL) {Initialize(V, S);}
Canberra::Utility::Core::String::String(float V, String S) : _Value(NULL), _ValueSZ(NULL) {Initialize(V, S);}
Canberra::Utility::Core::String::String(long V) : _Value(NULL), _ValueSZ(NULL) {String Null; Initialize(V, Null);}
Canberra::Utility::Core::String::String(unsigned long V) : _Value(NULL), _ValueSZ(NULL) {String Null; Initialize(V, Null);}
Canberra::Utility::Core::String::String(int V) : _Value(NULL), _ValueSZ(NULL) {String Null; Initialize(V, Null);}
Canberra::Utility::Core::String::String(unsigned int V) : _Value(NULL), _ValueSZ(NULL) {String Null; Initialize(V, Null);}
Canberra::Utility::Core::String::String(short V) : _Value(NULL), _ValueSZ(NULL) {String Null; Initialize(V, Null);}
Canberra::Utility::Core::String::String(unsigned short V) : _Value(NULL), _ValueSZ(NULL) {String Null; Initialize(V, Null);}
Canberra::Utility::Core::String::String(double V) : _Value(NULL), _ValueSZ(NULL) {String Null; Initialize(V, Null);}
Canberra::Utility::Core::String::String(float V) : _Value(NULL), _ValueSZ(NULL) {String Null; Initialize(V, Null);}
