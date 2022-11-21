//
//This file contains all of the device status bits code definitions for this application
//NOTE: This file is used on both C++ compilations along with MIDL compilations
//		Therefore, be aware of what gets put in here so both compilers are able
//		to compile this file without error or warnings
//		Several enumeration values are removed from being defined during MIDL
//		compilations.  The reason is due to the fact that they should not be
//		accessed from this env.
//

#ifndef __EXPOSE_TO_MIDL
#pragma once
namespace Canberra {
	namespace DataTypes {
		namespace Lynx {

			//
			//This enumeration is used to define all possible
			//DSA device status bits
			//
			namespace DeviceStatus {
#endif

				//Input status bits
				typedef enum StatusBitsTag {
					Idle=0,				//Input is idle
    				Busy=1,				//Input is busy acquiring
    				Fault=2,			//Input is in a fault state
					Rebooting=4,		//Device is in the process of rebooting
					UpdatingImage=8,	//Device is in the process of updating firmware
					GroupComplete=16,	//The MSS acq completed on current group
					Waiting=32,			//Waiting for external start/stop or changer ready
					Diagnosing=64,		//Diagnostics running
					APZinprog=128,		//Auto-pole zero running
					HVramping=256,		//HV is ramping
					Group1Active=512,	//Group 1 was active when acq started. When not set Group2 was active on acq start
					Disconnected=1024,	//Used by web application for disconnected state
					PresetReached=2048|4096|16384,	//The computational, time, or sweeps preset was reached
					PresetTimeReached=2048,			//Preset time reached
					PresetCompReached=4096,			//Computational Preset reached
					ExternalTriggerEvent=8192,		//External trigger event
					PresetSweepsReached=16384,		//Preset sweeps reached
					ExternalStop=32768,			//The acquisition has been stopped because external stop (BNC)
					OverflowStop=65536,			//The acquisition has been stopped because channel data overflow
					ManualStop=131072,			//The acquisition has been stopped because was commanded to
					AcqNotStarted=262144,			//Acquire not started because previous preset was reached
				} StatusBits;

				//Various fault conditions
				typedef enum FaultBitsTag {
					ResourceFault=1,				//Error allocating resources
					LvpsFault=2,					//LVPS error
					HvpsCommFault=4,				//HVPS comm. error
					ParamInitFault=8,				//Parameter init error
					Pos5VFault=16,					//+5V fault
					Neg5VFault=32,					//-5V fault
					Pos24VFault=64,					//+24V fault
					Neg24VFault=128,				//-24V fault
					Pos12VFault=512,				//+12V fault
					Neg12VFault=1024,				//-12V fault
					RTCBatFault=2048,				//RTC battery range fault
					RTCInitFault=4096,				//RTC init error
					LEDFault=8192,					//Problem detected with LEDs
					McaMemFault=16384,				//MCA memory fault
					StabORangeFault=32768,			//Stabilizer overrange fault
					StabURangeFault=65536,			//Stabilizer underrange fault
					StabFineGainChgFault=131072,	//Stabilizer fine gain changed fault
					StabRatioFault=262144,			//Stabilizer ratio error
					HVinhibitFault=524288,			//HV inhibit
					NANDFault=1048576,				//Unable to access NAND drive
					NORFault=2097152,				//Unable to access NOR drive
					RAMLowFault=4194304,			//Low program memory
					StoreLowFault=8388608,			//Low persistent memory
					BadFileSysFault=16777216,		//Persistent store is bad
					GenericFault=33554432,			//A generic fault see audit log for further details
					ExtSyncTimeO=67108864,			//Indicates that an external sync pulse was expected within the current sync timeout period but none was received
				} FaultBits;

			//Stabilizer status bits
			typedef enum StabilizerStatusBitsTag {
				StabStatusOff=0,				//Stabilizer is off
				StabStatusOn=1,					//Stabilizer is on
				StabStatusHold=2,				//Stabilizer is holding
				StabStatusFineGChng=128,		//Fine gain has been changed invalidating the centroid.
				StabStatusUnderRange=512,		//Underrange error
				StabStatusOverRange=256,		//Overrange error
				StabStatusRatioError=1024,		//Ratio error
			} StabilizerStatusBits;

			//Auto pole zero status bits
			typedef enum AutoPoleZeroStatusBitsTag {
				apzStatusInprogress=1,			//Busy performing APZ calc.
				apzStatusSucessFailure=2,		//Successed in determinination of PZ value
				apzStatusTimeOut=4,				//Timeout while attempting to converge on the PZ value
				apzStatusAborted=8,				//User aborted the APZ
				apzStatusRate2Lo=256,			//Input count rate is too low to determine PZ value
				apzStatusRate2Hi=512,			//Input count rate is too high to determine PZ value
			} AutoPoleZeroStatusBits;


#ifndef __EXPOSE_TO_MIDL
			}
		}
	}
}
#endif
