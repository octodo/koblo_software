/*!	\file CWindowWin.cpp
	\author Michael Olsen
	\date 30/Dec/2004
	\date 30/Dec/2004
*/


#include "geInternalOS.h"

//! Critical section for keyboard hooks (global)
CMutex* gpHookMutex = NULL;
//! Pointer to keyboard filter proc hook
HHOOK gpKeyboardFilterProcHook = NULL;
//! Keyboard hook proc
LRESULT CALLBACK KeyboardFilterProc(int nCode, WPARAM wParam, LPARAM lParam);
//! List of HWND's with hooks
std::list<HWND> gHookList;

CWindowWin::CWindowWin() : mhWnd(NULL), mhWndParent(NULL),
	mbKeyboardAcquired(false), mbMouseCaptured(false)
{
	mModifierFineAdjust = ModifierControl;
	mModifierReset = ModifierNone;
}

CWindowWin::~CWindowWin()
{
	// The mutex may be NULL (if we have no hook)
	if (gpHookMutex != NULL) {
		tbool bErasedOne = false;
		{
			CAutoLock Lock(*gpHookMutex);

			std::list<HWND>::iterator it = gHookList.begin();
			// Search for the window in the hook list
			for (; it != gHookList.end(); it++) {
				if ((*it) == mhWnd) {
					// Found it
					gHookList.erase(it);
					bErasedOne = true;
					break;
				}
			}
		}

		if (bErasedOne) {
			// If we found it, and...
			if (gHookList.empty()) {
				// ... the list now is empty, remove the hook and clean up
				UnhookWindowsHookEx(gpKeyboardFilterProcHook);
				gpKeyboardFilterProcHook = NULL;

				// Also clean up the mutex
				delete gpHookMutex;
				gpHookMutex = NULL;
			}
		}
	}
}

void CWindowWin::GetMouseFocus(IControl* pControl, tbool bCapture)
{
	CWindow::GetMouseFocus(pControl);

	ASSERT(mbMouseCaptured == false);
	if (bCapture) {
		::SetCapture(mhWnd);

		mbMouseCaptured = true;
	}
}

void CWindowWin::ReleaseMouseFocus()
{
	if (mbMouseCaptured) {
		::ReleaseCapture();

		mbMouseCaptured = false;
	}

	CWindow::ReleaseMouseFocus();
}

void CWindowWin::HideMouse()
{
	::ShowCursor(FALSE);
}

void CWindowWin::ShowMouse()
{
	::ShowCursor(TRUE);
}

void CWindowWin::DoSetMouseCursor(EMouseCursor Cursor)
{
	HCURSOR hCursor;
	switch(Cursor) {
		case CursorNormal:
			hCursor = ::LoadCursor(NULL, IDC_ARROW);
			break;
		case CursorWait:
			hCursor = ::LoadCursor(NULL, IDC_WAIT);
			break;
		case CursorHand:
			// MAKEINTRESOURCE(32649) == IDC_HAND
			hCursor = ::LoadCursor(NULL, MAKEINTRESOURCE(32649));
			break;
		case CursorArrowWE:
			hCursor = ::LoadCursor(NULL, IDC_SIZEWE);
			break;
		case CursorArrowNS:
			hCursor = ::LoadCursor(NULL, IDC_SIZENS);
			break;
		default:
			debug_break;
	}

	::SetCursor(hCursor);
}

void CWindowWin::SetMousePos(const SPos& Pos)
{
	// Convert coordinates to screen rects, which SetCursorPos needs
	POINT pnt;
	pnt.x = Pos.iX;
	pnt.y = Pos.iY;
	::ClientToScreen(mhWnd, &pnt);

	::SetCursorPos(pnt.x, pnt.y);
}

HWND CWindowWin::GetHwnd() const
{
	return mhWnd;
}

HWND CWindowWin::GetHwndParent() const
{
	return mhWndParent;
}

