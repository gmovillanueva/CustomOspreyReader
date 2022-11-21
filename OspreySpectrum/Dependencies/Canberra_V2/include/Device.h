#pragma once
#include "CommunicationsImpExp.h"

#include "TcpClient.h"
#include "SpectralData.h"
#include "MessageFactory.h"
#include "Variant.h"
#include "VariantArray.h"
#include "RegionOfInterest.h"
#include "CommandData.h"
#include "CounterData.h"
#include "ParameterValueTypes.h"
#include "ListData.h"


#include "ParameterMetaData.h"
#include "ScaData.h"
#include <vector>

namespace Canberra {
	namespace Protocols {
		namespace Lynx {

			class COMMUNICATIONS_DLLIMPORT SpectralDataList {
				SpectralDataList(const SpectralDataList&);
				SpectralDataList& operator=(const SpectralDataList&);

			protected:
				void *_ListData;
			public:
				SpectralDataList();
				virtual ~SpectralDataList();

				Canberra::DataTypes::Spectroscopy::SpectralData *GetData(LONG i);
				ULONG GetSize();

				void Add(Canberra::DataTypes::Spectroscopy::SpectralData *);
				void Clear();

			};

			//
			//An instance of this class is used to
			//communicate with a DSA 3K
			//
			class COMMUNICATIONS_DLLIMPORT Device
			{
				Device(const Device&);
				Device& operator=(const Device&);

			protected:
				Canberra::Utility::Network::TcpClient _ConfigChannel;					//The channel used for data comm
				Canberra::DataTypes::Serialization::MessageFactory _MessageFactory;		//The message factory
				USHORT _Port;															//The (device side) port to use for all data comm
				void *_Data;															//Extra data for extensiblity
				ULONG Control(Canberra::DataTypes::Application::CommandData &Cmd);		//Issues a command to the device
				ULONG Control(Canberra::DataTypes::Application::CommandData &Cmd, Canberra::DataTypes::Application::CommandData **Resp);	//Issues a command to the device
				ULONG CheckForError(Canberra::DataTypes::Application::CommandData *Resp);

			public:
				Device(void);
				virtual ~Device(void);

				//Opens a connection with the device
				ULONG Open(const Canberra::Utility::Core::String &Client, const Canberra::Utility::Core::String &Device);

				//Closes a connection with the device
				ULONG Close();

				//Get a parameter from the device
				ULONG GetParameter(LONG Code, Canberra::Utility::Core::Variant &Val, USHORT Input=0);

				//Put a parameter into the device
				ULONG PutParameter(LONG Code, const Canberra::Utility::Core::Variant &Val, USHORT Input=0);

				//Get a list of parameters from the device
				ULONG GetParameterList(std::vector<LONG> &Codes, std::vector<Canberra::Utility::Core::Variant> &Vals, USHORT Input=0);

				//Put a list of parameters into the device
				ULONG PutParameterList(std::vector<LONG> &Codes, std::vector<Canberra::Utility::Core::Variant> &Vals, USHORT Input=0);

				//Get a parameter from the device
				ULONG GetSpectrum(Canberra::DataTypes::Spectroscopy::Spectrum &Spec, USHORT Input=1, USHORT Group=1);

				//Put a parameter into the device
				ULONG PutSpectrum(Canberra::DataTypes::Spectroscopy::Spectrum &Spec, USHORT Input=1, USHORT Group=1);

				//Updates a user account
				ULONG UpdateUser(const Canberra::Utility::Core::String &User, const Canberra::Utility::Core::String &Pwd, const Canberra::Utility::Core::String &Desc, LONG Priv);

				//Adds a user account
				ULONG AddUser(const Canberra::Utility::Core::String &User, const Canberra::Utility::Core::String &Pwd, const Canberra::Utility::Core::String &Desc, LONG Priv);

				//Validates the user
				ULONG ValidateUser(const Canberra::Utility::Core::String &User, const Canberra::Utility::Core::String &Pwd, bool &Val);

