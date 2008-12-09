/*!	\file COpenDialogWin.cpp
	\author Michael Olsen
	\date 27/May/2005
	\date 27/May/2005
*/


#include "geInternalOS.h"


COpenDialogWin::COpenDialogWin()
{
}

COpenDialogWin::~COpenDialogWin()
{
}

IOpenDialog* IOpenDialog::Create()
{
	return dynamic_cast<IOpenDialog*>(new COpenDialogWin());
}

void COpenDialogWin::Destroy()
{
	delete dynamic_cast<COpenDialogWin*>(this);
}

void COpenDialogWin::DoDialog(tchar* pszPathNameOut, const tchar* pszPathName, const tchar* pszFileType, const tchar* pszFileTypeDescription, const tchar* pszDefaultName /*= NULL*/)
{
	std::string sPathName((const char*)pszPathName);
	pszPathNameOut[0] = 0;

	if ((pszDefaultName)&&(strlen((char*)pszDefaultName))) {
		tchar pszPathOnly[512];
		tchar pszNameOnly[512];
		if (IFile::SplitPathToPathAndName(pszDefaultName, pszPathOnly, pszNameOnly)) {
			std::string sPathOnly((const char*)pszPathOnly);
			std::string sNameOnly((const char*)pszNameOnly);
			if (sPathOnly.size()) {
				// Override the path in pszPathName
				sPathName = sPathOnly;
				// (mo) Note: The call does not accept relative paths, so ignore the comment just below
/*				if (IFile::IsRelativePath((tchar*)sPathName.c_str())) {
					// TODO: It's a relative path, so we really should prepend the "current path"
					tint32 iDummyForBreakPoint = 0;
				}*/
			}
			strcpy((char*)pszPathNameOut, sNameOnly.c_str());
		}
	}

	// Convert to OS path
	tchar pszTmp[512];
	strcpy((char*)pszTmp, sPathName.c_str());
	IFile::PathToOS(pszTmp);
	sPathName = std::string((const char*)pszTmp);

	// Lasse, mod 2008-08-19 - allow for more than one filter separated by '|' and possibly ';'
	/*
	char pchFileTypes[512];
	memset(pchFileTypes, 0, 512 * sizeof(char));
	strcpy(pchFileTypes, (char*)pszFileTypeDescription);
	strcpy(pchFileTypes + strlen((char*)pszFileTypeDescription) + 1, (char*)pszFileType);
	*/
	char pszDefaultExt[128];
	*pszDefaultExt = '\0';
	char pchFileTypes[1024];
	memset(pchFileTypes, 0, 1024 * sizeof(char));
	char* pcSrc0 = (char*)pszFileTypeDescription;
	char* pcSrc1 = (char*)pszFileType;
	char* pcDst = pchFileTypes;
	while (*pcSrc0) {
		char* pcSrc0End = pcSrc0 + 1;
		while ((*pcSrc0End) && (*pcSrc0End != '|'))
			pcSrc0End++;
		// Copy file type description
		tint32 iLen0 = pcSrc0End - pcSrc0;
		memcpy(pcDst, pcSrc0, iLen0);
		// Increase pointers
		pcSrc0 += iLen0;
		if (*pcSrc0)
			pcSrc0++;
		pcDst += iLen0;
		*pcDst++ = '\0';

		char* pcSrc1End = pcSrc1 + 1;
		char* pcSemiColon = NULL;
		while ((*pcSrc1End) && (*pcSrc1End != '|')) {
			if (*pcSrc1End == ';')
				pcSemiColon = pcSrc1End;
			pcSrc1End++;
		}
		// Copy extensions valid for file type (separeted by ';')
		tint32 iLen1 = pcSrc1End - pcSrc1;
		memcpy(pcDst, pcSrc1, iLen1);
		tint32 iLenDefaultExt = iLen1;
		// Copy only very first extension
		if (pcSemiColon)
			iLenDefaultExt = pcSemiColon - pcSrc1;
		memcpy(pszDefaultExt, pcSrc1, iLenDefaultExt);
		pszDefaultExt[iLenDefaultExt] = '\0';
		// Increase pointers
		pcSrc1 += iLen1;
		if (*pcSrc1)
			pcSrc1++;
		pcDst += iLen1;
		*pcDst++ = '\0';
	}
	// .. Lasse

	OPENFILENAME OpenFileName;
	OpenFileName.lStructSize = sizeof(OPENFILENAME);
	OpenFileName.hwndOwner = NULL;
	OpenFileName.hInstance = NULL;
	OpenFileName.lpstrFilter = (char*)pchFileTypes;
	OpenFileName.lpstrCustomFilter = NULL;
	OpenFileName.nMaxCustFilter = 0;
	OpenFileName.nFilterIndex = 1;
	OpenFileName.lpstrFile = (char*)pszPathNameOut;
	OpenFileName.nMaxFile = _MAX_PATH;
	OpenFileName.lpstrFileTitle = NULL;
	OpenFileName.nMaxFileTitle = 0;
	OpenFileName.lpstrInitialDir = sPathName.c_str();
	OpenFileName.lpstrTitle = NULL;
	OpenFileName.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	OpenFileName.nFileOffset = 0;
	OpenFileName.nFileExtension = 0;
	OpenFileName.lpstrDefExt = pszDefaultExt;	//(char*)pszFileType;
	OpenFileName.lCustData = 0;
	OpenFileName.lpfnHook = NULL;
	OpenFileName.lpTemplateName = NULL;
	BOOL bRet = ::GetOpenFileName(&OpenFileName);

	if (bRet == FALSE) {
		pszPathNameOut[0] = 0;
	}
	else {
		IFile::PathFromOS(pszPathNameOut);
	}
}






