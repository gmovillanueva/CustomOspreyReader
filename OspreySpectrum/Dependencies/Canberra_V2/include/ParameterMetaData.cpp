#include "DebugMacros.h"
#include "ParameterMetaData.h"
#include "MetaData.h"
#include "endianUtils.h"
#include <memory.h>

///<summary>
///This method will perform a deep copy
///</summary>
///<param name="v">The source</param>
void Canberra::DataTypes::Application::ParameterMetaData::Copy(const Canberra::DataTypes::Application::ParameterMetaData &v) {
	_Desc = v._Desc;
	_Code = v._Code;
	_Min = v._Min;
	_Max = v._Max;
	_Def = v._Def;
	_Step = v._Step;
	_Attr = v._Attr;
	_Name = v._Name;
	_Enum = v._Enum;
	_ParameterDataType = v._ParameterDataType;
	_EnumStrings=v._EnumStrings;
	_EnumStringIds=v._EnumStringIds;
}


///<summary>
///This method will overide the equals operator
///to perform a deep copy
///</summary>
Canberra::DataTypes::Application::ParameterMetaData& Canberra::DataTypes::Application::ParameterMetaData::operator=( const ParameterMetaData &Rhs) {Copy(Rhs); return *this;}

///<summary>
///This method will initialize an instance of this class.
///An instance of this class defines all of the attributes
///of a parameter (limits, defaults, description, etc...)
///</summary>
///<param name="v">The source</param>
Canberra::DataTypes::Application::ParameterMetaData::ParameterMetaData(const ParameterMetaData &v) : Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::ParameterMetaData), _Enum(Canberra::Utility::Core::Variant::Long, 0), _EnumStrings(Canberra::Utility::Core::Variant::Wstring, 0), _EnumStringIds(Canberra::Utility::Core::Variant::Long, 0) {Copy(v);}

///<summary>
///This method will initialize an instance of this class.
///An instance of this class defines all of the attributes
///of a parameter (limits, defaults, description, etc...)
///</summary>
Canberra::DataTypes::Application::ParameterMetaData::ParameterMetaData() :  Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::ParameterMetaData), _Enum(Canberra::Utility::Core::Variant::Long, 0), _EnumStrings(Canberra::Utility::Core::Variant::Wstring, 0), _EnumStringIds(Canberra::Utility::Core::Variant::Long, 0) {}

///<summary>
///This method will initialize an instance of this class.
///An instance of this class defines all of the attributes
///of a parameter (limits, defaults, description, etc...)
///</summary>
///<param name="C">The parameter code</param>
///<param name="Desc">The description</param>
///<param name="Nam">The name</param>
///<param name="Min">The min value</param>
///<param name="Max">The max value</param>
///<param name="Def">The default value</param>
///<param name="Step">The step size</param>
///<param name="Enum">An array of the enumeration values</param>
///<param name="EnumStrings">An array of the enumeration strings</param>
///<param name="EnumStringIds">An array of the enumeration strinig resource ids</param>
///<param name="Attr">The attributes (r-only, hardware write, etc...)</param>
///<param name="PType">The value type (float, int, etc...)</param>
Canberra::DataTypes::Application::ParameterMetaData::ParameterMetaData(LONG C,
							  const Canberra::Utility::Core::Variant &Desc,
							  const Canberra::Utility::Core::Variant &Nam,
							  const Canberra::Utility::Core::Variant &Min,
							  const Canberra::Utility::Core::Variant &Max,
							  const Canberra::Utility::Core::Variant &Def,
							  const Canberra::Utility::Core::Variant &Step,
							  Canberra::Utility::Core::VariantArray &Enum,
							  Canberra::Utility::Core::VariantArray &EnumStrings,
							  Canberra::Utility::Core::VariantArray &EnumStringIds,
							  const LONG Attr, const LONG PType) : Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::ParameterMetaData),
							 _Desc(Desc), _Name(Nam),
							 _Code(C), _Min(Min), _Max(Max), _Def(Def),
							 _Step(Step), _Attr(Attr), _Enum(Enum), _ParameterDataType(PType),
							 _EnumStrings(EnumStrings), _EnumStringIds(EnumStringIds)
							 {
}
Canberra::DataTypes::Application::ParameterMetaData::~ParameterMetaData() {}

