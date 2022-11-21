#include "StateOfHealth.h"
#include "Task.h"
#include "DebugMacros.h"
#include <stdlib.h>
#include <math.h>

#ifdef WINCE
#define clock() GetTickCount()
#endif

/// <summary>
///This method will intialize the watchdog information
///instance.
/// </summary>
/// <param name="Name">The name of the task to watch</param>
/// <exception cref="ULONG">The error code</exception>
Canberra::Utility::Core::StateOfHealth::StateOfHealth(const String &Name) :
		_Enabled(false), _Strobe(false), _Lock(Name+String(L"Watchdog_Lock"))
{}

Canberra::Utility::Core::StateOfHealth::~StateOfHealth(void) {}

/// <summary>
///This method will initialize the watchdog information
///instance.
/// </summary>
/// <param name="Name">The task name</param>
/// <exception cref="ULONG">The error code</exception>
void Canberra::Utility::Core::StateOfHealth::Initialize(const String &Name)
{
	_Enabled = false;
	_LastStrobed = clock();
}


/// <summary>
///This method will signal the strobed event
/// </summary>
/// <param name="State">The signal state</param>
void Canberra::Utility::Core::StateOfHealth::PutStrobed(bool State)
{
	ULONG Stat = 0;
	if (State) {
		_Strobe=true;

		LockManager LM(&_Lock);
		_LastStrobed = clock();
	}
	else _Strobe = false;
	if (Stat) {
		DEBUG_ERROR_MESSAGE(_T("StateOfHealth"), _T("PutStrobe"), _T("Error signaling strobe event"), Stat);
	}
}

/// <summary>
///This method will get the strobe events
///signal state
/// </summary>
/// <returns>The signal state</returns>
bool Canberra::Utility::Core::StateOfHealth::GetStrobed() {return _Strobe;}

/// <summary>
///This method will see if the watchdog
///policy has expired.
/// </summary>
/// <param name="TimeO">The timeout for the watchdog (ms)</param>
/// <returns>Policy expired state</returns>
bool Canberra::Utility::Core::StateOfHealth::PolicyExpired(LONG TimeO) {
	if (!_Enabled) return false;
	clock_t Now=clock();

	double TimeOutSec=TimeO;

	LockManager LM(&_Lock);
	if (abs(Now-_LastStrobed) > TimeOutSec) {
		return true;
	}
	else return false;
}

/// <summary>
///This method will get the enabled state of the
///watchdog
/// </summary>
/// <returns>Enabled state</returns>
bool Canberra::Utility::Core::StateOfHealth::GetEnabled() {return _Enabled;}


///<summary>
///This method will set the enabled state of the
///watchdog
///</summary>
///<param name="v">Enabled state</param>
void Canberra::Utility::Core::StateOfHealth::PutEnabled(bool v) {
	_Enabled = v;
	PutStrobed(v);
}
