/*!	\file CFile.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/


#include "buInternalOS.h"

#ifdef WIN32
#include <shlobj.h>
// Deprecated function (_open)
#pragma warning(disable: 4996)
#endif

CFile::CFile() : miSizeWhenOpened(0), miCurrentFilePos(0)
#ifdef WIN32
, miHandle(-1)
#endif	// WIN32
#ifdef _Mac
, mpHandle(NULL)
#endif	// _Mac
{
}

CFile::~CFile()
{
	Close();
}

IFile* IFile::Create()
{
	return dynamic_cast<IFile*>(new CFile());
}

void CFile::Destroy()
{
	delete dynamic_cast<CFile*>(this);
}

void ModifyPathName(std::string& rsPathName)
{
#ifdef WIN32
	if (rsPathName[0] == ':') {
		// Full path with drive name
		rsPathName[0] = rsPathName[1];
		rsPathName[1] = ':';
		rsPathName[2] = '\\';

		tuint uiPosition;
		for (uiPosition = 3; uiPosition < rsPathName.size(); uiPosition++) {
			if (rsPathName[uiPosition] == ':') {
				rsPathName[uiPosition] = '\\';
			}
		}
	}
	else {
		tuint uiPosition;
		for (uiPosition = 0; uiPosition < rsPathName.size(); uiPosition++) {
			if (rsPathName[uiPosition] == ':') {
				rsPathName[uiPosition] = '\\';
			}
		}

		if (rsPathName[1] == '\\' && rsPathName[2] == '\\') {
			rsPathName[1] = ':';
		}
	}
#endif	// WIN32

#ifdef _Mac
	if (rsPathName[0] == ':') {
		rsPathName = rsPathName.substr(1);
/*		// Convert from internal to unix
		tchar psz[2048];
		strcpy(psz, rsPathName.c_str());
		ToUnix(psz);
		rsPathName = std::string(psz);*/
	}
#endif	// _Mac
}

#ifdef WIN32

tbool CFile::Open(const tchar* pszPathName, EOpenFile OpenFile)
{
	Close();

	std::string sPathName = std::string((char*)pszPathName);
	// Lasse, that would crash on Windows for ""
	// ModifyPathName(sPathName);
	if (!PathToOS2(sPathName))
		return false;
	// .. Lasse

	int iOpenFlags = _O_BINARY | _O_SEQUENTIAL;

	switch(OpenFile) {
		case FileRead:
			iOpenFlags |= _O_RDONLY;
			break;
		case FileWrite:
			iOpenFlags |= _O_WRONLY;
			break;
		case FileCreate:
			iOpenFlags |= (_O_CREAT | _O_TRUNC | _O_WRONLY);
			break;
	}

	if (OpenFile != FileCreate) {
		miHandle = _open(sPathName.c_str(), iOpenFlags);
	}
	else {
		miHandle = _open(sPathName.c_str(), iOpenFlags, _S_IREAD | _S_IWRITE);
	}
	if (miHandle == -1) {
		return false;
	}

	miSizeWhenOpened = _lseek(miHandle, 0, SEEK_END);
	_lseek(miHandle, 0, SEEK_SET);

	msPathName = sPathName;

	miCurrentFilePos = 0;

	return true;
}

void CFile::Close()
{
	if (miHandle != -1) {
		if (_close(miHandle) == -1) {
		}
	}

	miHandle = -1;
	msPathName = "";
}

tuint64 CFile::Read(tchar* pch, tuint64 iSize)
{
	tint iRead = _read(miHandle, pch, (unsigned int)iSize);

	miCurrentFilePos += iRead;

	return iRead;
}

tuint64 CFile::Write(const tchar* pch, tuint64 iSize)
{
	tint iRet = _write(miHandle, pch, (unsigned int)iSize);

	miCurrentFilePos += iRet;

	return iRet;
}

tuint64 CFile::Seek(tuint64 iPos)
{
	miCurrentFilePos = _lseek(miHandle, (long)iPos, SEEK_SET);

	return miCurrentFilePos;
}

tbool IFile::DeleteFile(const tchar* pszPathName)
{
	// Don't crash on empty string
	if ((pszPathName == NULL) || (*pszPathName == '\0'))
		return false;

	tchar pszPathName2[1024];
	PathToOS2(pszPathName, pszPathName2);

	return ::DeleteFile((LPCTSTR)pszPathName2) ? true : false;
}

tbool IFile::MoveFile(const tchar* pszPathNameDest, const tchar* pszPathNameSrc, const tchar* pszName)
{
	std::string sSrcPath((const char*)pszPathNameSrc);
	CFile::PathToOS(sSrcPath);
	sSrcPath += std::string((const char*)pszName);

	std::string sDestPath((const char*)pszPathNameDest);
	CFile::PathToOS(sDestPath);
	sDestPath += std::string((const char*)pszName);

	return ::MoveFileEx(sSrcPath.c_str(),
		sDestPath.c_str(),
		MOVEFILE_COPY_ALLOWED | /*MOVEFILE_FAIL_IF_NOT_TRACKABLE |*/ MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) ? true : false;
}

tbool IFile::CopyFile(const tchar* pszPathNameDest, const tchar* pszPathNameSrc, const tchar* pszName)
{
	std::string sSrcPath((const char*)pszPathNameSrc);
	CFile::PathToOS(sSrcPath);
	sSrcPath += std::string((const char*)pszName);

	std::string sDestPath((const char*)pszPathNameDest);
	CFile::PathToOS(sDestPath);
	sDestPath += std::string((const char*)pszName);

	return ::CopyFile(sSrcPath.c_str(),
		sDestPath.c_str(),
		TRUE) ? true : false;
}


