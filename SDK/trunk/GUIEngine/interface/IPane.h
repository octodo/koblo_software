/*!	\file IPane.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

/*! \class IPane
 * \brief A Pane is a part of a window, and consists of x panes or controls.
 *
 * A pane is a part of a window containing 0 to n controls (or subpanes)<br>IT is itself a control, so Draw, OnMouse etc. is overridden<br>A topmost pane doesn't have a parent pane, i.e. it is not contrained in another pane<br>When controls (or panes) are added, they're stacked as first-in-bottommost, meaning the last to be added is the last to be drawn (topmost), and the first to receive mouse events
*/
class IPane : public virtual IControl, public virtual IDestructable
{
public:
	//! Create IPane
	static IPane* Create();

	//! Is top pane?
	/*!
		\return tbool: True if object is top pane, false otherwise
	*/
	virtual tbool IsTopPane() const = 0;

	//! Set parent window. Note: Should only be set for top pane (other panes will use parent pane to get it)<br>Note: This call should not be used by GUI engine client, since the window will call it automatically
	/*!
		\param pWindow [in]: Window to set
	*/
	virtual void SetParentWindow(IWindow* pWindow) = 0;

	//! Returns parent window
	/*!
		\return IWindow: Pointer to parent window
	*/
	virtual IWindow* GetParentWindow() = 0;
	//! \overload
	virtual const IWindow* GetParentWindow() const = 0;

	//! Add a control (or pane) to the pane<br>Any control added will be destoyed automatically at destruction time<br>Will notify control of change (SetParentPane())
	/*!
		\param pToAdd [in]: Pointer to control to add
		\param Pos [in]: Position where to add it
	*/
	virtual void AddControl(IControl* pToAdd, const SPos& Pos) = 0;

	//! Remove a previously added control (or pane) from the pane
	/*!
		\param pToRemove [in]: Pointer to control to remove
	*/
	virtual void RemoveControl(IControl* pToRemove) = 0;

	//! Redraws part of pane
	/*!
		\param rct [in]: Update rectangle
	*/
	virtual void Redraw(const SRect& rct) = 0;

	//! Set size
	/*!
		\param Size [in]: New size
		\param bRedraw [in]: If true, the old and new rect is redrawed
	*/
	virtual void SetSize(const SSize& Size, tbool bRedraw = false) = 0;

	//! Returns control with given ID, or NULL if none exist
	/*!
		\param iControlID [in]: ID of control to return
		\return iControl*: Control or NULL
	*/
	virtual IControl* GetControl(tint32 iControlID) = 0;

	//! During drawing a pane checks the update rectangle against its size and position to see if it needs painting. Calling this method will disable that check. This is used by the scrolling pane (which uses its position and size slightly differently than other panes)
	virtual void DisableDrawingRectCheck() = 0;

	//! Disables control caching (when a control is retrieved from an ID)
	virtual void DisableControlCaching() = 0;

	//! Gives the pane a background colour. If not called, the pane has no background
	virtual void SetBackgroundColour(SRGB Colour) = 0;

	//! Suppresses functionality of CPane::UpdatePositions(..)<br/>Useful for speeding up multiple control additions to a pane (remember to enable functionality afterwards!)
	/*!
		\param bSuppress [in]: True to suppress (faster AddControl calls, but doesn't update), False to allow (will also seek through entire list of controls and update all of them)
	*/
	virtual void SuppressUpdatePositions(tbool bSuppress) = 0;
};
