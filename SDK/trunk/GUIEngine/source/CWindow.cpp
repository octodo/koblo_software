/*!	\file CWindow.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/


#include "geInternalOS.h"

CWindow::CWindow()
	: mpPane(NULL), mMousePosCur(0, 0), miMouseStateCur(0), mbResizePaneToFit(false), mpControlOnTop(NULL),
	mMouseCursorCur(CursorNormal),
	mpToolTipText(NULL)
{
	mpInvalidater = IInvalidater::Create();
}

CWindow::~CWindow()
{
	mpInvalidater->Destroy();

	if (GetPane()) {
		GetPane()->Destroy();
		mpPane = NULL;
	}
}

void CWindow::SetPane(IPane* pPane, tbool bResizeToFit)
{
	mpPane = pPane;

	if (GetPane()) {
		GetPane()->SetParentWindow(dynamic_cast<IWindow*>(this));
	}

#ifdef _Mac
	// (mo) There's a weird bug in XCode, which stops us from dynamic casting it, even though it should work. Luckily a C-style cast seems to work.
	CPane* pCPane = (CPane*)(void*)mpPane;
#else	// _Mac
	CPane* pCPane = dynamic_cast<CPane*>(mpPane);
#endif	// _Mac
	pCPane->AddControlsToWindow(dynamic_cast<CWindow*>(this));

	mbResizePaneToFit = bResizeToFit;

	if (mbResizePaneToFit) {
		SSize Size;
		GetSize(Size);

		mpPane->SetSize(Size);
	}
}

IPane* CWindow::GetPane()
{
	return mpPane;
}

const IPane* CWindow::GetPane() const
{
	return mpPane;
}

void CWindow::GetMousePos(SPos& rPos) const
{
	rPos = mMousePosCur;
}

tint32 CWindow::GetMouseState() const
{
	return miMouseStateCur;
}

void CWindow::Redraw(const SRect& rct)
{
	mpInvalidater->InvalidateRect(rct);
}

void CWindow::MouseDown(const SPos& /*Pos*/)
{
	// Empty default function, since this is only called on OSX
}

void CWindow::KeyDown(tint32 /*iKey*/)
{
	// Empty default function, since this is only called on OSX
}

void CWindow::Paint()
{
	// Empty default function, since this is only called on OSX
}

void CWindow::RelayMouse(EMouseMsg Msg, const SPos& Pos)
{
	OnMouse(Msg, Pos);
}

void CWindow::GetInvalidatedRect(tint32& riX, tint32& riY, tint32& riCX, tint32& riCY)
{
	riX = riY = riCX = riCY = 0;

	if (mpInvalidater->IsRectInvalidated()) {
		SRect Rect;
		mpInvalidater->GetInvalidatedRect(Rect);

		riX = Rect.iX;
		riY = Rect.iY;
		riCX = Rect.iCX;
		riCY = Rect.iCY;
	}
}

tbool CWindow::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	mbResendMouseEvent = false;

	bool bRet = false;

	tbool bDone = false;
	while (bDone == false) {
		mMouseCursorNext = CursorNormal;

		mMousePosCur = Pos;

		tbool bRet2;
		if (mMouseFocus.size() != 0) {
			IControl* pControl = *(mMouseFocus.begin());
			bRet2 = pControl->OnMouse(MouseMsg, mMousePosCur);
		}
		else {
			bRet2 = GetPane()->OnMouse(MouseMsg, mMousePosCur);
		}
		if (bRet == false) {
			bRet = bRet2;
		}

		if (mMouseCursorCur != mMouseCursorNext) {
			mMouseCursorCur = mMouseCursorNext;

			DoSetMouseCursor(mMouseCursorNext);
		}

		bDone = mbResendMouseEvent ? false : true;
		mbResendMouseEvent = false;
	}

	return bRet;
}

void CWindow::OnMouseDrag(const SPos& Pos)
{
	mbResendMouseEvent = false;

	bool bRet = false;

	tbool bDone = false;
	while (bDone == false) {
		mMousePosCur = Pos;

		bRet = GetPane()->OnMouse(MouseDrag, mMousePosCur);

		bDone = mbResendMouseEvent ? false : true;
		mbResendMouseEvent = false;
	}

//	return bRet;
}

void CWindow::OnMouseDrop(const SPos& Pos)
{
	mbResendMouseEvent = false;

	bool bRet = false;

	tbool bDone = false;
	while (bDone == false) {
		mMousePosCur = Pos;

		bRet = GetPane()->OnMouse(MouseDrop, mMousePosCur);

		bDone = mbResendMouseEvent ? false : true;
		mbResendMouseEvent = false;
	}

//	return bRet;
}

