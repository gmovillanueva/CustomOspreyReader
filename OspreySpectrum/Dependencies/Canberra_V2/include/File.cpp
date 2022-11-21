#include "File.h"
#include "DebugMacros.h"
#include "LockManager.h"

#ifndef WIN32
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#else
#include <windows.h>

#ifdef WINCE
#define errno	E_FAIL
#endif
#endif
#include <memory.h>

///<summary>
///An instance of this class is used to
///Manage access to a file
///</summary>
Canberra::Utility::Core::File::File() : _Lock(String(L"FileAccessLock")), _IsUnicode(false), _File( NULL ) {}
Canberra::Utility::Core::File::~File() { Close();}


///<summary>
///This method will close and open file
///</summary>
///<returns>Win32 code.  0==Sucess</returns>
ULONG Canberra::Utility::Core::File::Close() {
	Canberra::Utility::Core::LockManager LM(&_Lock);
	if (!GetIsOpen()) return 0;

	_Name.Clear();

	ULONG Stat=0;
	if (0 != (Stat=fclose(_File))) {
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Close"), _T("Error with fclose."), Stat);
	}
	_File= NULL;
	return Stat;
}

///<summary>
///This method will open the specified file via CreateFile
///API.  The security attributes are NULL
///</summary>
///<param name="Name">The name of the file to open</param>
///<param name="Ops">The options</param>
///<returns>0==Sucess</returns>
ULONG Canberra::Utility::Core::File::Open(const String &Name, LONG Ops)
{
	if (GetIsOpen()) {
		ULONG Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Open"), _T("File is already open."), Stat);
		return Stat;
	}

	Canberra::Utility::Core::LockManager LM(&_Lock);
	bool Created=false;
	ULONG Stat=0;
	const char *Mode="r";
	if (Exists(Name)) {
		if (Ops&OPEN_RONLY) {
			Mode = "r";
		}
		else if (Ops&OPEN_APPEND) {
			Mode = "a+";
		}
		else {
			Created=true;
			Mode = "w+";
		}
	}
	else {Created=true; Mode = "w+";}

	_File = fopen(Name, Mode);
	if (NULL == _File) {
		Stat = errno;
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Open"), _T("Error with fopen."), Stat);
	}
	else {
		if (Created && _IsUnicode) {
			//Place encoding format into the file
#ifdef WIN32
			unsigned char Encode[] = {(unsigned char) 255, (unsigned char) 254};
			if (0 != (Stat = Write(Encode, sizeof(Encode)))) {
				DEBUG_ERROR_MESSAGE(L"File", L"Open", L"Error setting Unicode encoding information", Stat);
			}
#endif
		}
	}
	return Stat;
}

///<summary>
///This method will write the byte stream to the file
///</summary>
///<param name="Str">The stream to write</param>
///<param name="NumWritten">The number of bytes written</param>
///<returns>0==Sucess</returns>
ULONG Canberra::Utility::Core::File::Write(const Canberra::Utility::Core::ByteStream &Str, ULONG *NumWritten)
{
	return Write(const_cast<ByteStream &>(Str).GetBuffer(), const_cast<ByteStream &>(Str).GetSize(), NumWritten);
}

