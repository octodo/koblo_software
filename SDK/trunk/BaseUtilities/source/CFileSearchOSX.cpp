/*!	\file CFileSearchOSX.cpp
	\author Michael Olsen
	\date 10/May/2005
	\date 10/May/2005
*/


#include "buInternalOS.h"


CFileSearchOSX::CFileSearchOSX()
{
	mbInitSuccess = false;
}

CFileSearchOSX::~CFileSearchOSX()
{
}

IFileSearch* IFileSearch::Create()
{
	return dynamic_cast<IFileSearch*>(new CFileSearchOSX());
}

void CFileSearchOSX::Destroy()
{
}


const FSCatalogInfoBitmap kCatalogInfoBitmap = (kFSCatInfoNodeFlags | kFSCatInfoFinderInfo);


void CFileSearchOSX::Init(const tchar* pszPathName)
{
	tchar pszFullPath[1024];

	mbInitSuccess = false;

	// Convert to POSIX style full qualified path
	{
		tint iLenInput = strlen(pszPathName);
		tint iLastSlashIx = -1;
		tint iCpySrcIx = 0;
		tint iCpyDstIx = 0;
	
		iCpyDstIx = sprintf(pszFullPath, "/Volumes");
		while (iCpySrcIx < iLenInput) {
			// Translate char
			tchar c = pszPathName[iCpySrcIx];
			if (c == '*') {
				// This is filter part
			
				if (iLastSlashIx < (iCpySrcIx - 1)) {
					// '*' must be first char after '/'
					throw IException::Create(IException::TypeCode, IException::ReasonCodeInvalidArgument,
						EXCEPTION_INFO,
						"Filter not supported - '*' must be first char after ':'");
				}
			
				// Don't copy filter
				iLenInput = iCpySrcIx;
			
				// Save Filter
				msFilter = std::string(pszPathName + iCpySrcIx + 1);
				if (msFilter.compare(".*") == 0) {
					msFilter = std::string("");
				}
			}
			else {
				if (c == ':') { // replace ":" with "\"
					c = '/';
					iLastSlashIx = iCpySrcIx;
				}
				else if (c == '/') {
					iLastSlashIx = iCpySrcIx;
				}
				pszFullPath[iCpyDstIx] = c;
			
				// Advance
				iCpySrcIx++;
				iCpyDstIx++;
			}
		}
		// Zero terminate
		pszFullPath[iCpyDstIx] = 0;
	}

	// Initialize bulk iterator
	{
		mSearchIndex = 0;
		miBulkIx = -1;
		
		std::cout << "pszFullPath:\n  " << pszFullPath << "\n";

		Boolean bFolder = false;
		OSStatus rc = FSPathMakeRef((UInt8*)pszFullPath, &mFSRef, &bFolder);
		if (rc != 0) {
			tchar pszErr[128];
			sprintf(pszErr, "FSPathMakeRef() call failed - OSStatus == %d", rc);
			throw IException::Create(IException::TypeCode, IException::ReasonCodeInvalidArgument, EXCEPTION_INFO, pszErr);
		}
		else {
			// Create iterator
			rc = FSOpenIterator(&mFSRef, kFSIterateFlat, &mIterator);
			if (rc != 0) {
				tchar pszErr[128];
				sprintf(pszErr, "FSOpenIterator() call failed - OSStatus == %d", rc);
				throw IException::Create(IException::TypeCode, IException::ReasonCodeInvalidArgument, EXCEPTION_INFO, pszErr);
			}
		}
	}
	
	mbInitSuccess = true;
} // Init


// Lasse, added 2008-04-08
tbool CFileSearchOSX::Init2(const tchar* pszPathAndMask, tchar* pszErrMsgBuff /*= NULL*/, tint32 iErrMsgBuffSize /*= -1*/)
{
	mbInitSuccess = false;
	try {
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
			printf("\n*************************************\n internal format: ");
			printf(pszInternalFormat);
				   
			Init(pszInternalFormat);
		}
	}
	catch (IException* pex) {
		if (pszErrMsgBuff) {
			if (iErrMsgBuffSize < 0)
				strcpy(pszErrMsgBuff, pex->GetDescription());
			else if (iErrMsgBuffSize > 1) {
				strncpy(pszErrMsgBuff, pex->GetDescription(), iErrMsgBuffSize-1);
				pszErrMsgBuff[iErrMsgBuffSize-1] = '\0';
			}
		}
		pex->Destroy();
	}
	return mbInitSuccess;
} // Init2
// .. Lasse


tbool CFileSearchOSX::GetNext(tchar* pszName, tbool& rDirectory)
{
	OSStatus outStatus = noErr;
	tbool bFound = false;
	//HFSUniStr255 unicodeName;
	tbool bIsFolder;
	tbool bIsVisible;
	
	// Lasse, added 2008-04-08
	if (!mbInitSuccess)
		return false;
	// .. Lasse
	
	do {
		if (miBulkIx<0) {
			// Load a new bulk of info
			outStatus = FSGetCatalogInfoBulk(mIterator, kRequestCountPerIteration,
				&mActualCountBulk, NULL, kCatalogInfoBitmap, mCatalogInfoArray, NULL, NULL, mNames);
			miBulkIx = 0;
		}
		if ((outStatus == noErr) || ((outStatus == errFSNoMoreItems) && (miBulkIx < mActualCountBulk))) {
			FSCatalogInfo* pInfoCurr = &mCatalogInfoArray[miBulkIx];
			HFSUniStr255* pUnistrCurr = &mNames[miBulkIx];
			
			// Find name
			{
				bIsFolder = ((pInfoCurr->nodeFlags & kFSNodeIsDirectoryMask) != 0);
				if (bIsFolder) {
					FolderInfo* folderinfo = (FolderInfo*)&pInfoCurr->finderInfo;
					bIsVisible = ((folderinfo->finderFlags & kIsInvisible) == 0);
				}
				else {
					FileInfo* fileinfo = (FileInfo*)&pInfoCurr->finderInfo;
					bIsVisible = ((fileinfo->finderFlags & kIsInvisible) == 0);
				}
		
				if (bIsVisible) {
					tbool bTwoByte = false;
					tchar* pcReturn = pszName;
					tint iUnicodeChar;
					
					// Convert from 16-bit unicode to 8-bit tchar*
					for (tint iCharIx=0; (!bTwoByte) && (iCharIx<pUnistrCurr->length); iCharIx++) {
						iUnicodeChar = pUnistrCurr->unicode[iCharIx];
						if ((iUnicodeChar & 0xFF00) != 0) {
							// That's a two-byte char - it can't be converted to a tchar* string
							bTwoByte = true;
						}
						else {
							*pcReturn++ = (tchar)iUnicodeChar;
						}
					}
					
					// Zero-terminate
					*pcReturn = 0;
					
					// Compare with filter
					if (!bTwoByte) {
						tint iNameLen = pUnistrCurr->length;
						if (msFilter.length() == 0) {
							bFound = true;
						}
						else if (iNameLen >= msFilter.length()) {
							if (stricmp(pszName + (iNameLen - msFilter.length()), msFilter.c_str()) == 0) {
								bFound = true;
							}
						}
					}
				}
			}
			
			// Advance to next info
			if (++miBulkIx >= mActualCountBulk) {
				// Signal that we must load new bulk next time
				miBulkIx = -1;
			}
			
			// Maybe return found name
			if (bFound) {
				rDirectory = bIsFolder;
				return true;
			}			
		}
	} while ((outStatus == noErr) || ((outStatus == errFSNoMoreItems) && (miBulkIx < mActualCountBulk)));

	// No more
	return false;
} // GetNext


