/*!	\file CPane.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/



#include "geInternalOS.h"


CPane::CPane()
	: mpParentWindow(NULL),
	mbDoDrawingRectCheck(true),
	mbCacheControls(true),
	mbBackgroundColour(false), mpDrawPrimitives(NULL)
{
//	SetAutomaticResize(true, ResizeAbsolute, 1.0f);

	mbSuppressUpdatePositions = false;
}

CPane::~CPane()
{
	if (mpDrawPrimitives) {
		mpDrawPrimitives->Destroy();
	}

	// Traverse list of controls/panes and destroy
	std::list<IControl*>::iterator it = mControls.begin();
	for (; it != mControls.end(); it++) {
		IControl* pControl = *it;
		pControl->Destroy();
	}
	mControls.clear();
}

IPane* IPane::Create()
{
	return dynamic_cast<IPane*>(new CPane());
}

void CPane::Destroy()
{
	delete dynamic_cast<CPane*>(this);
}

tbool CPane::IsTopPane() const
{
	return (GetParentPane() == NULL);
}

void CPane::SetParentWindow(IWindow* pWindow)
{
	mpParentWindow = pWindow;
}

IWindow* CPane::GetParentWindow()
{
	if (IsTopPane()) {
		return mpParentWindow;
	}
	else {
		return GetParentPane()->GetParentWindow();
	}
}

const IWindow* CPane::GetParentWindow() const
{
	if (IsTopPane()) {
		return mpParentWindow;
	}
	else {
		return GetParentPane()->GetParentWindow();
	}
}

void CPane::SetSize(const SSize& Size, tbool bRedraw)
{
	if (bRedraw) {
		SRect rct = GetRect();
		Redraw(rct);
	}

//	mSize = Size;
	CControl::SetSize(Size);

	if (bRedraw) {
		SRect rct = GetRect();
		Redraw(rct);
	}
}

void CPane::DisableControlCaching()
{
	mbCacheControls = false;
}

void CPane::SetBackgroundColour(SRGB Colour)
{
	if (mpDrawPrimitives == NULL) {
		mpDrawPrimitives = IDrawPrimitives::Create();
		mbDrawPrimitivesInited = false;
	}

	mColour = Colour;

	mbBackgroundColour = true;
}

IControl* CPane::GetControl(tint32 iControlID)
{
	return GetParentWindow()->GetControl(iControlID);

/*	if (IsTopPane()) {
		// Check if we have the control in our map
		// (mo) If control has previously been removed, this may crash... code should be updated
		std::map<tint32, IControl*>::const_iterator it = mControlMap.find(iControlID);
		if (it != mControlMap.end()) {
			// It's there, return it
			IControl* pControl = it->second;
			return pControl;
		}
	}

	// If this has the right id, return it
	if (GetID() == iControlID) {
		return this;
	}

	// Since it is not this, iterate all our controls
	std::list<IControl*>::iterator it = mControls.begin();
	for (; it != mControls.end(); it++) {
		IControl* pControl = *it;

		// Check if it is this control
		if (pControl->GetID() == iControlID) {
			return pControl;
		}

		// If this control is a pane, call to see if it knows which control it is
		IPane* pPane = dynamic_cast<IPane*>(pControl);
		if (pPane) {
			IControl* pRet = pPane->GetControl(iControlID);
			if (pRet) {
				if (IsTopPane()) {
					// Put it in map
					if (mbCacheControls) {
						mControlMap.insert(std::pair<tint32, IControl*>(iControlID, pRet));
					}
				}
				return pRet;
			}
		}
	}

	// We didn't find it, return NULL
	// (mo) As an optimization we store the NULL in the map, but that will cause problems if the control is later created
	if (mbCacheControls) {
		mControlMap.insert(std::pair<tint32, IControl*>(iControlID, NULL));
	}
	return NULL;*/
}

void CPane::SetControlValue(tint32 iControlID, tint32 iValueNew, tbool bNotify)
{
	if (iControlID == GetID()) {
		SetConvertedValue(iValueNew, bNotify);
		return;
	}

	IControl* pControl = GetParentWindow()->GetControl(iControlID);
	if (pControl) {
		pControl->SetControlValue(iControlID, iValueNew, bNotify);
	}

/*	// It may seem strange that we check our own ID, since a pane is not supposed to use its value for anything,
	// but since some controls might derive from CPane (i.e. CRadioButtons) it makes perfect sense
	if (iControlID == GetID()) {
		SetConvertedValue(iValueNew, false);
	}
	else {
		std::list<IControl*>::iterator it = mControls.begin();
		for (; it != mControls.end(); it++) {
			IControl* pControl = *it;
			pControl->SetControlValue(iControlID, iValueNew);
		}
	}*/
}

