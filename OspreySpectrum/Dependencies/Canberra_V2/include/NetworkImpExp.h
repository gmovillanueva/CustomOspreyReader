#pragma once

#if defined(WIN32) && defined(BUILD_DLL)
#ifdef NETWORK_DLLIMPORT
#undef NETWORK_DLLIMPORT
#endif

#ifdef EXPIMP_TEMPLATE
#undef EXPIMP_TEMPLATE
#endif

#ifdef NETWORK_EXPORTS
#define NETWORK_DLLIMPORT __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define NETWORK_DLLIMPORT __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#endif
#else
#define NETWORK_DLLIMPORT
#define EXPIMP_TEMPLATE
#endif

#include "platformTypes.h"
#include "winerror.h"
