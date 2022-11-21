#include "CommandData.h"
#include "ApplicationSerializer.h"
#include "SerializableType.h"
#include <memory.h>
#include <list>
typedef std::list< Canberra::Serialization::SerializableObject *> ArgumentList;

#ifndef _WIN32_WCE
	#include <assert.h>
	#ifdef _DEBUG
		#define ASSERT(Condition) assert(Condition)
	#else
		#define ASSERT(Condition) 0
	#endif
#else
#ifndef ASSERT
	#define ASSERT(Condition) 0
#endif
#endif

///<summary>
///An instance of this class is used to encapsulate all command
///information used for communicating with this application
///</summary>
///<exception cref="ULONG">The Win32 error code</exception>
Canberra::DataTypes::Application::CommandData::CommandData(void) :
	Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::CommandData) {
	memset(&_Data, 0, sizeof(_Data));
	_Args = new ArgumentList;
	if (!_Args) {
		throw (ULONG) E_OUTOFMEMORY;
	}
}

///<summary>
///An instance of this class is used to encapsulate all command
///information used for communicating with this application
///</summary>
///<param name="Code">The command code</param>
///<param name="Input">The input</param>
///<exception cref="ULONG">The Win32 error code</exception>
Canberra::DataTypes::Application::CommandData::CommandData(LONG Code, USHORT Input) :
	Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::CommandData) {
	memset(&_Data, 0, sizeof(_Data));
	_Type = Canberra::DataTypes::Serialization::SerializableObjects::CommandData;
	_Data.Code = Code;
	_Data.Input = Input;
	_Args = new ArgumentList;
	if (!_Args) {
		throw (ULONG) E_OUTOFMEMORY;
	}
}

Canberra::DataTypes::Application::CommandData::~CommandData(void) {
	ClearArguments();
	ArgumentList *Args = reinterpret_cast<ArgumentList*>(_Args);
	delete Args;
}


///<summary>
///This method will clear the argument list
///</summary>
void Canberra::DataTypes::Application::CommandData::ClearArguments(void) {
	ArgumentList *Args = reinterpret_cast<ArgumentList*>(_Args);
	for(ArgumentList::iterator It=(*Args).begin(); It != (*Args).end(); It++) {
		Canberra::Serialization::SerializableObject *Obj = *(It);
		delete Obj;
	}
	(*Args).clear();
}

///<summary>
///This method will return a reference to the argument
///This memory is NOT to be deleted
///</summary>
///<param id="Index">The argument index</param>
///<returns>The argument reference</returns>
Canberra::Serialization::SerializableObject *Canberra::DataTypes::Application::CommandData::GetArgument(const USHORT Index)
{
	ArgumentList *Args = reinterpret_cast<ArgumentList*>(_Args);
	USHORT Cnt=0;
	for(ArgumentList::iterator It=(*Args).begin(); It != (*Args).end(); It++) {
		if (Index == Cnt) return *(It); Cnt++;
	}
	return NULL;
}

///<summary>
///This method will return the number of arguments
///</summary>
///<returns>The number of arguments</returns>
USHORT Canberra::DataTypes::Application::CommandData::GetNumberOfArguments() {
	ArgumentList *Args = reinterpret_cast<ArgumentList*>(_Args);
	return (USHORT) (*Args).size();
}

///<summary>
///This method will add an argument to the argument list
///</summary>
void Canberra::DataTypes::Application::CommandData::AddArgument(Canberra::Serialization::SerializableObject *Obj) {

	//Serializing guarantees a deep copy is done
	Canberra::DataTypes::Serialization::ApplicationSerializer Serializer;
	Canberra::Utility::Core::ByteStream Stream;
	Serializer.Serialize(Obj, Stream);

	Canberra::Serialization::SerializableObject *ObjCopy = Serializer.Deserialize(Stream);
	if (NULL == ObjCopy) {
		DEBUG_ERROR_MESSAGE(L"CommandData", L"AddArgument", L"Serializer returned NULL for Deserialize method", E_FAIL);
		return;
	}

	//Add to the arg list
	ArgumentList *Args = reinterpret_cast<ArgumentList*>(_Args);
	(*Args).push_back(ObjCopy);

	_Data.NumArgs = (USHORT) (*Args).size();
}

