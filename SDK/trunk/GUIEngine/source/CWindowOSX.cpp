/*!	\file CWindowOSX.cpp
	\author Michael Olsen
	\date 30/Dec/2004
	\date 30/Dec/2004
*/

//! Macro to make 16-bit int from 16 bits (used to make cursor)
#define BITS_TO_16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) \
(tint16)((a << 15) | (b << 14) | (c << 13) | (d << 12) | (e << 11) | (f << 10) | (g << 9) | (h << 8) | \
(i << 7) | (j << 6) | (k << 5) | (l << 4) | (m << 3) | (n << 2) | (o << 1) | (p))

//! Macro to make Bits16 from 16 16-bit ints (used to make cursor)
#define INT16_16_TO_BITS16(bits, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) \
bits[0] = (a); \
bits[1] = (b); \
bits[2] = (c); \
bits[3] = (d); \
bits[4] = (e); \
bits[5] = (f); \
bits[6] = (g); \
bits[7] = (h); \
bits[8] = (i); \
bits[9] = (j); \
bits[10] = (k); \
bits[11] = (l); \
bits[12] = (m); \
bits[13] = (n); \
bits[14] = (o); \
bits[15] = (p)

//! We skip giTimerSkip - 1 timer/idle calls before actually doing it
const int giTimerSkip = 8;

CWindowOSX::CWindowOSX() : mWindowRef(NULL), mSize(0, 0), mMouseRef(NULL), mKeyboardRef(NULL), mWindowEventRef(NULL), mCursorCur(CursorNormal),
	miTimerCounter(giTimerSkip)
	, mWindowGroup(0), mWindowRefParent(0), mPosLastClick(0, 0),
	mbBufferDrawing(false)
{
	muiTimeClickLast = ITime::GetTimeMS();

	mhCursorWatch = ::GetCursor(::watchCursor);

	INT16_16_TO_BITS16(mCursorWE.data, BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0), BITS_TO_16(0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0), BITS_TO_16(0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0), BITS_TO_16(0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0), BITS_TO_16(0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0), BITS_TO_16(0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0), BITS_TO_16(0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0));
	INT16_16_TO_BITS16(mCursorWE.mask, BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0), BITS_TO_16(0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0), BITS_TO_16(0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0), BITS_TO_16(0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0), BITS_TO_16(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1), BITS_TO_16(0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0), BITS_TO_16(0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0), BITS_TO_16(0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0), BITS_TO_16(0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0));
	mCursorWE.hotSpot.h = 8;
	mCursorWE.hotSpot.v = 7;

	INT16_16_TO_BITS16(mCursorNS.data, BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0));
	INT16_16_TO_BITS16(mCursorNS.mask, BITS_TO_16(0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0), BITS_TO_16(0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0), BITS_TO_16(0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0), BITS_TO_16(0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0), BITS_TO_16(0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0));
	mCursorNS.hotSpot.h = 7;
	mCursorNS.hotSpot.v = 8;

	INT16_16_TO_BITS16(mCursorHand.data, BITS_TO_16(0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,1,0,0,1,1,1,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,0), BITS_TO_16(0,1,1,0,1,0,0,1,0,0,1,0,1,1,0,0), BITS_TO_16(1,0,0,1,1,0,0,0,0,0,0,0,1,0,1,0), BITS_TO_16(1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0), BITS_TO_16(0,1,0,0,1,0,0,0,0,0,0,0,0,0,1,0), BITS_TO_16(0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0), BITS_TO_16(0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0), BITS_TO_16(0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0), BITS_TO_16(0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0), BITS_TO_16(0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0), BITS_TO_16(0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0));
	INT16_16_TO_BITS16(mCursorHand.mask, BITS_TO_16(0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0), BITS_TO_16(0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0), BITS_TO_16(0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,0), BITS_TO_16(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0), BITS_TO_16(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0), BITS_TO_16(0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0), BITS_TO_16(0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0), BITS_TO_16(0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0), BITS_TO_16(0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0), BITS_TO_16(0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0), BITS_TO_16(0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0), BITS_TO_16(0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0));
	mCursorHand.hotSpot.h = 6;
	mCursorHand.hotSpot.v = 0;
}

CWindowOSX::~CWindowOSX()
{
	if (mWindowEventRef != NULL) {
		::RemoveEventHandler(mWindowEventRef);
	}
	if (mMouseRef != NULL) {
		::RemoveEventHandler(mMouseRef);
	}

//	if (mWindowRef) {
//		::DisposeWindow(mWindowRef);
//	}

	if (mWindowGroup) {
		if (mWindowRefParent) {
			::SetWindowGroup(mWindowRefParent, GetWindowGroupParent(mWindowGroup));
		}
		::ReleaseWindowGroup(mWindowGroup);
	}
}

void CWindowOSX::GetMouseFocus(IControl* pControl)
{
	CWindow::GetMouseFocus(pControl);
}

