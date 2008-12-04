/*!	\file CMemoryDC.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/


#include "geInternalOS.h"


//SRGB gBackgroundColour(217, 217, 217);

CMemoryDC::CMemoryDC()
	: mpBitmapRes(NULL), miFrameIndex(0), mSize(0, 0)
{
}

CMemoryDC::~CMemoryDC()
{
}

void CMemoryDC::Init(IWindow* /*pWnd*/, IBitmapResource* pRes, tint iFrameIndex, tint iVariantIndex)
{
	mpBitmapRes = pRes;
	miFrameIndex = iFrameIndex;
	miVariantIndex = iVariantIndex;

	mpBitmapRes->GetRealSize(mSize);

/*	tint iNrOfBitmaps = miNrOfFrames * miNrOfVariants;
	mppcData = new tuchar*[iNrOfBitmaps];

	const tuchar* pDataSrc = mpBitmapRes->GetData();

	// Copy bitmap to multiple smaller bitmaps
	tint iVariant;
	for (iVariant = 0; iVariant < iNrOfVariants; iVariant++) {
		tint iFrame;
		for (iFrame = 0; iFrame < iNrOfFrames; iFrame++) {
			// The destination bitmap we're working on
			tint iIndex = iVariant * iNrOfFrames + iFrame;

			// Allocate destination bitmap
			mppcData[iIndex] = new tuchar[mSize.iCX * mSize.iCY * 4];

			// Do the actual copying
			for (tint iY = 0; iY < mSize.iCY; iY++) {
				// Our source pointer
				const tuchar* pSrc = pDataSrc;
				// Increment for variant
				pSrc += (iVariant * miNrOfFrames * mSize.iCY * mSize.iCX * 4);
				// Increment for frame
				pSrc += (iFrame * mSize.iCX * 4);
				// Increment for current y position
				pSrc += iY * miNrOfFrames * mSize.iCX * 4;

				// Our destination pointer
				tuchar* pDest = mppcData[iIndex];
				// Increment for current y position
				pDest += (iY * mSize.iCX * 4);

				if (pRes->GetBitsPerPixel() == 24) {
					// non-alpha bitmap
					for (tint iX = 0; iX < mSize.iCX; iX++) {
						*pDest++ = *pSrc++;
						*pDest++ = *pSrc++;
						*pDest++ = *pSrc++;
						*pDest++ = *pSrc++;
					}
				}
				else {
					// alpha bitmap
					for (tint iX = 0; iX < mSize.iCX; iX++) {
						tint iAlpha = pSrc[3];

						pDest[0] = (((pSrc[0] * iAlpha) >> 8) +
							((gBackgroundColour.gun.b * (255 - iAlpha)) >> 8));

						pDest[1] = (((pSrc[1] * iAlpha) >> 8) +
							((gBackgroundColour.gun.g * (255 - iAlpha)) >> 8));

						pDest[2] = (((pSrc[2] * iAlpha) >> 8) +
							((gBackgroundColour.gun.r * (255 - iAlpha)) >> 8));

						pDest += 4;
						pSrc += 4;
					}
				}
			}
		}
	}*/
}