void CPane::ControlUpdateAll()
{
	std::list<IControl*>::iterator it = mControls.begin();
	for (; it != mControls.end(); it++) {
		IControl* pControl = *it;

		CPane* pPane = dynamic_cast<CPane*>(pControl);
		if (pPane) {
			// This is a pane, so call ControlUpdateAll
			pPane->ControlUpdateAll();
		}
		else {
			// This is not a pane, so make it send an update
			pControl->NotifyValueChange();
		}
	}
}

void CPane::OnDraw(const SRect &rctUpdate)
{
	if (IsVisible() == false) {
		return;
	}

	// Check if we need to draw this pane at all
	SRect rctThis = GetRect();
	// (mo) If one of these asserts are triggered, then most likely the panes size has not been set
	ASSERT(rctThis.iCX > 0);
	ASSERT(rctThis.iCY > 0);
	if (mbDoDrawingRectCheck) {
		if (rctThis.Inside(rctUpdate) == false) {
			return;
		}
	}

	if (mbBackgroundColour) {
		ASSERT(mpDrawPrimitives);
		if (mbDrawPrimitivesInited == false) {
			mpDrawPrimitives->Init(GetParentWindow());

			mbDrawPrimitivesInited = true;
		}
		mpDrawPrimitives->DrawRect(rctUpdate, rctThis, mColour);
	}

	// Fit inside our rect, so we don't draw outside it in subpanes
	SRect RctUpdate2 = rctUpdate;
	RctUpdate2.FitInside(rctThis);

	std::list<IControl*>::iterator it = mControls.begin();
	for (; it != mControls.end(); it++) {
		IControl* pControl = *it;
//		if (mbDoDrawingRectCheck) {
//			pControl->OnDraw(RctUpdate2);
//		}
//		else {
			pControl->OnDraw(rctUpdate);
//		}
	}
}

tbool CPane::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	if (IsVisible() == false) {
		return false;
	}

/*	std::list<IControl*>::iterator it = mControls.end();
	it--;
	tint iNrOfControls = mControls.size();
	tint iControl;
	for (iControl = iNrOfControls - 1; iControl >= 0; iControl--, it--) {
		IControl* pControl = *it;
		if (dynamic_cast<CControl*>(pControl)->IsEnabled()) {
			if (pControl->OnMouse(MouseMsg, Pos) == true) {
				return true;
			}
		}
	}*/
	std::list<IControl*>::reverse_iterator it = mControls.rbegin();
	for (; it != mControls.rend(); it++) {
		IControl* pControl = *it;
		ASSERT(pControl);
		if (pControl->IsEnabled()) {
			if (pControl->OnMouse(MouseMsg, Pos) == true) {
				return true;
			}
		}
	}

	return false;
}

tbool CPane::OnKeyDown(EKey Key)
{
/*	std::list<IControl*>::iterator it = mControls.end();
	it--;
	tint iNrOfControls = mControls.size();
	tint iControl;
	for (iControl = iNrOfControls - 1; iControl >= 0; iControl--, it--) {
		IControl* pControl = *it;
		if (dynamic_cast<CControl*>(pControl)->IsEnabled()) {
			if (pControl->OnKeyDown(Key) == true) {
				return true;
			}
		}
	}*/
	std::list<IControl*>::reverse_iterator it = mControls.rbegin();
	for (; it != mControls.rend(); it++) {
		IControl* pControl = *it;
		if (pControl->IsEnabled()) {
			if (pControl->OnKeyDown(Key) == true) {
				return true;
			}
		}
	}

	return false;
}

void CPane::OnTimer()
{
	CControl::OnTimer();

	if (IsVisible() == false) {
		return;
	}

	std::list<IControl*>::iterator it = mControls.begin();
	for (; it != mControls.end(); it++) {
		IControl* pControl = *it;
		pControl->OnTimer();
	}
}

void CPane::AddControl(IControl* pToAdd, const SPos& Pos)
{
	ASSERT(dynamic_cast<CControl*>(pToAdd));

	mControls.insert(mControls.end(), pToAdd);

	pToAdd->SetParentPane(dynamic_cast<IPane*>(this));
	pToAdd->SetPos(Pos);

//	if (GetParentWindow()) {
//		dynamic_cast<CWindow*>(GetParentWindow())->ControlAdded(pToAdd->GetID(), pToAdd);
//	}
}

void CPane::RemoveControl(IControl* pToRemove)
{
	mControls.remove(pToRemove);
}

