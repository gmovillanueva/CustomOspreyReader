
#if defined(WIN32) && defined(BUILD_DLL)
#ifdef SERIALIZATION_IMPORT_USAGE
#undef SERIALIZATION_IMPORT_USAGE
#endif

#ifdef EXPIMP_TEMPLATE
#undef EXPIMP_TEMPLATE
#endif

#ifdef SERIALIZATION_EXPORTS
#define SERIALIZATION_IMPORT_USAGE __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define SERIALIZATION_IMPORT_USAGE __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#endif

#else
#define SERIALIZATION_IMPORT_USAGE
#define EXPIMP_TEMPLATE
#endif

#include "platformTypes.h"
#include "winerror.h"
#include "pbsmsgs_h.h"
