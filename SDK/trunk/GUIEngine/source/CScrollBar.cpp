/*!	\file CScrollBar.cpp
	\author Michael Olsen
	\date 17/Mar/2005
	\date 17/Mar/2005
*/


#include "geInternalOS.h"


CScrollBar::CScrollBar()
	: mpScrollPane(NULL), mbScrolling(false), mbExpandAreaSize(false)
{
	SetAutomaticResize(false);
}

CScrollBar::~CScrollBar()
{
	// Only these needs to be dec ref'ed, since we made controls with the others
	mppBitmapRes[BitmapLeftTopHandle]->DecRefCount();
	mppBitmapRes[BitmapRightDownHandle]->DecRefCount();
	mppBitmapRes[BitmapCenterHandle]->DecRefCount();
}

IScrollBar* IScrollBar::Create()
{
	return dynamic_cast<IScrollBar*>(new CScrollBar());
}

void CScrollBar::Destroy()
{
	delete dynamic_cast<CScrollBar*>(this);
}

void CScrollBar::AddControl(IControl* pToAdd, const SPos& Pos)
{
	if (mControls.size() < 2) {
		CPane::AddControl(pToAdd, Pos);
	}
	else {
		mControls.push_front(pToAdd);

		pToAdd->SetParentPane(dynamic_cast<IPane*>(this));
		pToAdd->SetPos(Pos);
	}
}

void CScrollBar::Init(tint32 iID, EType Type, IWindow* pWnd, IResourceManager* pResMan, tint32 piIDs[BitmapCount], IScrollPane* pScrollPane, const SScrollPos& ScrollPos)
{
	CControl::Init(iID);

	mpScrollPane = pScrollPane;
	mType = Type;
	mScrollPos = ScrollPos;

	// Lasse, this very important part got deleted!!!! - added again 2008-05-09
	// Prepare to catch update-signal from scroll-pane
	if (mpScrollPane)
		mpScrollPane->AddListener(this);
	// .. Lasse

	// Create resources for first 2 bitmaps, and get the sizes. Also create controls and add them
	tint iBitmap;
	for (iBitmap = 0; iBitmap < 2; iBitmap++) {
		mppBitmapRes[iBitmap] = pResMan->GetBitmap(pWnd, piIDs[iBitmap], 2, 1);
		mppBitmapRes[iBitmap]->GetRealSize(mpBitmapSizes[iBitmap]);

		IButton* pButton = IButton::Create();
		pButton->Init(-1, mppBitmapRes[iBitmap]);
		pButton->AddListener(dynamic_cast<IEventReceiver*>(this));

		AddControl(pButton, SPos(0, 0));
	}

	// Create resources for last 3 bitmaps, and get the sizes
	for (iBitmap = 2; iBitmap < BitmapCount; iBitmap++) {
		mppBitmapRes[iBitmap] = pResMan->GetBitmap(pWnd, piIDs[iBitmap], 1, 1);
		mppBitmapRes[iBitmap]->GetRealSize(mpBitmapSizes[iBitmap]);
	}

	SetSize((SSize)(mScrollPos.VisibleRect), false);

	switch(mType) {
		case TypeHorizontal:
			{
				// Left
				mSize.iCY = mpBitmapSizes[BitmapLeftTop].iCY;
			}
			break;
		case TypeVertical:
			{
				// Up
				mSize.iCX = mpBitmapSizes[BitmapLeftTop].iCX;
			}
			break;
	}

	PositionControls();

	dynamic_cast<CScrollPane*>(mpScrollPane)->SetScrollBar(dynamic_cast<CScrollBar*>(this));
}

