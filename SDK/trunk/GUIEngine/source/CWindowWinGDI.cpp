/*!	\file CWindowGDI.cpp
	\author Michael Olsen
	\date 30/Dec/2004
	\date 30/Dec/2004
*/


#include "geInternalOS.h"

CWindowWinGDI::CWindowWinGDI()
	: mhdc(NULL), mpcDrawBuffer(NULL), miDrawBufferSize(0)
{
	LPBITMAPINFOHEADER pHeader;
	pHeader = (LPBITMAPINFOHEADER)&mBmpInfo;
	pHeader->biSize = sizeof(BITMAPINFOHEADER);
	pHeader->biWidth = 0;
	pHeader->biHeight = 0;
	pHeader->biPlanes = 1;
	pHeader->biBitCount = 32;
	pHeader->biCompression = BI_RGB;
	pHeader->biSizeImage = 0;
	pHeader->biXPelsPerMeter = 0;
	pHeader->biYPelsPerMeter = 0;
	pHeader->biClrUsed = 0;
	pHeader->biClrImportant = 0;
}

CWindowWinGDI::~CWindowWinGDI()
{
	if (mpcDrawBuffer) {
		delete[] mpcDrawBuffer;
	}
}

IWindow* IWindow::Create()
{
	return dynamic_cast<IWindow*>(new CWindowWinGDI());
}

void CWindowWinGDI::Destroy()
{
	delete dynamic_cast<CWindowWinGDI*>(this);
}

void CWindowWinGDI::GetHDC(void** pp)
{
	*pp = (void*)mhdc;
}

void CWindowWinGDI::GetDrawPointer(void** pp)
{
	*pp = (void*)mpcDrawBuffer;
}

void CWindowWinGDI::OnPaint()
{
	if (mpPane == NULL) {
		return;
	}

	POINT pnt;
	pnt.x = 0;
	pnt.y = 0;
	::ClientToScreen(mhWnd, &pnt);

	// Check the size of our window, and by that the size of our draw buffer. If it's too small, we reallocate
	RECT rct;
	::GetClientRect(mhWnd, &rct);
	int iBytesNeeded = rct.right * rct.bottom * 4;
	if (iBytesNeeded > miDrawBufferSize) {
		if (mpcDrawBuffer) {
			delete[] mpcDrawBuffer;
		}

		miDrawBufferSize = iBytesNeeded;
		mpcDrawBuffer = new tuchar[miDrawBufferSize];
	}

	PAINTSTRUCT ps;
	mhdc = BeginPaint(mhWnd, &ps);

/*	SRect rctUpdate(ps.rcPaint);
	rctUpdate -= SSize(1, 1);*/
	SRect rctUpdate;
	rctUpdate = SRect(ps.rcPaint);
	rctUpdate -= SSize(1, 1);

	if (ps.rcPaint.left == 0 && ps.rcPaint.right == 0 && ps.rcPaint.top == 0 && ps.rcPaint.bottom == 0) {
		EndPaint(mhWnd, &ps);
		mhdc = NULL;
		return;
	}

	try {
		GetPane()->OnDraw(rctUpdate);
		if (mpControlOnTop) {
			mpControlOnTop->OnDraw(rctUpdate);
		}
	}
	catch(...) {
		EndPaint(mhWnd, &ps);
		mhdc = NULL;
		throw;
	}

	// Blit buffer content to window
	LPBITMAPINFOHEADER pHeader;
	pHeader = (LPBITMAPINFOHEADER)&mBmpInfo;
	pHeader->biWidth = rct.right;
	pHeader->biHeight = 0 - rct.bottom;
	pHeader->biSizeImage = iBytesNeeded;

	tint32 iLine;
//	for (iLine = 0; iLine < rct.bottom; iLine++){
	// This looks pretty weird, but it's because the drawing is reversed in the y direction
	for (iLine = rct.bottom - 1 - (rctUpdate.iY + rctUpdate.iCY - 1); iLine < rct.bottom - rctUpdate.iY; iLine++){
		::SetDIBitsToDevice(mhdc,
			0, 0,
			rct.right, rct.bottom,
			0, 0,
			iLine, 1,
			mpcDrawBuffer + (rct.bottom - 1 - iLine) * rct.right * 4,
			&mBmpInfo,
			DIB_RGB_COLORS);
	}

/*	::SetDIBitsToDevice(mhdc,
		0, 0,
		rct.right, rct.bottom,
		0, 0,
		0, rct.bottom,
		mpcDrawBuffer,
		&mBmpInfo,
		DIB_RGB_COLORS);*/

	EndPaint(mhWnd, &ps);
	mhdc = NULL;
}

LRESULT CWindowWinGDI::WndProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam, tbool& rbProcessed)
{
	// Make sure invalidater is empty
//	mpInvalidater->Reset();

	LRESULT ret = CWindowWin::WndProc(hwnd, uiMsg, wParam, lParam, rbProcessed);

/*	if (mpInvalidater->IsRectInvalidated()) {
		SRect Rect;

		mpInvalidater->GetInvalidatedRect(Rect);
		mpInvalidater->Reset();

		RECT rcUpdate;
		rcUpdate.left = (RECT)Rect;
		::RedrawWindow(hwnd, &rcUpdate, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
	}*/

	return ret;
}



