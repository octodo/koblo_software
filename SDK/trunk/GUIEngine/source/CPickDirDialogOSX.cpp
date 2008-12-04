/*!	\file CPickDirDialogOSX.cpp
	\author Michael Olsen
	\date 15/Jun/2005
	\date 15/Jun/2005
*/


#include "geInternalOS.h"


CPickDirDialogOSX::CPickDirDialogOSX()
{
}

CPickDirDialogOSX::~CPickDirDialogOSX()
{
}

IPickDirDialog* IPickDirDialog::Create()
{
	return dynamic_cast<IPickDirDialog*>(new CPickDirDialogOSX());
}

void CPickDirDialogOSX::Destroy()
{
	delete dynamic_cast<CPickDirDialogOSX*>(this);
}

void CPickDirDialogOSX::DoDialog(tchar* pszPathOut, const tchar* pszPath)
{
	tchar pszTmp[1024];
	strcpy(pszTmp, pszPath);
	IFile::PathToOS(pszTmp);
	std::string sPath((const char*)pszTmp);

	pszPathOut[0] = 0;

	NavDialogOptions navoptions;
	NavGetDefaultDialogOptions(&navoptions);

	// Convert to pascal string
	char pszPascal[256];
	strcpy((char*)&(pszPascal[1]), sPath.c_str());
	pszPascal[0] = strlen((const char*)&(pszPascal[1]));

	AEDesc suggested;
	FSSpec suggestedSpec;
	OSErr err = FSMakeFSSpec(0, 0, (StringPtr)pszPascal, &suggestedSpec);
	memcpy(navoptions.savedFileName,suggestedSpec.name,suggestedSpec.name[0]+1);
	suggestedSpec.name[0] = 0;
	err = AECreateDesc(typeFSS,&suggestedSpec,sizeof(FSSpec),&suggested);

	
	NavReplyRecord reply;
	err = ::NavChooseFolder(&suggested,
		&reply,
		&navoptions,
		0,
		0,
		0);

	AEDisposeDesc(&suggested);

	if (reply.validRecord && (err == noErr)) {
		long itemsInList;
		err = AECountItems (&reply.selection,&itemsInList);
		if ((itemsInList > 0) && (err == noErr)) {
			AEKeyword keywd;
			DescType returnedType;
			FSSpec theDocOpened;
			long actualSize;
			err = AEGetNthPtr(&reply.selection, 1, typeFSS, &keywd, &returnedType, 
					(Ptr) &theDocOpened, sizeof(theDocOpened), &actualSize);
			
			if (err == noErr) {
				char psz[512];
				FSPToCPath(&theDocOpened, (char*)psz);
				NavDisposeReply(&reply);
	
				strcpy((char*)pszPathOut, (const char*)psz);

				IFile::PathFromOS(pszPathOut);
			}
		}
	}

	NavDisposeReply(&reply);
}






