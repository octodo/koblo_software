/*!	\file CSlider.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 13/Dec/2005
*/


#include "geInternalOS.h"

CSlider::CSlider() : mfResponse(0.5f)
{
	mDragState = SliderDragNormal;
	mbMouseDown = false;

	mpLineBitmap = NULL;
}

CSlider::~CSlider()
{
	if(mpLineBitmap) {
		mpLineBitmap->DecRefCount();
	}
}

ISlider* ISlider::Create()
{
	return dynamic_cast<ISlider*>(new CSlider());
}

void CSlider::Destroy()
{
	delete dynamic_cast<CSlider*>(this);
}

void CSlider::Init(tint32 iID, IBitmapResource* pCapBitmap, IBitmapResource* pLineBitmap, SPos LineOffset)
{
	CControl::Init(iID, pCapBitmap);

	mpLineBitmap = pLineBitmap;
	mLinePos = LineOffset;
}

void CSlider::SetResponsiveness(tfloat fResponse)
{
	mfResponse = fResponse;
}

void CSlider::SetSize(const SSize& Size)
{
	SSize SizeNew = Size;

	// Patch size of it matches cap
	if (mpLineBitmap == NULL) {
		SSize SizeTmp;
		GetBitmapResource()->GetRealSize(SizeTmp);
		SizeNew.iCX = SizeTmp.iCX;
		SizeNew.iCY += SizeTmp.iCY;
	}

	CControl::SetSize(SizeNew);

	miNrOfPositions = mSize.iCY - GetBitmapResource()->GetHeight();
}

SPos CSlider::GetCapPos()
{
	tfloat fIndex;
	fIndex = (tfloat)(GetValue() - gControlMin);
	tint iRange = gControlMax;
	iRange -= gControlMin;
	iRange += 1;
	fIndex = fIndex / iRange;
	fIndex *= miNrOfPositions;

	tint iIndex = (tint)fIndex;
	tint iYDrawPos = GetPos().iY + miNrOfPositions - iIndex;

	return SPos(GetPos().iX, iYDrawPos);
}

void CSlider::OnDraw(const SRect &rUpdate)
{
	if (IsVisible() == false) {
		return;
	}

	// Line
	if (mpLineBitmap) {
		SSize LineSize;
		mpLineBitmap->GetRealSize(LineSize);
		DrawBitmap(mpLineBitmap, rUpdate, SRect(GetPos()+mLinePos,LineSize));
	}

	// Cap
	DrawBitmap(GetBitmapResource(), rUpdate, SRect(GetCapPos(), mSize));
}

tbool CSlider::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
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
			mCapPosStart = GetCapPos();
			mDragState = SliderDragDragging;
			GetParentWindow()->HideMouse();
			mMouseMoved = SSize(0, 0);
			miMouseMoveCounter = 0;
			dynamic_cast<CWindow*>(GetParentWindow())->GetMouseFocus(this);
			return true;
		}

		return false;
	}

	if (MouseMsg == LeftButtonUp) {
		mbMouseDown = false;
		if (mDragState == SliderDragDragging) {
			// Get the current cap pos so we can compensate
			SPos CapPos = GetCapPos();
			tint iDelta = CapPos.iY - mCapPosStart.iY;

			mDragState = SliderDragNormal;
			dynamic_cast<CWindow*>(GetParentWindow())->ReleaseMouseFocus();
			GetParentWindow()->SetMousePos(SPos(mPosDragStart.iX, mPosDragStart.iY + iDelta));
			GetParentWindow()->ShowMouse();
			return true;
		}
	}
	if (MouseMsg == MouseMove) {
		if (mDragState == SliderDragDragging) {
			// Compensate mouse position with the amount we've moved the mouse
			mMousePos = Pos - SPos(mMouseMoved.iCX, mMouseMoved.iCY);

			tint64 iPositions = (tint64)gControlMax - (tint64)gControlMin + 1;
//			tfloat fPixelsPerPosition = 300.0f / iPositions;
			tfloat fPixelsPerPosition = ((1 - mfResponse) * 600) / iPositions;
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

tbool CSlider::OnKeyDown(EKey /*Key*/)
{
	return false;
}

void CSlider::OnTimer()
{
	CControl::OnTimer();
}



