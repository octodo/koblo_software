/*!	\file CWindowOSXQT.cpp
	\author Michael Olsen
	\date 10/Mar/2005
	\date 10/Mar/2005
*/



CWindowOSXQT::CWindowOSXQT()
	: mpcDrawBuffer(NULL), miDrawBufferSize(0)
{
}

CWindowOSXQT::~CWindowOSXQT()
{
	if (mpcDrawBuffer) {
		delete[] mpcDrawBuffer;
	}
}

IWindow* IWindow::Create()
{
	return dynamic_cast<IWindow*>(new CWindowOSXQT());
}

void CWindowOSXQT::Destroy()
{
	delete dynamic_cast<CWindowOSXQT*>(this);
}

void CWindowOSXQT::GetHDC(void** pp)
{
	*pp = (void*)(GetWindowRef());
}

void CWindowOSXQT::GetDrawPointer(void** pp)
{
	*pp = (void*)mpcDrawBuffer;
}

void CWindowOSXQT::Paint()
{
	if (mpInvalidater->IsRectInvalidated()) {
		OnPaint();
	}
}

void CWindowOSXQT::OnPaint()
{
	int iBytesNeeded = (mSize.iCX + 1) * (mSize.iCY + 1) * 4;
	if (iBytesNeeded > miDrawBufferSize) {
		if (mpcDrawBuffer) {
			delete[] mpcDrawBuffer;
		}

		miDrawBufferSize = iBytesNeeded;
		mpcDrawBuffer = new tuchar[miDrawBufferSize];
	}

	if (mpInvalidater->IsRectInvalidated() == false) {
		// No updating neccasary
		return;
	}
	SRect RectUpdate;
	mpInvalidater->GetInvalidatedRect(RectUpdate);
	mpInvalidater->Reset();

	// Limit update rect to our actual size
	SRect RectThis(SPos(0, 0), mSize);
	RectUpdate.FitInside(RectThis);

	GetPane()->OnDraw(RectUpdate);
	if (mpControlOnTop) {
		mpControlOnTop->OnDraw(RectUpdate);
	}

	PixMap 			srcbm;
	GrafPtr 		saved;
	Rect 			portBounds;
	GrafPtr 		grafPtrThis;
	BitMapPtr 		dstbitmap;
	PixMapHandle	dstpixmap;

	memset(&srcbm,0,sizeof(PixMap));
	// setup begin
	srcbm.baseAddr = nil;
	srcbm.rowBytes = 0x8000;

	srcbm.bounds.left = 0;
	srcbm.bounds.top = 0;
	srcbm.bounds.right = mSize.iCX - 1;
	srcbm.bounds.bottom = mSize.iCY - 1;
	
	// ???
	int bytesPerRow = mSize.iCX * 4;

	srcbm.pmVersion = 4;
	srcbm.packType = 0;
	srcbm.packSize = 0;
	srcbm.hRes = 0x00480000;
	srcbm.vRes = 0x00480000;
	srcbm.pixelType = 0;
	srcbm.pixelSize = 0;
	srcbm.cmpCount = 0;
	srcbm.cmpSize = 0;
	srcbm.pmTable = nil;
	srcbm.pixelType = RGBDirect;
	srcbm.pixelSize = 32;
	srcbm.cmpCount = 3;
	srcbm.cmpSize = 5;
	srcbm.rowBytes = 0x8000 | (unsigned short) bytesPerRow;
	srcbm.baseAddr = (Ptr)mpcDrawBuffer;
	
	unsigned long ulOffsetX, ulOffsetY;
	ulOffsetX = 0;
	ulOffsetY = 0;
	
	Rect SrcRect;	
	SrcRect.left    = RectUpdate.iX;
	SrcRect.right   = RectUpdate.iX + RectUpdate.iCX - 1;
	SrcRect.top     = RectUpdate.iY;
	SrcRect.bottom  = RectUpdate.iY + RectUpdate.iCY - 1;

	Rect DstRect;
	DstRect = SrcRect;
		
	GetPort(&saved);
	
	grafPtrThis = (GrafPtr)::GetWindowPort(GetWindowRef());

	if (saved != grafPtrThis) {
		SetPort(grafPtrThis);
	}

	GetPortBounds(grafPtrThis, &portBounds);

	int port_offsetX = portBounds.left;
	int port_offsetY = portBounds.top;

	SetOrigin( port_offsetX, port_offsetY );
/*	Rect rctClip;
	rctClip.left = 0;
	rctClip.right = rctClip.left + mSize.iCX;
	rctClip.top = 0;
	rctClip.bottom = rctClip.top + mSize.iCY;
	ClipRect(&rctClip);*/
	
	dstpixmap = ::GetPortPixMap(grafPtrThis);
	
	dstbitmap = (BitMapPtr)(*dstpixmap);
	
	ForeColor(blackColor);
	BackColor(whiteColor);
	
	::CopyBits((BitMapPtr)&srcbm,
		dstbitmap,
		&SrcRect,
		&DstRect,
		srcCopy,
		nil);
		
//	SetOrigin(0, 0);

	if (saved != grafPtrThis) {
		SetPort(saved);
	}
}

void CWindowOSXQT::PaintToBuffer(tuchar* puchBuffer, tuint32 uiInterleave, const SRect& rUpdate)
{
	int iBytesNeeded = (mSize.iCX + 1) * (mSize.iCY + 1) * 4;
	if (iBytesNeeded > miDrawBufferSize) {
		if (mpcDrawBuffer) {
			delete[] mpcDrawBuffer;
		}

		miDrawBufferSize = iBytesNeeded;
		mpcDrawBuffer = new tuchar[miDrawBufferSize];
	}

	if (mpInvalidater->IsRectInvalidated() == true) {
		// We need to update the buffer
		SRect RectUpdate;
		mpInvalidater->GetInvalidatedRect(RectUpdate);
		mpInvalidater->Reset();

		// Limit update rect to our actual size
		SRect RectThis(SPos(0, 0), mSize);
		RectUpdate.FitInside(RectThis);
		// Limit update rect to, well, the update rect...
		RectUpdate.FitInside(rUpdate);

		GetPane()->OnDraw(RectUpdate);
		if (mpControlOnTop) {
			mpControlOnTop->OnDraw(RectUpdate);
		}
	}

	// Copy to buffer delivered
	tuint uiY;
	for (uiY = rUpdate.iY; uiY < mSize.iCY && uiY < (rUpdate.iY + rUpdate.iCY); uiY++) {
		tuchar* puchDest = puchBuffer + (uiInterleave * uiY);
		tuchar* puchSource = mpcDrawBuffer + (4 * mSize.iCX * uiY);
		tuint uiX;
		for (uiX = rUpdate.iX; uiX < mSize.iCX && uiX < (rUpdate.iX + rUpdate.iCX); uiX++) {
			((tuint32*)puchDest)[uiX] = ((tuint32*)puchSource)[uiX];
		}
	}
}