void CWindowWin::MakeWindow(void* pParent, const SRect* pRect)
{
	mhWndParent = (HWND)pParent;

	DWORD dwStyle = WS_OVERLAPPED | WS_CHILD;
	DWORD dwExStyle = 0;

	SRect Rect;
	Rect.iX = CW_USEDEFAULT;
	Rect.iCX = CW_USEDEFAULT;
	if (pRect != NULL) {
		Rect = *pRect;
	}
	if ((Rect.iCX == 0) || (Rect.iCY == 0)) {
		Rect.iX = CW_USEDEFAULT;
		Rect.iCY = CW_USEDEFAULT;
	}

	std::string s;
	dynamic_cast<CContextWin*>(GetContext())->GetClassName(s);

	mhWnd = CreateWindowEx(dwExStyle,
		s.c_str(),
		NULL,
		dwStyle,
		Rect.iX, Rect.iY, Rect.iCX, Rect.iCY,
		mhWndParent,
		NULL,
		(HINSTANCE)(GetContext()->GetInstanceData()),
		(LPVOID)dynamic_cast<IWindow*>(this));

	::SetTimer(mhWnd, 1, 50, NULL);

	DoSetMouseCursor(CursorNormal);
}

void CWindowWin::ShowWindow()
{
	::ShowWindow(mhWnd, SW_SHOWNORMAL);
}

void CWindowWin::GetSize(SSize& rSize)
{
	RECT rect;
	GetClientRect(mhWnd, &rect);

	rSize = SSize(SRect(rect));

	// Apparently we need to compensate, since Windows gave us the correct size (which it normally doesn't)
	rSize -= SSize(1, 1);
}


IWindow::EMsgBoxReturn IWindow::ShowMessageBox(const tchar* pszMessage, const tchar* pszCaption, IWindow::EMsgBoxButtons Buttons)
{
	tint32 iRet = 0;
	switch(Buttons) {
		case MsgBoxOK:
			iRet = ::MessageBox(NULL, (LPCTSTR)pszMessage, (LPCTSTR)pszCaption, MB_TASKMODAL | MB_TOPMOST | MB_OK);
			break;

		case MsgBoxOKCancel:
			iRet = ::MessageBox(NULL, (LPCTSTR)pszMessage, (LPCTSTR)pszCaption, MB_TASKMODAL | MB_TOPMOST | MB_OKCANCEL);
			break;

		case MsgBoxYesNo:
			iRet = ::MessageBox(NULL, (LPCTSTR)pszMessage, (LPCTSTR)pszCaption, MB_TASKMODAL | MB_TOPMOST | MB_YESNO);
			break;

		case MsgBoxYesNoCancel:
			iRet = ::MessageBox(NULL, (LPCTSTR)pszMessage, (LPCTSTR)pszCaption, MB_TASKMODAL | MB_TOPMOST | MB_YESNOCANCEL);
			break;

		case MsgBoxRetryCancel:
			iRet = ::MessageBox(NULL, (LPCTSTR)pszMessage, (LPCTSTR)pszCaption, MB_TASKMODAL | MB_TOPMOST | MB_RETRYCANCEL);
			break;

		default:
			throw IException::Create(IException::TypeCode, IException::ReasonCodePathInvalid, EXCEPTION_INFO, (tchar*)"Message box type not implemented");
	}

	switch(iRet) {
		case IDOK:
			return IWindow::MsgBoxRetOK;

		case IDCANCEL:
			return IWindow::MsgBoxRetCancel;

		case IDYES:
			return IWindow::MsgBoxRetYes;

		case IDNO:
			return IWindow::MsgBoxRetNo;

		case IDRETRY:
			return IWindow::MsgBoxRetRetry;

		default:
			return IWindow::MsgBoxRetError;
	}
} // ShowMessageBox


tbool IWindow::ControlPressed()
{
	return ((GetKeyState(VK_CONTROL) & 0x80) != 0);
}

tbool IWindow::ShiftPressed()
{
	return ((GetKeyState(VK_SHIFT) & 0x80) != 0);
}

tbool IWindow::AltPressed()
{
	return ((GetKeyState(VK_MENU) & 0x80) != 0);
}

tbool IWindow::OSPressed()
{
	return (((GetKeyState(VK_LWIN) & 0x80) != 0) ||
		((GetKeyState(VK_RWIN) & 0x80) != 0));
}

void CWindowWin::AcquireKeyboardInput()
{
	mbKeyboardAcquired = true;
}

void CWindowWin::ReleaseKeyboardInput()
{
	mbKeyboardAcquired = false;
}

