#include "Variant.h"
#include "VariantArray.h"
#include "DebugMacros.h"
#include <math.h>
#include <memory.h>


///<summary>
///This method will convert the variant contents into
///a string
///</summary>
///<returns>The string rep. of the value</returns>
Canberra::Utility::Core::String Canberra::Utility::Core::Variant::ConvertToString(LCID Locale) const
{

	//
	//	If it's an array, let the VariantArray class provide the string
	//
	if (GetIsArray())
	{
		return _uVal.pArrayVal->ToString(Locale);
	}

	switch(_Type)
	{
		case Uint64:
			return String((double) _uVal.ui64Val);
		case Int64:
			return String((double) _uVal.i64Val);
		case Ubyte:
			return String(_uVal.ubVal);
		case Ushort:
			return String(_uVal.usVal);
		case Ulong:
			return String(_uVal.ulVal);
		case Byte:
			return String(_uVal.bVal);
		case Short:
			return String(_uVal.sVal);
		case Bool:
			return String(_uVal.boolVal);
		case Long:
			return String(_uVal.lVal);
		case Float:
			return String(_uVal.fVal, String(L"%1.9e"));
		case Double:
			return String(_uVal.dVal, String(L"%1.16e"));
		case Wstring:
			return _strVal;
		case Null:
			return String(L"null");
		case DateTime:
			return String(_uVal.dateVal);
		case Unknown:
			return String(L"unknown");

		default:
			DEBUG_ERROR_MESSAGE(L"Variant", L"ConvertToString", L"Cannot convert unknown type", E_FAIL);
			break;
	}
	return String(L"");
}


ULONGULONG Canberra::Utility::Core::Variant::ConvertToInt64Time() const
{
	LONGLONG result = 0;

	//
	//	If its a double type, we need to convert it, otherwise
	//	we can just return the int value
	//
	switch(_Type)
	{
	case Float:
	case Double:
		result = DoubleTimeToInt64Time(VariantHelper<double>::Convert(*this));
		break;

	default:
		result = VariantHelper<ULONGULONG>::Convert(*this);
		break;
	}

	return result;
}


double Canberra::Utility::Core::Variant::ConvertToDoubleTime() const
{
	//
	//	If we can NOT make the conversion, we will return 0.0
	//
	double result = 0.0;

	//
	//	If this Variant is currently a float or double, we leave the
	//	value alone, otherwise we treat it as an int and convert
	//	from FILETIME (LONGLONG) to VARIANT DATE (double).
	//
	switch (_Type)
	{
	case Double:
		result = _uVal.dVal;
		break;

	case Float:
		result = _uVal.fVal;
		break;

	default:
		result = Int64TimeToDoubleTime(VariantHelper<ULONGULONG>::Convert(*this));
	}

	return result;
}


//
//	Notes for time conversions:
//		LONGLONG time is 100 nanosecond intervals from Jan 1, 1601
//		double time is days since Dec 30, 1899
//
//		Add 109205 to double time to offset to match LONGLONG time
//		Multiple double time by 864000000000 to scale to 100 nsec units
//
//		Notice that the number of days used in the conversion
//		(roughly the number of days between 1601 and 1900)
//		needs a fudge factor.  This would appear to be because
//		of leap seconds that have been added, making the exact
//		offset between earlier and later dates NOT an exact
//		number of days.
//
//		[bradoes 4/5/2006]
//		I have taken out the fudge factor because it was producing
//		a 345 msec difference between what Microsoft claims the proper
//		conversion is, and what I was claiming.  We will go with
//		Microsoft's number cuz, well, Bill's got more money than I do.
//
//static const long double __day_offset = 109205.00000399306;
static const long double __day_offset = 109205.0;
static const long double __nanosecond_conversion = 864000000000.0;

ULONGULONG Canberra::Utility::Core::Variant::DoubleTimeToInt64Time(double dt)
{
	long double _dt = dt;
	_dt += __day_offset;
	return (ULONGULONG)(_dt * __nanosecond_conversion);
}

double Canberra::Utility::Core::Variant::Int64TimeToDoubleTime(ULONGULONG ft)
{
	long double result = (long double)ft;
	result /= __nanosecond_conversion;
	result -= __day_offset;
	return (double)result;
}


