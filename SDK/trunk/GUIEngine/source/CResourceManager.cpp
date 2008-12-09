/*!	\file CResourceManager.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/



#include "geInternalOS.h"


CResourceManager::CResourceManager()
{
}

CResourceManager::~CResourceManager()
{
	std::list<IResource*>::iterator it;
	for (it = mResources.begin(); it != mResources.end(); it++) {
		IResource* pRes = *it;
		pRes->DecRefCount();
	}
	mResources.clear();

/*	std::list<SResourceInfo>::iterator iit;
	for (iit = mIndexHeaders.begin(); iit != mIndexHeaders.end(); iit++) {
		SResourceInfo Info = *iit;
		IFile* pFile = Info.pFile;
		pFile->Destroy();
	}*/
	if (mIndexHeaders.size() != 0) {
		SResourceInfo Info = *(mIndexHeaders.begin());
		IFile* pFile = Info.pFile;
		pFile->Destroy();
	}
	mIndexHeaders.clear();
}

IResourceManager* IResourceManager::Create()
{
	return dynamic_cast<IResourceManager*>(new CResourceManager());
}

void CResourceManager::Destroy()
{
	delete dynamic_cast<CResourceManager*>(this);
}

void CResourceManager::AddResourceFile(IChunkFile *pFile)
{
	IChunk* pChunkList = pFile->GetChunk(giChunkResourceList, false);
	if (pChunkList == NULL) {
		// Not a resource file
		throw IException::Create(IException::TypeFile, IException::ReasonFileWrongType, EXCEPTION_INFO, (tchar*)"Resource chunk list not found in file");
	}

	IFile* pFileList = dynamic_cast<IFile*>(pChunkList);
	tint32 iVersion;
	tint32 iNrOfResources;
	pFileList->Read((tchar*)&iVersion, sizeof(tint32));
	pFileList->Read((tchar*)&iNrOfResources, sizeof(tint32));
	iVersion = CPSwap(iVersion);
	iNrOfResources = CPSwap(iNrOfResources);

	// Lasse, mod 2008-05-04 - memory and speed optimization
	// Until this mod a memory chunk was created for each resource index.
	// This would make Koblo Studio app loads take > 90 seconds and use up all of virtual
	// memory if a CChunkFileMemory (built-in resources) was used instead of
	// a CChunkFile (resources loaded from .k2s file).
	// The gain of setting bCopyMem to false is almost zero compared to moving
	// the allocation outside of the for loop - but still a few CPU cycles spared there too.
	IChunk* pChunk = pFile->GetChunk(giChunkResources, false);
	// .. Lasse

	for (tint i = 0; i < iNrOfResources; i++) {
		SResourceInfo Info;
		pFileList->Read((tchar*)&Info, sizeof(tint32)*4);

		Info.iID = CPSwap(Info.iID);
		Info.iType = CPSwap(Info.iType);
		Info.iFileOffset = CPSwap(Info.iFileOffset);
		Info.iSize = CPSwap(Info.iSize);

		// Lasse, mod 2008-05-04 - memory and speed optimization
		// - see comment just above for loop
		//IChunk* pChunk = pFile->GetChunk(giChunkResources);
		// .. Lasse
		Info.pFile = dynamic_cast<IFile*>(pChunk);

		mIndexHeaders.insert(mIndexHeaders.end(), Info);
	}

	pChunkList->Destroy();

	// We're done with the file, so we can safely destroy it
	pFile->Destroy();
}

IBitmapResource* CResourceManager::GetBitmap(IWindow* pWnd, tint32 iResID, tint iNrOfFrames, tint iNrOfVariants)
{
	IBitmapResource* pBitmapResource = dynamic_cast<IBitmapResource*>(FindResource(iResID));
	if (pBitmapResource != NULL) {
		// Already exist
		pBitmapResource->IncRefCount();
		return pBitmapResource;
	}
	else {
		// Create new
		// Find in resource list
		std::list<SResourceInfo>::iterator it;
		for (it = mIndexHeaders.begin(); it != mIndexHeaders.end(); it++) {
			if ((*it).iID == iResID) {
				(*it).pFile->Seek((*it).iFileOffset);
				pBitmapResource = IBitmapResource::Create(iNrOfFrames, iNrOfVariants);
				pBitmapResource->Load(pWnd, (*it).pFile, (*it).iSize, iResID);

				// Increase ref count by 1, because we put it in list
				pBitmapResource->IncRefCount();

				mResources.insert(mResources.end(), pBitmapResource);
				return pBitmapResource;
			}
		}
		return NULL;
	}
}

IFontResource* CResourceManager::GetFont(IWindow* pWnd, tint32 iResID, tint iNrOfChars, const tint* piCharSizes, const SRGB& TextColour)
{
	IFontResource* pFontResource = dynamic_cast<IFontResource*>(FindResource(iResID, TextColour));
	if (pFontResource != NULL) {
		// Already exist
		pFontResource->IncRefCount();
		return pFontResource;
	}
	else {
	IFontResource* pFontResource = NULL;
		// Create new
		// Find in resource list
		std::list<SResourceInfo>::iterator it;
		for (it = mIndexHeaders.begin(); it != mIndexHeaders.end(); it++) {
			if ((*it).iID == iResID) {
				(*it).pFile->Seek((*it).iFileOffset);
				pFontResource = IFontResource::Create();
				pFontResource->Load(pWnd, (*it).pFile, (*it).iSize, iResID, iNrOfChars, piCharSizes, TextColour);

				// Increase ref count by 1, because we put it in list
				pFontResource->IncRefCount();

				mResources.insert(mResources.end(), pFontResource);
				return pFontResource;
			}
		}
		return NULL;
	}
}

IRawResource* CResourceManager::GetRawResource(tint32 iResID)
{
	IRawResource* pResource = dynamic_cast<IRawResource*>(FindResource(iResID));
	if (pResource != NULL) {
		// Already exist
		pResource->IncRefCount();
		return pResource;
	}
	else {
		// Create new
		// Find in resource list
		std::list<SResourceInfo>::iterator it;
		for (it = mIndexHeaders.begin(); it != mIndexHeaders.end(); it++) {
			if ((*it).iID == iResID) {
				(*it).pFile->Seek((*it).iFileOffset);
				pResource = IRawResource::Create();
				pResource->Load(NULL, (*it).pFile, (*it).iSize, iResID);

				// Increase ref count by 1, because we put it in list
				pResource->IncRefCount();

				mResources.insert(mResources.end(), pResource);
			}
		}
	}
	return pResource;
}


IResource* CResourceManager::FindResource(tint32 iResID)
{
	IResource* pRet = NULL;
	std::list<IResource*>::iterator it;
	for (it = mResources.begin(); (it != mResources.end()) && (pRet == NULL); it++) {
		if ((*it)->GetResID() == iResID) {
			pRet = (*it);
		}
	}

	return pRet;
}

IResource* CResourceManager::FindResource(tint32 iResID, const SRGB& RGB)
{
	IResource* pRet = NULL;
	std::list<IResource*>::iterator it;
	for (it = mResources.begin(); (it != mResources.end()) && (pRet == NULL); it++) {
		if ((*it)->GetResID() == iResID) {
			// We know this is a font resource
			CFontResource* pFont = dynamic_cast<CFontResource*>((*it));
			ASSERT(pFont);
			SRGB Colour = pFont->GetColour();
			if ((Colour.u.gun.r == RGB.u.gun.r) && (Colour.u.gun.g == RGB.u.gun.g) && (Colour.u.gun.b == RGB.u.gun.b)) {
				pRet = (*it);
			}
		}
	}

	return pRet;
}



