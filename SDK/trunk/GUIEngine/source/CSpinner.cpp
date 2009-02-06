/*!	\file CSpinner.cpp
	\author Michael Olsen (+Lasse Steen Bohnstedt)
	\date 13/Sep/2006
*/


#include "geInternalOS.h"


CSpinner::CSpinner()
{
	SetAutomaticResize(false);
}

CSpinner::~CSpinner()
{
}

ISpinner* ISpinner::Create()
{
	return dynamic_cast<ISpinner*>(new CSpinner());
}

void CSpinner::Destroy()
{
	delete dynamic_cast<CSpinner*>(this);
}

void CSpinner::Init(tint32 iID)
{
	CControl::Init(iID);
}

void CSpinner::SetParentPane(IPane* pPane)
{
	CPane::SetParentPane(pPane);

	// Lasse: Inherited from CRadioButton.cpp
	// (mo) Hack. To automatically set the radio button size we set it to the same size as it's parent pane
	SSize Size;
	GetParentPane()->GetSize(Size);
	SetSize(Size);
}

void CSpinner::ControlUpdateAll()
{
	NotifyValueChange();
}

void CSpinner::AddControl(IControl* pToAdd, const SPos& Pos)
{
	ASSERT(mControls.size()<2);

	// Call inherited
	CPane::AddControl(pToAdd, Pos);

	// Add ourselves as listener
	pToAdd->AddListener(dynamic_cast<IEventReceiver*>(this));
}

void CSpinner::EventValueChange(IControl *pSender, tint32 iValueNew)
{
	// If the value is not 1 it means the button was not turned on, so we need to turn it back on
	if (iValueNew != 1) {
		EditBegin();
		pSender->SetValue(1);
		EditEnd();
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

		tint32 iValue = GetValue();
		if (iIndex==0)
			iValue++;
		else
			iValue--;
		EditBegin();
		SetValue(iValue, true);
		EditEnd();
	}
}

