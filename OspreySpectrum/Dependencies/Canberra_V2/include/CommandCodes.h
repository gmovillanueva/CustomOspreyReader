//
//This file contains all of the command code definitions for this application
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
			//DSA commands used to control the device
			//
			namespace CommandCodes {
#endif

#ifndef __EXPOSE_TO_MIDL
				typedef enum CodeTag {
#else
				typedef enum CommandCodeTag {
#endif

#ifndef __EXPOSE_TO_MIDL
					Lock=1,					//Lock device
					Unlock=2,				//Unlock device
#endif

					Start=3,				//Start acq
					Stop=4,					//Stop acq
					Clear=5,				//Clear the memory

#ifndef __EXPOSE_TO_MIDL
					GetParameter=6,			//Get a parameter
					PutParameter=7,			//Put a parameter
					Response=8,				//Response to a command
#endif
					Commit=9,				//Commit any changes

#ifndef __EXPOSE_TO_MIDL
					Save=10,				//Saves the current inputs data to CAM file
#endif
					ClearLog=11,			//Clears the application log
					Rollback=12,			//Rollback changes
					GetSpectralData=13,		//Gets the spectral data (time, preset, counts, etc)
					GetSpectrum=14,			//Gets the spectrum (counts only)
					PutSpectrum=15,			//Sets the spectrum (counts only)
#ifndef __EXPOSE_TO_MIDL
					GetParameterList=16,	//Returns a list of parameters for an input
					PutParameterList=17,	//Sets a list of parameters into an input
					AddUser=18,				//Adds a user account
					DeleteUser=19,			//Deletes a user account
					EnumerateUsers=20,		//Enumerates all user accounts
					GetRegionsOfInterest=21,//Gets the regions of interest
					PutRegionsOfInterest=22,//Sets the regions of interest
					UpdateUser=23,			//Updates a user account
#endif
					ResetFactoryDefaults=24,//Resets the factory defaults
					Reboot=25,				//Hard reboots the device

#ifndef __EXPOSE_TO_MIDL
					ValidateUser=26,		//Validates the user account
					GetDsoData=27,			//Gets the DSO data
					TriggerDso=28,			//Triggers the DSO
#endif
					BeginTrans=29,			//Begins a transaction for making changes
					Restore=30,				//Restores settings based on supplied file
					UpdateFirmware=31,		//Updates the firmware based on supplied file
					HvpsReset=32,			//Reset a HV power supply
					ResetButtonClear=33,	//Clear the reset button pressed status
					ArmDso=34,				//Issues the DSO arm command
					ClearFaults=35,			//Clears fault condition on and input
					SoftReboot=36,			//Soft reboots the device
					GetCounterData=37,		//Gets the counter data (does the following: latch, read, clear, unlatch)
					StartAutoPoleZero=38,	//Starts auto-pole zero
					StopAutoPoleZero=39,	//Stops auto-pole zero
					ChangerAdvance=40,		//Advances sample changer
#ifndef __EXPOSE_TO_MIDL
					GetListData=41,			//Returns the list data
					DriverControl=42,		//Driver Control Command
					GetMSSData=43,			//Gets the MSS data
#endif
					ResetStabilizer=44,		//Resets the stabilizer
					Abort=45,				//Aborts an MCS or MSS acq
					CountersClear=46,			//Clears all counters and timers
					CountersLatch=47,			//Latches counters and timers
					CountersLatchAndClear=48,	//Latches and clears counters and timers
#ifndef __EXPOSE_TO_MIDL
					GetSCAbuffer=49,			//Gets the SCA buffer
					GetSCAdefinitions=50,		//Gets the SCA definitions
					PutSCAdefinitions=51,		//Sets the SCA definitions
#endif
					SCAcountersClear=52,			//Clears all SCA counters and timers
					SCAcountersLatch=53,			//Latches SCA counters and timers
					SCAcountersLatchAndClear=54,	//Latches SCA and clears counters and timers
					StartTranferToRemovableMedia=55,	//Starts moving files from device storage to removable media
					StopTranferToRemovableMedia=56,		//Stops moving files from device storage to removable media
#ifndef __EXPOSE_TO_MIDL
				} Code;
#else
				} CommandCode;
#endif

#ifndef __EXPOSE_TO_MIDL
			}
		}
	}
}
#endif
