#pragma once
#include "UtilityImpExp.h"

namespace Canberra {
	namespace Utility {
		namespace Core {

			//
			//An instance of this class is used to create and
			//manage stream of bytes
			//
			class UTILITY_DLLIMPORT ByteStream
			{

			protected:
				char *_Stream;		//The raw stream
				DWORD _Size;		//The size of the stream

			public:
				explicit ByteStream(void);
				explicit ByteStream(DWORD);
				explicit ByteStream(const char * const V, DWORD Size);
				ByteStream(const ByteStream &V);
				virtual ~ByteStream(void);

				//Allocate the memory for the stream
				DWORD Allocate(DWORD);

				//Resizes the memory preserving what is in the stream
				DWORD Resize(DWORD);

				//Copy a stream
				DWORD Copy(const char * const V, DWORD S);
				DWORD Copy(ByteStream &Src, DWORD SrcStart, DWORD DestStart, DWORD Num = 0);
				DWORD Copy(ByteStream &Src);

				//Clear or release the memory
				void Clear();

				//Return the pointer to the unmanaged stream
				const char *const GetBuffer() const;

				//Return the size of the stream (bytes)
				DWORD GetSize() const;

				//Returns the pointer to the unmanaged stream
				operator const char* const();

				//Assigns one stream to another (deep copy)
				ByteStream& operator=( const ByteStream& Rhs);
			};
		}
	}
}
