#pragma once

#include "ByteStream.h"

namespace Canberra {
	namespace Utility {
		namespace Core {

			//
			//An instance of this class is used to create
			//a CRC 32 value given a stream
			//
			class UTILITY_DLLIMPORT Crc32
			{
			protected:
				static bool _TableComputed;			//State indicating the CRC table comp state
				static ULONG _Table[256];			//The CRC table
				ULONG _Crc;							//The computed CRC

				void MakeTable();

			public:
				Crc32(void);
				virtual ~Crc32(void);

				//Computes the CRC
				static ULONG Compute(const ByteStream &BS, LONG Offset);

				//Clears the current CRC value
				void Reset();

				//Update the CRC value
				ULONG Update(const ByteStream &BS, LONG Offset=0);
				ULONG Update(const char * const Buf, LONG Size);

				//Accessor to the CRC value
				ULONG GetValue() {return _Crc;}
			};
		}
	}
}
