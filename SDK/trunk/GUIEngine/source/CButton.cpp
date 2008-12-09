/*!	\file CButton.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 14/Mar/2005
*/


#include "geInternalOS.h"


CButton::CButton()
	: mpText(NULL), mbEventMouseUp(false), mbEventMouseUpDown(false), mbButtonHit(false), mbLightAlways(false),
	mbShowHand(false), mbMouseStateFrames(false), mbMouseReleased(true)
{
	SetAutomaticResize(false);
}

CButton::~CButton()
{
	if (mpText) {
		mpText->Destroy();
	}
}

IButton* IButton::Create()
{
	return dynamic_cast<IButton*>(new CButton());
}

void CButton::Destroy()
{
	delete dynamic_cast<CButton*>(this);
}

void CButton::Init(tint32 iID, IBitmapResource* pBitmap)
{
	CControl::Init(iID, pBitmap);

	if (pBitmap) {
		pBitmap->GetRealSize(mSize);
	}
}

void CButton::SetEventMouseUp()
{
	mbEventMouseUp = true;
}

void CButton::SetEventMouseUpDown()
{
	mbEventMouseUpDown = true;
}

void CButton::EnableMouseStateFrames()
{
	mbMouseStateFrames = true;
}

void CButton::AddText(const tchar* pszText, IFontResource* pFont, const SSize& Size, const SPos* pPos, IText::EHorizontalAlignment HorzAlign)
{
	mpText = IText::Create();
	mpText->Init(IControl::giNoID, pFont);

	if (pPos) {
		mpText->SetPos(*pPos);
	}
	else {
		mpText->SetPos(SPos(0, 1));
	}
	mpText->SetSize(Size);

	mpText->SetParentPane(dynamic_cast<IPane*>(this));

	mpText->SetHorzAlignment(HorzAlign);
	mpText->SetVertAlignment(IText::VertAlignCenter);

	mpText->SetText(pszText);
}

void CButton::ChangeText(const tchar* pszText)
{
	if (mpText) {
		mpText->SetText(pszText);
	}
}

void CButton::SetLight(tbool bLight)
{
	mbLightAlways = bLight;

	CControl::Redraw();
}

void CButton::ActivateHandCursor()
{
	mbShowHand = true;
}

void CButton::OnDraw(const SRect &rUpdate)
{
	if( !IsVisible() )
		return;

	if (GetBitmapResource() == NULL) {
		// Button has no bitmap, i.e. is transparent
		return;
	}

	tbool bGhosted = false;
	if (mbCanGhost) {
		// Control can ghost...
		if (mbEnabled == false) {
			// ... and is currently disabled
			bGhosted = true;
		}
	}

	SPos MousePos;
	GetMousePos(MousePos);
	tint32 iMouseState = GetParentWindow()->GetMouseState();

	tint lFrame = 0;
	if (GetRect().Inside(MousePos)) {
		if (iMouseState & MouseStateLeftDown) {
			// Only do it if there's no mouse focus, or mouse focus is this control
			if (GetParentWindow()->HasMouseFocus(NULL) ||
				GetParentWindow()->HasMouseFocus(dynamic_cast<const IControl*>(this))) {
				lFrame = 1;
			}
		}
	}
	if (mbLightAlways) {
		lFrame = 1;
	}

	if (mbMouseStateFrames) {
		// Amount of frames is doubled
		lFrame *= 2;

		if (mbMouseReleased == false) {
			// Mouse is still down
			lFrame--;
			if (lFrame == -1) {
				lFrame = 3;
			}
		}

		DrawBitmapFrame(GetBitmapResource(), rUpdate, SRect(GetPos(), SSize(mSize.iCX, mSize.iCY)), lFrame, 4, bGhosted);
	}
	else {
		DrawBitmapFrame(GetBitmapResource(), rUpdate, SRect(GetPos(), SSize(mSize.iCX, mSize.iCY)), lFrame, 2, bGhosted);

		if (mpText) {
			mpText->OnDraw(rUpdate);
		}
	}
}

tbool CButton::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	if( !IsVisible() )
		return false;

	if (mbShowHand) {
		if (GetRect().Inside(Pos)) {
			GetParentWindow()->SetMouseCursor(IWindow::CursorHand);
		}
	}

	if (MouseMsg == LeftButtonDown || MouseMsg == LeftButtonDblClk) {
		if (GetRect().Inside(Pos)) {
			mbButtonHit = true;

			if (mbEventMouseUpDown) {
				SetValue(1);
			}
			else {
				if (mbEventMouseUp == false) {
					SetValue(0);
				}
			}

			CControl::Redraw();

			if (mbMouseStateFrames) {
				GetParentWindow()->GetMouseFocus(dynamic_cast<IControl*>(this));

				mbMouseReleased = false;
			}

			return true;
		}
		else {
			return false;
		}
	}

	if (MouseMsg == LeftButtonUp) {
		if (GetRect().Inside(Pos)) {
			if (mbButtonHit) {
				if (mbEventMouseUp) {
					SetValue(0);
				}
				if (mbEventMouseUpDown) {
					SetValue(0);
				}

				mbButtonHit = false;
			}
		}

		CControl::Redraw();

		if (mbMouseReleased == false) {
			ASSERT(mbMouseStateFrames);

			GetParentWindow()->ReleaseMouseFocus();

			mbMouseReleased = true;
		}
	}

	return false;
}

tbool CButton::OnKeyDown(EKey /*Key*/)
{
	return false;
}

void CButton::OnTimer()
{
	CControl::OnTimer();

	if( !IsVisible() )
		return;
}

