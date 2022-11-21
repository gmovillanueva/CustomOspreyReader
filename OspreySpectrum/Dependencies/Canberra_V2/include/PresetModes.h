#pragma once

#ifndef __EXPOSE_TO_MIDL
namespace Canberra {
	namespace DataTypes {
		namespace Lynx {
#endif

			//
			//Preset options
			//
			typedef enum PresetOptionsTag {
					PresetNone=0,
					PresetLiveTime=1,			//Live time
					PresetRealTime=2,			//Real time
					PresetSweeps=4,				//Sweeps
					PresetIntegral=8,			//Computational preset (Integral) (supported in hardware)
					PresetArea=16,				//Computational preset (Area) (not supported in hardware)
			} PresetOptions;


#ifndef __EXPOSE_TO_MIDL
		}
	}
}
#endif