void Canberra::Utility::Core::Variant::AssignDateTime(ULONGULONG int64Time)
{
	Clear();
	_uVal.int64DateVal = int64Time;
	_Type = DateTime;
}

void Canberra::Utility::Core::Variant::AssignDateTime(LONGLONG int64Time)
{
	Clear();
	_uVal.int64DateVal = int64Time;
	_Type = DateTime;
}

void Canberra::Utility::Core::Variant::AssignDateTime(double doubleTime)
{
	Clear();
	_uVal.int64DateVal = DoubleTimeToInt64Time(doubleTime);
	_Type = DateTime;
}


//
//	Function:	CompareDoubles
//
//	Returns:	LONG 1 = d1 > d2
//					 0 = d1 == d2 withing a small tolerance
//					-1 = d1 < d2
//
const double Canberra::Utility::Core::Variant::DBL_THRESHOLD = 0.000000000001;
const double Canberra::Utility::Core::Variant::FLT_THRESHOLD = 0.000001;

LONG Canberra::Utility::Core::Variant::CompareDoubles(double d1, double d2)
{
	return CompareDoublesHelper(d1, d2, DBL_THRESHOLD);
}


LONG Canberra::Utility::Core::Variant::CompareFloats(float d1, float d2)
{
	return CompareDoublesHelper(d1, d2, FLT_THRESHOLD);
}


LONG Canberra::Utility::Core::Variant::CompareDoublesHelper(double d1, double d2, double threshold)
{
	LONG result = 0;
	double diff = 0.0;

	//Special case for 0.0
	if ((d1 == 0.0) && (d2 != 0.0)) return -1;
	else if ((d1 != 0.0) && (d2 == 0.0)) return 1;

	if ((d1 != 0.0) && (d2 != 0.0))
	{
		double quot = d1/d2;
		diff = quot - 1.0;
	}

	else
	{
		diff = d1 - d2;
	}

	if (diff < -threshold) result = -1;
	else if (diff > threshold) result = 1;

	return result;
}



///<summary>
///This method will compare one variant to another
///	Returns:
///		Less than zero implies that THIS variant is less than the argument
///		Greater than zero implies that THIS variant is greater than the argument
///		Zero implies that the two variants have equal value (although not necessarily equal type)
///</summary>
///<param name="V">The variant to compare to</param>
///<returns>value</returns>
LONG Canberra::Utility::Core::Variant::Compare(const Variant &V) const
{
	//
	//	Handle the Null and No Value cases separately
	//
	if (!GetHasValue())
	{
		return !V.GetHasValue() ? 0 : -1;
	}

	else if (!V.GetHasValue())
	{
		return 1;
	}

	if (GetIsNull())
	{
		return V.GetIsNull() ? 0 : -1;
	}

	else if (V.GetIsNull())
	{
		return 1;
	}

	if (GetIsArray())
	{
		return V.GetIsArray() ? (getArrayValue().Equals(V.getArrayValue()) ? 0 : -1) : -1;
	}

	else if (V.GetIsArray())
	{
		return 1;
	}

	switch(_Type)
	{
		case Uint64:
			return (LONG) (_uVal.ui64Val - VariantHelper<ULONGULONG>::Convert(V._Type, V._uVal, V._strVal));
		case Int64:
			return (LONG) (_uVal.i64Val - VariantHelper<LONGLONG>::Convert(V._Type, V._uVal, V._strVal));
		case Ubyte:
			return (LONG) (_uVal.ubVal - VariantHelper<unsigned char>::Convert(V._Type, V._uVal, V._strVal));
		case Ushort:
			return (LONG) (_uVal.usVal - VariantHelper<unsigned short>::Convert(V._Type, V._uVal, V._strVal));
		case Ulong:
			return (LONG) (_uVal.ulVal - VariantHelper<unsigned long>::Convert(V._Type, V._uVal, V._strVal));
		case Byte:
			return (LONG) (_uVal.bVal - VariantHelper<char>::Convert(V._Type, V._uVal, V._strVal));
		case Short:
			return (LONG) (_uVal.sVal - VariantHelper<short>::Convert(V._Type, V._uVal, V._strVal));
		case Bool:
			return (LONG) (_uVal.boolVal - VariantHelper<bool>::Convert(V._Type, V._uVal, V._strVal));
		case Long:
			return (LONG) (_uVal.lVal - VariantHelper<long>::Convert(V._Type, V._uVal, V._strVal));

		case Float:
		{
			return CompareFloats(_uVal.fVal, VariantHelper<float>::Convert(V._Type, V._uVal, V._strVal));
		}

		case Double:
		{
			return CompareDoubles(_uVal.dVal, VariantHelper<double>::Convert(V._Type, V._uVal, V._strVal));
		}

		case Wstring:
		{
			return const_cast<Variant &>(V).ToString().Compare(_strVal);
			break;
			}

		case DateTime:
			{
				LONGLONG diff = ConvertToInt64Time() - V.ConvertToInt64Time();
				return diff > 0 ? 1L : (diff == 0 ? 0L : -1L);
			}

		default:
			{
#ifdef WIN32
#define swprintf _snwprintf
#endif
				wchar_t buff[1024];
				swprintf(buff, 1024, L"Cannot compare unknown Variant type: Unknown type: 0x%04X, other type: 0x%04X", _Type, V._Type);
				DEBUG_ERROR_MESSAGE(L"Variant", L"Compare", buff, 0);
				return -1;
			}
			break;
	}
}


