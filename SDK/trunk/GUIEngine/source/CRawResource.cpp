/*!	\file CRawResource.cpp
	\author Lasse Steen Bohnstedt
	\date 2008-06-09
*/


#include "geInternalOS.h"


CRawResource::CRawResource()
{
	mResType = eResTypeRaw;
}

CRawResource::~CRawResource()
{
}

IRawResource* IRawResource::Create()
{
	return dynamic_cast<IRawResource*>(new CRawResource());
}

void CRawResource::Destroy()
{
	delete dynamic_cast<CRawResource*>(this);
}

void CRawResource::Load(IWindow* pWnd, IFile* pFile, tint iSize, tint32 iResID)
{
	CResource::Load(pWnd, pFile, iSize, iResID);

}
