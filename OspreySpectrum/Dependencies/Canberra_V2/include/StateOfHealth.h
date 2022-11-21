#pragma once

#include "CriticalSection.h"
#include <time.h>

namespace Canberra {
	namespace Utility {
		namespace Core {
			class Task;
			//
			//An instance of this class is used to monitor
			//a task (process, thread, fiber, etc)
			//
			class UTILITY_DLLIMPORT StateOfHealth
			{
			protected:
				StateOfHealth(const StateOfHealth&);
				StateOfHealth& operator=(const StateOfHealth&);

			protected:
				bool _Enabled;					//State to indicate enabled
				clock_t _LastStrobed;			//Last time event was strobed
				bool _Strobe;					//The strobe event
				CriticalSection _Lock;			//The sync lock for multi-threaded apps

				//Initializes this instance
				void Initialize(const String &Name);

			public:

				StateOfHealth(Task *T);
				StateOfHealth(const String &N);
				virtual ~StateOfHealth(void);

				//Get/put state of strobe signal
				bool GetStrobed();
				void PutStrobed(bool State);

				//Get/put state of whether this instance is enabled
				bool GetEnabled();
				void PutEnabled(bool v);

				//Checks the watchdog policy
				bool PolicyExpired(LONG TimeO);

			};
		}
	}
}
