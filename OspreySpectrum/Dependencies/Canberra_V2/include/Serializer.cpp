#include "Serializer.h"
#include "MetaData.h"
#include "SerializableType.h"
#include "DebugMacros.h"

///<summary>
///This method will initialize this instance.  This class
///is a utility class used to serialize native data types
///</summary>
Canberra::Serialization::Serializer::Serializer() {}
Canberra::Serialization::Serializer::~Serializer() {}


///<summary>
///This method will serialize the application data type into the
///stream.
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The offset to start reading the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
void Canberra::Serialization::Serializer::Serialize(SerializableObject *Data, Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	Data->Serialize(Str, Offset);
}

///<summary>
///This method will deserialize the message and extract the data from
///it.  The data type contained in the message is created and
///returned to the caller.  The caller is responsible for releasing
///this memory
///</summary>
///<param name="Str">The stream</param>
///<returns>The deserialized data</returns>
///<exception cref="ULONG">The Win32 error code</exception>
Canberra::Serialization::SerializableObject *Canberra::Serialization::Serializer::Deserialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	//Extract the metadata to determine the type to create
	MetaData Hdr;
	Hdr.Deserialize(Str, Offset);

	//Create the specific type
	SerializableObject *Data=NULL;
	switch(Hdr.GetDataType()) {
		case SerializableObject::Byte:
			Data = new Byte;
			break;
		case SerializableObject::Ubyte:
			Data = new Ubyte;
			break;
		case SerializableObject::Short:
			Data = new Short;
			break;
		case SerializableObject::Ushort:
			Data = new Ushort;
			break;
		case SerializableObject::Long:
			Data = new Long;
			break;
		case SerializableObject::Ulong:
			Data = new Ulong;
			break;
		case SerializableObject::Float:
			Data = new Float;
			break;
		case SerializableObject::Double:
			Data = new Double;
			break;
		case SerializableObject::String:
			Data = new Wstring;
			break;
		case SerializableObject::Blob:
			Data = new Blob;
			break;
		default:
			DEBUG_ERROR_MESSAGE(L"Serializer", L"Deserialize", L"Unknown data type", TYPE_E_UNDEFINEDTYPE);
			throw (ULONG) TYPE_E_UNDEFINEDTYPE;
	}

	//Intentionally not incrementing offset because SerializableObject::Deserialize looks for the metadata
	Data->Deserialize(Str, Offset);
	return Data;
}



