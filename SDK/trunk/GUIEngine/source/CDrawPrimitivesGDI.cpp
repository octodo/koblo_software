/*!	\file CDrawPrimitivesGDI.cpp
	\author Michael Olsen
	\date 18/Mar/2005
	\date 18/Mar/2005
*/


#include "geInternalOS.h"


CDrawPrimitivesGDI::CDrawPrimitivesGDI()
	: mpWnd(NULL)
{
}

CDrawPrimitivesGDI::~CDrawPrimitivesGDI()
{
}

IDrawPrimitives* IDrawPrimitives::Create()
{
	return dynamic_cast<IDrawPrimitives*>(new CDrawPrimitivesGDI());
}

void CDrawPrimitivesGDI::Destroy()
{
	delete dynamic_cast<CDrawPrimitivesGDI*>(this);
}

void CDrawPrimitivesGDI::Init(IWindow* pWnd)
{
	mpWnd = pWnd;
}

void CDrawPrimitivesGDI::DrawPoint(const SRect& RectUpdate, const SPos& Pos, const SRGB& Colour)
{
	// Get buffer to draw into
	tuchar* pDestBuffer;
	mpWnd->GetDrawPointer((void**)&pDestBuffer);

	// Bytes per row in the destination buffer
	SSize SizeDest;
	mpWnd->GetSize(SizeDest);
	tint iBytesPerRowDest = SizeDest.iCX * 4;

	// Check update rectangle
	if (Pos.iX >= RectUpdate.iX) {
		if (Pos.iX < RectUpdate.iX + RectUpdate.iCX) {
			if (Pos.iY >= RectUpdate.iY) {
				if (Pos.iY < RectUpdate.iY + RectUpdate.iCY) {
					// Destination buffer, based on position
					tuchar* pDest = pDestBuffer + Pos.iY * iBytesPerRowDest + Pos.iX * 4;

					// Access the colour as a 32-bit integer
					tuint32 iRGB = Colour.u.pixel;

					*((tuint32*)pDest) = iRGB;
				}
			}
		}
	}
}

void CDrawPrimitivesGDI::DrawLine(const SRect& RectUpdate, const SPos& PosFrom, const SPos& PosTo, const SRGB& Colour)
{
	// Get buffer to draw into
	tuchar* pDestBuffer;
	mpWnd->GetDrawPointer((void**)&pDestBuffer);

	// Bytes per row in the destination buffer
	SSize SizeDest;
	mpWnd->GetSize(SizeDest);
	tint iBytesPerRowDest = SizeDest.iCX * 4;

	SPos PosStart = PosFrom;
	SPos PosEnd = PosTo;

	// Width
	tint cx = PosEnd.iX - PosStart.iX;
	// Height
	tint cy = PosEnd.iY - PosStart.iY;

	if (abs(cx) > abs(cy)) {
		if (PosStart.iX > PosEnd.iX) {
			Swap(PosStart.iX, PosEnd.iX);
			Swap(PosStart.iY, PosEnd.iY);
			cx = PosEnd.iX - PosStart.iX;
			cy = PosEnd.iY - PosStart.iY;
		}

		tfloat grad = 0;
		if (cx != 0) {
			grad = cy / (tfloat)cx;
		}
		tfloat yf = (tfloat)PosStart.iY;

		for (tint x = PosStart.iX; x <= PosEnd.iX && x < SizeDest.iCX && x < RectUpdate.iX + RectUpdate.iCX; x++) {
			if (x < RectUpdate.iX) {
				continue;
			}
			tint offset = x * 4 + (tint)yf * iBytesPerRowDest;

			if ((tint)yf < SizeDest.iCY && (tint)yf < RectUpdate.iY + RectUpdate.iCY && (tint)yf >= RectUpdate.iY) {
				tuchar* pDest = &pDestBuffer[offset];
				*((tuint*)pDest) = Colour.u.pixel;
			}

			yf += grad;
		}
	}
	else {
		if (PosStart.iY > PosEnd.iY) {
			Swap(PosStart.iX, PosEnd.iX);
			Swap(PosStart.iY, PosEnd.iY);
			cx = PosEnd.iX - PosStart.iX;
			cy = PosEnd.iY - PosStart.iY;
		}

		tfloat grad = 0;
		if (cy != 0) {
			grad = cx / (tfloat)cy;
		}
		tfloat xf = (tfloat)PosStart.iX;

		for (tint y = PosStart.iY; y <= PosEnd.iY && y < SizeDest.iCY && y < RectUpdate.iY + RectUpdate.iCY; y++) {
			if (y < RectUpdate.iY) {
				continue;
			}
			tint offset = (tint)xf * 4 + y * iBytesPerRowDest;

			if ((tint)xf < SizeDest.iCX && (tint)xf < RectUpdate.iX + RectUpdate.iCX && (tint)xf >= RectUpdate.iX) {
				tuchar* pDest = &pDestBuffer[offset];
				*((tuint*)pDest) = Colour.u.pixel;
			}

			offset = ((tint)xf + 1) * 4 + y * iBytesPerRowDest;

			xf += grad;
		}
	}
}

