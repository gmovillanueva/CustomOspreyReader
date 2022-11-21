#pragma once
#include "UtilityImpExp.h"
#include "UtilityString.h"
#include "Diagnostics.h"

#ifdef OutputDebugString
#undef OutputDebugString
#define OutputDebugString OutputDebugString
#endif

namespace Canberra {
	namespace Utility {
		namespace Core {

			//This is a utility class used to generate debug trace output messages
			class UTILITY_DLLIMPORT Debug
			{
				protected:
					Debug(void);
					virtual ~Debug(void);
					static LONG _MaxLogSize;
					static String _LogFile;
					static String _WarnEv;
					static String _ErrorEv;
					static String _InfoEv;
					static String _Resource;
					static bool _ToConsole;
					static bool _ToFile;

				public:
					//Get the name of a trace control event
					static String &GetTypeEvent(Canberra::Utility::Core::Diagnostics::TraceType T);

					//Trace output
					static void OutputDebugString(const String &Class, const String &Method, const String &Msg, Canberra::Utility::Core::Diagnostics::TraceType T);
					static void OutputDebugString(const String &Class, const String &Method, const String &Msg, ULONG Code);

					//Log file accessors
					static String GetLogFile();
					static void PutLogFile(const String &v);

					//Max size of log file
					static LONG GetMaxLogSize();
					static void PutMaxLogSize(LONG v);

					//Debug warning event name accessors
					static String GetWarnEvent();
					static void PutWarnEvent(const String &v);

					//Debug error event name accessors
					static String GetErrorEvent();
					static void PutErrorEvent(const String &v);

					//Debug error event name accessors
					static String GetInfoEvent();
					static void PutInfoEvent(const String &v);

					//Message resource file name
					static String GetResource();
					static void PutResource(const String &v);

					//Trace output to console
					static bool GetOutputToConsole();
					static void PutOutputToConsole(bool v);

					//Trace output to file
					static bool GetOutputToFile();
					static void PutOutputToFile(bool v);
			};
		}
	}
}
