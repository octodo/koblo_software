/*!	\file CMemoryDCGDI.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 16/Feb/2005
*/



#include "geInternalOS.h"

SRGB gBackgroundColour(217, 217, 217);

IDC* IDC::Create()
{
	return dynamic_cast<IDC*>(new CMemoryDCGDI());
}

void CMemoryDCGDI::Destroy()
{
	delete dynamic_cast<CMemoryDCGDI*>(this);
}

CMemoryDCGDI::CMemoryDCGDI()
{
}

CMemoryDCGDI::~CMemoryDCGDI()
{
}

void CMemoryDCGDI::Init(IWindow* pWnd, IBitmapResource* pRes, tint iFrameIndex, tint iVariantIndex)
{
	CMemoryDC::Init(pWnd, pRes, iFrameIndex, iVariantIndex);
}

void CMemoryDCGDI::Draw(IWindow* pWnd, const SRect& rctUpdate, const SRect& rctDraw)
{
/*	void* p;
	pWnd->GetHDC(&p);
	HDC hdcWindow = (HDC)p;

	// Change this to support variants
	tint iVariant = 0;

	// Image index
	tint iIndex = iVariant * miNrOfFrames + iFrame;

	tint iNrOfPixels = mSize.iCX * 4 * mSize.iCY;

	LPBITMAPINFOHEADER pHeader;
	pHeader = (LPBITMAPINFOHEADER)&mBmpInfo;
	pHeader->biWidth = mSize.iCX;
	pHeader->biHeight = 0 - mSize.iCY;
	pHeader->biSizeImage = iNrOfPixels;

	::SetDIBitsToDevice(hdcWindow,
		rctDraw.iX, rctDraw.iY,
		mSize.iCX, mSize.iCY,
		0, 0,
		0, mSize.iCY,
		mppcData[iIndex],
		&mBmpInfo,
		DIB_RGB_COLORS);*/

	if (mpBitmapRes->GetBitsPerPixel() == 32) {
		DrawAlpha(pWnd, rctUpdate, rctDraw);
		return;
	}

	// Get buffer to draw into
	tuchar* pDestBuffer;
	pWnd->GetDrawPointer((void**)&pDestBuffer);

	// Change this to support variants
//	tint iVariant = 0;

	// Locate buffer to draw from
	tuchar* pSrcBuffer = mpBitmapRes->GetData();
//	tuchar* pSrcBuffer = mppcData[iIndex];

	// Source buffer size
	SSize SizeSrc;
	mpBitmapRes->GetSize(SizeSrc);

	// Bytes per row in the source buffer
	tint iBytesPerRowSrc = SizeSrc.iCX * 4;
	// Bytes per row in the destination buffer
	SSize SizeDest;
	pWnd->GetSize(SizeDest);
	tint iBytesPerRowDest = SizeDest.iCX * 4;

	// Our start position depends on where to draw to and update rectangle
	tint iStartPosX = max(rctDraw.iX, rctUpdate.iX);
	// Our end position depends on where to draw to and update rectangle
	tint iEndPosX = min(rctDraw.iX + rctDraw.iCX - 1, rctUpdate.iX + rctUpdate.iCX - 1);
	// The end position also depends on the actual size of the bitmap we're drawing
	iEndPosX = min(iEndPosX, rctDraw.iX + mSize.iCX - 1);

	// What we did to X above, we also have to do with Y
	tint iStartPosY = max(rctDraw.iY, rctUpdate.iY);
	tint iEndPosY = min(rctDraw.iY + rctDraw.iCY - 1, rctUpdate.iY + rctUpdate.iCY - 1);
	iEndPosY = min(iEndPosY, rctDraw.iY + mSize.iCY - 1);

	// The X offset is the different between destination position and destination start position, i.e. it's an index into the source buffer
	tint iXOffset = iStartPosX - rctDraw.iX;
	// Same with Y offset
	tint iYOffset = iStartPosY - rctDraw.iY;

	for (tint y = iStartPosY; y <= iEndPosY; y++) {
		// Current destination buffer, based on position
		tuchar* pDest = pDestBuffer + y * iBytesPerRowDest + iStartPosX * 4;

		// Current source buffer, based on position
		const tuchar* pSrc = pSrcBuffer + (iYOffset + (y - iStartPosY)) * iBytesPerRowSrc + iXOffset * 4;

		// Offset source, based on frame
		pSrc += miFrameIndex * 4;

		// Offset source, based on variant
//		pSrc += mSize.iCY * iVariant * iBytesPerRowSrc;
		pSrc += miVariantIndex * iBytesPerRowSrc;

		for (int x = iStartPosX; x <= iEndPosX; x++) {
			*((tuint32*)pDest) = *((tuint32*)pSrc);
			pDest += 4;
			pSrc += 4;
		}
	}
}