void CWindowOSX::ReleaseMouseFocus()
{
	CWindow::ReleaseMouseFocus();
}

void CWindowOSX::HideMouse()
{
	// Since we cannot move the cursor, it's better not to hide it
//	::HideCursor();
}

void CWindowOSX::ShowMouse()
{
	// See notes for HideMouse()
//	::ShowCursor();
}

void CWindowOSX::SetMousePos(const SPos& /*Pos*/)
{
}

void CWindowOSX::SetCurrentMouseCursor()
{
	switch(mCursorCur) {
		case CursorNormal:
			::InitCursor();
			break;

		case CursorWait:
			::SetCursor(*mhCursorWatch);
			break;

		case CursorHand:
			::SetCursor(&mCursorHand);
			break;

		case CursorArrowWE:
			::SetCursor(&mCursorWE);
			break;

		case CursorArrowNS:
			::SetCursor(&mCursorNS);
			break;
	}
}

void CWindowOSX::DoSetMouseCursor(EMouseCursor Cursor)
{
	mCursorCur = Cursor;
}

WindowRef CWindowOSX::GetWindowRef() const
{
	return mWindowRef;
}

void* CWindowOSX::GetParent() const
{
	return (void*)GetWindowRef();
}

OSStatus MouseHandlerProc(EventHandlerCallRef inHandlerCallRef, EventRef inEvent, void* inUserData);
OSStatus WindowHandlerProc(EventHandlerCallRef inHandlerCallRef, EventRef inEvent, void* inUserData);

OSErr DragAndDropProc(WindowRef theWindow, void * handlerRefCon, DragRef theDrag);

void CWindowOSX::MakeWindow(void* pParent, const SRect* pRect)
{
	mSize = (SSize)(*pRect);

	mWindowRef = (WindowRef)pParent;
/*	{
		int	port_offsetX;
		int port_offsetY;
		{
			GrafPtr saved;
			Rect portBounds;

			GetPort(&saved);
	
			GrafPtr grafPtrThis = (GrafPtr)::GetWindowPort(mWindowRef);

			if (saved != grafPtrThis) {
				SetPort(grafPtrThis);
			}

			GetPortBounds(grafPtrThis, &portBounds);

			port_offsetX = portBounds.left;
			port_offsetY = portBounds.top;
//			printf("x: %d, y:%d\n", port_offsetX, port_offsetY);

			if (saved != grafPtrThis) {
				SetPort(saved);
			}
		}

		Rect bnds;
		bnds.top = 0;
		bnds.left = 0;
		bnds.right = mSize.iCX;
		bnds.bottom = mSize.iCY;

		WindowRef WndRefSub;
		int res = ::CreateNewWindow(kSheetWindowClass, kWindowNoShadowAttribute | kWindowStandardHandlerAttribute, &bnds, &WndRefSub);

		Rect r;
		::GetWindowBounds(mWindowRef, kWindowContentRgn, &r);
//		printf("r.left: %d, r.top: %d\n", r.left, r.top);
		r.left -= port_offsetX;
		r.top -= port_offsetY;
//		printf("r.left: %d, r.top: %d\n", r.left, r.top);
		r.bottom = r.top + mSize.iCY;
		r.right = r.left + mSize.iCX;
		::SetWindowBounds(WndRefSub, kWindowContentRgn, &r);

		::ShowWindow(WndRefSub);

		::BringToFront(WndRefSub);
		WindowGroupAttributes windowGroupAttributes = 0;
		::CreateWindowGroup(windowGroupAttributes, &mWindowGroup);
		::SetWindowGroupParent(mWindowGroup, GetWindowGroup(mWindowRef));

		::SetWindowGroup(WndRefSub, mWindowGroup);
		::SetWindowGroup(mWindowRef, mWindowGroup);
		::SendBehind(mWindowRef, WndRefSub);

		::ChangeWindowGroupAttributes(mWindowGroup, kWindowGroupAttrMoveTogether | kWindowGroupAttrLayerTogether | kWindowGroupAttrHideOnCollapse | kWindowGroupAttrSharedActivation, NULL); 

		::SendBehind(mWindowRef, WndRefSub);
		::ShowWindow(WndRefSub);

		mWindowRefParent = mWindowRef;
		mWindowRef = WndRefSub;
	}*/

	// Acquire mouse hook
	OSStatus err;

	EventTargetRef pEventTarget;
	pEventTarget = ::GetWindowEventTarget(GetWindowRef());

	EventTypeSpec pSpec[4];
	pSpec[0].eventClass = kEventClassMouse;
	pSpec[0].eventKind = kEventMouseDown;
	pSpec[1].eventClass = kEventClassMouse;
	pSpec[1].eventKind = kEventMouseUp;
	pSpec[2].eventClass = kEventClassMouse;
	pSpec[2].eventKind = kEventMouseMoved;
	pSpec[3].eventClass = kEventClassMouse;
	pSpec[3].eventKind = kEventMouseDragged;

	err = ::InstallEventHandler(pEventTarget,
		MouseHandlerProc,
		4,
		pSpec,
		(void*)dynamic_cast<CWindowOSX*>(this),
		&mMouseRef);
		
	if (err != noErr) {
		mMouseRef = NULL;
	}
	
	// Acquire window hook
	EventTypeSpec pSpec2[4];
	pSpec2[0].eventClass = kEventClassWindow;
	pSpec2[0].eventKind = kEventWindowCursorChange;
	pSpec2[1].eventClass = kEventClassWindow;
	pSpec2[1].eventKind = kEventWindowDrawContent;
	err = ::InstallEventHandler(pEventTarget,
		WindowHandlerProc,
		2,
		pSpec2,
		(void*)dynamic_cast<CWindowOSX*>(this),
		&mWindowEventRef);
	if (err != noErr) {
		mWindowEventRef = NULL;
	}

/*	EventTypeSpec pSpec3[4];
	pSpec3[0].eventClass = kEventClassWindow;
	pSpec3[0].eventKind = kEventWindowHidden;
	pSpec3[1].eventClass = kEventClassWindow;
	pSpec3[1].eventKind = kEventWindowShown;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRefParent),
		WindowHandlerProc,
		2,
		pSpec3,
		(void*)dynamic_cast<CWindowOSX*>(this),
		&mWindowEventRef);*/

	// Drag 'n drop
//	::InstallReceiveHandler(DragAndDropProc, mWindowRef, (void*)this);
}

