#ifndef __EXPOSE_TO_MIDL
#pragma once
namespace Canberra {
	namespace DataTypes {
		namespace Lynx {
			namespace ParameterValueTypes {
#endif
				typedef enum WebSecurityPrivilegesTag {
					CalibrationSetup = 2,   			//Allow access to setup calibrations
					NetworkSetup = 4,					//Allow access to setup network
					HardwareAdjust = 8,   				//Allow access to hardware adjust
					HvpsLimitSetup = 16,  				//Allow access to setup HVPS limit
					SystemSetup = 32,					//Allow access to setup system
					SecuritySetup = 64,					//Allow access to setup security
					FirmwareUpdate = 128,				//Allow access to update firmware
					AuditSetup = 256,					//Allow access to setup audit logs
					PresetSetup = 512,					//Allow access to setup acquisition
					InputDefinition = 1024,				//Allow access to define inputs
					DigitalOscilloscope = 2048,			//Allow access to DSO
					BackupAndRestore = 4096,			//Allow access to backup and restore
					TakeOver = 8192,					//Allow a user to take over an input
					Diagnostics = 16384,				//Allow access to diagnostics
				} WebSecurityPrivileges;

				//
				//This enumeration is used to define all possible
				//DSA device status bits
				//
				namespace DeviceStatus {

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
				}

				//
				//List mode acquisition options
				//
				typedef enum ListAcquisitionOptionsTag {
						TimeBase100ns=1,				//Timebase is 100ns.  If option is disabled the timebase is 1000ns
						LatchOnFastDiscPedge=2,			//Events are latched on fast discriminator positive edge.  If disabled events are latched on peak detect positive edge
						UseExternalSyncClockSrc=4,		//Enabled indicates use externally generated clock source on external sync input. Disabled indicates sync clock source is internal 1uSec clock
				} ListAcquisitionOptions;

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

				//
				//Acquisition Mode options
				//
				typedef enum AcquisitionModesTag {
					Pha=0,
					Mcs=1,
					Lfc=2,
					Dlfc=3,
					List=4,
					Tlist=5,
					Mss=6
				} AcquisitionModes;

				//
				//Mcs Discriminator Modes
				//
				typedef enum McsDiscModesTag {
						FDisc=0,
						TTL=1,
						ROI=2,
				} McsDiscModes;


				/// Contains definitions for the values that may be specified
				/// for the GPIOx_Control parameters where x has a value of 1,2,3 or 4
				typedef enum GPIOcontrolCodesTag {
					GPIOinput = 0,
					GPIOoutputLow = 1,
					GPIOoutputHigh = 2,
				} GPIOcontrolCodes;

				//Data types for the GetData() method
				typedef enum DataTypeCodesTag {
					DsoData=0,			//Digital Stoage Oscilloscope
					CounterData=1,		//Counter data
					ListData=2,			//List data
					ScaBufferData=3,	//SCA counter data
					MssData=4			//MSS data
				}DataTypeCodes;

				//Extended command codes
				typedef enum CommandCodesExTag {
					LEDdiagnostics=1008,	//MAKELONG(IOCTL_MCADRV_DIAGNOSTIC_PARAMETER, ID_DGS_RW_LED_CONTROL)
					VoltageMonitor=1003,	//MAKELONG(IOCTL_MCADRV_HARDWARE_PARAMETER, ID_HDW_RO_VOLTAGE_MONITOR)
				}CommandCodesEx;

				//Time constants
				typedef enum TimeConstantTag {
					TimeConstShort=0,
					TimeConstLong=1,
				}TimeConstant;

				//Shaper modes
				typedef enum ShaperModeTag {
					ShaperModeTriangular=0,
					ShaperModeTrapezoidal=1,
					ShaperModeGaussian=2
				}ShaperMode;

				//Polarity
				typedef enum PolarityTag {
					PositivePolarity=0,
					NegativePolarity=1
				}Polarity;

				//BLR modes
				typedef enum BLRmodeTag {
					BlrAutomatic=0,
					BlrSoft=3,
					BlrMedium=2,
					BlrHard=1,
				}BLRmode;

				//Preamp types
				typedef enum PreampTypeTag {
					RC=0,
					TRP=1
				}PreampType;

				//Fdisc shaping modes
				typedef enum FDiscShapingTag {
					Normal=0,
					LowEnergy=1
				}FDiscShaping;

				//Auto/manual
				typedef enum AutomaticManualTag {
					Automatic=0,
					Manual=1,
				}AutomaticManual;

				//On/off
				typedef enum OnOffTag {
					Off=0,
					On=1,
				}OnOff;

				//Pos/Neg
				typedef enum PositiveNegativeTag {
					Positive=0,
					Negative=1,
				}PositiveNegative;


				//Ex Control modes
				typedef enum ExternalControlModeTag {
					DSA2K=2,
					Lynx=1,
					None=0
				}ExternalControlMode;

				//Ex Control
				typedef enum ExternalControlTag {
					StartOnly=0,
					StopOnly=1,
					StartAndStop=2
				}ExternalControl;

				//Stab range
				typedef enum StabilizerCorrectionRangeTag {
					Ge=0,
					NaI=1,
				}StabilizerCorrectionRange;

				//Stab mode
				typedef enum StabilizerModeTag {
					StabOff=0,
					StabOn=1,
					StabHold=2
				}StabilizerMode;

				//Coinc Gate mode
				typedef enum CoincGateModeTag {
					CoincOff=0,
					Coinc=1,
					AntiCoinc=2
				}CoincGateMode;

				//External sync mode
				typedef enum ExternalSyncModeTag {
					SyncMasterB=2,
					SyncMaster=1,
					SyncSlave=0
				}ExternalSyncMode;

				//External sync status
				typedef enum ExternalSyncStatusTag {
					Disabled=0,
					Enabled=1,
					Independent=2
				}ExternalSyncStatus;

				//Clock source
				typedef enum ClockSourceTag {
					InternalClockSrc=0,		//Internal 1uS clock source
					ExternalClockSrc=1,		//External clock source via Ext Sync BNC
				}ClockSource;

				//SCA and Aux counters status
				typedef enum CounterStatusTag {
					IdleCntrStatus=0,		//Not acquiring (disabled)
					BusyCntrStatus=1,		//Acquiring (enabled)
					WaitingCntrStatus=32	//Waiting (armed and waiting to start)
				}CounterStatus;

				//SCA and Aux counters status
				typedef enum MonitorOutControlTag {
					DisableControl=0,	//Disabled
					SlowShaper=1,		//Slow shaper
					FastShaper=2,		//Fast shaper
					ADCAfterDelay=3		//ADC after
				}MonitorOutControl;

				typedef enum RASenableLineTag {
					rasNone = 0,				//Disable RAS
					rasDirect = 1,				//Enable the serial line
					rasModem = 2,				//Enable the modem
				} RASenableLine;

				//SCA preset mode
				typedef enum SCApresetModesTag {
					ScaPresetLive = 1,	//Preset live time
					ScaPresetReal = 2,	//Preset real time
				}SCApresetModes;

#ifndef __EXPOSE_TO_MIDL
			}
		}
	}
}
#endif
