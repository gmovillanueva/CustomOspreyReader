/**
@file main.cpp
*/

//========================================================= Allows usage of windows features
#ifndef _WIN32_WINNT        
#define _WIN32_WINNT 0x0501
#endif		

//========================================================= System Headers
#include <iostream>
#include <fstream>
#include <signal.h>
#include <math.h>
#include <string>
#include <time.h>
#include <string>
#include <chrono>

//========================================================= Library Linking
#pragma comment(lib,"Ws2_32" )
#pragma comment(lib,"iphlpapi" )

//========================================================= Including Class Headers
#include "CHI_OSPREY\chi_osprey.h"
#include "CHI_VECTOR\chi_vector.h"



//========================================================= Structure for Mapping
/** Structure initializing information for memory mapping. */
struct CST_MAPPING
{
	float			fTime;								    ///< Time in seconds 
	float			ospreyChannels[25];					    ///< Total channels for 5 ospreys			  
};

//========================================================= Function Prototypes
void					OspreyParseInput(std::string fileName);
std::string				OspreyExtractString(std::string str, char beg, char end);
void					SigBreak_Handler(int n_signal);


//========================================================= Variables
bool loopCount = true;									   ///< Checks to see if the program has looped before
bool closeProgram = false;								   ///<	Handle to close the program
bool calibrationMode = false;							   ///< Usermode if it should be in calibration mode or run mode.
CHI_OSPREY osprey[5];									   ///< Array of ospreys.
CHI_VECTOR<CHI_OSPREY>	ospreyStack;					   ///< Osprey Object.



//========================================================= Main function
/** Main function
\author Guillermo Villanueva */
int main()
{
	auto startTime = std::chrono::steady_clock::now();

	//===================================================== Introductory text
	printf("FAM Channels Mapping Program \n");

	//===================================================== Get a handle for a file mapping
	HANDLE fileHandle=CreateFileMapping(	INVALID_HANDLE_VALUE,
                                            NULL,                    // default security
                                            PAGE_READWRITE,          // read/write access
                                            0,                       // maximum object size (high-order DWORD)
                                            8,                       // maximum object size (low-order DWORD)
                                            "FAM_CHANNELS");        // name of mapping object)

	//===================================================== Handle error
	if (fileHandle==NULL)
	{
		std::cout << "ERROR: Could not create file mapping\n";
		return 0;
	}
	
	//===================================================== Creating a new map to copy over.
	CST_MAPPING* newMap = (CST_MAPPING*)MapViewOfFile(		fileHandle,             // handle to map object
                                             FILE_MAP_ALL_ACCESS,   // read/write permission
                                             0,
                                             0,
                                             sizeof(CST_MAPPING));

	//===================================================== Handle error
	if (newMap==NULL)
	{
		std::cout << "ERROR: Could not map view of file\n";
		return 0;
	}

	//===================================================== Copy over initial variables
	CST_MAPPING intValues;
	intValues.fTime = 0;
	for (int i = 0; i < 25; i++)
	{
		intValues.ospreyChannels[i] = 0;
	}

	CopyMemory(newMap, &intValues, sizeof(CST_MAPPING));


	//===================================================== Create a windows waitable timer (minimizes CPU usage)
	HANDLE hTimer = CreateWaitableTimer(NULL,                   // Default security attributes
	           	   	   	   	   	   	    NULL,                  // Create auto-reset timer
										TEXT("FAMTIMER"));       // Name of waitable timer

	//===================================================== Set timer parameters
	__int64         qwDueTime;
	LARGE_INTEGER   liDueTime;
	qwDueTime = -0 * 10000000;
	liDueTime.LowPart  = (DWORD) ( qwDueTime & 0xFFFFFFFF );
	liDueTime.HighPart = (LONG)  ( qwDueTime >> 32 );

	BOOL bSuccess = SetWaitableTimer(
	            hTimer,           // Handle to the timer object
	            &liDueTime,       // When timer will become signaled
	            16,					 // Periodic timer interval of 16 milli-seconds
	            NULL,				// Completion routine
	            NULL,          // Argument to the completion routine
	            FALSE );          // Do not restore a suspended system

	//===================================================== Assign close signal handler
	signal(SIGBREAK, &SigBreak_Handler);


	//===================================================== Running Script
	int acquisitionTime = 500;
	int ospreyCount = 2;

	OspreyParseInput("init.ini");

	
	for(int k = 0; k < ospreyCount; k++)
	{
		CHI_OSPREY* currentOsprey = ospreyStack.GetItem(k);
		if (std::stoi(currentOsprey->curOsprey->calibrationSleepTime) >= 10000)
		{
			calibrationMode = true;
		}
		else
		{
			calibrationMode = false;
		}

		osprey[k].OspreyInitialize(k, calibrationMode);
	}

	while (loopCount)
	{

		for (int k = 0; k < ospreyCount; k++)
		{
			CHI_OSPREY* currentOsprey = ospreyStack.GetItem(k);
			osprey[k].OspreyPullSpectrums(k, std::stoi(currentOsprey->curOsprey->acquisitionSleepTime));
			
			for (int i = 0; i < 5; i++)
			{
				intValues.ospreyChannels[5 * k + i] = osprey[k].channel[i+1];
			}
		
		}

		auto currentTime = std::chrono::steady_clock::now();
		intValues.fTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
		
		CopyMemory(newMap, &intValues, sizeof(CST_MAPPING));
		printf("Time: %f \n", intValues.fTime);

		if (HIBYTE(GetAsyncKeyState(VK_RETURN))) 
		{
			loopCount = false;
		}
	}



	//===================================================== Close the file handle
	UnmapViewOfFile(newMap);
	CloseHandle(fileHandle);

	std::cout<<"Program finished!\n";

	return 0;
}




