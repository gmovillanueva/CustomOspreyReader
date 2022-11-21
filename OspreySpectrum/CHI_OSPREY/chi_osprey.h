/**
@file chi_osprey.h
*/

#ifndef CHI_OSPREY_H
#define CHI_OSPREY_H

#include <Winsock2.h>
#pragma warning( disable : 4996)

#include <stdio.h>
#include <tchar.h>
#include <string>

#include "../Dependencies/Canberra_V1/include/utilities.h"

//######################################################### Osprey Information structure
struct CST_OSPREY_INFO
{
	//==================================== Internal Paramater
	std::string		ospreyID;

	//==================================== Parameter One
	std::string		ipAddress;

	//==================================== Test
	std::string		channelBounds[10];

	std::string		calibrationSleepTime;
	std::string		acquisitionSleepTime;
};

//############################################################################# Class definition
/***/
/** Object for handling Osprey.
\author G-mo */
class CHI_OSPREY
{
public:
	//==========================================-========== Common attributes
	DevCntl::IDevicePtr							dtb;
	int											spectrumValues[2048];
	long										ospInput;
	variant_t									memGroup;
	variant_t									spectrumMode;
	variant_t									presetMode;
	variant_t									voltageStatus;
	VARIANT										args;
	std::string									user;
	std::string									password;
	std::string									ipAddress;
	CST_OSPREY_INFO*							curOsprey;		
	int											channel[6];
	int											lowerBound[6];
	int											upperBound[6];
	bool										calibrationMode;
	bool										firstCycle;

public:
	//======================-============================== Methods
	//00 ConstrDestr
	 CHI_OSPREY();
	~CHI_OSPREY();

	//01 Functions
	void					OspreyInitialize(int ospID, bool userMode);
	void					OspreyPullSpectrums(int ospID, int sleepTime);
};

#endif