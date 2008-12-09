#include "KSOS.h"

CTrack_File_Browser::CTrack_File_Browser(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

}

CTrack_File_Browser::~CTrack_File_Browser()
{
}

void CTrack_File_Browser::SetInfo()
{
	mpKSPlugIn = dynamic_cast<CKSPlugIn*>(GetPlugIn());
}

void CTrack_File_Browser::Init()
{

	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(gTrack_File_Browser.iCX, gTrack_File_Browser.iCY));
	mpPane->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteY);
	mpPane->StickTo(ge::IControl::PosTopRight, ge::SPos(0, gTrack_Top.iCY));

	mpPane->SetBackgroundColour(ge::SRGB(76,76,76));
	
	//-------------------------------
	// Import button
	CreateButton(giChC_Import_Audio, IDB_Button_File_Import, ge::SPos(4, 3), false);
	
	//
	
	// Track_File
	// This pane shows the audio files in the project
	mpTrack_Files = new CTrack_Files(this, GetGUI());
	mpTrack_Files->SetInfo();
	mpTrack_Files->Init();
	mpPane->AddControl(mpTrack_Files->GetPane(), ge::SPos(0, 24));
	
	Create_Files_ScrollBar();
	
}


void CTrack_File_Browser::ConnectControls()
{
}

void CTrack_File_Browser::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);

	switch(pControl->GetID()) {
		case giChC_Import_Audio:
			{
				/*
				if (mpKSPlugIn->GetProjDir().length() == 0) {
					mpGUI->GetWindow()->ShowMessageBox("You must create or load a project before importing audio", "Sorry");
				}
				else*/
				if (!mpKSPlugIn->OnAudioFileImport()) {
					std::string sMsg = "Unable to import audio";
					std::string sExt = mpKSPlugIn->GetExtendedError();
					if (sExt.length() > 0)
						sMsg += std::string("\n\n") + sExt;
					mpGUI->GetWindow()->ShowMessageBox(sMsg.c_str(), "Sorry: Import failed");
				}
			}
			break;
	}
}

void CTrack_File_Browser::EventGeneric(ge::IControl* pControl, void* pEventData)
{
	
}


void CTrack_File_Browser::HandleMsg(SMsg* pMsg)
{
/*
	switch(pMsg->iOpcode) 
	{
		case MsgResize: {
			ge::SSize sPaneSize;
			mpPane->GetSize(sPaneSize);
			mpLine->SetLinePos(ge::SPos(0, giTopBarSizeY-1),ge::SPos(sPaneSize.iCX, giTopBarSizeY-1));
				
			break;
		}
	}
*/
}


void CTrack_File_Browser::Create_Files_ScrollBar()
{

	//--------------------------------------------
	// Track_Player Scrollbar and backdrop for vertical scrolling
	//-------------------------------
	// Scrollbar possition
	// Position and Stick To Podition
	ge::SPos Pos(gTrack_File_Browser.iCX -giScrollBarSize, 38);
	ge::SPos StickToPos( -14,25 );
	// Scrollbar Backdrop left top
	ge::IBitmap* pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Top, Pos);
	// Scrollbar Backdrop Center part
	Pos.iY			+= 9;
	pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Vertical_Center, Pos);
	// Lasse, fix 2008-05-13
	//pBmp->SetSize(ge::SSize(giScrollBarSize, gTrack_File_Browser.iCY - 76  ));
	pBmp->SetSize(ge::SSize(giScrollBarSize, gTrack_File_Browser.iCY - 60  ));
	// .. Lasse
	pBmp->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteY);

	//-----
	// Scrollbar Backdrop Bottom part
	Pos.iY = gTrack_File_Browser.iCY - giScrollBarSize - 25;
	pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Bottom, Pos);
	// Lasse, fix 2008-05-13
	//pBmp->StickTo(ge::IControl::PosBottomRight, ge::SPos(0, -30));
	pBmp->StickTo(ge::IControl::PosBottomRight, ge::SPos(0, -14));
	// .. Lasse
	//-----
	// Scroll pos
	ge::SScrollPos ScrollPos;
	ge::SSize size(gTrack_Files.iCX, gTrack_Files.iCY);
	ScrollPos.AreaSize = ge::SSize(gTrack_Files.iCX, gTrack_Files.iCY*2);
	ScrollPos.VisibleRect = ge::SRect(ge::SPos(0, 0), size);
	// ScrollBar resources
	tint32 piIDs[ge::IScrollBar::BitmapCount];
	piIDs[ge::IScrollBar::BitmapLeftTop]			= IDB_Button_Scroll_Up;
	piIDs[ge::IScrollBar::BitmapRightDown]			= IDB_Button_Scroll_Down;
	piIDs[ge::IScrollBar::BitmapLeftTopHandle]		= IDB_Scroll_Bar_Handle_Vertical_Top;
	piIDs[ge::IScrollBar::BitmapRightDownHandle]	= IDB_Scroll_Bar_Handle_Vertical_Bottom;
	piIDs[ge::IScrollBar::BitmapCenterHandle]		= IDB_Scroll_Bar_Handle_Vertical_Center;

	//-----
	// Create scrollbar
	mpScrollBarVertical = ge::IScrollBar::Create();
	mpScrollBarVertical->Init(giChC_Track_Player_ScrollBar_Vertical,
		ge::IScrollBar::TypeVertical,
		GetWindow(),
		GetResMan(),
		piIDs,
		mpTrack_Files->GetScrollPane(),
		ScrollPos);
	//-----
	// Add ScrollBar
	//-----
	Pos.iX =	gTrack_Files.iCX;
	Pos.iY =	24;
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpScrollBarVertical),Pos);
	
	//-----
	// Glue scrollbar to Top Right
//	StickToPos.iX = - gTrack_File_Browser.iCX;
//	StickToPos.iY = gTrack_Top.iCY + gTrack_Time_Pos.iCY;
	//ge::SPos StickToPos(- gTrack_File_Browser.iCX, gTrack_Top.iCY + gTrack_Time_Pos.iCY );
//	mpScrollBarVertical->StickTo(ge::IControl::PosTopRight, StickToPos);
	// Make it resize with main pane
	mpScrollBarVertical->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteY);
	mpTrack_Files->GetScrollPane()->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteY);
	
	mpTrack_Files->GetScrollPane()->SetScrollPos(ScrollPos, false);
	


	
}



ge::IScrollPane* CTrack_File_Browser::GetScrollPane()
{
	return mpTrack_Files->GetScrollPane();

}