void CWindowWin::InstallKeyboardHook()
{
	if (gpHookMutex == NULL) {
		// Mutex is NULL, so create it
		gpHookMutex = new CMutex();
	}

	ASSERT(gpHookMutex != NULL);
	CAutoLock Lock(*gpHookMutex);

	if (gpKeyboardFilterProcHook == NULL) {
		// This is our first instance, so install hook
		gpKeyboardFilterProcHook = ::SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KeyboardFilterProc, NULL, GetWindowThreadProcessId(mhWnd, NULL));
	}

	// Put our hwnd in hook list
	gHookList.push_back(mhWnd);
}


// Lasse 2007-05-09: Moved here (used to be part of CWindowWin::WndProc(..) )
EKey XlatCharToEKey(tchar c)
{
	EKey Key = (EKey)-1;
	switch (c) {
		case ' ':	Key = KeySpace;					break;
		case '!':	Key = KeyExclamation;			break;
		case '"':	Key = KeyQuotation;				break;
		case '#':	Key = KeyPound;					break;
		case '$':	Key = KeyDollar;				break;
		case '%':	Key = KeyPercent;				break;
		case '&':	Key = KeyAnd;					break;
		case '(':	Key = KeyBracketLeft;			break;
		case ')':	Key = KeyBracketRight;			break;
		case '*':	Key = KeyMul;					break;
		case '+':	Key = KeyPlus;					break;
		case ',':	Key = KeyComma;					break;
		case '-':	Key = KeyMinus;					break;
		case '.':	Key = KeyDot;					break;
		case '/':	Key = KeySlash;					break;
		case ':':	Key = KeyColon;					break;
		case ';':	Key = KeySemiColon;				break;
		case '<':	Key = KeySmaller;				break;
		case '=':	Key = KeyEqual;					break;
		case '>':	Key = KeyLarger;				break;
		case '?':	Key = KeyQuestion;				break;
		case '@':	Key = KeyAdd;					break;
		case '[':	Key = KeySquaredBrackedLeft;	break;
		case '\\':	Key = KeyBackSlash;				break;
		case ']':	Key = KeySquaredBrackedRight;	break;
		case '^':	Key = KeyHat;					break;
		case '_':	Key = KeyUnderLine;				break;
		case '`':	Key = KeyNote2;					break;
		case '{':	Key = KeyCurlyBrackedLeft;		break;
		case '|':	Key = KeyLine;					break;
		case '}':	Key = KeyCurlyBrackedRight;		break;
		case '~':	Key = KeyTilde;					break;
		case '\'':	Key = KeyNote;					break;

		default:
			{
				if ((c>='0')&&(c<='9')) {
					Key = (EKey)(Key0 + (c - '0'));
				}
				else if ((c>='A')&&(c<='Z')) {
					Key = (EKey)(KeyA + (c - 'A'));
				}
				else if ((c>='a')&&(c<='z')) {
				//	tbool bCapsLock = ((GetKeyState(VK_CAPITAL) & 0x01) != 0);
				//	if(bCapsLock)
				//	Key = (EKey)(KeyA + (c - 'a'));
				//	else
						Key = (EKey)(Keya + (c - 'a'));
				}
				else if ((c>=1)&&(c<=26)) /* Ctrl+A -> Ctrl+Z */ {
					tbool bCapsLock = ((GetKeyState(VK_CAPITAL) & 0x01) != 0);
					tbool bShift = ((GetKeyState(VK_SHIFT) & 0x8000) != 0); //ModifierPressed(ModifierShift);
					tbool bUpper = (bCapsLock^bShift);
					if (bUpper) {
						Key = (EKey)(KeyA + (c - 1));
					}
					else {
						Key = (EKey)(Keya + (c - 1));
					}
				}
			}
			break;
	}

	return Key;
} // XlatCharToEKey
// .. Lasse


#define LOSWORD(l)           ((short)(l))
#define HISWORD(l)           ((short)(((long)(l) >> 16) & 0xFFFF))