void CScrollBar::SetScrollPos(const SScrollPos& ScrollPos, bool bRedraw)
{
	mScrollPos = ScrollPos;

	// Limit scrolling position
	if (mScrollPos.VisibleRect.iX < 0) {
		mScrollPos.VisibleRect.iX = 0;
	}
	if (mScrollPos.VisibleRect.iX > mScrollPos.AreaSize.iCX - mScrollPos.VisibleRect.iCX) {
		mScrollPos.VisibleRect.iX = mScrollPos.AreaSize.iCX - mScrollPos.VisibleRect.iCX;
	}
	if (mScrollPos.VisibleRect.iY < 0) {
		mScrollPos.VisibleRect.iY = 0;
	}
	if (mScrollPos.VisibleRect.iY > mScrollPos.AreaSize.iCY - mScrollPos.VisibleRect.iCY) {
		mScrollPos.VisibleRect.iY = mScrollPos.AreaSize.iCY - mScrollPos.VisibleRect.iCY;
	}

	SScrollPos scrollposOfScrollPane;
	mpScrollPane->GetScrollPos(scrollposOfScrollPane);
	if (bRedraw) {
		// Redraw and set scroll pos (which will cause scroll pane to redraw)
		CControl::Redraw();
		if (mScrollPos != scrollposOfScrollPane)
			mpScrollPane->SetScrollPos(mScrollPos, true);
	}
	else {
		if (mScrollPos != scrollposOfScrollPane)
			mpScrollPane->SetScrollPos(mScrollPos, false);
	}
}

void CScrollBar::GetScrollPos(SScrollPos& ScrollPos)
{
	if (mpScrollPane)
		mpScrollPane->GetScrollPos(ScrollPos);
	else
		ScrollPos = mScrollPos;
}


/*
void CScrollBar::ScrollPaneSizeChanged(const SSize& SizeNew)
{
	mScrollPos.VisibleRect.iCX = SizeNew.iCX;
	mScrollPos.VisibleRect.iCY = SizeNew.iCY;

	tbool bDoSet = false;

	/*
	// Sanity check: If areasize is smaller than visible rect we must correct it
	if (mScrollPos.AreaSize.iCX < mScrollPos.VisibleRect.iCX) {
		mScrollPos.AreaSize.iCX = mScrollPos.VisibleRect.iCX;
		bDoSet = true;
	}
	if (mScrollPos.AreaSize.iCY < mScrollPos.VisibleRect.iCY) {
		mScrollPos.AreaSize.iCY = mScrollPos.VisibleRect.iCY;
		bDoSet = true;
	}
	* /

	// Attempt to slide visible rect if neccasary
	if (mScrollPos.VisibleRect.iX > mScrollPos.AreaSize.iCX - mScrollPos.VisibleRect.iCX) {
		if (mbExpandAreaSize) {
			mScrollPos.AreaSize.iCX = mScrollPos.VisibleRect.iX + mScrollPos.VisibleRect.iCX;
		}
		else {
			mScrollPos.VisibleRect.iX = mScrollPos.AreaSize.iCX - mScrollPos.VisibleRect.iCX;
		}
		//mpScrollPane->SetScrollPos(mScrollPos, true);
		bDoSet = true;
	}
	if (mScrollPos.VisibleRect.iY > mScrollPos.AreaSize.iCY - mScrollPos.VisibleRect.iCY) {
		if (mbExpandAreaSize) {
			mScrollPos.AreaSize.iCY = mScrollPos.VisibleRect.iY + mScrollPos.VisibleRect.iCY;
		}
		else {
			mScrollPos.VisibleRect.iY = mScrollPos.AreaSize.iCY - mScrollPos.VisibleRect.iCY;
		}
		//mpScrollPane->SetScrollPos(mScrollPos, true);
		bDoSet = true;
	}

	// Anything needed changing?
	if (bDoSet) {
		mpScrollPane->SetScrollPos(mScrollPos, true);
	}
}
*/

void CScrollBar::ExpandAreaSize()
{
	mbExpandAreaSize = true;
}

void CScrollBar::SetSize(const SSize& Size)
{
	CPane::SetSize(Size, false);

	PositionControls();
}

void CScrollBar::SetSize(const SSize& Size, tbool bRedraw)
{
	CPane::SetSize(Size, bRedraw);

	PositionControls();
}

