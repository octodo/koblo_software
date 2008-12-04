#include "KSOS.h"

CTrack_Side::CTrack_Side(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

}

CTrack_Side::~CTrack_Side()
{
}

void CTrack_Side::SetInfo()
{

}

void CTrack_Side::Init()
{

	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(gTrack_Side.iCX, gTrack_Side.iCY + giScrollBarSize));
	mpPane->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteY);
//	mpPane->SetBackgroundColour(ge::SRGB(255, 128, 0));//ge::SRGB(76,76,76));
	
	//--------------------------------------------
	// Time and Signatur Display background
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Track_Side, ge::SPos(0, 0));
	
//	// Tempo display
//	ge::IText* pText =  CreateDisplay( giChC_Tempo_Display, ge::SPos(88, 1),  ge::SSize(34, 14), CreateFont(Generic128, IDF_Fuxley_712, ge::SRGB(0, 0, 0)),true);
//	pText->SetClicksOutsideAsSuccess();
	
	//--------------------------------------------
	// Track_Setup
	// This pane scrols with the Track_Player pane
	mpTrack_Setup = new CTrack_Setup(this, GetGUI());
	mpTrack_Setup->SetInfo();
	mpTrack_Setup->Init();
	mpPane->AddControl(mpTrack_Setup->GetPane(), ge::SPos(0, gTrack_Time_Pos.iCY));
	
	
//	// Time signature selection popup
//	PreparePopups();
//	CreatePop(giControlID_Timesignatur_Popup, IDB_Invisible_Button_16_64, mpTime_Signature_Menu, ge::SPos(64, 32));
	
	//--------------------------------------------
	// Backdrop under file browser
	ge::IBitmap* pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Side_Buttom, ge::SPos(0, 0));
	pBmp->StickTo(ge::IControl::PosBottomLeft, ge::SPos(0, 0));
	
	
	// Zoom slider
	ge::IPanner* pPanner = CreatePanner(
		giChC_Zoom_Slider, ge::IControl::giNoID,
		NULL, IDB_Slider_Handle_Zoom,
		ge::SPos(0, 0), 
		ge::SPos(10, 0),
		ge::SSize(110, 14),
		NULL
	);

	pPanner->SetDirectMouseXlat(true);
//	pPanner->SetJumpToMouseClickPos(true);
	pPanner->StickTo(ge::IControl::PosBottomLeft, ge::SPos(0, 0));
	
	
}


void CTrack_Side::ConnectControls()
{
//	RegisterGlobalControlWithDisplayTranslater(giSectionGlobal, giChC_Tempo_Display, giParamID_KS_Tempo, 1, 5, 1);
//	RegisterGlobalControl(giSectionGlobal, giControlID_Timesignatur_Popup, giParamID_KS_Time_Signature);
	RegisterGlobalControlWithLinearTranslation(	giSectionGUI, giChC_Zoom_Slider,giParamID_Zoom);
	mpTrack_Setup->ConnectControls();
}

void CTrack_Side::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
	
}

void CTrack_Side::EventGeneric(ge::IControl* pControl, void* pEventData)
{
/*
	ge::SEventGeneric* pEvent = (ge::SEventGeneric*)pEventData;
	if (pEvent->eType = ge::giEventMouse) {
		//Handle_Mouse(pEvent);
		
		if (pEvent->iMsg == ge::MouseDrop) {

		tint32 iTest =0;
		iTest++;
		}
		if (pEvent->iMsg == ge::LeftButtonUp) {
		tint32 iTest =0;
		iTest++;
		}
		//else{
		//	Handle_Mouse(pEvent);
		
	}
*/
}


void CTrack_Side::HandleMsg(SMsg* pMsg)
{
	switch(pMsg->iOpcode) 
	{

		case Msg_Select_Track: {
		//	mpTrack_Setup->SelectTrack(iID);
			break;
		}
 
	}
}



void CTrack_Side::Set_Track_Info_Visible(tint32 i,tbool bTrackVisible)
{
	mpTrack_Setup->Set_Track_Info_Visible( i, bTrackVisible);

}


void CTrack_Side::Update_Tracks_Meters(void* mpvoid_sMeters_All)
{
	for (tint32 iTrack = 0; iTrack < giNumber_Of_Tracks; iTrack++) {
		mpTrack_Setup->UpdateTrackMeters_Maybe(iTrack, mpvoid_sMeters_All);
	}
}  // Update_Track_Meters


ge::IScrollPane* CTrack_Side::GetSetupScrollPane()
{
	return mpTrack_Setup->GetScrollPane();
}

/*
ge::IDropDownListBox* CTrack_Side::CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos)
{

	// Resources
	tint32 piIDs[ge::IPopupMenu::BitmapCount];
	piIDs[ge::IPopupMenu::BitmapFolderArrow]	= iBitmapID;
	piIDs[ge::IPopupMenu::BitmapTop]			= IDB_PopupBorder;
	piIDs[ge::IPopupMenu::BitmapCenter]			= IDB_POPUP_BODY;
	piIDs[ge::IPopupMenu::BitmapCenterInv]		= IDB_POPUP_BODY_INVERTED;
	piIDs[ge::IPopupMenu::BitmapBottom]			= IDB_PopupBorder;
	ge::IDropDownListBox* pListBox = CreateDropDownListBox( iControlID, 
															piIDs, 
															Generic128, 
															IDF_Fuxley_712, 
															iBitmapID, 
															List, Pos, 
															ge::SPos(0,0),
															ge::SSize(48,12));
//	pListBox->DontSetText();
	return pListBox;
}

void CTrack_Side::PreparePopups()
{
	
	// Out Popup
	mpTime_Signature_Menu.iItemCount = 6;										
	mpTime_Signature_Menu.pItems[0] = ge::IPopupMenu::SMenuItem((const tchar*)"2/4",	giSignature2_4,	NULL);
	mpTime_Signature_Menu.pItems[1] = ge::IPopupMenu::SMenuItem((const tchar*)"3/4",	giSignature3_4,	NULL);
	mpTime_Signature_Menu.pItems[2] = ge::IPopupMenu::SMenuItem((const tchar*)"4/4",	giSignature4_4,	NULL);
	mpTime_Signature_Menu.pItems[3] = ge::IPopupMenu::SMenuItem((const tchar*)"6/8",	giSignature6_8,	NULL);
	mpTime_Signature_Menu.pItems[4] = ge::IPopupMenu::SMenuItem((const tchar*)"9/8",	giSignature9_8,	NULL);
	mpTime_Signature_Menu.pItems[5] = ge::IPopupMenu::SMenuItem((const tchar*)"12/8",	giSignature12_8,NULL);

		
}
*/
void CTrack_Side::SelectTrack(tint32 iID)
{

//	mpTrack_Setup->SelectTrack(iID);

}

void CTrack_Side::Set_Track_Size(tint32 iTrack, tint32 iSize)
{

	mpTrack_Setup->Set_Track_Size( iTrack,  iSize);
}

void CTrack_Side::UpdateChannelName(tint32 iChannel, const std::string& sName)
{
	mpTrack_Setup->UpdateChannelName( iChannel, sName);
}