/*!	\file CPanner.cpp
	\author Lasse Steen Bohnstedt
	\date 2007-09-12
*/


#include "geInternalOS.h"

CPanner::CPanner() : mpBmpBack(NULL), mpKnobX(NULL), mpKnobY(NULL), mpBmpHndl(NULL)
{
	SetAutomaticResize(false);

	mDragState = PannerDragNormal;
	mbMouseDown = false;

	mbDirectMouseXlat = false;
	mbJumpToMouseClickPos = false;
}

CPanner::~CPanner()
{
	if (mpKnobX)
		mpKnobX->Destroy();
	if (mpKnobY)
		mpKnobY->Destroy();
	if (mpBmpHndl)
		mpBmpHndl->Destroy();
}

IPanner* IPanner::Create()
{
	return dynamic_cast<IPanner*>(new CPanner());
}

void CPanner::Destroy()
{
	delete dynamic_cast<CPanner*>(this);
}

void CPanner::AddListener(IEventReceiver *pListener)
{
	mpKnobX->AddListener(pListener);
	mpKnobY->AddListener(pListener);
}

void CPanner::Init(tint32 iCtrlIDHorizontal, tint32 iCtrlIDVertical,
				   IBitmapResource* pBmpResBack, IBitmapResource* pBmpResHandle,
				   SPos posArea, SSize sizeArea, tbool bBitmapStaysInsideArea)
{
	CPane::Init(ge::IControl::giNoID, NULL);
	
	// Lasse, bug-fix 2008-04-30
	// Make sure area size is valid
	if (sizeArea.iCX <= 0)
		sizeArea.iCX = 1;
	if (sizeArea.iCY <= 0)
		sizeArea.iCY = 1;
	// .. Lasse

	if (pBmpResBack) {
		mpBmpBack = dynamic_cast<IBitmap*>(CBitmap::Create());
		mpBmpBack->Init(ge::IControl::giNoID, pBmpResBack);
		this->AddControl(mpBmpBack, ge::SPos(0, 0));
	}
	
	// Invisible control for horizontal position
	mpKnobX = dynamic_cast<IKnob*>(CKnob::Create());
	mpKnobX->Init(iCtrlIDHorizontal, NULL);
	this->AddControl(mpKnobX, SPos(0, 0));
	mpKnobX->SetValue(0);
	miLastX = 0;
	mpKnobX->AddListener(dynamic_cast<IEventReceiver*>(this));

	// Invisible control for vertical position
	mpKnobY = dynamic_cast<IKnob*>(CKnob::Create());
	mpKnobY->Init(iCtrlIDVertical, NULL);
	this->AddControl(mpKnobY, SPos(0, 0));
	mpKnobY->SetValue(0);
	miLastY = 0;
	mpKnobY->AddListener(dynamic_cast<IEventReceiver*>(this));
	
	mposRelArea = posArea;
	SSize sizePane = SSize(1, 1);
	msizeBmpHndl = SSize(0, 0);
	if (pBmpResHandle) {
		mpBmpHndl = dynamic_cast<IBitmap*>(CBitmap::Create());
		mpBmpHndl->Init(ge::IControl::giNoID, pBmpResHandle);
		mpBmpHndl->GetSize(msizeBmpHndl);
		if (!bBitmapStaysInsideArea) {
			// Use moving area as it is
			msizeAreaCorrected = sizeArea;
			// Calculate total pane area from moving area
			tint32 iCX = sizeArea.iCX + msizeBmpHndl.iCX + mposRelArea.iX;
			tint32 iCY = sizeArea.iCY + msizeBmpHndl.iCY + mposRelArea.iY;
			sizePane = SSize(iCX, iCY);
		}
		else {
			// Try to use pane area as it is
			sizePane = sizeArea;
			// Calculate the moving area from the pane area
			tint32 iCX = sizeArea.iCX - msizeBmpHndl.iCX;
			if (iCX <= mposRelArea.iX) {
				sizePane.iCX += (mposRelArea.iX - iCX);
				iCX = 1;
			}
			tint32 iCY = sizeArea.iCY - msizeBmpHndl.iCY;
			if (iCY <= mposRelArea.iY) {
				sizePane.iCY += (mposRelArea.iY - iCY);
				iCY = 1;
			}
			msizeAreaCorrected = SSize(iCX, iCY);
		}
	}
	this->SetSize(sizePane, false);
	if (mpBmpHndl) {
		this->AddControl(mpBmpHndl, SPos(msizeAreaCorrected.iCX >> 1, msizeAreaCorrected.iCY >> 1) + mposRelArea);
		mposAbsArea = mposRelArea + this->GetAbsPos();
	}
}

