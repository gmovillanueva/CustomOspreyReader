#pragma once
#include "UtilityImpExp.h"
#include "CriticalSection.h"
#include "StateOfHealth.h"
#include "LockManager.h"

#ifndef WPARAM
#define WPARAM LONG
#endif
#ifndef LPARAM
#define LPARAM LONG
#endif

namespace Canberra {
	namespace Utility {
		namespace Core {

			//
			//This is an abstract class for encapsulating a task
			//A task may be a process, thread, fiber, etc.  In
			//a nutshell some form of job to run.
			//
			class UTILITY_DLLIMPORT Task
			{
			public:
				//Want this namespace so place it here
				typedef enum TaskStateTag {
					Stopped=0,			//Task is no longer running/stopped
					Stopping=1,			//Task is in the process of stopping
					Starting=2,			//Task is in the process of starting
					Started=3			//Task is running/started
				} TaskState;

			protected:
				String _Name;						//Task name
				ULONG _TimeOut;						//Shutdown timeout value
				TaskState _State;					//State of the task
				bool _WaitForTaskExit;				//Indicates Stop should wait for task to exit
				CriticalSection _Lock;				//A lock for the task
				StateOfHealth _StateOfHealth;		//State of health information

			public:
				Task(const String &Name);
				virtual ~Task(void);

				//The name of this task
				String GetName();

				//The shutdown timeout
				ULONG GetTimeOut();
				void PutTimeOut(ULONG v);

				//The state of health information
				StateOfHealth &GetStateOfHealth();

				//This method starts the task
				virtual ULONG Start()=0;

				//This method stops the task
				virtual ULONG Stop()=0;

				//This method resume the task
				virtual ULONG PostMessage(UINT, WPARAM =0, LPARAM =0)=0;

				//This method wait for the task
				virtual ULONG Join(LONG TimeOut=0)=0;

				//This method returns the task state
				virtual TaskState GetState();

				//This method returns the state of whether Stop should wait for task to exit
				bool GetWaitForTaskExit();
				void PutWaitForTaskExit(bool v);
			};
		}
	}
}
