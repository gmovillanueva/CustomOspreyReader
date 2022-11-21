#include "UtilityThread.h"
#include "DebugMacros.h"
#include "LockManager.h"

#ifndef WIN32
#include <unistd.h>
#define GetExitCodeThread(a, b) ((0 != (*b = _ExitCode)) ? true : true)
#define GetCurrentThreadId() pthread_self()
#define ExitThread(c) pthread_exit(c)
#define STILL_ACTIVE (LONG) -1
#define GetLastError() errno
#define THREAD_PRIORITY_NORMAL  -1
#include <deque>
#include <sched.h>
#endif

#ifdef WINCE
#pragma warning(disable:4018)
#define clock() GetTickCount()
#endif

#define MAX_QUEUESIZE	5000

///<summary>
///This method will sleep for a specified # of milliseconds
///</summary>
///<param name="Ms">The sleep time (ms)</param>
void Canberra::Utility::Core::Sleep(LONG Ms)
{
#ifdef WIN32
	::Sleep(Ms);
#else
	usleep(Ms*1000);
#endif
}
///<summary>
///This method will sleep for a specified # of milliseconds
///</summary>
void Canberra::Utility::Core::Yeild()
{
#ifdef WIN32
	::Sleep(0);
#else
	sched_yield();
#endif
}

///<summary>
///This method initializes this instance with the supplied information
///</summary>
///<param name="Name">Name to associated with this instance</param>
///<exception cref="ULONG">Win32 error code</exception>
Canberra::Utility::Core::Thread::Thread(const String &Name) : Task(Name),
										_InterruptEv(false), _StartupEv(false), _AbortEv(false),
										_ThreadHandle(0), _ThreadId(0),
										_ExitCode(0), _Data(NULL),
										_Priority(THREAD_PRIORITY_NORMAL)
{
#ifndef WIN32
	_Data = new std::deque<MSG>();
	if (NULL == _Data) {
		DEBUG_ERROR_MESSAGE(_T("Thread"), _T("Thread"), _T("Unable to allocate message queue"), E_OUTOFMEMORY);
		throw ((ULONG) E_OUTOFMEMORY);
	}
#endif
}

