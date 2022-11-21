
#ifndef _DEBUGMACROS
#define _DEBUGMACROS

#include "UtilityDebug.h"
#include <exception>			//Needed for generic catch hander
#include <memory>				//Needed for generic catch hander

#ifdef DEBUG_MESSAGE
#undef DEBUG_MESSAGE
#endif

#ifdef DEBUG_ERROR_MESSAGE
#undef DEBUG_ERROR_MESSAGE
#endif

#ifdef OutputDebugString
#undef OutputDebugString
#define OutputDebugString OutputDebugString
#endif

#define DEBUG_MESSAGE(Class, Method, Message) Canberra::Utility::Core::Debug::OutputDebugString(Canberra::Utility::Core::String(Class), Canberra::Utility::Core::String(Method), Canberra::Utility::Core::String(Message), Canberra::Utility::Core::Diagnostics::Information);
#define DEBUG_TYPED_MESSAGE(Class, Method, Message, Type) Canberra::Utility::Core::Debug::OutputDebugString(Canberra::Utility::Core::String(Class), Canberra::Utility::Core::String(Method), Canberra::Utility::Core::String(Message), (Canberra::Utility::Core::Diagnostics::TraceType) Type);
#define DEBUG_ERROR_MESSAGE(Class, Method, Message, Stat) Canberra::Utility::Core::Debug::OutputDebugString(Canberra::Utility::Core::String(Class), Canberra::Utility::Core::String(Method), Canberra::Utility::Core::String(Message), (ULONG) Stat);

//Custom catch handler
#ifdef CATCHTRACE
	#undef CATCHTRACE
#endif

//Custom def of PAS__OK if not already defined
#ifndef PAS__OK
	#define PAS__OK 0
#endif


#define CATCHTRACE(_Class_DM, _Method_DM, _Stat_DM) \
		catch(Canberra::Utility::Core::String &_Msg_DM) {\
			_Stat_DM = (ULONG) EVENT_E_USER_EXCEPTION;\
			if (_Msg_DM.GetLength()) {DEBUG_ERROR_MESSAGE(_Class_DM, _Method_DM, _Msg_DM, _Stat_DM);}\
			else {DEBUG_ERROR_MESSAGE(_Class_DM, _Method_DM, _T("Exception thrown with empty Error String"), _Stat_DM);}\
		}\
		catch(HRESULT &_Err_DM) {\
			_Stat_DM = (ULONG) _Err_DM;\
			if ((0 != _Stat_DM) && (PAS__OK != _Stat_DM)) {DEBUG_ERROR_MESSAGE(_Class_DM, _Method_DM, _T("Caught exception"), _Stat_DM);}\
		}\
		catch(ULONG &_Err_DM) {\
			_Stat_DM = _Err_DM;\
			if ((0 != _Stat_DM) && (PAS__OK != _Stat_DM)) {DEBUG_ERROR_MESSAGE(_Class_DM, _Method_DM, _T("Caught exception"), _Stat_DM);}\
		}\
		catch(int &_Err_DM) {\
			_Stat_DM = _Err_DM;\
			if ((0 != _Stat_DM) && (PAS__OK != _Stat_DM)) {DEBUG_ERROR_MESSAGE(_Class_DM, _Method_DM, _T("Caught exception"), _Stat_DM);}\
		}\
		catch(short &_Err_DM) {\
			_Stat_DM = _Err_DM;\
			if ((0 != _Stat_DM) && (PAS__OK != _Stat_DM)) {DEBUG_ERROR_MESSAGE(_Class_DM, _Method_DM, _T("Caught exception"), _Stat_DM);}\
		}\
		catch(const std::bad_alloc& ) {\
			_Stat_DM = (ULONG) E_OUTOFMEMORY;\
			DEBUG_ERROR_MESSAGE(_Class_DM, _Method_DM, _T("Caught exception"), _Stat_DM);\
		}\
		catch(const std::exception& ) {\
			_Stat_DM = (ULONG) EVENT_E_INTERNALEXCEPTION;\
			DEBUG_ERROR_MESSAGE(_Class_DM, _Method_DM, _T("Caught STL exception."), _Stat_DM);\
		}\
		catch(...) {\
			_Stat_DM = (ULONG) STG_E_UNKNOWN;\
			DEBUG_ERROR_MESSAGE(_Class_DM, _Method_DM, _T("Caught Unknown exception."), _Stat_DM);\
		}

#endif
