/*!	\file CDropDownListBox.cpp
	\author Michael Olsen
	\date 14/Mar/2005
	\date 14/Mar/2005
*/


#include "geInternalOS.h"


CDropDownListBox::CDropDownListBox()
	: mbPopUpActive(false), mbSetText(true)
{
	SetAutomaticResize(false);
}

CDropDownListBox::~CDropDownListBox()
{
}

IDropDownListBox* IDropDownListBox::Create()
{
	return dynamic_cast<IDropDownListBox*>(new CDropDownListBox());
}

void CDropDownListBox::Destroy()
{
	delete dynamic_cast<CDropDownListBox*>(this);
}

void CDropDownListBox::Init(tint32 iID)
{
	CControl::Init(iID);

	// Disable draw rect checking for this control
	CPane::DisableDrawingRectCheck();
}

void CDropDownListBox::DontSetText()
{
	mbSetText = false;
}

void CDropDownListBox::ControlUpdateAll()
{
	NotifyValueChange();
}

IPopupMenu* CDropDownListBox::GetPopup()
{
#ifdef _DEBUG
	if (mControls.size() < 2)
		return NULL;
#endif // #ifdef _DEBUG
	
	std::list<IControl*>::iterator it = mControls.begin();
	it++;
	return dynamic_cast<IPopupMenu*>(*it);
}


void CDropDownListBox::SetValue(tint32 iNew, tbool bNotify)
{
	if (mbSetText) {
		// Retrieve text
		tchar psz[64];
		std::list<IControl*>::iterator it = mControls.begin();
		it++;
		IPopupMenu* pPopup = dynamic_cast<IPopupMenu*>(*it);
		pPopup->GetText(iNew, (tchar*)psz);
		pPopup->SetValue(iNew, false);

		// Set text
		dynamic_cast<IButton*>((*(mControls.begin())))->ChangeText((tchar*)psz);
	}

	CPane::SetValue(iNew, bNotify);
}

void CDropDownListBox::AddControl(IControl* pToAdd, const SPos& Pos)
{
	SPos ActualPos;
	IButton* pButton = dynamic_cast<IButton*>(pToAdd);
	IPopupMenu* pPopup = dynamic_cast<IPopupMenu*>(pToAdd);
	if (pButton) {
		// This is the button
//		ActualPos = SPos(0, 0);
		ActualPos = Pos;
	}
	if (pPopup) {
		// This is the popup part
		SSize SizeButton;
		(*(mControls.begin()))->GetSize(SizeButton);
		ActualPos = SPos(0, SizeButton.iCY);

		pToAdd->SetVisible(false);

		// Lasse 2006-11-08: That's not good - should be rethought and rewritten
		// Listen for all mouse click message types outside
/*		pPopup->EnableOneTrapOutside(ge::LeftButtonDown);
		pPopup->EnableOneTrapOutside(ge::LeftButtonDblClk);
		pPopup->EnableOneTrapOutside(ge::RightButtonDown);
		pPopup->EnableOneTrapOutside(ge::RightButtonDblClk);
*/		// .. Lasse
	}

	// Call inherited
	CPane::AddControl(pToAdd, ActualPos);

	// Add ourselves as listener
	pToAdd->AddListener(dynamic_cast<IEventReceiver*>(this));

	// Lasse: Test, rewritten 2006-11-26 late in the evening - seems to work
	// Now *all* clicks outside will close the popup(s).
	/*
	if (pPopup) {
		// Calculate our size
		std::list<IControl*>::iterator it = mControls.begin();
		SSize Size;
		(*it)->GetSize(Size);
		it++;
		SSize Size2;
		(*it)->GetSize(Size2);
		Size.iCY += Size2.iCY;

		SetSize(Size);
	}
	*/
	if (pButton) {
		SSize size;
		pButton->GetSize(size);
		SetSize(size);
	}
	// .. Lasse
}

void CDropDownListBox::ShowPopUp()
{
	std::list<IControl*>::iterator it = mControls.begin();
	it++;
	IPopupMenu* pPopup = dynamic_cast<IPopupMenu*>(*it);

	ge::IControl* pButton = *(mControls.begin());
	ge::SPos posBestAbsolute = pPopup->GetBestAbsPos(pButton);
	ge::SPos posBestRelative = posBestAbsolute - this->GetAbsPos();
	pPopup->SetPos(posBestRelative);

	mbPopUpActive = true;
	pPopup->SetVisible(true);

	pPopup->SetValue(miValue, false);

	CWindow* pWindow = dynamic_cast<CWindow*>(GetParentWindow());
	pWindow->GetMouseFocus(dynamic_cast<IControl*>(this));
	pWindow->ControlAlwaysOnTop(dynamic_cast<IControl*>(this));
}