void CPanner::SetVisible(tbool bVisible)
{
	tbool bBefore = IsVisibleSafe();
	
	CPane::SetVisible(bVisible);
	
	if (!bBefore) {
		tbool bAfter = IsVisibleSafe();
		if (bAfter) {
			CalcHandlePositionFromValues();
		}
	}
}

void CPanner::OnDraw(const SRect &rUpdate)
{
	if (IsVisible() == false) {
		return;
	}
	
	if (
		(miLastX != mpKnobX->GetValue())
		||
		(miLastY != mpKnobY->GetValue())
	)
	{
		CalcHandlePositionFromValues();
		miLastX = mpKnobX->GetValue();
		miLastY = mpKnobY->GetValue();
	}

	// Lasse, mod 2008-04-30 - avoid crash on no back-drop
	/*
	mpBmpBack->OnDraw(rUpdate);
	*/
	if (mpBmpBack)
		mpBmpBack->OnDraw(rUpdate);
	// .. Lasse
	mpBmpHndl->OnDraw(rUpdate);
}

void CPanner::CalcHandlePositionFromValues()
{
	if (mpBmpHndl) {
		tfloat32 fControlRange = (tfloat32)gControlMax - (tfloat32)gControlMin + 1;

		SPos posOfsBmp = SPos(0,0);
		
		// Calc offset Y pos
		tfloat32 fX = 0.5;
		if (msizeAreaCorrected.iCX > 1) {
			tint32 iRawValueX = mpKnobX->GetValue();
			fX = (tfloat32)(iRawValueX - gControlMin)/fControlRange;
		}
		posOfsBmp.iX = Float2Int(fX * (tfloat32)(msizeAreaCorrected.iCX - 1));
		
		// Calc offset X pos
		tfloat32 fY = 0.5;
		if (msizeAreaCorrected.iCY > 1) {
			tint32 iRawValueY = mpKnobY->GetValue();
			fY = 1 - (tfloat32)(iRawValueY - gControlMin)/fControlRange;
		}
		posOfsBmp.iY = Float2Int(fY * (tfloat32)(msizeAreaCorrected.iCY - 1));
		
		// Convert "offset" pos (relative to area) to "relative" pos (relative to IPane)
		SPos posRelBmp = mposRelArea + posOfsBmp;
		
		// Set relative pos of bitmap
		SPos posRelBmpLast;
		mpBmpHndl->GetRelPos(posRelBmpLast);
		if ((posRelBmp.iX != posRelBmpLast.iX) || (posRelBmp.iY != posRelBmpLast.iY)) {
			mpBmpHndl->SetRelPos(posRelBmp);
		
			// Update parent pane
			IPane* pPane = GetParentPane();
			if (pPane) {
				SRect r;
				pPane->GetRect(r);
				pPane->Redraw(r);
			}
		}
		
	}
}

void CPanner::CalcValuesFromHandlePosition()
{
	if (mpBmpHndl) {
		SPos posRelBmp;
		mpBmpHndl->GetRelPos(posRelBmp);
		SPos posOfs = posRelBmp - mposRelArea;
		
		tfloat32 fControlRange = (tfloat32)gControlMax - (tfloat32)gControlMin + 1;
		
		// Calc X value
		tfloat32 fX = 0.5;
		if (msizeAreaCorrected.iCX > 1)
			fX = (tfloat32)posOfs.iX / (tfloat32)(msizeAreaCorrected.iCX - 1);
		
		tint32 iRawValueX = Float2Int(fX * fControlRange + (tfloat32)gControlMin);
		mpKnobX->SetValue(iRawValueX, true);
	
		// Calc Y value
		tfloat32 fY = 0.5;
		if (msizeAreaCorrected.iCY > 1)
			fY = (tfloat32)posOfs.iY / (tfloat32)(msizeAreaCorrected.iCY - 1);
		tint32 iRawValueY = Float2Int(fY * fControlRange - (tfloat32)gControlMin);
		mpKnobY->SetValue(iRawValueY, true);
	}
}