void CWindowOSX::PrepareBuffer(const SRect* pRect)
{
	mSize = (SSize)(*pRect);

	mbBufferDrawing = true;
}

void CWindowOSX::Shown(tbool bShown)
{
	if (bShown) {
		::ShowWindow(mWindowRef);
	}
	else {
		::HideWindow(mWindowRef);
	}
}

void CWindowOSX::ShowWindow()
{
}

void CWindowOSX::GetSize(SSize& rSize)
{
	rSize = mSize;
}

void CWindowOSX::SetSize(const SSize& rSize)
{
	mSize = rSize;
}


IWindow::EMsgBoxReturn IWindow::ShowMessageBox(const tchar* pszMessage, const tchar* pszCaption, IWindow::EMsgBoxButtons Buttons)
{
	CFStringRef strCaption = CFSTR("Error");
	if (pszCaption != NULL) {
		strCaption = CFStringCreateWithCStringNoCopy(NULL, pszCaption, kCFStringEncodingASCII, kCFAllocatorNull); 
	}
	CFStringRef strError = CFSTR("");
	tint iErrorLen = (pszMessage == NULL) ? 0 : strlen(pszMessage);
	if (iErrorLen > 0) {
		strError = CFStringCreateWithCStringNoCopy(NULL, pszMessage, kCFStringEncodingASCII, kCFAllocatorNull);
	}

	AlertStdCFStringAlertParamRec ParamRec;
	ParamRec.version = kStdCFStringAlertVersionOne;
	ParamRec.movable = 1;
	ParamRec.helpButton = 0;
	//ParamRec.filterProc = nil;
	ParamRec.defaultButton = kAlertStdAlertOKButton;
	ParamRec.cancelButton = kAlertStdAlertCancelButton;
	ParamRec.position = kWindowAlertPositionParentWindowScreen;
	ParamRec.flags = 0;
 
	switch(Buttons) {
		case MsgBoxOK:
			ParamRec.defaultText = CFSTR("OK");
			ParamRec.cancelText = nil;
			ParamRec.otherText = nil;
			break;

		case MsgBoxOKCancel:
			ParamRec.defaultText = CFSTR("OK");
			ParamRec.cancelText = CFSTR("Cancel");
			ParamRec.otherText = nil;
			break;

		case MsgBoxYesNo:
			ParamRec.defaultText = CFSTR("Yes");
			ParamRec.cancelText = CFSTR("No");
			ParamRec.otherText = nil;
			break;

		case MsgBoxYesNoCancel:
			ParamRec.defaultText = CFSTR("Yes");
			ParamRec.cancelText = CFSTR("No");
			ParamRec.otherText = CFSTR("Cancel");
			ParamRec.cancelButton = kAlertStdAlertOtherButton;
			break;

		case MsgBoxRetryCancel:
			ParamRec.defaultText = CFSTR("Retry");
			ParamRec.cancelText = CFSTR("Cancel");
			ParamRec.otherText = nil;
			break;
	}
	
	DialogRef DlgRef;

	OSStatus osStat = ::CreateStandardAlert(kAlertPlainAlert, strCaption, strError, &ParamRec, &DlgRef);
	if (osStat == 0) {
		DialogItemIndex iItemHit;
		osStat = ::RunStandardAlert(DlgRef, NULL, &iItemHit);
		if (osStat == 0) {
			switch(iItemHit) {
				case kAlertStdAlertOKButton:
					switch(Buttons) {
						case MsgBoxOK:
						case MsgBoxOKCancel:
							return MsgBoxRetOK;

						case MsgBoxYesNo:
						case MsgBoxYesNoCancel:
							return MsgBoxRetYes;

						default:
							return MsgBoxRetRetry;
					}

				case kAlertStdAlertCancelButton:
					switch(Buttons) {
						case MsgBoxYesNoCancel:
							return MsgBoxRetNo;

						default:
							return MsgBoxRetCancel;
					}

				case kAlertStdAlertOtherButton:
					return MsgBoxRetCancel;
			}
		}
	}
		
	return MsgBoxRetError;
} // ShowMessageBox


