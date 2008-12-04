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


/*! \class CKnob
 * \brief Implementation of IKnob
*/
class CKnob : public virtual IKnob, public virtual CControl
{
public:
	//! Constructor
	CKnob();
	//! Destructor
	virtual ~CKnob();

	//! IDestructable override
	virtual void Destroy();

	//! IControl override
	virtual void OnDraw(const SRect &rUpdate);
	//! IControl override
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos);
	//! IControl override
	virtual tbool OnKeyDown(EKey Key);
	//! IControl override
	virtual void OnTimer();

	//! IKnob override
	virtual void Init(tint iID, IBitmapResource* pBitmap);

	//! IKnob implementation...
	virtual void SetIncludeFrames(tint32 iFirstIncludeFrame, tint32 iLastIncludeFrame = -1);

protected:
	//! Drag state
	enum EDragState {
		//! Normal drag state
		KnobDragNormal = 0,
		//! Dragging drag state (i.e. mouse has been pressed down on knob, and not released yet)
		KnobDragDragging
	};

	//! Current drag state
	EDragState mDragState;
	//! Mouse position when dragging was initiated
	SPos mPosDragStart;
	//! Value when dragging was initiated
	tint32 miValueDragStart;
	//! Current mouse position
	SPos mMousePos;
	//! Is mouse button pressed?
	tbool mbMouseDown;
	//! Since we're continuously updating the mouse position, to avoid leaving the screen area, we need to compensate with the amount we've changed
	SSize mMouseMoved;
	//! We cannot update the mouse position everytime it is moved, because that somehow causes Windows to not update the graphics. Instead we use this counter to do on every 10 mouse move message
	tint miMouseMoveCounter;

	//! Returns the frame to draw currently
	/*!
		\return tint: Frame to draw
	*/
	tint GetCurrentFrame() const;

	//! Count of positions in the knob. If 0 the knob is invisible. This is not the same as a normal invisible control, in that it can still be used.
	tint miNrOfPositions;

	//! Whether mouse adjust was down on last mouse move (or mouse down).
	tbool mbFineAdjustLast;

	//! First used frame in bitmap resource, (default = 0)
	tint32 miFirstIncludeFrame;
	//! Last used frame in bitmap resource (default = miNrOfPositions-1)
	tint32 miLastIncludeFrame;
	//! Number of frames that will be used
	tint32 miNrOfIncludedFrames;
};
