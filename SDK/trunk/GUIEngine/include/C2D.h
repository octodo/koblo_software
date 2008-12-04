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


/*! \class C2D
 * \brief Implementation of I2D
*/
class C2D : public virtual I2D, public virtual CControl
{
public:
	//! Constructor
	C2D();
	//! Destructor
	virtual ~C2D();

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

	//! CControl override
	virtual void SetSize(const SSize& Size);
	//! CControl override
	void SetValue(tint32 iNew, tbool bNotify);

	//! I2D override
	virtual void Init(tint iID, IBitmapResource* pBitmap);
	//! I2D override
	virtual tuint GetXPositions();
	//! I2D override
	virtual tuint GetYPositions();

	// Lasse: test
	//! IControl override
	virtual tint32 GetValue() const;
	// .. Lasse
	
	virtual tint32 Test(){return 5;}; 

protected:
	//! Drag state
	enum EDragState {
		//! Normal drag state
		DragNormal = 0,
		//! Dragging drag state (i.e. mouse has been pressed down on knob, and not released yet)
		DragDragging
	};

	//! Current drag state
	EDragState mDragState;

	//! Size of bitmap
	SSize mSizeBitmap;

	//! Number of positions in x direction
	tuint32 muiXPositions;

	//! Number of positions in y direction
	tuint32 muiYPositions;

	//! Difference between mouse and bitmap position at mouse down
	SPos mMouseOffset;

	//! Is mouse button pressed?
	tbool mbMouseDown;

	//! Last mouse position where we send the generic message saying the fineadjust was pressed (and mouse moved), or position at mouse click
	SPos mFineAdjustMousePos;

	//! True: either X or Y is locked (depending on which dimension is pulled "hardest")
	tbool mbLockOneDimension;

	//! Position at time of lock one dimension
	SPos mPosLockOneDimension;
};
