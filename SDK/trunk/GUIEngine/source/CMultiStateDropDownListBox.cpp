/*!	\file CMultiStateDropDownListBox.cpp
	\author Lasse Steen Bohnstedt
	\date 28/Nov/2006
*/


#include "geInternalOS.h"


CMultiStateDropDownListBox::CMultiStateDropDownListBox()
{
	SetAutomaticResize(false);

	mbPopUpActive = false;
	meMouseMsgTrigger = RightButtonUp;
	mbPopupAtClickPos = true;
} // constructor


CMultiStateDropDownListBox::~CMultiStateDropDownListBox()
{
} // destructor


IMultiStateDropDownListBox* IMultiStateDropDownListBox::Create()
{
	return dynamic_cast<IMultiStateDropDownListBox*>(new CMultiStateDropDownListBox());
} // Create

void CMultiStateDropDownListBox::Destroy()
{
	delete dynamic_cast<CMultiStateDropDownListBox*>(this);
} // Destroy


void CMultiStateDropDownListBox::Init(tint32 iID, EMouseMsg msgTrigger /*= RightButtonUp*/, tbool bPopupAtClickPos /*= true*/)
{
	CControl::Init(iID);

	meMouseMsgTrigger = msgTrigger;
	mbPopupAtClickPos = bPopupAtClickPos;
} // Init


void CMultiStateDropDownListBox::SetParentPane(IPane* pPane)
{
	CPane::SetParentPane(pPane);
/*
	// Lasse: Inherited from CRadioButton.cpp
	// (mo) Hack. To automatically set the radio button size we set it to the same size as it's parent pane
	SSize Size;
	GetParentPane()->GetSize(Size);
	SetSize(Size);
	*/
}

void CMultiStateDropDownListBox::ControlUpdateAll()
{
	NotifyValueChange();
}

void CMultiStateDropDownListBox::AddControl(IControl* pToAdd, const SPos& Pos)
{
	// Call inherited
	CPane::AddControl(pToAdd, Pos);

	// Add ourselves as listener
	pToAdd->AddListener(dynamic_cast<IEventReceiver*>(this));

	// Maybe resize to fit button
	IMultiStateButton* pBtn = dynamic_cast<IMultiStateButton*>(pToAdd);
	if (pBtn) {
		SSize sizeBtn;
		pBtn->GetSize(sizeBtn);
		tint32 iCX = Pos.iX + sizeBtn.iCX;
		tint32 iCY = Pos.iY + sizeBtn.iCY;

		if (iCX>mSize.iCX) {
			mSize.iCX = iCX;
		}
		if (iCY>mSize.iCY) {
			mSize.iCY = iCY;
		}
	}
}


void CMultiStateDropDownListBox::EventGeneric(IControl *pSender, void *pEventData)
{
	// Should be pop-up or trap
	IPopupMenu* pPopup = dynamic_cast<IPopupMenu*>(pSender);
	if (pPopup) {
		// Pop-up sent a message - maybe hide pop-up
		SEventGeneric* pEvent = (SEventGeneric*)pEventData;
		if (pEvent->eType==giEventKeyboard) {
			if (pEvent->iMsg == KeyEscape) {
				HidePopup();
			}
		}
		else if (pEvent->eType==giEventMouse) {
			// Note: Right now (2006-11-28) no mouse messages are sent from popup. May change though.
			if (!pEvent->bInsideControlRect) {
				HidePopup();
			}
		}
	}
	else {
		IMouseTrap* pTrap = dynamic_cast<IMouseTrap*>(pSender);
		if (pTrap) {
			// Mouse trap sent a message - maybe show popup
			SEventGeneric* pEvent = (SEventGeneric*)pEventData;
			if (pEvent->eType==giEventMouse) {
				if ((pEvent->iMsg==meMouseMsgTrigger)&&(pEvent->bInsideControlRect)) {
					// We received the trigger - now show pop-up
					ShowPopup(pEvent->pos);
				}
				else {
					// This is not meant for us - relay on to our listeners
					NotifyGeneric(pEventData);
				}
			}
		}
	}
} // CMultiStateDropDownListBox::EventGeneric


