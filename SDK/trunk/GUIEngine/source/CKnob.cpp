/*!	\file CKnob.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/


#include "geInternalOS.h"


CKnob::CKnob()
{
	mDragState = KnobDragNormal;
	mbMouseDown = false;

	// Lasse: Added 2006-10-06:
	miFirstIncludeFrame = 0;
	miLastIncludeFrame = -1;
	miNrOfIncludedFrames = 0;
	// .. Lasse
}

CKnob::~CKnob()
{
}

IKnob* IKnob::Create()
{
	return dynamic_cast<IKnob*>(new CKnob());
}

void CKnob::Destroy()
{
	delete dynamic_cast<CKnob*>(this);
}

void CKnob::Init(tint32 iID, IBitmapResource* pBitmap)
{
	CControl::Init(iID, pBitmap);

	if (pBitmap) {
		pBitmap->GetRealSize(mSize);

		miNrOfPositions = pBitmap->GetWidth() / mSize.iCX;
	}
	else {
		// No bitmap, so control is invisible
		miNrOfPositions = 0;
	}

	// Lasse: Added 2006-10-06:
	miFirstIncludeFrame = 0;
	miLastIncludeFrame = miNrOfPositions-1;
	miNrOfIncludedFrames = miNrOfPositions;
	// .. Lasse
}

tint CKnob::GetCurrentFrame() const
{
	ASSERT(miNrOfPositions > 0);

	tint64 iMax = (tint64)gControlMax - (tint64)gControlMin;
	tint64 iCur = (tint64)GetValue() - (tint64)gControlMin;
	tfloat fIndex = iCur / (float)iMax;

	// Lasse: Modified 2006-10-06:
	//tfloat fFrame = fIndex * (miNrOfPositions - 1);
	tfloat fFrame = miFirstIncludeFrame + ( fIndex * (miNrOfIncludedFrames - 1) );
	// .. Lasse
	tint32 iFrame = Float2Int(fFrame);

	return iFrame;
}

void CKnob::OnDraw(const SRect &rUpdate)
{
	if (IsVisible() == false) {
		return;
	}

	if (miNrOfPositions != 0) {
		if (mbCanGhost) {
			// Ghosting available...
			if (mbEnabled) {
				// ... but disabled
				DrawBitmapFrame(GetBitmapResource(), rUpdate, SRect(GetPos(), SSize(mSize.iCX, mSize.iCY)), GetCurrentFrame(), miNrOfPositions);
			}
			else {
				// ... and enabled
				DrawBitmapFrame(GetBitmapResource(), rUpdate, SRect(GetPos(), SSize(mSize.iCX, mSize.iCY)), GetCurrentFrame(), miNrOfPositions, true);
			}
		}
		else {
			// Ghosting not available
			DrawBitmapFrame(GetBitmapResource(), rUpdate, SRect(GetPos(), SSize(mSize.iCX, mSize.iCY)), GetCurrentFrame(), miNrOfPositions);
		}
	}
}

tbool CKnob::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	if (IsVisible() == false) {
		return false;
	}

	SRect rctThis(GetPos(), GetSize());

	if (MouseMsg == LeftButtonDblClk) {
		if (rctThis.Inside(Pos) == true) {
			SetToDefaultValue();
			return true;
		}
	}

	if (MouseMsg == LeftButtonDown) {
		if (rctThis.Inside(Pos) == true) {
			if (GetParentWindow()->ResetPressed()) {
				SetToDefaultValue();
				return true;
			}

			mbMouseDown = true;
			miValueDragStart = GetValue();
			mPosDragStart = Pos;
			mDragState = KnobDragDragging;
			GetParentWindow()->HideMouse();
			mMouseMoved = SSize(0, 0);
			miMouseMoveCounter = 0;
			mbFineAdjustLast = GetParentWindow()->FineAdjustPressed();
			dynamic_cast<CWindow*>(GetParentWindow())->GetMouseFocus(this);
			return true;
		}

		return false;
	}

	if (MouseMsg == LeftButtonUp) {
		mbMouseDown = false;
		if (mDragState == KnobDragDragging) {
			mDragState = KnobDragNormal;
			dynamic_cast<CWindow*>(GetParentWindow())->ReleaseMouseFocus();
			GetParentWindow()->SetMousePos(mPosDragStart);
			GetParentWindow()->ShowMouse();
			return true;
		}
	}
	if (MouseMsg == MouseMove) {
		if (mDragState == KnobDragDragging) {
			if (mbFineAdjustLast != GetParentWindow()->FineAdjustPressed()) {
				miValueDragStart = GetValue();
				mPosDragStart = Pos;
				mMouseMoved = SSize(0, 0);
				miMouseMoveCounter = 0;
				mbFineAdjustLast = GetParentWindow()->FineAdjustPressed();
			}

			// Compensate mouse position with the amount we've moved the mouse
			mMousePos = Pos - SPos(mMouseMoved.iCX, mMouseMoved.iCY);

			tint64 iPositions = (tint64)gControlMax - (tint64)gControlMin + 1;
			tfloat fPixelsPerPosition = 300.0f / iPositions;
			tint32 iDelta = mPosDragStart.iY - mMousePos.iY;
			tint32 iPositionDelta = Float2Int(iDelta / fPixelsPerPosition);
			if (GetParentWindow()->FineAdjustPressed()) {
				iPositionDelta = iPositionDelta / 10;
			}
			SetValue(miValueDragStart + iPositionDelta);

#ifdef WIN32
			// Move the mouse position, to avoid leaving the screen area
			if (miMouseMoveCounter++ == 10) {
				GetParentWindow()->SetMousePos(mPosDragStart);
				mMouseMoved.iCX += mPosDragStart.iX - Pos.iX;
				mMouseMoved.iCY += mPosDragStart.iY - Pos.iY;
				miMouseMoveCounter = 0;
			}
#endif

			return true;
		}
	}

	return false;
}

tbool CKnob::OnKeyDown(EKey /*Key*/)
{
	return false;
}

void CKnob::OnTimer()
{
	CControl::OnTimer();
}


void CKnob::SetIncludeFrames(tint32 iFirstIncludeFrame, tint32 iLastIncludeFrame /*= -1*/)
{
	ASSERT(miNrOfPositions);

	if (iLastIncludeFrame==-1) {
		iLastIncludeFrame = miNrOfPositions-1;
	}

	ASSERT(iFirstIncludeFrame>=0);
	ASSERT(iLastIncludeFrame>=iFirstIncludeFrame);
	ASSERT(iLastIncludeFrame<miNrOfPositions);

	miFirstIncludeFrame = iFirstIncludeFrame;
	miLastIncludeFrame = iLastIncludeFrame;
	miNrOfIncludedFrames = miLastIncludeFrame-miFirstIncludeFrame+1;
} // CKnob::SetIncludePositions
