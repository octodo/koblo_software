/*!	\file IWindow.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

// Forward declarations
class IPane;

// Mouse state flag
//! Mouse left button is down
const tint32 MouseStateLeftDown = 1;
//! Mouse right button is down
const tint32 MouseStateRightDown = 2;

/*! \class IWindow
 * \brief Interface to handle windows and generic functionality related thereto
 *
 * A window is what Microsoft calls a 'window', and which is sometimes called a 'pane' on Mac.<br>An IWindow contains 1 IPane, which can have subpanes<br>Controls (IControl) cannot be layed out directly in IWindow, but rather have to be inside a pane (IPane)
*/
class IWindow : public virtual IDestructable, public virtual IInContext, public virtual ISetContext
{
public:
	// Create IWindow
	static IWindow* Create();

	//! Sets the pane. A new pane can be set by calling this 2 times, but the old pane is not destroyed automatically<br>At destruction time any attached pane is destroyed automatically<br>Window will notify pane of change (SetParentWindow())
	/*!
		\param pPane [in]: Pane to set
		\param bResizeToFit [in]: If true, pPane will automatically be resized to window size, both initially and when window size is changed
	*/
	virtual void SetPane(IPane* pPane, tbool bResizeToFit = true) = 0;

	//! Returns the current attached pane
	/*!
		\return IPane: Current attached pane
	*/
	virtual IPane* GetPane() = 0;
	//! \override
	virtual const IPane* GetPane() const = 0;

	//! Returns the current mouse position
	/*!
		\param rPos [out]: Current mouse position
	*/
	virtual void GetMousePos(SPos& rPos) const = 0;

	//! Gets the current mouse state flag
	/*!
		\return tint32: Mouse state flag
	*/
	virtual tint32 GetMouseState() const = 0;

	//! Redraws part of window (invalidates it)
	/*!
		\param rct [in]: Update rectangle
	*/
	virtual void Redraw(const SRect& rct) = 0;
	
	//! Paints window if any part is invalidated
	virtual void Paint() = 0;

	//! Gets parts of window  that needs to be redrawn, or 0, 0, 0, 0 if none. It does not clear any invalidation.
	virtual void GetInvalidatedRect(tint32& riX, tint32& riY, tint32& riCX, tint32& riCY) = 0;

	//! Paints whole window into 32-bit buffer (alpha channel is ignored). This call is synchronous.
	/*!
		\param puchBuffer [out]: Buffer to be filled
		\param uiInterleave [in]: Distance (in bytes) between each row.
		\param rUpdate [in]: Update rectangle
	*/
	virtual void PaintToBuffer(tuchar* puchBuffer, tuint32 uiInterleave, const SRect& rUpdate) = 0;

	//! Create window. Remember to call SetContext() first
	/*!
		\param pParent [in]: System specific. Windows: Parent HWND
		\param pRect [in]: Client rectangle.
	*/
	virtual void MakeWindow(void* pParent, const SRect* pRectF, tbool bInstallEventHandler = true) = 0;

	virtual void PrepareBuffer(const SRect* pRect) = 0;

	//! Returns the window parent.
	/*!
		\return void*: System specific. Windows: Parent HWND. OSX: Windows WindowRef
	*/
	virtual void* GetParent() const = 0;

	//! Shows window
	virtual void ShowWindow() = 0;

	//! Returns system specific variable only valid inside drawing. Windows (GDI): HDC, Windows (DirectX): IDirectDrawSurface*
	/*!
		\param pp [out]: Pointer to object to be returned
	*/
	virtual void GetHDC(void** pp) = 0;

	//! Returns system specific variable. Windows (GDI): Pointer to buffer to draw into, Windows (DirectX): IDirectDraw3*
	/*!
		\param pp [out]: Pointer to object to be returned
	*/
	virtual void GetDrawPointer(void** pp) = 0;

	//! Returns the size of the drawing area ("client rect")
	/*!
		\param rSize [out]: Size of drawing area
	*/
	virtual void GetSize(SSize& rSize) = 0;

	//! Updates the size of the drawing area ("client rect")
	/*!
		\param rSize [in]: Size of drawing area
	*/
	virtual void SetSize(const SSize& rSize) = 0;

	//! Call this to update a control with given ID to new value. Note that the value is the converted value if converted exists (i.e. not a "GUI value")
	/*!
		\param iControlID [in]: ID of control to update
		\param iValueNew [in]: New value to set
		\param bNotify [in]: If true, the controls listeners will be notified, otherwise not
	*/
	virtual void SetControlValue(tint32 iControlID, tint32 iValueNew, tbool bNotify = false) = 0;

	//! Call this to cause all controls to send an EventValueChange()
	virtual void ControlUpdateAll() = 0;