void CDropDownListBox::EventValueChange(IControl *pSender, tint32 iValueNew)
{
	// Figure out whether it was the button or the popup by trying to cast the sender
	IButton* pButton = dynamic_cast<IButton*>(pSender);
	IPopupMenu* pPopup = dynamic_cast<IPopupMenu*>(pSender);

	if (pButton) {
		if (mbPopUpActive == false) {
			ShowPopUp();
		}
		else {
			// Popup is active, close it
			std::list<IControl*>::iterator it = mControls.begin();
			it++;
			IControl* pPopup = *it;

			pPopup->SetVisible(false);
			dynamic_cast<CWindow*>(GetParentWindow())->ReleaseMouseFocus();
			dynamic_cast<CWindow*>(GetParentWindow())->ControlAlwaysOnTopDone();
			mbPopUpActive = false;

			// Redraw whole window
			SSize SizeWindow;
			GetParentWindow()->GetSize(SizeWindow);
			SRect rctRedraw(SPos(0, 0), SizeWindow);
			GetParentWindow()->Redraw(rctRedraw);
		}
	}

	if (pPopup) {
		if (mbPopUpActive == true) {
			if (iValueNew != -1) {
				pPopup->SetVisible(false);

				EditBegin();
				SetValue(iValueNew, true);
				EditEnd();

				dynamic_cast<CWindow*>(GetParentWindow())->ReleaseMouseFocus();
				dynamic_cast<CWindow*>(GetParentWindow())->ControlAlwaysOnTopDone();

				mbPopUpActive = false;

				// Redraw whole window
				SSize SizeWindow;
				GetParentWindow()->GetSize(SizeWindow);
				SRect rctRedraw(SPos(0, 0), SizeWindow);
				GetParentWindow()->Redraw(rctRedraw);
			}
		}
	}
} // CDropDownListBox::EventValueChange


void CDropDownListBox::EventGeneric(IControl* pSender, void* pEventData)
{
	IPopupMenu* pPopup = dynamic_cast<IPopupMenu*>(pSender);
	if (pPopup) {
		SEventGeneric* pEvent = (ge::SEventGeneric*)pEventData;
		if (
			// Was mouse clicked outside pop-up?
			((pEvent->eType==ge::giEventMouse)&&(!pEvent->bInsideControlRect))
			// .. or ..
			||
			// .. was Escape Key caught by pop-up?
			((pEvent->eType==ge::giEventKeyboard)&&(pEvent->iMsg == ge::KeyEscape))
		) {
			pPopup->SetVisible(false);
			dynamic_cast<CWindow*>(GetParentWindow())->ReleaseMouseFocus();
			dynamic_cast<CWindow*>(GetParentWindow())->ControlAlwaysOnTopDone();
			mbPopUpActive = false;
		}

	}
} // CDropDownListBox::EventGeneric


tbool CDropDownListBox::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	if (!IsVisible()) {
		return false;
	}

	tbool bRet = CPane::OnMouse(MouseMsg, Pos);

	// Lasse - the previous approach doesn't work correctly if the popup(s) are placed at 
	// "unexpected" positions - i.e. somewhere outside the CDropDownListBox's implicit IPane
	// All is disabled as of 2006-12-01
//	if (mbPopUpActive) {
//		if (MouseMsg == LeftButtonUp) {
//			if (GetRect().Inside(Pos) == false) {
//				// Left button was released ourside our control, but we're still active... That can only mean the mouse was clicked outside the popup, and we need to close it
//				std::list<IControl*>::iterator it = mControls.begin();
//				it++;
//				IControl* pPopup = *it;
//
//				pPopup->SetVisible(false);
//				dynamic_cast<CWindow*>(GetParentWindow())->ReleaseMouseFocus();
//				dynamic_cast<CWindow*>(GetParentWindow())->ControlAlwaysOnTopDone();
//				mbPopUpActive = false;
//
//				// Redraw whole window
//				SSize SizeWindow;
//				GetParentWindow()->GetSize(SizeWindow);
//				SRect rctRedraw(SPos(0, 0), SizeWindow);
//				GetParentWindow()->Redraw(rctRedraw);
//			}
//		}
//
//		return true;
//	}
	// .. Lasse

	return bRet;
}

