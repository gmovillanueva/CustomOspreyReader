#pragma once
#include "DataTypesImpExp.h"
#include "Spectrum.h"
#include "SerializableObjectCodes.h"
#include "endianUtils.h"
#include "DebugMacros.h"

//Data description for spectral acquisitions
#pragma pack(push, 1)
struct SpectralDataS {
	LONGLONG StartTime;			//The start time
	ULONG Status;				//The status of collection
	USHORT Input;				//The input number
	USHORT Group;				//The group number
};
#pragma pack(pop)

namespace Canberra {
	namespace DataTypes {
		namespace Spectroscopy {

			//
			//This is a base class that is used
			//for serializing any data type that contains
			//a spectrum
			//
			class DATATYPES_DLLIMPORT SpectralData : public Canberra::Serialization::SerializableObject
			{
			protected:
				Spectrum _Spectrum;				//The spectrum
				struct SpectralDataS _Source;	//Data that describes spectral data

			public:
				SpectralData(Canberra::DataTypes::Serialization::SerializableObjects::TypeCode Type, ULONG Chans);
				virtual ~SpectralData(void);

				//Get/set spectrum
				Spectrum &GetSpectrum();

				//Get/set start time
				LONGLONG GetStartTime();
				void PutStartTime(LONGLONG v);

				//Get/status
				ULONG GetStatus();
				void PutStatus(ULONG v);

				//Get/set input number
				USHORT GetInput();
				void PutInput(USHORT v);

				//Get/set group number
				USHORT GetGroup();
				void PutGroup(USHORT v);

			protected:
				//Returns the size
				virtual ULONG GetDataSize();
				virtual ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &, const LONG Offset);
				virtual ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &, const LONG Offset);

			};

			//
			//This is a class template that is used
			//for serializing any data type that contains
			//a spectrum
			//
			template<class Data>
			class SpectralDataImpl : public SpectralData
			{
			protected:
				Data _Data;						//Derived classes specific data

			public:
				SpectralDataImpl(Canberra::DataTypes::Serialization::SerializableObjects::TypeCode Type, ULONG Chans) :
				SpectralData(Type, Chans) {
					memset(&_Data, 0, sizeof(Data));
				}
				virtual ~SpectralDataImpl(void) {}


			protected:
				//Returns the size
				virtual ULONG GetDataSize() {return sizeof(_Data) + SpectralData::GetDataSize(); }

				//Handles the endianness of the platform. This method is invoked before and after serialization
				virtual void HandleEndianness() {}

				///<summary>
				///This method will serialize this instance into the byte stream
				///</summary>
				///<param name="Str">The stream</param>
				///<param name="MD">The metadata</param>
				///<param name="Offset">The byte position to start the write in the stream</param>
				///<exception cref="ULONG">The Win32 error code</exception>
				///<returns>The number of bytes serialized</returns>
				virtual ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &M, const LONG Offset)
				{
					LONG Pos=Offset;
					LONG NumCpy=0;
					Data CopyOfData = _Data;
					HandleEndianness();
					memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_Data, NumCpy=sizeof(_Data)); Pos+=NumCpy;
					_Data=CopyOfData;
					NumCpy=SpectralData::SerializeData(Str, M, Pos); Pos+=NumCpy;
					return Pos-Offset;
				}

				///<summary>
				///This method will deserialize info from the byte stream into this instance
				///</summary>
				///<param name="Str">The stream</param>
				///<param name="Offset">The byte position to start the read in the stream</param>
				///<exception cref="ULONG">The Win32 error code</exception>
				///<returns>The number of bytes deserialized</returns>
				virtual ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &M, const LONG Offset)
				{
					LONG Pos=Offset;
					LONG NumCpy=0;
					memcpy(&_Data, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_Data)); Pos+=NumCpy;
					HandleEndianness();
					NumCpy=SpectralData::DeserializeData(Str, M, Pos); Pos+=NumCpy;
					return Pos-Offset;
				}

			};
		}
	}
}