				//Deletes a user account
				ULONG DeleteUser(const Canberra::Utility::Core::String &User);

				//Lists the user accounts
				ULONG EnumerateUsers(std::vector<Canberra::Utility::Core::Variant> &Vals);

				//Gets the regions of interest
				ULONG GetRegionsOfInterest(USHORT Input, std::vector<Canberra::DataTypes::Spectroscopy::RegionOfInterest> &Vals);

				//Sets the regions of interest
				ULONG PutRegionsOfInterest(USHORT Input, std::vector<Canberra::DataTypes::Spectroscopy::RegionOfInterest> &Vals);

				//Executes a control command
				ULONG Control(LONG Code, USHORT Input=0);

				//Saves the spectrum on the device
				ULONG Save(USHORT Input, USHORT Group, Canberra::Utility::Core::String &Name);

				//Unlocks an input
				ULONG Unlock(const Canberra::Utility::Core::String &User, const Canberra::Utility::Core::String &Pwd, USHORT Input);

				//Locks an input
				ULONG Lock(const Canberra::Utility::Core::String &User, const Canberra::Utility::Core::String &Pwd, USHORT Input);

				//Gets counter data
				ULONG GetCounterData(Canberra::DataTypes::Spectroscopy::CounterData &Data, USHORT Input=1);

				//Gets MSS data
				ULONG GetMSSData(SpectralDataList &Data, USHORT Input=1);

				//Gets List data
				ULONG GetListData(Canberra::DataTypes::Spectroscopy::ListDataBase **Data, USHORT Input=1);

				//Associated mem release method
				ULONG Delete(Canberra::DataTypes::Spectroscopy::ListDataBase **Data);

				//Executes a control command with args and results
				ULONG Control(LONG Code, USHORT Input, Canberra::Utility::Core::ByteStream &In, Canberra::Utility::Core::ByteStream &Out);

				//Get parameter metadata from the device
				ULONG GetParameterMetaData(LONG Code, Canberra::DataTypes::Application::ParameterMetaData &PD, USHORT Input=0);

				//Accessor to the socket port
				USHORT GetPort();
				void PutPort(USHORT v);

				//Accessor to the send time out
				ULONG GetSendTimeout();
				void PutSendTimeout(ULONG v);

				//Accessor to the receive time out
				ULONG GetReceiveTimeout();
				void PutReceiveTimeout(ULONG v);

				//Accessor to the keep alive
				bool GetKeepAlive();
				void PutKeepAlive(bool v);

				//Accessor to name/value pair for extensibility
				ULONG PutProperty(const Canberra::Utility::Core::String &N, const Canberra::Utility::Core::Variant &V);

				//Accessor to name/value pair for extensibility
				ULONG GetProperty(const Canberra::Utility::Core::String &N, Canberra::Utility::Core::Variant &V);

				//Accessor to named method for extensibility
				ULONG ExtMethod(const Canberra::Utility::Core::String &N, const Canberra::Utility::Core::VariantArray &Args, Canberra::Utility::Core::VariantArray &Ret);

				//Accessor to the open state of the connection
				bool GetIsOpen();

			};

			//Extended interface for accessing SCA information
			class COMMUNICATIONS_DLLIMPORT DeviceEx : public Device
			{
			public:
				DeviceEx();
				virtual ~DeviceEx();

				//Gets the SCA defs
				ULONG GetSCAdefinitions(USHORT Input, Canberra::DataTypes::Spectroscopy::SCAdefinitions &Vals);

				//Sets the SCA defs
				ULONG PutSCAdefinitions(USHORT Input,  Canberra::DataTypes::Spectroscopy::SCAdefinitions &Vals);

				//Gets the SCA buffer
				ULONG GetSCAbuffer(USHORT Input, Canberra::DataTypes::Spectroscopy::SCAbuffer &Vals);

			};
		}
	}
}