//========================================================= Utility Function
/** \brief  Utility Function used for extracting a substring.


The function extracts a substring from a given string, that is between two delimiters.

\param str The main string.
\param beg The beginning delimiter.
\param end The ending delimiter.

\author Guillermo Vilanueva */
std::string OspreyExtractString(std::string str, char beg, char end)
{
	std::size_t begPos;
	if ((begPos = str.find(beg)) != std::string::npos)
	{
		std::size_t endPos;
		if ((endPos = str.find(end, begPos)) != std::string::npos && endPos != begPos + 1)
			return str.substr(begPos + 1, endPos - begPos - 1);
	}

	return std::string();
}



//========================================================= Utility Function
/** \brief  Utility Function used for parsing the input file.


The function extracts the information provided by the input file, and stores the found information
into a structure. 

\param fileName The file to be parsed.

\author Guillermo Vilanueva */
void OspreyParseInput(std::string fileName)
{
	//==================================== Defining pointer array
	CST_OSPREY_INFO* newOsprey;
	std::string parserStruct[14];


	//==================================== Declaring variables
	int indexCount = 0;
	int ospreyCount = 0;
	std::string currentLine;

	//==================================== Declaring filestream & opening file
	std::ifstream inputFile;
	inputFile.open(fileName);

	//==================================== Checking to see if input failed
	if (!inputFile.is_open())
	{
		printf("File: %s failed to load. \n", fileName.c_str());
		return;
	}
	else
	{
		printf("File: %s succeeded to load. \n", fileName.c_str());
	}

	//==================================== Searchs through loaded file.
	while (std::getline(inputFile, currentLine))
	{
		//==================================== Declaring variables
		size_t decCount;
		size_t comCount;		
		size_t tCount;			

		std::string mainString;
		std::string subStringOne;
		std::string subStringTwo;
		std::string subStringThree;

		//==================================== Parsing lines
		mainString = OspreyExtractString(currentLine, '(', ')');
		subStringOne = mainString.substr(0, mainString.find(','));
		subStringTwo = mainString.substr(mainString.find(',') + 1);
		subStringThree = mainString.substr(0, mainString.find('t'));

		//==================================== Counting Occurences
		decCount = std::count(mainString.begin(), mainString.end(), '.');
		comCount = std::count(mainString.begin(), mainString.end(), ',');
		tCount = std::count(mainString.begin(), mainString.end(), 't');

		//==================================== Checking for delimiter
		if (currentLine.find('{') != std::string::npos)
		{
			newOsprey = new CST_OSPREY_INFO;
			ospreyCount = ospreyCount + 1;
			parserStruct[indexCount] = std::to_string(ospreyCount);
			indexCount = indexCount + 1;
		}

		//==================================== Checking for delimiter
		if (decCount >= 3)
		{
			parserStruct[indexCount] = mainString;
			indexCount = indexCount + 1;
		}

		//==================================== Checking for delimiter
		if (comCount >= 1)
		{
			parserStruct[indexCount] = subStringOne;
			indexCount = indexCount + 1;
			parserStruct[indexCount] = subStringTwo;
			indexCount = indexCount + 1;
		}

		//==================================== Checking for delimiter
		if (tCount >= 1)
		{
			parserStruct[indexCount] = subStringThree;
			indexCount = indexCount + 1;
			
		}



		//==================================== Checking for delimiter & pushing to stack
		if (currentLine.find('}') != std::string::npos)
		{
			newOsprey->ospreyID = parserStruct[0];
			newOsprey->ipAddress = parserStruct[1];
			for (int k = 0; k < 10; k++)
			{
				newOsprey->channelBounds[k] = parserStruct[k+2];
			}
			newOsprey->acquisitionSleepTime = parserStruct[12];
			newOsprey->calibrationSleepTime = parserStruct[13];

			CHI_OSPREY* newOspreyObject = new CHI_OSPREY;
			newOspreyObject->curOsprey = newOsprey;
			ospreyStack.PushItem(newOspreyObject);


			indexCount = 0;
		}
	}

	//==================================== Closing file
	inputFile.close();
	return;
}



//========================================================= Utility Function
/** \brief  Utility Function used for exiting the program.


A signal is passed which calls for the program to be closed.

\param n_signal The signal to be passed.

\author Guillermo Vilanueva */
void SigBreak_Handler(int n_signal)
{
	closeProgram = true;
}