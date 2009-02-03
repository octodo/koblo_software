#include "KSOS.h"


CKSPlugInEffectPane::CKSPlugInEffectPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI),
	mpGUI(NULL), mpTimer(NULL),
	mbGUILoaded(false)
{
}

CKSPlugInEffectPane::~CKSPlugInEffectPane()
{
	if (mpTimer) {
		mpTimer->Destroy();
	}

#ifdef _Mac
	::DisposeWindow(mWindowRef);
#endif	// _Mac
}

void CKSPlugInEffectPane::Init()
{
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(1000, 1000));
}

void CKSPlugInEffectPane::ConnectControls()
{
}

void CKSPlugInEffectPane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	switch(pControl->GetID()) {
	}

	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew); 
}

#ifdef WIN32
LRESULT CALLBACK PlugInWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

extern HINSTANCE ghInstance;

HWND ghWndChild;
BOOL CALLBACK EnumChild(HWND hwnd, LPARAM lParam)
{
	ghWndChild = hwnd;

	return FALSE;
}
#endif	// WIN32

#ifdef _Mac
static HIObjectClassRef gClass = NULL;
static EventTypeSpec gClassEvents[] =
{
{ kEventClassHIObject, kEventHIObjectConstruct },
{ kEventClassHIObject, kEventHIObjectInitialize },
{ kEventClassHIObject, kEventHIObjectDestruct },
{ kEventClassKeyboard, kEventRawKeyDown},
{ kEventClassKeyboard, kEventRawKeyUp},
{ kEventClassKeyboard, kEventRawKeyRepeat},
{ kEventClassMouse,    kEventMouseMoved},
{ kEventClassMouse,    kEventMouseWheelMoved},
{ kEventClassControl,  kEventControlBoundsChanged},
{ kEventClassControl,  kEventControlDraw},
{ kEventClassControl,  kEventControlSetFocusPart },
{ kEventClassControl,  kEventControlClick },
{ kEventClassControl,  kEventControlTrack }
};
#define gViewClassID CFSTR("com.Koblo.Studio.PlugInView")

static pascal OSStatus ViewClassHandler(EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon);

static pascal OSStatus ViewClassHandler(EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon)
{
	OSStatus result = eventNotHandledErr;
	int* pain = (int*)inRefcon;

	if (GetEventClass(inEvent) == kEventClassHIObject) {
		switch (GetEventKind(inEvent)) {
			case kEventHIObjectConstruct: {
				int* pa = new int;
				GetEventParameter(inEvent,kEventParamHIObjectInstance, typeHIObjectRef, NULL, sizeof(int), NULL, &pa);
				result = SetEventParameter(inEvent, kEventParamHIObjectInstance, typeVoidPtr, sizeof(int), &pa);
				break;
			}

			case kEventHIObjectDestruct: {
				if (pain != NULL) {
					delete pain;
				}
                result = noErr;
                break;
			}
		}
	}

	return result;
}

static pascal OSStatus TransparentWindowHandler(EventHandlerCallRef inHandlerCallRef, EventRef inEvent, void *inUserData)
{
	OSStatus status = eventNotHandledErr;
	
	switch(GetEventKind(inEvent))
	{
		case kEventWindowGetRegion:
		{
			WindowRegionCode code;
			RgnHandle rgn;
			
			// which region code is being queried?
			GetEventParameter(inEvent, kEventParamWindowRegionCode, typeWindowRegionCode, NULL, sizeof(code), NULL, &code);
			
			// if it is the opaque region code then set the region to Empty and return noErr to stop the propagation
			if (code == kWindowOpaqueRgn)
			{
				GetEventParameter(inEvent, kEventParamRgnHandle, typeQDRgnHandle, NULL, sizeof(rgn), NULL, &rgn);
				SetEmptyRgn(rgn);
				status = noErr;
			}
			break;
		}
			
		case kEventWindowDrawContent:
		{
			GrafPtr port;
			CGContextRef context;
			Rect portBounds;
			HIRect bounds;
			
			GetPort(&port);
			GetPortBounds(port, &portBounds);
			
			// we need a Quartz context so that we can use transparency
			QDBeginCGContext(port, &context);
			
			// make the whole content transparent
			bounds = CGRectMake(0, 0, portBounds.right, portBounds.bottom);
			CGContextClearRect(context, bounds);
			
			QDEndCGContext(port, &context);
			
			// we need to let the HIToolbox and the regular kEventWindowDrawContent handler do their work,
			// mainly draw the subviews, so we return eventNotHandledErr to propagate.
			status = eventNotHandledErr;
			break;
		}
			
		case kEventControlDraw:
		{
			CGContextRef context;
			HIRect bounds;
			
			GetEventParameter(inEvent, kEventParamCGContextRef, typeCGContextRef, NULL, sizeof(context), NULL, &context);
			HIViewGetBounds((HIViewRef)inUserData, &bounds);
			
			// make the whole content transparent
			CGContextClearRect(context, bounds);
			
			// we must not let the default draw handler of the content view be called (it would draw the default opaque theme)
			// so we return noErr to stop the propagation.
			status = noErr;
			break;
		}
	}
	
	return status;
}
#endif