tbool IFile::CreateDirectory(const tchar* pszPathName)
{
	std::string sPathName((const char*)pszPathName);

	// Multiple (nesting) directories cannot be created at once, so we need to make a recursive algorithm
	if (sPathName.size() > 3) {
		std::string sPathName2;
		sPathName2 = sPathName.substr(0, sPathName.size() - 1);

		tint iPos = sPathName2.find_last_of(':');
		if (iPos != std::string::npos && iPos >= 3) {
			sPathName2 = sPathName2.substr(0, iPos);
			CreateDirectory((const tchar*)sPathName2.c_str());
		}
	}

	CFile::PathToOS(sPathName);

	if (sPathName[sPathName.size() - 1] == '\\') {
		sPathName = sPathName.substr(0, sPathName.size() - 1);
	}

	return ::CreateDirectory((LPCTSTR)sPathName.c_str(), NULL) ? true : false;
}

void IFile::GetSystemDirectory(ESystemDir SystemDir, tchar* pszPathName)
{
	switch(SystemDir) {
		case SystemDirDesktop:
			{
				::CoInitialize(NULL);

				::SHGetSpecialFolderPath(NULL,
					(LPSTR)pszPathName,
					CSIDL_DESKTOP,
					FALSE);

				strcat((char*)pszPathName, "\\");

				::CoUninitialize();
			}
			break;

		case SystemDirDocuments:
			{
				::CoInitialize(NULL);

				::SHGetSpecialFolderPath(NULL,
					(LPSTR)pszPathName,
					CSIDL_PERSONAL,
					FALSE);

				strcat((char*)pszPathName, "\\");

				::CoUninitialize();
			}
			break;

		case SystemDirPreferences:
			{
				::CoInitialize(NULL);

				::SHGetSpecialFolderPath(NULL,
					(LPSTR)pszPathName,
					CSIDL_APPDATA,
					FALSE);

				strcat((char*)pszPathName, "\\");

				::CoUninitialize();
			}
			break;

		case SystemDirApplications:
			{
				::CoInitialize(NULL);

				::SHGetSpecialFolderPath(NULL,
					(LPSTR)pszPathName,
					CSIDL_PROGRAM_FILES,
					FALSE);

				strcat((char*)pszPathName, "\\");

				::CoUninitialize();
			}
			break;

		case SystemDirApplicationSupport:
			{
				::CoInitialize(NULL);

				::SHGetSpecialFolderPath(NULL,
					(LPSTR)pszPathName,
					CSIDL_PROGRAM_FILES_COMMON,
					FALSE);

				strcat((char*)pszPathName, "\\");

				::CoUninitialize();
			}
			break;

		case SystemDirMyMusic:
			{
				::CoInitialize(NULL);

				::SHGetSpecialFolderPath(NULL,
					(LPSTR)pszPathName,
					CSIDL_MYMUSIC,
					FALSE);

				strcat((char*)pszPathName, "\\");

				::CoUninitialize();
			}
			break;

		case SystemDirScratch:
			{
				::GetTempPath(512, pszPathName);
			}
			break;

		default:
			throw IException::Create(IException::TypeCode, IException::ReasonCodePathInvalid, EXCEPTION_INFO, (tchar*)"Not implemented for Win32");
	}

	// Convert to internal path type
	std::string sRet((const char*)pszPathName);
	CFile::PathFromOS(sRet);
	strcpy((char*)pszPathName, sRet.c_str());
}

void CFile::PathFromOS(std::string& rsPathName)
{
	rsPathName[2] = ':';
	rsPathName[1] = rsPathName[0];
	rsPathName[0] = ':';

	tint iSize = rsPathName.size();
	tint iPos;
	for (iPos = 3; iPos < iSize; iPos++) {
		if (rsPathName[iPos] == '\\') {
			rsPathName[iPos] = ':';
		}
	}
}

void CFile::PathToOS(std::string& rsPathName)
{
	ModifyPathName(rsPathName);
}


tbool IFile::IsAbsPath2(const tchar* pszPathName)
{
	if (pszPathName != NULL) {
		if (*pszPathName == ':')
			return true;
		tchar c = *pszPathName;
		if (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'))) {
			if ((pszPathName[1] == ':') && (pszPathName[2] == '\\'))
				return true;
		}
	}
	
	return false;
}


tbool IFile::Exists(const tchar* pszItem, tbool* pbIsFolder /*= NULL*/)
{
	if (*pszItem == '\0')
		return false;

	tchar pszItem2[1024];
	PathToOS2(pszItem, pszItem2);
	tint32 iLen = strlen(pszItem2);
#ifdef WIN32
	const char cPathDelimiter = '\\';
#endif // WIN32
#ifdef _Mac
	const char cPathDelimiter = '/';
#endif // _Mac
	if (pszItem2[iLen - 1] == cPathDelimiter) {
		pszItem2[iLen - 1] = '\0';
	}

	WIN32_FIND_DATA data;
	HANDLE handle = ::FindFirstFile(pszItem2, &data);
	if (handle == INVALID_HANDLE_VALUE)
		return false;

	if (pbIsFolder)
		*pbIsFolder = ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);

	::FindClose(handle);
	return true;
}


