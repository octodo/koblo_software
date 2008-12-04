/*!	\file C2D.cpp
	\author Michael Olsen
	\date 14/Dec/2005
	\date 14/Dec/2005
*/


#include "geInternalOS.h"


C2D::C2D()
{
	mDragState = DragNormal;
	mbMouseDown = false;
}

C2D::~C2D()
{
}

I2D* I2D::Create()
{
	return dynamic_cast<I2D*>(new C2D());
}

void C2D::Destroy()
{
	delete dynamic_cast<C2D*>(this);
}

void C2D::Init(tint32 iID, IBitmapResource* pBitmap)
{
	CControl::Init(iID, pBitmap);

	pBitmap->GetRealSize(mSizeBitmap);
}

tuint C2D::GetXPositions()
{
	return muiXPositions;
}

tuint C2D::GetYPositions()
{
	return muiYPositions;
}

void C2D::SetSize(const SSize& Size)
{
	CControl::SetSize(Size);

	muiXPositions = mSize.iCX - mSizeBitmap.iCX + 1;
	muiYPositions = mSize.iCY - mSizeBitmap.iCY + 1;
}

void C2D::SetValue(tint32 iNew, tbool bNotify)
{
	miValue = iNew;

	if (bNotify) {
		NotifyValueChange();
	}

	Redraw();
}

void C2D::OnDraw(const SRect &rUpdate)
{
	if (IsVisible() == false) {
		return;
	}

	SPos BitmapPos;
	BitmapPos.iX = GetValue() >> 16;
	if (BitmapPos.iX < 0) {
		BitmapPos.iX = 0;
	}
	if (BitmapPos.iX >= (tint)muiXPositions) {
		BitmapPos.iX = muiXPositions - 1;
	}
	BitmapPos.iY = GetValue() & 0xffff;
	if (BitmapPos.iY < 0) {
		BitmapPos.iY = 0;
	}
	if (BitmapPos.iY >= (tint)muiYPositions) {
		BitmapPos.iY = muiYPositions - 1;
	}

	DrawBitmap(GetBitmapResource(), rUpdate, SRect(GetPos() + BitmapPos, mSizeBitmap));
}

tbool C2D::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	if (IsVisible() == false) {
		return false;
	}

	SPos BitmapPos;
	BitmapPos.iX = GetValue() >> 16;
	if (BitmapPos.iX < 0) {
		BitmapPos.iX = 0;
	}
	if (BitmapPos.iX >= (tint)muiXPositions) {
		BitmapPos.iX = muiXPositions - 1;
	}
	BitmapPos.iY = GetValue() & 0xffff;
	if (BitmapPos.iY < 0) {
		BitmapPos.iY = 0;
	}
	if (BitmapPos.iY >= (tint)muiYPositions) {
		BitmapPos.iY = muiYPositions - 1;
	}
	SRect rctBitmap(GetPos() + BitmapPos, mSizeBitmap);

	if (MouseMsg == LeftButtonDblClk) {
		if (rctBitmap.Inside(Pos) == true) {
			SetToDefaultValue();
			return true;
		}
	}

	if (MouseMsg == LeftButtonDown) {
		if (rctBitmap.Inside(Pos) == true) {
			if (GetParentWindow()->ResetPressed()) {
				SetToDefaultValue();
				return true;
			}

			mbLockOneDimension = (GetParentWindow()->ShiftPressed());
			if (mbLockOneDimension) {
				mPosLockOneDimension = BitmapPos;
			}

			mFineAdjustMousePos = Pos - GetPos();
			
			mMouseOffset = Pos - GetPos() - BitmapPos;

			mbMouseDown = true;
			mDragState = DragDragging;
			//GetParentWindow()->HideMouse();
			dynamic_cast<CWindow*>(GetParentWindow())->GetMouseFocus(this);
			return true;
		}

		return false;
	}

	if (MouseMsg == LeftButtonUp) {
		mbMouseDown = false;
		if (mDragState == DragDragging) {
			mDragState = DragNormal;
			dynamic_cast<CWindow*>(GetParentWindow())->ReleaseMouseFocus();
			//GetParentWindow()->ShowMouse();

			//GetParentWindow()->SetMousePos(BitmapPos + GetPos());

			return true;
		}
	}

	if (MouseMsg == MouseMove) {
		if (mDragState == DragDragging) {
			if (GetParentWindow()->FineAdjustPressed()) {
				// Fine adjust button pressed
				SPos FineAdjustNew = Pos - GetPos();
				SPos Diff = FineAdjustNew - mFineAdjustMousePos;
				mFineAdjustMousePos = FineAdjustNew;

				tint32 piMessage[2];
				piMessage[0] = 0;
				piMessage[1] = Diff.iY;

				NotifyGeneric((void*)piMessage);
			}
			else {
				// Fine adjust button not pressed
				SPos BitmapPos = Pos - GetPos();
				BitmapPos -= mMouseOffset;

				// Lasse 2006-11-01: Maybe lock one dimension
				if (GetParentWindow()->ShiftPressed()) {
					if (!mbLockOneDimension) {
						mbLockOneDimension = true;
						mPosLockOneDimension = BitmapPos;
					}
				}
				else {
					mbLockOneDimension = false;
				}

				if (mbLockOneDimension) {
					SPos posMoved = mPosLockOneDimension-BitmapPos;
					if (abs(posMoved.iX)>abs(posMoved.iY)) {
						BitmapPos.iY = mPosLockOneDimension.iY;
					}
					else {
						BitmapPos.iX = mPosLockOneDimension.iX;
					}
				}
				// .. Lasse

				if (BitmapPos.iX < 0) {
					BitmapPos.iX = 0;
				}
				if (BitmapPos.iX >= (tint)muiXPositions) {
					BitmapPos.iX = muiXPositions - 1;
				}
				if (BitmapPos.iY < 0) {
					BitmapPos.iY = 0;
				}
				// Lasse: work-around 2006-10-23
				/*
				if (BitmapPos.iY >= (tint)muiYPositions) {
					BitmapPos.iY = muiYPositions - 1;
				}
				*/
				// (mo): That's a no-go
/*				if (BitmapPos.iY >= (tint)muiYPositions-1) {
					BitmapPos.iY = muiYPositions - 2;
				}
				// .. Lasse*/
				if (BitmapPos.iY >= (tint)muiYPositions) {
					BitmapPos.iY = muiYPositions - 1;
				}

				SetValue((BitmapPos.iX << 16) | BitmapPos.iY, true);

				tint32 piMessage[3];
				piMessage[0] = 1;
				piMessage[1] = BitmapPos.iX;
				piMessage[2] = BitmapPos.iY;

				NotifyGeneric((void*)piMessage);
			}

			return true;
		}
	}

	return false;
}

tbool C2D::OnKeyDown(EKey /*Key*/)
{
	return false;
}

void C2D::OnTimer()
{
	CControl::OnTimer();
}


// Lasse: test
tint32 C2D::GetValue() const
{
	tint32 iValue = CControl::GetValue();
	return iValue;
}
// .. Lasse