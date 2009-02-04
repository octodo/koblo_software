#include "KSOS.h"

CTrack_Files::CTrack_Files(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

}

CTrack_Files::~CTrack_Files()
{
}

void CTrack_Files::SetInfo()
{

}

void CTrack_Files::Init()
{
/*	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(gTrack_File_Browser.iCX - giScrollBarSize, gTrack_File_Browser.iCY-16 - giScrollBarSize));
	mpPane->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteY );
	mpPane->SetBackgroundColour(ge::SRGB(200,200,200));
*/	
	// New
	// Lasse, size may be wrong! fix later
	ge::SSize SizeThis(gTrack_File_Browser.iCX - giScrollBarSize, gTrack_File_Browser.iCY - 16 - giScrollBarSize);
	mpScrollPane = ge::IScrollPane::Create();
	mpScrollPane->Init(ge::IControl::giNoID , giTimerID_TrackFiles_ScrollPane);
	mpPane = dynamic_cast<ge::IPane*>(mpScrollPane);
	mpPane->SetSize(SizeThis);
	mpPane->GetSize(SizeThis);

	// Lasse, that's probably not what we want? Fix later
	mpPane->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteY );
	
	mpPane->SetBackgroundColour(ge::SRGB(239,239,239));

	CreateListBox(giCtrl_File_List,
											ge::SPos(4, 0), ge::SSize(SizeThis.iCX-4, SizeThis.iCY),
											CreateFont(Generic128, 
											IDB_Font_Minix_Tight, 
											ge::SRGB(10, 10, 10)));
}


void CTrack_Files::ConnectControls()
{
}

void CTrack_Files::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
}

void CTrack_Files::EventGeneric(ge::IControl* pControl, void* pEventData)
{
}


void CTrack_Files::HandleMsg(SMsg* pMsg)
{
	if (pMsg->iOpcode == Msg_Update_File_List) {
		std::list<CSample_Data*>* pList = (std::list<CSample_Data*>*)(pMsg->pDataIn);

		std::string sText;
		std::list<CSample_Data*>::const_iterator it = pList->begin();
		for (; it != pList->end(); it++) {
			sText += (*it)->Name();
			sText += '@';
		}
		sText = sText.substr(0, sText.size() - 1);

		ge::IListBox* pListBox = dynamic_cast<ge::IListBox*>(GetControl(giCtrl_File_List));
		pListBox->SetText(sText.c_str());
	}
	/*
	ge::SPos Pos;
	GetPane()->GetPos(Pos);
	ge::SSize Size;
	GetPane()->GetSize(Size);
	ge::SRect Rct(Pos, Size);
	mpPane->Redraw(Rct);
	*/
	Redraw_Pane_Rect();
}


