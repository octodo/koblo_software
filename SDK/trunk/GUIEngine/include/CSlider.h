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



/*! \class CSlider
 * \brief Implementation of ISlider
*/
class CSlider : public virtual ISlider, public virtual CControl
{
public:
	//! Constructor
	CSlider();
	//! Destructor
	virtual ~CSlider();

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
	//! IControl override
	virtual void SetSize(const SSize& Size);

	//! ISlider override
	virtual void Init(tint32 iID, IBitmapResource* pCapBitmap, IBitmapResource* pLineBitmap, SPos Offset);
	//! ISlider override
	virtual void SetResponsiveness(tfloat fResponse);

protected:
	tfloat mfResponse;

	//! Drag state
	enum EDragState {
		//! Normal drag state
		SliderDragNormal = 0,
		//! Dragging drag state (i.e. mouse has been pressed down on Slider, and not released yet)
		SliderDragDragging
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
	//! Cap pos when we started dragging
	SPos mCapPosStart;

	//! Returns the count of positions in the Slider
	/*!
		\return tint: Position count
	*/
	tint miNrOfPositions;

	//! Returns the position to draw the cap with the current settings
	/*!
		\return SPos: Position to draw the cap with the current settings
	*/
	SPos GetCapPos();

	IBitmapResource* mpLineBitmap;
	SPos mLinePos;
};