///<summary>
///This method releases all resources allocated to this instance
///</summary>
Canberra::Utility::Core::Thread::~Thread(void)
{
	_WaitForTaskExit = true;
	Stop();

#ifndef WIN32
	if (NULL != _Data) {
		std::deque<MSG> *Queue=(std::deque<MSG> *)_Data;
		delete Queue;
		_Data = NULL;
	}
#endif
}
///<summary>
///This method is used to start the thread.  It will
///not return until the thread has been initialized
///and invoked.  See CreateThread
///</summary>
///<returns>0==Success</returns>
ULONG Canberra::Utility::Core::Thread::Start()
{
	LONG Stat=0;
	if (_ThreadHandle) {
		if (STILL_ACTIVE == GetExitCode()) {
			DEBUG_MESSAGE(_T("Thread"), _T("Start"), _T("Thread is still running"));
			return((ULONG) E_FAIL);
		}
		else _ThreadHandle = 0;
	}

	_AbortEv = false;
	_StartupEv=false;
	_InterruptEv=false;
	_State = Task::Starting;
	DEBUG_MESSAGE(_T("Thread"), _T("Start"), String(_T("Starting Thread: Name ="))+ String(_Name));

	_ThreadId = 0;
	_ExitCode = STILL_ACTIVE;
#ifdef WIN32
	_ThreadHandle = CreateThread(NULL, 0, _ThreadFnc, (void *) this, CREATE_SUSPENDED, &_ThreadId);
	if (NULL == _ThreadHandle) {
		LONG Stat = GetLastError();
		DEBUG_ERROR_MESSAGE(_T("Thread"), _T("Start"), String(_T("Failed to create thread: ")) + String(_Name), Stat);
		return Stat;
	}
	if (!SetThreadPriority(_ThreadHandle, _Priority)) {
		Stat = GetLastError();
		DEBUG_ERROR_MESSAGE(_T("Thread"), _T("Start"), String(_T("Failed to set priority for thread: ")) + String(_Name), Stat);
	}
	if (((DWORD) -1) == ResumeThread(_ThreadHandle)) {
		Stat = GetLastError();
		DEBUG_ERROR_MESSAGE(_T("Thread"), _T("Start"), String(_T("Failed to resume thread: ")) + String(_Name), Stat);
		return Stat;
	}
#else

	bool setPriority=false;
	pthread_attr_t tattr;
	sched_param param;

	//See if we need to change the priority level
	if (THREAD_PRIORITY_NORMAL != _Priority) {
		//Initialized with default attributes
		Stat = pthread_attr_init (&tattr);
		if (Stat) {DEBUG_ERROR_MESSAGE(_T("Thread"), _T("Start"), String(_T("Failed to initialize thread attributes for thread: ")) + String(_Name), Stat);}
		else {
			//Get existing scheduling parameters
			Stat = pthread_attr_getschedparam (&tattr, &param);
			if (Stat) {DEBUG_ERROR_MESSAGE(_T("Thread"), _T("Start"), String(_T("Failed to get schedule parameters for thread: ")) + String(_Name), Stat);}
			else {
				int policy=SCHED_RR;
				Stat = pthread_attr_setschedpolicy(&tattr, policy);
				if (Stat) {
					DEBUG_ERROR_MESSAGE(_T("Thread"), _T("Start"), String(_T("Failed to set thread schedule policy for thread: ")) + String(_Name), Stat);
				}

				//Set the priority; others are unchanged
				int max=sched_get_priority_max(policy);
				int min=sched_get_priority_min(policy);
				if (_Priority > max) _Priority=max-1;
				if (_Priority < min) _Priority=min;
				param.sched_priority = _Priority;

				//Set the new scheduling parameters
				Stat = pthread_attr_setschedparam (&tattr, &param);
				if (Stat) {DEBUG_ERROR_MESSAGE(_T("Thread"), _T("Start"), String(_T("Failed to set schedule parameters for thread: ")) + String(_Name), Stat);}
				else setPriority=true;
			}
		}
	}

	Stat=pthread_create(&_ThreadHandle, setPriority ? &tattr : NULL, _ThreadFnc, (void *) this);
	if (Stat) {
		_ExitCode = 0;
		DEBUG_ERROR_MESSAGE(_T("Thread"), _T("Start"), String(_T("Failed to create thread: ")) + String(_Name), Stat);
		return Stat;
	}
	_ThreadId = (DWORD) _ThreadHandle;
#endif

	//Wait for the message queue to get created
	clock_t StartTicks=clock();
	bool Loop=!_StartupEv;
	while(Loop) {
		Loop = ((clock()-StartTicks) < (clock_t) _TimeOut) ? !_StartupEv : false;
#ifdef WIN32
		Canberra::Utility::Core::Sleep(100);
#else
		sched_yield();
#endif
	}
	_StartupEv = false;

	//Set strobe stated of health
	_StateOfHealth.PutStrobed(true);

	return 0;
}

///<summary>
///This method will get the thread priority
///</summary>
///<param name="Priority">The priority</param>
///<returns>0==Success</returns>
ULONG Canberra::Utility::Core::Thread::GetPriority(LONG &Priority)
{
	ULONG Stat=0;
	if (!_ThreadHandle) {
#ifndef WIN32
		int policy=SCHED_RR;
		int max=sched_get_priority_max(policy);
		int min=sched_get_priority_min(policy);
		Priority = (max-min)/2 + min;
#else
		Priority=_Priority;
#endif
		return Stat;
	}
#ifdef WIN32
	LONG val = GetThreadPriority(_ThreadHandle);
	if (THREAD_PRIORITY_ERROR_RETURN == val) {
		Stat = GetLastError();
		DEBUG_ERROR_MESSAGE(_T("Thread"), _T("GetPriority"), String(_T("Failed to get thread priority for thread: ")) + String(_Name), Stat);
	}
	else Priority = val;
#else
	int policy;
	struct sched_param param;
	Stat = pthread_getschedparam(_ThreadHandle, &policy, &param);
	if (Stat) {
		DEBUG_ERROR_MESSAGE(_T("Thread"), _T("GetPriority"), String(_T("Failed to get thread priority for thread: ")) + String(_Name), Stat);
	}
	else Priority = param.sched_priority;
#endif

	return Stat;
}


