/**
@file chi_osprey_00_constrdestr.cpp
*/

#include "chi_osprey.h"

//############################################################################# Default constructor
/** Default constructor.*/
CHI_OSPREY::CHI_OSPREY()
{
	this->args;

	this->ospInput			= 1;
	this->memGroup			= 1L;
	this->spectrumMode		= variant_t((LONG)DevCntl::Pha);;
	this->presetMode		= variant_t(2);
	this->voltageStatus		= true;

	this->user				= "administrator";
	this->password			= "password";
	this->ipAddress			= "";

	this->calibrationMode = false;

	this->firstCycle = true;

	for (int i = 0; i < 2048; i++)
	{
		this->spectrumValues[i] = 0;
	}
}


//############################################################################# Default destructor
/** Default destructor .*/
CHI_OSPREY::~CHI_OSPREY()
{

}