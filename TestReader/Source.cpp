#include<iostream>
#include<Windows.h>
#include<signal.h>
#include<math.h> //Just for test signal

//######################################################### Variables
bool closeProgram=false;
float argonCountRate=1.0;
float xenonCountRate=2.0;
double time=0.0;

//######################################################### Mapped structure
struct CST_MAPPING
{
	float			fTime;								    ///< Time in seconds 
	float			ospreyChannels[25];					    ///< Total channels for 5 ospreys			  
};

//######################################################### Close signal handler
void SigBreak_Handler(int n_signal)
{
    closeProgram=true;
}

//######################################################### Main function
int main()
{
	//===================================================== Introductory text
	std::cout << "FAM_CHANNELS Mapping program\n";

	//===================================================== Get a handle for a file mapping
	HANDLE fileHandle=OpenFileMapping(FILE_MAP_READ ,true,"FAM_CHANNELS");        // name of mapping object

	//===================================================== Handle error
	if (fileHandle==NULL)
	{
		std::cout << "ERROR: Could not create file mapping\n";
		return 0;
	}
	

	struct CST_MAPPING* newMap = (struct CST_MAPPING*)MapViewOfFile(fileHandle, FILE_MAP_READ, 0, 0, sizeof(struct CST_MAPPING));

	//===================================================== Handle error
	if (newMap==NULL)
	{
		std::cout << "ERROR: Could not map view of file\n";
		return 0;
	}

	//===================================================== Copy over initial variables

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
	            16,             // Periodic timer interval of 16 milli-seconds
	            NULL,     // Completion routine
	            NULL,          // Argument to the completion routine
	            FALSE );          // Do not restore a suspended system

	//===================================================== Assign close signal handler
	signal(SIGBREAK, &SigBreak_Handler);


	//===================================================== Start the loop
	std::cout<<"Mapping Created, running sampler.\n";
	printf("Time = %5.0f, Count Rate = %5.0f", newMap->fTime, newMap->ospreyChannels[1]);
	while (!closeProgram)
	{
		
		WaitForSingleObject(hTimer,1000);
		
		//printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
		printf("Time = %5.0f, Count Rate = %5.0f \n",newMap->fTime,newMap->ospreyChannels[1]);
	}

	//===================================================== Close the file handle
	UnmapViewOfFile(newMap);
	CloseHandle(fileHandle);

	std::cout<<" Program finished!\n";

	return 0;
}