void CWindowOSX::OnMouseHook(EMouseMsg Msg, const SPos& Pos)
{
	// Update current mouse position
	mMousePosCur = Pos;

	tbool bDoubleClick = false;
	if (Msg == LeftButtonDown) {
		// (lasse) Crash-fix 2008-12-01
		if (miMouseStateCur & MouseStateLeftDown) {
			// Chaos! We have two left button clicks!
			// We MUST release any previous mouse focus in order not to crash
			// Note: This is quite hard-core and may lead to unexpected behavior... fix later
			ReleaseMouseFocus();
		}
		// .. (lasse)
		
		// Update mouse state
		miMouseStateCur |= MouseStateLeftDown;

		// Determine if this is a double-click
		tuint32 uiTimeThis = ITime::GetTimeMS();
		// 500 ms
		if (uiTimeThis < muiTimeClickLast + 500) {
			if (Pos.iX > mPosLastClick.iX - 5 &&
				Pos.iX < mPosLastClick.iX + 5 &&
				Pos.iY > mPosLastClick.iY - 5 &&
				Pos.iY < mPosLastClick.iY + 5) {

				bDoubleClick = true;
			}
		}
		muiTimeClickLast = uiTimeThis;

		mPosLastClick = Pos;
	}
	
	if (bDoubleClick) {
//		OnMouse(Msg, mMousePosCur);
		OnMouse(LeftButtonDblClk, mMousePosCur);
	}
	else {
		OnMouse(Msg, mMousePosCur);
	}

	if (Msg == LeftButtonUp) {
		// Update mouse state
		miMouseStateCur &= (~MouseStateLeftDown);
	}
}

tbool CWindowOSX::IsMousePressed()
{
	tbool bRet = false;

	tint32 iMouseState = GetMouseState();
	if (iMouseState & MouseStateLeftDown) {
		bRet = true;
	}
	if (iMouseState & MouseStateRightDown) {
		bRet = true;
	}

	return bRet;
}

void CWindowOSX::MouseDown(const SPos& Pos)
{
	return;
	// Determine if this is a double-click
	tbool bDoubleClick = false;
	tuint32 uiTimeThis = ITime::GetTimeMS();
	// 1000 ms
	if (uiTimeThis < muiTimeClickLast + 200) {
		bDoubleClick = true;
	}
	muiTimeClickLast = uiTimeThis;

	// Update mouse state
	miMouseStateCur |= MouseStateLeftDown;

	// On OSX we only receive a mouse down message, and we're supposed to enter a loop until the mouse button is released.
	SPos PosCur = Pos;
	if (bDoubleClick) {
		OnMouse(LeftButtonDown, PosCur);
		OnMouse(LeftButtonDblClk, PosCur);
	}
	else {
		OnMouse(LeftButtonDown, PosCur);
	}

	// Redraw if neccasary
	Paint();

	while (::Button()) {
		// As long as the button is pressed

		// Wait for the next event
		EventRecord Dummy;
	    WaitNextEvent(0, &Dummy, 0, nil);

/*		// Get the mouse point and call with MouseMove
		Point Pnt;
		::GetMouse(&Pnt);
		PosCur = SPos(Pnt.h, Pnt.v);
		OnMouse(MouseMove, PosCur);*/
		
		// Call with MouseMove and simulate "idle"
		OnTimer();

		// Redraw if neccasary
		Paint();
	}

	// Update mouse state
	miMouseStateCur &= (~MouseStateLeftDown);

	// Call button up with last position
	OnMouse(LeftButtonUp, SPos(PosCur));

	// Call with MouseMove and simulate "idle"
	OnTimer();

	// Redraw if neccasary
	Paint();
}

