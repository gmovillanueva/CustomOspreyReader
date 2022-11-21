#pragma once
#include "UtilityString.h"
#include "ByteStream.h"
#include "CriticalSection.h"
#include <stdio.h>
#define OPEN_CREATE			1
#define OPEN_APPEND			2
#define OPEN_RONLY			4
#define MAX_READLINE_SIZE sizeof(wchar_t)*256


namespace Canberra {
	namespace Utility {
		namespace Core {

			//An OS independent getcwd()
			UTILITY_DLLIMPORT String GetCurrentWorkingDirectory();

			//An OS independent path sep
			UTILITY_DLLIMPORT String GetPathSeparator();

			//
			//This is a class used to access a file
			//
			class UTILITY_DLLIMPORT File
			{
				File(const File&);
				File& operator=(const File&);

			protected:
				CriticalSection _Lock;
				bool _IsUnicode;
				FILE *_File;
				String _Name;

			public:
				File(void);
				virtual ~File(void);

				//Returns the open state
				bool GetIsOpen();

				//Returns the name of the file
				String GetName();

				//Opens the file
				ULONG Open(const String &Name, LONG Ops=OPEN_CREATE|OPEN_APPEND);

				//Closes the file
				ULONG Close();

				//Writes a byte stream to the file
				ULONG Write(const ByteStream &Str, ULONG *NumWritten=NULL);

				//Writes a blob to the file
				ULONG Write(const void * const Data, ULONG Size, ULONG *NumWritten=NULL);

				//Writes a string with carrage return and line feed
				ULONG WriteLine(const String &Str, ULONG *NumWritten=NULL);

				//Reads a block of data
				ULONG Read(void *Data, LONG Size, LONG *NumRead=NULL);

				//Reads a byte stream
				ULONG Read(ByteStream &Str, LONG Size, LONG *NumRead=NULL);

				//Reads TCHAR string until line feed found;
				ULONG ReadLine(String &Str, ULONG MaxReadSize=MAX_READLINE_SIZE);

				//Flushes the information to file
				ULONG Flush();

				//Moves the file pointer
				ULONG Seek(LONG Offset, LONG Origin);

				//State to indicate the file is unicode
				bool GetIsUnicode();
				void PutIsUnicode(bool v);

				//Existence of a file
				static bool Exists(const String &V);

				//Deletes a file
				static ULONG Delete(const String &V);

			};
		}
	}
}
