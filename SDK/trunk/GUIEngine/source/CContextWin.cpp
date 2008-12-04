/*!	\file CContextWin.cpp
	\author Michael Olsen
	\date 12/Jan/2005
	\date 12/Jan/2005
*/


#include "geInternalOS.h"

CContextWin::CContextWin() : mhInstance((HINSTANCE)INVALID_HANDLE_VALUE)
{
	// Lasse, mhMenu (and thus dynamic menus) are not actually implemented... yet
	mhMenu = NULL;
}

CContextWin::~CContextWin()
{
}

k2s::ge::IContext* k2s::ge::IContext::Create()
{
	return dynamic_cast<k2s::ge::IContext*>(new CContextWin());
}

void CContextWin::Destroy()
{
	delete dynamic_cast<CContextWin*>(this);
}

void CContextWin::SetInstanceData(void* p)
{
	mhInstance = (HINSTANCE)p;
}

void* CContextWin::GetInstanceData()
{
	return (void*)mhInstance;
}

const void* CContextWin::GetInstanceData() const
{
	return (const void*)mhInstance;
}

void CContextWin::GetPathName(tchar* pszPathName)
{
	::GetModuleFileName((HMODULE)mhInstance, (char*)pszPathName, 513);

	// Step backwards in string until last '\' is found
	tchar* pChar = pszPathName + strlen((char*)pszPathName) - 1;
	while (*pChar != '\\') {
		pChar--;
	}
	// We have the last '\', so simply advance one, and set to 0 (to terminate string)
	pChar++;
	*pChar = 0;
}

extern "C" LRESULT CALLBACK K2SGEWndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK K2SGEContextWndProc(HWND, UINT, WPARAM, LPARAM);

void CContextWin::GetClassName(std::string& rs)
{
	tchar pszName[65];
	mpCallback->GetName((tchar*)pszName);
	std::string sName((char*)pszName);

	std::string sName2;
	tuint l;
	tchar psz[2];
	psz[1] = NULL;
	for (l = 0; l < sName.size(); l++) {
		if ((sName[l] >= 'a' && sName[l] <= 'z') ||
			(sName[l] >= 'A' && sName[l] <= 'Z') ||
			(sName[l] >= '0' && sName[l] <= '9')) {
			psz[0] = sName[l];
			sName2 += std::string((char*)psz);
		}
	}

	rs = sName2;
}

void CContextWin::Init()
{
	WNDCLASS wc;

	std::string s;
	GetClassName(s);

	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc   = (WNDPROC)K2SGEWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = mhInstance;
	// Lasse, LoadIcon always fail - why call it at all?
	wc.hIcon         = LoadIcon (mhInstance, s.c_str());
	wc.hCursor       = NULL;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = s.c_str();

	// Call may fail, but that just means we already registered it
	RegisterClass(&wc);
}

void CContextWin::PreDestroyMainWnd()
{
	PostQuitMessage(0);
}