void CWindow::SetDragAndDropInfo(const SDragAndDropInfo& Info)
{
	DragAndDropInfo = Info;
}

void CWindow::GetDragAndDropInfo(SDragAndDropInfo& Info)
{
	Info = DragAndDropInfo;
}

void CWindow::ResendMouseEvent()
{
	mbResendMouseEvent = true;
}

tbool CWindow::OnKeyDown(EKey Key)
{
	tbool bRet = GetPane()->OnKeyDown(Key);

	if (bRet) {
		return true;
	}
	else {
		return dynamic_cast<CContext*>(GetContext())->GetCallback()->OnKeyDown(Key);
	}
}

void CWindow::OnTimer()
{
	if (GetPane()) {
		GetPane()->OnTimer();
	}
}

void CWindow::SetControlValue(tint32 iControlID, tint32 iValueNew, tbool bNotify)
{
	GetPane()->SetControlValue(iControlID, iValueNew, bNotify);
}

void CWindow::ControlUpdateAll()
{
	dynamic_cast<CPane*>(GetPane())->ControlUpdateAll();
}

IControl* CWindow::GetControl(tint32 iControlID)
{
//	return GetPane()->GetControl(iControlID);

	std::map<tint32, IControl*>::const_iterator it = mControlMap.find(iControlID);

	if (it != mControlMap.end()) {
		// It's there, return it
		IControl* pControl = it->second;
		return pControl;
	}

	// Control wasn't in map. We're requesting a control which doesn't exist in this window
	return NULL;
}

void CWindow::GetMouseFocus(IControl* pControl, tbool /*mbCaptured*/)
{
	mMouseFocus.push_front(pControl);
}

void CWindow::ReleaseMouseFocus()
{
	//ASSERT(mMouseFocus.size() != 0);
	if (mMouseFocus.size() == 0) return;

	mMouseFocus.pop_front();
}

tbool CWindow::HasMouseFocus(const IControl* pControl)
{
	if (pControl == NULL) {
		return (mMouseFocus.size() == 0);
	}
	else {
		if (mMouseFocus.size() == 0) {
			return false;
		}
		else {
//			return (*(mMouseFocus.begin()) == pControl);
			IControl* pControlFocus = *(mMouseFocus.begin());
			IControl* pControlToCheck = (IControl*)pControl;
			IPane* pPaneTop = pControlToCheck->GetTopPane();
			while (1) {
				if (pControlFocus == pControlToCheck) {
					return true;
				}

				if (dynamic_cast<IPane*>(pControlToCheck) == pPaneTop) {
					return false;
				}

				pControlToCheck = pControlToCheck->GetParentPane();
			}
		}
	}
}

void CWindow::ControlAlwaysOnTop(IControl* pControl)
{
	mpControlOnTop = pControl;
}

void CWindow::ControlAlwaysOnTopDone()
{
	mpControlOnTop = NULL;
}

void CWindow::SetMouseCursor(EMouseCursor Cursor)
{
	mMouseCursorNext = Cursor;
}

void CWindow::InitToopTips(IFontResource* pFont)
{
	// Make sure we're not called twice
	ASSERT(mpToolTipText == NULL);

	mpToolTipRect = IRect::Create();
	mpToolTipRect->Init(-1, SRect(0, 0, 0, 0), SRGB(255, 255, 225), dynamic_cast<IWindow*>(this));
	GetPane()->AddControl(dynamic_cast<IControl*>(mpToolTipRect), SPos(0, 0));
	mpToolTipRect->SetVisible(false);

	tint iLine;
	for (iLine = 0; iLine < 4; iLine++) {
		mpToolTipLines[iLine] = ILine::Create();
		mpToolTipLines[iLine]->Init(-1, SPos(0, 0), SPos(0, 0), SRGB(12, 10, 10), dynamic_cast<IWindow*>(this));
		GetPane()->AddControl(dynamic_cast<IControl*>(mpToolTipLines[iLine]), SPos(0, 0));
		mpToolTipLines[iLine]->SetVisible(false);
	}

	mpToolTipText = IText::Create();
	mpToolTipText->Init(-1, pFont);
	mpToolTipText->SetHorzAlignment(IText::HorzAlignCenter);
	mpToolTipText->SetVertAlignment(IText::VertAlignCenter);
	GetPane()->AddControl(dynamic_cast<IControl*>(mpToolTipText), SPos(0, 0));
	mpToolTipText->SetVisible(false);
}

