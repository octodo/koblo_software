/*!	\file IControl.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

// Forward declarations
class IPane;

/*! \class IControl
 * \brief A control is a knob, a slider or the like. Also known as a 'widget'
*/
class IControl : public virtual IDestructable, public virtual IInContext
{
public:
	//! This value is generally used throughout the system for controls with no ID
	static const tint32 giNoID;

	//! Sets the parent pane<br>Note: Should not be called, since pane calls it automatically
	/*!
		\param pPane [in]: Pointer to parent pane
	*/
	virtual void SetParentPane(IPane* pPane) = 0;

	//! Returns the parent pane
	/*!
		\return IPane*: Pointer to parent pane
	*/
	virtual IPane* GetParentPane() = 0;
	//! \overload
	virtual const IPane* GetParentPane() const = 0;

	//! Gets the top pane (i.e. traverses panes upward until it finds the top one). Note: Might return itself
	/*!
		\return IPane*: Pointer to top pane
	*/
	virtual IPane* GetTopPane() = 0;
	//! \overload
	virtual const IPane* GetTopPane() const = 0;

	//! Gets the parent window
	/*!
		\return IWindow*: Parent window
	*/
	virtual IWindow* GetParentWindow() = 0;
	//! \overload
	virtual const IWindow* GetParentWindow() const = 0;

	//! Initializion. If overriding (or making new version with new parameters), remember to call base version<br>Note: At destruction time bitmaps ref count is decreased by 1 by control
	/*!
		\param iID [in]: Control ID or IControl::giNoID for none
		\param pBitmap [in]: Bitmap resource to use, or NULL if none is required
	*/
	virtual void Init(tint32 iID, IBitmapResource* pBitmap) = 0;

	//! Returns control ID
	/*!
		\return tint32: Control ID
	*/
	virtual tint32 GetID() const = 0;

	//! Returns resource<br>Note: Typically this would be a bitmap resource, but for expandability IResource is returned
	/*!
		\return IResource*: Pointer to controls resource
	*/
	virtual IResource* GetResource() = 0;
	//! \overload
	virtual const IResource* GetResource() const = 0;

	//! Returns resources as bitmap resource
	/*!
		\return IBitmapResource*: Pointer to controls resource
	*/
	virtual IBitmapResource* GetBitmapResource() = 0;
	//! \overload
	virtual const IBitmapResource* GetBitmapResource() const = 0;

	//! Set control's absolute position (relative to client window)
	/*!
		\param PosAbsolute [in]: New absolute position
	*/
	virtual void SetAbsPos(const SPos& PosAbsolute) = 0;

	//! Set control's position relative to containing pane(s)
	/*!
		\param PosRelative [in]: New relative position
	*/
	virtual void SetRelPos(const SPos& PosRelative) = 0;

	//! <b>Obsolete, don't use!</b><br>Use SetAbsPos(...) - which works in the same way
	/*!
		\param Pos [in]: New absolute position
	*/
	virtual void SetPos(const SPos& Pos) = 0; // Obsolete - Use SetAbsPos(...) instead

	//! Set controls size
	/*!
		\param Size [in]: New size
	*/
	virtual void SetSize(const SSize& Size) = 0;

	//! Gets control's absolute positions (relative to client window)
	/*!
		\param rPosAbsolute [out]: Controls absolute position
	*/
	virtual void GetAbsPos(SPos& rPosAbsolute) const = 0;

	//! Gets control's position relative to containing pane(s)
	/*!
		\param rPosRelative [out]: Controls relative position
	*/
	virtual void GetRelPos(SPos& rPosRelative) const = 0;

	//! <b>Obsolete, don't use!</b><br>Use GetRelPos(...) - which works in the same way
	/*!
		\param rPos [out]: Controls relative position
	*/
	virtual void GetPos(SPos& rPos) const = 0; // Obsolete - Use GetRelPos(...) instead

	//! Gets controls rectangle, relative to client window (i.e. absolute position)
	/*!
		\param rRect [out]: Control's rectangle. Position is absolute (i.e. relative to client window)
	*/
	virtual void GetRect(SRect& rRect) const = 0;

	//! Returns current size of control
	/*!
		\param rSize [out]: Current size of control
	*/
	virtual void GetSize(SSize& rSize) const = 0;

	//! Resize and reposition this control to the SRect of another control.<br>Note! Doesn't change the stack order (a.k.a. y-position), so which-ever is created last gets first take at mouse events
	/*!
		\param pOtherControl [in]: The other control whose size and pos should be "mimicked"
		\return tbool: True => success, False => Some error, probably NULL pointer? Nothing done
	*/
	virtual tbool CloneRect(IControl* pOtherControl) = 0;

	//! Resize and reposition this control to the same SRect as that of its parent IPane
	/*!
		\return tbool: True => success, False => Some error, probably no parent? Nothing done
	*/
	virtual tbool CloneParentRect() = 0;

