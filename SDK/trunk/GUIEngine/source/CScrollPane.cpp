/*!	\file CScrollPane.cpp
	\author Michael Olsen
	\date 16/Mar/2005
	\date 16/Mar/2005
*/


#include "geInternalOS.h"


CScrollPane::CScrollPane()
	: mpScrollBar(NULL)
{
	mbIsSliding = false;
	mpTimer = NULL;
	miTimerID = -1;
	//miSliderPreventDisasterLevel = 0;
}

CScrollPane::~CScrollPane()
{
	if (mpTimer)
		mpTimer->Destroy();
	mpTimer = NULL;
}

IScrollPane* IScrollPane::Create()
{
	return dynamic_cast<IScrollPane*>(new CScrollPane());
}

void CScrollPane::Destroy()
{
	delete dynamic_cast<CScrollPane*>(this);
}

void CScrollPane::Init(tint32 iID, tint32 iTimerID)
{
	CControl::Init(iID);
	miTimerID = iTimerID;

	IPane* pSubPane = IPane::Create();
	CPane::AddControl(pSubPane, SPos(0, 0));
	// Disable the drawing rect check for subpane (to avoid problem with missing redraws)
	pSubPane->DisableDrawingRectCheck();
}

void CScrollPane::SetScrollPos(const SScrollPos& spNew_const, bool bRedraw)
{
	// If VisibleRect.iX or .iY is negative this was caused by resize peculiarity.
	// We must fix it before comparing (to avoid unnecessary screen updates)
	SScrollPos spNew = spNew_const;
	if (spNew.VisibleRect.iX < 0)
		spNew.VisibleRect.iX = 0;
	if (spNew.VisibleRect.iY < 0)
		spNew.VisibleRect.iY = 0;

	// Update if necessary
	SScrollPos spOld;
	GetScrollPos(spOld);
	if (spNew != spOld) {
		// Update the sub-pane position
		SPos posNew(-spNew.VisibleRect.iX, -spNew.VisibleRect.iY);
		GetSubPane()->SetRelPos(posNew);

		// Set sizes - do sub-pane twice to circumvent possible minimum-size restrictions
		GetSubPane()->SetSize(spNew.AreaSize, false);
		SetSize((SSize)spNew.VisibleRect, false);//bRedraw);
		GetSubPane()->SetSize(spNew.AreaSize, false);

		// If relative position of sub-pane is positive this was caused by resize peculiarity. Fix it
		posNew = ge::SPos(-spNew.VisibleRect.iX, -spNew.VisibleRect.iY);
		if (posNew.iX > 0)
			posNew.iX = 0;
		if (posNew.iY > 0)
			posNew.iY = 0;

		// Update the control positions
		GetSubPane()->SetRelPos(posNew);

		// Lasse, out-comment 2008-05-09 - redundant data
		/*
		// Save the positions
		mScrollPos = spNew;
		*/
		// .. Lasse

		// Lasse, this very important part got deleted!!!! - added again 2008-05-09
		// Signal to scroll-bar that it should update
		SetValue(0);
		// .. Lasse

	}

	// Lasse, added 2008-05-13
	// Update slave scroll-panes
	UpdateSlaves();
	// .. Lasse

	if (bRedraw) {
		// Redraw
		CControl::Redraw();
	}
}

void CScrollPane::SuppressUpdatePositions(tbool bSuppress)
{
	GetSubPane()->SuppressUpdatePositions(bSuppress);
}

void CScrollPane::GetScrollPos(SScrollPos& ScrollPos)
{
	// Lasse, modify 2008-05-09 - avoid redundant data
	/*
	ScrollPos = mScrollPos;
	*/
	SPos posSub;
	SSize sizeSub;
	GetSubPane()->GetRelPos(posSub);
	GetSubPane()->GetSize(sizeSub);
	ScrollPos.AreaSize = sizeSub;
	ScrollPos.VisibleRect = SRect(SPos(-posSub.iX, -posSub.iY), GetSize());
	// .. Lasse
}

