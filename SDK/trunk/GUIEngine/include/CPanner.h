// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
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


/*! \class CPanner
 * \brief Implementation of IPanner
*/
class CPanner : public virtual IPanner, public virtual CPane, public virtual CEventReceiver
{
public:
	//! Constructor
	CPanner();
	//! Destructor
	virtual ~CPanner();

	//! IDestructable override
	virtual void Destroy();

	//! IPane override
	virtual void OnDraw(const SRect &rUpdate);
	//! IPane override
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos);
	//! IPane override
	virtual tbool OnKeyDown(EKey Key);
	//! IPane override
	virtual void OnTimer();
	//! CPane override
	void AddListener(IEventReceiver *pListener);
	
	//! IPane override
	virtual void SetVisible(tbool bVisible);

	//! IPanner override
	virtual void Init(tint32 iCtrlIDHorizontal, tint32 iCtrlIDVertical,
					  IBitmapResource* pBmpBack, IBitmapResource* pBmpHandle,
					  SPos posArea, SSize sizeArea, tbool bBitmapStaysInsideArea);

	//! IPanner override
	virtual void SetDirectMouseXlat(tbool bDirectXlat = true);
	//! IPanner
	virtual void SetJumpToMouseClickPos(tbool bJumpToMouse = true);

protected:
	//! Drag state
	enum EDragState {
		//! Normal drag state
		PannerDragNormal = 0,
		//! Dragging drag state (i.e. mouse has been pressed down on Panner, and not released yet)
		PannerDragDragging
	};

	//! Current drag state
	EDragState mDragState;
	// Lasse, mod 2008-01-21
	/*
	//! Is mouse button pressed?
	tbool mbMouseDown;
	*/
	//! Mouse position when dragging was initiated
	SPos mPosDragStart;
	//! Value when dragging was initiated
	tint32 miValueXDragStart;
	//! Value when dragging was initiated
	tint32 miValueYDragStart;
	//! Current mouse position
	SPos mMousePos;
	//! Is mouse button pressed?
	tbool mbMouseDown;
	//! Since we're continuously updating the mouse position, to avoid leaving the screen area, we need to compensate with the amount we've changed
	SSize mMouseMoved;
	//! We cannot update the mouse position everytime it is moved, because that somehow causes Windows to not update the graphics. Instead we use this counter to do on every 10 mouse move message
	tint miMouseMoveCounter;
	//! Whether mouse adjust was down on last mouse move (or mouse down).
	tbool mbFineAdjustLast;
	// .. Lasse

	//! The absolute offset of the area that the handle bitmap will travel.
	SPos mposAbsArea;
	//! The relative offset of the area that the handle bitmap will travel.
	SPos mposRelArea;
	//! The size of the area that the top left corner of the handle bitmap will travel.
	SSize msizeAreaCorrected;
	
	//! The handle bitmap
	IBitmap* mpBmpBack;
	//! Invisible knob for X
	IKnob* mpKnobX;
	//! Invisible knob for Y
	IKnob* mpKnobY;
	//! The handle bitmap
	IBitmap* mpBmpHndl;
	//! The size of the handle bitmap
	SSize msizeBmpHndl;
	
	//! Calculates the two dimensional values from the current position of the handle bitmap
	void CalcValuesFromHandlePosition();
	//! Move handle bitmap and recalculate the two dimensional values accordingly
	void MoveHandleRelPos(tint32 iRelX, tint32 iRelY);
	//! Try to move handle bitmap to absolute position and recalculate the two dimensional values accordingly
	void MoveHandleAbsPos(tint32 iAbsX, tint32 iAbsY);
	//! Calculates the position of the handle bitmap from the two dimensional values
	void CalcHandlePositionFromValues();
	
	//! CControl implementation
	virtual void SetToDefaultValue();
	
	//! Last X value used for calculating knob position
	tint32 miLastX;
	//! Last Y value used for calculating knob position
	tint32 miLastY;

	//! CEventReceiver implementation
	virtual void EventValueChange(IControl *pSender, tint32 iValueNew);

	tbool mbDirectMouseXlat;
	tbool mbJumpToMouseClickPos;	
};