// Lasse, added 2008-04-09
#include <direct.h>
//! Does the hard work for PathToOS2(...) and PathFromOS2(...) methods.
tbool _PathToOrFromOS2(tbool bFromOS, const tchar* pszPathNameIn, tchar* pszPathNameOut, tbool bMakeAbsPath /*= true*/, tbool* pbIsAbsPath /*= NULL*/)
{
	// To and from Windows / DOS paths
	
	tbool bIsAbsPath = false;
	std::string sVolumePart = "";
	tchar* pszWD = NULL;
	
	tchar* pc = (tchar*)pszPathNameIn;
	if (1) {//bFromOS) {
		// First see if we need to make this an absolute path
		if (!IFile::IsAbsPath2(pc)) {
			// This is a relative path!
			if (bMakeAbsPath) {
				// Make absolute path by prepending working directory
				
				pszWD = new tchar[513];
				if (_getcwd(pszWD, 512) == NULL) {
					// Unable to get working directory - why?
					if (pszWD)
						delete [] pszWD;
					return false;
				}
				if (*pc == '\\') {
					// Only copy drive letter (and colon) from working directory
					pszWD[2] = '\0';
				}
				strncat(pszWD, "\\", 512);
				strncat(pszWD, pszPathNameIn, 512);
				pc = pszWD;
				bIsAbsPath = true;
			}
		}
	}
	
	if (*pc == ':') { // Internal format path
		bIsAbsPath = true;
		
		// Verify internal format
		pc++;
		tchar cLetter = *pc;
		tchar cColon = (cLetter == '\0') ? '\0' : pc[1];
		tbool bIsDriveLetter = (((cLetter >= 'A') && (cLetter <= 'Z')) || ((cLetter >= 'a') && (cLetter <= 'z')));
		if ((!bIsDriveLetter) || ((cColon != ':') && (cColon != '\0'))) {
			// This won't translate into a DOS path
			return false;
		}
		
		if (bFromOS) {
			// From OS (POSIX) path to internal format
			// This is already an internal path - but we mustn't fail!
			
			tchar pszTmp[4];
			sprintf(pszTmp, ":%c%c", cLetter, cColon);
			sVolumePart = pszTmp;
			pc += 2;
		}
		else {
			// From internal format to OS (DOS) path
			
			tchar pszTmp[4];
			sprintf(pszTmp, "%c%c\\", cLetter, cColon);
			sVolumePart = pszTmp;
			pc += 2;
		}
	}
	else if (IFile::IsAbsPath2(pc)) { // Of the "C:\<what-ever>" DOS format
		bIsAbsPath = true;
		
		if (bFromOS) {
			// From OS (DOS) path to internal format

			sVolumePart = std::string(":") + *pc;
			pc += 2;
		}
		else {
			// From internal format to OS (POSIX) path
			// This is already an OS path - but we mustn't fail!
			
			// No volume name conversion needed
			sVolumePart.insert(0, pc, 2);
			pc += 2;
		}
	}
	
	// Copy contents while translating any ".." sequencies
	tchar cDelim = (bFromOS ? ':' : '\\');
	tchar pszAfterVolume[512];
	tchar* pcDst = pszAfterVolume;
	std::list<tchar*> listColons;
	std::list<tchar*>::iterator it;
	while (*pc) {
		if ((*pc == '\\') || (*pc == ':')) {
			*pcDst = cDelim;//':';
			if (listColons.size() > 0) {
				it = listColons.end();
				it--;
				tchar* pcLastColon = *it;
				tint iLen = pcDst - pcLastColon - 1;
				if ((iLen == 2) && (pcLastColon[1] == '.') && (pcLastColon[2] == '.')) {
					// ".." sequence found
					listColons.erase(it);
					if (listColons.size() == 0) {
						if (bIsAbsPath) {
							// Serious error - can't translate ".." starting sequence in absolute path
							if (pszWD)
								delete [] pszWD;
							return false;
						}
						else {
							// Ignore intial ".." sequence in relative path
							// Do nothing
						}
					}
					else {
						// Move back before last path part which is to be unwound by ".." sequence
						pcDst = listColons.back();
						listColons.pop_back();
					}
				}
				else {
					// This isn't a ".." sequence
					listColons.push_back(pcDst);
				}
			}
			else {
				// No previous ':' in path
				listColons.push_back(pcDst);
			}
		}
		else
			*pcDst = *pc;
		pc++;
		pcDst++;
	}
	*pcDst = '\0';
	
	if (pbIsAbsPath != NULL)
		*pbIsAbsPath = bIsAbsPath;
	sprintf(pszPathNameOut, "%s%s", sVolumePart.c_str(), pszAfterVolume);
	
	if (pszWD)
		delete [] pszWD;
	return true;
} // _PathToOrFromOS2
// .. Lasse


tbool IFile::GetDriveNames(tchar* pszEnumNames, tint32 iBuffSize /*= -1*/, char cDelimiter /*= '@'*/, tbool bAddExtraInfo /*= false*/)
{
	if (iBuffSize == 0)
		return false;
	*pszEnumNames = '\0';

	tchar* pszEnumStringIndex = pszEnumNames;
	tint32 iEnumStringLen = 0;

	tchar pszLetterAndColon[4] = "x:\\";
	for (char cLetter = 'A'; cLetter <= 'Z'; cLetter++) {
		*pszLetterAndColon = cLetter;
		tchar pszVolumeName[512];
		//tchar pszFileSystem[64];
		DWORD dwSerialNb, dwMaxChars, dwFlags;
		BOOL bSuccess = ::GetVolumeInformation(
			pszLetterAndColon,
			pszVolumeName, sizeof(pszVolumeName),
			&dwSerialNb, &dwMaxChars, &dwFlags,
			NULL, 0);

		if (bSuccess) {
			// Format item
			tchar pszItem[1024];
			tint32 iItemLen;
			if (bAddExtraInfo) {
				iItemLen = sprintf(pszItem, "%c (%s)", cLetter, pszVolumeName);
			}
			else {
				iItemLen = sprintf(pszItem, "%c", cLetter);
			}

			// Verify length will fit
			if (iBuffSize > -1) {
				tint32 iNewLen = iEnumStringLen + iItemLen;
				if (iEnumStringLen > 0) {
					iNewLen++;
				}
				if (iNewLen >= iBuffSize) {
					// Not enough space
					return false;
				}
			}

			// Append enum
			if (iEnumStringLen > 0) {
				*pszEnumStringIndex++ = cDelimiter;
			}
			strcpy(pszEnumStringIndex, pszItem);
			pszEnumStringIndex += iItemLen;
			iEnumStringLen += iItemLen;
		}
	}

	return (iEnumStringLen > 0);
} // GetDriveNames


#endif	// WIN32

#ifdef _Mac