	//! Show or hide control
	/*!
		\param bVisible [in]: If true control is visible, otherwise invisible
	*/
	virtual void SetVisible(tbool bVisible) = 0;

	//! Returns visibility state
	/*!
		\return tbool: If true control is visible, otherwise invisible
	*/
	virtual tbool IsVisible() const = 0;

	//! Returns visibility state, by looking at not only control, but also its parent panes. If any of them is invisible, the control is also invisible.
	/*!
		\return tbool: If true control and parent panes are visible, otherwise invisible
	*/
	virtual tbool IsVisibleSafe() const = 0;

	//! Set value
	/*!
		\param iNew [in]: New value of control
		\param bNotify [in]: If true listeners will be notified
	*/
	virtual void SetValue(tint32 iNew, tbool bNotify = true) = 0;

	//! Set value. If converter is attached, the value is converted, otherwise it is the same as calling SetValue
	/*!
		\param iNew [in]: New converted value of control
		\param bNotify [in]: If true listeners will be notified
	*/
	virtual void SetConvertedValue(tint32 iNew, tbool bNotify = true) = 0;

	//! Returns current value.
	/*!
		\return tint32: Current value
	*/
	virtual tint32 GetValue() const = 0;

	//! Returns current converted value, if converted is attached, otherwise the same as GetValue
	/*!
		\return tint32: Current value
	*/
	virtual tint32 GetConvertedValue() const = 0;

	//! Sets the default value that the control will reverb to if double-clicked (for controls where this makes sense). If not set, no default setting action will happen
	/*!
		\param iValue [in]: Converted value to revert to. If no converter exists value is a raw control value
	*/
	virtual void SetDefaultValueConverted(tint32 iValue) = 0;

	//! Add event listener
	/*!
		\param pListener [in]: Event listener to add
	*/
	virtual void AddListener(IEventReceiver *pListener) = 0;

	//! Notifies all listeners of value change
	virtual void NotifyValueChange() = 0;

	//! Generic notification to all listeners
	virtual void NotifyGeneric(void *pNotificationData) = 0;

	//! Returns mouse position, relative to window
	/*!
		\param rPos [out]: Current mouse position
	*/
	virtual void GetMousePos(SPos& rPos) const = 0;

	//! Draw control.<br>Controls must implement this
	/*!
		\param rUpdate [in]: Update rectangle
	*/
	virtual void OnDraw(const SRect &rUpdate) = 0;