void CPanner::MoveHandleRelPos(tint32 iRelX, tint32 iRelY)
{
	if (mpBmpHndl) {
		SPos posRelBefore;
		mpBmpHndl->GetRelPos(posRelBefore);

		// Verify X position
		tint32 iOfsX = iRelX - mposRelArea.iX;
		if (iOfsX < 0)
			iOfsX = 0;
		else if (iOfsX >= msizeAreaCorrected.iCX)
			iOfsX = msizeAreaCorrected.iCX - 1;
		iRelX = iOfsX + mposRelArea.iX;
		
		// Verify Y position
		tint32 iOfsY = iRelY - mposRelArea.iY;
		if (iOfsY < 0)
			iOfsY = 0;
		else if (iOfsY >= msizeAreaCorrected.iCY)
			iOfsY = msizeAreaCorrected.iCY - 1;
		iRelY = iOfsY + mposRelArea.iY;
		
		if ((iRelX != posRelBefore.iX) || (iRelY != posRelBefore.iY)) {
			SRect rctRedraw(posRelBefore, msizeBmpHndl);
			SPos posRel(iRelX, iRelY);
			mpBmpHndl->SetRelPos(posRel);
			//CalcValuesFromHandlePosition();
			// Maybe?
			CalcValuesFromHandlePosition();
			// Maybe expand the redraw area in dimension X
			if (iRelX > posRelBefore.iX) {
				rctRedraw.iCX += iRelX - posRelBefore.iX;
			}
			else if (iRelX < posRelBefore.iX) {
				rctRedraw.iX = iRelX;
				rctRedraw.iCX += posRelBefore.iX - iRelX;
			}
			// Maybe expand the redraw area in dimension Y
			if (iRelY > posRelBefore.iY) {
				rctRedraw.iCY += iRelY - posRelBefore.iY;
			}
			else if (iRelY < posRelBefore.iY) {
				rctRedraw.iY = iRelY;
				rctRedraw.iCY += posRelBefore.iY - iRelY;
			}
			// Redraw the area that needs it
			SPos posAbsThis = GetAbsPos();			
			rctRedraw.iX += posAbsThis.iX;
			rctRedraw.iY += posAbsThis.iY;
			rctRedraw.iCX++;
			rctRedraw.iCY++;
			this->Redraw(rctRedraw);
		}
	}
}

void CPanner::MoveHandleAbsPos(tint32 iAbsX, tint32 iAbsY)
{
	if (mpBmpHndl) {
		SPos posAbsThis = this->GetAbsPos();

		// Calc new X
		tint32 iRelX = iAbsX - posAbsThis.iX - (msizeBmpHndl.iCX >> 1);
		
		// Calc new Y
		tint32 iRelY = iAbsY - posAbsThis.iY - (msizeBmpHndl.iCY >> 1);
		
		// Move handle bitmap
		MoveHandleRelPos(iRelX, iRelY);
	}
}