	//! Returns control with given ID, or NULL if none exist
	/*!
		\param iControlID [in]: ID of control to return
		\return iControl*: Control or NULL
	*/
	virtual IControl* GetControl(tint32 iControlID) = 0;

	//! Call this to hide the mouse cursor. Must be matched with a call to ShowMouse(), unless you intentionally wants the cursor to be hidden. Since the calls are matched, 2 calls to HideMouse() followby one call to ShowMouse() will cause the mouse to be hidden
	virtual void HideMouse() = 0;

	//! Call this to show the mouse cursor. For more information see HideMouse()
	virtual void ShowMouse() = 0;

	//! Sets the mouse position
	/*!
		\param Pos [in]: New mouse position to set
	*/
	virtual void SetMousePos(const SPos& Pos) = 0;
	
	//! Call this when mouse button is pressed. (Mac only)
	/*!
		\param Pos [in]: Mouse position
	*/
	virtual void MouseDown(const SPos& Pos) = 0;
	
	//! Call this when key is pressed. (Mac only)
	/*!
		\param iKey [in]: Key code
	*/
	virtual void KeyDown(tint32 iKey) = 0;

	//! Call this to relay mouse messages from the outside into the GUI system
	/*
		\param Msg [in]: Mouse message
		\param Pos [in]: Position of mouse
	*/
	virtual void RelayMouse(EMouseMsg Msg, const SPos& Pos) = 0;

	//! Mouse cursors
	enum EMouseCursor {
		//! Standard cursor
		CursorNormal = 0,
		//! Waiting cursor
		CursorWait,
		//! Pointing hand cursor
		CursorHand,
		//! Left-right arrow
		CursorArrowWE,
		//! Up-down arrow
		CursorArrowNS
	};

	//! Intended to be used by controls. Informs window that control wants the mouse focus. Must be matched with ReleaseMouseFocus(). Derived classes can override this, but should call CWindow::GetMouseFocus (which sets mpMouseFocusControl)
	/*!
		\param pControl [in]: Control which wants the mouse focus
		\param bCapture [in]: If true, the mouse will only react to our window. Should probably only be used for controls which capture the mouse while the button is down, i.e. knobs.
	*/
	virtual void GetMouseFocus(IControl* pControl, tbool bCapture = true) = 0;

	//! Intended to be used by controls. Informs window that control is done with the mouse focus. Must be matched with GetMouseFocus(). Derived classes can override this, but should call CWindow::ReleaseMouseFocus
	virtual void ReleaseMouseFocus() = 0;

	//! Checks if the control or any of its parent panes has the mouse focus
	/*!
		\param pControl [in]: Control to check for. If NULL there's checked to see if any control has focus. In this case a return value of true means that no control has focus.
		\return tbool: True if control or a parent pane has mouse focus, otherwise false
	*/
	virtual tbool HasMouseFocus(const IControl* pControl) = 0;

	//! Sets the mouse cursor (until next mouse move). Can only be called inside mouse handling routines. Cursor is not set before mouse handling routine returns. Should never be called to set cursor to CursorNormal (instead just don't call, CursorNormal is the default).
	virtual void SetMouseCursor(EMouseCursor Cursor) = 0;

	//! Intended to be used by controls. Informs window that control wants to be drawn on top. Must be matched with ControlAlwaysOnTopDone(). Derived classes can override this, but should call CWindow::ControlAlwaysOnTop. Currently only works for controls with no transparency (since they will be drawn twice). Also it probably doesn't work inside an IScrollPane
	/*!
		\param pControl [in]: Control which should be drawn on top
	*/
	virtual void ControlAlwaysOnTop(IControl* pControl) = 0;

	//! Intended to be used by controls. Informs window that control no longer should be on top. Must be matched with ControlAlwaysOnTop(). Derived classes can override this, but should call CWindow::ControlAlwaysOnTopDone
	virtual void ControlAlwaysOnTopDone() = 0;

	//! Prepares window for tool tips. Must be called after the main pane has been set, and all its subpanes has been added.
	/*!
		\param pFont [in]: Font to use. Will be destroyed by window
	*/
	virtual void InitToopTips(IFontResource* pFont) = 0;
	
	//! Timer call
	virtual void OnTimer() = 0;

	//! A modifier key can change a mouse action into a different action
	enum EModifier {
		// No key
		ModifierNone = 0,
		// Control key
		ModifierControl,
		// Shift key
		ModifierShift,
		// Alt key
		ModifierAlt,
		// Windows: Windows key. OSX: Apple key
		ModifierOS
	};

	//! Changes which key (if any) is used for fine adjust
	/*!
		\param Modifier [in]: Modifier used for fine adjust
	*/
	virtual void SetFineAdjustModifier(EModifier Modifier) = 0;

