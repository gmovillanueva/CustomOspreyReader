#pragma once
#include <typeinfo>
#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <iosfwd>
#include "UtilityString.h"
#include "DebugMacros.h"
#include "UtilityImpExp.h"


using namespace std;


//
//	Currently, the UTILITY_IMPORT_USAGE macro MUST be defined by all users of
//	the Utility DLL (and thus, the Variant class).  This sets up the declarations
//	to import rather than export classes and methods.
//
//	This macro causes the UTILITY_DLLIMPORT and EXPIMP_TEMPLATE macros to be
//	defined as either importing or exporting.  This is particularly important
//	here because we must export our usage of the STL vector<> class.
//

namespace Canberra {
	namespace Utility {
		namespace Core {
		template <class T>
		bool from_string(T &t,  const std::string &s, std::ios_base & (*f)(std::ios_base&))
		{
			std::istringstream iss(s);
			return !(iss>>f>>t).fail();
		}

			class VariantArray;

			//
			//This class represents a variant type than can contain any of the following data types
			//bool, unsigned char, unsigned short, unsigned long, char, short, long, float, double
			//or String
			//
			//This class does not use any COM technology.  The purpose is to minimize dependent resources
			//and complexity of the COM technology while gain some useful features of that technology.
			//
			class UTILITY_DLLIMPORT Variant
			{
			public:
				typedef enum VariantTypeTag {
					Unknown =	0,		// no array of Unknown is permitted
					Ubyte =		1,
					Ushort =	2,
					Ulong =		3,
					Byte =		4,
					Short =		5,
					Long =		6,
					Float =		7,
					Double =	8,
					Wstring =	9,
					Int64 =		10,
					Uint64 =	11,
					Bool =		12,
					DateTime =	13,
					Null =		14,		// no array of NULL is permitted

					Array =		0x8000,

					aUbyte =	0x8000 + 1,
					aUshort =	0x8000 + 2,
					aUlong =	0x8000 + 3,
					aByte =		0x8000 + 4,
					aShort =	0x8000 + 5,
					aLong =		0x8000 + 6,
					aFloat =	0x8000 + 7,
					aDouble =	0x8000 + 8,
					aWstring =	0x8000 + 9,
					aInt64 =	0x8000 + 10,
					aUint64 =	0x8000 + 11,
					aBool =		0x8000 + 12,
					aDateTime =	0x8000 + 13,
				} VariantType;


			protected:
				VariantType _Type;
				union NativeValues {
					unsigned char ubVal;
					unsigned short usVal;
					unsigned int ulVal;			//this is intentional for 64 bit and 32 bit to be equiv

					char bVal;
					short sVal;
					int lVal;					//this is intentional for 64 bit and 32 bit to be equiv
					float fVal;
					double dVal;
					bool boolVal;
					LONGLONG i64Val;
					ULONGULONG ui64Val;

					union
					{
						//
						//	FILETIME is a struct of two DWORDS.  We will overlay
						//	that on an int 64 for convenience.
						//
						ULONGULONG int64DateVal;
						FILETIME	dateVal;
					};

					VariantArray*	pArrayVal;
				} _uVal;

				String		_strVal;
				mutable String		_fmtVal;
				mutable VariantArray*	_varArray;

				mutable union
				{
					mutable ULONGULONG		_fileTimeIntVal;
					mutable FILETIME	_fileTime;
				};

				template <class T>
				class VariantHelper
				{
				public:
					VariantHelper() {}
					~VariantHelper() {}

					static T Convert(Variant* pVar)
					{
						return Convert(pVar->_Type, pVar->_uVal, pVar->_strVal);
					}

					static T Convert(const Variant& var)
					{
						return Convert(var._Type, var._uVal, var._strVal);
					}