tbool GetOldSchoolMacName(const tchar* pszNameIn, tchar* pszNameOut)
{
	FSRef ref;
	UInt8* p8 = (UInt8*)pszNameIn;
	OSStatus err = FSPathMakeRef(p8, &ref, NULL);
	if (err != 0) {
		*pszNameOut = '\0';
		return false;
	}
	else {
		FSSpec spec;
		err = FSGetCatalogInfo(&ref, kFSCatInfoNone, NULL, NULL, &spec, NULL);
		if (err != 0) {
			*pszNameOut = '\0';
			return false;
		}
		else {
			tint iLen = spec.name[0];
			memcpy(pszNameOut, spec.name + 1, iLen);
			pszNameOut[iLen] = '\0';
		}
	}
}

tbool CFile::Open(const tchar* pszPathName, EOpenFile OpenFile)
{
	Close();

	tchar pszPathName2[2048];
/*
	strcpy(pszPathName2, pszPathName);
	if (pszPathName2[0] == ':') {
		// Convert from internal to unix
		ToUnix(pszPathName2);
	}
*/
	PathToOS2(pszPathName, pszPathName2);

/*	FSSpec spec;
	FileNameToFSSpec((char*)pszPathName, &spec);
	OSErr err;
	
	switch(OpenFile) {
		case FileRead:
			err = FSpOpenDF(&spec, SignedByte(fsRdPerm), &miRefNum);
			break;
		case FileWrite:
			err = FSpOpenDF(&spec, SignedByte(fsWrPerm), &miRefNum);
			break;
		case FileCreate:
			err = FSpCreate(&spec, 'KTOS', 'KTOS', 0);
			err = FSpOpenDF(&spec, SignedByte(fsWrPerm), &miRefNum);
			break;
	}

	if (err!=noErr) {
		miRefNum = -1;
		return false;
	}

	int i;
	GetEOF(miRefNum, (long*)&i);
	miSizeWhenOpened = i;*/

	switch(OpenFile) {
		case FileRead:
			mpHandle = fopen(pszPathName2, "r");
			break;
		case FileWrite:
			mpHandle = fopen(pszPathName2, "r+");
			break;
		case FileCreate:
			mpHandle = fopen(pszPathName2, "w");
			break;
	}

	if (mpHandle == NULL) {
		return false;
	}

	fseek(mpHandle, (size_t)0, SEEK_END);
	miSizeWhenOpened = ftell(mpHandle);
	fseek(mpHandle, (size_t)0, SEEK_SET);

	msPathName = std::string((const char*)pszPathName);

	miCurrentFilePos = 0;

	return true;
}

void CFile::Close()
{
	if (mpHandle != NULL) {
		fclose(mpHandle);
		mpHandle = NULL;
		msPathName = "";
	}
}

tuint64 CFile::Read(tchar* pch, tuint64 iSize)
{
//	int i = iSize;
//	FSRead(miRefNum, (long*)&i, (void*)pch);
	int i = fread((void*)pch, (size_t)1, (size_t)iSize, mpHandle);
	miCurrentFilePos += i;
	return i;
}

tuint64 CFile::Write(const tchar* pch, tuint64 iSize)
{
//	int i = iSize;
//	FSWrite(miRefNum, (long*)&i, (void*)pch);
	int i = fwrite((const void*)pch, (size_t)1, (size_t)iSize, mpHandle);
	miCurrentFilePos += i;
	return i;
}

tuint64 CFile::Seek(tuint64 iPos)
{
//	SetFPos(miRefNum, fsFromStart, iPos);

	// Some error checking is needed here
//	int i;
//	GetFPos(miRefNum, (long*)&i);
//	miCurrentFilePos = i;

//	return miCurrentFilePos;

	fseek(mpHandle, (size_t)iPos, SEEK_SET);

	miCurrentFilePos = ftell(mpHandle);

	return miCurrentFilePos;
}

tbool IFile::DeleteFile(const tchar* pszPathName)
{
	// Don't crash on empty string
	if ((pszPathName == NULL) || (*pszPathName == '\0'))
		return false;

	tchar pszPathName2[1024];
	PathFromOS2(pszPathName, pszPathName2);

	// Don't crash on delete folder
	tint32 iLenPath2 = strlen(pszPathName2);
	if ((iLenPath2 <= 0) || (pszPathName2[iLenPath2 - 1] == ':'))
		return false;
	
	FSSpec spec;
	FileNameToFSSpec(pszPathName2, &spec);
	OSErr err = ::FSpDelete(&spec);
	if (err!=noErr) {
		return false;
	}
	return true;
}

tbool IFile::MoveFile(const tchar* pszPathNameDest, const tchar* pszPathNameSrc, const tchar* pszName)
{
	tchar pszSrc[512];
	strcpy((char*)pszSrc, (const char*)pszPathNameSrc);
	strcat((char*)pszSrc, (const char*)pszName);

	std::string sPathNameSrc((const char*)pszSrc);
	CFile::PathToOS(sPathNameSrc);
	std::string sPathNameDest((const char*)pszPathNameDest);
	CFile::PathToOS(sPathNameDest);

	FSSpec SpecSrc;
	FileNameToFSSpec(sPathNameSrc.c_str(), &SpecSrc);

	FSSpec SpecDest;
	FileNameToFSSpec(sPathNameDest.c_str(), &SpecDest);

	FSRef RefSrc;
	::FSpMakeFSRef(&SpecSrc, &RefSrc);

	FSRef RefDest;
	::FSpMakeFSRef(&SpecDest, &RefDest);

	OSErr err = ::FSMoveObject(&RefSrc, &RefDest, NULL);
	
	if (err == noErr) {
		return true;
	}
	else {
		return false;
	}
}