	//! Changes which key (if any) is used for reset
	/*!
		\param Modifier [in]: Modifier used for reset
	*/
	virtual void SetResetModifier(EModifier Modifier) = 0;

	//! Checks if the fine-adjust modifier is pressed (Default: Windows: Ctrl, OSX: Alt)
	virtual tbool FineAdjustPressed() = 0;

	//! Checks if the reset modifier is pressed (Default: Disabled)
	virtual tbool ResetPressed() = 0;

	//! Checks if the control key is pressed
	static tbool ControlPressed();

	//! Checks if the alt key is pressed
	static tbool AltPressed();

	//! Checks if the shift modifier is pressed
	static tbool ShiftPressed();

	//! Checks if the OS specific modifier is pressed (Windows: Windows key. OSX: Apple key)
	static tbool OSPressed();

	//! Installs the keyboard hook (Windows only. Should not be called when running as application)
	virtual void InstallKeyboardHook() = 0;

	//! Can only be called inside OnMouse methods. Means that the current mouse event needs to be send again. Is used by CText when clicked outside control while editing.
	virtual void ResendMouseEvent() = 0;
	
	//! Call this to start accepting keyboard input (must be matched with ReleaseKeyboardInput)
	virtual void AcquireKeyboardInput() = 0;

	//! Call this to end accepting keyboard input (must be matched with AcquireKeybaordInput)
	virtual void ReleaseKeyboardInput() = 0;

	//! Enum of buttons for message boxes
	enum EMsgBoxButtons {
		//! OK
		MsgBoxOK = 0,
		//! OK + Cancel
		MsgBoxOKCancel,
		//! Yes + No
		MsgBoxYesNo,
		//! Yes + No + Cancel
		MsgBoxYesNoCancel,
		//! Retry + Cancel
		MsgBoxRetryCancel
	};

	//! Enum of return values for message boxes
	enum EMsgBoxReturn {
		//! An error occured
		MsgBoxRetError = 0,

		//! OK
		MsgBoxRetOK,

		//! Cancel
		MsgBoxRetCancel,

		//! Yes
		MsgBoxRetYes,

		//! No
		MsgBoxRetNo,

		//! Retry
		MsgBoxRetRetry
	};

	//! Shows a (system) message box
	/*!
		\param pszText [in]: Text of message
		\param pszCaption [in]: Caption of message. Can be NULL, in which case default caption is used (Win32: "Error" or whatever the local language word is. OS X: "Error").
		\param Buttons [in]: Tells which buttons should be in the box. Defaults to a single OK button.
		\return EMsgBoxReturn: Button pressed, or MsgBoxRetError if error
	*/
	static EMsgBoxReturn ShowMessageBox(const tchar* pszMessage, const tchar* pszCaption, EMsgBoxButtons Buttons = MsgBoxOK);
	//virtual EMsgBoxReturn ShowMessageBox(const tchar* pszMessage, const tchar* pszCaption, EMsgBoxButtons Buttons = MsgBoxOK) = 0;

	//! Called by the pane when a control is added
	/*!
		\param iControlID [in]: ID of added control
		\param pControl [in]: Pointer to added control
	*/
	virtual void ControlAdded(tint32 iControlID, IControl* pControl) = 0;

	//! Called by a control to send a MouseDrag event to all controls
	/*!
		\param Pos [in]: Current mouse position, i.e. drag position
	*/
	virtual void OnMouseDrag(const SPos& Pos) = 0;

	//! Called by a control send send a MouseDrop event to all controls
	/*!
		\param Pos [in]: Current mouse position, i.e. drop position
	*/
	virtual void OnMouseDrop(const SPos& Pos) = 0;

	//! Struct to hold info about current Drag and drop
	struct SDragAndDropInfo {
		//! Origin of drag and drop
		ge::IControl* pOrigin;

		//! Text. Control specific.
		tchar psz[2048];

		//! Value. Control specific.
		tint32 miValue;

		SDragAndDropInfo(tint32 iValue) { miValue = iValue; };
		SDragAndDropInfo() { miValue = -1; };
	};

	//! Sets the current drag 'n drop info. Should be called by control which initiates drag and drop. Can be changed (called again) while dragging (if for instance modifier key state changes).
	/*!
		\param Info [in]: New Drag 'n drop info
	*/
	virtual void SetDragAndDropInfo(const SDragAndDropInfo& Info) = 0;

	//! Gets the current drag 'n drop info. Should be called by receiver of drag and drop after drop.
	/*!
		\param Info [out]: Drag 'n drop info
	*/
	virtual void GetDragAndDropInfo(SDragAndDropInfo& Info) = 0;
};