					static T Convert(VariantType _Type, union NativeValues _uVal, const String &_strVal)
					{
						T result = NULL;

						switch(_Type) {
							case Int64:
								return (T) _uVal.i64Val;
							case Uint64:
								return (T) _uVal.ui64Val;
							case Bool:
								return (T) (_uVal.boolVal ? 1 : 0);
							case Ubyte:
								return (T) _uVal.ubVal;
							case Ushort:
								return (T) _uVal.usVal;
							case Ulong:
								return (T) _uVal.ulVal;
							case Byte:
								return (T) _uVal.bVal;
							case Short:
								return (T) _uVal.sVal;
							case Long:
								return (T) _uVal.lVal;
							case Float:
								return (T) _uVal.fVal;
							case Double:
								return (T) _uVal.dVal;
							case DateTime:
								return (T) _uVal.int64DateVal;
							case Null:
								return (T) 0;
							case Wstring:
							{
								//
								//	TODO : There is a defect here ... we are not handling signed vs unsigned ints
								//
								if ((typeid(T) == typeid(float)) || (typeid(T) == typeid(double)))
								{
									wchar_t *StopChar=NULL;
									if (_strVal.GetLength()) return (T) wcstod(const_cast<String &>(_strVal), &StopChar);
									else return (T) 0;
								}

								else if (typeid(T) == typeid(bool))
								{
									//
									//	We will support various text representations
									//	of boolean values.  Text starting with the
									//	characters 'y' or 't' will cause the result
									//	to be TRUE, and text starting with the
									//	characters 'n' or 'f' will cause the result
									//	to be false.
									//
									//	If neither case is true, then we will parse
									//	the value as a number, and return TRUE for
									//	non-zero results.  Note that this will cause
									//	any other non-numeric text to return FALSE.
									//
									if (_strVal.GetLength() == 0)
									{
										return false;
									}

									wchar_t c = ((const wchar_t *)_strVal)[0];
									if (c == L'y' || c == L'Y' || c == L'T' || c == L't')
									{
										return true;
									}

									else if (c == L'n' || c == L'N' || c == L'F' || c == L'f')
									{
										return false;
									}

									return atol(const_cast<String &>(_strVal)) != 0 ? true : false;
								}

								else
								{
									//
									//	parse the result into an __i64, then cast
									//	it down to T
									//
									LONGLONG ival = 0;
									if (_strVal.GetLength()) {
										//wstring theVal((const wchar_t*)_strVal);
										//wistringstream is(theVal);
										//is >> ival;
										from_string<LONGLONG>(ival,std::string(_strVal),std::dec);
										return (T) ival;
									}
									else return (T) 0;
								}
							}

							//
						    //	Now handle the Array types
						    //
							case aUbyte:
							case aUshort:
							case aUlong:
							case aByte:
							case aShort:
							case aLong:
							case aFloat:
							case aDouble:
							case aWstring:
							case aInt64:
							case aUint64:
							case aBool:
							case aDateTime:
								DEBUG_ERROR_MESSAGE(L"Variant", L"ConvertTo", L"Conversion from array types is not currently supported", E_FAIL);
								break;


							default:
								DEBUG_ERROR_MESSAGE(L"Variant", L"ConvertTo", String(L"Cannot convert unknown type. Type code is: ") + String(_Type), E_FAIL);

								break;
						}

						return result;
					}
				};
#if (defined(WIN32) || defined(WINCE))
				//template
				template<>
				class VariantHelper<String>
				{
				public:
					static String Convert(Variant* pVar)
					{
						return Convert(*pVar);
					}

					static String Convert(const Variant& var)
					{
						return var.ConvertToString();
					}
				};


				//template
				template<>
				class VariantHelper<FILETIME>
				{
				public:
					static FILETIME Convert(Variant* pVar)
					{
						return Convert(*pVar);
					}

					static FILETIME Convert(const Variant& var)
					{
						union
						{
							ULONGULONG ift;
							FILETIME ft;
						};

						ift = var.ConvertToInt64Time();
						return ft;
					}
				};
#endif


			public:
				String ConvertToString(LCID =1033) const;
				double ConvertToDoubleTime() const;
				ULONGULONG ConvertToInt64Time() const;
				void AssignDateTime(ULONGULONG int64Time);
				void AssignDateTime(LONGLONG int64Time);
				void AssignDateTime(double doubleTime);
				static ULONGULONG DoubleTimeToInt64Time(double dt);
				static double Int64TimeToDoubleTime(ULONGULONG ft);

