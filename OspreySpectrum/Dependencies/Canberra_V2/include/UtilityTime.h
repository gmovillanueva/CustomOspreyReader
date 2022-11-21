#pragma once
#include "UtilityString.h"

namespace Canberra {

	namespace Utility {

		namespace Core {

			//
			//This is a utility class used to manipulate
			//time information
			//
			class UTILITY_DLLIMPORT Time {
			public:
				//Converts the C-time into a WIN32 FILETIME
				static time_t FileTimeToTime(const FILETIME &ftime);
				//Converts the WIN32 FILETIME into a C-time
				static FILETIME TimeToFileTime(const time_t &utime);

				//Returns the time now as a C-time
				static time_t TimeNow();
				//Returns the time now as a FILETIME
				static FILETIME FileTimeNow();
			};

		}

	}

}