	//! Mouse event.<br>Controls must implement this<br>
	/*!
		\param MouseMsg [in]: Reason for call
		\param Pos [in]: Mouse position at event time
		\return tbool: True if handled, false otherwise
	*/
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos) = 0;

	//! Keyboard key down event.<br>Controls must implement this
	/*!
		\param iKey [in]: Key that was pressed
		\return tbool: True if handled, false otherwise
	*/
	virtual tbool OnKeyDown(EKey Key) = 0;

	//! Timer event<br>If overriding, be sure to call the CControl implementation
	virtual void OnTimer() = 0;

	//! Drawing primitive, which draws bitmap resource
	/*!
		\param pResource [in]: Resource to draw
		\param rctUpdate [in]: Current update rectangle
		\param rct [in]: Target rectangle
	*/
	virtual void DrawBitmap(IBitmapResource *pResource, const SRect &prctUpdate, const SRect &rct ) = 0;

	//! Drawing primitive, which draws 1 frame from bitmap resource
	/*!
		\param pResource [in]: Resource to draw
		\param rctUpdate [in]: Current update rectangle
		\param rct [in]: Target rectangle
		\param iFrame [in]: Frame to draw (0-based)
		\param iNrOfFrames [in]: Number of frames in resource
		\param bGhost [in]: If true, the last variant is drawn
	*/
	virtual void DrawBitmapFrame(IBitmapResource *pResource, const SRect &rctUpdate, const SRect &rct, tint iFrame, tint iNrOfFrames, tbool bGhost) = 0;

	//! Drawing primitive, which draws font resource
	/*!
		\param pResource [in]: Resource to draw
		\param rctUpdate [in]: Current update rectangle
		\param rct [in]: Target rectangle
		\param psz [in]: Text to draw
	*/
	virtual void DrawFont(IFontResource *pResource, const SRect &prctUpdate, const SRect &rct, const tchar* psz) = 0;

	//! Attach a data converter to a control. Note that any attached converter will be destroyed with the control.
	/*!
		\param pConverter [in]: Converter to attach, or NULL to remove. If removing, any old converter (if any) is detroyed
	*/
	virtual void SetConverter(de::IConverter* pConverter) = 0;

	//! Get the current converter, if any, or NULL
	/*!
		\return IConverter*: Pointer to current converter
	*/
	virtual de::IConverter* GetConverter() = 0;

	//! \overload
	virtual const de::IConverter* GetConverter() const = 0;

	//! Attach a display converter to a control. Note that any attached converter will be destroyed with the control.
	/*!
		\param pConverter [in]: Display converter to attach, or NULL to remove. If removing, any old converter (if any) is destroyed
	*/
	virtual void SetDisplayConverter(de::IDisplayConverter* pConverter) = 0;

	//! Get the current display converter, if any, or NULL
	/*!
		\return IDisplayConverter*: Pointer to current converter
	*/
	virtual de::IDisplayConverter* GetDisplayConverter() = 0;

	//! \overload
	virtual const de::IDisplayConverter* GetDisplayConverter() const = 0;

	//! If control ID matches control it will update its value, otherwise it will try to pass it on (i.e. a pane will call subcontrols)
	/*!
		\param iControlID [in]: ID of control to update
		\param iValueNew [in]: New value to set
		\param bNotify [in]: If true listeners will be notified, otherwise not
	*/
	virtual void SetControlValue(tint32 iControlID, tint32 iValueNew, tbool bNotify = false) = 0;

	//! Sets a tooltip text for the control
	/*!
		\param pszText [in]: Text to set. Must point to global memory.
	*/
	virtual void SetToolTipText(const tchar* pszText) = 0;

	//! Locks the control positions into grid (useful for controls which needs to lock to positions, i.e. knob)
	/*!
		\param iGridCount [in]: Number of positions in grid. Must be larger than 0
	*/
	virtual void SetGrid(tint iGridCount) = 0;

	//! Enables or disables input (mouse + keyboard) for the control. Default is enabled. The main pane cannot be disabled.
	/*!
		\param bEnable [in]: If true control is enabled (default), if false control is disabled.
	*/
	virtual void Enable(tbool bEnable = true) = 0;

	//! If called, the control has an extra row of frames shown when the control is disabled (Enable(false)).. Current only implemented for knobs and buttons (IButton and IMultiStateButton).
	virtual void CanGhost() = 0;

	//! Enum which describes positions that a control can stick to
	enum EPosition {
		//! Control doesn't stick to anything
		PosNone = 0,
		//! Control sticks to the top left corner
		PosTopLeft,
		//! Control sticks to top right corner (top right corner + (0, 1) for siblings)
		PosTopRight,
		//! Control sticks to bottom left corner (bottom left corner + (1, 0) for siblings)
		PosBottomLeft,
		//! Control sticks to bottom right corner
		PosBottomRight,
		//! Control sticks to center
		PosCenter
	};

	//! Enum which describes what a control sticks to
	enum EStickTo {
		//! The control sticks to the pane it is residing in
		StickToParent = 0,
		//! The control sticks to the control previously added. If the control is first it sticks to the top left position of the pane it is residing in.
		StickToSibling
	};

	//! Automatical reposition of control when parent/sibling is repositioned
	/*!
		\param Position [in]: Position to stick to, or PosNone to disable sticking
		\param Offset [in]: Offset relative to position
		\param StickTo [in]: Determines whether the control sticks to the pane it is residing in, or another control.
	*/
	virtual void StickTo(EPosition Position, SPos Offset = SPos(0, 0), EStickTo StickTo = StickToParent) = 0;

	//! Sets a max size for the control. If not called there's no max size. Any call to SetSize is checked against the max size.
	/*!
		\param Size [in]: Max size of control, or -1, -1 to disable max. size. -1 in one of the directions will disable max size for that direction only.
	*/
	virtual void SetMaxSize(SSize Size) = 0;

	//! Enum which describes the automatic resize mode
	enum EResizeMode {
		//! Resizing is done with the same amount of pixels in both directions
		ResizeAbsolute = 0,
		//! Resizing is done with the same amount of pixels in the X direction only
		ResizeAbsoluteX,
		//! Resizing is done with the same amount of pixels in the Y direction only
		ResizeAbsoluteY,
		//! Resizing is done with the same ratio
		ResizeRelative
	};

	//! Enables or disables automatic resizing
	/*!
		\param bEnabled [in]: If true, resizing is enabled. If false it is disabled, and the other parameters are ignored
		\param Mode [in]: Resizing mode
		\param fValue [in]: If the resize mode is ResizeRelative, this is the ratio, which can be any positive number (except 0). 1.0 means that the ratio is 100%, 0.5 means 50%, etc.
	*/
	virtual void SetAutomaticResize(tbool bEnabled = true, EResizeMode Mode = ResizeAbsolute, tfloat fValue = 1.0f) = 0;

	//! Reveals whether or not auto-resize is enabled for control
	/*!
		\return tbool: True => resize is enabled
	*/
	virtual tbool IsResizeEnabled() const = 0;

	//! Prevent control from getting smaller than this size
	/*!
		\param sizeMin [in]: The minimum size limit. Set one or both dimensions to 0 to ignore it and allow shrinking to 0
	*/
	virtual void SetMinSize(SSize sizeMin) = 0;

	//! Returns the controls enable state
	/*!
		\return tbool: If true control is enabled (receives mouse + keyboard), otherwise disabled.
	*/
	virtual tbool IsEnabled() const = 0;
};