void CWindowOSX::OnTimer()
{
	// Get mouse position
/*	Point Pnt;
	::GetMouse(&Pnt);

	// Convert to window points
	CGrafPtr port;
	GDHandle gdh;
	GetGWorld(&port, &gdh);
	SetGWorld(GetWindowPort(GetWindowRef()), GetGDevice());
	GlobalToLocal(&Pnt);

	// Get size of port
//	Rect rctPort;
//	::GetPortBounds(GetWindowPort(GetWindowRef()), &rctPort);
//	Pnt.h += rctPort.left;
//	Pnt.v += rctPort.top;

	SetGWorld(port, gdh);

	SSize SizeWindow;
	GetSize(SizeWindow);

	// We need to reset cursor if
	// A: Mouse is outside our window
	// and
	// B: Mouse buttons are not down
	// and
	// C: Mouse was previously inside our window
//	tbool bMouseInsideWindow = true;
//	if (Pnt.h < 0) {
//		bMouseInsideWindow = false;
//	}
//	else if (Pnt.v < 0) {
//		bMouseInsideWindow = false;
//	}
//	else if (Pnt.h > SizeWindow.iCX) {
//		bMouseInsideWindow = false;
//	}
//	else if (Pnt.v > SizeWindow.iCY) {
//		bMouseInsideWindow = false;
//	}
//	if (mbMouseInsideWindowWhenTimerCalled) {
		// Mouse was previously inside our window (C above)
//		if (bMouseInsideWindow == false) {
			// Mouse is current outside our window (A above)
//			if ((miMouseStateCur & MouseStateLeftDown) == 0) {
//				if ((miMouseStateCur & MouseStateRightDown) == 0) {
					// Mouse buttons are not down (B above)
//					::InitCursor();
//				}
//			}
//		}
//	}
	// Update for next timer
//	mbMouseInsideWindowWhenTimerCalled = bMouseInsideWindow;

	// Only do mouse if inside window. Note, dragging (for instance move a knob even if pointer is outside window) is handled by dragging event, and thus doesn't need to be handled in idle
	tbool bDoMouse = true;
	if (Pnt.h < 0) {
		bDoMouse = false;
	}
	if (Pnt.v < 0) {
		bDoMouse = false;
	}
	if (Pnt.h > SizeWindow.iCX) {
		bDoMouse = false;
	}
	if (Pnt.v > SizeWindow.iCY) {
		bDoMouse = false;
	}
	// Only do mouse if button current is not pressed.
	if (miMouseStateCur & MouseStateLeftDown) {
		bDoMouse = false;
	}
	if (miMouseStateCur & MouseStateRightDown) {
		bDoMouse = false;
	}

	SPos PosNew = SPos(Pnt.h, Pnt.v);
	if (bDoMouse && (PosNew != mMousePosCur)) {
		mMousePosCur = PosNew;

		OnMouse(MouseMove, mMousePosCur);
	}
	else {
		// Make sure mouse position is updated in any case
		mMousePosCur = PosNew;
	}*/

	if (miTimerCounter-- == 0) {
		CWindow::OnTimer();

		miTimerCounter = giTimerSkip;
	}
}

void CWindowOSX::KeyDown(tint32 iKey)
{
	EKey Key = (EKey)-1;

	if ((iKey >= 'a') && (iKey <= 'z')) {
		Key = (EKey)(Keya + (iKey - 'a'));
	}
	else if ((iKey >= 'A') && (iKey <= 'Z')) {
		Key = (EKey)(KeyA + (iKey - 'A'));
	}
	else if ((iKey >= '0') && (iKey <= '9')) {
		Key = (EKey)(Key0 + (iKey - '0'));
	}
	else if (iKey == ' ') {
		Key = KeySpace;
	}
	else if (iKey == '.') {
		Key = KeyDot;
	}
	else if (iKey == ',') {
		Key = KeyComma;
	}
	else if (iKey == '-') {
		Key = KeyMinus;
	}
	else if (iKey == '+') {
		Key = KeyPlus;
	}
	else if (iKey == 46) {
		Key = KeyDelete;
	}
	else if (iKey == 8) {
		Key = KeyBackSpace;
	}
	else if (iKey == 37) {
		Key = KeyLeft;
	}
	else if (iKey == 39) {
		Key = KeyRight;
	}
	else if (iKey == 38) {
		Key = KeyUp;
	}
	else if (iKey == 40) {
		Key = ge::KeyDown;
	}
	else if (iKey == 13) {
		Key = KeyReturn;
	}
	else if (iKey == 27) {
		Key = KeyEscape;
	}

	if (Key != (EKey)-1) {
		OnKeyDown(Key);
	}
}

tbool IWindow::ControlPressed()
{
	KeyMap Map;
	GetKeys(Map);
	tchar* Map2;
	Map2 = (tchar*)Map;

	if ((Map2[7] >> 3) & 1) {
		return true;
	}
	else {
		return false;
	}
}

tbool IWindow::ShiftPressed()
{
	KeyMap Map;
	GetKeys(Map);
	tchar* Map2;
	Map2 = (tchar*)Map;

	if (Map2[7] & 1) {
		return true;
	}
	else {
		return false;
	}
}

tbool IWindow::AltPressed()
{
	throw (IException::Create(IException::TypeCode, IException::ReasonCodePathInvalid, EXCEPTION_INFO, (tchar*)"IWindow::AltPressed() not implemented on OSX"));
}