///<summary>
///This method will write the byte stream to the file
///</summary>
///<param name="Data">The stream to write</param>
///<param name="Size">The stream size</param>
///<param name="NumWritten">The number of bytes written</param>
///<returns>0==Sucess</returns>
ULONG Canberra::Utility::Core::File::Write(const void * const Data, ULONG Size, ULONG *NumWritten)
{

	Canberra::Utility::Core::LockManager LM(&_Lock);
	if (NumWritten) *NumWritten=0;
	if (!Data) {
		ULONG Stat=(ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Write"), _T("NULL == Data"), Stat);
		return Stat;
	}
	if (!Size) {
		ULONG Stat=(ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Write"), _T("0 == Size"), Stat);
		return Stat;
	}
	if (!GetIsOpen()) {
		ULONG Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Write"), _T("File not open."), Stat);
		return Stat;
	}

	ULONG Stat=0;
	ULONG NumW = fwrite(Data, 1, Size, _File);
	if (NumWritten) *NumWritten=NumW;
	if (NumW != Size) {
		Stat = errno;
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Write"), _T("Error with fwrite."), Stat);
	}
	return Stat;

}

///<summary>
///This method will write the a string to a file
///The string will be delimited with \r\n
///</summary>
///<param name="Str">The string to write</param>
///<param name="NumWritten">The number of bytes written</param>
///<returns>0==Sucess</returns>
ULONG Canberra::Utility::Core::File::WriteLine(const String &Str, ULONG *NumWritten)
{
	String _Str = const_cast<String &>(Str) + String(L"\r\n");
	if (!_IsUnicode) return Write(((char *) _Str), _Str.GetLength()*sizeof(char), NumWritten);
	else return Write(((wchar_t *) _Str), _Str.GetLength()*sizeof(wchar_t), NumWritten);
}


///<summary>
///This method will read the byte stream to the file
///</summary>
///<param name="Data">The stream to read into</param>
///<param name="Size">The stream size</param>
///<param name="NumRead">The number of bytes read</param>
///<returns>0==Sucess</returns>
ULONG Canberra::Utility::Core::File::Read(void *Data, LONG Size, LONG *NumRead)
{
	Canberra::Utility::Core::LockManager LM(&_Lock);
	if (NumRead) *NumRead=0;
	if (!Data) {
		ULONG Stat=(ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Read"), _T("NULL == Data"), Stat);
		return Stat;
	}
	if (!Size) {
		ULONG Stat=(ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Read"), _T("0 == Size"), Stat);
		return Stat;
	}
	if (!GetIsOpen()) {
		ULONG Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Read"), _T("File not open."), Stat);
		return Stat;
	}

	ULONG Stat=0;
	LONG NumR= fread(Data, 1, Size, _File);
	if (NumR) *NumRead=NumR;
	if (NumR != Size) {
		Stat = errno;
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Read"), _T("Error with fread."), Stat);
		return Stat;
	}
	return Stat;
}


///<summary>
///This method will read the string from the file till
///an '\n' or '\r' or EOF is found
///</summary>
///<param name="Data">The string to read into</param>
///<param name="MaxReadSize">The number of chars to read before aborting if "\n\r" not found</param>
///<returns>0==Sucess</returns>
ULONG Canberra::Utility::Core::File::ReadLine(String &Str, ULONG MaxReadSize)
{
	Canberra::Utility::Core::LockManager LM(&_Lock);
	Str.Clear();
	if (!GetIsOpen()) {
		ULONG Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("File"), _T("ReadLine"), _T("File not open."), Stat);
		return Stat;
	}

	std::auto_ptr<char> Buf(new char[MaxReadSize+1]);
	if (!Buf.get()) {
		ULONG Stat=(ULONG) E_OUTOFMEMORY;
		DEBUG_ERROR_MESSAGE(_T("File"), _T("ReadLine"), _T("File to allocate memory buffer for reading."), Stat);
		return Stat;
	}
	ULONG Stat=0;
	memset(Buf.get(), 0, MaxReadSize+1);
	ULONG NumRead=0, NumCurRead=0;
	do {
		NumCurRead=fread(Buf.get()+NumRead, 1, 1, _File);
		if (NumCurRead != 1) {
			Stat = errno;
			DEBUG_ERROR_MESSAGE(_T("File"), _T("Read"), _T("Error with fread."), Stat);
			return Stat;
		}
		//Check for End Of File
		else if (!NumCurRead) {
			*(Buf.get()+NumRead) = 0;
			break;
		}

		if ('\r' == *(Buf.get()+NumRead)) {
			//Remove the '\r'
			*(Buf.get()+NumRead) = 0;
		}
		else if ('\n' == *(Buf.get()+NumRead)) {
			//Remove the '\n'
			*(Buf.get()+NumRead) = 0;
			break;
		}
		NumRead++;
	}
	while(NumRead < MaxReadSize);

	if (MaxReadSize <= NumRead) {
		ULONG Stat = (ULONG) MEM_E_INVALID_SIZE;
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Read"), _T("Exceeded maximum read size"), Stat);
		return Stat;
	}
	if (_IsUnicode) Str.CopyFrom((wchar_t *) Buf.get());
	else Str.CopyFrom((char *) Buf.get());
	return 0;
}

///<summary>
///This method will get state indicating that the file is a
///unicode file
///</summary>
///<returns>The value</returns>
bool Canberra::Utility::Core::File::GetIsUnicode() {return _IsUnicode;}

///<summary>
///This method will set state indicating that the file is a
///unicode file
///</summary>
///<param name="v">The value</param>
void Canberra::Utility::Core::File::PutIsUnicode(bool v) {_IsUnicode=v;}

///<summary>
///This method will flush the buffers
///</summary>
///<returns>Win32 code.  0==Sucess</returns>
ULONG Canberra::Utility::Core::File::Flush()
{
	Canberra::Utility::Core::LockManager LM(&_Lock);
	if (!GetIsOpen()) {
		ULONG Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Flush"), _T("File not open."), Stat);
		return Stat;
	}
	if (!fflush(_File)) return 0;

	ULONG Stat = errno;
	DEBUG_ERROR_MESSAGE(_T("File"), _T("Flush"), _T("Error fflush."), Stat);
	return Stat;
}


///<summary>
///This method will return the name of the currently open file
///</summary>
///<returns>name</returns>
Canberra::Utility::Core::String Canberra::Utility::Core::File::GetName() {return _Name;}

///<summary>
///This method will return information indicating whether
///the file is open
///</summary>
///<returns>Open state</returns>
bool Canberra::Utility::Core::File::GetIsOpen() {return (NULL != _File);}

///<summary>
///This method will delete the specified file
///</summary>
///<param name="Name">The name of the file</param>
///<returns>0==Success else Win32 error code</returns>
ULONG Canberra::Utility::Core::File::Delete(const String &Name) {
	ULONG Stat=0;
	try {
		if (!File::Exists(Name)) {
			Stat = (ULONG) E_FAIL;
			DEBUG_ERROR_MESSAGE(_T("File"), _T("Delete"), _T("File does not exit."), Stat);
			return Stat;
		}
	}CATCHTRACE(L"File", L"Delete", Stat);
	if (Stat) return Stat;

#ifdef WIN32
	if (!DeleteFile(const_cast<String &>(Name))) {
		Stat = GetLastError();
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Delete"), _T("Error DeleteFile."), Stat);
		return Stat;
	}
#else
	if (!unlink(const_cast<String &>(Name))) {
		Stat = errno;
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Delete"), _T("Error unlink."), Stat);
		return Stat;
	}
#endif
	return 0;
}

///<summary>
///This method will return information indicating whether
///the file exists
///</summary>
///<returns>Existance state</returns>
///<exception cref="ULONG">Error code</exception>
bool Canberra::Utility::Core::File::Exists(const String &Name) {
	if (!const_cast<String &>(Name).GetLength()) {
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Exists"), _T("NULL == Name"), E_INVALIDARG);
		return false;
	}

#if WIN32
	WIN32_FIND_DATA Data={0};
	HANDLE Search = FindFirstFile(const_cast<String &>(Name), &Data);
	if (INVALID_HANDLE_VALUE == Search) {
		ULONG Stat = GetLastError();
		if ((ERROR_NO_MORE_FILES == Stat) || (ERROR_FILE_NOT_FOUND == Stat) || (ERROR_PATH_NOT_FOUND == Stat)) return false;
		else {
			DEBUG_ERROR_MESSAGE(_T("File"), _T("Exists"), _T("Error with FindFirstFile. "), Stat);
			return false;
		}
	}
	FindClose(Search);
	return true;
#else
	struct stat st={0};
	if(stat(Name,&st) == -1) {
		return false;
	}
	else {
		if (S_ISREG(st.st_mode)) {
			return true;
		}
		else {
			DEBUG_ERROR_MESSAGE(_T("File"), _T("Exists"), _T("This in not a file. "), E_INVALIDARG);
			return false;
		}
	}
#endif

}

///<summary>
///This method will set the file pointer
///</summary>
///<param name="Offset">The number of bytes from the origin</param>
///<param name="Origin">The origin see (SEEK_SET=Beginning of file, SEEK_CUR=End of file, SEEK_END=End of file)</param>
///<exception cref="ULONG">Error code</exception>
ULONG Canberra::Utility::Core::File::Seek(LONG Offset, LONG Origin) {
	ULONG Stat=0;
	if (!GetIsOpen()) {
		Stat=(ULONG) E_FAIL;
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Flush"), _T("File not open."), Stat);
		return Stat;
	}
	if (fseek(_File, Offset, Origin)) {
		Stat = errno;
		DEBUG_ERROR_MESSAGE(_T("File"), _T("Seek"), _T("Error with fseek."), Stat);
	}
	return Stat;
}


///<summary>
///This method will return the current working directory
///</summary>
///<return>The value</return>
Canberra::Utility::Core::String Canberra::Utility::Core::GetCurrentWorkingDirectory()
{
	
#ifdef WIN32
	TCHAR AppName[MAX_PATH];
	::GetModuleFileName(NULL, AppName, sizeof(AppName));
	
	TCHAR Path[MAX_PATH];
	memset(Path, 0, sizeof(Path));
	for(size_t i=_tcsclen(AppName); i>0; i--) {
		if(AppName[i-1]==_T('\\')) {
			_tcsncpy(Path, AppName, i);
			break;
		}
	}
	String Folder(Path);

#else
	char Buf[4096];
	Canberra::Utility::Core::String Folder(getcwd(Buf, sizeof(Buf)));
#endif
	return Folder;
}

///<summary>
///This method will return the separator character for a file system
///path ('/' or '\') based on operating system
///</summary>
///<return>The value</return>
Canberra::Utility::Core::String Canberra::Utility::Core::GetPathSeparator()
{
#ifdef WIN32
	return String(L"\\");
#else
	return String(L"/");
#endif
}

