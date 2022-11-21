#include "DebugMacros.h"
#include "ApplicationSerializer.h"
#include "SerializableType.h"
#include "Parameter.h"
#include "MetaData.h"
#include <memory.h>

///<summary>
///This method will perform a deep copy
///</summary>
///<param name="v">The source</param>
void Canberra::DataTypes::Application::Parameter::Copy(const Canberra::DataTypes::Application::Parameter &v) {
	_ParameterCode = v._ParameterCode;
	_Value = v._Value;
}


///<summary>
///This method will overide the equals operator
///to perform a deep copy
///</summary>
Canberra::DataTypes::Application::Parameter& Canberra::DataTypes::Application::Parameter::operator=( const Parameter &Rhs) {Copy(Rhs); return *this;}


///<summary>
///This method will initialize an instance of this class.  An instance of this
///class is used to define a parameter (code, value) pair
///</summary>
///<param name="v">The source</param>
Canberra::DataTypes::Application::Parameter::Parameter(const Parameter &v) : Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::ParameterData) {Copy(v);}

///<summary>
///This method will initialize an instance of this class.   An instance of this
///class is used to define a parameter (code, value) pair
///</summary>
Canberra::DataTypes::Application::Parameter::Parameter() :  Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::ParameterData) {}

///<summary>
///This method will initialize an instance of this class.   An instance of this
///class is used to define a parameter (code, value) pair
///</summary>
///<param name="C">The parameter code</param>
///<param name="Val">The parameter value</param>
Canberra::DataTypes::Application::Parameter::Parameter(LONG C, const Canberra::Utility::Core::Variant &Val) :
				Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::ParameterData),
				_ParameterCode(C), _Value(Val) {_Type = Canberra::DataTypes::Serialization::SerializableObjects::ParameterData;}
Canberra::DataTypes::Application::Parameter::~Parameter() {}

//Accessors
ULONG Canberra::DataTypes::Application::Parameter::GetDataSize() {return Canberra::Serialization::VariantSerializer::GetSize(_Value) + sizeof(_ParameterCode);}
LONG Canberra::DataTypes::Application::Parameter::GetParameterCode() {return _ParameterCode;}
void Canberra::DataTypes::Application::Parameter::PutParameterCode(LONG v) {_ParameterCode = v;}
const Canberra::Utility::Core::Variant &Canberra::DataTypes::Application::Parameter::GetValue() {return _Value;}
void Canberra::DataTypes::Application::Parameter::PutValue(const Canberra::Utility::Core::Variant &v) {_Value=v;}

///<summary>
///This method will serialize this instance into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="MD">The metadata</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Application::Parameter::SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &, const LONG Offset)
{
	Canberra::DataTypes::Serialization::ApplicationSerializer Serializer;
	LONG Pos=Offset;
	LONG NumCpy=0;
	NumCpy = Serializer.Serialize(Canberra::Serialization::Ulong(_ParameterCode), Str, Pos); Pos+=NumCpy;
	NumCpy = Canberra::Serialization::VariantSerializer::Serialize(Str, _Value, Pos); Pos+=NumCpy;
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
ULONG Canberra::DataTypes::Application::Parameter::DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &, const LONG Offset)
{
	Canberra::DataTypes::Serialization::ApplicationSerializer Serializer;
	LONG Pos=Offset;
	LONG NumCpy=0;

	memcpy(&_ParameterCode, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_ParameterCode)); Pos+=NumCpy;
	SWAPNBYTES(&_ParameterCode, sizeof(_ParameterCode));
	NumCpy = Canberra::Serialization::VariantSerializer::Deserialize(Str, _Value, Pos);	Pos+=NumCpy;
	return Pos-Offset;
}
