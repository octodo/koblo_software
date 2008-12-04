#include "KSOS.h"

CTrack_Edit::CTrack_Edit(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

}

CTrack_Edit::~CTrack_Edit()
{
}

void CTrack_Edit::SetInfo()
{

}

void CTrack_Edit::Init()
{
	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(gTrack_Edit);
	mpPane->SetAutomaticResize(true, ge::IControl::ResizeAbsolute);
	mpPane->SetBackgroundColour(ge::SRGB(240,240,240));
	
	
	
	
}


void CTrack_Edit::ConnectControls()
{
}

void CTrack_Edit::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
}

void CTrack_Edit::EventGeneric(ge::IControl* pControl, void* pEventData)
{
}


void CTrack_Edit::HandleMsg(SMsg* pMsg)
{

}


