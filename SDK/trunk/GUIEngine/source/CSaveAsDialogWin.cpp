/*!	\file CSaveAsDialogWin.cpp
	\author Michael Olsen
	\date 05/Apr/2005
	\date 05/Apr/2005
*/


#include "geInternalOS.h"


CSaveAsDialogWin::CSaveAsDialogWin()
{
}

CSaveAsDialogWin::~CSaveAsDialogWin()
{
}

ISaveAsDialog* ISaveAsDialog::Create()
{
	return dynamic_cast<ISaveAsDialog*>(new CSaveAsDialogWin());
}

void CSaveAsDialogWin::Destroy()
{
	delete dynamic_cast<CSaveAsDialogWin*>(this);
}

void CSaveAsDialogWin::DoDialog(tchar* pszPathNameOut, const tchar* pszPathName, const tchar* pszFileType, const tchar* pszFileTypeDescription, const tchar* pszDefaultName)
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

	char pchFileTypes[512];
	memset(pchFileTypes, 0, 512 * sizeof(char));
	strcpy(pchFileTypes, (char*)pszFileTypeDescription);
	strcpy(pchFileTypes + strlen((char*)pszFileTypeDescription) + 1, (char*)pszFileType);

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
	OpenFileName.lpstrDefExt = (char*)pszFileType;
	OpenFileName.lCustData = 0;
	OpenFileName.lpfnHook = NULL;
	OpenFileName.lpTemplateName = NULL;
	BOOL bRet = ::GetSaveFileName(&OpenFileName);

	if (bRet == FALSE) {
		pszPathNameOut[0] = 0;
	}
	else {
		IFile::PathFromOS(pszPathNameOut);
	}
}






