#include "Device.h"
#include "Parameter.h"
#include "CommandData.h"
#include "PhaData.h"
#include "McsData.h"
#include "CounterData.h"
#include "ListData.h"
#include "SerializableObject.h"
#include "SerializableType.h"
#include "pbsmsgs_h.h"
#include "ApplicationSerializer.h"
#include "LockManager.h"
#include "StreamChannel.h"
#include "endianUtils.h"
#ifdef WIN32
#include <wininet.h>
#else
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#endif

using namespace Canberra::Utility::Network;
using namespace Canberra::Serialization;
using namespace Canberra::Utility::Core;
using namespace Canberra::DataTypes::Application;
using namespace Canberra::DataTypes::Spectroscopy;
using namespace Canberra::DataTypes::Serialization;
using namespace Canberra::DataTypes::Lynx;

///<summary>
///An instance of this class contains Lynx version 1.1
///modifications and provides an interface,_Data, for the next
///version as well.
///</summary>
class Extensions {
protected:
	Canberra::Protocols::Lynx::StreamChannel *_StreamChannel;			//Class for accessing the streaming interface for Lynx
	void *_Data;															//Data for next version
	Canberra::Protocols::Lynx::ChannelSettings _ConfigChanSettings;	//Socket settings for the config channel
	Canberra::Utility::Core::CriticalSection _ConfigChanLock;				//A lock for the config channel

public:
	Extensions(MessageFactory &MF, USHORT Port) :
		_StreamChannel(new Canberra::Protocols::Lynx::StreamChannel(MF)), _Data(NULL),
		_ConfigChanSettings(Port), _ConfigChanLock(String(L"ConfigChannelLock")) {}
	virtual ~Extensions() {_StreamChannel->Close();delete _StreamChannel;_StreamChannel = NULL;}

	//Accessor to the streaming interface
	Canberra::Protocols::Lynx::StreamChannel &GetStreamingInterface() { return *_StreamChannel;}

	//Accessor to the config channel settings
	Canberra::Protocols::Lynx::ChannelSettings &GetConfigurationChannelSettings() { return _ConfigChanSettings;}

	//Accessor to the config channel lock
	Canberra::Utility::Core::CriticalSection &GetConfigurationChannelLock() { return _ConfigChanLock;}

	//Accessor to the next versions data
	void *GetVersionDataPtr() { return _Data;}

};


///<summary>
///This is the constructor for a list type that contains
///instances of spectral data.  It is specifically used
///for MSS
///</summary>
Canberra::Protocols::Lynx::SpectralDataList::SpectralDataList() : _ListData(new std::vector<SpectralData *>) {
}
Canberra::Protocols::Lynx::SpectralDataList::~SpectralDataList() {
	Clear();
	if (_ListData) {
		std::vector<SpectralData *> *List = reinterpret_cast<std::vector<SpectralData *> *>(_ListData);
		delete List;
		_ListData=NULL;
	}
}

///<summary>
///This method clears the spectral data from the list
///</summary>
void Canberra::Protocols::Lynx::SpectralDataList::Clear() {
	if (!_ListData) return;

	std::vector<SpectralData *> *List = reinterpret_cast<std::vector<SpectralData *> *>(_ListData);
	for(size_t i=0; i<List->size(); i++) {
		delete List->at(i);
	}
	List->clear();
}

///<summary>
///This method adds spectral data to the list
///</summary>
///<param name="Data">The data to add</param>
void Canberra::Protocols::Lynx::SpectralDataList::Add(SpectralData *Data) {
	if (!_ListData) return;
	std::vector<SpectralData *> *List = reinterpret_cast<std::vector<SpectralData *> *>(_ListData);
	List->push_back(Data);
}

///<summary>
///This method gets spectral data from the list at
///the specified index.  If the index is out of bounds
///NULL will be returned.
///</summary>
///<param name="i">The index</param>
///<returns>The data</returns>
SpectralData * Canberra::Protocols::Lynx::SpectralDataList::GetData(LONG i) {
	if (!_ListData) return NULL;
	std::vector<SpectralData *> *List = reinterpret_cast<std::vector<SpectralData *> *>(_ListData);
	if ((i < 0) || ((size_t) i >= List->size())) return NULL;
	return List->at(i);
}


///<summary>
///This method gets number of items in the
///list
///</summary>
///<returns>The size of the list</returns>
ULONG Canberra::Protocols::Lynx::SpectralDataList::GetSize() {
	if (!_ListData) return 0;
	std::vector<SpectralData *> *List = reinterpret_cast<std::vector<SpectralData *> *>(_ListData);
	return List->size();
}

///<summary>
///This method will encrypt the supplied string
///</summary>
///<param name="v">The string to encrypt</param>
///<returns>The encrypted string</returns>
ULONG Encrypt(const String &in, String &out)
{
	out.Clear();
	out = in;
	return 0;
}

///<summary>
///This method will create and instance of this class.  An instance of this class may be used to send and receive information
///to and from a DSA 3K device
///</summary>
Canberra::Protocols::Lynx::Device::Device(void) : _ConfigChannel(String(L"DSA Data Comm")), _Data(new Extensions(_MessageFactory, 16385)) {
	_ConfigChannel.GetAddress().PutPort(16385);
	Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
	if (Ext) {Ext->GetConfigurationChannelSettings().Port = _ConfigChannel.GetAddress().GetPort();}
	else {
		DEBUG_ERROR_MESSAGE(L"Device", L"ctor", "Error allocating _Data.  Pointer is NULL", E_OUTOFMEMORY);
		throw (ULONG) E_OUTOFMEMORY;
	}
}
Canberra::Protocols::Lynx::Device::~Device(void) {
	Close();
	Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
	if (Ext) {delete Ext; _Data=NULL;}
}

