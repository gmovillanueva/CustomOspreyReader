/**
@file chi_osprey_01_functions.cpp
*/

#include <iostream>
#include <string>
#include <fstream>
#include <iterator>
#include <sstream>
#include "../Dependencies/Canberra_V1/include/utilities.h"
#include "../CHI_VECTOR/chi_vector.h"
#include "chi_osprey.h"

extern CHI_VECTOR<CHI_OSPREY>	ospreyStack;


void CHI_OSPREY::OspreyInitialize(int ospID, bool userMode)
{
	CHI_OSPREY* currentOsprey = ospreyStack.GetItem(ospID);
	
	if (currentOsprey == NULL)
	{
		printf("NULL value pulled. \n");
		return;
	}

	this->ipAddress = currentOsprey->curOsprey->ipAddress.c_str();

	// Create instance for communication
	this->dtb = Utilities::Device();
	if (this->dtb == NULL)
	{
		printf("Unable to create device library.");
		return;
	}


	// Obtain address & opening port
	bstr_t ospreyAddress(this->ipAddress.c_str());
	printf("Working with: %s \n", (char*)ospreyAddress);
	this->dtb->Open(Utilities::getLocalAddress(ospreyAddress), ospreyAddress);
	cout << "Connecting to Osprey... \n";
	

	// Display the name of the Osprey
	cout << "You are connected to: " << Utilities::GetString((bstr_t)(dtb->GetParameter(DevCntl::Network_MachineName, (short)0))) << "\n";
	

	// Gain ownership of Osprey
	//this->dtb->LockInput(_bstr_t("administrator"), _bstr_t("password"), this->ospInput);
	this->dtb->LockInput(_bstr_t(this->user.c_str()), _bstr_t(this->password.c_str()), this->ospInput);


	// Disable acquisition
	Utilities::disableAcquisition(this->dtb, this->ospInput);
	printf("Disabling any acquisitions. \n");
	

	// Setting the acquisition mode
	//this->spectrumMode = variant_t((LONG)DevCntl::Pha);
	this->dtb->PutParameter(DevCntl::Input_Mode, this->ospInput, &this->spectrumMode);
	printf("Setting the acquisition mode. \n");
	

	// Setting up the presets
	this->dtb->PutParameter(DevCntl::Preset_Options, this->ospInput, &this->presetMode);
	printf("Setting up the presets. \n");

	// Clearing date & time
	VariantInit(&this->args);
	this->dtb->Control(DevCntl::Clear, this->ospInput, &this->args);
	printf("Setting the acquisition mode. \n");
	

	// Probe & High Voltage
	this->dtb->PutParameter(DevCntl::Input_Voltage, this->ospInput, &variant_t(750));
	this->dtb->PutParameter(DevCntl::Input_VoltageStatus, this->ospInput, &this->voltageStatus);
	printf("High Voltage Setting is on. \n");
	

	// Waiting for the High Voltage to ramp
	printf("High Voltage is ramping. \n");
	while (VARIANT_TRUE == (VARIANT_BOOL)(variant_t)(dtb->GetParameter(DevCntl::Input_VoltageRamping, this->ospInput)))
	{
		Sleep(200);
	}
	printf("High Voltage has ramped. \n");
	

	// Setting memory
	this->dtb->PutParameter(DevCntl::Input_CurrentGroup, this->ospInput, &this->memGroup);


	channel[1]	  = 0;
	channel[2]	  = 0;
	channel[3]	  = 0;
	channel[4]	  = 0;
	channel[5]	  = 0;

	for (int k = 1; k < 6; k++)
	{
		lowerBound[k] = std::stoi(currentOsprey->curOsprey->channelBounds[2*k - 2]);
		upperBound[k] = std::stoi(currentOsprey->curOsprey->channelBounds[2*k-1]);
	}

	if (userMode)
	{
		printf("Calibration Mode Enabled. \n");
		this->calibrationMode = true;
		OspreyPullSpectrums(ospID, std::stoi(currentOsprey->curOsprey->calibrationSleepTime));
	}

	return;
}



void CHI_OSPREY::OspreyPullSpectrums(int ospID, int sleepTime)
{

	CHI_OSPREY* currentOsprey = ospreyStack.GetItem(ospID);
	if (currentOsprey == NULL)
	{
		printf("NULL value pulled. \n");
		return;
	}
	
	int curID = std::stoi(currentOsprey->curOsprey->ospreyID);

	if (this->firstCycle)
	{
		// Disable acquisition
		Utilities::disableAcquisition(this->dtb, this->ospInput);

		// Start the acquisition
		this->dtb->Control(DevCntl::Start, this->ospInput, &this->args);
	}

	// Clearing internal memory
	VariantInit(&this->args);
	this->dtb->Control(DevCntl::Clear, this->ospInput, &this->args);


	Sleep(sleepTime);

	// This code gets just the spectrum from the device 
	variant_t dataValues = dtb->GetSpectrum(this->ospInput, memGroup);

	// Assigns spectrums to values
	for (int k = 1; k <= 5; ++k)
	{
		channel[k] = 0;

		for (int i = lowerBound[k]; i <= upperBound[k]; ++i)
		{
			channel[k] += (int)(Utilities::Get1DSafeArrayElement(dataValues, i));
		}
	}

	if (this->calibrationMode)
	{
		Sleep(sleepTime);

		// This code gets just the spectrum from the device 
		variant_t dataValues = dtb->GetSpectrum(this->ospInput, memGroup);

		for (int i = 0; i < Utilities::GetCount(dataValues); ++i)
		{
			this->spectrumValues[i] = (int)(Utilities::Get1DSafeArrayElement(dataValues, i)) + this->spectrumValues[i];
		}

		char fileName[100];
		std::strcpy(fileName, "Osprey");
		std::strcat(fileName, currentOsprey->curOsprey->ospreyID.c_str());
		std::strcat(fileName, "Spectrum.txt");
		
		ofstream spectrumFile;
		spectrumFile.open(fileName);
		printf("\nWriting to file. \n");
		for (int k = 0; k < 2048; k++)
		{
			spectrumFile << spectrumValues[k] << "\n";
		}
		spectrumFile.close();
		this->calibrationMode = false;
	}

	long inputMode = (LONG)dtb->GetParameter(DevCntl::Input_Mode, this->ospInput);
	
	printf("Osprey %d: %5d %5d %5d %5d %5d \n", curID,
									 channel[1], 
									 channel[2],
									 channel[3],
									 channel[4],
									 channel[5]);

}
