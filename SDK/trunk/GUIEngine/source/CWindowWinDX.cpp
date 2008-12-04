/*!	\file CWindowWinDX.cpp
	\author Michael Olsen
	\date 30/Dec/2004
	\date 30/Dec/2004
*/


#include "geInternal.h"

static const GUID IID_IDirectDraw3 =
{0x618f8ad4, 0x8b7a, 0x11d0, 0x8f, 0xcc, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0x9d};

static const GUID CLSID_DirectDrawFactory =
{0x4fd2a832, 0x86c8, 0x11d0, 0x8f, 0xca, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0x9d};

static const GUID IID_IDirectDrawFactory =
{0x4fd2a833, 0x86c8, 0x11d0, 0x8f, 0xca, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0x9d};

static const GUID IID_IDirectDrawSurface3 =
{0xDA044E00,0x69B2,0x11D0,0xA1,0xD5,0x00,0xAA,0x00,0xB8,0xDF,0xBB};

CWindowWinDX::CWindowWinDX()
{
}

CWindowWinDX::~CWindowWinDX()
{
	// We have to destroy the bitmaps before we release the DirectX objects, so we need to destroy the pane here
	// Since mpPane is set to NULL, the destruction of the pane in CWindow will be ignored.
	if (GetPane()) {
		GetPane()->Destroy();
		mpPane = NULL;
	}

	mpDirectDraw->Release();
	mpDirectDrawFactory->Release();
	mpDirectDrawSurfaceB->Release();
	mpDirectDrawSurface->Release();
	CoUninitialize();
}

IWindow* IWindow::Create()
{
	return dynamic_cast<IWindow*>(new CWindowWinDX());
}

void CWindowWinDX::Destroy()
{
	delete dynamic_cast<CWindowWinDX*>(this);
}

void CWindowWinDX::MakeWindow(void* pParent, const SRect* pRect)
{
	CWindowWin::MakeWindow(pParent, pRect);

	DirectXInit();
}

void CWindowWinDX::DirectXInit()
{
	HRESULT hr;

	CoInitialize(NULL);

	const tint32 iCX = 800;
	const tint32 iCY = 480;

	hr = CoCreateInstance(CLSID_DirectDrawFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IDirectDrawFactory,
		(void **)&mpDirectDrawFactory);
	if (hr != S_OK) {
	}

	IDirectDraw* pDirectDraw;
	hr = mpDirectDrawFactory->CreateDirectDraw(NULL,
		GetHwndParent(),
		DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_NOWINDOWCHANGES,
		NULL,
		NULL,
		&pDirectDraw);
	if (hr != DD_OK) {
	}

	hr = pDirectDraw->QueryInterface(IID_IDirectDraw3, (void**)&mpDirectDraw);
	if (hr != S_OK) {
	}

	// Change screen resolution
	hr = pDirectDraw->SetDisplayMode(iCX, iCY, 32);
	mpDirectDraw->Release();

	// Create flipping surfaces
	DDSURFACEDESC surfaceDesc;
	memset(&surfaceDesc, 0, sizeof(DDSURFACEDESC));
	surfaceDesc.dwSize = sizeof(DDSURFACEDESC);
	surfaceDesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	surfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	surfaceDesc.dwBackBufferCount = 1;
	hr = mpDirectDraw->CreateSurface(&surfaceDesc,
		&mpDirectDrawSurface,
		NULL);
	if (hr != S_OK) {
	}

	// Get back surface
	DDSCAPS ddsCaps;
	ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
	hr = mpDirectDrawSurface->GetAttachedSurface(&ddsCaps, &mpDirectDrawSurfaceB);
	if (hr != S_OK) {
	}
}

void CWindowWinDX::GetHDC(void** pp)
{
	*pp = mpDirectDrawSurfaceB;
}

void CWindowWinDX::GetDrawPointer(void** pp)
{
	*pp = (void*)mpDirectDraw;
}

void CWindowWinDX::OnPaint()
{
	POINT pnt;
	pnt.x = 0;
	pnt.y = 0;
	::ClientToScreen(mhWnd, &pnt);

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(mhWnd, &ps);
	SRect rctUpdate(ps.rcPaint);
	rctUpdate -= SSize(1, 1);
	if (ps.rcPaint.left == 0 && ps.rcPaint.right == 0 && ps.rcPaint.top == 0 && ps.rcPaint.bottom == 0) {
		EndPaint(mhWnd, &ps);
		return;
	}

	try {
		GetPane()->OnDraw(rctUpdate);
	}
	catch(...) {
		EndPaint(mhWnd, &ps);
		throw;
	}

	mpDirectDrawSurface->Flip(NULL, DDFLIP_WAIT);

	EndPaint(mhWnd, &ps);
}

tbool CWindowWinDX::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	SPos Pos2 = Pos;
	Pos2.iY += 10;

	return CWindowWin::OnMouse(MouseMsg, Pos2);
}