tbool IFile::CopyFile(const tchar* pszPathNameDest, const tchar* pszPathNameSrc, const tchar* pszName)
{
	tchar pszSrc[512];
	strcpy((char*)pszSrc, (const char*)pszPathNameSrc);
	strcat((char*)pszSrc, (const char*)pszName);

	std::string sPathNameSrc((const char*)pszSrc);
	CFile::PathToOS(sPathNameSrc);
	std::string sPathNameDest((const char*)pszPathNameDest);
	CFile::PathToOS(sPathNameDest);

	FSSpec SpecSrc;
	FileNameToFSSpec(sPathNameSrc.c_str(), &SpecSrc);

	FSSpec SpecDest;
	FileNameToFSSpec(sPathNameDest.c_str(), &SpecDest);

	FSRef RefSrc;
	::FSpMakeFSRef(&SpecSrc, &RefSrc);

	FSRef RefDest;
	::FSpMakeFSRef(&SpecDest, &RefDest);

	OSErr err = ::FSCopyObjectSync(&RefSrc, &RefDest, NULL, NULL, 0);
	
	if (err == noErr) {
		return true;
	}
	else {
		return false;
	}
}

void IFile::GetSystemDirectory(ESystemDir SystemDir, tchar* pszPathName)
{
	tchar psz[1024];
	*psz = '\0';
	short sVolRef;
	long lDirID;

	switch(SystemDir) {
		case SystemDirPreferences:
			{
				OSErr err = ::FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder, &sVolRef, &lDirID);
				if (err == 0)
					PathNameFromDirID(lDirID, sVolRef, (char*)psz);
			}
			break;

		case SystemDirDesktop:
			{
				OSErr err = ::FindFolder(kOnSystemDisk, kDesktopFolderType, kDontCreateFolder, &sVolRef, &lDirID);
				if (err == 0)
					PathNameFromDirID(lDirID, sVolRef, (char*)psz);
			}
			break;

		case SystemDirApplications:
			{
				OSErr err = ::FindFolder(kOnSystemDisk, 'apps', kDontCreateFolder, &sVolRef, &lDirID);
				if (err == 0)
					PathNameFromDirID(lDirID, sVolRef, (char*)psz);
			}
			break;

		case SystemDirApplicationSupport:
			{
				OSErr err = ::FindFolder(kOnSystemDisk, kApplicationSupportFolderType, kDontCreateFolder, &sVolRef, &lDirID);
				if (err == 0)
					PathNameFromDirID(lDirID, sVolRef, (char*)psz);
			}
			break;

		case SystemDirDocuments:
			{
				OSErr err = ::FindFolder(kOnSystemDisk, kDocumentsFolderType, kDontCreateFolder, &sVolRef, &lDirID);
				if (err == 0)
					::PathNameFromDirID(lDirID, sVolRef, (char*)psz);
			}
			break;
			
		case SystemDirMyMusic:
			{
				OSErr err = ::FindFolder(kOnSystemDisk, kCurrentUserFolderType, kDontCreateFolder, &sVolRef, &lDirID);
				if (err == 0) {
					::PathNameFromDirID(lDirID, sVolRef, (char*)psz);
					strcat(psz, "Music/");
				}
			}
			break;
			
		case SystemDirScratch:
			{
				// Lasse; note: We must supply the kCreateFolder flag here - otherwise the call will fail with err -43 (file not found)
				// This is because the "chewable items" folder is always destroyed upon boot
				OSErr err = ::FindFolder(kOnSystemDisk, kChewableItemsFolderType, kCreateFolder, &sVolRef, &lDirID);
				if (err == 0)
					::PathNameFromDirID(lDirID, sVolRef, (char*)psz);
			}
			break;
			
		default:
			throw IException::Create(IException::TypeCode, IException::ReasonCodePathInvalid, EXCEPTION_INFO, (tchar*)"Not implemented for OSX");
	}

	// Convert to internal path type
	/*
	std::string sRet((const char*)pszPathName);
	CFile::PathFromOS(sRet);
	strcpy((char*)pszPathName, sRet.c_str());
	*/
	CFile::PathFromOS2(psz, pszPathName);
}

tbool IFile::CreateDirectory(const tchar* pszPathName)
{
	std::string sPathName((const char*)pszPathName);

	// Multiple (nesting) directories cannot be created at once, so we need to make a recursive algorithm
	if (sPathName.size() > 3) {
		std::string sPathName2;
		sPathName2 = sPathName.substr(0, sPathName.size() - 1);

		tint iPos = sPathName2.find_last_of(':');
		if (iPos != std::string::npos && iPos >= 3) {
			sPathName2 = sPathName2.substr(0, iPos);
			CreateDirectory((const tchar*)sPathName2.c_str());
		}
	}

	if (sPathName.find_last_of(':') == 0) {
		// Cannot create drive
		return false;
	}

	CFile::PathToOS(sPathName);

	if (sPathName[sPathName.size() - 1] == ':') {
		sPathName = sPathName.substr(0, sPathName.size() - 1);
	}

	FSSpec Spec;
	FileNameToFSSpec(sPathName.c_str(), &Spec);

	long ID;
	OSErr err = ::DirCreate(Spec.vRefNum, Spec.parID, Spec.name, &ID);
	
	return (err == noErr);
}

void CFile::PathFromOS(std::string& rsPathName)
{
	rsPathName = std::string(":") + rsPathName;
}

void CFile::PathToOS(std::string& rsPathName)
{
	ModifyPathName(rsPathName);
}


tbool IFile::IsAbsPath2(const tchar* pszPathName)
{
	if (pszPathName != NULL) {
		if ((*pszPathName == ':') || (*pszPathName == '/'))
			return true;
		
		// This is a work-around for old-style (Carbon and older) addressing of harddisk
		tchar pszHD[256];
		GetOldSchoolMacName("/", pszHD);
		strcat(pszHD, ":");
		if (strncasecmp(pszPathName, pszHD, strlen(pszHD)) == 0)
			return true;
	}
	
	return false;
}


tbool IFile::Exists(const tchar* pszItem, tbool* pbIsFolder /*= NULL*/)
{
	tchar pszItem2[1024];
	PathToOS2(pszItem, pszItem2);
	
	FSRef ref;
	Boolean bIsDirectory;
	OSStatus status = FSPathMakeRef((UInt8*)pszItem2, &ref, &bIsDirectory);
	if (status != 0)
		return false;
	
	if (pbIsFolder)
		*pbIsFolder = bIsDirectory;
	return true;
}