void CScrollPane::AddControl(IControl* pToAdd, const SPos& Pos)
{
	GetSubPane()->AddControl(pToAdd, Pos);
}

void CScrollPane::RemoveControl(IControl* pToRemove)
{
	GetSubPane()->RemoveControl(pToRemove);
}

void CScrollPane::SetSize(const SSize& Size, tbool bRedraw)
{
	// Even if bRedraw is true we can't redraw until all has been sanitized
	const tbool DONT_REDRAW_YET = false;
	
	// Lasse, not needed?
	/*
	// Keep copy of original sizes to check if we actually resized anything
	SScrollPos spBefore = mScrollPos;
	*/

	// Resize outer pane - this may auto-resize inner-pane if SetAutomaticResizeOfSubPane(...) has been called
	CPane::SetSize(Size, DONT_REDRAW_YET);

	// Read back outer pane size
	SSize sizeOuter;
	CPane::GetSize(sizeOuter);
	// Lasse, modify 2008-05-09 - don't keep redundant data
	/*
	mScrollPos.VisibleRect.iCX = sizeOuter.iCX;
	mScrollPos.VisibleRect.iCY = sizeOuter.iCY;
	//mScrollPos.VisibleRect.iCX = mSize.iCX;
	//mScrollPos.VisibleRect.iCY = mSize.iCY;
	*/
	// .. Lasse

	// Read back size of inner pane
	// Lasse, modify 2008-05-09 - don't keep redundant data
	/*
	GetSubPane()->GetSize(mScrollPos.AreaSize);
	*/
	SSize sizeSub;
	GetSubPane()->GetSize(sizeSub);
	// .. Lasse

	// Sanitize inner pane size - it must *never* be smaller than VisibleRect
	tbool bFixSubSize = false;
	// Lasse, modify 2008-05-09 - don't keep redundant data
	/*
	if (mScrollPos.AreaSize.iCX < mScrollPos.VisibleRect.iCX) {
		mScrollPos.AreaSize.iCX = mScrollPos.VisibleRect.iCX;
		bFixSubSize = true;
	}
	if (mScrollPos.AreaSize.iCY < mScrollPos.VisibleRect.iCY) {
		mScrollPos.AreaSize.iCY = mScrollPos.VisibleRect.iCY;
		bFixSubSize = true;
	}
	if (bFixSubSize) {
		GetSubPane()->SetSize(mScrollPos.AreaSize);
	}
	*/
	if (sizeSub.iCX < sizeOuter.iCX) {
		sizeSub.iCX = sizeOuter.iCX;
		bFixSubSize = true;
	}
	if (sizeSub.iCY < sizeOuter.iCY) {
		sizeSub.iCY = sizeOuter.iCY;
		bFixSubSize = true;
	}
	if (bFixSubSize) {
		GetSubPane()->SetSize(sizeSub);
	}
	// .. Lasse

	// Lasse, added 2005-08-13
	// Sanitize position of sub-pane
	// If a resize causes sub-pane to be scrolled down/right further than there is content area, fix it
	// Note! We don't need to check for sub-pane size sanity - that was taken care of above
	SPos posRelSub;
	GetSubPane()->GetRelPos(posRelSub);
	if ((sizeSub.iCX + posRelSub.iX) < sizeOuter.iCX) {
		posRelSub.iX = sizeOuter.iCX - sizeSub.iCX;
	}
	if ((sizeSub.iCY + posRelSub.iY) < sizeOuter.iCY) {
		posRelSub.iY = sizeOuter.iCY - sizeSub.iCY;
	}
	GetSubPane()->SetRelPos(posRelSub);
	// .. Lasse

	// Lasse, added 2008-05-13
	// Update slave scrollpanes
	UpdateSlaves();
	// .. Lasse

	// Now we're ready to draw
	if (bRedraw) {
		// Lasse, not needed?
		/*
		tbool bAnyReason = (mScrollPos != spBefore);
		if (bAnyReason) {
			CPane::Redraw(mScrollPos.VisibleRect);
		}
		*/
		// .. Lasse
		CPane::Redraw(GetRect());
	}

	if (mpScrollBar) {
		// Lasse, mod 2008-05-09 - not enough to set size
		/*
		mpScrollBar->ScrollPaneSizeChanged(mSize);
		*/
		SScrollPos spNew, spOld;
		GetScrollPos(spNew);
		mpScrollBar->GetScrollPos(spOld);
		if (spNew != spOld)
			mpScrollBar->SetScrollPos(spNew, true);//bRedraw);
		// .. Lasse
	}
}

