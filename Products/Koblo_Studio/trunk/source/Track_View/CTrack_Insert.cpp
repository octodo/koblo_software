#include "KSOS.h"

CTrack_Insert::CTrack_Insert(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

}

CTrack_Insert::~CTrack_Insert()
{
}

void CTrack_Insert::SetInfo()
{

}

void CTrack_Insert::Init()
{
	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(gTrack_File_Browser.iCX - giScrollBarSize, gTrack_File_Browser.iCY-16 - giScrollBarSize));
	mpPane->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteY );
	mpPane->SetBackgroundColour(ge::SRGB(200,200,200));
}


void CTrack_Insert::ConnectControls()
{
}

void CTrack_Insert::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
}

void CTrack_Insert::EventGeneric(ge::IControl* pControl, void* pEventData)
{
}


void CTrack_Insert::HandleMsg(SMsg* pMsg)
{

}