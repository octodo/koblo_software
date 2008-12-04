/*!	\file CFileSearchWin.cpp
	\author Michael Olsen
	\date 01/Apr/2005
	\date 01/Apr/2005
*/

#include "buInternalOS.h"

#ifdef WIN32
#include <shlobj.h>
// Deprecated function (strncpy)
#pragma warning(disable: 4996)
#endif

CFileSearchWin::CFileSearchWin()
	: mhSearch(INVALID_HANDLE_VALUE)
{
}

CFileSearchWin::~CFileSearchWin()
{
	if (mhSearch != INVALID_HANDLE_VALUE) {
		::FindClose(mhSearch);
	}
}

IFileSearch* IFileSearch::Create()
{
	return dynamic_cast<IFileSearch*>(new CFileSearchWin());
}

void CFileSearchWin::Destroy()
{
	delete dynamic_cast<CFileSearchWin*>(this);
}

void CFileSearchWin::Init(const tchar* pszPathName)
{
	mbInitSuccess = false;
	
	std::string s((char*)pszPathName);
	CFile::PathToOS(s);
	s += "*";

	memset(&mFindFileData, 0, sizeof(mFindFileData));
	mhSearch = ::FindFirstFile(s.c_str(), &mFindFileData);

	mbInitSuccess = (mhSearch != INVALID_HANDLE_VALUE);
} // CFileSearchWin::Init


// Lasse, added 2008-04-08
tbool CFileSearchWin::Init2(const tchar* pszPathAndMask, tchar* pszErrMsgBuff /*= NULL*/, tint32 iErrMsgBuffSize /*= -1*/)
{
	mbInitSuccess = false;
	tchar pszInternalFormat[513];
	if (!CFile::PathFromOS2(pszPathAndMask, pszInternalFormat)) {
		if (pszErrMsgBuff) {
			tchar pszErrPath[1024];
			sprintf(pszErrPath, "Uninterpretable path:\n  %s", pszPathAndMask);
			if (iErrMsgBuffSize < 0)
				strcpy(pszErrMsgBuff, pszErrPath);
			else if (iErrMsgBuffSize > 1) {
				strncpy(pszErrMsgBuff, pszErrPath, iErrMsgBuffSize-1);
				pszErrMsgBuff[iErrMsgBuffSize-1] = '\0';
			}
		}
	}
	else {
		//std::cout << "Internal: " << pszInternalFormat << "\n";
		Init(pszInternalFormat);
		if (!mbInitSuccess) {
			if (pszErrMsgBuff) {
				DWORD dwErr = ::GetLastError();
				tchar pszErr[512];
				if (::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErr, 0, pszErr, 512, NULL) > 0) {
					if (iErrMsgBuffSize < 0)
						strcpy(pszErrMsgBuff, pszErr);
					else if (iErrMsgBuffSize > 1) {
						strncpy(pszErrMsgBuff, pszErr, iErrMsgBuffSize-1);
						pszErrMsgBuff[iErrMsgBuffSize-1] = '\0';
					}
				}
			}
		}
	}
	return mbInitSuccess;
} // Init2
// .. Lasse


tbool CFileSearchWin::GetNext(tchar* pszName, tbool& rDirectory)
{
	// Lasse, added 2008-04-08
	if (!mbInitSuccess)
		return false;
	// .. Lasse
		
	if (mhSearch != INVALID_HANDLE_VALUE) {
		do {
			if ((strcmp(mFindFileData.cFileName, "..") == 0)	||
				(strcmp(mFindFileData.cFileName, ".") == 0)	||
				((mFindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0) ||
				((mFindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0)) {
				continue;
			}

			if ((mFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
				rDirectory = true;
			}
			else {
				rDirectory = false;
			}

			strncpy((char*)pszName, mFindFileData.cFileName, 255);
			if (::FindNextFile(mhSearch, &mFindFileData) == false) {
				::FindClose(mhSearch);
				mhSearch = INVALID_HANDLE_VALUE;
				// Lasse, added 2008-04-09
				mbInitSuccess = false;
				// .. Lasse
			}
			return true;

		} while (::FindNextFile(mhSearch, &mFindFileData));
	}

	return false;
}