void CScrollPane::SetAutomaticResizeOfSubPane(tbool bEnabled /*= true*/, EResizeMode Mode /*= ResizeAbsolute*/, tfloat fValue /*= 1.0f*/)
{
	GetSubPane()->SetAutomaticResize(bEnabled, Mode, fValue);
}

void CScrollPane::SetMinSizeOfSubPane(SSize sizeMin)
{
	GetSubPane()->SetMinSize(sizeMin);
}

void CScrollPane::GetSizeOfSubPane(SSize& sizeSub)
{
	GetSubPane()->GetSize(sizeSub);
}

void CScrollPane::SetSize(const SSize& Size)
{
	SetSize(Size, false);
}

IControl* CScrollPane::GetControl(tint32 iControlID)
{
	if (iControlID == GetID()) {
		return dynamic_cast<IControl*>(this);
	}

	return GetSubPane()->GetControl(iControlID);
}

void CScrollPane::OnDraw(const SRect &rUpdate)
{
	if (!IsVisible()) {
		return;
	}

	if (mbBackgroundColour) {
		ASSERT(mpDrawPrimitives);
		if (mbDrawPrimitivesInited == false) {
			mpDrawPrimitives->Init(GetParentWindow());

			mbDrawPrimitivesInited = true;
		}
		mpDrawPrimitives->DrawRect(rUpdate, GetRect(), mColour);
	}

	SRect RctUpdate(rUpdate);
	// Is our rect inside the update rect?
	if (RctUpdate.Inside(GetRect())) {
		// It is inside. Make the update rect fit inside our size
		RctUpdate.FitInside(GetRect());

		GetSubPane()->OnDraw(RctUpdate);
	}
}

tbool CScrollPane::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	if (!IsVisible()) {
		return false;
	}

/*	SPos Pos2(Pos);
	Pos2.iX += mScrollPos.VisibleRect.iX;
	Pos2.iY += mScrollPos.VisibleRect.iY;

	return GetSubPane()->OnMouse(MouseMsg, Pos2);*/

	// Only allow mouse if within visible rect
	// Lasse, mod 2008-05-09 - we no longer keep redundant info
	/*
	SPos SubPanePos;
	GetSubPane()->GetPos(SubPanePos);
	if (mScrollPos.VisibleRect.Inside(Pos - SubPanePos)) {
		return GetSubPane()->OnMouse(MouseMsg, Pos);
	}
	*/
	SPos PosThis;
	GetPos(PosThis);
	if (SRect(PosThis, GetSize()).Inside(Pos)) {
		return GetSubPane()->OnMouse(MouseMsg, Pos);
	}
	// .. Lasse
	else {
		return false;
	}
}

void CScrollPane::GetMousePos(SPos& rPos) const
{
	CPane::GetMousePos(rPos);
}

void CScrollPane::SetControlValue(tint32 iControlID, tint32 iValueNew, bool bNotify)
{
	GetSubPane()->SetControlValue(iControlID, iValueNew, bNotify);
}

void CScrollPane::EventValueChange(IControl* /*pSender*/, tint32 /*iValueNew*/)
{
}

