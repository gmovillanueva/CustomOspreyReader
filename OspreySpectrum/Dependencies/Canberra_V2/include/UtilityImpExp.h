#pragma once

#if defined(WIN32) && defined(BUILD_DLL)
#ifdef UTILITY_DLLIMPORT
#undef UTILITY_DLLIMPORT
#endif

#ifdef EXPIMP_TEMPLATE
#undef EXPIMP_TEMPLATE
#endif

#ifdef UTILITY_EXPORTS
#define UTILITY_DLLIMPORT __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define UTILITY_DLLIMPORT __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#endif
#else
#define UTILITY_DLLIMPORT
#define EXPIMP_TEMPLATE
#endif

#include "platformTypes.h"
#include "winerror.h"
