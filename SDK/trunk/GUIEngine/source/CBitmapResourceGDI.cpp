/*!	\file CBitmapResourceGDI.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/



#include "geInternal.h"

CBitmapResourceGDI::CBitmapResourceGDI(tint iNrOfFrames, tint iNrOfVariants)
	: CBitmapResource(iNrOfFrames, iNrOfVariants)
{
}

CBitmapResourceGDI::~CBitmapResourceGDI()
{
}

IBitmapResource* IBitmapResource::Create(tint iNrOfFrames, tint iNrOfVariants)
{
	return dynamic_cast<IBitmapResource*>(new CBitmapResourceGDI(iNrOfFrames, iNrOfVariants));
}

void CBitmapResourceGDI::Destroy()
{
	delete dynamic_cast<CBitmapResourceGDI*>(this);
}

void CBitmapResourceGDI::Load(IWindow* pWnd, IFile* pFile, tint iSize, tint32 iResID)
{
	CBitmapResource::Load(pWnd, pFile, iSize, iResID);

	CMemoryDCGDI::Init(pWnd, dynamic_cast<IBitmapResource*>(this), GetNrOfFrames(), GetNrOfVariants());

	// The data cannot be deleted anymore, it is used for text blending
/*	// The data is not needed anymore
	delete[] mpcData;
	mpcData = NULL;*/
}

void CBitmapResourceGDI::ReInit(IWindow* pWnd)
{
	CMemoryDCGDI::Init(pWnd, dynamic_cast<IBitmapResource*>(this), GetNrOfFrames(), GetNrOfVariants());
}

void CBitmapResourceGDI::Draw(IWindow* pWnd, const SRect& rctUpdate, const SRect& rctDraw, tint iFrame)
{
	CMemoryDCGDI::Draw(pWnd, rctUpdate, rctDraw, iFrame);
}
