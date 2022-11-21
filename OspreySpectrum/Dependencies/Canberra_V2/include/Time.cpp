#ifdef WINCE
#include <windows.h>
#include <types.h>
#else
#include <sys/types.h>
#endif
#include "UtilityTime.h"
#include <memory.h>

#define EPOCH_DIFF 0x019DB1DED53E8000LL /* 116444736000000000 nsecs */
#define RATE_DIFF 10000000 /* 100 nsecs */

///<summary>
///This method will convert the C-time into a Win32 FILETIME
///</summary>
///<param name="utime">The C time</param>
///<returns>The file time</returns>
FILETIME Canberra::Utility::Core::Time::TimeToFileTime(const time_t &utime) {
	LONGLONG tconv = ((LONGLONG)utime * RATE_DIFF) + EPOCH_DIFF;
	FILETIME ft={0};
	memcpy(&ft, &tconv, sizeof(LONGLONG));
	return ft;
}


///<summary>
///This method will convert the Win32 FILETIME into a C time
///</summary>
///<param name="ftime">The file time</param>
///<returns>The C-time</returns>
time_t Canberra::Utility::Core::Time::FileTimeToTime(const FILETIME &ftime) {
	LONGLONG FT = {0};
	memcpy(&FT, &ftime, sizeof(LONGLONG));
    LONGLONG tconv = (FT - EPOCH_DIFF) / RATE_DIFF;
    return (time_t)tconv;
}

///<summary>
///This method will return the time now as a FILETIME
///</summary>
///<returns>The file time</returns>
FILETIME Canberra::Utility::Core::Time::FileTimeNow() {
	time_t now=TimeNow();
	return TimeToFileTime(now);
}

///<summary>
///This method will return the time now as a C-time
///</summary>
///<returns>The C-time</returns>
time_t Canberra::Utility::Core::Time::TimeNow() {
	return time(NULL);
}