void CScrollBar::PositionControls()
{
	switch(mType) {
		case TypeHorizontal:
			{
				// Left
				std::list<IControl*>::iterator it = mControls.begin();
				if (mControls.size() > 2) {
					it++;
				}
				IControl* pControl = *it++;
				pControl->SetPos(SPos(0, 0));

				// Right
				pControl = *it;
				SPos Pos;
				Pos.iY = 0;
				Pos.iX = GetSize().iCX - mpBitmapSizes[BitmapRightDown].iCX;
				pControl->SetPos(Pos);
			}
			break;
		case TypeVertical:
			{
				// Up
				std::list<IControl*>::iterator it = mControls.begin();
				if (mControls.size() > 2) {
					it++;
				}
				IControl* pControl = *it++;
				pControl->SetPos(SPos(0, 0));

				// Down
				pControl = *it;
				SPos Pos2;
				Pos2.iY = GetSize().iCY - mpBitmapSizes[BitmapRightDown].iCY;
				Pos2.iX = 0;
				pControl->SetPos(Pos2);
			}
			break;
	}
}

SRect CScrollBar::GetHandleRect()
{
	SRect rctHandle;

	if (mpScrollPane)
		mpScrollPane->GetScrollPos(mScrollPos);

	switch(mType) {
		case TypeHorizontal:
			{
				// The leftmost top-left point is 1 pixel to the right of the top-right point of the left arrow
				std::list<IControl*>::iterator it = mControls.begin();
				if (mControls.size() > 2) {
					it++;
				}
				IControl* pControl = *it++;
				SRect RctArrow;
				pControl->GetRect(RctArrow);

				SPos Pos(RctArrow.iX + RctArrow.iCX, RctArrow.iY);

				// Calculate the relative position (top-left point)
				tint iDiff = mScrollPos.AreaSize.iCX - mScrollPos.VisibleRect.iCX;
				tfloat64 fPositionRelative;
				if (iDiff == 0) {
					fPositionRelative = 0;
				}
				else {
					fPositionRelative = (double)mScrollPos.VisibleRect.iX / iDiff;
				}

				// Calculate the maximum width of the handle
				pControl = *it;
				SPos PosRightArrow;
				pControl->GetPos(PosRightArrow);
				tint iMaxWidth = PosRightArrow.iX - Pos.iX;

				// Calculate the relative width we occupy
				tfloat64 fWidthRelative = mScrollPos.VisibleRect.iCX / (double)mScrollPos.AreaSize.iCX;

				// Calculate the absolute size
				rctHandle = SRect(Pos, SSize(0, 0));
				rctHandle.iCY = mpBitmapSizes[BitmapCenterHandle].iCY;
				rctHandle.iCX = (int)((fWidthRelative * iMaxWidth) + 0.5);

				// Calculate the absolute position
				rctHandle.iX += (int)((fPositionRelative * (iMaxWidth - rctHandle.iCX)) + 0.5);

				if (rctHandle.iCX < mpBitmapSizes[BitmapLeftTopHandle].iCX + mpBitmapSizes[BitmapRightDownHandle].iCX) {
					rctHandle.iCX = mpBitmapSizes[BitmapLeftTopHandle].iCX + mpBitmapSizes[BitmapRightDownHandle].iCX;
				}
			}
			break;
		case TypeVertical:
			{
				// The topmost top-left point is 1 pixel below the bottom-left point of the up arrow
				std::list<IControl*>::iterator it = mControls.begin();
				if (mControls.size() > 2) {
					it++;
				}
				IControl* pControl = *it++;
				SRect RctArrow;
				pControl->GetRect(RctArrow);

				SPos Pos(RctArrow.iX, RctArrow.iY + RctArrow.iCY);

				// Calculate the relative position (top-left point)
				tint iDiff = mScrollPos.AreaSize.iCY - mScrollPos.VisibleRect.iCY;
				tfloat64 fPositionRelative;
				if (iDiff == 0) {
					fPositionRelative = 0;
				}
				else {
					fPositionRelative = (double)mScrollPos.VisibleRect.iY / iDiff;
				}

				// Calculate the maximum height of the handle
				pControl = *it;
				SPos PosDownArrow;
				pControl->GetPos(PosDownArrow);
				tint iMaxHeight = PosDownArrow.iY - Pos.iY;

				// Calculate the relative height we occupy
				tfloat64 fHeightRelative = mScrollPos.VisibleRect.iCY / (double)mScrollPos.AreaSize.iCY;

				// Calculate the absolute size
				rctHandle = SRect(Pos, SSize(0, 0));
				rctHandle.iCX = mpBitmapSizes[BitmapCenterHandle].iCX;
				rctHandle.iCY = (int)((fHeightRelative * iMaxHeight) + 0.5);

				// Calculate the absolute position
				rctHandle.iY += (int)((fPositionRelative * (iMaxHeight - rctHandle.iCY)) + 0.5);

				if (rctHandle.iCY < mpBitmapSizes[BitmapLeftTopHandle].iCY + mpBitmapSizes[BitmapRightDownHandle].iCY) {
					rctHandle.iCY = mpBitmapSizes[BitmapLeftTopHandle].iCY + mpBitmapSizes[BitmapRightDownHandle].iCY;
				}
			}
			break;
	}

	return rctHandle;
}