///<summary>
///This method will set the thread priority
///</summary>
///<param name="Priority">The priority</param>
///<returns>0==Success</returns>
ULONG Canberra::Utility::Core::Thread::PutPriority(LONG Priority)
{
	ULONG Stat=0;
	if (!_ThreadHandle) {
		_Priority = Priority;
		return Stat;
	}

#ifdef WIN32
	if (((DWORD)-1) == SetThreadPriority(_ThreadHandle, Priority)) {
		Stat = GetLastError();
		DEBUG_ERROR_MESSAGE(_T("Thread"), _T("PutPriority"), String(_T("Failed to set priority for thread: ")) + String(_Name), Stat);
	}
	else _Priority = Priority;
#else

	int policy;
	struct sched_param param;
	Stat = pthread_getschedparam(_ThreadHandle, &policy, &param);
	if (Stat) {
		DEBUG_ERROR_MESSAGE(_T("Thread"), _T("PutPriority"), String(_T("Failed to get thread priority for thread: ")) + String(_Name), Stat);
	}
	else {
		param.sched_priority = Priority;
		Stat = pthread_setschedparam(_ThreadHandle, policy, &param);
		if (Stat) {
			DEBUG_ERROR_MESSAGE(_T("Thread"), _T("PutPriority"), String(_T("Failed to set thread priority for thread: ")) + String(_Name), Stat);
		}
		else _Priority = Priority;
	}

#endif
	return Stat;
}

///<summary>
///This method will wait for the thread to complete for
///a specified number of milliseconds
///</summary>
///<param name="TimeO">The timeout in ms</param>
///<returns>0==Success</returns>
ULONG  Canberra::Utility::Core::Thread::Join(LONG TimeO) {
	if (!_ThreadHandle) {
		DEBUG_MESSAGE(_T("Thread"), _T("Join"), _T("Thread is not running"));
		return((ULONG) E_FAIL);
	}
	clock_t StartTicks=clock();
	bool Loop=(STILL_ACTIVE == _ExitCode);
	while(Loop) {
		if (TimeO <= 0) {
			Loop = (STILL_ACTIVE == _ExitCode);
		}
		else {
			Loop = ((clock()-StartTicks) < (clock_t) TimeO) ? (STILL_ACTIVE == _ExitCode) : false;
		}
#ifdef WIN32
		Canberra::Utility::Core::Sleep(100);
#else
		sched_yield();
#endif
	}
	return 0;
}

