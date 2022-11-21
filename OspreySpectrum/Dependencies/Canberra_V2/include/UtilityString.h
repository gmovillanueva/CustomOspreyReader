#pragma once
#include "UtilityImpExp.h"
#include <time.h>

#ifdef WIN32
#include <windows.h>
#endif

namespace Canberra {
	namespace Utility {
		namespace Core {

			//
			//An implementation of a safe string that handles SBC and Unicode formats
			//The implementation could be improved via reference counting but it works
			//well enough for current implementations.  It is best to pass Unicode
			//string such that no string conversion is performed.  The format of
			//numeric and time information uses the default format specified by
			//the ANSI string conversion routines
			//
			class UTILITY_DLLIMPORT String
			{
			protected:
				wchar_t *_Value;
				char *_ValueSZ;
				void Initialize(long V, String);
				void Initialize(unsigned long V, String);
				void Initialize(int V, String);
				void Initialize(unsigned int V, String);
				void Initialize(short V, String);
				void Initialize(unsigned short V, String);
				void Initialize(double V, String);
				void Initialize(float V, String);

			public:
				explicit String(const wchar_t * const S1, const wchar_t* const S2 = NULL);
				explicit String(const char * const S);
				explicit String();
				explicit String(long V, String S);
				explicit String(unsigned long V, String S);
				explicit String(int V, String S);
				explicit String(unsigned int V, String S);
				explicit String(short V, String S);
				explicit String(unsigned short V, String S);
				explicit String(double V, String S);
				explicit String(float V, String S);
				explicit String(long V);
				explicit String(unsigned long V);
				explicit String(int V);
				explicit String(unsigned int V);
				explicit String(short V);
				explicit String(unsigned short V);
				explicit String(double V);
				explicit String(float V);
				explicit String(bool V);
				explicit String(const FILETIME &V);
#ifdef WIN32
				explicit String(const SYSTEMTIME &V);
#else
				explicit String(const struct tm &V);
#endif
				String(const String& V);
				virtual ~String(void);

				ULONG CopyFrom(const wchar_t *const S1, const wchar_t *const S2 = NULL);
				ULONG CopyFrom(const char *const V);
				void Clear();
				ULONG GetLength() const;
				const wchar_t * const GetValue() const;


				String& operator=( const wchar_t* Rhs);
				String& operator=( const char * Rhs);
				String& operator=( const String& Rhs);
				String operator+( const String& a) const;
				String& operator+=(const String& a);

				operator char*();
				operator wchar_t*();

				//
				//	These two functions make it more convenient to use a
				//	const String
				//
				inline operator char const * const () const { return (const_cast<String*>(this))->operator char*(); }
				inline operator wchar_t const * const () const { return (const_cast<String*>(this))->operator wchar_t*(); }

				LONG IndexOf(const String &, long Start=0) const;
				LONG IndexOf(wchar_t, long Start=0) const;
				String Replace(const String &Old, const String &New) const;
				String Replace(wchar_t Old, wchar_t  New) const;
				String Substring(unsigned long Start, unsigned long Count=0) const;
				String ToUpper() const;
				String ToLower() const;

				String FormatString(const wchar_t* const args[]) const;

				LONG Compare(const String& Rhs) const;
				LONG Compare(const wchar_t* pRhs) const;
				LONG CompareI(const String& Rhs) const;
				LONG CompareI(const wchar_t* pRhs) const;
				bool operator==(const String& Rhs) const;
				bool operator==(const wchar_t* pRhs) const;
				bool operator!=(const String& Rhs) const;
				bool operator!=(const wchar_t* pRhs) const;
				bool operator<(const String& Rhs) const;
				bool operator>(const String& Rhs) const;
				bool operator<=(const String& Rhs) const;
				bool operator>=(const String& Rhs) const;

			};
		}
	}
}