tbool CScrollPane::SlideTo(const SPos& posTarget, tint32 iMethod, tint32 iPctJump, tint32 iPctSlowest)
{
	if (miTimerID <= 0)
		return false;

	mposTarget = posTarget;
	miSlideMethod = iMethod;

	// Lasse, mod 2008-05-09 - we no longer keep redundant data
	/*
	tint32 iXDist = mposTarget.iX - mScrollPos.VisibleRect.iX;
	if (iXDist < 0)
		iXDist = -iXDist;
//	else if (iXDist == 0)
//		iXDist = 1;

	tint32 iYDist = mposTarget.iY - mScrollPos.VisibleRect.iY;
	if (iYDist < 0)
		iYDist = -iYDist;
//	else if (iYDist == 0)
//		iYDist = 1;
	*/
/*	SPos posSub;
	GetSubPane()->GetRelPos(posSub);
	tint32 iXDist = mposTarget.iX - posSub.iX;
	if (iXDist < 0)
		iXDist = -iXDist;
	tint32 iYDist = mposTarget.iY - posSub.iY;
	if (iYDist < 0)
		iYDist = -iYDist;
	// .. Lasse*/
	SScrollPos ScrollPos;
	GetScrollPos(ScrollPos);
	tint32 iXDist = mposTarget.iX - ScrollPos.VisibleRect.iX;
	if (iXDist < 0) {
		iXDist = -iXDist;
	}

	tint32 iYDist = mposTarget.iY - ScrollPos.VisibleRect.iY;
	if (iYDist < 0) {
		iYDist = -iYDist;
	}

	if (iXDist == 0) {
		if (iYDist == 0) {
			// No sliding neccasary
			return true;
		}
	}
	
	msizeSlideJump = SSize((iXDist * iPctJump) / 100, (iYDist * iPctJump) / 100);
	msizeSlideSlowest = SSize((iXDist * iPctSlowest) / 100, (iYDist * iPctSlowest) / 100);;
	mbIsSliding = true;
	if (mpTimer == NULL) {
		//CAutoDelete<ITimer>pTimer(ITimer::Create());
		mpTimer = ITimer::Create();
		mpTimer->Init(miTimerID, this, 20);
	}

	return true;
}


void CScrollPane::OnTimer(tint32 iTimerID)
{
	if ((mbIsSliding) && (iTimerID == miTimerID)) {
		if (true) {//(++miSliderPreventDisasterLevel == 1) {
			// Lasse, mod 2008-05-09 - we no longer keep redundant data
			/*
			SScrollPos scrlpos = mScrollPos;
			*/
			SScrollPos scrlpos;
			GetScrollPos(scrlpos);
			// .. Lasse
			
			tint32 iXDistance = mposTarget.iX - scrlpos.VisibleRect.iX;
			tint32 iXMove = CalcJump(miSlideMethod, iXDistance, msizeSlideJump.iCX, msizeSlideSlowest.iCX);
			
			tint32 iYDistance = mposTarget.iY - scrlpos.VisibleRect.iY;
			tint32 iYMove = CalcJump(miSlideMethod, iYDistance, msizeSlideJump.iCY, msizeSlideSlowest.iCY);
			
			if ((iXMove == 0) && (iYMove == 0)) {
				mbIsSliding = false;
//				mpTimer->Destroy();
//				mpTimer = NULL;
			}
			else {
				scrlpos.VisibleRect.iX += iXMove;
				scrlpos.VisibleRect.iY += iYMove;
				SetScrollPos(scrlpos, true);
			}
		}
		//miSliderPreventDisasterLevel--;
	}
}