void CScrollBar::OnDraw(const SRect &rUpdate)
{
	if (!IsVisible()) {
		return;
	}

	CPane::OnDraw(rUpdate);

	// Get the rect that the handle must occupy
	SRect rctHandle = GetHandleRect();
	switch(mType) {
		case TypeHorizontal:
			{
				// Draw left-most part
				SRect rctCur(rctHandle);
				rctCur.iCX = mpBitmapSizes[BitmapLeftTopHandle].iCX;
				rctCur.iCY = mpBitmapSizes[BitmapLeftTopHandle].iCY;
				DrawBitmap(mppBitmapRes[BitmapLeftTopHandle], rUpdate, rctCur);

				// Calculate rect for right-most part
				SRect rctRight(rctHandle);
				rctRight.iX = rctRight.iX + rctRight.iCX - mpBitmapSizes[BitmapRightDownHandle].iCX;
				rctRight.iCX = mpBitmapSizes[BitmapRightDownHandle].iCX;

				// Move position for center part
				rctCur.iX += rctCur.iCX;
				// Update with center size
				rctCur.iCX = mpBitmapSizes[BitmapCenterHandle].iCX;
				rctCur.iCY = mpBitmapSizes[BitmapCenterHandle].iCY;
				// Draw center part as many times as neccasary
				while (rctCur.iX < rctRight.iX) {
					DrawBitmap(mppBitmapRes[BitmapCenterHandle], rUpdate, rctCur);
					rctCur.iX += rctCur.iCX;
				}

				// Draw right-most part
				DrawBitmap(mppBitmapRes[BitmapRightDownHandle], rUpdate, rctRight);
			}
			break;
		case TypeVertical:
			{
				// Draw top-most part
				SRect rctCur(rctHandle);
				rctCur.iCX = mpBitmapSizes[BitmapLeftTopHandle].iCX;
				rctCur.iCY = mpBitmapSizes[BitmapLeftTopHandle].iCY;
				DrawBitmap(mppBitmapRes[BitmapLeftTopHandle], rUpdate, rctCur);

				// Calculate rect for bottom-most part
				SRect rctBottom(rctHandle);
				rctBottom.iY = rctBottom.iY + rctBottom.iCY - mpBitmapSizes[BitmapRightDownHandle].iCY;
				rctBottom.iCY = mpBitmapSizes[BitmapRightDownHandle].iCY;

				// Move position for center part
				rctCur.iY += rctCur.iCY;
				// Update with center size
				rctCur.iCX = mpBitmapSizes[BitmapCenterHandle].iCX;
				rctCur.iCY = mpBitmapSizes[BitmapCenterHandle].iCY;
				// Draw center part as many times as neccasary
				while (rctCur.iY < rctBottom.iY) {
					DrawBitmap(mppBitmapRes[BitmapCenterHandle], rUpdate, rctCur);
					rctCur.iY += rctCur.iCY;
				}

				// Draw right-most part
				DrawBitmap(mppBitmapRes[BitmapRightDownHandle], rUpdate, rctBottom);
			}
			break;
	}
}