///<summary>
///This method will open a connection with the device
///This is a blocking call until the connection is made with the
///device
///</summary>
///<param name="Client">The client address</param>
///<param name="Device">The device address</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::Open(const String &Client, const String &Device)
{
	ULONG Stat=0;

	Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
	LockManager LM(&(Ext->GetConfigurationChannelLock()));
	if (Ext->GetConfigurationChannelSettings().Port != ((USHORT) -1)) {
		USHORT Port = Ext->GetConfigurationChannelSettings().Port;
		Stat=_ConfigChannel.Open(EndPoint(Client, 0));
		if (!Stat) {
			//Want blocking mode
			_ConfigChannel.PutIsBlocking(true);

			//Setup the channel (Note: It is not fatal if there are any errors with these calls.  Hence, not returned to caller)
			LONG Size=sizeof(Ext->GetConfigurationChannelSettings().SendTimeOut);
#ifdef WIN32
			Stat=_ConfigChannel.SetOption(SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char *>(&Ext->GetConfigurationChannelSettings().SendTimeOut), Size);
			if (Stat) {DEBUG_ERROR_MESSAGE(L"Device", L"Open", L"Error setting SO_SNDTIMEO", Stat);}
			Size=sizeof(Ext->GetConfigurationChannelSettings().RecvTimeOut);
			Stat=_ConfigChannel.SetOption(SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&Ext->GetConfigurationChannelSettings().RecvTimeOut), Size);
			if (Stat) {DEBUG_ERROR_MESSAGE(L"Device", L"Open", L"Error setting SO_RCVTIMEO", Stat);}
#endif
			Size=sizeof(Ext->GetConfigurationChannelSettings().SendBufferSize);
			Stat=_ConfigChannel.SetOption(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char *>(&Ext->GetConfigurationChannelSettings().SendBufferSize), Size);
			if (Stat) {DEBUG_ERROR_MESSAGE(L"Device", L"Open", L"Error setting SO_SNDBUF", Stat);}
			Size=sizeof(Ext->GetConfigurationChannelSettings().RecvBufferSize);
			Stat=_ConfigChannel.SetOption(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&Ext->GetConfigurationChannelSettings().RecvBufferSize), Size);
			if (Stat) {DEBUG_ERROR_MESSAGE(L"Device", L"Open", L"Error setting SO_RCVBUF", Stat);}
			Size=sizeof(Ext->GetConfigurationChannelSettings().NoDelay);
			Stat=_ConfigChannel.SetOption(IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&Ext->GetConfigurationChannelSettings().NoDelay), Size);
			if (Stat) {DEBUG_ERROR_MESSAGE(L"Device", L"Open", L"Error setting TCP_NODELAY", Stat);}
			Size=sizeof(Ext->GetConfigurationChannelSettings().KeepAlive);
			Stat=_ConfigChannel.SetOption(SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char *>(&Ext->GetConfigurationChannelSettings().KeepAlive), Size);
			if (Stat) {DEBUG_ERROR_MESSAGE(L"Device", L"Open", L"Error setting TCP_KEEPAlIVE", Stat);}

			Stat = _ConfigChannel.Connect(EndPoint(Device, Port));
			if (Stat) {
				//Intentionally ignoring error code.  Debug output window will display it.
				_ConfigChannel.Close();
			}
			else {
				Stat = Ext->GetStreamingInterface().Open(Client ,Device);
			}
		}
		if (Stat) {
			DEBUG_ERROR_MESSAGE(L"Device", L"Open", L"Error connecting to device for performing configuration.", Stat);
			return Stat;
		}

	}


	return Stat;
}

///<summary>
///This method will close the connection with the device
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::Close()
{
	Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
	LockManager LM(&(Ext->GetConfigurationChannelLock()));
	if (_ConfigChannel.GetIsOpen()) {
		_ConfigChannel.Shutdown(SD_BOTH);
		_ConfigChannel.Close();
	}
	Ext->GetStreamingInterface().Close();
	return 0;
}


///<summary>
///This method will get the parameter meta data
///</summary>
///<param name="Code">The parameter code</param>
///<param name="Val">The meta data</param>
///<param name="Input">The input to get it from</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::GetParameterMetaData(LONG Code, ParameterMetaData &PD, USHORT Input)
{
	//Send the command
	CommandData *Resp=NULL;
	CommandData Cmd(CommandCodes::GetParameter, Input);
	Ulong ParamCode(Code);
	Cmd.AddArgument(&ParamCode);
	Ulong Option(1);
	Cmd.AddArgument(&Option);

	ULONG Stat = Control(Cmd, &Resp);
	{
	if (Stat) goto EXIT;

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

	if (!Resp->GetNumberOfArguments()) {
		Stat = GEN_INVALID_RESPONSE;
		DEBUG_ERROR_MESSAGE(L"Device", L"GetParameterMetaData", L"Error device response is invalid.", Stat);
		goto EXIT;
	}

	ParameterMetaData *Param  = dynamic_cast<ParameterMetaData *>(Resp->GetArgument(0));
	if (!Param) {
		Stat = GEN_INVALID_RESPONSE;
		DEBUG_ERROR_MESSAGE(L"Device", L"GetParameterMetaData", L"Error device response is invalid. Argument is not a ParameterMetaData type.", Stat);
		goto EXIT;
	}

	//Got the value
	PD.Copy(*Param);
	}
EXIT:
	if (Resp) delete Resp;
	return Stat;
}

///<summary>
///This method will get a parameter from the device
///</summary>
///<param name="Code">The parameter code</param>
///<param name="Val">The value</param>
///<param name="Input">The input to get it from</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::GetParameter(LONG Code, Variant &Val, USHORT Input)
{
	Val.Clear();

	//Send the command
	CommandData *Resp=NULL;
	CommandData Cmd(CommandCodes::GetParameter, Input);
	Ulong ParamCode(Code);
	Cmd.AddArgument(&ParamCode);

	ULONG Stat = Control(Cmd, &Resp);
	{
	if (Stat) goto EXIT;

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

	if (!Resp->GetNumberOfArguments()) {
		Stat = GEN_INVALID_RESPONSE;
		DEBUG_ERROR_MESSAGE(L"Device", L"GetParameter", L"Error device response is invalid.", Stat);
		goto EXIT;
	}

	Parameter *Param  = dynamic_cast<Parameter *>(Resp->GetArgument(0));
	if (!Param) {
		Stat = GEN_INVALID_RESPONSE;
		DEBUG_ERROR_MESSAGE(L"Device", L"GetParameter", L"Error device response is invalid. Argument is not a Parameter type.", Stat);
		goto EXIT;
	}

	//Got the value
	Val = Param->GetValue();
	}
EXIT:
	if (Resp) delete Resp;
	return Stat;
}


///<summary>
///This method will get a list of parameters from the device
///</summary>
///<param name="Code">The parameter codes</param>
///<param name="Val">The values</param>
///<param name="Input">The input to get it from</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::GetParameterList(std::vector<LONG> &Codes, std::vector<Variant> &Val, USHORT Input)
{
	Val.clear();

	//Send the command
	CommandData *Resp=NULL;
	CommandData Cmd(CommandCodes::GetParameterList, Input);
	for(size_t p=0; p<Codes.size(); p++) {
		Ulong ParamCode(Codes[p]);
		Cmd.AddArgument(&ParamCode);
	}

	ULONG Stat = Control(Cmd, &Resp);
	if (Stat) goto EXIT;

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

	if (!Resp->GetNumberOfArguments()) {
		Stat = GEN_INVALID_RESPONSE;
		DEBUG_ERROR_MESSAGE(L"Device", L"GetParameterList", L"Error device response is invalid.", Stat);
		goto EXIT;
	}

	for(USHORT i=0; i<Resp->GetNumberOfArguments(); i++) {
		Parameter *Param  = dynamic_cast<Parameter *>(Resp->GetArgument(i));
		if (!Param) {
			Stat = GEN_INVALID_RESPONSE;
			DEBUG_ERROR_MESSAGE(L"Device", L"GetParameterList", L"Error device response is invalid. Argument is not a Parameter type.", Stat);
			goto EXIT;
		}
		//Got the value
		Val.push_back(Param->GetValue());
	}

EXIT:
	if (Resp) delete Resp;
	return Stat;
}

///<summary>
///This method will set a parameter into the device
///</summary>
///<param name="Code">The parameter code</param>
///<param name="Val">The value</param>
///<param name="Input">The input to set it into</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::PutParameter(LONG Code, const Canberra::Utility::Core::Variant &Val, USHORT Input)
{
	//Send the command
	CommandData *Resp=NULL;
	CommandData Cmd(CommandCodes::PutParameter, Input);
	Parameter Par((ParameterCodes::Code) Code, Val);
	Cmd.AddArgument(&Par);

	ULONG Stat = Control(Cmd, &Resp);
	if (Stat) goto EXIT;

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

EXIT:
	if (Resp) delete Resp;
	return Stat;
}