//Accessors
LONG Canberra::DataTypes::Application::ParameterMetaData::GetParameterCode() {return _Code;}
Canberra::Utility::Core::String Canberra::DataTypes::Application::ParameterMetaData::GetDescription() {return _Desc.ToString();}
Canberra::Utility::Core::String Canberra::DataTypes::Application::ParameterMetaData::GetName() {return _Name.ToString();}
Canberra::Utility::Core::Variant Canberra::DataTypes::Application::ParameterMetaData::GetMinimum() {return _Min;}
Canberra::Utility::Core::Variant Canberra::DataTypes::Application::ParameterMetaData::GetMaximum() {return _Max;}
Canberra::Utility::Core::Variant Canberra::DataTypes::Application::ParameterMetaData::GetDefault() {return _Def;}
Canberra::Utility::Core::Variant Canberra::DataTypes::Application::ParameterMetaData::GetStepSize() {return _Step;}
Canberra::DataTypes::Application::ParameterAttributes::Attributes Canberra::DataTypes::Application::ParameterMetaData::GetProperties() {return (Canberra::DataTypes::Application::ParameterAttributes::Attributes) _Attr;}
Canberra::DataTypes::Application::ParameterAttributes::ValueType Canberra::DataTypes::Application::ParameterMetaData::GetParameterDataType() {return (Canberra::DataTypes::Application::ParameterAttributes::ValueType) _ParameterDataType;}
Canberra::Utility::Core::VariantArray Canberra::DataTypes::Application::ParameterMetaData::GetEnumeration() {return _Enum;}
Canberra::Utility::Core::VariantArray Canberra::DataTypes::Application::ParameterMetaData::GetEnumerationStrings() {return _EnumStrings;}
Canberra::Utility::Core::VariantArray Canberra::DataTypes::Application::ParameterMetaData::GetEnumerationStringIds() {return _EnumStringIds;}

///<summary>
///This method will return the number of byte necessary to serialize
///the data contained within this instance
///</summary>
///<returns>The number of bytes</returns>
ULONG Canberra::DataTypes::Application::ParameterMetaData::GetDataSize() {
	LONG Size=Canberra::Serialization::VariantSerializer::GetSize(_Min);
	Size+=Canberra::Serialization::VariantSerializer::GetSize(_Max);
	Size+=Canberra::Serialization::VariantSerializer::GetSize(_Def);
	Size+=Canberra::Serialization::VariantSerializer::GetSize(_Step);
	Size+=Canberra::Serialization::VariantSerializer::GetSize(_Desc);
	Size+=Canberra::Serialization::VariantSerializer::GetSize(_Name);
	Size+=sizeof(_Code);
	Size+=sizeof(_Attr);
	Size+=sizeof(_ParameterDataType);

	Canberra::Utility::Core::Variant Val;

	size_t NumEnums=_Enum.GetSize();
	if (NumEnums > _EnumStrings.GetSize()) NumEnums =  _EnumStrings.GetSize();
	if (NumEnums > _EnumStringIds.GetSize()) NumEnums =  _EnumStringIds.GetSize();
	for(size_t i=0; i<NumEnums; i++) {
		Val.Clear();
		_Enum.Get(i, Val);
		Size+=Canberra::Serialization::VariantSerializer::GetSize(Val);
	}
	for(size_t i=0; i<NumEnums; i++) {
		Val.Clear();
		_EnumStrings.Get(i, Val);
		Size+=Canberra::Serialization::VariantSerializer::GetSize(Val);
	}
	for(size_t i=0; i<NumEnums; i++) {
		Val.Clear();
		_EnumStringIds.Get(i, Val);
		Size+=Canberra::Serialization::VariantSerializer::GetSize(Val);
	}
	return Size;

}

///<summary>
///This method will compare the value to the min and
///max values to see if it is in range
///</summary>
///<param name="v">The value to check</param>
///<returns>valid state</returns>
bool Canberra::DataTypes::Application::ParameterMetaData::GetIsValid(const Canberra::Utility::Core::Variant &v) {
	if ((_Min.Compare(v) > 0) || (_Max.Compare(v) < 0)) return false;
	else return true;
}