tbool CScrollBar::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	if (!IsVisible()) {
		return false;
	}

	if (CPane::OnMouse(MouseMsg, Pos)) {
		return true;
	}

	if (mbScrolling) {
		// We're currently scrolling
		switch(MouseMsg) {
			case MouseMove:
				{
					switch(mType) {
						case TypeHorizontal:
							{
								// Calculate the mouse delta
								tint iMouseDelta = Pos.iX - mMousePosOrg.iX;

								// Calculate where we want the handle to be
								tint iHandlePosX = mScrollBarRectOrg.iX + iMouseDelta;

								// Calculate movememt from mouse delta
								std::list<IControl*>::iterator it = mControls.begin();
								if (mControls.size() > 2) {
									it++;
								}
								IControl* pControl = *it++;
								SRect RctArrowLeft;
								pControl->GetRect(RctArrowLeft);

								// Left-most possible handle position
								SPos PosLeftMost(RctArrowLeft.iX + RctArrowLeft.iCX, RctArrowLeft.iY);

								pControl = *it;
								SPos PosRightArrow;
								pControl->GetPos(PosRightArrow);
								tint iMaxWidth = PosRightArrow.iX - PosLeftMost.iX;

								// Calculate the relative width we occupy
								tfloat64 fWidthRelative = mScrollPos.VisibleRect.iCX / (double)mScrollPos.AreaSize.iCX;

								// Calculate the ralative position to be
								tfloat64 fPositionRelative = (iHandlePosX - PosLeftMost.iX) / (iMaxWidth - (fWidthRelative * iMaxWidth));

								// Update scrolling position
								mScrollPos = mScrollPosOrg;
								mScrollPos.VisibleRect.iX = (int)((fPositionRelative * (mScrollPos.AreaSize.iCX - mScrollPos.VisibleRect.iCX)) + 0.5f);

								// Limit scrolling position
								if (mScrollPos.VisibleRect.iX < 0) {
									mScrollPos.VisibleRect.iX = 0;
								}
								if (mScrollPos.VisibleRect.iX > mScrollPos.AreaSize.iCX - mScrollPos.VisibleRect.iCX) {
									mScrollPos.VisibleRect.iX = mScrollPos.AreaSize.iCX - mScrollPos.VisibleRect.iCX;
								}

								// Redraw and set scroll pos (which will cause scroll pane to redraw)
								CControl::Redraw();
								mpScrollPane->SetScrollPos(mScrollPos);

								SetValue(0);
							}
							break;
						case TypeVertical:
							{
								// Calculate the mouse delta
								tint iMouseDelta = Pos.iY - mMousePosOrg.iY;

								// Calculate where we want the handle to be
								tint iHandlePosY = mScrollBarRectOrg.iY + iMouseDelta;

								// Calculate movememt from mouse delta
								std::list<IControl*>::iterator it = mControls.begin();
								if (mControls.size() > 2) {
									it++;
								}
								IControl* pControl = *it++;
								SRect RctArrowTop;
								pControl->GetRect(RctArrowTop);

								// Top-most possible handle position
								SPos PosTopMost(RctArrowTop.iX, RctArrowTop.iY + RctArrowTop.iCY);

								pControl = *it;
								SPos PosBottomArrow;
								pControl->GetPos(PosBottomArrow);
								tint iMaxHeight = PosBottomArrow.iY - PosTopMost.iY;

								// Calculate the relative height we occupy
								tfloat64 fHeightRelative = mScrollPos.VisibleRect.iCY / (double)mScrollPos.AreaSize.iCY;

								// Calculate the ralative position to be
								tfloat64 fPositionRelative = (iHandlePosY - PosTopMost.iY) / (iMaxHeight - (fHeightRelative * iMaxHeight));

								// Update scrolling position
								mScrollPos = mScrollPosOrg;
								mScrollPos.VisibleRect.iY = (int)((fPositionRelative * (mScrollPos.AreaSize.iCY - mScrollPos.VisibleRect.iCY)) + 0.5f);

								// Limit scrolling position
								if (mScrollPos.VisibleRect.iY < 0) {
									mScrollPos.VisibleRect.iY = 0;
								}
								if (mScrollPos.VisibleRect.iY > mScrollPos.AreaSize.iCY - mScrollPos.VisibleRect.iCY) {
									mScrollPos.VisibleRect.iY = mScrollPos.AreaSize.iCY - mScrollPos.VisibleRect.iCY;
								}

								// Redraw and set scroll pos (which will cause scroll pane to redraw)
								CControl::Redraw();
								mpScrollPane->SetScrollPos(mScrollPos);

								SetValue(0);
							}
							break;
					}
				}
				break;
			case LeftButtonUp:
				{
					dynamic_cast<CWindow*>(GetParentWindow())->ReleaseMouseFocus();

					mbScrolling = false;

					return true;
				}
				break;
		}
	}

	SRect rctHandle = GetHandleRect();
	if (rctHandle.Inside(Pos)) {
		// We hit the handle with the mouse
		if (MouseMsg == LeftButtonDown) {
			mMousePosOrg = Pos;
			mScrollPosOrg = mScrollPos;
			mScrollBarRectOrg = rctHandle;

			dynamic_cast<CWindow*>(GetParentWindow())->GetMouseFocus(dynamic_cast<IControl*>(this));

			mbScrolling = true;

			return true;
		}
	}
	else {
		if (MouseMsg == LeftButtonDown) {
			SRect RctThis;
			GetRect(RctThis);
			if (RctThis.Inside(Pos)) {
				// We hit inside the control (but not the handle)
				switch(mType) {
					case TypeHorizontal:
						{
							// Calculate the mouse delta
							tint iMouseDelta;
							if (Pos.iX < rctHandle.iX) {
								iMouseDelta = -rctHandle.iCX;
							}
							else {
								iMouseDelta = rctHandle.iCX;
							}

							// Calculate where we want the handle to be
							tint iHandlePosX = rctHandle.iX + iMouseDelta;

							// Calculate movememt from mouse delta
							std::list<IControl*>::iterator it = mControls.begin();
							if (mControls.size() > 2) {
								it++;
							}
							IControl* pControl = *it++;
							SRect RctArrowLeft;
							pControl->GetRect(RctArrowLeft);

							// Left-most possible handle position
							SPos PosLeftMost(RctArrowLeft.iX + RctArrowLeft.iCX, RctArrowLeft.iY);

							pControl = *it;
							SPos PosRightArrow;
							pControl->GetPos(PosRightArrow);
							tint iMaxWidth = PosRightArrow.iX - PosLeftMost.iX;

							// Calculate the relative width we occupy
							tfloat64 fWidthRelative = mScrollPos.VisibleRect.iCX / (double)mScrollPos.AreaSize.iCX;

							// Calculate the ralative position to be
							tfloat64 fPositionRelative = (iHandlePosX - PosLeftMost.iX) / (iMaxWidth - (fWidthRelative * iMaxWidth));

							// Update scrolling position
							mScrollPos = mScrollPos;
							mScrollPos.VisibleRect.iX = (int)((fPositionRelative * (mScrollPos.AreaSize.iCX - mScrollPos.VisibleRect.iCX)) + 0.5f);

							// Limit scrolling position
							if (mScrollPos.VisibleRect.iX < 0) {
								mScrollPos.VisibleRect.iX = 0;
							}
							if (mScrollPos.VisibleRect.iX > mScrollPos.AreaSize.iCX - mScrollPos.VisibleRect.iCX) {
								mScrollPos.VisibleRect.iX = mScrollPos.AreaSize.iCX - mScrollPos.VisibleRect.iCX;
							}

							// Redraw and set scroll pos (which will cause scroll pane to redraw)
							CControl::Redraw();
							mpScrollPane->SetScrollPos(mScrollPos);
							
							// Lasse, added 2008-04-17 - bug-fix, didn't fire listeners' EventValueChange
							SetValue(0);
							// .. Lasse
						}
						break;

					case TypeVertical:
						{
							// Calculate the mouse delta
							tint iMouseDelta;
							if (Pos.iY < rctHandle.iY) {
								iMouseDelta = -rctHandle.iCY;
							}
							else {
								iMouseDelta = rctHandle.iCY;
							}

							// Calculate where we want the handle to be
							tint iHandlePosY = rctHandle.iY + iMouseDelta;

							// Calculate movememt from mouse delta
							std::list<IControl*>::iterator it = mControls.begin();
							if (mControls.size() > 2) {
								it++;
							}
							IControl* pControl = *it++;
							SRect RctArrowTop;
							pControl->GetRect(RctArrowTop);

							// Top-most possible handle position
							SPos PosTopMost(RctArrowTop.iX, RctArrowTop.iY + RctArrowTop.iCY);

							pControl = *it;
							SPos PosBottomArrow;
							pControl->GetPos(PosBottomArrow);
							tint iMaxHeight = PosBottomArrow.iY - PosTopMost.iY;

							// Calculate the relative height we occupy
							tfloat64 fHeightRelative = mScrollPos.VisibleRect.iCY / (double)mScrollPos.AreaSize.iCY;

							// Calculate the ralative position to be
							tfloat64 fPositionRelative = (iHandlePosY - PosTopMost.iY) / (iMaxHeight - (fHeightRelative * iMaxHeight));

							// Update scrolling position
							mScrollPos = mScrollPos;
							mScrollPos.VisibleRect.iY = (int)((fPositionRelative * (mScrollPos.AreaSize.iCY - mScrollPos.VisibleRect.iCY)) + 0.5f);

							// Limit scrolling position
							if (mScrollPos.VisibleRect.iY < 0) {
								mScrollPos.VisibleRect.iY = 0;
							}
							if (mScrollPos.VisibleRect.iY > mScrollPos.AreaSize.iCY - mScrollPos.VisibleRect.iCY) {
								mScrollPos.VisibleRect.iY = mScrollPos.AreaSize.iCY - mScrollPos.VisibleRect.iCY;
							}

							// Redraw and set scroll pos (which will cause scroll pane to redraw)
							CControl::Redraw();
							mpScrollPane->SetScrollPos(mScrollPos);
							
							// Lasse, added 2008-04-17 - bug-fix, didn't fire listeners' EventValueChange
							SetValue(0);
							// .. Lasse
						}
						break;
				}
			}
		}
	}

	return false;
}