///<summary>
///This method will set a list of parameters into the device
///</summary>
///<param name="Code">The parameter codes</param>
///<param name="Val">The values</param>
///<param name="Input">The input to set it into</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::PutParameterList(std::vector<LONG> &Codes, std::vector<Canberra::Utility::Core::Variant> &Val, USHORT Input)
{
	//Validate the we have the same list sizes
	if (Codes.size() != Val.size()) {
		ULONG Stat = (ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(L"Device", L"PutParameterList", L"Error Codes.size() != Vals.size().", Stat);
		return Stat;
	}

	//Send the command
	CommandData *Resp=NULL;
	CommandData Cmd(CommandCodes::PutParameterList, Input);
	for(size_t p=0; p<Codes.size(); p++) {
		Parameter Par((ParameterCodes::Code) Codes[p], Val[p]);
		Cmd.AddArgument(&Par);
	}

	ULONG Stat = Control(Cmd, &Resp);
	if (Stat) goto EXIT;

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

EXIT:
	if (Resp) delete Resp;
	return Stat;
}

///<summary>
///This method will execute a control command on a specified input
///The assumption is that the device response contains no return data
///</summary>
///<param name="Code">The parameter code</param>
///<param name="Input">The input to get it from</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::Control(LONG Code, USHORT Input)
{
	CommandData Cmd(static_cast<CommandCodes::Code>(Code), Input);
	return Control(Cmd);
}


///<summary>
///This method will execute a control command on a specified input
///</summary>
///<param name="Code">The parameter code</param>
///<param name="Input">The input to get it from</param>
///<param name="In">The command arguments</param>
///<param name="Out">The command results</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::Control(LONG Code, USHORT Input, Canberra::Utility::Core::ByteStream &In, Canberra::Utility::Core::ByteStream &Out)
{
	Out.Clear();
	CommandData Cmd(static_cast<CommandCodes::Code>(CommandCodes::DriverControl), Input);
	Ulong DriverCode(Code);
	Cmd.AddArgument(&DriverCode);
	Canberra::Serialization::Blob *Data = NULL;
	CommandData *Resp=NULL;
	ULONG Stat=0;

	if (In.GetSize()) {
		try {
			Canberra::Serialization::Blob Val(In);
			Cmd.AddArgument(&Val);
		}
		CATCHTRACE(L"Device", L"Control", Stat);
		if (Stat) goto EXIT;
	}

	Stat=Control(Cmd, &Resp);
	if (Stat) goto EXIT;

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

	Data = dynamic_cast<Canberra::Serialization::Blob *>(Resp->GetArgument(0));
	if (Data) {
		try {
			Data->CopyTo(Out);
		}
		CATCHTRACE(L"Device", L"Control", Stat);
		if (Stat) goto EXIT;
	}

EXIT:
	if (Resp) delete Resp;
	return Stat;
}

///<summary>
///This method will execute a control command using the command data
///The assumption is that the device response contains no return data
///</summary>
///<param name="Cmd">The command</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::Control(CommandData &Cmd)
{
	CommandData *Resp=NULL;
	ULONG Stat=Control(Cmd, &Resp);
	if (Stat) goto EXIT;

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

EXIT:
	if (Resp) delete Resp;
	return Stat;
}

///<summary>
///This method will locks an input.
///</summary>
///<param name="User">The user name</param>
///<param name="Pwd">The password</param>
///<param name="Input">The input</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::Lock(const Canberra::Utility::Core::String &User, const Canberra::Utility::Core::String &Pwd, USHORT Input)
{
	//Send the command
	CommandData Cmd(CommandCodes::Lock, Input);
	String Passwd;
	ULONG Stat=Encrypt(Pwd, Passwd);
	if (Stat) return Stat;
	Wstring UserN(const_cast<Canberra::Utility::Core::String &>(User)),
			Password(Passwd),
			Mac(_ConfigChannel.GetAddress().GetAddressString());
	Cmd.AddArgument(&UserN);
	Cmd.AddArgument(&Password);
	Cmd.AddArgument(&Mac);
	return Control(Cmd);
}

///<summary>
///This method will unlocks an input
///</summary>
///<param name="User">The user name</param>
///<param name="Pwd">The password</param>
///<param name="Input">The input</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::Unlock(const Canberra::Utility::Core::String &User, const Canberra::Utility::Core::String &Pwd, USHORT Input)
{
	//Send the command
	CommandData Cmd(CommandCodes::Unlock, Input);
	String Passwd;
	ULONG Stat=Encrypt(Pwd, Passwd);
	if (Stat) return Stat;
	Wstring UserN(const_cast<Canberra::Utility::Core::String &>(User)),
			Password(Passwd),
			Mac(_ConfigChannel.GetAddress().GetAddressString());
	Cmd.AddArgument(&UserN);
	Cmd.AddArgument(&Password);
	Cmd.AddArgument(&Mac);
	return Control(Cmd);
}

///<summary>
///This method will save the spectrum and return
///the specified filename
///</summary>
///<param name="Input">The input to get it from</param>
///<param name="Name">The name of the file on the device</param>
///<param name="Group">The memory group to save 0==Use current memory group</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::Save(USHORT Input, USHORT Group, Canberra::Utility::Core::String &Name)
{
	Name.Clear();

	//Send the command
	CommandData *Resp=NULL;
	CommandData Cmd(CommandCodes::Save, Input);
	Ushort Grp(Group);
	Cmd.AddArgument(&Grp);
	ULONG Stat=Control(Cmd, &Resp);
	{
	if (Stat) goto EXIT;

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

	if (!Resp->GetNumberOfArguments()) {
		Stat = GEN_INVALID_RESPONSE;
		DEBUG_ERROR_MESSAGE(L"Device", L"Save", L"Error received from the device.", Stat);
		goto EXIT;
	}
	Wstring *Res  = dynamic_cast<Wstring *>(Resp->GetArgument(0));
	if (!Res) {
		Stat = GEN_INVALID_RESPONSE;
		DEBUG_ERROR_MESSAGE(L"Device", L"Save", L"Error received from the device.", Stat);
		goto EXIT;
	}

	Name=Res->GetValue();
	}
EXIT:
	if (Resp) delete Resp;
	return Stat;
}


///<summary>
///This method will execute a control command using the command data and
///returns the command response data
///</summary>
///<param name="Cmd">The command</param>
///<param name="Resp">The command response</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::Control(CommandData &Cmd, CommandData **Resp)
{
	ULONG Stat=0;
	try {
		//Send the command
		ByteStream Str;
		ULONG Size=0;

		Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
		LockManager LM(&(Ext->GetConfigurationChannelLock()));

		_MessageFactory.SerializeToMessage(&Cmd, Str);
		ULONG Stat = _ConfigChannel.Send(Str, &Size);
		if (Stat || !Size) {
			if (!Stat) Stat = WSAECONNRESET;
			DEBUG_ERROR_MESSAGE(L"Device", L"Control", L"Error while sending device command.", Stat);
			return Stat;
		}

		ULONG MsgSize=0;
		Str.Clear();
		Str.Allocate(sizeof(MsgSize));

		//Get the size of the response message (1'st 4 bytes)
		Stat = _ConfigChannel.Receive(Str, &Size);
		if (Stat || !Size) {
			if (!Stat) Stat = WSAECONNRESET;
			DEBUG_ERROR_MESSAGE(L"Device", L"Control", L"Error while receiving device response header.", Stat);
			return Stat;
		}

		//Allocate memory for the rest of the message and read
		memcpy(&MsgSize, Str.GetBuffer(), sizeof(MsgSize));
		SWAPNBYTES(&MsgSize, 4);
		Canberra::Utility::Core::ByteStream Frag;
		LONG PSize=MsgSize-sizeof(MsgSize);
		if (PSize < 0) {
			Stat = (ULONG) E_FAIL;
			DEBUG_ERROR_MESSAGE(L"Device", L"Control", L"Error while receiving packet size.  Size is < 1", Stat);
			return Stat;
		}

		Frag.Allocate(PSize);
		Stat = _ConfigChannel.Receive(Frag, &Size);
		if (Stat || !Size) {
			if (!Stat) Stat = WSAECONNRESET;
			DEBUG_ERROR_MESSAGE(L"Device", L"Control", L"Error while receiving device response body.", Stat);
			return Stat;
		}

		Stat=0;
		SerializableObject *Obj=NULL;
		Str.Copy(Frag, 0, sizeof(MsgSize));
		try {
			Obj = _MessageFactory.DeserializeFromMessage(Str);
			*Resp = dynamic_cast<CommandData *>(Obj);
		}
		CATCHTRACE(L"Device", L"Control", Stat);

		if (Stat) {
			delete Obj;
			Obj=NULL;
			return Stat;
		}
		if (NULL == *Resp) {
			delete Obj;
			Obj = NULL;
			Stat = GEN_INVALID_RESPONSE;
			DEBUG_ERROR_MESSAGE(L"Device", L"Control", L"Error device response is invalid.", Stat);
		}
		if (CommandCodes::Response != (*Resp)->GetCommandCode()) {
			delete Obj;
			Obj = NULL;
			Stat = GEN_INVALID_RESPONSE;
			DEBUG_ERROR_MESSAGE(L"Device", L"Control", L"Error device response is invalid.", Stat);
		}
	}
	CATCHTRACE(L"Device", L"Control", Stat);

	return Stat;
}

///<summary>
///This method will check the command data to see if it
///contains a Ulong data type in the first data element.
///The assumption is that the Ulong is an error code
///</summary>
///<param name="Cmd">The command</param>
///<param name="Resp">The command response</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::CheckForError(CommandData *Resp)
{
	if (Resp->GetNumberOfArguments() < 1) return 0;

	Ulong *Err  = dynamic_cast<Ulong *>(Resp->GetArgument(0));
	if (Err) return Err->GetValue();

	return 0;
}

///<summary>
///This method will get a spectrum from the device
///</summary>
///<param name="Spec">The spectrum</param>
///<param name="Input">The input to get it from</param>
///<param name="Group">The memory group to get it from</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::GetSpectrum(Spectrum &Spec, USHORT Input, USHORT Group)
{

	//Send the command
	ByteStream Str;
	CommandData *Resp=NULL;
	CommandData Cmd(CommandCodes::GetSpectrum, Input);
	Ulong G((ULONG) Group);
	Cmd.AddArgument(&G);

	ULONG Stat = Control(Cmd, &Resp);
	{
	if (Stat) goto EXIT;

	if (Resp->GetNumberOfArguments() < 1) {
		Stat = GEN_INVALID_RESPONSE;
		DEBUG_ERROR_MESSAGE(L"Device", L"GetSpectrum", L"Error device response is invalid.", Stat);
		goto EXIT;
	}
	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

	Spectrum *Data  = dynamic_cast<Spectrum *>(Resp->GetArgument(0));
	if (!Data) {
		Stat = GEN_INVALID_RESPONSE;
		DEBUG_ERROR_MESSAGE(L"Device", L"GetSpectrum", L"Error received from the device.", Stat);
		goto EXIT;
	}

	try {
		Data->Serialize(Str, 0);
		Spec.PutSerializeWithMeta(true);
		Spec.Deserialize(Str, 0);
	}
	CATCHTRACE(L"Device", L"GetSpectrum", Stat);
	}
EXIT:
	if (Resp) delete Resp;
	return Stat;

}

///<summary>
///This method will set a spectrum into the device
///</summary>
///<param name="Spec">The spectrum</param>
///<param name="Input">The input to get it from</param>
///<param name="Group">The memory group to get it from</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::PutSpectrum(Spectrum &Spec, USHORT Input, USHORT Group)
{
	//Send the command
	CommandData *Resp=NULL;
	CommandData Cmd(CommandCodes::PutSpectrum, Input);
	Ulong G((ULONG) Group);
	Cmd.AddArgument(&G);
	Spec.PutSerializeWithMeta(true);
	Cmd.AddArgument(&Spec);

	ULONG Stat = Control(Cmd, &Resp);
	if (Stat) goto EXIT;

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

EXIT:
	if (Resp) delete Resp;
	return Stat;
}


///<summary>
///This method will get the extended internet error information
///</summary>
///<returns>The Win32 error code. 0 ==Success</returns>
LONG GetInternetError()
{

#if defined(WIN32) && !defined(_WIN32_WCE)
	LONG Stat = GetLastError();
	if (ERROR_INTERNET_EXTENDED_ERROR == Stat) {
		TCHAR ErrBuf[8192]={'\0'};
		DWORD Size = 8192;
		DWORD ExtStat=0;
		if (InternetGetLastResponseInfo(&ExtStat, ErrBuf, &Size)) {
			if (ExtStat) Stat = ExtStat;
			else Stat = ERROR_LOCK_VIOLATION;  //This is the mostlikely cause
			DEBUG_TYPED_MESSAGE(L"Device", L"GetInternetError", ErrBuf, Canberra::Utility::Core::Diagnostics::Error);
		}
	}
#else
	LONG Stat = 0;
#endif
	return Stat;
}
#if 0
///<summary>
///This method will get a a file from the device and place
///it at the destination location.  There are two different
///implementations.
///		-) For remote PC based systems FTP is used
///		-) For local WinCE based systems a local file copy is used
//
///</summary>
///<param name="Src">The source file (on the device)</param>
///<param name="Dest">The destination</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::GetFile(const Canberra::Utility::Core::String &Src, const Canberra::Utility::Core::String &Dest, bool Binary)
{

#ifdef _WIN32_WCE
	ULONG Stat=0;
	Canberra::Utility::Core::String SrcF=const_cast<Canberra::Utility::Core::String &>(Src);
	Canberra::Utility::Core::String &DestF=const_cast<Canberra::Utility::Core::String &>(Dest);
	if (!CopyFile(SrcF, DestF, FALSE)) {
		Stat=GetLastError();
		DEBUG_ERROR_MESSAGE(L"Device", L"GetFile", L"Error calling CopyFile.", Stat);
		return Stat;
	}
	return Stat;

#elif defined(WIN32)
	ULONG Stat=0;
	Canberra::Utility::Core::String SrcF=const_cast<Canberra::Utility::Core::String &>(Src);
	Canberra::Utility::Core::String &DestF=const_cast<Canberra::Utility::Core::String &>(Dest);

	HINTERNET Internet = NULL;
	HINTERNET Ftp=NULL;
	Canberra::Utility::Core::String Name(L"");
	Canberra::Utility::Core::String Pwd(L"");

	try {
		//
		//Using INTERNET_OPEN_TYPE_PRECONFIG because is configurable via registry
		//
		Internet = InternetOpen(L"Lynx", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (NULL == Internet) {
			Stat = GetInternetError();
			DEBUG_ERROR_MESSAGE(L"Device", L"GetFile", L"Error calling InternetOpen.", Stat);
			throw Stat;
		}

		Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
		LockManager LM(&(Ext->GetConfigurationChannelLock()));
		Ftp=InternetConnect(Internet, _ConfigChannel.GetServerAddress().GetAddressString(), INTERNET_DEFAULT_FTP_PORT, Name, Pwd, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
		if (NULL == Ftp) {
			Stat = GetInternetError();
			DEBUG_ERROR_MESSAGE(L"Device", L"GetFile", L"Error calling InternetConnect.", Stat);
			throw Stat;
		}
		LM.Unlock();

		//
		//Using INTERNET_FLAG_RELOAD|INTERNET_FLAG_NO_CACHE_WRITE to get the latest
		//file version not a cached version
		//
		BOOL Ret=FtpGetFile(Ftp, SrcF, DestF, FALSE, FILE_ATTRIBUTE_NORMAL,
						   ((Binary) ? FTP_TRANSFER_TYPE_BINARY : FTP_TRANSFER_TYPE_ASCII)|INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
		if (!Ret) {
			Stat = GetInternetError();
			DEBUG_ERROR_MESSAGE(L"Device", L"GetFile", L"Error calling FtpGetFile.", Stat);
			throw Stat;
		}
	}
	CATCHTRACE(L"Device", L"GetFile", Stat);

	if (Ftp) InternetCloseHandle(Ftp);
	if (Internet) InternetCloseHandle(Internet);
	return Stat;
#else
	DEBUG_ERROR_MESSAGE(L"Device", L"GetFile", L"This method is not supported on this operating system.", E_FAIL);
	return E_FAIL;
#endif
}
#endif


///<summary>
///This method will return the open state of the
///communication connection
///</summary>
///<returns>Open state</returns>
bool Canberra::Protocols::Lynx::Device::GetIsOpen() {
	Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
	LockManager LM(&(Ext->GetConfigurationChannelLock()));
	if (_ConfigChannel.GetIsOpen()) return true;
	return false;
}

//Accessor to the socket port
USHORT Canberra::Protocols::Lynx::Device::GetPort() {
	Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
	LockManager LM(&(Ext->GetConfigurationChannelLock()));
	return Ext->GetConfigurationChannelSettings().Port;
}
void Canberra::Protocols::Lynx::Device::PutPort(USHORT v) {
	Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
	LockManager LM(&(Ext->GetConfigurationChannelLock()));
	Ext->GetConfigurationChannelSettings().Port=v;
}


///<summary>
///This method sets the send timeout
///</summary>
///<param name="TO">The time out (ms) </param>
void Canberra::Protocols::Lynx::Device::PutSendTimeout(ULONG TO) {
	int Val=TO;
	int Size=sizeof(Val);
	Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
	LockManager LM(&(Ext->GetConfigurationChannelLock()));
	_ConfigChannel.SetOption(SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char *>(&Val), Size);
	Ext->GetStreamingInterface().PutProperty(String(L"SendTimeout"), Variant(TO));
}
///<summary>
///This method gets the send timeout
///</summary>
///<returns>The time out (ms) </returns>
ULONG Canberra::Protocols::Lynx::Device::GetSendTimeout() {
	int Val=0;
	LONG Size=sizeof(Val);
	Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
	LockManager LM(&(Ext->GetConfigurationChannelLock()));
	_ConfigChannel.GetOption(SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char *>(&Val), &Size);
	return Val;
}

///<summary>
///This method sets the receive timeout
///</summary>
///<param name="TO">The time out (ms) </param>
void Canberra::Protocols::Lynx::Device::PutReceiveTimeout(ULONG TO) {
	int Val=TO;
	int Size=sizeof(Val);
	Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
	LockManager LM(&(Ext->GetConfigurationChannelLock()));
	_ConfigChannel.SetOption(SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&Val), Size);
	Ext->GetStreamingInterface().PutProperty(String(L"ReceiveTimeout"), Variant(TO));
}

///<summary>
///This method gets the receive timeout.
///</summary>
///<returns>The time out (ms)</returns>
ULONG Canberra::Protocols::Lynx::Device::GetReceiveTimeout() {
	int Val=0;
	LONG Size=sizeof(Val);
	Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
	LockManager LM(&(Ext->GetConfigurationChannelLock()));
	_ConfigChannel.GetOption(SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&Val), &Size);
	return Val;
}

//Accessor to keep-alive
bool Canberra::Protocols::Lynx::Device::GetKeepAlive() {return _ConfigChannel.GetKeepAlive();}
void Canberra::Protocols::Lynx::Device::PutKeepAlive(bool v) {
	Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
	LockManager LM(&(Ext->GetConfigurationChannelLock()));
	_ConfigChannel.PutKeepAlive(v);
	Ext->GetStreamingInterface().PutProperty(String(L"KeepAlive"), Variant(v));
}

///<summary>
///This method adds a user account to the device
///</summary>
///<param name="Name">The user name </param>
///<param name="Pwd">The password </param>
///<param name="Desc">The description </param>
///<param name="Priv">The privs </param>
///<returns>Execution status</returns>
ULONG Canberra::Protocols::Lynx::Device::AddUser(const Canberra::Utility::Core::String &User, const Canberra::Utility::Core::String &Pwd, const Canberra::Utility::Core::String &Desc, LONG Priv)
{
	//Send the command
	CommandData Cmd(CommandCodes::AddUser, 0);
	String Passwd;
	ULONG Stat=Encrypt(Pwd, Passwd);
	if (Stat) return Stat;
	Wstring UserN(const_cast<Canberra::Utility::Core::String &>(User)),
			UserD(const_cast<Canberra::Utility::Core::String &>(Desc)),
			Password(Passwd);
	Long UserP(Priv);
	Cmd.AddArgument(&UserN);
	Cmd.AddArgument(&Password);
	Cmd.AddArgument(&UserD);
	Cmd.AddArgument(&UserP);
	return Control(Cmd);
}


///<summary>
///This method updates a user account on the device
///</summary>
///<param name="Name">The user name </param>
///<param name="Pwd">The password </param>
///<param name="Desc">The description </param>
///<param name="Priv">The privs </param>
///<returns>Execution status</returns>
ULONG Canberra::Protocols::Lynx::Device::UpdateUser(const Canberra::Utility::Core::String &User, const Canberra::Utility::Core::String &Pwd, const Canberra::Utility::Core::String &Desc, LONG Priv)
{
	//Send the command
	CommandData Cmd(CommandCodes::UpdateUser, 0);
	String Passwd;
	ULONG Stat=Encrypt(Pwd, Passwd);
	if (Stat) return Stat;
	Wstring UserN(const_cast<Canberra::Utility::Core::String &>(User)),
			UserD(const_cast<Canberra::Utility::Core::String &>(Desc)),
			Password(Passwd);
	Long UserP(Priv);
	Cmd.AddArgument(&UserN);
	Cmd.AddArgument(&Password);
	Cmd.AddArgument(&UserD);
	Cmd.AddArgument(&UserP);
	return Control(Cmd);
}


///<summary>
///This method validates a user account on the device
///</summary>
///<param name="Name">The user name </param>
///<param name="Pwd">The password </param>
///<param name="Val">The validation state </param>
///<returns>Execution status</returns>
ULONG Canberra::Protocols::Lynx::Device::ValidateUser(const Canberra::Utility::Core::String &User, const Canberra::Utility::Core::String &Pwd, bool &Val)
{
	Val = false;

	//Send the command
	CommandData Cmd(CommandCodes::ValidateUser, 0);
	String Passwd;
	ULONG Stat=Encrypt(Pwd, Passwd);
	if (Stat) return Stat;
	Wstring UserN(const_cast<Canberra::Utility::Core::String &>(User)),
			Password(Passwd);
	Cmd.AddArgument(&UserN);
	Cmd.AddArgument(&Password);
	Stat = Control(Cmd);
	if ((Stat) && (DSA3K_USERNOTEXIST == Stat) ){
		Val = false;
		Stat = 0;
	}
	else Val = true;
	return Stat;
}

///<summary>
///This method deletes a user account on the device
///</summary>
///<param name="Name">The user name </param>
///<param name="Pwd">The password </param>
///<returns>Execution status</returns>
ULONG Canberra::Protocols::Lynx::Device::DeleteUser(const Canberra::Utility::Core::String &User)
{
	//Send the command
	CommandData Cmd(CommandCodes::DeleteUser, 0);
	Wstring UserN(const_cast<Canberra::Utility::Core::String &>(User));
	Cmd.AddArgument(&UserN);
	return Control(Cmd);
}


///<summary>
///This method enumerates all user accounts
///</summary>
///<param name="Val">The user names</param>
///<returns>Execution status</returns>
ULONG Canberra::Protocols::Lynx::Device::EnumerateUsers(std::vector<Canberra::Utility::Core::Variant> &Val)
{
	Val.clear();

	//Send the command
	CommandData *Resp=NULL;
	CommandData Cmd(CommandCodes::EnumerateUsers, 0);
	ULONG Stat = Control(Cmd, &Resp);
	if (Stat) goto EXIT;

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

	for(USHORT i=0; i<Resp->GetNumberOfArguments(); i++) {
		Wstring *Param  = dynamic_cast<Wstring *>(Resp->GetArgument(i));
		if (!Param) {
			Stat = GEN_INVALID_RESPONSE;
			DEBUG_ERROR_MESSAGE(L"Device", L"EnumerateUsers", L"Error device response is invalid. Argument is not a Wstring type.", Stat);
			goto EXIT;
		}
		//Got the value
		Canberra::Utility::Core::Variant N(Param->GetValue());
		Val.push_back(N);
	}

EXIT:
	if (Resp) delete Resp;
	return Stat;
}

///<summary>
///This method gets the regions of interest
///</summary>
///<param name="Input">The input to operate on</param>
///<param name="Val">The regions of interest</param>
///<returns>Execution status</returns>
ULONG Canberra::Protocols::Lynx::Device::GetRegionsOfInterest(USHORT Input, std::vector<RegionOfInterest> &Val)
{
	Val.clear();

	//Send the command
	CommandData *Resp=NULL;
	CommandData Cmd(CommandCodes::GetRegionsOfInterest, Input);
	ULONG Stat = Control(Cmd, &Resp);
	if (Stat) goto EXIT;

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

	for(USHORT i=0; i<Resp->GetNumberOfArguments(); i++) {
		RegionOfInterest *Param  = dynamic_cast<RegionOfInterest *>(Resp->GetArgument(i));
		if (!Param) {
			Stat = GEN_INVALID_RESPONSE;
			DEBUG_ERROR_MESSAGE(L"Device", L"GetRegionsOfInterest", L"Error device response is invalid. Argument is not a RegionOfInterest type.", Stat);
			goto EXIT;
		}
		//Got the value
		Val.push_back(*(Param));
	}

EXIT:
	if (Resp) delete Resp;
	return Stat;
}


///<summary>
///This method sets the regions of interest
///</summary>
///<param name="Input">The input to operate on</param>
///<param name="Vals">The regions of interest</param>
///<returns>Execution status</returns>
ULONG Canberra::Protocols::Lynx::Device::PutRegionsOfInterest(USHORT Input, std::vector<RegionOfInterest> &Val)
{
	//Send the command
	CommandData *Resp=NULL;
	CommandData Cmd(CommandCodes::PutRegionsOfInterest, Input);
	ByteStream Str;
	for(size_t i=0; i<Val.size(); i++) {
		Cmd.AddArgument(&(Val[i]));
	}
	ULONG Stat = Control(Cmd, &Resp);
	if (Stat) goto EXIT;

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

EXIT:
	if (Resp) delete Resp;
	return Stat;
}

///<summary>
///This method will get counter data from the device
///</summary>
///<param name="Val">The counter data</param>
///<param name="Input">The input to get it from</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::GetCounterData(CounterData &Val, USHORT Input)
{
	Val.Clear();

	//Send the command
	CommandData *Resp=NULL;
	CommandData Cmd(CommandCodes::GetCounterData, Input);

	ULONG Stat = Control(Cmd, &Resp);
	{
	if (Stat) goto EXIT;

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

	//No data
	if (!Resp->GetNumberOfArguments()) goto EXIT;

	//Data
	CounterData *CD  = dynamic_cast<CounterData *>(Resp->GetArgument(0));
	if (!CD) {
		Stat = GEN_INVALID_RESPONSE;
		DEBUG_ERROR_MESSAGE(L"Device", L"GetCounterData", L"Error device response is invalid. Argument is not a CounterData type.", Stat);
		goto EXIT;
	}

	//Got the value
	Val = *CD;
	}
EXIT:
	if (Resp) delete Resp;
	return Stat;
}

#if 0
///<summary>
///This method will get DSO data from the device
///</summary>
///<param name="Val">The DSO data</param>
///<param name="Input">The input to get it from</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::GetDsoData(DsoData &Val, USHORT Input)
{
	Val.Clear();

	//Send the command
	CommandData *Resp=NULL;
	CommandData Cmd(CommandCodes::GetDsoData, Input);

	ULONG Stat = Control(Cmd, &Resp);
	if (Stat) goto EXIT;

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

	//No data
	if (!Resp->GetNumberOfArguments()) goto EXIT;

	//Data
	DsoData *DSO  = dynamic_cast<DsoData *>(Resp->GetArgument(0));
	if (!DSO) {
		Stat = GEN_INVALID_RESPONSE;
		DEBUG_ERROR_MESSAGE(L"Device", L"GetDsoData", L"Error device response is invalid. Argument is not a DsoData type.", Stat);
		goto EXIT;
	}

	//Got the value
	Val = *DSO;

EXIT:
	if (Resp) delete Resp;
	return Stat;
}
#endif


///<summary>
///This method will get MSS data from the device
///</summary>
///<param name="Val">The data</param>
///<param name="Input">The input to get it from</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::GetMSSData(SpectralDataList &Val, USHORT Input)
{
	//Make sure we are empty
	Val.Clear();

	//Send the command
	CommandData *Resp=NULL;
	CommandData Cmd(CommandCodes::GetMSSData, Input);

	//See if we get data from streaming interface
	ULONG Stat=0;
	bool SendCmd=true;

	Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
	LockManager LM(&(Ext->GetConfigurationChannelLock()));
	if (Ext->GetStreamingInterface().GetActive()) {
		Stat = Ext->GetStreamingInterface().ReceiveCommand(&Resp);
		if (Stat) goto EXIT;
		if (Resp) SendCmd=false;
	}
	LM.Unlock();

	//Case get data by issuing command instead of using stream
	if (SendCmd) {
		Stat = Control(Cmd, &Resp);
		if (Stat) goto EXIT;
	}

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

	//No data
	if (!Resp->GetNumberOfArguments()) goto EXIT;

	//Data
	for(USHORT i=0; i<Resp->GetNumberOfArguments(); i++) {
		SpectralData *SD  = dynamic_cast<SpectralData *>(Resp->GetArgument(i));
		if (!SD) {
			Stat = GEN_INVALID_RESPONSE;
			DEBUG_ERROR_MESSAGE(L"Device", L"GetMSSData", L"Error device response is invalid. Argument is not a SpectralData type.", Stat);
			goto EXIT;
		}

		if (dynamic_cast<PhaData *>(SD)) {
			PhaData *Data=new PhaData();
			ByteStream Str;
			SD->Serialize(Str);
			Data->Deserialize(Str);
			Val.Add(Data);
		}
		else if (dynamic_cast<McsData *>(SD)) {
			McsData *Data=new McsData();
			ByteStream Str;
			SD->Serialize(Str);
			Data->Deserialize(Str);
			Val.Add(Data);
		}


	}

EXIT:
	if (Resp) delete Resp;
	return Stat;
}


///<summary>
///This method will get list data from the device
///</summary>
///<param name="Val">The data</param>
///<param name="Input">The input to get it from</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::GetListData(ListDataBase **Val, USHORT Input)
{
	*Val = NULL;

	ULONG Stat=0;
	CommandData *Resp=NULL;
	ListData *LD  = NULL;
	TlistData *TD  = NULL;
	Long Lval(0);

	//Send the command
	CommandData Cmd(CommandCodes::GetListData, Input);
	Cmd.AddArgument(&Lval);

	//See if we get data from streaming interface
	bool SendCmd=true;
	Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
	LockManager LM(&(Ext->GetConfigurationChannelLock()));
	if (Ext->GetStreamingInterface().GetActive()) {
		Stat = Ext->GetStreamingInterface().ReceiveCommand(&Resp);
		if (Stat) goto EXIT;
		if (Resp) SendCmd=false;
	}
	LM.Unlock();

	//Case get data by issuing command instead of using stream
	if (SendCmd) {
		Stat = Control(Cmd, &Resp);
		if (Stat) goto EXIT;
	}

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

	//No data
	if (!Resp->GetNumberOfArguments()) goto EXIT;

	LD  = dynamic_cast<ListData *>(Resp->GetArgument(0));
	TD  = dynamic_cast<TlistData *>(Resp->GetArgument(0));
	if (LD) {
		ListData *Copy = new ListData();
		if (!Copy) {
			Stat = (ULONG) E_OUTOFMEMORY;
			DEBUG_ERROR_MESSAGE(L"Device", L"GetListData", L"Error allocating memory.", Stat);
			goto EXIT;
		}
		*Copy = *LD;
		*Val = Copy;
	}
	else if (TD) {
		TlistData *Copy = new TlistData();
		if (!Copy) {
			Stat = (ULONG) E_OUTOFMEMORY;
			DEBUG_ERROR_MESSAGE(L"Device", L"GetListData", L"Error allocating memory.", Stat);
			goto EXIT;
		}
		*Copy = *TD;
		*Val = Copy;
	}
	else {
		Stat = GEN_INVALID_RESPONSE;
		DEBUG_ERROR_MESSAGE(L"Device", L"GetListData", L"Error device response is invalid. Argument is not a ListData nor TlistData type.", Stat);
		goto EXIT;
	}

EXIT:
	if (Resp) delete Resp;
	return Stat;
}


///<summary>
///This method will release the list data memory allocated by this instance
///</summary>
///<param name="Data">The data to release</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::Delete(ListDataBase **Data)
{
	ULONG Stat=0;
	if (!Data) {
		Stat = (ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(L"Device", L"Delete", L"Error deleting memory invalid arg (arg==NULL).", Stat);
		goto EXIT;
	}
	delete *Data;
	*Data = NULL;
EXIT:
	return Stat;
}


///<summary>
///This method set the value for a named property
///This method exists for extending the features of
///this class without breaking the contract
///</summary>
///<param name="N">(in) The name</param>
///<param name="V">(in) The value</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::PutProperty(const Canberra::Utility::Core::String &N, const Canberra::Utility::Core::Variant &Val)
{
	ULONG Stat=0;
	Canberra::Utility::Core::Variant &V = const_cast<Canberra::Utility::Core::Variant &>(Val);
	Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
	Stat=Ext->GetStreamingInterface().PutProperty(N, V);
	if (!N.CompareI(L"SendTimeout")) {
		int Val=V.ToLong();

		Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
		LockManager LM(&(Ext->GetConfigurationChannelLock()));
		if (_ConfigChannel.GetIsOpen()) {
#ifdef WIN32
			int Size=sizeof(Val);
			Stat = _ConfigChannel.SetOption(SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char *>(&Val), Size);
#endif
			if (!Stat) Ext->GetConfigurationChannelSettings().SendTimeOut = Val;
		}
		else Ext->GetConfigurationChannelSettings().SendTimeOut = Val;
	}
	else if (!N.CompareI(L"ReceiveTimeout")) {
		int Val=V.ToLong();

		Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
		LockManager LM(&(Ext->GetConfigurationChannelLock()));
		if (_ConfigChannel.GetIsOpen()) {
#ifdef WIN32
			int Size=sizeof(Val);
			Stat = _ConfigChannel.SetOption(SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&Val), Size);
#endif
			if (!Stat) Ext->GetConfigurationChannelSettings().RecvTimeOut = Val;
		}
		else Ext->GetConfigurationChannelSettings().RecvTimeOut = Val;
	}
	else if (!N.CompareI(L"SendBufferSize")) {
		int Val=V.ToLong();
		int Size=sizeof(Val);

		Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
		LockManager LM(&(Ext->GetConfigurationChannelLock()));
		if (_ConfigChannel.GetIsOpen()) {
			Stat=_ConfigChannel.SetOption(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char *>(&Val), Size);
			if (!Stat) Ext->GetConfigurationChannelSettings().SendBufferSize = Val;
		}
		else Ext->GetConfigurationChannelSettings().SendBufferSize = Val;
	}
	else if (!N.CompareI(L"ReceiveBufferSize")) {
		int Val=V.ToLong();
		int Size=sizeof(Val);

		Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
		LockManager LM(&(Ext->GetConfigurationChannelLock()));
		if (_ConfigChannel.GetIsOpen()) {
			Stat=_ConfigChannel.SetOption(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&Val), Size);
			if (!Stat) Ext->GetConfigurationChannelSettings().RecvBufferSize = Val;
		}
		else Ext->GetConfigurationChannelSettings().RecvBufferSize = Val;

	}
	else if (!N.CompareI(L"KeepAlive")) {
		int Val=V.ToLong() ? TRUE : FALSE;
		int Size=sizeof(Val);

		Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
		LockManager LM(&(Ext->GetConfigurationChannelLock()));
		if (_ConfigChannel.GetIsOpen()) {
			Stat=_ConfigChannel.SetOption(SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char *>(&Val), Size);
			if (!Stat) Ext->GetConfigurationChannelSettings().KeepAlive = Val;
		}
		else Ext->GetConfigurationChannelSettings().KeepAlive = Val;
	}
	else if (!N.CompareI(L"NoDelay")) {
		BOOL Val=V.ToLong() ? TRUE : FALSE;
		int Size=sizeof(Val);

		Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
		LockManager LM(&(Ext->GetConfigurationChannelLock()));
		if (_ConfigChannel.GetIsOpen()) {
			Stat=_ConfigChannel.SetOption(IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&Val), Size);
			if (!Stat) Ext->GetConfigurationChannelSettings().NoDelay = Val;
		}
		else Ext->GetConfigurationChannelSettings().NoDelay = Val;

	}
	else if (!N.CompareI(L"Port")) {
		USHORT Val=V.ToUshort() ? TRUE : FALSE;
		Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
		LockManager LM(&(Ext->GetConfigurationChannelLock()));
		Ext->GetConfigurationChannelSettings().Port = Val;
	}

	return Stat;
}