void CMultiStateDropDownListBox::EventValueChange(IControl *pSender, tint32 iValueNew)
{
	IPopupMenu* pPopup = dynamic_cast<IPopupMenu*>(pSender);
	if (pPopup) {
		// Popup was clicked (SetValue called) - now hide it
		HidePopup();
		// Set value of multi-state button
		std::list<IControl*>::iterator it = mControls.begin();
		for (; it != mControls.end(); it++) {
			IControl* pControl = *it;
			IMultiStateButton* pBtn = dynamic_cast<IMultiStateButton*>(pControl);
			if (pBtn) {
				pBtn->SetValue(iValueNew);
			}
		}
	}
	else {
		IMultiStateButton* pBtn = dynamic_cast<IMultiStateButton*>(pSender);
		if (pBtn) {
			// Multi-state button was clicked (SetValue called) - now hide the pop-up
			HidePopup();
		}
	}
} // CMultiStateDropDownListBox::EventValueChange


void CMultiStateDropDownListBox::HidePopup()
{
	if (mbPopUpActive) {
		IWindow* pWindow = GetParentWindow();

		// Hide pop-up - and save absolute position of all controls
		std::list<IControl*>::iterator it = mControls.begin();
		for (; it != mControls.end(); it++) {
			IControl* pControl = *it;

			IPopupMenu* pPopup = dynamic_cast<IPopupMenu*>(pControl);
			if ((pPopup)&&(mbPopUpActive)) {
				// Hide pop-up
				if (pPopup->IsVisible()) {
					// Hide popup
					if (true) { //(pWindow->IsKeyboardAcquired()) {
						pWindow->ReleaseKeyboardInput();
					}
					if (true) { //pWindow->IsControlAlwaysOnTop(this)) {
						pWindow->ControlAlwaysOnTopDone();
					}
					if (pWindow->HasMouseFocus(this)) {
						pWindow->ReleaseMouseFocus();
					}

					pPopup->SetVisible(false);
					mbPopUpActive = false;

					// Send a message to owner (GUI or whatever) that we've closed
					SEventGeneric event;
					event.eType = giEventCustom;
					event.bInsideControlRect = true;	// Irrelevant
					event.iMsg = 0;						// 0 = we've closed
					event.pos = SPos(0,0);				// Irrelevant
					NotifyGeneric(&event);
				}
			}
		}
	}
} // CMultiStateDropDownListBox::HidePopup


void CMultiStateDropDownListBox::ShowPopup(SPos& posAbsClicked)
{
	IWindow* pWindow = GetParentWindow();

	// Send a message to owner (GUI or whatever) to make it close all open pop-ups (including this - we need to reposition it)
	SEventGeneric event;
	event.eType = giEventCustom;
	event.bInsideControlRect = true;	// Irrelevant
	event.iMsg = 1;						// 1 = we're about to open
	event.pos = posAbsClicked;
	NotifyGeneric(&event);


	// Find out where cirka to place the popup
	SPos posAbsPopup = posAbsClicked;
	SSize sizeObstructiveObject = SSize(1, 0);
	if (!mbPopupAtClickPos) {
		std::list<IControl*>::iterator it = mControls.begin();
		for (; it != mControls.end(); it++) {
			IControl* pControl = dynamic_cast<CControl*>(*it);
			IMultiStateButton* pBtn= dynamic_cast<IMultiStateButton*>(pControl);
			if (pBtn) {
				SPos posAbs;
				pControl->GetAbsPos(posAbs);
				SSize size;
				pControl->GetSize(size);
				SRect rect = SRect(posAbs, size);
				if (rect.Inside(posAbsClicked)) {
					// This is the control that was clicked
					posAbsPopup = posAbs;
					sizeObstructiveObject = size;
				}
			}
		}
	}

	// Show pop-up
	{
		std::list<IControl*>::iterator it = mControls.begin();
		for (; it != mControls.end(); it++) {
			IControl* pControl = *it;
			IPopupMenu* pPopup = dynamic_cast<IPopupMenu*>(pControl);
			if (pPopup) {
				// This is a pop-up. Calculate where excactly to put it

				// Show pop-up in correct position
				{
					// Hide the pop-up (if for some reason it is still visible)
					if (pPopup->IsVisible()) {
						pPopup->SetVisible(false);
					}
					SPos posAbsPossible = pPopup->GetBestAbsPos(posAbsPopup, sizeObstructiveObject);
					pPopup->SetAbsPos(posAbsPossible);
					pPopup->SetVisible(true);
				}
				
				// Get full mouse attention
				if (!mbPopUpActive) {
					pWindow->GetMouseFocus(this);
					pWindow->ControlAlwaysOnTop(this);
					pWindow->AcquireKeyboardInput();
					mbPopUpActive = true;
				}
			}
		}
	}
} // CMultiStateDropDownListBox::ShowPopup


