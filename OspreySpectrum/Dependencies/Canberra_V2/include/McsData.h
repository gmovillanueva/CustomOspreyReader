#pragma once
#include "DataTypesImpExp.h"
#include "SerializableObjectCodes.h"
#include "SpectralData.h"

namespace Canberra {
	namespace DataTypes {
		namespace Spectroscopy {

#pragma pack(push, 1)
			//Data specific for Multi-Channel Sweep
			struct McsDataS {
				LONGLONG Dwell;	//Dwell time (nS)
				LONGLONG Sweeps;	//Current elapsed sweeps
			};
#pragma pack(pop)
			//EXPIMP_TEMPLATE template class DATATYPES_DLLIMPORT SpectralDataImpl<struct McsDataS> ;

			//
			//An instance of this class defines all of the
			//measurement data associated with Multi-Channel Sweep
			//The main purpose is to define a common means for serializing
			//this data type.
			//
			class DATATYPES_DLLIMPORT McsData : public SpectralDataImpl<struct McsDataS>
			{
				McsData(const McsData&);
				McsData& operator=(const McsData&);

			protected:
				//Handles the endianness of the platform
				//This method is invoked before and after serialization
				void HandleEndianness() {
					SWAPNBYTES(&(_Data.Dwell), sizeof(_Data.Dwell));
					SWAPNBYTES(&(_Data.Sweeps), sizeof(_Data.Sweeps));
				}

			public:
				McsData(ULONG Chans=0) : SpectralDataImpl<struct McsDataS>(Canberra::DataTypes::Serialization::SerializableObjects::McsData, Chans) {}
				virtual ~McsData(void) {}

				//Get/set sweeps
				LONGLONG GetSweeps() {return _Data.Sweeps;}
				void PutSweeps(LONGLONG v) {_Data.Sweeps=v;}

				//Get/set dwell (nS)
				LONGLONG GetDwell() {return _Data.Dwell;}
				void PutDwell(LONGLONG v) {_Data.Dwell=v;}

			};
		}
	}
}