///<summary>
///This method will clear the value of this variant
///instance
///</summary>
void Canberra::Utility::Core::Variant::Clear()
{
	bool isArray = GetIsArray();
	_Type = Unknown;

	if (isArray)
	{
		delete _uVal.pArrayVal;
	}

	if (_varArray)
	{
		delete _varArray;
		_varArray = NULL;
	}

	memset(&_uVal, 0, sizeof(_uVal));
	_strVal.Clear();
}


//
//	Constructors
//
namespace Canberra { namespace Utility { namespace Core
{
	Variant::Variant(void) : _Type(Unknown), _varArray(NULL) {Clear();}
	Variant::Variant(const Variant &v) : _Type(Unknown), _varArray(NULL) {Assign(v);}
	Variant::Variant(unsigned char v) : _Type(Unknown), _varArray(NULL) {Assign(v);}
	Variant::Variant(unsigned short v) : _Type(Unknown), _varArray(NULL) {Assign(v);}
	Variant::Variant(unsigned int v) : _Type(Unknown), _varArray(NULL) {Assign(v);}
	Variant::Variant(unsigned long v) : _Type(Unknown), _varArray(NULL) {Assign(v);}
	Variant::Variant(ULONGULONG v) : _Type(Unknown), _varArray(NULL) {Assign(v);}
	Variant::Variant(char v) : _Type(Unknown), _varArray(NULL) {Assign(v);}
	Variant::Variant(short v) : _Type(Unknown), _varArray(NULL) {Assign(v);}
	Variant::Variant(int v) : _Type(Unknown), _varArray(NULL) { Assign(v); }
	Variant::Variant(long v) : _Type(Unknown), _varArray(NULL) { Assign(v); }
	Variant::Variant(LONGLONG v) : _Type(Unknown), _varArray(NULL) {Assign(v);}
	Variant::Variant(float v) : _Type(Unknown), _varArray(NULL) {Assign(v);}
	Variant::Variant(double v) : _Type(Unknown), _varArray(NULL) {Assign(v);}
	Variant::Variant(const wchar_t *v) : _Type(Unknown), _varArray(NULL) {Assign(v);}
	Variant::Variant(bool v) : _Type(Unknown), _varArray(NULL) {Assign(v);}
	Variant::Variant(const FILETIME& v) : _Type(Unknown), _varArray(NULL) {Assign(v);}
	Variant::Variant(const VariantArray& v) : _Type(Unknown), _varArray(NULL)  { Assign(v); }
}}}

Canberra::Utility::Core::Variant::~Variant(void)
{
	Clear();
}


//Returns the has value state
bool Canberra::Utility::Core::Variant::GetHasValue() const {return (Unknown != _Type);}

//Returns the type
Canberra::Utility::Core::Variant::VariantType Canberra::Utility::Core::Variant::GetType() const
{
	return _Type;
}


//
//	Make this Variant represent a valid Null value
//
void Canberra::Utility::Core::Variant::MakeNull()
{
	Clear();
	_Type = Null;
}


