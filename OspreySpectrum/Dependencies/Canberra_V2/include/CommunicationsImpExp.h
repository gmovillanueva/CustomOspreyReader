
#if defined(WIN32) && defined(BUILD_DLL)
#ifdef COMMUNICATIONS_DLLIMPORT
#undef COMMUNICATIONS_DLLIMPORT
#endif

#ifdef EXPIMP_TEMPLATE
#undef EXPIMP_TEMPLATE
#endif

#ifdef COMMUNICATIONS_EXPORTS
#define COMMUNICATIONS_DLLIMPORT __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define COMMUNICATIONS_DLLIMPORT __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#endif
#else
#define COMMUNICATIONS_DLLIMPORT
#define EXPIMP_TEMPLATE
#endif

#include "platformTypes.h"
#include "winerror.h"
#define SERIALIZATION_IMPORT_USAGE
#define UTILITY_IMPORT_USAGE
#define NETWORK_IMPORT_USAGE
#define DATATYPES_IMPORT_USAGE