tbool IWindow::OSPressed()
{
	throw (IException::Create(IException::TypeCode, IException::ReasonCodePathInvalid, EXCEPTION_INFO, (tchar*)"IWindow::OSPressed() not implemented on OSX"));
}

void CWindowOSX::InstallKeyboardHook()
{
}

OSStatus KeyboardHandlerProc(EventHandlerCallRef inHandlerCallRef, EventRef inEvent, void* inUserData);

void CWindowOSX::AcquireKeyboardInput()
{
	// Lasse - added 2007-05
	if (mKeyboardRef) {
		// Already hooked - do nothing
		return;
	}
	// .. Lasse

	OSStatus err;

	EventTargetRef pEventTarget;
	pEventTarget = ::GetWindowEventTarget(GetWindowRef());

	EventTypeSpec pSpec[1];
	pSpec[0].eventClass = kEventClassKeyboard;
	pSpec[0].eventKind = kEventRawKeyDown;

	err = ::InstallEventHandler(pEventTarget,
		KeyboardHandlerProc,
		1,
		pSpec,
		(void*)dynamic_cast<CWindowOSX*>(this),
		&mKeyboardRef);

/*	err = ::InstallApplicationEventHandler(KeyboardHandlerProc,
		1,
		pSpec,
		(void*)dynamic_cast<CWindowOSX*>(this),
		&mKeyboardRef);*/
		
	if (err != noErr) {
		mKeyboardRef = NULL;
	}
}

void CWindowOSX::ReleaseKeyboardInput()
{
//	::RemoveEventHandler(mKeyboardRef);
	
//	mKeyboardRef = NULL;
}


// Lasse 2007-05-09: Moved here (used to be part of KeyboardHandlerProc(..) )
EKey XlatCharToEKey(tint32 iKey)
{
	EKey Key = (EKey)-1;

	switch(iKey) {
	
		case 'a': Key = Keya; break;
		case 'b': Key = Keyb; break;
		case 'c': Key = Keyc; break;
		case 'd': Key = Keyd; break;
		case 'e': Key = Keye; break;
		case 'f': Key = Keyf; break;
		case 'g': Key = Keyg; break;
		case 'h': Key = Keyh; break;
		case 'i': Key = Keyi; break;
		case 'j': Key = Keyj; break;
		case 'k': Key = Keyk; break;
		case 'l': Key = Keyl; break;
		case 'm': Key = Keym; break;
		case 'n': Key = Keyn; break;
		case 'o': Key = Keyo; break;
		case 'p': Key = Keyp; break;
		case 'q': Key = Keyq; break;
		case 'r': Key = Keyr; break;
		case 's': Key = Keys; break;
		case 't': Key = Keyt; break;
		case 'u': Key = Keyu; break;
		case 'v': Key = Keyv; break;
		case 'w': Key = Keyw; break;
		case 'x': Key = Keyx; break;
		case 'y': Key = Keyy; break;
		case 'z': Key = Keyz; break;
		case 'A': Key = KeyA; break;
		case 'B': Key = KeyB; break;
		case 'C': Key = KeyC; break;
		case 'D': Key = KeyD; break;
		case 'E': Key = KeyE; break;
		case 'F': Key = KeyF; break;
		case 'G': Key = KeyG; break;
		case 'H': Key = KeyH; break;
		case 'I': Key = KeyI; break;
		case 'J': Key = KeyJ; break;
		case 'K': Key = KeyK; break;
		case 'L': Key = KeyL; break;
		case 'M': Key = KeyM; break;
		case 'N': Key = KeyN; break;
		case 'O': Key = KeyO; break;
		case 'P': Key = KeyP; break;
		case 'Q': Key = KeyQ; break;
		case 'R': Key = KeyR; break;
		case 'S': Key = KeyS; break;
		case 'T': Key = KeyT; break;
		case 'U': Key = KeyU; break;
		case 'V': Key = KeyV; break;
		case 'W': Key = KeyW; break;
		case 'X': Key = KeyX; break;
		case 'Y': Key = KeyY; break;
		case 'Z': Key = KeyZ; break;
		case ' ': Key = KeySpace; break;
		case '+':	Key = KeyPlus; break;
		case '-':	Key = KeyMinus; break;
		case ',':	Key = KeyComma; break;
		case '.':	Key = KeyDot; break;
		case 127:	Key = KeyDelete; break;
		case 8:		Key = KeyBackSpace; break;
		case 27:	Key = KeyEscape; break;
		case 28:	Key = KeyLeft; break;
		case 29:	Key = KeyRight; break;
		case 30:	Key = KeyUp; break;
		case 31:	Key = KeyDown; break;
		case 13:
		case 0x03:	Key = KeyReturn; break;
		case '!': Key = KeyExclamation; break;
		case '"': Key = KeyQuotation; break;
		case '#': Key = KeyPound; break;
		case '$': Key = KeyDollar; break;
		case '%': Key = KeyPercent; break;
		case '&': Key = KeyAnd; break;
		case '@': Key = KeyAdd; break;
		case '(': Key = KeyBracketLeft; break;
		case ')': Key = KeyBracketRight; break;
		case '*': Key = KeyMul; break;
		case '/': Key = KeySlash; break;
		case ':': Key = KeyColon; break;
		case ';': Key = KeySemiColon; break;
		case '>': Key = KeyLarger; break;
		case '=': Key = KeyEqual; break;
		case '<': Key = KeySmaller; break;
		case '?': Key = KeyQuestion; break;
		case '[': Key = KeySquaredBrackedLeft; break;
		case '\\': Key = KeyBackSlash; break;
		case ']': Key = KeySquaredBrackedRight; break;
		case '^': Key = KeyHat; break;
		case '_': Key = KeyUnderLine; break;
		case '\'': Key = KeyNote; break;
		case '`': Key = KeyNote2; break;
		case '{': Key = KeyCurlyBrackedLeft; break;
		case '|': Key = KeyLine; break;
		case '}': Key = KeyCurlyBrackedRight; break;
		case '~': Key = KeyTilde; break;
		case '0': Key = Key0; break;
		case '1': Key = Key1; break;
		case '2': Key = Key2; break;
		case '3': Key = Key3; break;
		case '4': Key = Key4; break;
		case '5': Key = Key5; break;
		case '6': Key = Key6; break;
		case '7': Key = Key7; break;
		case '8': Key = Key8; break;
		case '9': Key = Key9; break;
		
	}

	return Key;
} // XlatCharToEKey
// .. Lasse


