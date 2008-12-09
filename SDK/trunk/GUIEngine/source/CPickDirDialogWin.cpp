/*!	\file CPickDirDialogWin.cpp
	\author Michael Olsen
	\date 15/Jun/2005
	\date 15/Jun/2005
	\todo MAke CPickDirDialogWin work with other root folders than the desktop
*/


#include "geInternalOS.h"

#include <shlobj.h>


CPickDirDialogWin::CPickDirDialogWin()
{
}

CPickDirDialogWin::~CPickDirDialogWin()
{
}

IPickDirDialog* IPickDirDialog::Create()
{
	return dynamic_cast<IPickDirDialog*>(new CPickDirDialogWin());
}

void CPickDirDialogWin::Destroy()
{
	delete dynamic_cast<CPickDirDialogWin*>(this);
}


#ifndef BIF_USENEWUI
#define BIF_NEWDIALOGSTYLE     0x0040

#define BIF_USENEWUI           (BIF_NEWDIALOGSTYLE | BIF_EDITBOX)
#endif	// BIF_USENEWUI

void CPickDirDialogWin::DoDialog(tchar* pszPathOut, const tchar* pszPath)
{
	// Convert to OS path
	tchar pszTmp[512];
	strcpy((char*)pszTmp, (const char*)pszPath);
	IFile::PathToOS(pszTmp);
	std::string sPath((const char*)pszTmp);

	::CoInitialize(NULL);

	ITEMIDLIST* pItemList = NULL;
/*	if (sPath.size() != 0) {
		// Convert to unicode
		tint16 pUnicode[1024];
		if (::MultiByteToWideChar(CP_ACP,
			0,
			sPath.c_str(),
			-1,
			(LPWSTR)pUnicode,
			1024) == 0) {
			return;
		}

		IShellFolder* pDesktop;
		if (::SHGetDesktopFolder(&pDesktop) != 0) {
			return;
		}
		if (pDesktop->ParseDisplayName(NULL,
			NULL,
			(LPOLESTR)pUnicode,
			NULL,
			&pItemList,
			NULL) != 0) {
			// Maybe it would be better to continue with no default path, instead of returning

			pszPathOut[0] = 0;

			::CoUninitialize();
			return;
		}
		pDesktop->Release();
	}*/

	BROWSEINFO Info;
	Info.hwndOwner = NULL;
//	Info.pidlRoot = pItemList;
	Info.pidlRoot = NULL;
	Info.pszDisplayName = (char*)pszPathOut;
	Info.lpszTitle = "Please Select Folder";
	Info.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	Info.lpfn = NULL;
	Info.lParam = 0;
	Info.iImage = 0;

	ITEMIDLIST* pIDList = ::SHBrowseForFolder(&Info);

	if (pIDList == NULL) {
		pszPathOut[0] = 0;

		::CoUninitialize();
		return;
	}

	if (::SHGetPathFromIDList(pIDList, (char*)pszPathOut) == FALSE) {
		pszPathOut[0] = 0;

		::CoUninitialize();
		return;
	}

	::CoUninitialize();

	tint iSize = strlen((char*)pszPathOut);
	if (pszPathOut[iSize - 1] != '\\') {
		strcat((char*)pszPathOut, "\\");
	}

	IFile::PathFromOS(pszPathOut);
}






