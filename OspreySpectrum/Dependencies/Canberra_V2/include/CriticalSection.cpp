#include "CriticalSection.h"
#include "DebugMacros.h"

///<summary>
///An instance of this class is used to
///Manage access to a critical section
///</summary>
Canberra::Utility::Core::CriticalSection::CriticalSection(const String &Name) : SyncObject(Name){Create();}

//This is the destructor
Canberra::Utility::Core::CriticalSection::~CriticalSection(void)
{
	if (_Initialized) Close();
}

///<summary>
///This method will invoke InitializeCriticalSection
///</summary>
///<returns>0==Success</returns>
ULONG Canberra::Utility::Core::CriticalSection::Create()
{
	if (_Initialized) {
		DEBUG_MESSAGE(_T("CriticalSection"), _T("Create"), _T("CriticalSection is already open. Must invoke Close method"));
		return (ULONG) E_FAIL;
	}
#ifdef WIN32
	InitializeCriticalSection(&_CritSection);
#else
	 pthread_mutex_init (&_CritSection, NULL);
#endif
	_Initialized = true;
	return 0;
}

///<summary>
///This method will invoke DeleteCriticalSection
///<returns>0==Success</returns>
///</summary>
ULONG Canberra::Utility::Core::CriticalSection::Close()
{
	if (!_Initialized) {
		DEBUG_MESSAGE(_T("CriticalSection"), _T("Close"), _T("CriticalSection is already closed."));
		return (ULONG) E_FAIL;
	}
	_Initialized = false;
#ifdef WIN32
	DeleteCriticalSection(&_CritSection);
#else
	 pthread_mutex_destroy(&_CritSection);
#endif
	return 0;
}

/// <summary>
///This method will invoke EnterCriticalSection
/// </summary>
/// <param name="TimeO">The time out (ms)</param>
/// <returns>true == Locked</returns>
bool Canberra::Utility::Core::CriticalSection::WaitOne(LONG TimeO)
{
	if (!_Initialized) {
		DEBUG_MESSAGE(_T("CriticalSection"), _T("WaitOne"), _T("CriticalSection is not opened"));
		return false;
	}
#ifdef WIN32
	::EnterCriticalSection(&_CritSection);
#else
	 pthread_mutex_lock(&_CritSection);
#endif
	return true;
}

///<summary>
///This method will invoke EnterCriticalSection
///<returns>0 == Locked</returns>
///</summary>
ULONG Canberra::Utility::Core::CriticalSection::Lock()
{
	if (!_Initialized) {
		DEBUG_MESSAGE(_T("CriticalSection"), _T("Lock"), _T("CriticalSection is not opened"));
		return (ULONG) E_FAIL;
	}
#ifdef WIN32
	::EnterCriticalSection(&_CritSection);
#else
	pthread_mutex_lock(&_CritSection);
#endif
	return 0;
}

///<summary>
///This method will invoke LeaveCriticalSection
///<returns>0 == Locked</returns>
///</summary>
ULONG Canberra::Utility::Core::CriticalSection::Unlock()
{
	if (!_Initialized) {
		DEBUG_MESSAGE(_T("CriticalSection"), _T("Unlock"), _T("CriticalSection is not opened"));
		return (ULONG) E_FAIL;
	}
#ifdef WIN32
	::LeaveCriticalSection(&_CritSection);
#else
	pthread_mutex_unlock(&_CritSection);
#endif
	return 0;
}


///<summary>
///This method will invoke TryEnterCriticalSection
///<returns>true == Locked</returns>
///</summary>
bool Canberra::Utility::Core::CriticalSection::TryLock()
{
	if (!_Initialized) {
		DEBUG_MESSAGE(_T("CriticalSection"), _T("TryLock"), _T("CriticalSection is not opened"));
		return true;
	}
#ifdef WIN32
	return ::TryEnterCriticalSection(&_CritSection) ? true : false;
#else
	return 0 == pthread_mutex_trylock(&_CritSection) ? true : false;
#endif
}