///<summary>
///This method will serialize this instance into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="MD">The metadata</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Application::ParameterMetaData::SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &, const LONG Offset)
{
	LONG Pos=Offset;
	LONG NumCpy=0;
	size_t NumEnums=_Enum.GetSize();
	if (NumEnums > _EnumStrings.GetSize()) NumEnums =  _EnumStrings.GetSize();
	if (NumEnums > _EnumStringIds.GetSize()) NumEnums =  _EnumStringIds.GetSize();
	SWAPNBYTES(&_Code, sizeof(_Code));
	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_Code, NumCpy=sizeof(_Code)); Pos+=NumCpy;
	SWAPNBYTES(&_Code, sizeof(_Code));

	SWAPNBYTES(&_Attr, sizeof(_Attr));
	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_Attr, NumCpy=sizeof(_Attr)); Pos+=NumCpy;
	SWAPNBYTES(&_Attr, sizeof(_Attr));

	SWAPNBYTES(&_ParameterDataType, sizeof(_ParameterDataType));
	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_ParameterDataType, NumCpy=sizeof(_ParameterDataType)); Pos+=NumCpy;
	SWAPNBYTES(&_ParameterDataType, sizeof(_ParameterDataType));

	SWAPNBYTES(&NumEnums, sizeof(NumEnums));
	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &NumEnums, NumCpy=sizeof(NumEnums)); Pos+=NumCpy;
	SWAPNBYTES(&NumEnums, sizeof(NumEnums));

	NumCpy = Canberra::Serialization::VariantSerializer::Serialize(Str, _Min, Pos); Pos+=NumCpy;
	NumCpy = Canberra::Serialization::VariantSerializer::Serialize(Str, _Max, Pos); Pos+=NumCpy;
	NumCpy = Canberra::Serialization::VariantSerializer::Serialize(Str, _Def, Pos); Pos+=NumCpy;
	NumCpy = Canberra::Serialization::VariantSerializer::Serialize(Str, _Desc, Pos); Pos+=NumCpy;
	NumCpy = Canberra::Serialization::VariantSerializer::Serialize(Str, _Name, Pos); Pos+=NumCpy;
	NumCpy = Canberra::Serialization::VariantSerializer::Serialize(Str, _Step, Pos); Pos+=NumCpy;

	Canberra::Utility::Core::Variant Val;
	for(size_t i=0; i<NumEnums; i++) {
		Val.Clear();
		_Enum.Get(i, Val);
		NumCpy=Canberra::Serialization::VariantSerializer::Serialize(Str, Val, Pos); Pos+=NumCpy;
	}
	for(size_t i=0; i<NumEnums; i++) {
		Val.Clear();
		_EnumStrings.Get(i, Val);
		NumCpy=Canberra::Serialization::VariantSerializer::Serialize(Str, Val, Pos); Pos+=NumCpy;
	}
	for(size_t i=0; i<NumEnums; i++) {
		Val.Clear();
		_EnumStringIds.Get(i, Val);
		NumCpy=Canberra::Serialization::VariantSerializer::Serialize(Str, Val, Pos); Pos+=NumCpy;
	}
	return Pos-Offset;
}

///<summary>
///This method will deserialize info from the byte stream into this instance
///</summary>
///<param name="Str">The stream</param>
///<param name="MD">The metadata</param>
///<param name="Offset">The byte position to start the read in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
ULONG Canberra::DataTypes::Application::ParameterMetaData::DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &, const LONG Offset)
{
	LONG Pos=Offset;
	LONG NumCpy=0;
	LONG NumEnums=0;
	memcpy(&_Code, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_Code)); Pos+=NumCpy;
	SWAPNBYTES(&_Code, sizeof(_Code));
	memcpy(&_Attr, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_Attr)); Pos+=NumCpy;
	SWAPNBYTES(&_Attr, sizeof(_Attr));
	memcpy(&_ParameterDataType, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_ParameterDataType)); Pos+=NumCpy;
	SWAPNBYTES(&_ParameterDataType, sizeof(_ParameterDataType));
	memcpy(&NumEnums, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(NumEnums)); Pos+=NumCpy;
	SWAPNBYTES(&NumEnums, sizeof(NumEnums));
	NumCpy = Canberra::Serialization::VariantSerializer::Deserialize(Str, _Min, Pos);	Pos+=NumCpy;
	NumCpy = Canberra::Serialization::VariantSerializer::Deserialize(Str, _Max, Pos);	Pos+=NumCpy;
	NumCpy = Canberra::Serialization::VariantSerializer::Deserialize(Str, _Def, Pos);	Pos+=NumCpy;
	NumCpy = Canberra::Serialization::VariantSerializer::Deserialize(Str, _Desc, Pos);	Pos+=NumCpy;
	NumCpy = Canberra::Serialization::VariantSerializer::Deserialize(Str, _Name, Pos);	Pos+=NumCpy;
	NumCpy = Canberra::Serialization::VariantSerializer::Deserialize(Str, _Step, Pos);	Pos+=NumCpy;

	Canberra::Utility::Core::Variant Val;
	_Enum.Clear();
	_EnumStrings.Clear();
	_EnumStringIds.Clear();
	for(LONG i=0; i<NumEnums; i++) {
		Val.Clear();
		NumCpy=Canberra::Serialization::VariantSerializer::Deserialize(Str, Val, Pos); Pos+=NumCpy;
		if (0 == i) {_Enum.PutArrayType(Val.GetType());}
		_Enum.Add(Val);
	}
	for(LONG i=0; i<NumEnums; i++) {
		Val.Clear();
		NumCpy=Canberra::Serialization::VariantSerializer::Deserialize(Str, Val, Pos); Pos+=NumCpy;
		if (0 == i) {_EnumStrings.PutArrayType(Val.GetType());}
		_EnumStrings.Add(Val);
	}
	for(LONG i=0; i<NumEnums; i++) {
		Val.Clear();
		NumCpy=Canberra::Serialization::VariantSerializer::Deserialize(Str, Val, Pos); Pos+=NumCpy;
		if (0 == i) {_EnumStringIds.PutArrayType(Val.GetType());}
		_EnumStringIds.Add(Val);
	}
	return Pos-Offset;
}
