#include "UtilityDebug.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#else
#include <tchar.h>
#endif
//Returns the symbol associated with this type
#define TYPE_SYMBOL(Type) ((Canberra::Utility::Core::Diagnostics::Information==Type) ? L"[I]" : ((Canberra::Utility::Core::Diagnostics::Warning==Type) ? L"[W]" : L"[X]"))

Canberra::Utility::Core::String Canberra::Utility::Core::Debug::_LogFile(L"");
Canberra::Utility::Core::String Canberra::Utility::Core::Debug::_WarnEv(L"");
Canberra::Utility::Core::String Canberra::Utility::Core::Debug::_ErrorEv(L"");
Canberra::Utility::Core::String Canberra::Utility::Core::Debug::_InfoEv(L"");
Canberra::Utility::Core::String Canberra::Utility::Core::Debug::_Resource(L"");
bool Canberra::Utility::Core::Debug::_ToConsole=false;
bool Canberra::Utility::Core::Debug::_ToFile=true;
LONG Canberra::Utility::Core::Debug::_MaxLogSize=90000;

//Constructor/destructors
Canberra::Utility::Core::Debug::Debug(void) {}
Canberra::Utility::Core::Debug::~Debug(void) {}


///<summary>
///This method may be used to generate trace output messages that are formated
///with information such as class name, method name, message, thread context,
///and date time stamp
///</summary>
///<param name="Class">The class name where this macro is called</param>
///<param name="Method">The method name where this macro is called</param>
///<param name="Message">The trace message</param>
///<param name="Type">The trace message type</param>
void Canberra::Utility::Core::Debug::OutputDebugString(const String &Class, const String &Method, const String &Message, Diagnostics::TraceType Type)
{
#if 0
	//See if client wants to filter this message
	String &Event=GetTypeEvent(Type);
	if (Event.Length) {
		HANDLE DbgEv=::OpenEvent(EVENT_ALL_ACCESS, FALSE, Event);
		if (DbgEv) {
			BOOL Filter=FALSE;
			if (WAIT_OBJECT_0!=WaitForSingleObject(DbgEv, 0)) Filter=TRUE;
			CloseHandle(DbgEv);
			if (Filter) return;
		}
	}
#endif

#ifdef WIN32
	SYSTEMTIME Time;
	GetLocalTime(&Time);
	String _Thread_XXX=String(_T(", TID: ")) + String((ULONG) GetCurrentThreadId(),String(_T("%X"))) + String(_T(", "));
	String _TimeStamp_XXX=String(_T(" [")) + String(Time) + String(_T("]"));
#else
	time_t now = time(0);
	struct tm *Time = localtime(&now);
	String _Thread_XXX=String(_T(", TID: ")) + String((ULONG) pthread_self(),String(_T("%X"))) + String(_T(", "));
	String _TimeStamp_XXX=String(_T(" [")) + String(*Time) + String(_T("]"));
#endif

	String _Type_XXX(TYPE_SYMBOL(Type));
	String _Space_XXX(_T(" "));
	String _Class_XXX(Class);
	String _Method_XXX(Method);
	String _Message_XXX(Message);
	String _Sep_XXX(_T("::"));
	String _NewLine_XXX(_T("\r\n"));
	String _Output_XXX = _Type_XXX + _TimeStamp_XXX + _Thread_XXX + _Class_XXX + _Sep_XXX + _Method_XXX + _Sep_XXX + _Message_XXX + _NewLine_XXX;
#ifdef WIN32
	::OutputDebugStringW(_Output_XXX);
	if (_ToConsole) wprintf(_Output_XXX);
#else
	perror(_Output_XXX);
	if (_ToConsole) wprintf(_Output_XXX);
#endif
	if (!_ToFile) return;

	if(_LogFile.GetLength()) {

		//See if we need to monitor file size size
		if (_MaxLogSize > 0) {
#ifdef WIN32
			bool Delete=false;
			HANDLE File = CreateFile(_LogFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (INVALID_HANDLE_VALUE != File) {
				BY_HANDLE_FILE_INFORMATION Info={0};
				if (::GetFileInformationByHandle(File, &Info)) {
					__int64 Size= (Info.nFileSizeHigh * MAXDWORD) + Info.nFileSizeLow;
					if (Size > _MaxLogSize) Delete=true;
				}
				CloseHandle(File);
			}

			//Delete it
			if (Delete) DeleteFile(_LogFile);
#else
			bool Delete=false;
			struct stat info;
			if (-1 != stat(_LogFile, &info)) {
				if (info.st_size > _MaxLogSize) Delete=true;
			}
			//Delete it
			if (Delete) unlink(_LogFile);
#endif
		}

		FILE *Stream=0;
		if ((Stream = fopen(_LogFile, "a")) == NULL) return;

		char *Data = (char *) _Output_XXX;
		int j=strlen(Data);
		for (int i=0; i<j; i++) putc(Data[i], Stream);
		fclose(Stream);
	}

}

///<summary>
///This method may be used to generate trace output messages that are formated
///with information such as class name, method name, message, thread context,
///and date time stamp
///</summary>
///<param name="Class">The class name where this macro is called</param>
///<param name="Method">The method name where this macro is called</param>
///<param name="Message">The trace message</param>
///<param name="Code">The error code</param>
void Canberra::Utility::Core::Debug::OutputDebugString(const String &Class, const String &Method, const String &Message, ULONG Code)
{
	String MsgEx=Canberra::Utility::Core::String(Message) +Canberra::Utility::Core::String(_T(" Code: ")) + Canberra::Utility::Core::String((ULONG) Code,Canberra::Utility::Core::String(_T("%X")));

#ifdef WIN32
	if (Code) {
		LONG LangId=0;
		LPVOID MessageBuffer=NULL;
		int Stat=0;

		if (_Resource.GetLength() > 0) {
			HMODULE Module = ::LoadLibrary(_Resource);
			Stat = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
								 ((NULL == Module) ? FORMAT_MESSAGE_FROM_SYSTEM : FORMAT_MESSAGE_FROM_HMODULE) |  FORMAT_MESSAGE_IGNORE_INSERTS,
								  Module, Code, LangId, (LPTSTR) &MessageBuffer, 0, 0);
			if (Module) FreeLibrary(Module);
		}

		if (0 == Stat) {
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER| FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						  NULL, Code, LangId, (LPTSTR) &MessageBuffer, 0, 0);
		}

		if (MessageBuffer) {
			MsgEx = MsgEx + Canberra::Utility::Core::String(L" Description: ") + Canberra::Utility::Core::String((TCHAR *) MessageBuffer);
			LocalFree(MessageBuffer);
		}

	}