///<summary>
///This method is used to stop the thread.  It will
///not return until the thread has been stopped.  The
///stop process is performed by posting the WM_QUIT message
///and signaling the interrupt event.  It then waits for
///the specified time out or thread to exit.  If it does
///not exit it will terminate the thread.  All resources
///are released
///</summary>
///<returns>0==Success</returns>
ULONG Canberra::Utility::Core::Thread::Stop() {

	if (!_ThreadHandle) {
		DEBUG_MESSAGE(_T("Thread"), _T("Stop"), _T("Thread is not running"));
		return((ULONG) E_FAIL);
	}

	_State = Task::Stopping;
	DEBUG_MESSAGE(_T("Thread"), _T("Stop"), String(_T("Stopping Thread: Name ="))+ String(_Name));

	//Force the thread to wake up
	_AbortEv=true;
	Interrupt();

	//Get the exit code
	LONG ThreadEC = GetExitCode();
	if (STILL_ACTIVE == ThreadEC) {

		//Tell the thread to stop
		if (PostMessage(WM_QUIT)) {
			//This may not be errored because thread may have already exited
			DEBUG_MESSAGE(_T("Thread"), _T("Stop"), String(_T("Warning thread is suspended during shutdown. Name ="))+ String(_Name));
		}

		//See wait till thread has exited
		if (_WaitForTaskExit) {
			Join(_TimeOut);
		}

		ThreadEC = GetExitCode();
	}

	//See if we need to terminate
	if ((STILL_ACTIVE == ThreadEC) && _WaitForTaskExit) {
		DEBUG_MESSAGE(_T("Thread"), _T("Stop"), String(_T("Thread STILL ACTIVE.  Calling Terminate thread. Name ="))+ String(_Name));
#ifdef WIN32
		TerminateThread(_ThreadHandle, (DWORD) -1);
#else
		pthread_cancel(_ThreadHandle);
#endif
		_State = Task::Stopped;
	}
	else if (STILL_ACTIVE != ThreadEC) {
		if (ThreadEC) {DEBUG_ERROR_MESSAGE(_T("Thread"), _T("Stop"), String(_T("Thread exited. Name ="))+ String(_Name), ThreadEC);}
		else {DEBUG_MESSAGE(_T("Thread"), _T("Stop"), String(_T("Thread exited. Name ="))+ String(_Name));}
	}
	else {
		DEBUG_MESSAGE(_T("Thread"), _T("Stop"), String(_T("Posted quit message to thread and it is still active. Name ="))+ String(_Name));
	}

	_ThreadHandle = 0;
	_ThreadId=0;

	return 0;
}

///<summary>
///This method is used to setup the thread context
///and invoke the derived implementation within that
///thread context
///</summary>
///<returns>0==Exit code</returns>
ULONG Canberra::Utility::Core::Thread::ThreadStart()
{
	LONG Stat=0;
	_State = Task::Started;
	try {

		//Create the message queue (old Win32 help stated this was a way to force a creation in new thread)
		MSG Msg={0};
		PeekMessage(&Msg);

		//Signal the startup event to indicate we are all set
		_StartupEv = true;

		//Call the derived classes method
		_ExitCode = ThreadFunc();
	}
	CATCHTRACE(_T("Thread"), _T("ThreadStart"), Stat);
	_State = Task::Stopped;
	return _ExitCode;
}
///<summary>
///This method is invoked on a
///separate thread context.   It
///will invoke ThreadStart to setup
///this instance and call the derived
///implementation
///</summary>
///<param name="pArgs">The arguments</param>
///<returns>Exit code</returns>
THREADFNC_API Canberra::Utility::Core::Thread::_ThreadFnc(void *pArgs)
{
	Thread *Task = reinterpret_cast<Thread*>(pArgs);
	THREADFNC_RETURN_TYPE Stat=0;
	Task->ThreadStart();
	ExitThread(Stat);
	return Stat;
}
///<summary>
///This method will return the exit code
///of the thread.  If it is still running
///it returns STILL_RUNNING.  See GetExitCodeThread
///</summary>
///<returns>Exit code</returns>
LONG Canberra::Utility::Core::Thread::GetExitCode() {
	if (!_ThreadHandle) {
		DEBUG_MESSAGE(_T("Thread"), _T("GetExitCode"), _T("Thread is not running"));
		return 0;
	}

	LONG Stat=_ExitCode;

#ifdef WIN32
	LONG Ret=GetExitCodeThread(_ThreadHandle, (DWORD *) &Stat);
	if (!Ret) {
		LONG LastError = GetLastError();
		DEBUG_ERROR_MESSAGE(_T("Thread"), _T("GetExitCode"), String(_T("Error getting exit code for thread. Name=")) + String(_Name), LastError);
		Stat=(ULONG) E_FAIL;
	}

#endif
	return Stat;
}