//
//	Clears the variant and assigns it a new type and value
//
namespace Canberra { namespace Utility { namespace Core
{
	//
	//	We need some help copying the _uVal union from the original
	//	Variant to this Variant.  For non-array values, this is
	//	easy, and we can use simple assignment as was done in the
	//	original code.  However, for an array, _uVal contains a pointer
	//	to a VariantArray which was created with the 'new' operator.
	//	If we simply copy it, deleting the original Variant will
	//	invalidate the VariantArray out from under us.
	//	This make software go boom.
	//
	void Variant::copyUVal(const Variant& Rhs)
	{
		if (!Rhs.GetIsArray())
		{
			_uVal = Rhs._uVal;
		}

		else if (Rhs._uVal.pArrayVal != NULL)
		{
			_uVal.pArrayVal = new VariantArray(*Rhs._uVal.pArrayVal);
		}

		else
		{
			//
			//	This case is fatally wrong, but there is nothing we can do
			//	here, since the original Variant is already corrupted.
			//
			_uVal.pArrayVal = NULL;
		}
	}

	void Variant::Assign(const Variant &v) {Clear(); _Type = v._Type; copyUVal(v); _strVal = v._strVal;}
	void Variant::Assign(unsigned char v) {Clear(); _uVal.ubVal = v;_Type = Ubyte;}
	void Variant::Assign(unsigned short v) {Clear(); _uVal.usVal = v;_Type = Ushort;}
	void Variant::Assign(unsigned int v) {Clear(); _uVal.ulVal = v;_Type = Ulong;}
	void Variant::Assign(unsigned long v) {Clear(); _uVal.ulVal = v;_Type = Ulong;}
	void Variant::Assign(ULONGULONG v) {Clear(); _uVal.ui64Val = v;_Type = Uint64;}
	void Variant::Assign(char v) {Clear(); _uVal.bVal = v;_Type = Byte;}
	void Variant::Assign(short v) {Clear(); _uVal.sVal = v;_Type = Short;}
	void Variant::Assign(LONGLONG v) {Clear(); _uVal.i64Val = v;_Type = Int64;}
	void Variant::Assign(int v) {Clear(); _uVal.lVal = v;_Type = Long;}
	void Variant::Assign(long v) {Clear(); _uVal.lVal = v;_Type = Long;}
	void Variant::Assign(float v) {Clear(); _uVal.fVal = v;_Type = Float;}
	void Variant::Assign(double v) {Clear(); _uVal.dVal = v;_Type = Double;}
	void Variant::Assign(const wchar_t *v) {Clear(); _strVal = v;_Type = Wstring;}
	void Variant::Assign(bool v) {Clear();_uVal.bVal = v;_Type = Bool;}
	void Variant::Assign(const FILETIME& v) { Clear(); _uVal.dateVal=v; _Type=DateTime; }
	void Variant::Assign(const VariantArray& v)
	{
		Clear();
		_Type = (Variant::VariantType)(Array | v.GetArrayType());
		_uVal.pArrayVal = new VariantArray(v);
	}
}}}


Canberra::Utility::Core::String Canberra::Utility::Core::Variant::ToString(LCID Locale) const { return ConvertToString(Locale);}
float Canberra::Utility::Core::Variant::ToFloat() const {return VariantHelper<float>::Convert(_Type, _uVal, _strVal);}
double Canberra::Utility::Core::Variant::ToDouble() const{return VariantHelper<double>::Convert(_Type, _uVal, _strVal);}
int Canberra::Utility::Core::Variant::ToInt() const {return VariantHelper<int>::Convert(_Type, _uVal, _strVal);}
long Canberra::Utility::Core::Variant::ToLong() const {return VariantHelper<long>::Convert(_Type, _uVal, _strVal);}
char Canberra::Utility::Core::Variant::ToByte() const {return VariantHelper<char>::Convert(_Type, _uVal, _strVal);}
short Canberra::Utility::Core::Variant::ToShort() const {return VariantHelper<short>::Convert(_Type, _uVal, _strVal);}
bool Canberra::Utility::Core::Variant::ToBool() const {return VariantHelper<bool>::Convert(_Type, _uVal, _strVal);}
LONGLONG Canberra::Utility::Core::Variant::ToInt64() const {return VariantHelper<LONGLONG>::Convert(_Type, _uVal, _strVal);}
unsigned char Canberra::Utility::Core::Variant::ToUbyte() const {return VariantHelper<unsigned char>::Convert(_Type, _uVal, _strVal);}
unsigned short Canberra::Utility::Core::Variant::ToUshort() const {return VariantHelper<unsigned short>::Convert(_Type, _uVal, _strVal);}
unsigned long Canberra::Utility::Core::Variant::ToUlong() const {return VariantHelper<unsigned long>::Convert(_Type, _uVal, _strVal);}
unsigned int Canberra::Utility::Core::Variant::ToUint() const {return VariantHelper<unsigned int>::Convert(_Type, _uVal, _strVal);}
ULONGULONG Canberra::Utility::Core::Variant::ToUint64()  const{return VariantHelper<ULONGULONG>::Convert(_Type, _uVal, _strVal);}