///<summary>
///This method get the value of a named property
///This method exists for extending the features of
///this class without breaking the contract
///</summary>
///<param name="N">(in) The name</param>
///<param name="V">(in, out) The value</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::GetProperty(const Canberra::Utility::Core::String &N, Canberra::Utility::Core::Variant &V)
{
	ULONG Stat=0;
	V.Clear();
	Extensions *Ext=reinterpret_cast<Extensions *>(_Data);

	if (!N.CompareI(L"SendTimeout")) {
		Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
		LockManager LM(&(Ext->GetConfigurationChannelLock()));
		V.Assign((LONG) Ext->GetConfigurationChannelSettings().SendTimeOut);
	}
	else if (!N.CompareI(L"ReceiveTimeout")) {
		Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
		LockManager LM(&(Ext->GetConfigurationChannelLock()));
		V.Assign((LONG) Ext->GetConfigurationChannelSettings().RecvTimeOut);
	}
	else if (!N.CompareI(L"SendBufferSize")) {
		Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
		LockManager LM(&(Ext->GetConfigurationChannelLock()));
		V.Assign((LONG) Ext->GetConfigurationChannelSettings().SendBufferSize);
	}
	else if (!N.CompareI(L"ReceiveBufferSize")) {
		Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
		LockManager LM(&(Ext->GetConfigurationChannelLock()));
		V.Assign((LONG) Ext->GetConfigurationChannelSettings().RecvBufferSize);
	}
	else if (!N.CompareI(L"KeepAlive")) {
		Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
		LockManager LM(&(Ext->GetConfigurationChannelLock()));
		V.Assign((LONG) Ext->GetConfigurationChannelSettings().KeepAlive);
	}
	else if (!N.CompareI(L"NoDelay")) {
		Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
		LockManager LM(&(Ext->GetConfigurationChannelLock()));
		V.Assign((LONG) Ext->GetConfigurationChannelSettings().NoDelay);
	}
	else if (!N.CompareI(L"Port")) {
		Extensions *Ext=reinterpret_cast<Extensions *>(_Data);
		LockManager LM(&(Ext->GetConfigurationChannelLock()));
		V.Assign((LONG) Ext->GetConfigurationChannelSettings().Port);
	}
	else if (!N.CompareI(L"StreamingDataAvailable") && !Ext->GetStreamingInterface().GetEnable()) {
		if (_ConfigChannel.GetIsOpen()) V.Assign(_ConfigChannel.GetIsReceiving());
		else V.Assign((bool) false);
	}
	if (!V.GetHasValue()) Stat = Ext->GetStreamingInterface().GetProperty(N, V);
	return Stat;
}