LRESULT CALLBACK K2SGEWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	try {
		switch (message) {
			case WM_CREATE:
				{
					IWindow* pWnd = (IWindow*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
					SetWindowLong(hWnd, GWL_USERDATA, (LONG)pWnd);
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
			case WM_DESTROY:
				{
					IWindow* pWnd = (IWindow*)GetWindowLong(hWnd, GWL_USERDATA);
					if (pWnd == NULL) {
						return DefWindowProc(hWnd, message, wParam, lParam);
					}

					SetWindowLong(hWnd, GWL_USERDATA, NULL);

					// Lasse, why no break? a bug? - added 2008-05-06
					break;
					// .. Lasse

//					dynamic_cast<CContext*>(pWnd->GetContext())->PreDestroyMainWnd();
//					pWnd->Destroy();

/*					tbool bProcessed = false;
					LRESULT lRet = dynamic_cast<CWindowWin*>(pWnd)->WndProc(hWnd, message, wParam, lParam, bProcessed);
					if (bProcessed) {
						return lRet;
					}
					else {
						return DefWindowProc(hWnd, message, wParam, lParam);
					}*/
				}

			default:
				{
					IWindow* pWnd = (IWindow*)GetWindowLong(hWnd, GWL_USERDATA);
					if (pWnd == NULL) {
						return DefWindowProc(hWnd, message, wParam, lParam);
					}

					tbool bProcessed = false;
					LRESULT lRet;
					CWindowWin* pWnd2 = dynamic_cast<CWindowWin*>(pWnd);
					if (pWnd2) {
						lRet = pWnd2->WndProc(hWnd, message, wParam, lParam, bProcessed);
					}
					if (bProcessed) {
						return lRet;
					}
					else {
						return DefWindowProc(hWnd, message, wParam, lParam);
					}
				}
				break;
		}
	}
	catch(...) {
		static tbool bMessageShown = false;
		if (bMessageShown == false) {
			bMessageShown = true;

			::MessageBox(NULL, "An error have occured running this software.\nThis could be due to an error in software or in your hardware/drivers.\nPlease contact technical support for further information.\nThis software will now attempt to close down.",
				"Severe Error", MB_OK | MB_SETFOREGROUND | MB_TASKMODAL);

/*			CWnd* pWnd = (CWnd*)GetWindowLong(hWnd, GWL_USERDATA);
			if (pWnd != NULL) {
				pWnd->GetApp()->PrePanicShutDown();
			}*/

			::exit(0);
		}

		// Keep compiler happy
		return 0;
	}

	// Lasse, no - compiler wasn't happy: 
	return DefWindowProc(hWnd, message, wParam, lParam);;
}

CContextWin* gpMainContext = NULL;

void* CContextWin::CreateMainWindow(SSize Size, tint32 iWindowsOnly_MenuResourceID /*= -1*/, tint32 iWindowsOnly_IconResourceID /*= -1*/)
{
	gpMainContext = this;

	WNDCLASS wc;

	std::string s;
	GetClassName(s);
	std::string sName = s;
	s += "main";

	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc   = (WNDPROC)K2SGEContextWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = mhInstance;
	//wc.hIcon         = LoadIcon (mhInstance, s.c_str());
	wc.hIcon         = LoadIcon (mhInstance, (iWindowsOnly_IconResourceID >= 0) ? MAKEINTRESOURCE(iWindowsOnly_IconResourceID) : s.c_str());
	wc.hCursor       = NULL;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	//wc.lpszMenuName  = NULL;
	wc.lpszMenuName  = (iWindowsOnly_MenuResourceID >= 0) ? MAKEINTRESOURCE(iWindowsOnly_MenuResourceID) : NULL;
	wc.lpszClassName = s.c_str();

	::RegisterClass(&wc);

	{
		// Register a second class for extra windows (with no menu)
		std::string s;
		GetClassName(s);
		std::string sName = s;
		s += "extra";

		wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wc.lpfnWndProc   = (WNDPROC)K2SGEContextWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = mhInstance;
		//wc.hIcon         = LoadIcon (mhInstance, s.c_str());
		wc.hIcon         = LoadIcon (mhInstance, (iWindowsOnly_IconResourceID >= 0) ? MAKEINTRESOURCE(iWindowsOnly_IconResourceID) : s.c_str());
		wc.hCursor       = NULL;
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = s.c_str();

		::RegisterClass(&wc);
	}

	DWORD dwStyle = WS_OVERLAPPEDWINDOW;
	DWORD dwExStyle = 0;

	// Get desktop size, so we can calculate position
	RECT rctDesktop;
	::GetWindowRect(::GetDesktopWindow(), &rctDesktop);

	HWND hWnd = ::CreateWindowEx(dwExStyle,
		s.c_str(),
		sName.c_str(),
		dwStyle,
		(rctDesktop.right - rctDesktop.left) / 2 - (Size.iCX / 2),
		(rctDesktop.bottom - rctDesktop.top) / 2 - (Size.iCY / 2),
		Size.iCX, Size.iCY,
		NULL,
		NULL,
		(HINSTANCE)GetInstanceData(),
		//(LPVOID)dynamic_cast<IContext*>(this));
		(LPVOID)this);

	RECT Rect;
	::GetClientRect(hWnd, &Rect);
	// Calculate how much we need to resize... The smaller the client rect is, the more we got "off" when doing the original size
	SSize SizeClient(Rect.right - Rect.left, Rect.bottom - Rect.top);
	Size += (Size - SizeClient);
	::SetWindowPos(hWnd, 0, 0, 0, Size.iCX, Size.iCY, SWP_NOMOVE | SWP_NOZORDER);

	return (void*)hWnd;
}

void* CContextWin::CreateExtraWindow(void* pszResName, SSize Size, tbool bAlwaysOnTop)
{
	std::string s;
	GetClassName(s);
	s += "extra";

	DWORD dwStyle = WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX;
	DWORD dwExStyle = 0;
	if (bAlwaysOnTop) {
		dwExStyle |= WS_EX_TOPMOST;
	}

	// Get desktop size, so we can calculate position
	RECT rctDesktop;
	::GetWindowRect(::GetDesktopWindow(), &rctDesktop);

	HWND hWnd = ::CreateWindowEx(dwExStyle,
		s.c_str(),
		(LPCTSTR)pszResName,//NULL,
		dwStyle,
		(rctDesktop.right - rctDesktop.left) / 2 - (Size.iCX / 2),
		(rctDesktop.bottom - rctDesktop.top) / 2 - (Size.iCY / 2),
		Size.iCX, Size.iCY,
		NULL,
		NULL,
		(HINSTANCE)GetInstanceData(),
		//(LPVOID)dynamic_cast<IContext*>(this));
		(LPVOID)this);

	RECT Rect;
	::GetClientRect(hWnd, &Rect);
	// Calculate how much we need to resize... The smaller the client rect is, the more we got "off" when doing the original size
	SSize SizeClient(Rect.right - Rect.left, Rect.bottom - Rect.top);
	Size += (Size - SizeClient);
	::SetWindowPos(hWnd, 0, 0, 0, Size.iCX, Size.iCY, SWP_NOMOVE | SWP_NOZORDER);

	return (void*)hWnd;
}

void CContextWin::LimitedHandleEventQueue(tbool bAllowMouseAndKeyboard, tbool* pbBreak)
{
	*pbBreak = false;

	MSG msg;
	// Handle all waiting messages
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if ((msg.message == WM_QUIT) || (msg.message == WM_COMMAND)) {
			// Don't handle application quit
			*pbBreak = true;
			return;
		}

		// New message
		GetMessage(&msg, NULL, 0, 0);
		if (
			(!bAllowMouseAndKeyboard)
			&&
			(
				(
					(msg.message == WM_LBUTTONDOWN)
					||
					(msg.message == WM_LBUTTONDBLCLK)
					||
					(msg.message == WM_RBUTTONDOWN)
					||
					(msg.message == WM_RBUTTONDBLCLK)
					||
					(msg.message == WM_MBUTTONDOWN)
					||
					(msg.message == WM_MBUTTONDBLCLK)
				)
				||
				((msg.message >= WM_KEYFIRST) && (msg.message <= WM_KEYLAST))
			)
		) {
			// We must ignore this message
			// Is it OK to just throw it away? Dunno...
		}
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
} // LimitedHandleEventQueue

void CContextWin::RunMainLoop()
{
	MSG msg;
//!!!
/*	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(ghInstance, (LPCTSTR)IDC_INETSYNTHAPP);*/

	tint32 iRunIndex = 0;
	tbool bStop = false;

//!!!
	while (bStop == false) {
		// Lasse, bug-fix 2008-06-18 - make sure redraws get through
		/*
		// old code:
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			// New message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// No message
			if (mpAppCallback) {
				bStop = mpAppCallback->OnInit(iRunIndex++);
			}
			else {
				bStop = true;
			}
		}
		*/
		// new code:
		// Handle all waiting messages
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			// New message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// No waiting messages - perform next init step
		if (mpAppCallback) {
			bStop = mpAppCallback->OnInit(iRunIndex++);
		}
		else {
			bStop = true;
		}
		// .. Lasse
	}

	while (GetMessage(&msg, NULL, 0, 0)) {
//!!!
//		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
//		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
//		}
	}
}

void CContextWin::CreateMenu(const SMenuItemList* pList)
{
	// Lasse, later dude...
	/*
	if (mhMenu) {
		mhMenu = ::CreateMenu();
		if (mhMenu) {

		}
	}
	*/
}

void CContextWin::ShowWindow(void* pWnd, tbool bShow)
{
	::ShowWindow((HWND)pWnd, bShow ? SW_SHOW : SW_HIDE);
	if (bShow)
		::RedrawWindow((HWND)pWnd, NULL, NULL, RDW_INVALIDATE);
}

tbool CContextWin::IsWindowVisible(void* pWnd)
{
	return (::IsWindowVisible((HWND)pWnd) != FALSE);
}

void CContextWin::SelectWindow(void* pWnd)
{
	// Lasse, implemented 2008-04-30
	//throw IException::Create(IException::TypeCode, IException::ReasonCodePathInvalid, EXCEPTION_INFO, "Not implemented");
	ShowWindow(pWnd, TRUE);
	::SetForegroundWindow((HWND)pWnd);
	// .. Lasse
}

void CContextWin::CloseWindow(void* pWnd)
{
	::DestroyWindow((HWND)pWnd);
}

struct SContextAndClosingBehaviour {
	ge::CContextWin* pContextWin;
	ge::IContext::EClosingBehaviour eClosingBehaviour;
	POINT pointSizeMin, pointSizeMax;

	SContextAndClosingBehaviour()
	{
		eClosingBehaviour = ge::IContext::giClosingReallyCloses;
		pointSizeMin.x = pointSizeMin.y = 0;
		pointSizeMax.x = pointSizeMax.y = LONG_MAX;
	}
};

LRESULT CALLBACK K2SGEContextWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	try {
		switch (message) {
			case WM_CREATE:
				{
					CContextWin* pContext = (CContextWin*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
					SContextAndClosingBehaviour* pStruct = new SContextAndClosingBehaviour();
					pStruct->pContextWin = pContext;
					SetWindowLong(hWnd, GWL_USERDATA, (LONG)pStruct);
					return DefWindowProc(hWnd, message, wParam, lParam);
				}

			// Lasse, added 2008-04-30
			case WM_CLOSE:
				{
					SContextAndClosingBehaviour* pStruct = (SContextAndClosingBehaviour*)GetWindowLong(hWnd, GWL_USERDATA);
					if (pStruct) {
						if (pStruct->eClosingBehaviour == ge::IContext::giClosingOnlyHides) {
							::ShowWindow(hWnd, SW_HIDE);
							return 1;
						}
					}
				}
			// .. Lasse

			case WM_DESTROY:
				{
					//CContextWin* pContext = (CContextWin*)GetWindowLong(hWnd, GWL_USERDATA);
					CContextWin* pContext = NULL;
					SContextAndClosingBehaviour* pStruct = (SContextAndClosingBehaviour*)GetWindowLong(hWnd, GWL_USERDATA);
					if (pStruct)
						pContext = pStruct->pContextWin;
					if (pContext == NULL) {
						return DefWindowProc(hWnd, message, wParam, lParam);
					}

					delete pStruct;
					SetWindowLong(hWnd, GWL_USERDATA, NULL);

//					::PostQuitMessage(0);
					gpMainContext->WindowClosed((void*)hWnd);

					// Lasse, added 2008-04-30 - bug-fix (would fall through to case below)
					return DefWindowProc(hWnd, message, wParam, lParam);
					// .. Lasse

//					dynamic_cast<CContext*>(pContext->GetContext())->PreDestroyMainWnd();
//					pContext->Destroy();

/*					tbool bProcessed = false;
					LRESULT lRet = dynamic_cast<CWindowWin*>(pContext)->WndProc(hWnd, message, wParam, lParam, bProcessed);
					if (bProcessed) {
						return lRet;
					}
					else {
						return DefWindowProc(hWnd, message, wParam, lParam);
					}*/
				}

				// Lasse, added 2008-05-07
			case WM_GETMINMAXINFO:
				{
					//ge::IContext* pContext = NULL;
					SContextAndClosingBehaviour* pStruct = (SContextAndClosingBehaviour*)GetWindowLong(hWnd, GWL_USERDATA);
					if (pStruct) {
						MINMAXINFO* p = (MINMAXINFO*)lParam;
						p->ptMinTrackSize = pStruct->pointSizeMin;
						p->ptMaxTrackSize = pStruct->pointSizeMax;
					}
				}
				// We don't return but pass on to default handler
				break;
				// .. Lasse

			case WM_SIZE:
				{
					switch(wParam)
					{
						case SIZE_MAXIMIZED:
						case SIZE_RESTORED:
							{
								tint32 iSizeX = lParam & 0xffff;
								tint32 iSizeY = (lParam >> 16) & 0xffff;

								gpMainContext->SizeChanged(hWnd, SSize(iSizeX, iSizeY));
							}
							break;
					}
				}
				return DefWindowProc(hWnd, message, wParam, lParam);

			case WM_ERASEBKGND:
				// For WM_ERASENKGRND the "I did this" return-code is 1
				return 1;
			
				// Lasse, later dude... when we get dynamic menus
			//case WM_MENUCOMMAND:
			//break:
				// .. Lasse

			case WM_COMMAND:
				{
					SContextAndClosingBehaviour* pStruct = (SContextAndClosingBehaviour*)GetWindowLong(hWnd, GWL_USERDATA);
					if (pStruct) {
						CContextWin* pContext = pStruct->pContextWin;
						if (pContext) {
							WORD ucItemID = LOWORD(wParam);
							WORD ucOrigin = HIWORD(wParam);
							tbool bIsMenu = (ucOrigin == 0);
							if (ucItemID != 0) {
								// Move into 32 bit int before typecasting to string. This will make zero-termination work (in case interpretation on receiving end is wrong)
								tint32 iMessage = ucItemID;
								tchar* pszMessage = (tchar*)&iMessage;
								if (pContext->SendOnMenuEvent(pszMessage)) {
									// For WM_COMMAND - the "I handled this" return-code is 0
									return 0;
								}
							}
						}
					}
					// Pass on to default handler
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
				break;

			default:
				{
					//CContextWin* pContext = (CContextWin*)GetWindowLong(hWnd, GWL_USERDATA);
					ge::IContext* pContext = NULL;
					SContextAndClosingBehaviour* pStruct = (SContextAndClosingBehaviour*)GetWindowLong(hWnd, GWL_USERDATA);
					if (pStruct)
						pContext = pStruct->pContextWin;
					if (pContext == NULL) {
						return DefWindowProc(hWnd, message, wParam, lParam);
					}

/*					tbool bProcessed = false;
					LRESULT lRet;
					CContextWin* pContext2 = dynamic_cast<CContextWin*>(pContext);
					if (pContext2) {
						lRet = pContext2->WndProc(hWnd, message, wParam, lParam, bProcessed);
					}
					if (bProcessed) {
						return lRet;
					}
					else {
						return DefWindowProc(hWnd, message, wParam, lParam);
					}*/
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
		}
	}
	catch(...) {
		static tbool bMessageShown = false;
		if (bMessageShown == false) {
			bMessageShown = true;

			::MessageBox(NULL, "An error have occured running this software.\nThis could be due to an error in software or in your hardware/drivers.\nPlease contact technical support for further information.\nThis software will now attempt to close down.",
				"Severe Error", MB_OK | MB_SETFOREGROUND | MB_TASKMODAL);

/*			CWnd* pContext = (CWnd*)GetWindowLong(hWnd, GWL_USERDATA);
			if (pContext != NULL) {
				pContext->GetApp()->PrePanicShutDown();
			}*/

			::exit(0);
		}

		// Lasse, this is junk
		/*
		// Keep compiler happy
		return 0;
		*/
		// .. Lasse
	}

	// Compiler wasn't happy - now it is
	return DefWindowProc(hWnd, message, wParam, lParam);
}

tbool CContextWin::SendOnMenuEvent(tchar* pszMenEvent)
{
	if (mpAppCallback == NULL)
		return false;

	mpAppCallback->OnMenuEvent(pszMenEvent);
	return true;
}

void CContextWin::SizeChanged(void* pWnd, SSize SizeNew)
{
	if (mpAppCallback) {
		mpAppCallback->SizeChanged(pWnd, SizeNew);
	}
}


void CContextWin::SetClosingBehaviour(void* pWnd, EClosingBehaviour eClosingBehaviour)
{
	SContextAndClosingBehaviour* pStruct = (SContextAndClosingBehaviour*)GetWindowLong((HWND)pWnd, GWL_USERDATA);
	if (pStruct) {
		pStruct->eClosingBehaviour = eClosingBehaviour;
	}
}

const ge::IContext::EClosingBehaviour CContextWin::GetClosingBehaviour(void* pWnd)
{
	SContextAndClosingBehaviour* pStruct = (SContextAndClosingBehaviour*)GetWindowLong((HWND)pWnd, GWL_USERDATA);
	if (pStruct) {
		return pStruct->eClosingBehaviour;
	}

	return ge::IContext::giClosingReallyCloses;
}

void CContextWin::SetMinWindowSize(IWindow* pWnd, SSize sizeMin)
{
	// We get the size of client drawing area - we must add the frame width
	// (because restrictions only apply to framed windows)

	// Adjust for frame width
	HWND hwndParent = dynamic_cast<CWindowWin*>(pWnd)->GetHwndParent();
	RECT rectParent;
	if (::GetWindowRect(hwndParent, &rectParent)) {
		HWND hwndInside = dynamic_cast<CWindowWin*>(pWnd)->GetHwnd();
		RECT rectInside;
		if (::GetWindowRect(hwndInside, &rectInside)) {
			tint32 iDiffX = (rectParent.right - rectParent.left) - (rectInside.right - rectInside.left);
			if (sizeMin.iCX > 0)
				sizeMin.iCX += iDiffX;

			tint32 iDiffY = (rectParent.bottom - rectParent.top) - (rectInside.bottom - rectInside.top);
			if (sizeMin.iCY > 0)
				sizeMin.iCY += iDiffY;
		}
	}

	// Set minimum size
	SContextAndClosingBehaviour* pStruct = (SContextAndClosingBehaviour*)GetWindowLong(hwndParent, GWL_USERDATA);
	if (pStruct) {
		pStruct->pointSizeMin.x = sizeMin.iCX;
		pStruct->pointSizeMin.y = sizeMin.iCY;
	}
}

void CContextWin::SetMaxWindowSize(IWindow* pWnd, SSize sizeMax)
{
	// We get the size of client drawing area - we must add the frame width
	// (because restrictions only apply to framed windows)

	// Adjust for frame width
	HWND hwndParent = dynamic_cast<CWindowWin*>(pWnd)->GetHwndParent();
	RECT rectParent;
	if (::GetWindowRect(hwndParent, &rectParent)) {
		HWND hwndInside = dynamic_cast<CWindowWin*>(pWnd)->GetHwnd();
		RECT rectInside;
		if (::GetWindowRect(hwndInside, &rectInside)) {
			tint32 iDiffX = (rectParent.right - rectParent.left) - (rectInside.right - rectInside.left);
			if (sizeMax.iCX > 0)
				sizeMax.iCX += iDiffX;
			else
				sizeMax.iCX = LONG_MAX;

			tint32 iDiffY = (rectParent.bottom - rectParent.top) - (rectInside.bottom - rectInside.top);
			if (sizeMax.iCY > 0)
				sizeMax.iCY += iDiffY;
			else
				sizeMax.iCY = LONG_MAX;
		}
	}

	// Set maximum size
	SContextAndClosingBehaviour* pStruct = (SContextAndClosingBehaviour*)GetWindowLong(hwndParent, GWL_USERDATA);
	if (pStruct) {
		pStruct->pointSizeMax.x = sizeMax.iCX;
		pStruct->pointSizeMax.y = sizeMax.iCY;
	}
}

void CContextWin::WindowClosed(void* pWnd)
{
	if (mpAppCallback) {
		mpAppCallback->WindowClosed(pWnd);
	}
}