void CPane::AddControlsToWindow(CWindow* pWindow)
{
	std::list<IControl*>::iterator it = mControls.begin();
	for (; it != mControls.end(); it++) {
		IControl* pControl = *it;

		pWindow->ControlAdded(pControl->GetID(), pControl);

		CPane* pPane = dynamic_cast<CPane*>(pControl);
		if (pPane) {
			// This is a pane, so call the panes control adder
			pPane->AddControlsToWindow(pWindow);
		}
	}
}

void CPane::Redraw(const SRect& rct)
{
	if (IsTopPane()) {
		IWindow* pParentWindow = GetParentWindow();
		if (pParentWindow) {
			pParentWindow->Redraw(rct);
		}
	}
	else {
		GetParentPane()->Redraw(rct);
	}
}

void CPane::GetMousePos(SPos& rPos) const
{
	if (IsTopPane()) {
		GetParentWindow()->GetMousePos(rPos);
	}
	else {
		GetParentPane()->GetMousePos(rPos);
	}
}

void CPane::DisableDrawingRectCheck()
{
	mbDoDrawingRectCheck = false;
}

tbool CPane::IsAbsPosWithinAControl(const SPos& rPosAbsolute, tbool bConsiderDisabledControls /*= false*/)
{
	tbool bWithin = false;

	std::list<ge::IControl*>::iterator it = mControls.begin();
	for (; (!bWithin)&&(it != mControls.end()); it++) {
		IControl* pIControl = *it;
		if (pIControl->IsVisible()) {
			CControl* pCControl = dynamic_cast<CControl*>(pIControl);
			if ((bConsiderDisabledControls)||(pIControl->IsEnabled())) {
				IPopupMenu* pPopup = dynamic_cast<IPopupMenu*>(pIControl);
				if (pPopup) {
					// Ignore pop-ups - they're weird, may appear anywhere and have full mouse focus
				}
				else {
					SRect rectControl;
					pIControl->GetRect(rectControl);
					if (rectControl.Inside(rPosAbsolute)) {
						bWithin = true;
					}
				}
			}
		}
	}

	return bWithin;
} // CPane::IsAbsPosWithinAControl

void CPane::SuppressUpdatePositions(tbool bSuppress)
{
	mbSuppressUpdatePositions = bSuppress;
	if (!bSuppress) {
		// We must update everything now
		UpdatePositions(NULL);
	}
}

void CPane::UpdatePositions(const IControl* pExcludeFrom)
{
	if (mbSuppressUpdatePositions)
		return;

	tbool bUpdate = false;
	if (pExcludeFrom == NULL) {
		// No limitations on which controls to update
		bUpdate = true;
	}

	// Prev control for stick-to-sibling controls
	IControl* pPrev = NULL;
	std::list<IControl*>::iterator it = mControls.begin();
	for (; it != mControls.end(); it++) {
#ifdef _Mac
		IControl* pCTmp = *it;
		IControl* pC = dynamic_cast<IControl*>(pCTmp);
		if (pC == NULL) {
			pC = pCTmp;
		}
#else
		IControl* pC = dynamic_cast<IControl*>(*it);
#endif
		if (pC == pExcludeFrom) {
			// Skip this control, but update from next control
			bUpdate = true;
			pPrev = pC;
			continue;
		}
		else {
			if (bUpdate == false) {
				// Not there yet...
				pPrev = pC;
				continue;
			}
		}
		CControl* pControl = dynamic_cast<CControl*>(pC);

		EPosition Position = pControl->GetStickPosition();
		SPos PosOffset = pControl->GetStickOffset();
		EStickTo StickTo = pControl->GetStickTo();

		if (StickTo == StickToParent) {
			// We stick to parent
			SRect RctThis;
			{
				SPos Pos;
				SSize Size;
				GetRelPos(Pos);
				GetSize(Size);
				RctThis = SRect(Pos, Size);
			}
			SRect RctControl;
			{
				SPos Pos;
				SSize Size;
				pControl->GetRelPos(Pos);
				pControl->GetSize(Size);
				RctControl = SRect(Pos, Size);
			}

			switch(Position) {
				case PosNone:
					break;
				case PosTopLeft:
						pControl->SetPos(/*(SPos)RctThis +*/ PosOffset);
					break;
				case PosTopRight:
						pControl->SetPos(/*(SPos)RctThis +*/ SPos(RctThis.iCX, 0) - SPos(RctControl.iCX, 0) + PosOffset);
					break;
				case PosBottomLeft:
						pControl->SetPos(/*(SPos)RctThis +*/ SPos(0, RctThis.iCY) - SPos(0, RctControl.iCY) + PosOffset);
					break;
				case PosBottomRight:
						pControl->SetPos(/*(SPos)RctThis +*/ SPos(RctThis.iCX, RctThis.iCY) - SPos(RctControl.iCX, RctControl.iCY) + PosOffset);
					break;
				case PosCenter:
					{
						tint32 iX = RctThis.iX + RctThis.iCX / 2 - RctControl.iCX / 2;
						tint32 iY = RctThis.iY + RctThis.iCY / 2 - RctControl.iCY / 2;
						pControl->SetPos(SPos(iX, iY) + PosOffset);
					}
					break;
			}
		}
		else {
			// We stick to sibling
			SRect RctPrev;
			if (pPrev) {
				// We have a prev control, so use it
				SPos Pos;
				SSize Size;
				pPrev->GetRelPos(Pos);
				pPrev->GetSize(Size);
				RctPrev = SRect(Pos, Size);
			}
			else {
				// No prev control, so stick to top left(!)
				RctPrev = SRect(SPos(0, 0), SSize(0, 0));
			}
			SRect RctControl;
			{
				SPos Pos;
				SSize Size;
				pControl->GetRelPos(Pos);
				pControl->GetSize(Size);
				RctControl = SRect(Pos, Size);
			}

			switch(Position) {
				case PosNone:
					break;
				case PosTopLeft:
						pControl->SetPos((SPos)RctPrev + PosOffset);
					break;
				case PosTopRight:
						pControl->SetPos((SPos)RctPrev + SPos(RctPrev.iCX, 0) + PosOffset);
					break;
				case PosBottomLeft:
						pControl->SetPos((SPos)RctPrev + SPos(0, RctPrev.iCY) + PosOffset);
					break;
				case PosBottomRight:
						pControl->SetPos((SPos)RctPrev + SPos(RctPrev.iCX, RctPrev.iCY) + PosOffset);
					break;
				case PosCenter:
					{
						tint32 iX = RctPrev.iX + RctPrev.iCX / 2 - RctControl.iCX / 2;
						tint32 iY = RctPrev.iY + RctPrev.iCY / 2 - RctControl.iCY / 2;
						pControl->SetPos(SPos(iX, iY) + PosOffset);
					}
					break;
			}
		}
		pPrev = pControl;
	}
}