			public:
				Variant(void);
				Variant(const Variant &v);
				Variant(unsigned char v);
				Variant(unsigned short v);
				Variant(unsigned long v);
				Variant(unsigned int v);
				Variant(ULONGULONG v);
				Variant(char v);
				Variant(short v);
				Variant(long v);
				Variant(int v);
				Variant(LONGLONG v);
				Variant(float v);
				Variant(double v);
				explicit Variant(bool v);
				Variant(const FILETIME&);
				Variant(const VariantArray&);

				explicit Variant(const wchar_t *v);

				virtual ~Variant(void);

				//Returns the has value state
				bool GetHasValue() const;

				//Returns the type
				VariantType GetType() const;


				//	Returns TRUE is the variant contains an array type
				inline bool GetIsArray() const { return (_Type & Array) ? true : false; }

				//	Returns TRUE is the Variant type is NULL
				inline bool GetIsNull() const { return (_Type == Null) ? true : false; }

				//	Clears the Variant and than sets its type to Null.
				void MakeNull();

				//Clears the variant
				void Clear();
				void Assign(const Variant &v);
				void Assign(unsigned char v);
				void Assign(unsigned short v);
				void Assign(unsigned long v);
				void Assign(unsigned int v);
				void Assign(ULONGULONG v);
				void Assign(char v);
				void Assign(short v);
				void Assign(LONGLONG v);
				void Assign(int v);
				void Assign(long v);
				void Assign(float v);
				void Assign(double v);
				void Assign(const wchar_t *v);
				void Assign(bool v);
				void Assign(const FILETIME&);
				void Assign(const VariantArray&);


				String ToString(LCID =1033) const;
				float ToFloat() const;
				double ToDouble() const;
				long ToLong() const;
				int ToInt() const;
				char ToByte() const;
				short ToShort() const;
				bool ToBool() const;
				LONGLONG ToInt64() const;
				unsigned char ToUbyte() const;
				unsigned short ToUshort() const;
				unsigned long ToUlong() const;
				unsigned int ToUint() const;
				ULONGULONG ToUint64() const;
				FILETIME ToFileTime() const;


				operator const wchar_t *() const;
				operator float() const;
				operator double() const;
				operator long() const;
				operator int() const;
				operator LONGLONG() const;
				operator char() const;
				operator short() const;
				operator bool() const;
				operator unsigned char() const;
				operator unsigned short() const;
				operator unsigned long() const;
				operator unsigned int() const;
				operator ULONGULONG() const;
				operator FILETIME() const;

				//
				//	I am having LOTS of trouble getting a cast operator
				//	to work for extracting the array value, so we are
				//	going to use a normal method instead
				//
				const VariantArray& getArrayValue() const;

				Variant& operator=( const wchar_t* Rhs);
				Variant& operator=( const float Rhs);
				Variant& operator=( const double Rhs);
				Variant& operator=( const char Rhs);
				Variant& operator=( const short Rhs);
				Variant& operator=( const long Rhs);
				Variant& operator=( const int Rhs);
				Variant& operator=( const LONGLONG &Rhs);
				Variant& operator=( const Variant &Rhs);
				Variant& operator=( const bool Rhs);
				Variant& operator=( const unsigned char Rhs);
				Variant& operator=( const unsigned short Rhs);
				Variant& operator=( const unsigned int Rhs);
				Variant& operator=( const unsigned long Rhs);
				Variant& operator=( const ULONGULONG &Rhs);
				Variant& operator=( const FILETIME& Rhs);
				Variant& operator=( const VariantArray& Rhs );

				LONG Compare(const Variant &V) const;
				static LONG CompareDoubles(double d1, double d2);
				static LONG CompareFloats(float d1, float d2);

			private:
				static const double DBL_THRESHOLD;
				static const double FLT_THRESHOLD;
				static LONG CompareDoublesHelper(double d1, double d2, double threshold);

				//
				//	Add stuff to convert between Type ID and text name for Type ID
				//
			private:
				static const long typeNumbers [];
				static const wchar_t* const typeNames [];

			public:
				static const long TYPE_ENUM = 1024L;
				static const long TYPE_BLOB = 1025L;
				static long typeName2Number(const wchar_t* const name);
				static const wchar_t* typeNumber2Name(long num);

			private:
				//
				//	Helper functions
				//
				void copyUVal(const Variant& Rhs);
			};
		}
	}
}

