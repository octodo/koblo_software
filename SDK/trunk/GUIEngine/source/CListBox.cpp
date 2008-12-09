/*!	\file CListBox.cpp
	\author Michael Olsen
	\date 24/Apr/2008
*/

#include "geInternalOS.h"


CListBox::CListBox()
	: mbDragging(false)
{
	// Disable all extra mouse events by default
	memset(&mabExtraMouseEvents, false, sizeof(mabExtraMouseEvents));
} // constructor


CListBox::~CListBox()
{
	mpDrawPrimitives->Destroy();
} // destructor


IListBox* IListBox::Create()
{
	return dynamic_cast<IListBox*>(new CListBox());
} // Create


void CListBox::Destroy()
{
	delete dynamic_cast<CListBox*>(this);
} // Destroy


void CListBox::Init(tint32 iID, IFontResource* pFont)
{
	CControl::Init(iID, pFont);

	miDragIndex = -1;
	miValue = -1;

	mpDrawPrimitives = NULL;
} // Init


void CListBox::SetText(const tchar* pszEnumList, tchar cEnumDelimiter)
{
	mTexts.clear();
	miDragIndex = -1;
	miValue = -1;

	if (pszEnumList) {
		tchar* pcStart = (tchar*)pszEnumList;
		while (*pcStart) {
			tchar* pcEnd = pcStart;
			while ((*pcEnd) && (*pcEnd != cEnumDelimiter)) {
				pcEnd++;
			}

			// Insert into list
			{
				tint32 iLen = pcEnd - pcStart;
				tchar* pszTemp = new tchar[iLen + 1];
				memcpy(pszTemp, pcStart, iLen);
				pszTemp[iLen] = '\0';
				mTexts.insert(mTexts.end(), std::string((char*)pszTemp));
				delete[] pszTemp;
				pszTemp = NULL;
			}

			// Move to next
			if (*pcEnd == cEnumDelimiter) {
				// Point after delimiter char
				pcStart = pcEnd + 1;
			}
			else {
				// Last - point to ending '\0' char
				pcStart = pcEnd;
			}
		}
	}
} // SetText


void CListBox::EnableExtraMouseEvent(EMouseMsg eMsg /*= LeftButtonDblClk*/, tint32 iValueToSet /*= -1*/)
{
	if ((eMsg >= 0) && (eMsg < NrOfMouseMessages)) {
		mabExtraMouseEvents[eMsg] = true;
		maiExtraReturnValues[eMsg] = iValueToSet;
	}
} // EnableExtraMouseEvent


void CListBox::OnDraw(const SRect &rUpdate)
{
	if (mpDrawPrimitives == NULL) {
		mpDrawPrimitives = IDrawPrimitives::Create();
		mpDrawPrimitives->Init(GetParentWindow());
	}

	SRect rctDraw(GetPos(), SSize(GetSize().iCX, 10));

	tint32 iIndex = 0;
	std::list<std::string>::const_iterator it = mTexts.begin();
	for (; it != mTexts.end(); it++, iIndex++) {
		const std::string& sText = *it;

		DrawFont(dynamic_cast<IFontResource*>(GetBitmapResource()), rUpdate, rctDraw, (const tchar*)(sText.c_str()));

		if (iIndex == miDragIndex) {
			SRect rctInvert(rctDraw);
			rctInvert.iY -= 2;
			rctInvert.iCY += 4;
			mpDrawPrimitives->InvertRect(rUpdate, rctInvert);
		}

		rctDraw.iY += 14;
	}
} // OnDraw


tbool CListBox::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	if (IsVisible() == false) {
		return false;
	}

	if (mbDragging) {
		GetParentWindow()->SetMouseCursor(IWindow::CursorHand);
	}

	if (MouseMsg == LeftButtonDown) {
		if (GetRect().Inside(Pos)) {
			Redraw();

			tint32 iFiles = mTexts.size();

			tint32 iIndex = (Pos.iY - GetPos().iY) / 14;

			if (iIndex < iFiles) {
				mbDragging = true;
				miDragIndex = iIndex;

//				GetParentWindow()->SetMouseCursor(IWindow::CursorHand);

				dynamic_cast<CWindow*>(GetParentWindow())->GetMouseFocus(this);

				IWindow::SDragAndDropInfo Info(iIndex);
				Info.pOrigin = dynamic_cast<ge::IControl*>(this);
				std::list<std::string>::const_iterator it = mTexts.begin();
				tint32 iIndexCur = 0;
				for (; iIndexCur < iIndex; iIndexCur++, it++) {
				}
				strcpy(Info.psz, it->c_str());
				GetParentWindow()->SetDragAndDropInfo(Info);

				SetValue(iIndex);

				return true;
			}
			else {
				SetValue(-1);
			}
		}
	}

	if (MouseMsg == MouseMove) {
		if (mbDragging) {
			GetParentWindow()->OnMouseDrag(Pos);

			GetParentWindow()->SetMouseCursor(IWindow::CursorHand);

			return true;
		}
	}

	if (MouseMsg == LeftButtonUp) {
		if (mbDragging) {
			GetParentWindow()->OnMouseDrop(Pos);

			dynamic_cast<CWindow*>(GetParentWindow())->ReleaseMouseFocus();

			mbDragging = false;

			return true;
		}
	}

	if ((MouseMsg >= 0) && (MouseMsg < NrOfMouseMessages)) {
		if (mabExtraMouseEvents[MouseMsg]) {
			tint32 iValue = maiExtraReturnValues[MouseMsg];
			if (iValue == -1) {
				// Use same value as ordinary operation
				iValue = GetValue();
			}
			SetValue(iValue);
		}
	}

	return false;
} // OnMouse


tbool CListBox::OnKeyDown(EKey Key)
{
	if (IsVisible() == false) {
		return false;
	}

	return false;
} // OnKeyDown


void CListBox::OnTimer()
{
} // OnTimer