///<summary>
///This method get the value of a named method
///This method exists for extending the features of
///this class without breaking the contract
///</summary>
///<param name="N">(in) The method name</param>
///<param name="Args">(in) The args</param>
///<param name="Ret">(in) The return value</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::Device::ExtMethod(const Canberra::Utility::Core::String &, const Canberra::Utility::Core::VariantArray &, Canberra::Utility::Core::VariantArray &)
{
	ULONG Stat=0;

	return Stat;
}


///<summary>
///This method will create and instance of this class.  An instance of this class may be used to send and receive information
///to and from a DSA 3K device.  It extends the base class to expose SCA functionality
///</summary>
Canberra::Protocols::Lynx::DeviceEx::DeviceEx() {}
Canberra::Protocols::Lynx::DeviceEx::~DeviceEx() {}

///<summary>
///This method get SCA definitions from the device
///</summary>
///<param name="Input">(in) The input operated on</param>
///<param name="Vals">(in, out) The defs</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::DeviceEx::GetSCAdefinitions(USHORT Input, SCAdefinitions &Val)
{
	Val.Clear();

	//Send the command
	CommandData *Resp=NULL;
	CommandData Cmd(CommandCodes::GetSCAdefinitions, Input);
	ULONG Stat = Control(Cmd, &Resp);
	{
	if (Stat) goto EXIT;

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

	SCAdefinitions *Param  = dynamic_cast<SCAdefinitions *>(Resp->GetArgument(0));
	if (!Param) {
		Stat = GEN_INVALID_RESPONSE;
		DEBUG_ERROR_MESSAGE(L"Device", L"GetSCAdefinitions", L"Error device response is invalid. Argument is not a SCAdefinitions type.", Stat);
		goto EXIT;
	}
	Val = *Param;
	}
EXIT:
	if (Resp) delete Resp;
	return Stat;
}