// Lasse, added 2008-04-07 & 04-08
//! Does the hard work for PathToOS2(...) and PathFromOS2(...) methods.
tbool _PathToOrFromOS2(tbool bFromOS, const tchar* pszPathNameIn, tchar* pszPathNameOut, tbool bMakeAbsPath /*= true*/, tbool* pbIsAbsPath /*= NULL*/)
{
	// To and from Mac OS X / POSIX paths
	
	tchar pszHD[256];
	GetOldSchoolMacName("/", pszHD);
	
	tbool bIsAbsPath = false;
	std::string sVolumePart = "";
	tchar* pszWD = NULL;
	
	tchar* pc = (tchar*)pszPathNameIn;
	// First see if we need to make this an absolute path
	if (IFile::IsAbsPath2(pc)) {
		bIsAbsPath = true;
	}
	else {
		// This is a relative path!
		if (bMakeAbsPath) {
			// Make absolute path by prepending working directory
			
			pszWD = new tchar[513];
			if (getcwd(pszWD, 512) == NULL) {
				// Unable to get working directory - why?
				if (pszWD)
					delete [] pszWD;
				return false;
			}
			strncat(pszWD, "/", 512);
			strncat(pszWD, pszPathNameIn, 512);
			pc = pszWD;
			bIsAbsPath = true;
		}
	}
	
	if (*pc == '/') {
		bIsAbsPath = true;
		
		if (bFromOS) {
			// From OS (POSIX) path to internal format
			
			// We must search for the volume-name
			tchar pszVolume[512];
			pc++;
			tchar* pc2 = pc;
			while ((*pc2 != '\0') && (*pc2 != ':') && (*pc2 != '/')) {
				pc2++;
			}
			tint32 iLen = pc2 - pc;
			if (iLen <= 0) {
				// Unexpected end-of string - what do we do?
				
				sVolumePart = std::string(":") + pszHD + ":";
				pc = pc2;
			}
			else {
				strncpy(pszVolume, pc, iLen);
				pszVolume[iLen] = '\0';
				if (stricmp(pszVolume, "Volumes") != 0) {
					// Not in "/Volumes" mount - assume primary harddisk
					sVolumePart = std::string(":") + pszHD + ":";
				}
				else {
					// Explicitly stated volume - detect name
					if (*pc2 == '\0') {
						// Unexpected end-of string - what do we do?
						sVolumePart = std::string(":") + pszHD + ":" + pszVolume + ":";
						// Make sure we don't add any rubbish
						pc = pc2;
					}
					else {
						// Detect volume name
						pc2++;
						pc = pc2;
						while ((*pc2 != '\0') && (*pc2 != ':') && (*pc2 != '/')) {
							pc2++;
						}
						iLen = pc2 - pc - 1;
						if (iLen <= 0) {
							// Unexpected end-of string after "/Volumes/" - what do we do?
							
							sVolumePart = std::string(":") + pszHD + ":Volumes:";
							// Make sure we don't add any rubbish
							pc = pc2;
						}
						else {
							strncpy(pszVolume, pc, iLen);
							pszVolume[iLen] = '\0';
							sVolumePart += std::string(":") + std::string(pszVolume) + ":";
							// Skip past "/Volumes/<what-ever>/" part
							pc = ++pc2;
						}
					}
				}
			}
		}
		else {
			// From internal format to OS (POSIX) path
			// This is already an OS path - but we mustn't fail!
			
			// No volume name conversion needed
			sVolumePart = "/";
			pc++;
		}
	}
	else if (*pc == ':') {
		bIsAbsPath = true;
		
		if (bFromOS) {
			// From OS (POSIX) path to internal format
			// This is already an internal path - but we mustn't fail!
			
			// Don't translate volume name
			sVolumePart = ":";
			pc++;
		}
		else {
			// From internal format to OS (POSIX) path
			
			// We must search for the volume-name
			tchar pszVolume[512];
			tchar* pc2 = pc + 1;
			while ((*pc2 != '\0') && (*pc2 != ':') && (*pc2 != '/')) {
				pc2++;
			}
			tint32 iLen = pc2 - pc - 1;
			if (iLen <= 0) {
				// No volume name - what do we do?
				
				sVolumePart = "/";
			}
			else {
				strncpy(pszVolume, pc + 1, iLen);
				pszVolume[iLen] = '\0';
				if (stricmp(pszVolume, pszHD) == 0) {
					// Primary harddisk
					sVolumePart = "/";
				}
				else {
					// Secondary harddisk or removable device
					sVolumePart = "/Volumes/";
					sVolumePart += pszVolume;
				}
			}
			// Skip past volume part
			pc = pc2;
			if ((*pc == ':') && (sVolumePart.c_str()[sVolumePart.length() - 1] == '/')) {
				// Already added trailing slash for volume - don't add again
				pc++;
			}
		}
	}
	else {
		if (bIsAbsPath) {
			// We start with the name of primary harddisk (usually "Macintosh HD:") - checked in IsAbsPath2(...)
			// Old-style full path
			bIsAbsPath = true;
			if (bFromOS) {
				// From OS (POSIX) path to internal format
								
				sVolumePart = std::string(":") + pszHD + ":";
				pc += strlen(pszHD) + 1;
			}
			else {
				// From internal format to OS (POSIX) path
				sVolumePart = "/";
				pc += strlen(pszHD);
			}
		}
	}
	
	// Copy contents while translating any ".." sequencies
	tchar cColon = (bFromOS ? ':' : '/');
	tchar pszAfterVolume[512];
	tchar* pcDst = pszAfterVolume;
	std::list<tchar*> listColons;
	std::list<tchar*>::iterator it;
	while (*pc) {
		if ((*pc == '/') || (*pc == ':')) {
			*pcDst = cColon;//':';
			if (listColons.size() > 0) {
				it = listColons.end()--;
				tchar* pcLastColon = *it;
				tint iLen = pcDst - pcLastColon - 1;
				if ((iLen == 2) && (pcLastColon[1] == '.') && (pcLastColon[2] == '.')) {
					// ".." sequence found
					listColons.erase(it);
					if (listColons.size() == 0) {
						if (bIsAbsPath) {
							// Serious error - can't translate ".." starting sequence in absolute path
							if (pszWD)
								delete [] pszWD;
							return false;
						}
						else {
							// Ignore intial ".." sequence in relative path
							// Do nothing
						}
					}
					else {
						// Move back before last path part which is to be unwound by ".." sequence
						pcDst = listColons.back();
						listColons.pop_back();
					}
				}
				else {
					// This isn't a ".." sequence
					listColons.push_back(pcDst);
				}
			}
			else {
				// No previous ':' in path
				listColons.push_back(pcDst);
			}
		}
		else
			*pcDst = *pc;
		pc++;
		pcDst++;
	}
	*pcDst = '\0';
	
	if (pbIsAbsPath != NULL)
		*pbIsAbsPath = bIsAbsPath;
	sprintf(pszPathNameOut, "%s%s", sVolumePart.c_str(), pszAfterVolume);
	
	if (pszWD)
		delete [] pszWD;
	return true;
} // _PathToOrFromOS2
// .. Lasse


