#pragma once

#include "SyncObject.h"
#ifndef WIN32
#include <pthread.h>
#else
#include <windows.h>
#endif

namespace Canberra {
	namespace Utility {
		namespace Core {

			//
			//An instance of this class is used to create and
			//manage a critical section
			//
			class UTILITY_DLLIMPORT CriticalSection  : public SyncObject
			{
				CriticalSection(const CriticalSection&);
				CriticalSection& operator=(const CriticalSection&);

			protected:
#ifdef WIN32
				CRITICAL_SECTION _CritSection;			//The critical section
#else
				pthread_mutex_t  _CritSection;			//The critical section
#endif
			public:
				CriticalSection(const String &Name);
				virtual ~CriticalSection(void);

				//This method will allocate the critical section
				ULONG Create();

				//This method will close the critical section
				ULONG Close();

				//This method will lock the critical section
				bool WaitOne(LONG TimeO=0);

				//This method will lock the critical section
				ULONG Lock();

				//This method will unlock the critical section
				ULONG Unlock();

				//This method will test the critical section
				bool TryLock();
			};
		}
	}
}