///<summary>
///This method set SCA definitions into the device
///</summary>
///<param name="Input">(in) The input operated on</param>
///<param name="Vals">(in) The defs</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::DeviceEx::PutSCAdefinitions(USHORT Input, SCAdefinitions &Vals)
{
	//Send the command
	CommandData *Resp=NULL;
	CommandData Cmd(CommandCodes::PutSCAdefinitions, Input);
	ByteStream Str;
	Cmd.AddArgument(&Vals);

	ULONG Stat = Control(Cmd, &Resp);
	if (Stat) goto EXIT;

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

EXIT:
	if (Resp) delete Resp;
	return Stat;
}

///<summary>
///This method get SCA buffer from the device
///</summary>
///<param name="Input">(in) The input operated on</param>
///<param name="Vals">(in, out) The buffer</param>
///<returns>The Win32 error code. 0 ==Success</returns>
ULONG Canberra::Protocols::Lynx::DeviceEx::GetSCAbuffer(USHORT Input, SCAbuffer &Val)
{
	Val.Clear();

	//Send the command
	CommandData *Resp=NULL;
	CommandData Cmd(CommandCodes::GetSCAbuffer, Input);
	ULONG Stat = Control(Cmd, &Resp);
	{
	if (Stat) goto EXIT;

	Stat = CheckForError(Resp);
	if (Stat) goto EXIT;

	SCAbuffer *Param  = dynamic_cast<SCAbuffer *>(Resp->GetArgument(0));
	if (!Param) {
		Stat = GEN_INVALID_RESPONSE;
		DEBUG_ERROR_MESSAGE(L"Device", L"GetSCAbuffer", L"Error device response is invalid. Argument is not a SCAbuffer type.", Stat);
		goto EXIT;
	}
	Val = *Param;
	}
EXIT:
	if (Resp) delete Resp;
	return Stat;


}