///<summary>
///This method will returns the number of bytes this
///instance manages for serialization
///</summary>
///<returns>The number of bytes to serialize</returns>
ULONG Canberra::DataTypes::Application::CommandData::GetDataSize()
{
	LONG Size = sizeof(_Data);
	ArgumentList *Args = reinterpret_cast<ArgumentList*>(_Args);
	for(ArgumentList::iterator It=(*Args).begin(); It != (*Args).end(); It++) {
		Size += (*It)->GetSize();
	}
	return Size;
}

///<summary>
///This method will serialize this instance into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="MD">The metadata</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Application::CommandData::SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &, const LONG Offset)
{
	//Update the number of arguments
	ArgumentList *Args = reinterpret_cast<ArgumentList*>(_Args);
	_Data.NumArgs = (USHORT) (*Args).size();

	Canberra::DataTypes::Serialization::ApplicationSerializer Serializer;
	LONG Pos=Offset;
	LONG NumCpy=0;

	NumCpy=Serializer.Serialize(Canberra::Serialization::Long(_Data.Code), Str, Pos); Pos+=NumCpy;
	NumCpy=Serializer.Serialize(Canberra::Serialization::Short(_Data.Input), Str, Pos); Pos+=NumCpy;
	NumCpy=Serializer.Serialize(Canberra::Serialization::Short(_Data.NumArgs), Str, Pos); Pos+=NumCpy;
	for(ArgumentList::iterator It=(*Args).begin(); It != (*Args).end(); It++) {
		NumCpy = (*It)->Serialize(Str, Pos); Pos+=NumCpy;
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
ULONG Canberra::DataTypes::Application::CommandData::DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &, const LONG Offset)
{
	//Release old argument list
	ArgumentList *Args = reinterpret_cast<ArgumentList*>(_Args);
	if ((*Args).size() > 0) ClearArguments();

	//Deserialize the command information
	LONG Pos=Offset;
	LONG NumCpy=0;

	memcpy(&_Data.Code, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_Data.Code)); Pos+=NumCpy;
	SWAPNBYTES(&_Data.Code, sizeof(_Data.Code));
	memcpy(&_Data.Input, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_Data.Input)); Pos+=NumCpy;
	SWAPNBYTES(&_Data.Input, sizeof(_Data.Input));
	memcpy(&_Data.NumArgs, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_Data.NumArgs)); Pos+=NumCpy;
	SWAPNBYTES(&_Data.NumArgs, sizeof(_Data.NumArgs));

	//Deserialize each argument
	Canberra::DataTypes::Serialization::ApplicationSerializer Serializer;
	for(USHORT i=0; i<_Data.NumArgs; i++) {
		Canberra::Serialization::SerializableObject *Obj = Serializer.Deserialize(Str, Pos);
		if (NULL == Obj) {
			DEBUG_ERROR_MESSAGE(L"CommandData", L"DeserializeData", L"Serializer returned NULL for Deserialize method", E_FAIL);
			return Pos-Offset;
		}

		NumCpy = Obj->GetSize(); Pos+=NumCpy;
		(*Args).push_back(Obj);
	}
	return Pos-Offset;
}

LONG Canberra::DataTypes::Application::CommandData::GetCommandCode() {return _Data.Code;}
void Canberra::DataTypes::Application::CommandData::PutCommandCode(LONG v) {_Data.Code = v;}

USHORT Canberra::DataTypes::Application::CommandData::GetInput() {return _Data.Input;}
void Canberra::DataTypes::Application::CommandData::PutInput(USHORT v) {_Data.Input = v;}

