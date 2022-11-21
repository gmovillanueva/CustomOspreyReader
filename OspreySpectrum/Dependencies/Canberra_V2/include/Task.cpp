#include "Task.h"


///<summary>
///This is a utility class used below for parsing
///strings
///</summary>
///<param name="Name">The name of the task</param>
Canberra::Utility::Core::Task::Task(const String &Name) : _Name(Name), _TimeOut(120000), _State(Stopped),
					_WaitForTaskExit(true), _Lock(String(Name) + String("_BaseLock")),
					_StateOfHealth(Name) {}
Canberra::Utility::Core::Task::~Task(void) {}


//Accessors
Canberra::Utility::Core::Task::TaskState Canberra::Utility::Core::Task::GetState() {return _State;}
bool Canberra::Utility::Core::Task::GetWaitForTaskExit() {return _WaitForTaskExit;}
void Canberra::Utility::Core::Task::PutWaitForTaskExit(bool v) {_WaitForTaskExit=v;}
Canberra::Utility::Core::StateOfHealth &Canberra::Utility::Core::Task::GetStateOfHealth() {return _StateOfHealth;}
ULONG Canberra::Utility::Core::Task::GetTimeOut() {return _TimeOut;}
void Canberra::Utility::Core::Task::PutTimeOut(ULONG v) {_TimeOut=v;}
Canberra::Utility::Core::String Canberra::Utility::Core::Task::GetName() {return _Name;}