tbool IFile::GetDriveNames(tchar* pszEnumNames, tint32 iBuffSize /*= -1*/, char cDelimiter /*= '@'*/, tbool /*bAddExtraInfo /*= false*/)
{
	if (iBuffSize == 0)
		return false;
	*pszEnumNames = '\0';

	tchar* pszEnumStringIndex = pszEnumNames;
	tint32 iEnumStringLen = 0;

	// This may work:
	CAutoDelete<IFileSearch> pSearch(IFileSearch::Create());
	pSearch->Init2("/Volumes/*");
	tchar pszName[1024];
	tbool bIsFolder_Ignored;
	while (pSearch->GetNext(pszName, bIsFolder_Ignored)) {
		tint32 iItemLen = strlen(pszName);
		
		// Verify enough space
		if (iBuffSize > -1) {
			tint32 iNewLen = iEnumStringLen + iItemLen;
			if (iEnumStringLen > 0) {
				iNewLen++;
			}
			if (iNewLen >= iBuffSize) {
				// Insufficient space for new item
				return false;
			}
		}
		
		// Append item
		if (iEnumStringLen > 0) {
			*pszEnumStringIndex++ = cDelimiter;
			iEnumStringLen++;
		}
		strcpy(pszEnumStringIndex, pszName);
		pszEnumStringIndex += iItemLen;
		iEnumStringLen += iItemLen;
	}

	return (iEnumStringLen > 0);
} // GetDriveNames


#endif	// _Mac


// Lasse, added 2008-04-08
tbool CFile::PathFromOS2(std::string& rsPathName, tbool bMakeAbsPath /*= true*/, tbool* pbIsAbsPath /*= NULL*/)
{
	tchar pszTmp[513];
	pszTmp[0] = '\0';
	tbool bRC = PathFromOS2(rsPathName.c_str(), pszTmp, bMakeAbsPath, pbIsAbsPath);
	rsPathName = pszTmp;
	return bRC;
}
// .. Lasse

// Lasse, added 2008-04-08
tbool CFile::PathToOS2(std::string& rsPathName, tbool bMakeAbsPath /*= true*/, tbool* pbIsAbsPath /*= NULL*/)
{
	tchar pszTmp[513];
	pszTmp[0] = '\0';
	tbool bRC = PathToOS2(rsPathName.c_str(), pszTmp, bMakeAbsPath, pbIsAbsPath);
	rsPathName = pszTmp;
	return bRC;
}
// .. Lasse

// Lasse, added 2008-04-08
tbool CFile::PathFromOS2(const tchar* pszPathNameIn, tchar* pszPathNameOut, tbool bMakeAbsPath /*= true*/, tbool* pbIsAbsPath /*= NULL*/)
{
	return _PathToOrFromOS2(true, pszPathNameIn, pszPathNameOut, bMakeAbsPath, pbIsAbsPath);
}
// .. Lasse

// Lasse, added 2008-04-07
tbool IFile::PathFromOS2(const tchar* pszPathNameIn, tchar* pszPathNameOut, tbool bMakeAbsPath /*= true*/, tbool* pbIsAbsPath /*= NULL*/)
{
	return _PathToOrFromOS2(true, pszPathNameIn, pszPathNameOut, bMakeAbsPath, pbIsAbsPath);
}
// .. Lasse

// Lasse, added 2008-04-07
tbool IFile::PathToOS2(const tchar* pszPathNameIn, tchar* pszPathNameOut, tbool bMakeAbsPath /*= true*/, tbool* pbIsAbsPath /*= NULL*/)
{
	return _PathToOrFromOS2(false, pszPathNameIn, pszPathNameOut, bMakeAbsPath, pbIsAbsPath);
}
// .. Lasse

// Lasse, added 2008-04-07
tbool CFile::PathToOS2(const tchar* pszPathNameIn, tchar* pszPathNameOut, tbool bMakeAbsPath /*= true*/, tbool* pbIsAbsPath /*= NULL*/)
{
	return _PathToOrFromOS2(false, pszPathNameIn, pszPathNameOut, bMakeAbsPath, pbIsAbsPath);
} // CFile::PathToOS2
// .. Lasse


tuint64 CFile::GetSizeWhenOpened() const
{
	return miSizeWhenOpened;
}

void CFile::GetPathName(tchar* pszPathName) const
{
	//strcpy((char*)pszPathName, msPathName.c_str());
	PathFromOS2(msPathName.c_str(), pszPathName);
}

tuint64 CFile::GetCurrentFilePosition() const
{
	return miCurrentFilePos;
}