void CMemoryDCGDI::DrawAlpha(IWindow* pWnd, const SRect& rctUpdate, const SRect& rctDraw)
{
	// Get buffer to draw into
	tuchar* pDestBuffer;
	pWnd->GetDrawPointer((void**)&pDestBuffer);

	// Change this to support variants
//	tint iVariant = 0;

	// Locate buffer to draw from
	tuchar* pSrcBuffer = mpBitmapRes->GetData();
//	tuchar* pSrcBuffer = mppcData[iIndex];

	// Source buffer size
	SSize SizeSrc;
	mpBitmapRes->GetSize(SizeSrc);

	// Bytes per row in the source buffer
	tint iBytesPerRowSrc = SizeSrc.iCX * 4;
	// Bytes per row in the destination buffer
	SSize SizeDest;
	pWnd->GetSize(SizeDest);
	tint iBytesPerRowDest = SizeDest.iCX * 4;

	// Our start position depends on where to draw to and update rectangle
	tint iStartPosX = max(rctDraw.iX, rctUpdate.iX);
	// Our end position depends on where to draw to and update rectangle
	tint iEndPosX = min(rctDraw.iX + rctDraw.iCX - 1, rctUpdate.iX + rctUpdate.iCX - 1);
	// The end position also depends on the actual size of the bitmap we're drawing
	iEndPosX = min(iEndPosX, rctDraw.iX + mSize.iCX - 1);

	// What we did to X above, we also have to do with Y
	tint iStartPosY = max(rctDraw.iY, rctUpdate.iY);
	tint iEndPosY = min(rctDraw.iY + rctDraw.iCY - 1, rctUpdate.iY + rctUpdate.iCY - 1);
	iEndPosY = min(iEndPosY, rctDraw.iY + mSize.iCY - 1);

	// The X offset is the different between destination position and destination start position, i.e. it's an index into the source buffer
	tint iXOffset = iStartPosX - rctDraw.iX;
	// Same with Y offset
	tint iYOffset = iStartPosY - rctDraw.iY;

	for (tint y = iStartPosY; y <= iEndPosY; y++) {
		// Current destination buffer, based on position
		tuchar* pDest = pDestBuffer + y * iBytesPerRowDest + iStartPosX * 4;

		// Current source buffer, based on position
		const tuchar* pSrc = pSrcBuffer + (iYOffset + (y - iStartPosY)) * iBytesPerRowSrc + iXOffset * 4;

		// Offset source, based on frame
		pSrc += miFrameIndex * 4;

		// Offset source, based on variant
//		pSrc += mSize.iCY * iVariant * iBytesPerRowSrc;
		pSrc += miVariantIndex * iBytesPerRowSrc;

		for (int x = iStartPosX; x <= iEndPosX; x++) {
			tuint uiAlpha = pSrc[3] + 1;
			tuint uiAlphaInv = 256 - uiAlpha;

			*((tuint*)pDest) = (((uiAlpha * *pSrc + uiAlphaInv * *pDest) >> 8)) |
				(((uiAlpha * pSrc[1] + uiAlphaInv * pDest[1]) >> 8) << 8) |
				(((uiAlpha * pSrc[2] + uiAlphaInv * pDest[2]) >> 8) << 16);

			pSrc += 4;
			pDest += 4;
		}
	}
}










