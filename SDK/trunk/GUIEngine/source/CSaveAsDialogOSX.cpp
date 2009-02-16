/*!	\file CSaveAsDialogOSX.cpp
	\author Michael Olsen
	\date 10/May/2005
	\date 10/May/2005
*/


#include "geInternalOS.h"


CSaveAsDialogOSX::CSaveAsDialogOSX() : miBundleBehaviour(0)
{
}

CSaveAsDialogOSX::~CSaveAsDialogOSX()
{
}

ISaveAsDialog* ISaveAsDialog::Create()
{
	return dynamic_cast<ISaveAsDialog*>(new CSaveAsDialogOSX());
}

void CSaveAsDialogOSX::Destroy()
{
	delete dynamic_cast<CSaveAsDialogOSX*>(this);
}

void CSaveAsDialogOSX::DoDialog(tchar* pszPathNameOut, const tchar* pszPathName, const tchar* pszFileType, const tchar* /*pszFileTypeDescription*/, const tchar* pszDefaultName)
{
	//!!! why does i got the extencion "%f" in the release build?????
	tchar pszTmp[1024];
	
	IFile::PathFromOS2(pszPathName, pszTmp, true);
	std::string sMusic_Koblo((const char*)pszTmp + 1);

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
	strcpy((char*)&(pszPascal[1]), sMusic_Koblo.c_str());
	if (pszDefaultName) {
		// If an extension is included in default-name (i.e. Windows style) we must remove it to avoid double extensions on OS X
		tbool bRemovedExt = false;
		if ((pszFileType) && (*pszFileType == '*') && (pszFileType[1] == '.')) {
			const tchar* pszExt = pszFileType + 1;
			tint32 iLenExt = strlen(pszExt);
			tint32 iLenDefaultName = strlen(pszDefaultName);
			tint32 iMaybeExtIndex = iLenDefaultName - iLenExt;
			if (iMaybeExtIndex >= 0) {
				if (stricmp(pszDefaultName + iMaybeExtIndex, pszExt) == 0) {
					// Yup - an extension must be removed
					bRemovedExt = true;
					strncat((char*)&(pszPascal[1]), (const char*)pszDefaultName, iMaybeExtIndex);
				}
			}
		}
		if (!bRemovedExt) {
			// We didn't remove an extension - just append entire default name string
			strcat((char*)&(pszPascal[1]), (const char*)pszDefaultName);
		}
	}
	else {
		strcat((char*)&(pszPascal[1]), (const char*)"New");
	}
	pszPascal[0] = strlen((const char*)&(pszPascal[1]));

	err = FSMakeFSSpec(0, 0, (StringPtr)pszPascal, &suggestedSpec);
	memcpy(navoptions.savedFileName,suggestedSpec.name,suggestedSpec.name[0]+1);
	suggestedSpec.name[0] = 0;
	err = AECreateDesc(typeFSS,&suggestedSpec,sizeof(FSSpec),&suggested);
	err = NavPutFile(&suggested, &reply,&navoptions,0,0,0,0);
	AEDisposeDesc(&suggested);
//	err = NavPutFile(nil, &reply,&navoptions,0,0,0,0);
	
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
				//printf("\npszPathName_Temp=%s\n", pszPathName_Temp);
				// Find out if we need to add extension
				tbool bAddExt = false;
				const tchar* pszExt = (*pszFileType) ? (const char*)(pszFileType + 1) : "";
				tint32 iLenExt = strlen(pszExt);
				tint32 iLenPath = strlen(pszPathNameOut);
				tint32 iExtIndex = iLenPath - iLenExt;
				if (iExtIndex <= 0) {
					bAddExt = true;
				}
				else {
					if (stricmp((pszPathNameOut + iExtIndex), pszExt) != 0) {
						bAddExt = true;
					}
				}
				if (bAddExt) {
					strcat((char*)pszPathNameOut, pszExt);
					//printf("\npszPathName_Temp=%s\n", pszPathName_Temp);
				}

				IFile::PathFromOS2(pszPathNameOut, pszPathNameOut);
			}
		}
	}

	NavDisposeReply(&reply);
}