OSStatus KeyboardHandlerProc(EventHandlerCallRef inHandlerCallRef, EventRef inEvent, void* inUserData)
{
	tbool bDone = false;

	OSStatus err;
//	UInt32 iKey;
//	err = ::GetEventParameter(inEvent,
//		kEventParamKeyCode,
//		typeUInt32,
//		nil,
//		sizeof(iKey),
//		nil,
//		&iKey);
	unsigned char uch;
	err = ::GetEventParameter(inEvent,
		kEventParamKeyMacCharCodes,
		typeChar,
		nil,
		sizeof(uch),
		nil,
		&uch);
	tint32 iKey = (tint32)uch;

//	GetEventParameter( inEventRef, kEventParamKeyModifiers, typeUInt32, nil,
//		sizeof( modifiers ), nil, &modifiers );

	if (err == noErr) {
		EKey Key = XlatCharToEKey(iKey);

		if (Key == Keyv) {
			// Check for apple-v, i.e. paste
			KeyMap Map;
			GetKeys(Map);
			tchar* Map2;
			Map2 = (tchar*)Map;

			if (((unsigned char*)Map2)[6] >> 7) {
				// Paste
				OSStatus err;
				PasteboardRef Clipboard;
				err = ::PasteboardCreate(kPasteboardClipboard, &Clipboard);

				PasteboardSyncFlags SyncFlags;
				ItemCount Count;

				SyncFlags = ::PasteboardSynchronize(Clipboard);

				err = ::PasteboardGetItemCount(Clipboard, &Count);

				for (UInt32 ItemIndex = 1; ItemIndex < Count + 1; ItemIndex++) {
					PasteboardItemID ItemID;
					CFArrayRef FlavorTypeArray;
					CFIndex FlavorCount;

					err = ::PasteboardGetItemIdentifier(Clipboard, ItemIndex, &ItemID);

					err = ::PasteboardCopyItemFlavors(Clipboard, ItemID, &FlavorTypeArray);

					FlavorCount = CFArrayGetCount(FlavorTypeArray);

					for (CFIndex FlavorIndex = 0; FlavorIndex < FlavorCount; FlavorIndex++) {
						CFStringRef FlavorType;
						CFDataRef FlavorData;
						CFIndex FlavorDataSize;

						FlavorType = (CFStringRef)::CFArrayGetValueAtIndex(FlavorTypeArray, FlavorIndex);

						if (::UTTypeConformsTo(FlavorType, CFSTR("public.utf8-plain-text"))) {
							err = ::PasteboardCopyItemFlavorData(Clipboard, ItemID, FlavorType, &FlavorData);

							FlavorDataSize = ::CFDataGetLength(FlavorData);

							FlavorDataSize = (FlavorDataSize < 254) ? FlavorDataSize : 254;

							for (short DataIndex = 0; DataIndex < FlavorDataSize; DataIndex++) {
								char iKey = *(CFDataGetBytePtr(FlavorData) + DataIndex);

								// Lasse - rewritten 2007-05-09
								/*
								EKey Key = (EKey)-1;
								if ((iKey >= 'a') && (iKey <= 'z')) {
									Key = (EKey)(Keya + (iKey - 'a'));
								}
								else if ((iKey >= 'A') && (iKey <= 'Z')) {
									Key = (EKey)(KeyA + (iKey - 'A'));
								}
								else if ((iKey >= '0') && (iKey <= '9')) {
									Key = (EKey)(Key0 + (iKey - '0'));
								}
								else if (iKey == '-') {
									Key = KeyMinus;
								}

								if (Key != (EKey)-1) {
									CWindowOSX* pWindow = (CWindowOSX*)inUserData;
									pWindow->OnKeyDown(Key);
								}
								*/
								// We need to allow for space and all other chars.
								// To avoid re-inventing the soup-plate we simply throw 
								// the chars into the already existing handler:
								EKey Key = XlatCharToEKey(iKey);
								if (Key != (EKey)-1) {
									CWindowOSX* pWindow = (CWindowOSX*)inUserData;
									pWindow->OnKeyDown(Key);
								}
								// .. Lasse
							}

							::CFRelease(FlavorData);
						}
					}
				}

				::CFRelease(Clipboard);

				bDone = true;
			}
		}

		if (bDone == false) {
			if (Key != (EKey)-1) {
				CWindowOSX* pWindow = (CWindowOSX*)inUserData;
				pWindow->OnKeyDown(Key);
				bDone = true;
			}
		}
	}

	return (bDone ? noErr : eventNotHandledErr);
}