LRESULT CWindowWin::WndProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam, tbool& rbProcessed)
{
	LRESULT	rcode = -1;

	switch (uiMsg) {
		case WM_COMMAND:
			{
				// Can this cause something to not get painted?
				mpInvalidater->Reset();
			}
			break;
		case WM_ERASEBKGND:
			rbProcessed = true;
			rcode = 0;
			break;
		case WM_PAINT:
			{
				OnPaint();

				mpInvalidater->Reset();

				rbProcessed = true;
				rcode = 0;
				break;
			}
		case WM_DESTROY:
			rbProcessed = true;
			rcode = 0;
			break;
		case WM_KILLFOCUS:
			miMouseStateCur = 0;
			rbProcessed = true;
			rcode = 0;
			break;
		case WM_LBUTTONDOWN:
			// (lasse) Crash-fix 2008-12-01
			if (miMouseStateCur & MouseStateLeftDown) {
				// Chaos! We have two left button clicks!
				// We MUST release any previous mouse focus in order not to crash
				// Note: This is quite hard-core and may lead to unexpected behavior... fix later
				ReleaseMouseFocus();
			}
			// .. (lasse)			
			miMouseStateCur |= MouseStateLeftDown;
			if (OnMouse(LeftButtonDown, SPos(LOSWORD(lParam), HISWORD(lParam))) == true) {
				rbProcessed = true;
				rcode = 0;
			}
			break;
		case WM_LBUTTONUP:
			miMouseStateCur &= (~MouseStateLeftDown);
			if (OnMouse(LeftButtonUp, SPos(LOSWORD(lParam), HISWORD(lParam))) == true) {
				rbProcessed = true;
				rcode = 0;
			}
			break;
		case WM_LBUTTONDBLCLK:
			if (OnMouse(LeftButtonDblClk, SPos(LOSWORD(lParam), HISWORD(lParam))) == true) {
				rbProcessed = true;
				rcode = 0;
			}
			break;
		case WM_RBUTTONDOWN:
			miMouseStateCur |= MouseStateRightDown;
			if (OnMouse(RightButtonDown, SPos(LOSWORD(lParam), HISWORD(lParam))) == true) {
				rbProcessed = true;
				rcode = 0;
			}
			break;
		case WM_RBUTTONUP:
			miMouseStateCur &= (~MouseStateRightDown);
			if (OnMouse(RightButtonUp, SPos(LOSWORD(lParam), HISWORD(lParam))) == true) {
				rbProcessed = true;
				rcode = 0;
			}
			break;
		case WM_RBUTTONDBLCLK:
			if (OnMouse(RightButtonDblClk, SPos(LOSWORD(lParam), HISWORD(lParam))) == true) {
				rbProcessed = true;
				rcode = 0;
			}
			break;
		case WM_MOUSEMOVE:
			{
				SPos PosMouse = SPos(LOSWORD(lParam), HISWORD(lParam));
				if (OnMouse(MouseMove, PosMouse) == true) {
					rbProcessed = true;
					rcode = 0;
				}
			}
			break;
		case WM_KEYDOWN:
			{
/*				if (OnKeyDown((int)wParam) == true) {
					rbProcessed = true;
					rcode = 0;
				}*/
			}
			break;
		case WM_TIMER:
			OnTimer();
			rbProcessed = true;
			rcode = 0;
			break;
		case WM_SIZE:
			{
				if ((int)wParam == SIZE_MAXIMIZED ||(int)wParam == SIZE_RESTORED) {
					if (mbResizePaneToFit) {
						RECT rect;
						GetClientRect(hwnd, &rect);
						mpPane->SetSize(SSize(rect.right - rect.left + 1, rect.bottom - rect.top + 1));
					}
				}
				rbProcessed = true;
				rcode = 0;
			}
			break;

		case WM_SETCURSOR:
			DoSetMouseCursor(mMouseCursorCur);
			rbProcessed = true;
			rcode = 1;
			break;

		// Lasse: Added 2006-09-18:
		case WM_CHAR:
			{
				IWindow* pWindow = (IWindow*)::GetWindowLong(hwnd, GWL_USERDATA);
				if (pWindow) {
					if (dynamic_cast<CWindowWin*>(pWindow)->IsKeyboardAcquired()) {
						TCHAR c = (TCHAR)wParam;
						EKey Key = XlatCharToEKey(c);

						if (Key != (EKey)-1) {
							if (dynamic_cast<CWindow*>(pWindow)->OnKeyDown(Key)) {
								rbProcessed = true;
								rcode = 0;
							}
						}
					}
				}
			}
			break;
		// .. Lasse

	};

	{
		if (mpInvalidater->IsRectInvalidated()) {
			SRect rct;
			mpInvalidater->GetInvalidatedRect(rct);
			::InvalidateRect(GetHwnd(), &RECT(rct), false);
		}
	}

	return rcode;
}

