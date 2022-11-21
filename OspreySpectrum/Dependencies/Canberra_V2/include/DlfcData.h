#pragma once
#include "DataTypesImpExp.h"
#include "SerializableObjectCodes.h"
#include "SpectralData.h"

namespace Canberra {
	namespace DataTypes {
		namespace Spectroscopy {
#pragma pack(push, 1)
			struct DlfcDataS {
				LONGLONG LiveTime;		//Current live time (uS)
				LONGLONG RealTime;		//Current real time (uS)
				LONGLONG CompValue;		//Computational value
			};
#pragma pack(pop)
			//EXPIMP_TEMPLATE template class DATATYPES_DLLIMPORT SpectralDataImpl<struct DlfcDataS> ;

			//
			//An instance of this class defines all of the
			//measurement data associated with Dual Loss Free Counting
			//The main purpose is to define a common means for serializing
			//this data type.
			//
			class DATATYPES_DLLIMPORT DlfcData : public SpectralDataImpl<struct DlfcDataS>
			{
				DlfcData(const DlfcData&);
				DlfcData& operator=(const DlfcData&);

				Canberra::DataTypes::Spectroscopy::Spectrum _CorrSpectrum;

			protected:
				//Handles the endianness of the platform
				//This method is invoked before and after serialization
				void HandleEndianness() {
					SWAPNBYTES(&(_Data.CompValue), sizeof(_Data.CompValue));
					SWAPNBYTES(&(_Data.LiveTime), sizeof(_Data.LiveTime));
					SWAPNBYTES(&(_Data.RealTime), sizeof(_Data.RealTime));
				}

			public:
				DlfcData(ULONG Chans=0) : SpectralDataImpl<struct DlfcDataS>(Canberra::DataTypes::Serialization::SerializableObjects::DlfcData, Chans), _CorrSpectrum(Chans) {}
				virtual ~DlfcData(void) {}

				//Get/set live time (uS)
				LONGLONG GetLiveTime() {return _Data.LiveTime;}
				void PutLiveTime(LONGLONG v) {_Data.LiveTime=v;}

				//Get/set real time (uS)
				LONGLONG GetRealTime() {return _Data.RealTime;}
				void PutRealTime(LONGLONG v) {_Data.RealTime=v;}

				//Get/set computational value
				LONGLONG GetCompValue() {return _Data.CompValue;}
				void PutCompValue(LONGLONG v) {_Data.CompValue=v;}

				//Get/set spectrum
				Canberra::DataTypes::Spectroscopy::Spectrum &GetCorrectedSpectrum() {return _CorrSpectrum;}

				//Returns the size (override base class)
				ULONG GetDataSize() {return SpectralDataImpl<struct DlfcDataS>::GetDataSize() + _CorrSpectrum.GetSize(); }

				///<summary>
				///This method will serialize this instance into the byte stream (override base class)
				///</summary>
				///<param name="Str">The stream</param>
				///<param name="MD">The metadata</param>
				///<param name="Offset">The byte position to start the write in the stream</param>
				///<exception cref="ULONG">The Win32 error code</exception>
				///<returns>The number of bytes serialized</returns>
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &M, const LONG Offset)
				{
					LONG Pos=Offset;
					LONG NumCpy=0;
					struct DlfcDataS Copy=_Data;

					HandleEndianness();
					NumCpy=SpectralDataImpl<struct DlfcDataS>::SerializeData(Str, M, Pos); Pos+=NumCpy;
					_Data=Copy;

					NumCpy=_CorrSpectrum.Serialize(Str, Pos); Pos+=NumCpy;
					return Pos-Offset;
				}

				///<summary>
				///This method will deserialize info from the byte stream into this instance (override base class)
				///</summary>
				///<param name="Str">The stream</param>
				///<param name="Offset">The byte position to start the read in the stream</param>
				///<exception cref="ULONG">The Win32 error code</exception>
				///<returns>The number of bytes deserialized</returns>
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &M, const LONG Offset)
				{
					LONG Pos=Offset;
					LONG NumCpy=0;
					NumCpy=SpectralDataImpl<struct DlfcDataS>::DeserializeData(Str, M, Pos); Pos+=NumCpy;
					HandleEndianness();
					NumCpy=_CorrSpectrum.Deserialize(Str, Pos); Pos+=NumCpy;
					return Pos-Offset;
				}

			};
		}
	}
}