#ifdef _Mac
OSStatus MakeWindowTransparent(WindowRef aWindowRef)
{
	OSStatus status = paramErr;
	require(aWindowRef != NULL, paramErr);
	
	// is the window compositing or not?
	WindowAttributes attributes;
	status = GetWindowAttributes(aWindowRef, &attributes);
	require_noerr(status, GetWindowAttributes);
	
/*	if (attributes & kWindowCompositingAttribute)
	{
		// it is compositing so we intercept the kEventWindowGetRegion event to be able to specify an empty opaque region
		EventTypeSpec wCompositingEvents = { kEventClassWindow, kEventWindowGetRegion };
		status = InstallWindowEventHandler(aWindowRef, TransparentWindowHandler, 1, &wCompositingEvents, aWindowRef, NULL);
		require_noerr(status, InstallWindowEventHandler);
		
		HIViewRef contentView;
		status = HIViewFindByID(HIViewGetRoot(aWindowRef), kHIViewWindowContentID, &contentView);
		require_noerr(status, HIViewFindByID);
		
		// and we intercept the kEventControlDraw event of our content view so that we can make it transparent
		EventTypeSpec cCompositingEvents = { kEventClassControl, kEventControlDraw };
		status = InstallControlEventHandler(contentView, TransparentWindowHandler, 1, &cCompositingEvents, contentView, NULL);
		require_noerr(status, InstallControlEventHandler);
	}
	else*/
	{
		// it is non-compositing so we intercept the kEventWindowGetRegion event to be able to specify an empty opaque region
		// and we intercept the kEventWindowDrawContent event of our window so that we can make it transparent
		EventTypeSpec wNonCompositingEvents[] =
		{
			{ kEventClassWindow, kEventWindowGetRegion },
			{ kEventClassWindow, kEventWindowDrawContent }
		};
		status = InstallWindowEventHandler(aWindowRef, TransparentWindowHandler, GetEventTypeCount(wNonCompositingEvents), wNonCompositingEvents, aWindowRef, NULL);
		require_noerr(status, InstallWindowEventHandler);
	}
	
	// telling the HIToolbox that our window is not opaque so that we will be asked for the opaque region
	UInt32 features;
	status = GetWindowFeatures(aWindowRef, &features);
	require_noerr(status, GetWindowFeatures);
	if ( ( features & kWindowIsOpaque ) != 0 )
	{
		status = HIWindowChangeFeatures(aWindowRef, 0, kWindowIsOpaque);
		require_noerr(status, HIWindowChangeFeatures);
	}
    
	// force opaque shape to be recalculated
	status = ReshapeCustomWindow(aWindowRef);
	require_noerr(status, ReshapeCustomWindow);
	
	// ensure that HIToolbox doesn't use standard shadow style, which defeats custom opaque shape
	status = SetWindowAlpha(aWindowRef, 0.999);
	require_noerr(status, SetWindowAlpha);
	
SetWindowAlpha:
ReshapeCustomWindow:
HIWindowChangeFeatures:
GetWindowFeatures:
InstallControlEventHandler:
HIViewFindByID:
InstallWindowEventHandler:
GetWindowAttributes:
paramErr:
	
	return status;
}
#endif

