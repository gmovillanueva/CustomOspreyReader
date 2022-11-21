#pragma once

#include "Task.h"

#ifdef WIN32
#define THREADFNC_API DWORD WINAPI
#define THREADFNC_RETURN_TYPE DWORD
#define THREADHANDLE HANDLE
#else
#include <pthread.h>
#define THREADFNC_API void *
#define THREADFNC_RETURN_TYPE void *
#define THREADHANDLE pthread_t
typedef struct _MSG {
	LONG hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
} MSG;
#define WM_QUIT 0
#endif


namespace Canberra {
	namespace Utility {
		namespace Core {

			//An OS independent sleep
			UTILITY_DLLIMPORT void Sleep(LONG Ms);
			UTILITY_DLLIMPORT void Yeild();

			//
			//An instance of this class is used to manage a thread.
			//
			class UTILITY_DLLIMPORT Thread : public Task
			{
				Thread(const Thread&);
				Thread& operator=(const Thread&);

			protected:
				static THREADFNC_API _ThreadFnc(void *);
				friend THREADFNC_API _ThreadFnc(void *);
				virtual LONG ThreadFunc()=0;	//This is the method to override for your thread implementation

				LONG PeekMessage(MSG *Msg);		//Peeks at the message queue
				LONG GetMessage(MSG *Msg);		//Extracts a message from the queue
				ULONG ThreadStart();			//This starts the thread by calling ThreadFunc()
				bool _InterruptEv;				//The interrupt event used to interrupt a sleep state
				bool _StartupEv;				//The startup event used to sync Start() with the thread startup
				bool _AbortEv;					//The abort event
				THREADHANDLE _ThreadHandle;		//The thread handle
				DWORD _ThreadId;				//The thread identifier
				LONG _ExitCode;					//The thread exit code
				void *_Data;					//Extra data for platform specific features
				LONG _Priority;					//The thread priority

				//Sleeps (this can be interrupted via Interrupt() method)
				virtual ULONG Sleep(LONG Ms);

			public:
				Thread(const String &Name);
				virtual ~Thread(void);

				//Posts a message to the thread
				virtual ULONG PostMessage(UINT Message, WPARAM wParam=0, LPARAM lParam=0);

				//Starts the thread
				virtual ULONG Start();

				//Stops the thread
				virtual ULONG Stop();

				//Returns the exit code
				LONG GetExitCode();

				//Gets the thread priority
				ULONG GetPriority(LONG &);

				//Sets the thread priority
				ULONG PutPriority(LONG );

				//Waits for the thread to exit
				virtual ULONG Join(LONG TimeOut=0);

				//Interrupts the thread
				virtual ULONG Interrupt();
				virtual ULONG ResetInterrupt();

				//Returns the interrupt state
				bool GetIsInterrupted();

			};
		}
	}
}
