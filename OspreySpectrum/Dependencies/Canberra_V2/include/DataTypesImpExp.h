
#if defined(WIN32) && defined(BUILD_DLL)
#ifdef DATATYPES_DLLIMPORT
#undef DATATYPES_DLLIMPORT
#endif

#ifdef EXPIMP_TEMPLATE
#undef EXPIMP_TEMPLATE
#endif

#ifdef DATATYPES_EXPORTS
#define DATATYPES_DLLIMPORT __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define DATATYPES_DLLIMPORT __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#endif
#else
#define DATATYPES_DLLIMPORT
#define EXPIMP_TEMPLATE
#endif

#define SERIALIZATION_IMPORT_USAGE
#define UTILITY_IMPORT_USAGE
#define NETWORK_IMPORT_USAGE
#include "platformTypes.h"
#include "winerror.h"