LRESULT CALLBACK KeyboardFilterProc(int nCode, WPARAM wParam, LPARAM lParam) 
{
	ASSERT(gpHookMutex != NULL);
	CAutoLock Lock(*gpHookMutex);

	long hi_lParam;

	if (nCode < HC_ACTION)
		return CallNextHookEx(gpKeyboardFilterProcHook, nCode, wParam, lParam);

	hi_lParam = HIWORD(lParam);

	if (!(hi_lParam & KF_UP) && !(hi_lParam & KF_MENUMODE) && LOWORD(lParam) == 1) {
		std::list<HWND>::iterator it;
		bool bActionDone = false;
		for (it = gHookList.begin(); (it != gHookList.end()) && (bActionDone == false); it++) {
			HWND hwnd = (*it);

			if (IsWindow(hwnd)) {
				if (IsChild(GetForegroundWindow(), hwnd)) {
					IWindow* pWindow = (IWindow*)::GetWindowLong(hwnd, GWL_USERDATA);
					if (pWindow) {
						if (dynamic_cast<CWindowWin*>(pWindow)->IsKeyboardAcquired()) {
							// Convert the key
							int iKey = LOWORD(wParam);

							// Set it to non-valid value
							EKey Key = (EKey)-1;

							// Lasse: Rewritten 2006-09-18:
							// Max updated 2007-02-21
							tbool bCaps = dynamic_cast<CWindowWin*>(pWindow)->ShiftPressed();
							if(!bCaps)
								bCaps = ((GetKeyState(VK_CAPITAL) & 0x01) != 0);

							switch(iKey) {
								// Lasse - added 2007-05-22
								case VK_F4: {
									tbool bAlt = dynamic_cast<CWindowWin*>(pWindow)->AltPressed();
									if (bAlt) {
										// Alt+F4 pressed - abort editing
										Key = KeyEscape;
										if (dynamic_cast<CWindow*>(pWindow)->OnKeyDown(Key)) {
											// ...
										}
									}
								}
								break;
								// .. Lasse

								case 37: /* Left arrow */	Key = KeyLeft;		break;
								case 39: /* Right arrow */	Key = KeyRight;		break;
								case 38: /* Up arrow */		Key = KeyUp;		break;
								case 40: /* Down arrow */	Key = KeyDown;		break;
								case 33: /* PgUp, Ctrl+PgUp */ {
									Key = (pWindow->FineAdjustPressed()) ? Key = KeyUp : KeyBigUp;
									break;
								}
								case 34: /* PgDn, Ctrl+PgDn */ {
									Key = (pWindow->FineAdjustPressed()) ? Key = KeyDown : KeyBigDown;
									break;
								}
								case 36: /* Home, Ctrl+Home */ {
									Key = (pWindow->FineAdjustPressed()) ? KeyBigHome : KeyHome;
									break;
								}
								case 35: /* End, Ctrl+End*/ {
									Key = (pWindow->FineAdjustPressed()) ? KeyBigEnd : KeyEnd;
									break;
								}
								case 13: /* Enter, Ctrl+Enter, Alt+Enter, Shift+Enter */ {
									Key = KeyReturn;
									break;
								}
								case 27: /* Escape, Ctrl+Escape, Alt+Escape, Shift+Escape */ {
									Key = KeyEscape;
									break;
								}
								case 46: /* Delete, Ctrl+Delete, Alt+Delete, Shift+Delete */ {
									Key = KeyDelete;
									break;
								}
								case 8: /* BackSpace, Ctrl+BackSpace, Alt+BackSpace, Shift+BackSpace */ {
									Key = KeyBackSpace;
									break;
								}
								case 9: /* Tab, Ctrl+Tab, Alt+Tab, Shift+Tab */ {
									// Ignore (for now)
									break;
								}
								// Numbers above QWERTY and Numeric Keypad
								case 96:	Key = Key0;		break;
								case 97:	Key = Key1;		break;
								case 98:	Key = Key2;		break;
								case 99:	Key = Key3;		break;
								case 100:	Key = Key4;		break;
								case 101:	Key = Key5;		break;
								case 102:	Key = Key6;		break;
								case 103:	Key = Key7;		break;
								case 104:	Key = Key8;		break;
								case 105:	Key = Key9;		break;
								//case 188:	Key = KeyComma; break;
								case 110:	Key = KeyComma; break;
								// Numebers
							/*	case 48:// 0 //		Key = Key0;		break;
								case 49:// 1 //		Key = Key1;		break;
								case 50:// 2 //		Key = Key2;		break;
								case 51:// 3 //		Key = Key3;		break;
								case 52:// 4 //		Key = Key4;		break;
								case 53:// 5 //		Key = Key5;		break;
								case 54:// 6 //		Key = Key6;		break;
								case 55:// 7 //		Key = Key7;		break;
								case 56:// 8 //		Key = Key8;		break;
								case 57:// 9 //		Key = Key9;		break;

								case 188:
								case 110:// "," //	Key = KeyComma; break;
								case 190:// "." //  Key = KeyDot;	break;
							*/
								// Letters
								case 65:	bCaps ? Key = KeyA:		Key = Keya;	break;
								case 66:	bCaps ? Key = KeyB:		Key = Keyb;	break;
								case 67:	bCaps ? Key = KeyC:		Key = Keyc;	break;
								case 68:	bCaps ? Key = KeyD:		Key = Keyd;	break;
								case 69:	bCaps ? Key = KeyE:		Key = Keye;	break;
								case 70:	bCaps ? Key = KeyF:		Key = Keyf;	break;
								case 71:	bCaps ? Key = KeyG:		Key = Keyg;	break;
								case 72:	bCaps ? Key = KeyH:		Key = Keyh;	break;
								case 73:	bCaps ? Key = KeyI:		Key = Keyi;	break;
								case 74:	bCaps ? Key = KeyJ:		Key = Keyj;	break;
								case 75:	bCaps ? Key = KeyK:		Key = Keyk;	break;
								case 76:	bCaps ? Key = KeyL:		Key = Keyl;	break;
								case 77:	bCaps ? Key = KeyM:		Key = Keym;	break;
								case 78:	bCaps ? Key = KeyN:		Key = Keyn;	break;
								case 79:	bCaps ? Key = KeyO:		Key = Keyo;	break;
								case 80:	bCaps ? Key = KeyP:		Key = Keyp;	break;
								case 81:	bCaps ? Key = KeyQ:		Key = Keyq;	break;
								case 82:	bCaps ? Key = KeyR:		Key = Keyr;	break;
								case 83:	bCaps ? Key = KeyS:		Key = Keys;	break;
								case 84:	bCaps ? Key = KeyT:		Key = Keyt;	break;
								case 85:	bCaps ? Key = KeyU:		Key = Keyu;	break;
								case 86:	bCaps ? Key = KeyV:		Key = Keyv;	break;
								case 87:	bCaps ? Key = KeyW:		Key = Keyw;	break;
								case 88:	bCaps ? Key = KeyX:		Key = Keyx;	break;
								case 89:	bCaps ? Key = KeyY:		Key = Keyy;	break;
								case 90:	bCaps ? Key = KeyZ:		Key = Keyz;	break;
								// Aretmic
								case 106: Key = KeyMul;		break;
								case 107: Key = KeyPlus;	break;
								case 109: Key = KeyMinus;	break;
								case 111: Key = KeySlash;	break;
								
									
								
								default : {
									// Throw message directly into Windows sub-system for translation.

									// This will allow '@', '£', '$' chars etc to work correctly for
									// non-US keyboard layouts.
									// We will do this almost no matter the state of Ctrl or Alt keys, since
									// these often have special meaning on non-US keyboard layouts. If
									// something weird happens it will be filtered in the receiving
									// message handler: CWindowWin::WndProc(..).

									// Lasse - rem'ed out 2007-05-22
									//tbool bAlt = dynamic_cast<CWindowWin*>(pWindow)->AltPressed();
									// .. Lasse
									tbool bCtrl = dynamic_cast<CWindowWin*>(pWindow)->ControlPressed();
									// Lasse - rem'ed out 2007-05-22
									//if ((bAlt)&&(!bCtrl)) {
									//	// This is for instance Alt+F4 - we shouldn't trap that!
									//	if (iKey==18) {
									//		// The Alt key itself
									//		Key = KeyEscape;
									//	}
									//	else {
									//		// Do nothing... we should never get here!
									//	}
									//}
									//else {
									{
										if (!bCtrl || iKey != 86) {
											// (mo) We don't want to be here if the ctrl+v is pressed (paste)
											MSG msg;
											msg.hwnd = hwnd;
											msg.message = WM_KEYDOWN;
											msg.wParam = wParam;
											msg.lParam = lParam;
											// Init time (doesn't matter here)
											msg.time = 0;
											// Init mouse position x and y (doesn't matter here)
											msg.pt.x = 0;
											msg.pt.y = 0;
											// Lasse - rewritten 2007-05-22
											if (TranslateMessage(&msg)) {
												// Try to get the message back from Windows immediately
												if (PeekMessage(&msg, hwnd, WM_CHAR, WM_CHAR, PM_REMOVE)) {
													// We got the message, and it was removed from queue (PM_REMOVE),
													// so we will never catch it in CWindowWin::WndProc(...).

													// Translate to our own key set
													TCHAR c = (TCHAR)msg.wParam;
													EKey Key = XlatCharToEKey(c);

													if (Key != (EKey)-1) {
														if (dynamic_cast<CWindow*>(pWindow)->OnKeyDown(Key)) {
															return true;
															//rbProcessed = true;
															//rcode = 0;
														}
													}
												}
												//DispatchMessage(&msg);
												//return true;
											}
											// .. Lasse
										}
									}
									break;
								} // default
							}
							// .. Lasse tel 28552397
							tbool bDone = false;
							if (dynamic_cast<CWindowWin*>(pWindow)->ControlPressed()) {
//								if (Key == Keyv) {
								if (iKey == 86) {
									// Ctrl-v pressed, i.e. paste
									if (::OpenClipboard(NULL)) {
										// Request a pointer to the text on the clipboard.
										HGLOBAL hGlobal = ::GetClipboardData(CF_TEXT);

										if (hGlobal != NULL) {
											// We have some text
											LPSTR lpszData = (LPSTR)::GlobalLock(hGlobal);

											tint32 iSize = ::GlobalSize(hGlobal);

											for (tint32 iIndex = 0; iIndex < iSize; iIndex++) {
												tint32 iKey = lpszData[iIndex];

												// Lasse - rewritten 2007-05-09
												/*
												EKey Key = (EKey)-1;
												if ((iKey >= 'a') && (iKey <= 'z')) {
													Key = (EKey)(Keya + (iKey - 'a'));
												}
												else if ((iKey >= '0') && (iKey <= '9')) {
													Key = (EKey)(Key0 + (iKey - '0'));
												}
												else if ((iKey >= 'A') && (iKey <= 'Z')) {
													Key = (EKey)(KeyA + (iKey - 'A'));
												}

												if (Key != (EKey)-1) {
													if (dynamic_cast<CWindow*>(pWindow)->OnKeyDown(Key)) {
														bActionDone = true;
													}
												}
												*/
												// We need to allow for space and all other chars.
												// To avoid re-inventing the soup-plate we simply throw 
												// the chars into the already existing handler:
												EKey Key = XlatCharToEKey(iKey);
												if (Key != (EKey)-1) {
													if (dynamic_cast<CWindow*>(pWindow)->OnKeyDown(Key)) {
														bActionDone = true;
													}
												}
												// .. Lasse
											}
											::GlobalUnlock(hGlobal);
										}
										::CloseClipboard();
								}
									bDone = true;
								}
							}
							if (bDone == false) {
								if (Key != (EKey)-1) {
									if (dynamic_cast<CWindow*>(pWindow)->OnKeyDown(Key)) {
										bActionDone = true;
									}
								}
							}
						}
					}
				}
			}
		}

		if (bActionDone) {
			return TRUE;
		}
	}

	return CallNextHookEx(gpKeyboardFilterProcHook, nCode, wParam, lParam);
}

void CWindowWin::PaintToBuffer(tuchar* puchBuffer, tuint32 uiInterleave, const SRect& rUpdate)
{
	throw IException::Create(IException::TypeCode, IException::ReasonCodePathInvalid, EXCEPTION_INFO, "Not implemented");
}

void CWindowWin::PrepareBuffer(const SRect* pRect)
{
	throw IException::Create(IException::TypeCode, IException::ReasonCodePathInvalid, EXCEPTION_INFO, "Not implemented");
}

void* CWindowWin::GetParent() const
{
	return (void*)mhWndParent;
}

void CWindowWin::SetSize(const SSize& rSize)
{
	::SetWindowPos(GetHwnd(), 0, 0, 0, rSize.iCX, rSize.iCY, SWP_NOMOVE | SWP_NOZORDER);
}