tbool CPanner::OnMouse(EMouseMsg MouseMsg, const SPos& posAbsMouse)
{
	if ((IsVisible() == false) || (mpBmpHndl == NULL)) {
		return false;
	}

	SPos posAbsArea = this->GetAbsPos();
	posAbsArea += mposRelArea;
	SRect rctAbsArea(posAbsArea, this->GetSize());
	
	SPos posAbsBmp;
	mpBmpHndl->GetAbsPos(posAbsBmp);
	SRect rctAbsBitmap(posAbsBmp, msizeBmpHndl);
	
	if (MouseMsg == LeftButtonDblClk) {
		if (rctAbsArea.Inside(posAbsMouse) == true) {
			EditBegin();
			SetToDefaultValue();
			EditEnd();
			return true;
		}
	}

	if (MouseMsg == LeftButtonDown) {
		if (rctAbsArea.Inside(posAbsMouse) == true) {
			if (mbDirectMouseXlat) {
				if (!rctAbsBitmap.Inside(posAbsMouse)) {
					if (mbJumpToMouseClickPos) {
						// Click was inside pane but not on top of handle bitmap - so move it
						MoveHandleAbsPos(posAbsMouse.iX, posAbsMouse.iY);
					}
					else {
						// We must ignore this click
						// False => Didn't handle the click, so pass on through
						return false;
					}
				}
				
				// Prepare for dragging
				mbMouseDown = true;
				mDragState = PannerDragDragging;
				dynamic_cast<CWindow*>(GetParentWindow())->GetMouseFocus(this);
				return true;
			}

			if (GetParentWindow()->ResetPressed()) {
				SetToDefaultValue();
				return true;
			}
			
			mbMouseDown = true;
			miValueXDragStart = mpKnobX->GetValue();
			miValueYDragStart = mpKnobY->GetValue();
			mPosDragStart = posAbsMouse;
			mDragState = PannerDragDragging;
			GetParentWindow()->HideMouse();
			mMouseMoved = SSize(0, 0);
			miMouseMoveCounter = 0;
			mbFineAdjustLast = GetParentWindow()->FineAdjustPressed();
			dynamic_cast<CWindow*>(GetParentWindow())->GetMouseFocus(this);
			EditBegin();
			return true;
			// .. Lasse
		}

		return false;
	}

	if (MouseMsg == LeftButtonUp) {
		if (mbDirectMouseXlat) {
			mbMouseDown = false;
			if (mDragState == PannerDragDragging) {
				MoveHandleAbsPos(posAbsMouse.iX, posAbsMouse.iY);
				mDragState = PannerDragNormal;
				dynamic_cast<CWindow*>(GetParentWindow())->ReleaseMouseFocus();
				EditEnd();
				return true;
			}
		}

		mbMouseDown = false;
		if (mDragState == PannerDragDragging) {
			mDragState = PannerDragNormal;
			dynamic_cast<CWindow*>(GetParentWindow())->ReleaseMouseFocus();
			GetParentWindow()->SetMousePos(mPosDragStart);
			GetParentWindow()->ShowMouse();
			EditEnd();
			return true;
		}
		// .. Lasse
	}


	if (MouseMsg == MouseMove) {
		if (mbDirectMouseXlat) {
			if (mDragState == PannerDragDragging) {
				MoveHandleAbsPos(posAbsMouse.iX, posAbsMouse.iY);
				return true;
			}
		}

		if (mDragState == PannerDragDragging) {
			if (mbFineAdjustLast != GetParentWindow()->FineAdjustPressed()) {
				miValueXDragStart = mpKnobX->GetValue();
				miValueYDragStart = mpKnobY->GetValue();
				mPosDragStart = posAbsMouse;
				mMouseMoved = SSize(0, 0);
				miMouseMoveCounter = 0;
				mbFineAdjustLast = GetParentWindow()->FineAdjustPressed();
			}
			
			// Compensate mouse position with the amount we've moved the mouse
			mMousePos = posAbsMouse - SPos(mMouseMoved.iCX, mMouseMoved.iCY);
			
			tint64 iPositions = (tint64)gControlMax - (tint64)gControlMin + 1;
			tfloat fPixelsPerPosition = 200.0f / iPositions;
			tint32 iDeltaX = mPosDragStart.iX - mMousePos.iX;
			tint32 iDeltaY = mPosDragStart.iY - mMousePos.iY;
			tint32 iPositionDeltaX = -Float2Int(iDeltaX / fPixelsPerPosition);
			tint32 iPositionDeltaY = Float2Int(iDeltaY / fPixelsPerPosition);
			if (mbFineAdjustLast) {
				iPositionDeltaX /= 10;
				iPositionDeltaY /= 10;
			}
			tint32 iValueNewX = miValueXDragStart + iPositionDeltaX;
			tint32 iValueNewY = miValueYDragStart + iPositionDeltaY;
			mpKnobX->SetValue(iValueNewX);
			mpKnobY->SetValue(iValueNewY);
			
#ifdef WIN32
			// Move the mouse position, to avoid leaving the screen area
			// Lasse: Look here...
/*			if (miMouseMoveCounter++ == 10) {
				GetParentWindow()->SetMousePos(mPosDragStart);
				mMouseMoved.iCX += mPosDragStart.iX - Pos.iX;
				mMouseMoved.iCY += mPosDragStart.iY - Pos.iY;
				miMouseMoveCounter = 0;
			}*/
#endif
			
			return true;
		}
		// .. Lasse
	}

	return false;
}

void CPanner::SetToDefaultValue()
{
	dynamic_cast<CControl*>(mpKnobX)->SetToDefaultValue();
	dynamic_cast<CControl*>(mpKnobY)->SetToDefaultValue();
	//mpBmpHndl->SetPos(SPos(msizeAreaCorrected.iCX >> 1, msizeAreaCorrected.iCY >> 1) + mposRelArea);
	CalcHandlePositionFromValues();
}

tbool CPanner::OnKeyDown(EKey /*Key*/)
{
	return false;
}

void CPanner::OnTimer()
{
	CPane::OnTimer();
}

void CPanner::EventValueChange(IControl *pSender, tint32 iValueNew)
{
	IKnob* pKnob = dynamic_cast<IKnob*>(pSender);
	if ((pKnob == mpKnobX) || (pKnob == mpKnobY)) {
		CalcHandlePositionFromValues();
	}
}


void CPanner::SetDirectMouseXlat(tbool bDirectXlat /*= true*/)
{
	mbDirectMouseXlat = bDirectXlat;
}

void CPanner::SetJumpToMouseClickPos(tbool bJumpToMouse /*= true*/)
{
	mbJumpToMouseClickPos = bJumpToMouse;
}
