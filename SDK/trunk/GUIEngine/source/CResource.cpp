/*!	\file CResource.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/


#include "geInternalOS.h"


CResource::CResource() : miResID(-1), mResType(eResTypeUnknown), mpcData(NULL), miDataSize(0)
{
}

CResource::~CResource()
{
	if (mpcData) {
		delete[] mpcData;
	}
}

void CResource::Load(IWindow* /*pWnd*/, IFile* pFile, tint iSize, tint32 iResID)
{
	miResID = iResID;

	miDataSize = iSize;
	mpcData = new tuchar[miDataSize];
	if (pFile->Read((tchar*)mpcData, miDataSize) != miDataSize) {
		DecRefCount();
		return;
	}
}

tint32 CResource::GetResID() const
{
	return miResID;
}

tuchar* CResource::GetData()
{
	return mpcData;
}

IResource::EResourceType CResource::GetResType() const
{
	return mResType;
}



