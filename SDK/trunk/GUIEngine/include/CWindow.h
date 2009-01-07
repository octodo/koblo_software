// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
//
// This file is part of the Koblo SDK.
//
// the Koblo SDK is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo SDK is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.



/*! \class CWindow
 * \brief Implementation of IWindow
*/
class CWindow : public virtual IWindow, public virtual CInContext, public virtual CSetContext
{
public:
	//! Constructor
	CWindow();
	//! Destructor
	virtual ~CWindow();

	//! IWindow override
	virtual void SetPane(IPane* pPane, tbool bResizeToFit = true);
	//! IWindow override
	virtual IPane* GetPane();
	//! IWindow override
	virtual const IPane* GetPane() const;
	//! IWindow override
	virtual void GetMousePos(SPos& rPos) const;
	//! IWindow override
	virtual tint32 GetMouseState() const;
	//! IWindow override
	virtual void Redraw(const SRect& rct);
	//! IWindow override
	virtual void Paint();
	//! IWindow override
	virtual void GetInvalidatedRect(tint32& riX, tint32& riY, tint32& riCX, tint32& riCY);
	//! IWindow override
	virtual void SetControlValue(tint32 iControlID, tint32 iValueNew, tbool bNotify = false);
	//! IWindow override
	virtual void ControlUpdateAll();
	//! IWindow override
	virtual IControl* GetControl(tint32 iControlID);
	//! IWindow override
	virtual void MouseDown(const SPos& Pos);
	//! IWindow override
	virtual void KeyDown(tint32 iKey);
	//! IWindow override
	virtual void RelayMouse(EMouseMsg Msg, const SPos& Pos);
	//! IWindow override
	virtual void SetMouseCursor(EMouseCursor Cursor);
	//! IWindow override
	virtual void GetMouseFocus(IControl* pControl, tbool bCapture = true);
	//! IWindow override
	virtual void ReleaseMouseFocus();
	//! IWindow override
	virtual tbool HasMouseFocus(const IControl* pControl);
	//! IWindow override
	virtual void ControlAlwaysOnTop(IControl* pControl);
	//! IWindow override
	virtual void ControlAlwaysOnTopDone();
	//! IWindow override
	virtual void InitToopTips(IFontResource* pFont);
	//! IWindow override
	virtual void OnTimer();
	//! IWindow override
	virtual void ResendMouseEvent();
	//! IWindow override
	virtual void SetFineAdjustModifier(EModifier Modifier);
	//! IWindow override
	virtual void SetResetModifier(EModifier Modifier);
	//! IWindow override
	virtual tbool FineAdjustPressed();
	//! IWindow override
	virtual tbool ResetPressed();
	//! IWindow override
	virtual void ControlAdded(tint32 iControlID, IControl* pControl);
	//! IWindow override
	virtual void OnMouseDrag(const SPos& Pos);
	//! IWindow override
	virtual void OnMouseDrop(const SPos& Pos);
	//! IWindow override
	virtual void SetDragAndDropInfo(const SDragAndDropInfo& Info);
	//! IWindow override
	virtual void GetDragAndDropInfo(SDragAndDropInfo& Info);

	//! Shows tool tip at a specific positions
	/*!
		\param Pos [in]: Position to show tool tip
		\param sText [in]: Text to show
	*/
	virtual void ShowToolTip(const SPos& Pos, const std::string& sText);

	//! Hides tool tip
	virtual void HideToolTip();

	//! Keyboard key down message action
	/*!
		\param iKey [in]: Key which was pressed
		\return tbool: If message was processed true, false otherwise
	*/
	virtual tbool OnKeyDown(EKey Key);

protected:
	//! Each window contains 1 pane (the "top pane")
	IPane* mpPane;

	//! Current (last reported) mouse position
	SPos mMousePosCur;

	//! Current mouse state flag
	tint32 miMouseStateCur;

	//! If true, the top pane will be resized with window
	tbool mbResizePaneToFit;

	//! Current shape of mouse cursor
	EMouseCursor mMouseCursorCur;

	//! Shape we're asked to set during mouse handling
	EMouseCursor mMouseCursorNext;

	//! Invalidater to keep track of window updating
	IInvalidater* mpInvalidater;

	//! The list is used as a stack (first in - last out). The top of the stack is the beginning of the list. The top of the stack is the control which currently has the mouse focus. If the stack is empty, there's none. Mouse focus means that (1) the control will get mouse messages, also from outside window (if possible), and (2) only this control will receive mouse messages
	std::list<IControl*> mMouseFocus;

	//! Control which currently should be drawn on top, or none.
	IControl* mpControlOnTop;

	//! Text to use for tool tips, or NULL if not initialized
	IText* mpToolTipText;

	//! Rect to use for tool tips
	IRect* mpToolTipRect;

	//! Lines to use for tool tips
	ILine* mpToolTipLines[4];

	//! If true the current mouse event needs to be send again
	tbool mbResendMouseEvent;

	//! Modifier (key) used for fine adjust
	EModifier mModifierFineAdjust;

	//! Modifier (key) used for reset
	EModifier mModifierReset;

	//! Map used to store control IDs. As we learn about them, we add them to the map. Note: This will currently not work if controls are deleted.
	std::map<tint32, IControl*> mControlMap;

	//! Drawing message action
	virtual void OnPaint() = 0;

	//! Current drag 'n drop info
	SDragAndDropInfo DragAndDropInfo;

	//! Mouse message action
	/*!
		\param MouseMsg [in]: Reason for call
		\param Pos [in]: Mouse position when call originated
		\return tbool: If message was processed true, false otherwise
	*/
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos);
	
	//! Internal call to set the mouse cursor
	virtual void DoSetMouseCursor(EMouseCursor Cursor) = 0;

	//! Checks whether a modifier (key) is pressed.
	/*!
		\param Modifier [in]: Key to check for
		\return tbool: True if key is pressed, false otherwise
	*/
	virtual tbool ModifierPressed(EModifier Modifier);
};

