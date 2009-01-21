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
	mpPane->SetSize(ge::SSize(396, 100));
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

	::CreateNewWindow(kSheetWindowClass, kWindowNoShadowAttribute | kWindowStandardHandlerAttribute, &bnds, &mWindowRef);

	Rect r;
	::GetWindowBounds(RefParent, kWindowContentRgn, &r);
	r.right = r.left + uiPlugSizeX;
	r.top += iY;
	r.bottom = r.top + uiPlugSizeY;
	::SetWindowBounds(mWindowRef, kWindowContentRgn, &r);

	::ShowWindow(mWindowRef);

	WindowGroupRef WindowGroup;
	::BringToFront(mWindowRef);
	WindowGroupAttributes windowGroupAttributes = 0;
	::CreateWindowGroup(windowGroupAttributes, &WindowGroup);
	::SetWindowGroupParent(WindowGroup, GetWindowGroup(RefParent));

	::SetWindowGroup(mWindowRef, WindowGroup);
	::SetWindowGroup(RefParent, WindowGroup);
	::SendBehind(RefParent, mWindowRef);

	::ChangeWindowGroupAttributes(WindowGroup, kWindowGroupAttrMoveTogether | kWindowGroupAttrLayerTogether | kWindowGroupAttrHideOnCollapse | kWindowGroupAttrSharedActivation, NULL); 

	::SendBehind(RefParent, mWindowRef);
	::ShowWindow(mWindowRef);

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