void CDrawPrimitivesGDI::DrawRect(const SRect& RectUpdate, const SRect& RectFill, const SRGB& Colour)
{
	// Get buffer to draw into
	tuchar* pDestBuffer;
	mpWnd->GetDrawPointer((void**)&pDestBuffer);

	// Bytes per row in the destination buffer
	SSize SizeDest;
	mpWnd->GetSize(SizeDest);
	tint iBytesPerRowDest = SizeDest.iCX * 4;

	// Our start position depends on where to draw to and update rectangle
	tint iStartPosX = max(RectFill.iX, RectUpdate.iX);
	// Our end position depends on where to draw to and update rectangle
	tint iEndPosX = min(RectFill.iX + RectFill.iCX - 1, RectUpdate.iX + RectUpdate.iCX - 1);
	// The end position also depends on the actual size of the rect we're drawing
	tint iThiscx = iEndPosX - iStartPosX + 1;

	// What we did to X above, we also have to do with Y
	tint iStartPosY = max(RectFill.iY, RectUpdate.iY);
	tint iEndPosY = min(RectFill.iY + RectFill.iCY - 1, RectUpdate.iY + RectUpdate.iCY - 1);
	tint iThiscy = iEndPosY - iStartPosY + 1;

	// Access the colour as a 32-bit integer
	tuint32 iRGB = Colour.u.pixel;

	for (int y = iStartPosY; y <= iEndPosY; y++) {
		// Current destination buffer, based on position
		tuchar* pDest = pDestBuffer + y * iBytesPerRowDest + iStartPosX * 4;

		for (int x = iStartPosX; x <= iEndPosX; x++) {
			*((tuint32*)pDest) = iRGB;

			pDest += 4;
		}
	}
}

void CDrawPrimitivesGDI::InvertRect(const SRect& RectUpdate, const SRect& RectInvert)
{
	// Get buffer to draw into
	tuchar* pDestBuffer;
	mpWnd->GetDrawPointer((void**)&pDestBuffer);

	// Bytes per row in the destination buffer
	SSize SizeDest;
	mpWnd->GetSize(SizeDest);
	tint iBytesPerRowDest = SizeDest.iCX * 4;

	// Our start position depends on where to draw to and update rectangle
	tint iStartPosX = max(RectInvert.iX, RectUpdate.iX);
	// Our end position depends on where to draw to and update rectangle
	tint iEndPosX = min(RectInvert.iX + RectInvert.iCX - 1, RectUpdate.iX + RectUpdate.iCX - 1);
	// The end position also depends on the actual size of the rect we're drawing
	tint iThiscx = iEndPosX - iStartPosX + 1;

	// What we did to X above, we also have to do with Y
	tint iStartPosY = max(RectInvert.iY, RectUpdate.iY);
	tint iEndPosY = min(RectInvert.iY + RectInvert.iCY - 1, RectUpdate.iY + RectUpdate.iCY - 1);
	tint iThiscy = iEndPosY - iStartPosY + 1;

	for (int y = iStartPosY; y <= iEndPosY; y++) {
		// Current destination buffer, based on position
		tuchar* pDest = pDestBuffer + y * iBytesPerRowDest + iStartPosX * 4;

		for (int x = iStartPosX; x <= iEndPosX; x++) {
			*pDest = 255 - *pDest;
			pDest++;

			*pDest = 255 - *pDest;
			pDest++;

			*pDest = 255 - *pDest;
			pDest++;

			pDest++;
		}
	}
}