namespace Canberra { namespace Utility { namespace Core
{
	Variant::operator const wchar_t *() const { _fmtVal=ConvertToString(); return _fmtVal;}
	Variant::operator float() const {return VariantHelper<float>::Convert(_Type, _uVal, _strVal);}
	Variant::operator double() const {return VariantHelper<double>::Convert(_Type, _uVal, _strVal);}
	Variant::operator int() const { return VariantHelper<int>::Convert(_Type, _uVal, _strVal); }
	Variant::operator long() const { return VariantHelper<long>::Convert(_Type, _uVal, _strVal); }
	Variant::operator LONGLONG() const {return VariantHelper<LONGLONG>::Convert(_Type, _uVal, _strVal);}
	Variant::operator char() const {return VariantHelper<char>::Convert(_Type, _uVal, _strVal);}
	Variant::operator short() const {return VariantHelper<short>::Convert(_Type, _uVal, _strVal);}
	Variant::operator bool() const {return VariantHelper<bool>::Convert(_Type, _uVal, _strVal);}
	Variant::operator unsigned char() const {return VariantHelper<unsigned char>::Convert(_Type, _uVal, _strVal);}
	Variant::operator unsigned short() const {return VariantHelper<unsigned short>::Convert(_Type, _uVal, _strVal);}
	Variant::operator unsigned int() const {return VariantHelper<unsigned long>::Convert(_Type, _uVal, _strVal);}
	Variant::operator unsigned long() const {return VariantHelper<unsigned long>::Convert(_Type, _uVal, _strVal);}
	Variant::operator ULONGULONG() const {return VariantHelper<ULONGULONG>::Convert(_Type, _uVal, _strVal);}
	Variant::operator FILETIME () const { _fileTimeIntVal = ConvertToInt64Time(); return _fileTime; }
	FILETIME Variant::ToFileTime() const { _fileTimeIntVal = ConvertToInt64Time(); return _fileTime; }

	const VariantArray& Variant::getArrayValue() const
	{
		if (GetIsArray())
		{
			return *(_uVal.pArrayVal);
		}

		else if (!GetIsNull() && GetHasValue())
		{
			//
			//	If the variant has a real value, we will create
			//	an array of length one containing that value
			//
			_varArray = new VariantArray(GetType(), *this);
			return *_varArray;
		}

		else
		{
			//
			//	If the variant does NOT have a value,
			//	we will return a zero length array of type Null.
			//
			_varArray = new VariantArray(Null);
			return *_varArray;
		}
	}
/*
	Variant::operator const VariantArray& () const
	{
		if (GetIsArray())
		{
			return *(_uVal.pArrayVal);
		}

		else
		{
			_varArray = new VariantArray(Type, *this);
			return *_varArray;
		}
	}
*/
/*	Variant::operator VariantArray& ()
	{
		if (GetIsArray())
		{
			return *(_uVal.pArrayVal);
		}

		else
		{
			_varArray = new VariantArray(*this);
			return *_varArray;
		}
	}*/
}}}