tbool CScrollBar::OnKeyDown(EKey Key)
{
	if (!IsVisible()) {
		return false;
	}

	if (CPane::OnKeyDown(Key)) {
		return true;
	}

	return false;
}

void CScrollBar::EventValueChange(IControl *pSender, tint32 /*iValueNew*/)
{
	// Lasse, add 2008-05-09
	if ((mpScrollPane) && (pSender == mpScrollPane)) {
		SScrollPos sp;
		mpScrollPane->GetScrollPos(sp);
		if (mScrollPos != sp) {
			SRect rectTest1 = GetHandleRect();
			SetScrollPos(sp, true);
			//PositionControls();
			Redraw(GetRect());
			SRect rectTest2 = GetHandleRect();
			int iDummy = 0;
		}
		return;
	}
	// .. Lasse

	// Figure out which one of the controls it is
	std::list<IControl*>::iterator it = mControls.begin();
	if (mControls.size() > 2) {
		it++;
	}
	tint iDelta;
	if (pSender == *it) {
		// Up / left
		iDelta = -10;
	}
	else {
		// Down / right
		iDelta = 10;
	}

	SRect rctHandle = GetHandleRect();

	switch(mType) {
		case TypeHorizontal:
			{
				// Calculate where we want the handle to be
				tint iHandlePosX = rctHandle.iX + iDelta;

				// Calculate movememt from mouse delta
				std::list<IControl*>::iterator it = mControls.begin();
				if (mControls.size() > 2) {
					it++;
				}
				IControl* pControl = *it++;
				SRect RctArrowLeft;
				pControl->GetRect(RctArrowLeft);

				// Left-most possible handle position
				SPos PosLeftMost(RctArrowLeft.iX + RctArrowLeft.iCX, RctArrowLeft.iY);

				pControl = *it;
				SPos PosRightArrow;
				pControl->GetPos(PosRightArrow);
				tint iMaxWidth = PosRightArrow.iX - PosLeftMost.iX;

				// Calculate the relative width we occupy
				tfloat64 fWidthRelative = mScrollPos.VisibleRect.iCX / (double)mScrollPos.AreaSize.iCX;

				// Calculate the ralative position to be
				tfloat64 fPositionRelative = (iHandlePosX - PosLeftMost.iX) / (iMaxWidth - (fWidthRelative * iMaxWidth));

				// Update scrolling position
				mScrollPos.VisibleRect.iX = (int)((fPositionRelative * (mScrollPos.AreaSize.iCX - mScrollPos.VisibleRect.iCX)) + 0.5f);

				// Limit scrolling position
				if (mScrollPos.VisibleRect.iX < 0) {
					mScrollPos.VisibleRect.iX = 0;
				}
				if (mScrollPos.VisibleRect.iX > mScrollPos.AreaSize.iCX - mScrollPos.VisibleRect.iCX) {
					mScrollPos.VisibleRect.iX = mScrollPos.AreaSize.iCX - mScrollPos.VisibleRect.iCX;
				}

				// Redraw and set scroll pos (which will cause scroll pane to redraw)
				CControl::Redraw();
				mpScrollPane->SetScrollPos(mScrollPos);
			}
			break;
		case TypeVertical:
			{
				// Calculate where we want the handle to be
				tint iHandlePosY = rctHandle.iY + iDelta;

				// Calculate movememt from mouse delta
				std::list<IControl*>::iterator it = mControls.begin();
				if (mControls.size() > 2) {
					it++;
				}
				IControl* pControl = *it++;
				SRect RctArrowTop;
				pControl->GetRect(RctArrowTop);

				// Top-most possible handle position
				SPos PosTopMost(RctArrowTop.iX, RctArrowTop.iY + RctArrowTop.iCY);

				pControl = *it;
				SPos PosBottomArrow;
				pControl->GetPos(PosBottomArrow);
				tint iMaxHeight = PosBottomArrow.iY - PosTopMost.iY;

				// Calculate the relative height we occupy
				tfloat64 fHeightRelative = mScrollPos.VisibleRect.iCY / (double)mScrollPos.AreaSize.iCY;

				// Calculate the ralative position to be
				tfloat64 fPositionRelative = (iHandlePosY - PosTopMost.iY) / (iMaxHeight - (fHeightRelative * iMaxHeight));

				// Update scrolling position
				mScrollPos.VisibleRect.iY = (int)((fPositionRelative * (mScrollPos.AreaSize.iCY - mScrollPos.VisibleRect.iCY)) + 0.5f);

				// Limit scrolling position
				if (mScrollPos.VisibleRect.iY < 0) {
					mScrollPos.VisibleRect.iY = 0;
				}
				if (mScrollPos.VisibleRect.iY > mScrollPos.AreaSize.iCY - mScrollPos.VisibleRect.iCY) {
					mScrollPos.VisibleRect.iY = mScrollPos.AreaSize.iCY - mScrollPos.VisibleRect.iCY;
				}

				// Redraw and set scroll pos (which will cause scroll pane to redraw)
				CControl::Redraw();
				mpScrollPane->SetScrollPos(mScrollPos);
			}
			break;
	}
}


