/*!	\file CRadioButtons.cpp
	\author Michael Olsen
	\date 21/Jan/2005
	\date 21/Jan/2005
*/


#include "geInternalOS.h"


CRadioButtons::CRadioButtons()
{
	SetAutomaticResize(false);
}

CRadioButtons::~CRadioButtons()
{
}

IRadioButtons* IRadioButtons::Create()
{
	return dynamic_cast<IRadioButtons*>(new CRadioButtons());
}

void CRadioButtons::Destroy()
{
	delete dynamic_cast<CRadioButtons*>(this);
}

void CRadioButtons::Init(tint32 iID)
{
	CControl::Init(iID);
}

void CRadioButtons::SetParentPane(IPane* pPane)
{
	CPane::SetParentPane(pPane);

	// (mo) Hack. To automatically set the radio button size we set it to the same size as it's parent pane
	SSize Size;
	GetParentPane()->GetSize(Size);
	SetSize(Size);
}

void CRadioButtons::ControlUpdateAll()
{
	NotifyValueChange();
}

void CRadioButtons::SetValue(tint32 iNew, tbool bNotify)
{
	CPane::SetValue(iNew, bNotify);

	std::list<IControl*>::iterator it = mControls.begin();
	tuint uiIndex;
	for (uiIndex = 0; uiIndex < mControls.size(); uiIndex++, it++) {
		if (uiIndex == GetValue()) {
			// This is the control corresponding to the value, i.e. set it
			(*it)->SetValue(1, false);
		}
		else {
			// This is not the control corresponding to the value, i.e. clear it
			(*it)->SetValue(0, false);
		}
	}
}

void CRadioButtons::AddControl(IControl* pToAdd, const SPos& Pos)
{
	// Call inherited
	CPane::AddControl(pToAdd, Pos);

	// Add ourselves as listener
	pToAdd->AddListener(dynamic_cast<IEventReceiver*>(this));
}

void CRadioButtons::EventValueChange(IControl *pSender, tint32 iValueNew)
{
	// If the value is not 1 it means the button was not turned on, so we need to turn it back on
	if (iValueNew != 1) {
		pSender->SetValue(1);
	}
	else {
		// The value was 1, so the button was turned on.
		// Find the index, and set our value. This will turn off the other buttons
		tint iIndex = 0;
		std::list<IControl*>::iterator it = mControls.begin();
		while ((*it) != pSender) {
			iIndex++;
			it++;
		}

		SetValue(iIndex, true);
	}
}