#else
	char *ErrStr = strerror(Code);
	if (NULL != ErrStr) {
		MsgEx = MsgEx + Canberra::Utility::Core::String(L" Description: ") + Canberra::Utility::Core::String(ErrStr);
	}
#endif

	OutputDebugString(Class, Method, MsgEx, Diagnostics::Error);
}

//Accessors to the maximum logfile size
LONG Canberra::Utility::Core::Debug::GetMaxLogSize() {return _MaxLogSize;}
void Canberra::Utility::Core::Debug::PutMaxLogSize(LONG v) {_MaxLogSize=v;}

//Accessors to the logfile name
Canberra::Utility::Core::String Canberra::Utility::Core::Debug::GetLogFile() {return _LogFile;}
void Canberra::Utility::Core::Debug::PutLogFile(const String &v) {_LogFile=v;}

//Debug warning event name accessors
Canberra::Utility::Core::String Canberra::Utility::Core::Debug::GetWarnEvent() {return _WarnEv;}
void Canberra::Utility::Core::Debug::PutWarnEvent(const String &v) {_WarnEv=v;}

//Debug error event name accessors
Canberra::Utility::Core::String Canberra::Utility::Core::Debug::GetErrorEvent() {return _ErrorEv;}
void Canberra::Utility::Core::Debug::PutErrorEvent(const String &v) {_ErrorEv=v;}

//Debug information event name accessors
Canberra::Utility::Core::String Canberra::Utility::Core::Debug::GetInfoEvent() {return _InfoEv;}
void Canberra::Utility::Core::Debug::PutInfoEvent(const String &v) {_InfoEv=v;}

//Resource file name containing 3'rd party message defs
Canberra::Utility::Core::String Canberra::Utility::Core::Debug::GetResource() {return _Resource;}
void Canberra::Utility::Core::Debug::PutResource(const String &v) {_Resource=v;}

//Gets the name of the event associated with this type of diag message
Canberra::Utility::Core::String &Canberra::Utility::Core::Debug::GetTypeEvent(Canberra::Utility::Core::Diagnostics::TraceType T) {
	switch(T) {
		case Canberra::Utility::Core::Diagnostics::Warning:
			return _WarnEv;
		case Canberra::Utility::Core::Diagnostics::Error:
			return _ErrorEv;
		default:
			return _InfoEv;
	}
}

//Trace output to console
bool Canberra::Utility::Core::Debug::GetOutputToConsole() {return _ToConsole;}
void Canberra::Utility::Core::Debug::PutOutputToConsole(bool v) {_ToConsole=v;}

//Trace output to file
bool Canberra::Utility::Core::Debug::GetOutputToFile() {return _ToFile;}
void Canberra::Utility::Core::Debug::PutOutputToFile(bool v) {_ToFile=v;}
