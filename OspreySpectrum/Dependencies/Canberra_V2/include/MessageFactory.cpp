#include "MessageFactory.h"
#include "MessageHeader.h"
#include "ApplicationSerializer.h"
#include "DebugMacros.h"
#include "DigitalSignature.h"

//Supported versions
LONG Canberra::DataTypes::Serialization::MessageFactory::MSG_VERSION_1 = 1;

///<summary>
///An instance of this class is used to create/interpret
///messages
///</summary>
Canberra::DataTypes::Serialization::MessageFactory::MessageFactory(void) : _Signature(false) {}
Canberra::DataTypes::Serialization::MessageFactory::~MessageFactory(void){}

///<summary>
///This method will create a message containing the specified data and serialize it
///to the stream
///</summary>
///<param name="Data">The data to put in the message</param>
///<param name="Str">The stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
void Canberra::DataTypes::Serialization::MessageFactory::SerializeToMessage(Canberra::Serialization::SerializableObject *Data, Canberra::Utility::Core::ByteStream &Str)
{
	Str.Clear();

	//Create the header
	MessageHeader Hdr(Data->GetSize() + _Signature.GetSize());
	if (Str.GetSize() < Hdr.GetSize()) Str.Allocate(Hdr.GetSize());


	//Write the data
	Data->Serialize(Str, Hdr.GetHeaderSize()+_Signature.GetSize());

	//Apply digital signature
	if (_Signature.GetEnableSigning()) _Signature.Sign(Str, Hdr.GetHeaderSize()+_Signature.GetSize());
	_Signature.Serialize(Str, Hdr.GetHeaderSize());

	//
	//Header is written last because checksum is
	//computed based on stream contents
	//
	Hdr.Serialize(Str);
	//DEBUG_MESSAGE(L"MessageFactory", L"SerializeToMessage", Canberra::Utility::Core::String(L"Message size is: ") + Canberra::Utility::Core::String(Hdr.GetSize()));
	//DEBUG_MESSAGE(L"MessageFactory", L"SerializeToMessage", Canberra::Utility::Core::String(L"Stream size is: ") + Canberra::Utility::Core::String(Str.GetSize()));
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
Canberra::Serialization::SerializableObject *Canberra::DataTypes::Serialization::MessageFactory::DeserializeFromMessage(Canberra::Utility::Core::ByteStream &Str)
{
	//Extract the header
	MessageHeader Hdr;
	LONG Offset=Hdr.Deserialize(Str);

	//Check to see if this version is supported
	if (!SupportsMsgVersion(Hdr.GetVersion())) {
		DEBUG_ERROR_MESSAGE(L"MessageFactory", L"DeserializeFromMessage",
							Canberra::Utility::Core::String("Message version is not supported.  Received message with version: ") +
							Canberra::Utility::Core::String(Hdr.GetVersion()), GEN_UNSUPPORTED_COMMVERSION);
		throw GEN_UNSUPPORTED_COMMVERSION;
	}

	//Check signature
	DigitalSignature DS(false);
	Offset += DS.Deserialize(Str, Hdr.GetHeaderSize());
	ULONG Stat = DS.Verify(Str, Hdr.GetHeaderSize()+DS.GetSize());
	if (Stat) throw Stat;

	//Have the app deserialize the data
	ApplicationSerializer Serializer;
	return Serializer.Deserialize(Str, Offset);
}

///<summary>
///This method will return information indicating whether
///the message version is supported
///</summary>
///<param name="Version">The message version number</param>
///<returns>The support state</returns>
bool Canberra::DataTypes::Serialization::MessageFactory::SupportsMsgVersion(const LONG Version) {
	return (MSG_VERSION_1 == Version);
}

///<summary>
///Accessor to enable signing
///<exceptions cref="ULONG">Win32 error code </exceptions>
///</summary>
bool Canberra::DataTypes::Serialization::MessageFactory::GetDigitallySign() { return _Signature.GetEnableSigning();}
void Canberra::DataTypes::Serialization::MessageFactory::PutDigitallySign(bool v) {_Signature.PutEnableSigning(v);}
