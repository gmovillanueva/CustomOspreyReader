#pragma once
#include "SerializableObject.h"
namespace Canberra {
	namespace DataTypes {
		namespace Serialization {
			namespace SerializableObjects {
				typedef enum TypeCodeTag {
					PhaData=Canberra::Serialization::SerializableObject::CustomTypeOffset+1,				//Data for PHA
					LfcData=Canberra::Serialization::SerializableObject::CustomTypeOffset+2,				//Data for LFC
					McsData=Canberra::Serialization::SerializableObject::CustomTypeOffset+3,				//Data for MCS
					TlistData=Canberra::Serialization::SerializableObject::CustomTypeOffset+4,				//Timestamped list mode
					SpectralData=Canberra::Serialization::SerializableObject::CustomTypeOffset+5,			//Simple spectrum (chans/counts)
					CommandData=Canberra::Serialization::SerializableObject::CustomTypeOffset+6,			//Device Command
					ParameterData=Canberra::Serialization::SerializableObject::CustomTypeOffset+7,			//A parameter
					ParameterMetaData=Canberra::Serialization::SerializableObject::CustomTypeOffset+8,		//A parameter's metadata
					RegionOfInterestData=Canberra::Serialization::SerializableObject::CustomTypeOffset+9,	//A region of interest
					DsoData=Canberra::Serialization::SerializableObject::CustomTypeOffset+10,				//DSO sample data
					CounterData=Canberra::Serialization::SerializableObject::CustomTypeOffset+11,			//Counter data
					DlfcData=Canberra::Serialization::SerializableObject::CustomTypeOffset+12,				//Data for DLFC
					ListData=Canberra::Serialization::SerializableObject::CustomTypeOffset+13,				//List mode
					SCAdefinitionData=Canberra::Serialization::SerializableObject::CustomTypeOffset+14,		//SCA definition data
					SCAbufferData=Canberra::Serialization::SerializableObject::CustomTypeOffset+15,			//SCA buffer data
				} TypeCode;
			}
		}
	}
}
