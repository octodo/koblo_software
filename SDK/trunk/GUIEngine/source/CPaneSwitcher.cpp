/*!	\file CPaneSwitcher.cpp
	\author Michael Olsen
	\date 06/Sep/2007
	\date 06/Sep/2007
*/


#include "geInternalOS.h"


CPaneSwitcher::CPaneSwitcher()
{
}

CPaneSwitcher::~CPaneSwitcher()
{
}

IPaneSwitcher* IPaneSwitcher::Create()
{
	return dynamic_cast<IPaneSwitcher*>(new CPaneSwitcher());
}

void CPaneSwitcher::Destroy()
{
	delete dynamic_cast<CPaneSwitcher*>(this);
}

void CPaneSwitcher::Init(tint32 iID)
{
//	CPane::Init(iID);
	// Apparently there's a bug in xcode(?), because this line seems to be neccasary, instead of the above
	dynamic_cast<CPane*>(this)->Init(iID);

	// Disable draw rect checking for this control
	CPane::DisableDrawingRectCheck();
}

void CPaneSwitcher::SetParentPane(IPane* pPane)
{
	CPane::SetParentPane(pPane);

	SSize Size;
	GetParentPane()->GetSize(Size);
	SetSize(Size);
}

void CPaneSwitcher::ControlUpdateAll()
{
	NotifyValueChange();
}

void CPaneSwitcher::SetValue(tint32 iNew, tbool bNotify)
{
	CPane::SetValue(iNew, bNotify);

	std::list<IControl*>::iterator it = mControls.begin();
	tuint uiIndex;
	for (uiIndex = 0; uiIndex < mControls.size(); uiIndex++, it++) {
		if (uiIndex == GetValue()) {
			// This is the control corresponding to the value, i.e. set it
			(*it)->SetVisible(true);
		}
		else {
			// This is not the control corresponding to the value, i.e. clear it
			(*it)->SetVisible(false);
		}
	}
}

void CPaneSwitcher::AddControl(IControl* pToAdd, const SPos& Pos)
{
	// Call inherited
	CPane::AddControl(pToAdd, Pos);
}