void CKSPlugInEffectPane::SetGUI(kspi::IGUI* pGUI)
{
	tuint32 uiPlugSizeX;
	tuint32 uiPlugSizeY;
	pGUI->GetSize(&uiPlugSizeX, &uiPlugSizeY);

	mpPane->SetSize(ge::SSize(uiPlugSizeX, uiPlugSizeY));
	
	dynamic_cast<CKSPlugInGUIPane*>(mpParent)->SetWidth(uiPlugSizeX);

	tint32 iY = 24;

#ifdef _Mac
	WindowRef RefParent = (WindowRef)(GetWindow()->GetParent());

	Rect rctSize;
	::GetWindowBounds(RefParent, kWindowContentRgn, &rctSize);
	rctSize.right = rctSize.left + uiPlugSizeX;
	rctSize.bottom = rctSize.top + uiPlugSizeY + iY;
	::SetWindowBounds(RefParent, kWindowContentRgn, &rctSize);

	Rect bnds;
	bnds.top = 0;
	bnds.left = 0;
	bnds.right = uiPlugSizeX;
	bnds.bottom = uiPlugSizeY;

	::CreateNewWindow(kOverlayWindowClass, kWindowNoShadowAttribute | kWindowStandardHandlerAttribute, &bnds, &mWindowRef);
//	::CreateNewWindow(kFloatingWindowClass, kWindowNoShadowAttribute | kWindowStandardHandlerAttribute, &bnds, &mWindowRef);
//	::CreateNewWindow(kOverlayWindowClass, kWindowCompositingAttribute | kWindowNoShadowAttribute | kWindowStandardHandlerAttribute, &bnds, &mWindowRef);

/*	static int a = 0;
	if (a++ == 0) {
		HIObjectRegisterSubclass(gViewClassID,
			kHIViewClassID,
			0,
			ViewClassHandler,
			GetEventTypeCount(gClassEvents),
			gClassEvents,
			0,
			&gClass);
	}

	OSErr err;
	HIViewRef ViewRef = NULL;
	EventRef Event = NULL;
	err = ::CreateEvent( NULL, kEventClassHIObject, kEventHIObjectInitialize, GetCurrentEventTime(), 0, &Event);
	HIRect Bounds;
	Bounds.origin.x = 0;
	Bounds.origin.y = 0;
	Bounds.size.width = uiPlugSizeX;
	Bounds.size.height = uiPlugSizeY;
	err = ::SetEventParameter(Event, kEventParamBounds, typeHIRect, sizeof(HIRect), &Bounds);
	err = ::HIObjectCreate(gViewClassID, Event, (HIObjectRef*)&ViewRef);
	err = ::HIViewChangeFeatures(ViewRef, kHIViewAllowsSubviews, 0);

	HIViewRef contentView;
	if (HIViewFindByID (HIViewGetRoot ((WindowRef)RefParent), kHIViewWindowContentID, &contentView) == noErr) {
		err = ::HIViewChangeFeatures(contentView, kHIViewAllowsSubviews, 0);
		err = ::HIViewAddSubview(contentView, ViewRef);
	}

	mWindowRef = ;*/

	Rect r;
	::GetWindowBounds(RefParent, kWindowContentRgn, &r);
	r.right = r.left + uiPlugSizeX;
	r.top += iY;
	r.bottom = r.top + uiPlugSizeY;
	::SetWindowBounds(mWindowRef, kWindowContentRgn, &r);

	::ShowWindow(mWindowRef);

/*	WindowGroupRef WindowGroup;
	::BringToFront(mWindowRef);
	WindowGroupAttributes windowGroupAttributes = 0;
	::CreateWindowGroup(windowGroupAttributes, &WindowGroup);
//	::SetWindowGroupParent(WindowGroup, GetWindowGroup(RefParent));

	::SetWindowGroup(mWindowRef, WindowGroup);
	::SetWindowGroup(RefParent, WindowGroup);
	::SendBehind(RefParent, mWindowRef);

	::ChangeWindowGroupAttributes(WindowGroup, kWindowGroupAttrMoveTogether | kWindowGroupAttrLayerTogether | kWindowGroupAttrHideOnCollapse | kWindowGroupAttrSharedActivation, NULL); 

	::SendBehind(RefParent, mWindowRef);
	::ShowWindow(mWindowRef);*/

	MakeWindowTransparent(RefParent);

	::BringToFront(mWindowRef);
	::SendBehind(RefParent, mWindowRef);

	WindowGroupRef WindowGroup;
	WindowGroupAttributes windowGroupAttributes = kWindowGroupAttrMoveTogether | kWindowGroupAttrLayerTogether | kWindowGroupAttrHideOnCollapse | kWindowGroupAttrSharedActivation;
	::CreateWindowGroup(windowGroupAttributes, &WindowGroup);
	 
	::SetWindowGroup(mWindowRef, WindowGroup);
	::SetWindowGroup(RefParent, WindowGroup);
	 
	::ShowWindow(mWindowRef);

//	::DebugPrintWindowGroup(WindowGroup);

	pGUI->MakeWindow((void*)mWindowRef);

	pGUI->Paint();

	mpGUI = pGUI;

	mpTimer = ITimer::Create();
	mpTimer->Init(0, dynamic_cast<ITimerCallback*>(this), 50);
#endif	// _Mac

#ifdef WIN32
	HWND hwndParent = (HWND)(GetWindow()->GetParent());

//	::SetWindowPos(hwndParent, 0, 0, 0, uiPlugSizeX, uiPlugSizeY + iY, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	static tbool gbPlugInClassRegistered = false;
	if (gbPlugInClassRegistered == false) {
		WNDCLASS wc;
		wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wc.lpfnWndProc   = (WNDPROC)PlugInWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = ghInstance;
		wc.hIcon         = 0;
		wc.hCursor       = NULL;
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wc.lpszMenuName  = 0;
		wc.lpszClassName = "KSPlugInWnd";

		::RegisterClass(&wc);
	}

	DWORD dwStyle = WS_CHILD | WS_VISIBLE;
	DWORD dwExStyle = 0;

	ghWndChild = NULL;
	::EnumChildWindows(hwndParent, EnumChild, 0);

	::SetWindowPos(hwndParent, 0, 0, 0, uiPlugSizeX, uiPlugSizeY + iY, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	RECT rctParent;
	::GetClientRect(hwndParent, &rctParent);
	::SetWindowPos(hwndParent, 0, 0, 0, uiPlugSizeX + (uiPlugSizeX - rctParent.right),
		uiPlugSizeY + iY + (uiPlugSizeY + iY - rctParent.bottom),
		SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	::SetWindowPos(ghWndChild, 0, 0, 0, uiPlugSizeX, uiPlugSizeY + iY, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	HWND hWndPlug = ::CreateWindowEx(dwExStyle,
		"KSPlugInWnd",
		NULL,
		dwStyle,
		0, iY, uiPlugSizeX, uiPlugSizeY,
		ghWndChild,
		NULL,
		ghInstance,
		(LPVOID)this);

	pGUI->MakeWindow((void*)hWndPlug);

	pGUI->Paint();

	mpGUI = pGUI;

	mpTimer = ITimer::Create();
	mpTimer->Init(0, dynamic_cast<ITimerCallback*>(this), 50);
#endif	// WIN32

	mbGUILoaded = true;
}

void CKSPlugInEffectPane::OnTimer(tint32 iID)
{
	if (mbGUILoaded) {
		mpGUI->OnTimer();
	}
}

void CKSPlugInEffectPane::OnCloseGUI()
{
	mbGUILoaded = false;

	mpGUI->Destroy();

#ifdef _Mac
	::DisposeWindow(mWindowRef);
#endif
}

#ifdef WIN32
LRESULT CALLBACK PlugInWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}
#endif	// WIN32