/*
void DoMsgBox(const tchar* psz) {
std::string s(psz);
#ifdef _Mac
// On OSX we can only show 255 characters in a dialogue box, so we need to limit the string
if (s.size() > 240) {
	s = s.substr(0, 240);
}
#endif	// _Mac

Str255 String;
// Convert to pascal string
String[0] = s.size();
strcpy((char*)&(String[1]), s.c_str());

AlertStdAlertParamRec ParamRec;
ParamRec.movable = 1;
ParamRec.helpButton = 0;
ParamRec.filterProc = nil;
ParamRec.defaultButton = kAlertStdAlertOKButton;
ParamRec.cancelButton = kAlertStdAlertCancelButton;
ParamRec.position = kWindowAlertPositionParentWindowScreen;
ParamRec.defaultText = "\pOK";
ParamRec.cancelText = nil;
ParamRec.otherText = nil;

SInt16 iItemHit;
::StandardAlert(kAlertPlainAlert,
				String,
				NULL,
				&ParamRec,
				&iItemHit);
}
*/

void CScrollPane::OnTimer()
{
	if ((mpTimer) && (!mbIsSliding)) {
		// A timer is running even though sliding is finished
		if (true) {//++miSliderPreventDisasterLevel == 1) {
			// Stop timer
			mpTimer->Destroy();
			mpTimer = NULL;
		}
		//miSliderPreventDisasterLevel--;
	}

	CPane::OnTimer();
}


int CScrollPane::CalcJump(tint32 iMethod, tint32 iDistance, tint32 iJump, tint32 iSlowest)
{
	tint32 iMove = 0;
	if (iDistance != 0) {
		tbool bNeg = false;
		if (iDistance < 0) {
			iDistance = -iDistance;
			bNeg = true;
		}
		switch (iMethod) {
			case 0:
			default:
				if (iDistance > iJump)
					iMove = iJump;
				else {
					iMove = (iDistance / 2);
					if (iMove < iSlowest)
						iMove = iDistance;
				}
					break;
		}
		if (bNeg)
			iMove = -iMove;
	}
	return iMove;
}

void CScrollPane::AddSlaveScrollPane(IScrollPane* pSlave, EResizeMode eMode)
{
	SSlave sSlave(pSlave, eMode);
	mlistSlaves.insert(mlistSlaves.end(), sSlave);
}

tbool CScrollPane::RemoveSlaveScrollPane(IScrollPane* pSlave)
{
	tbool bRemoved = false;

	std::list<SSlave>::iterator it = mlistSlaves.end();
	while (it!=mlistSlaves.begin()) {
		it--;
		SSlave& sSlave = *it;
		if (sSlave.mpSlave == pSlave) {
			mlistSlaves.erase(it);
			bRemoved = true;
		}
	}

	return bRemoved;
}

void CScrollPane::UpdateSlaves()
{
	if (!mlistSlaves.empty()) {
		SScrollPos spThis;
		GetScrollPos(spThis);

		std::list<SSlave>::iterator it = mlistSlaves.begin();
		for ( ; it != mlistSlaves.end(); it++) {
			SSlave& sSlave = *it;
			switch (sSlave.meMode) {
				case ResizeAbsolute:
					sSlave.mpSlave->SetScrollPos(spThis);
					break;
				case ResizeAbsoluteX:
					{
						SScrollPos spSlave;
						sSlave.mpSlave->GetScrollPos(spSlave);
						spSlave.AreaSize.iCX = spThis.AreaSize.iCX;
						spSlave.VisibleRect.iCX = spThis.VisibleRect.iCX;
						spSlave.VisibleRect.iX = spThis.VisibleRect.iX;
						sSlave.mpSlave->SetScrollPos(spSlave);
					}
					break;
				case ResizeAbsoluteY:
					{
						SScrollPos spSlave;
						sSlave.mpSlave->GetScrollPos(spSlave);
						spSlave.AreaSize.iCY = spThis.AreaSize.iCY;
						spSlave.VisibleRect.iCY = spThis.VisibleRect.iCY;
						spSlave.VisibleRect.iY = spThis.VisibleRect.iY;
						sSlave.mpSlave->SetScrollPos(spSlave);
					}
					break;
				default:
					// This is not supported
					break;
			}
		}
	}
}