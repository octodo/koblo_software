/*!	\file CWindowOSX.h
	\author Michael Olsen
	\date 10/Mar/2005
	\date 10/Mar/2005
*/

/*! \class CWindowOSX
 * \brief OSX specific part of IWindow implementation
*/
class CWindowOSX : public virtual CWindow
{
public:
	//! Constructor
	CWindowOSX();
	//! Destructor
	virtual ~CWindowOSX();

	//! IWindow override
	virtual void MakeWindow(void* pParent, const SRect* pRect, tbool bInstallEventHandler = true);
	//! IWindow override
	virtual void PrepareBuffer(const SRect* pRect);
	//! IWindow override
	void* GetParent() const;
	//! IWindow override
	virtual void ShowWindow();
	//! IWindow override
	virtual void GetSize(SSize& rSize);
	//! IWindow override
	virtual void SetSize(const SSize& rSize);
	//! IWindow override
	virtual void HideMouse();
	//! IWindow override
	virtual void ShowMouse();
	//! IWindow override
	virtual void SetMousePos(const SPos& Pos);
	////! IWindow override
	//virtual tbool ControlPressed();
	////! IWindow override
	//virtual tbool ShiftPressed();
	////! IWindow override
	//virtual tbool AltPressed();
	////! IWindow override
	//virtual tbool OSPressed();
	//! IWindow override
	virtual void InstallKeyboardHook();
	//! IWindow override
	virtual void AcquireKeyboardInput();
	//! IWindow override
	virtual void ReleaseKeyboardInput();
//	//! IWindow override
//	virtual EMsgBoxReturn ShowMessageBox(const tchar* pszMessage, const tchar* pszCaption, EMsgBoxButtons Buttons = MsgBoxOK);
	//! CWindow override
	virtual void GetMouseFocus(IControl* pControl);
	//! CWindow override
	virtual void ReleaseMouseFocus();
	//! CWindow override
	virtual void MouseDown(const SPos& Pos);
	//! CWindow override
	virtual void KeyDown(tint32 iKey);
	//! CWindow override
	virtual void OnTimer();

	//! Called from mouse event
	virtual void OnMouseHook(EMouseMsg Msg, const SPos& Pos);

	//! Returns window ref
	/*!
		\return WindowRef: Window ref
	*/
	WindowRef GetWindowRef() const;
	
	//! Ask if mouse is currently pressed
	/*!
		\return tbool: If mouse is currently pressed true, otherwise false
	*/
	virtual tbool IsMousePressed();
	
	//! This is where we actually set the mouse cursor
	virtual void SetCurrentMouseCursor();

	void Shown(tbool bShown);

protected:
	//! Size of window
	SSize mSize;

	//! Original (parent) window ref
	WindowRef mWindowRefParent;

	//! Window ref (sub window)
	WindowRef mWindowRef;
	
	//! Window group
	WindowGroupRef mWindowGroup;

	//! Window ref
//	WindowRef mWindowRef;
	
	//! Cursor to set on next kEventWindowCursorChange event
	EMouseCursor mCursorCur;

	//! CWindow override
	virtual void DoSetMouseCursor(EMouseCursor Cursor);

	//! Time for last click (used for determining double clicks)
	tuint32 muiTimeClickLast;

	//! Position for last click (used for determining double clicks)
	SPos mPosLastClick;

	//! If true the mouse was inside our window in the last timer call, otherwise false
//	tbool mbMouseInsideWindowWhenTimerCalled;

	//! Handle for keyboard events.
	EventHandlerRef mKeyboardRef;
	
	//! Handle for mouse events.
	EventHandlerRef mMouseRef;
	
	//! Handle for window events.
	EventHandlerRef mWindowEventRef;
	
	//! Handle to wait cursor (system cursor)
	CursHandle mhCursorWatch;

	//! WE (left-right) cursor
	Cursor mCursorWE;

	//! NS (up-down) cursor
	Cursor mCursorNS;

	//! Hand (pointing) cursor
	Cursor mCursorHand;

	//! Timer counter (we don't react on every idle/timer call)
	int miTimerCounter;

	//! If true, we're drawing into a buffer, instead of onto a window
	tbool mbBufferDrawing;
};



