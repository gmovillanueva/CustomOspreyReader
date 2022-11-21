#include "ApplicationSerializer.h"
#include "Serializer.h"
#include "SerializableType.h"
#include "DebugMacros.h"
#include "MetaData.h"
#include <memory.h>

#include "DlfcData.h"
#include "McsData.h"
#include "PhaData.h"
#include "ListData.h"
#include "ParameterMetaData.h"
#include "RegionOfInterest.h"
//#include "DsoData.h"
#include "CounterData.h"
#include "ScaData.h"

#include "CommandData.h"
#include "Parameter.h"

///<summary>
///This method will initialize this instance.  This class
///is a utility class used to serialize native data types
///</summary>
Canberra::DataTypes::Serialization::ApplicationSerializer::ApplicationSerializer() {}
Canberra::DataTypes::Serialization::ApplicationSerializer::~ApplicationSerializer() {}

///<summary>
///This method will serialize the application data type into the
///stream.
///</summary>
///<param name="Val">The value to serialize</param>
///<param name="Str">The stream</param>
///<param name="Offset">The offset to start reading the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Serialization::ApplicationSerializer::Serialize(const Canberra::Serialization::SerializableObject &V, Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	Canberra::Serialization::SerializableObject &Val = const_cast<Canberra::Serialization::SerializableObject &>(V);
	ULONG Size=0;

	//Native types will not contain the metadata
	if (Val.GetType() < Canberra::Serialization::SerializableObject::CustomTypeOffset) {
		Val.PutUseMetaData(false);
		Size=Val.Serialize(Str, Offset);
		Val.PutUseMetaData(true);
	}
	else {
		Size=Val.Serialize(Str, Offset);
	}
	return Size;
}

///<summary>
///This method will serialize the application data type into the
///stream.
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The offset to start reading the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
void Canberra::DataTypes::Serialization::ApplicationSerializer::Serialize(Canberra::Serialization::SerializableObject *Data, Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	//Call base class implementation.
	Serializer::Serialize(Data, Str, Offset);
}

///<summary>
///This method will deserialize the stream into one of this
///applications data types.  The pointer that is returns
///must be free'd by the caller
///</summary>
///<param name="Val">The value to deserialize from the stream</param>
///<param name="Str">The stream</param>
///<param name="Offset">The offset to start reading the stream</param>
///<returns>The number of bytes deserialized</returns>
///<exception cref="ULONG">The Win32 error code</exception>
ULONG Canberra::DataTypes::Serialization::ApplicationSerializer::Deserialize(Canberra::Serialization::SerializableObject &Val, Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	ULONG Size=0;

	//Native types will not contain the metadata
	if (Val.GetType() < Canberra::Serialization::SerializableObject::CustomTypeOffset) {
		Val.PutUseMetaData(false);
		Size = Val.Deserialize(Str, Offset);
		Val.PutUseMetaData(true);
	}
	else {
		Size = Val.Deserialize(Str, Offset);
	}
	return Size;
}
///<summary>
///This method will deserialize the stream into one of this
///applications data types.  The pointer that is returns
///must be free'd by the caller
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The offset to start reading the stream</param>
///<returns>The deserialized data</returns>
///<exception cref="ULONG">The Win32 error code</exception>
Canberra::Serialization::SerializableObject *Canberra::DataTypes::Serialization::ApplicationSerializer::Deserialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	//Extract the metadata to determine the type to create
	Canberra::Serialization::MetaData Hdr;
	Hdr.Deserialize(Str, Offset);

	//Handle this applications data types
	Canberra::Serialization::SerializableObject *Data=NULL;
	switch(Hdr.GetDataType()) {
		case SerializableObjects::DlfcData:
			Data = new Canberra::DataTypes::Spectroscopy::DlfcData();
			break;
		case SerializableObjects::McsData:
			Data = new Canberra::DataTypes::Spectroscopy::McsData();
			break;
		case SerializableObjects::PhaData:
			Data = new Canberra::DataTypes::Spectroscopy::PhaData();
			break;
		case SerializableObjects::CommandData:
			Data = new Canberra::DataTypes::Application::CommandData();
			break;
		case SerializableObjects::ParameterData:
			Data = new Canberra::DataTypes::Application::Parameter();
			break;
		case SerializableObjects::SpectralData:
			Data = new Canberra::DataTypes::Spectroscopy::Spectrum(true);
			break;
		case SerializableObjects::RegionOfInterestData:
			Data = new Canberra::DataTypes::Spectroscopy::RegionOfInterest();
			break;
		case SerializableObjects::DsoData:
			//Data = new DsoData();
			break;
		case SerializableObjects::CounterData:
			Data = new Canberra::DataTypes::Spectroscopy::CounterData();
			break;
		case SerializableObjects::ListData:
			Data = new Canberra::DataTypes::Spectroscopy::ListData();
			break;
		case SerializableObjects::TlistData:
			Data = new Canberra::DataTypes::Spectroscopy::TlistData();
			break;
		case SerializableObjects::ParameterMetaData:
			Data = new Canberra::DataTypes::Application::ParameterMetaData();
			break;
		case SerializableObjects::SCAbufferData:
			Data = new Canberra::DataTypes::Spectroscopy::SCAbuffer();
			break;
		case SerializableObjects::SCAdefinitionData:
			Data = new Canberra::DataTypes::Spectroscopy::SCAdefinitions();
			break;
		default:
			//Call base class implementation.  Handles native data types
			Canberra::Serialization::SerializableObject *Data=Serializer::Deserialize(Str, Offset);
			if (Data) return Data;
			break;
	}
	Data->Deserialize(Str, Offset);
	return Data;
}