namespace Canberra { namespace Utility { namespace Core
{
	Variant& Variant::operator=( const wchar_t* Rhs) {Assign(Rhs); return *this;}
	Variant& Variant::operator=( const float Rhs) {Assign(Rhs); return *this;}
	Variant& Variant::operator=( const double Rhs) {Assign(Rhs); return *this;}
	Variant& Variant::operator=( const char Rhs) {Assign(Rhs); return *this;}
	Variant& Variant::operator=( const short Rhs) {Assign(Rhs); return *this;}
	Variant& Variant::operator=( const int Rhs) {Assign(Rhs); return *this;}
	Variant& Variant::operator=( const long Rhs) {Assign(Rhs); return *this;}
	Variant& Variant::operator=( const LONGLONG &Rhs) {Assign(Rhs); return *this;}
	Variant& Variant::operator=( const Variant &Rhs) {Assign(Rhs); return *this;}
	Variant& Variant::operator=( const bool Rhs) {Assign(Rhs); return *this;}
	Variant& Variant::operator=( const unsigned char Rhs) {Assign(Rhs); return *this;}
	Variant& Variant::operator=( const unsigned short Rhs) {Assign(Rhs); return *this;}
	Variant& Variant::operator=( const unsigned int Rhs) {Assign(Rhs); return *this;}
	Variant& Variant::operator=( const unsigned long Rhs) {Assign(Rhs); return *this;}
	Variant& Variant::operator=( const ULONGULONG &Rhs) {Assign(Rhs); return *this;}
	Variant& Variant::operator=( const FILETIME& Rhs ) { Assign(Rhs); return *this; }
	Variant& Variant::operator = ( const VariantArray& Rhs ) { Assign(Rhs); return *this; }
}}}



//
//	Note that these two types are not formally supported in the
//	Variant, so we have independent definitions for them
//
//	An Enum value is stored as a long
//	A Blob value is stored as an array of unsigned bytes
//
//const long Canberra::Utility::Core::Variant::TYPE_ENUM = 1024L;
//const long Canberra::Utility::Core::Variant::TYPE_BLOB = 1025L;

const long Canberra::Utility::Core::Variant::typeNumbers[] =
{
		// SINGLE VALUES
		Variant::Ubyte,
		Variant::Ushort,
		Variant::Ulong,
		Variant::Byte,
		Variant::Short,
		Variant::Long,
		Variant::Float,
		Variant::Double,
		Variant::Wstring,
		Variant::Int64,
		Variant::Uint64,
		Variant::Bool,
		Variant::DateTime,
		Variant::Null,

		// THESE TWO ARE NOT DIRECTLY SUPPORTED BY THE VARIANT CLASS
		TYPE_ENUM,		// will be a Variant Long
		TYPE_BLOB,		// will be a Variant UnsignedByteArray

		// ARRAYS
		Variant::aUbyte,
		Variant::aUshort,
		Variant::aUlong,
		Variant::aByte,
		Variant::aShort,
		Variant::aLong,
		Variant::aFloat,
		Variant::aDouble,
		Variant::aWstring,
		Variant::aInt64,
		Variant::aUint64,
		Variant::aBool,
		Variant::aDateTime,

		//	Uninitialized
		Variant::Unknown,
		-1L
};

const wchar_t* const Canberra::Utility::Core::Variant::typeNames [] =
{
		// SINGLE VALUES
		L"UnsignedByte",
		L"UnsignedShort",
		L"UnsignedLong",
		L"Byte",
		L"Short",
		L"Long",
		L"Float",
		L"Double",
		L"String",
		L"Int64",
		L"UnsignedInt64",
		L"Boolean",
		L"DateTime",
		L"Null",

		// THESE TWO ARE NOT DIRECTLY SUPPORTED BY THE VARIANT CLASS
		L"Enum",		// will be a Variant Long
		L"Blob",		// will be a Variant UnsignedByteArray

		// ARRAYS
		L"UnsignedByteArray",
		L"UnsignedShortArray",
		L"UnsignedLongArray",
		L"ByteArray",
		L"ShortArray",
		L"LongArray",
		L"FloatArray",
		L"DoubleArray",
		L"TextArray",
		L"Int64Array",
		L"UnsignedInt64Array",
		L"BooleanArray",
		L"DateTimeArray",

		//	Uninitialized
		L"Unknown",
		NULL
};


long Canberra::Utility::Core::Variant::typeName2Number(const wchar_t* const name)
{
	long result = -1L;

	for (int i = 0; typeNames[i]; ++i)
	{
		if (::wcscmp(name, typeNames[i]) == 0)
		{
			result = typeNumbers[i];
			break;
		}
	}

	return result;
}


const wchar_t* Canberra::Utility::Core::Variant::typeNumber2Name(long num)
{
	const wchar_t* result = NULL;

	for (int i = 0; typeNumbers[i] >= 0; ++i)
	{
		if (num == typeNumbers[i])
		{
			result = typeNames[i];
			break;
		}
	}

	return result;
}