///<summary>
///This method will sleep for a time interval (ms)
///The thread managed by this instance should invoke
///this method such that it can respond to the interrupt
///event
///</summary>
///<param name="Ms">The sleep time (ms)</param>
///<returns>0==Success</returns>
ULONG Canberra::Utility::Core::Thread::Sleep(LONG Ms) {
	clock_t StartTicks=clock();
	bool Loop=!_InterruptEv;
	while(Loop) {
		Loop = ((clock()-StartTicks) < (clock_t)  Ms) ? !_InterruptEv : false;
#ifdef WIN32
		Canberra::Utility::Core::Sleep(100);
#else
		sched_yield();
		Canberra::Utility::Core::Sleep(100);
#endif
	}
	return 0;
}
///<summary>
///This method will get the state of the interrupt
///event
///</summary>
///<returns>State of the event</returns>
bool Canberra::Utility::Core::Thread::GetIsInterrupted()
{
	return _InterruptEv;
}

///<summary>
///This method will set the state of the interrupt
///event to signaled
///</summary>
///<returns>0==Success</returns>
ULONG Canberra::Utility::Core::Thread::Interrupt() {
	_InterruptEv = true;
	return 0;
}

///<summary>
///This method will reset the state of the interrupt
///event to signaled
///</summary>
///<returns>0==Success</returns>
ULONG Canberra::Utility::Core::Thread::ResetInterrupt() {
	_InterruptEv=false;
	return 0;
}


///<summary>
///This method will post a message to the thread
///</summary>
///<param name="Message">The message code</param>
///<param name="wParam">The wParam</param>
///<param name="lParam">The lParam</param>
///<returns>0==Success</returns>
ULONG Canberra::Utility::Core::Thread::PostMessage(UINT Message, WPARAM wParam, LPARAM lParam) {

	if (!_ThreadHandle) {
		DEBUG_MESSAGE(_T("Thread"), _T("PostMessage"), _T("Thread is not running"));
		return (ULONG) E_FAIL;
	}
#if WIN32
	if (!PostThreadMessage(_ThreadId, Message, wParam, lParam)) {
		LONG Stat=GetLastError();
		DEBUG_ERROR_MESSAGE(_T("Thread"), _T("PostMessage"), String(_T("Error posting message to thread. Name=")) + String(_Name), Stat);
		return Stat;
	}
	return 0;
#else
	LockManager LM(&_Lock);
	std::deque<MSG> *Queue = (std::deque<MSG> *) _Data;
	if (Queue->size() > MAX_QUEUESIZE) {
		LONG Stat=E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("Thread"), _T("PostMessage"), String(_T("Too many unprocessed messages.  Error posting message to thread. Name=")) + String(_Name), Stat);
		return Stat;
	}
	MSG Msg={0};
	Msg.wParam = wParam;
	Msg.lParam = lParam;
	Queue->push_back(Msg);
	return 0;
#endif
}


///<summary>
///This method will extract a message from the message queue
///</summary>
///<param name="Msg">The message</param>
///<returns>0 Indicates WM_QUIT was received. Non zero indicates success</returns>
LONG Canberra::Utility::Core::Thread::GetMessage(MSG *Msg)
{
#ifndef WIN32
	LockManager LM(&_Lock, false);
	std::deque<MSG> *Queue = (std::deque<MSG> *) _Data;
	bool Loop=!_AbortEv;
	while(Loop) {
		Loop = _AbortEv;
		Canberra::Utility::Core::Sleep(250);
		LM.Lock();
		if (Queue->size() > 0) {
			*Msg = Queue->at(0);
			Queue->pop_front();
			if (WM_QUIT == Msg->message) return 0;
			else return 1;
		}
		LM.Unlock();
	}
	return 0;
#else
	return ::GetMessage(Msg, NULL, 0, 0);
#endif
}

///<summary>
///This method will peek at the message queue without removing any
///messages
///</summary>
///<param name="Msg">The first message in the queue if available</param>
///<returns>0 Indicates no messages available.  Non-zero indicates message available</returns>
LONG Canberra::Utility::Core::Thread::PeekMessage(MSG *Msg)
{
#ifndef WIN32
	LockManager LM(&_Lock);
	std::deque<MSG> *Queue = (std::deque<MSG> *) _Data;
	if (Queue->size() > 0) {
		if (Msg) *Msg = Queue->at(0);
		return 1;
	}
	return 0;
#else
	return ::PeekMessage(Msg, NULL, 0, 0, PM_NOREMOVE);
#endif
}