void CFile::ModifyPathName(std::string& rsPathName)
{
	::ModifyPathName(rsPathName);
}


void IFile::PathFromOS(tchar* pszPathName)
{
	std::string s((const char*)pszPathName);
	CFile::PathFromOS(s);
	strcpy((char*)pszPathName, s.c_str());
}

void IFile::PathToOS(tchar* pszPathName)
{
	// Lasse, that's not good enough - will crash on Windows for ""
	/*
	std::string s((const char*)pszPathName);
	CFile::PathToOS(s);
	strcpy((char*)pszPathName, s.c_str());
	*/
	CFile::PathToOS2(pszPathName, pszPathName);
	// .. Lasse
}


tint32 CFile::SplitPath(const std::string& rsPath, std::list<tchar*>& rlistOfPathElements)
{
	tint32 iElements = 0;
	rlistOfPathElements.clear();

	tchar* pcStart = (tchar*)rsPath.c_str();
	while (*pcStart) {
		tchar* pcEnd = pcStart;
		while ((*pcEnd)&&(*pcEnd!=':')) {
			pcEnd++;
		}
		tint32 iLen = pcEnd-pcStart;
		tchar* pszElement = new tchar[iLen + 1];
		strncpy((char*)pszElement, (char*)pcStart, iLen);
		pszElement[iLen] = '\0';
		rlistOfPathElements.insert(rlistOfPathElements.end(), pszElement);
		iElements++;

		if (*pcEnd) {
			// Move past ':'
			pcStart = pcEnd + 1;
		}
		else {				
			// '\0' has been reached
			pcStart = pcEnd;
		}
	}

	return iElements;
} // IFile::SplitPath


tint32 CFile::SplicePath(const std::list<tchar*>& rlistOfPathElements, std::string& rsSplicedPath)
{
	tint32 iElements = 0;
	rsSplicedPath = "";

	// Work-around: get a non-const pointer
	std::list<tchar*>& rlistInternal = (std::list<tchar*>&)rlistOfPathElements;
	std::list<tchar*>::iterator it = rlistInternal.begin();
	for (; it!=rlistInternal.end(); it++) {
		iElements++;
		rsSplicedPath += (char*)*it;
		rsSplicedPath += ':';
	}

	return iElements;
} // IFile::SplicePath


tbool CFile::IsAbsolutePath(const tchar* pszPath)
{
	return ((pszPath!=NULL)&&(pszPath[0]==':'));
} // IFile::IsAbsolutePath


tbool CFile::IsRelativePath(const tchar* pszPath)
{
	return ((pszPath!=NULL)&&(pszPath[0]!=':'));
} // IFile::IsRelativePath


tbool CFile::ToRelativePath(std::string& rsPath, const tchar* pszPathToSubstract)
{
	tint32 iLen = 0;
	if (pszPathToSubstract) {
		iLen = strlen((char*)pszPathToSubstract);
	}

	if (iLen>0) {
		tint32 iCompareRc = strnicmp(rsPath.c_str(), (char*)pszPathToSubstract, iLen);
		if (iCompareRc!=0) {
			return false;
		}
		rsPath = rsPath.substr(iLen);
	}

	return true;
} // IFile::ToRelativePath


tbool CFile::FromRelativePath(std::string& rsPath, const tchar* pszPathToPrepend)
{
	if (IsAbsolutePath((tchar*)rsPath.c_str())) {
		// That's not a relative path!
		return false;
	}

	if ((pszPathToPrepend==NULL)||(pszPathToPrepend[0]=='\0')) {
		// No path to prepend
		return false;
	}

	if (pszPathToPrepend[0]==':') {
		rsPath = std::string((char*)pszPathToPrepend) + rsPath;
	}
	else {
		rsPath = std::string((char*)pszPathToPrepend) + std::string(":") + rsPath;
	}

	return true;
} // IFile::FromRelativePath


tbool CFile::SplitPathToPathAndName(const tchar* pszFullPath, std::string& rsPathOnly, std::string& rsNameOnly, tbool bAcceptEmptyPath /*= true*/, tbool bAcceptEmptyName /*= true*/)
{
	tbool bSuccess = false;

	if (pszFullPath) {
		std::list<tchar*> listOfPathElements;
		tint32 iPathElements = CFile::SplitPath((char*)pszFullPath, listOfPathElements);
		tint32 iMinPathElements = (bAcceptEmptyPath) ? 1 : 2;
		if (iPathElements>=iMinPathElements) {
			std::list<tchar*>::iterator it = listOfPathElements.end();
			it--;
			tchar* pszFileName = *it;
			listOfPathElements.erase(it);
			if ((strlen((char*)pszFileName))||(bAcceptEmptyName)) {
				CFile::SplicePath(listOfPathElements, rsPathOnly);
				rsNameOnly = (char*)pszFileName;
				delete[] pszFileName;
				pszFileName = NULL;
			}
			for (it=listOfPathElements.begin(); it!=listOfPathElements.end(); it++) {
				delete[] *it;
				*it = NULL;
			}
			bSuccess = true;
		}
	}

	return bSuccess;
} // IFile::SplitPathToPathAndName

tbool IFile::SplitPathToPathAndName(const tchar* pszFullPath, tchar* pszPathOnly, tchar* pszNameOnly, tbool bAcceptEmptyPath, tbool bAcceptEmptyName)
{
	std::string sPathOnly;
	std::string sNameOnly;

	tbool bRet = CFile::SplitPathToPathAndName(pszFullPath, sPathOnly, sNameOnly, bAcceptEmptyPath, bAcceptEmptyName);

	pszPathOnly[0] = 0;
	if (sPathOnly.size() != 0) {
		strcpy((char*)pszPathOnly, sPathOnly.c_str());
	}

	pszNameOnly[0] = 0;
	if (sNameOnly.size() != 0) {
		strcpy((char*)pszNameOnly, sNameOnly.c_str());
	}

	return bRet;
}

