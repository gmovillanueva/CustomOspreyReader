#pragma once

#define EVENTLOG_INFORMATION_TYPE	0x0004
#define EVENTLOG_WARNING_TYPE		0x0002
#define EVENTLOG_ERROR_TYPE			0x0001

namespace Canberra {
	namespace Utility {
		namespace Core {
			namespace Diagnostics {
				typedef enum TraceTypeTag {
					Information=EVENTLOG_INFORMATION_TYPE,
					Warning=EVENTLOG_WARNING_TYPE,
					Error=EVENTLOG_ERROR_TYPE
				} TraceType;
			}
		}
	}
}
