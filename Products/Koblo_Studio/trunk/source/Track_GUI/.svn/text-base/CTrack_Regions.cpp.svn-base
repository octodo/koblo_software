//#include "KS.h"

CTrack_Object::CTrack_Object(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

}

CTrack_Object::~CTrack_Object()
{
}

void CTrack_Object::SetInfo()
{

}

void CTrack_Object::Init()
{
	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(gTrack_File_Browser.iCX - giScrollBarSize, (gTrack_File_Browser.iCY/2)-32));
//	mpPane->SetAutomaticResize(true, ge::IControl::ResizeRelative);
	mpPane->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteY);
	mpPane->SetBackgroundColour(ge::SRGB(200,200,200));
}


void CTrack_Object::ConnectControls()
{
}

void CTrack_Object::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
}

void CTrack_Object::EventGeneric(ge::IControl* pControl, void* pEventData)
{
}


void CTrack_Object::HandleMsg(SMsg* pMsg)
{

}


