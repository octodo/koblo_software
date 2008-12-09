/*!	\file CWindowWin.h
	\author Michael Olsen
	\date 30/Dec/2004
	\date 30/Dec/2004
*/

/*! \class CWindowWin
 * \brief Windows specific part of IWindow implementation
*/
class CWindowWin : public virtual CWindow
{
public:
	//! Constructor
	CWindowWin();
	//! Destructor
	virtual ~CWindowWin();

	//! IWindow override
	virtual void MakeWindow(void* pParent, const SRect* pRect);
	//! IWindow override
	virtual void ShowWindow();
	//! IWindow override
	virtual void GetSize(SSize& rSize);
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
	//! IWindow override
	virtual void ReleaseMouseFocus();
	//! IWindow override
	virtual void PaintToBuffer(tuchar* puchBuffer, tuint32 uiInterleave, const SRect& rUpdate);
	//! IWindow override
	virtual void PrepareBuffer(const SRect* pRect);
	//! IWindow override
	virtual void* GetParent() const;
	//! IWindow override
	virtual void SetSize(const SSize& rSize);
	//! CWindow override
	virtual void GetMouseFocus(IControl* pControl, tbool bCapture = true);
	//! CWindow override

	//! Handles window messages
	/*!
		\param hwnd [in]: Standard Windows message parameter
		\param uiMsg [in]: Standard Windows message parameter
		\param wParam [in]: Standard Windows message parameter
		\param lParam [in]: Standard Windows message parameter
		\param rbProcessed [out]: If message has been processed true, false otherwise
		\return LRESULT: Standard Windows return value
	*/
	virtual LRESULT WndProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam, tbool& rbProcessed);

	//! Returns true if the keyboard is currently "acquired" (in use by a control)
	/*!
		\return True if the keyboard is currently "acquired" (in use by a control), otherwise false
	*/
	tbool IsKeyboardAcquired() const {return mbKeyboardAcquired;}

	//! Returns window handle
	/*!
		\return HWND: Window handle
	*/
	HWND GetHwnd() const;

	//! Returns parent window handle
	/*!
		\return HWND: Parent window handle
	*/
	HWND GetHwndParent() const;

protected:
	//! Window handle
	HWND mhWnd;
	//! Parent window handle
	HWND mhWndParent;

	//! If true the keyboard is current "acquired" (in use by a control), otherwise not. This is mainly to keep compability with the mac version.
	tbool mbKeyboardAcquired;

	//! If true, the mouse is captured
	tbool mbMouseCaptured;

	//! CWindow override
	virtual void DoSetMouseCursor(EMouseCursor Cursor);
};



