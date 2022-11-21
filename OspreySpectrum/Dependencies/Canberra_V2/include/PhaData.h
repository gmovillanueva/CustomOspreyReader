#pragma once
#include "DataTypesImpExp.h"
#include "SerializableObjectCodes.h"
#include "SpectralData.h"

namespace Canberra {
	namespace DataTypes {
		namespace Spectroscopy {

#pragma pack(push, 1)
			//Data specific to Multi-Channel Analyzer
			struct PhaDataS {
				LONGLONG LiveTime; //Live time (uS)
				LONGLONG RealTime; //Real time (uS)
				LONGLONG CompValue;//Computational value
			};
#pragma pack(pop)

			//EXPIMP_TEMPLATE template class DATATYPES_DLLIMPORT SpectralDataImpl<struct PhaDataS> ;

			//
			//An instance of this class defines all of the
			//measurement data associated with Multi-Channel Analyzer
			//The main purpose is to define a common means for serializing
			//this data type.
			//
			class DATATYPES_DLLIMPORT PhaData : public SpectralDataImpl<struct PhaDataS>
			{
				PhaData(const PhaData&);
				PhaData& operator=(const PhaData&);
			protected:
				//Handles the endianness of the platform
				//This method is invoked before and after serialization
				void HandleEndianness() {
					SWAPNBYTES(&(_Data.CompValue), sizeof(_Data.CompValue));
					SWAPNBYTES(&(_Data.LiveTime), sizeof(_Data.LiveTime));
					SWAPNBYTES(&(_Data.RealTime), sizeof(_Data.RealTime));
				}

			public:

				PhaData(ULONG Chans=0) : SpectralDataImpl<struct PhaDataS>(Canberra::DataTypes::Serialization::SerializableObjects::PhaData, Chans) {}
				virtual ~PhaData(void) {}

				//Get/set live time (uS)
				LONGLONG GetLiveTime() {return _Data.LiveTime;}
				void PutLiveTime(LONGLONG v) {_Data.LiveTime=v;}

				//Get/set real time (uS)
				LONGLONG GetRealTime() {return _Data.RealTime;}
				void PutRealTime(LONGLONG v) {_Data.RealTime=v;}

				//Get/set computational value
				LONGLONG GetCompValue() {return _Data.CompValue;}
				void PutCompValue(LONGLONG v) {_Data.CompValue=v;}

				//Serialize data
				ULONG SerializeDataEx(Canberra::Utility::Core::ByteStream &BS, LONG Offset=0) {
					ULONG StreamSize=GetDataSize();
					if (Offset < 0) {
						ULONG Stat = (ULONG) E_INVALIDARG;
						DEBUG_ERROR_MESSAGE(L"SerializableObject", L"Serialize", L"Invalid Offset size", Stat);
						throw Stat;
					}
					if ((Offset+StreamSize) > BS.GetSize()) BS.Resize(Offset+StreamSize);
					return SerializeData(BS, Canberra::Serialization::MetaData::MetaData(), Offset);
				}
				ULONG DeserializeDataEx(Canberra::Utility::Core::ByteStream &BS, LONG Offset=0) {
					return DeserializeData(BS, Canberra::Serialization::MetaData::MetaData(), Offset);
				}
			};
		}
	}
}