void CWindow::ShowToolTip(const SPos& Pos, const std::string& sToolTipText)
{
	SPos PosModified(Pos);
	PosModified.iY -= 16;

	SSize SizeText;
	SizeText.iCX = dynamic_cast<IFontResource*>(mpToolTipText->GetBitmapResource())->GetTextWidth((tchar*)(sToolTipText.c_str()));
	SizeText.iCY = dynamic_cast<IFontResource*>(mpToolTipText->GetBitmapResource())->GetTextHeight();

	// This is complete rect
	SRect rct(PosModified - SPos(3, 3), SizeText + SSize(6, 6));

	// Reposition if neccasary
	SSize SizeWindow;
	GetSize(SizeWindow);
	if (rct.iX < 0) {
		PosModified.iX -= rct.iX;
	}
	if (rct.iY < 0) {
		PosModified.iY -= rct.iY;
	}
	// Recalculate
	rct = SRect(PosModified - SPos(3, 3), SizeText + SSize(6, 6));
	if (rct.iX + rct.iCX + 1 >= SizeWindow.iCX) {
		PosModified.iX -= rct.iX + rct.iCX + 1 - SizeWindow.iCX;
	}
	if (rct.iY + rct.iCY + 1 >= SizeWindow.iCY) {
		PosModified.iY -= rct.iY + rct.iCY + 1 - SizeWindow.iCY;
	}

	mpToolTipText->SetPos(PosModified);
	mpToolTipText->SetSize(SizeText + SSize(0, 2));
	mpToolTipText->SetText((tchar*)(sToolTipText.c_str()));
	mpToolTipText->SetVisible(true);

	mpToolTipRect->SetPos(PosModified - SPos(2, 2));
	mpToolTipRect->SetSize(SizeText + SSize(5, 5));
	mpToolTipRect->SetRect(SRect(SPos(0, 0), SizeText + SSize(5, 5)));
	mpToolTipRect->SetVisible(true);

	tint iLine;
	for (iLine = 0; iLine < 4; iLine++) {
		mpToolTipLines[iLine]->SetPos(PosModified - SPos(3, 3));
		mpToolTipLines[iLine]->SetSize(SizeText + SSize(6, 6));
	}

	mpToolTipLines[0]->SetLinePos(SPos(0, 0), SPos(SizeText.iCX + 6, 0));
	mpToolTipLines[1]->SetLinePos(SPos(0, 0), SPos(0, SizeText.iCY + 6));
	mpToolTipLines[2]->SetLinePos(SPos(SizeText.iCX + 6, 0), SPos(SizeText.iCX + 6, SizeText.iCY + 6));
	mpToolTipLines[3]->SetLinePos(SPos(0, SizeText.iCY + 6), SPos(SizeText.iCX + 6, SizeText.iCY + 6));

	for (iLine = 0; iLine < 4; iLine++) {
		mpToolTipLines[iLine]->SetVisible(true);
	}

	// To avoid redraw problems, we better redraw the whole window
	SRect Rect(SPos(0, 0), SizeWindow);
	Redraw(Rect);
}

void CWindow::HideToolTip()
{
	mpToolTipText->SetVisible(false);

	mpToolTipRect->SetVisible(false);

	tint iLine;
	for (iLine = 0; iLine < 4; iLine++) {
		mpToolTipLines[iLine]->SetVisible(false);
	}

	// To avoid redraw problems, we better redraw the whole window
	SSize SizeWindow;
	GetSize(SizeWindow);
	SRect Rect(SPos(0, 0), SizeWindow);
	Redraw(Rect);
}

void CWindow::SetFineAdjustModifier(EModifier Modifier)
{
	mModifierFineAdjust = Modifier;
}

void CWindow::SetResetModifier(EModifier Modifier)
{
	mModifierReset = Modifier;
}

tbool CWindow::ModifierPressed(EModifier Modifier)
{
	switch (Modifier) {
		case ModifierNone:
			return false;

		case ModifierControl:
			return ControlPressed();

		case ModifierShift:
			return ShiftPressed();

		case ModifierAlt:
			return AltPressed();

		case ModifierOS:
			return OSPressed();
	}

	// Keep compiler happy
	return false;
}

tbool CWindow::FineAdjustPressed()
{
	return ModifierPressed(mModifierFineAdjust);
}

tbool CWindow::ResetPressed()
{
	return ModifierPressed(mModifierReset);
}

void CWindow::ControlAdded(tint32 iControlID, IControl* pControl)
{
	ASSERT(pControl->GetID() == iControlID);

	// Ignore controls with no id
	if (iControlID == IControl::giNoID) {
		return;
	}

	mControlMap.insert(std::pair<tint32, IControl*>(iControlID, pControl));
}