OSStatus MouseHandlerProc(EventHandlerCallRef inHandlerCallRef, EventRef inEvent, void* inUserData)
{
	// Get point
	Point pt;
	::GetEventParameter(inEvent, kEventParamMouseLocation, typeQDPoint, 0, sizeof(pt), NULL, &pt);

	// Convert to window points
	CGrafPtr port;
	GDHandle gdh;
	GetGWorld(&port, &gdh);
	CWindowOSX* pWin = ((CWindowOSX*)inUserData);
	SetGWorld(GetWindowPort(pWin->GetWindowRef()), GetGDevice());
	GlobalToLocal(&pt);

	// Get size of port
/*	Rect rctPort;
	::GetPortBounds(GetWindowPort(pWin->GetWindowRef()), &rctPort);
	pt.h += rctPort.left;
	pt.v += rctPort.top;*/

	SetGWorld(port, gdh);

	SSize SizeWindow;
	pWin->GetSize(SizeWindow);

	if (pt.h < 0) {
		if (((CWindowOSX*)inUserData)->IsMousePressed() == false) {
			return eventNotHandledErr;
		}
	}
	if (pt.v < 0) {
		if (((CWindowOSX*)inUserData)->IsMousePressed() == false) {
			return eventNotHandledErr;
		}
	}
	if (pt.h > SizeWindow.iCX) {
		if (((CWindowOSX*)inUserData)->IsMousePressed() == false) {
			return eventNotHandledErr;
		}
	}
	if (pt.v > SizeWindow.iCY) {
		if (((CWindowOSX*)inUserData)->IsMousePressed() == false) {
			return eventNotHandledErr;
		}
	}

	switch(GetEventKind(inEvent)) {
		case kEventMouseDown:
		{
			SPos Pos(pt.h, pt.v);
			((CWindowOSX*)inUserData)->OnMouseHook(LeftButtonDown, Pos);
		}
		CallNextEventHandler(inHandlerCallRef, inEvent);
		return noErr;
//		return eventNotHandledErr;

		case kEventMouseUp:
		{
			SPos Pos(pt.h, pt.v);
			((CWindowOSX*)inUserData)->OnMouseHook(LeftButtonUp, Pos);
		}
		return noErr;
//		return eventNotHandledErr;

		case kEventMouseMoved:
		case kEventMouseDragged:
		{
			SPos Pos(pt.h, pt.v);
			((CWindowOSX*)inUserData)->OnMouseHook(MouseMove, Pos);
		}
		return noErr;
//		return eventNotHandledErr;
	}

	return eventNotHandledErr;
}

OSStatus WindowHandlerProc(EventHandlerCallRef inHandlerCallRef, EventRef inEvent, void* inUserData)
{
	switch(GetEventKind(inEvent)) {
		case kEventWindowCursorChange:
		{
			((CWindowOSX*)inUserData)->SetCurrentMouseCursor();
		}
		return noErr;

		case kEventWindowDrawContent:
		{
			ge::SSize Size;
			((CWindowOSX*)inUserData)->GetSize(Size);
			ge::SRect Rect(ge::SPos(0, 0), Size);
			((CWindowOSX*)inUserData)->Redraw(Rect);
		}
		return noErr;

/*		case kEventWindowHidden:
		{
			((CWindowOSX*)inUserData)->Shown(false);
		}
		return noErr;*/

/*		case kEventWindowShown:
		{
			((CWindowOSX*)inUserData)->Shown(true);
		}
		return noErr;*/
	}

	return eventNotHandledErr;
}

OSErr DragAndDropProc(WindowRef theWindow, void * handlerRefCon, DragRef theDrag)
{
	return noErr;
}









