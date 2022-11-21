#ifndef _CANBERRA_TYPES
#define _CANBERRA_TYPES

#if defined(WIN32) || defined(WINCE)

#include <windows.h>
#include <tchar.h>

#ifndef STDCALL
#define STDCALL _stdcall
#endif
#ifndef LONGLONG
#define LONGLONG __int64
#endif
#ifndef ULONGULONG
#define ULONGULONG unsigned __int64
#endif
#ifndef REAL
#define REAL float
#endif
#ifndef BYTE
#define BYTE char
#endif
#ifndef UBYTE
#define UBYTE unsigned char
#endif

#else

#ifndef STDCALL
#define STDCALL
#endif
#ifndef FAR
#define FAR
#endif
#ifndef SHORT
#define SHORT short
#endif
#ifndef USHORT
#define USHORT unsigned short
#endif
#ifndef REAL
#define REAL float
#endif
#ifndef HMEM
#define HMEM int
#endif
#ifndef CHAR
#define CHAR char
#endif
#ifndef UCHAR
#define UCHAR unsigned char
#endif
#ifndef DOUBLE
#define DOUBLE double
#endif
#ifndef LONG
#define LONG int
#endif
#ifndef ULONG
#define ULONG unsigned int
#endif
#ifndef DWORD
#define DWORD ULONG
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef BYTE
#define BYTE char
#endif
#ifndef UBYTE
#define UBYTE unsigned char
#endif
#ifndef BOOL
#define BOOL int
#endif
#ifndef WORD
#define WORD short
#endif
#ifndef INT
#define INT int
#endif
#ifndef UINT
#define UINT unsigned int
#endif
#ifndef HRESULT
#define HRESULT unsigned int
#endif
#ifndef LCID
#define LCID int
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef HANDLE
#define HANDLE LONG
#endif

#pragma pack(push, 1)
typedef struct _FILETIME {
	unsigned int dwLowDateTime;			//This is done to ensure 32 bit and 64 bit representations are the same
	unsigned int dwHighDateTime;
} FILETIME;
#pragma pack(pop)

#ifndef _T
#define _T(x)      L ## x
#endif

#if defined(LINUX)

#include <stdint.h>

#ifndef LONGLONG
#define LONGLONG long long //int64_t
#endif
#ifndef ULONGULONG
#define ULONGULONG unsigned long long //uint64_t
#endif

#elif defined(MACOS)
#include <stdint.h>
#ifndef LONGLONG
#define LONGLONG long long //int64_t
#endif
#ifndef ULONGULONG
#define ULONGULONG unsigned long long //uint64_t
#endif

#endif

#endif

#endif

