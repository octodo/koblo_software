/*!	\file COpenDialogOSX.cpp
	\author Michael Olsen
	\date 18/May/2005
	\date 18/May/2005
*/


#include "geInternalOS.h"


COpenDialogOSX::COpenDialogOSX()
{
}

COpenDialogOSX::~COpenDialogOSX()
{
}

IOpenDialog* IOpenDialog::Create()
{
	return dynamic_cast<IOpenDialog*>(new COpenDialogOSX());
}

void COpenDialogOSX::Destroy()
{
	delete dynamic_cast<COpenDialogOSX*>(this);
}

void COpenDialogOSX::DoDialog(tchar* pszPathNameOut, const tchar* pszPathName, const tchar* /*pszFileType*/, const tchar* /*pszFileTypeDescription*/, const tchar* pszDefaultName)
{
	tchar pszTmp[1024];
	strcpy(pszTmp, pszPathName);
	IFile::PathFromOS2(pszPathName, pszTmp);
	std::string sPathName((const char*)pszTmp + 1);

	pszPathNameOut[0] = 0;

	NavReplyRecord 		reply;
	OSErr				err;
	long				itemsInList;
	long				actualSize;
	FSSpec				theDocOpened;
	AEKeyword			keywd;
	DescType			returnedType;
	AEDesc				suggested;
	FSSpec				suggestedSpec;
	NavDialogOptions	navoptions;

	NavGetDefaultDialogOptions(&navoptions);

	switch (miBundleBehaviour) {
		case 1: // Handle bundles
			navoptions.dialogOptionFlags |= (kNavNoTypePopup | kNavSupportPackages);
			break;
		case 2: // Browse bundles
			navoptions.dialogOptionFlags |= (kNavNoTypePopup | kNavAllowOpenPackages);
			break;
		default: // Ignore bundles
			navoptions.dialogOptionFlags |= kNavNoTypePopup;
			break;
	}	
	
	// Convert to pascal string, and add file name
	char pszPascal[256];
	strcpy((char*)&(pszPascal[1]), sPathName.c_str());
	if ((pszDefaultName) && (*pszDefaultName))
		strcat((char*)&(pszPascal[1]), pszDefaultName);
	else
		strcat((char*)&(pszPascal[1]), (const char*)"New");
	pszPascal[0] = strlen((const char*)&(pszPascal[1]));

	err = FSMakeFSSpec(0, 0, (StringPtr)pszPascal, &suggestedSpec);
	memcpy(navoptions.savedFileName,suggestedSpec.name,suggestedSpec.name[0]+1);
	suggestedSpec.name[0] = 0;
	err = AECreateDesc(typeFSS,&suggestedSpec,sizeof(FSSpec),&suggested);
	err = NavGetFile(&suggested, &reply,&navoptions,0,0,0,0,0);
	AEDisposeDesc(&suggested);
//	err = NavGetFile(nil, &reply,&navoptions,0,0,0,0);
	
	if (reply.validRecord && (err == noErr)) {
		
		err = AECountItems (&reply.selection,&itemsInList);
		if ((itemsInList > 0) && (err == noErr)) {
		
			err = AEGetNthPtr(&reply.selection, 1, typeFSS, &keywd, &returnedType, 
					(Ptr) &theDocOpened, sizeof(theDocOpened), &actualSize);
			
			if (err == noErr) {
				char psz[512];
				FSPToCPath(&theDocOpened, (char*)psz);
				NavDisposeReply(&reply);
	
				strcpy((char*)pszPathNameOut, (const char*)psz);

				IFile::PathFromOS(pszPathNameOut);
			}
		}
	}

	NavDisposeReply(&reply);
}