void CPane::UpdateSizes(SSize SizeDelta, tfloat32 fRatioX, tfloat32 fRatioY)
{
	std::list<IControl*>::iterator it = mControls.begin();
	for (; it != mControls.end(); it++) {
		IControl* pC = *it;
		CControl* pControl = dynamic_cast<CControl*>(pC);

		tbool bResizeEnabled = pControl->IsResizeEnabled();
		EResizeMode ResizeMode = pControl->GetResizeMode();
		tfloat fResizeRatio = pControl->GetResizeRatio();

		if (bResizeEnabled) {
			// Automatic resizing enabled for this control
			switch(ResizeMode) {
				case ResizeAbsolute:
					{
						SSize SizeCur;
						pControl->GetSize(SizeCur);
						pControl->SetSize(SizeCur + SizeDelta);
					}
					break;

				case ResizeAbsoluteX:
					{
						SSize SizeCur;
						pControl->GetSize(SizeCur);
						pControl->SetSize(SSize(SizeCur.iCX + SizeDelta.iCX, SizeCur.iCY));
					}
					break;

				case ResizeAbsoluteY:
					{
						SSize SizeCur;
						pControl->GetSize(SizeCur);
						pControl->SetSize(SSize(SizeCur.iCX, SizeCur.iCY + SizeDelta.iCY));
					}
					break;

				case ResizeRelative:
					{
						SSize SizeCur;
						pControl->GetSize(SizeCur);
						SSize SizeNew;
						SizeNew.iCX = (tint32)(SizeCur.iCX * fRatioX * fResizeRatio);
						SizeNew.iCY = (tint32)(SizeCur.iCY * fRatioY * fResizeRatio);
						pControl->SetSize(SizeNew);
					}
					break;
			}
		}
	}
}

// --------------------------------------------------
	// Drawing primitives
// --------------------------------------------------

void CPane::DrawBitmapFrame(IBitmapResource *pResource, const SRect &rctUpdate, const SRect &rct, tint iFrame, tint iNrOfFrames, tbool bGhost)
{
	DoDrawBitmap(pResource, rctUpdate, rct, iFrame, iNrOfFrames, bGhost);
}

void CPane::DrawFont(IFontResource *pResource, const SRect &prctUpdate, const SRect &rct, const tchar* psz)
{
	pResource->Draw(GetParentWindow(), prctUpdate, rct, std::string((char*)psz));
}

void CPane::DoDrawBitmap(IBitmapResource* pResource, const SRect& rctUpdate, const SRect& rct, tint iFrame, tint /*iNrOfFrames*/, tbool bGhost)
{
	pResource->Draw(GetParentWindow(), rctUpdate, rct, iFrame, bGhost);
}


