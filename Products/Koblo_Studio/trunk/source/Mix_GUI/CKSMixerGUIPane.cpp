
#include "KSOS.h"


CKSMixerGUIPane::CKSMixerGUIPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	CBaseDSPEngine* pD = GetPlugIn()->GetDSPEngine();
	mpDSP = dynamic_cast<CDSP*>(pD);

	
	mbFirstMeterSet = true;
	mbIsVisible = false;
}

CKSMixerGUIPane::~CKSMixerGUIPane()
{
	
}

void CKSMixerGUIPane::Init()
{

	
	mpPane = ge::IPane::Create();
	mpPane->SetBackgroundColour(ge::SRGB(0, 0, 0));
	mpPane->SetSize(gMix);
	

	
	//--------------------------------------------
	// Scroll pane holds the channels
	mpMix_Scroll = new CMix_Scroll(this, GetGUI());
	mpMix_Scroll->SetInfo();
	mpMix_Scroll->Init();
	mpPane->AddControl(mpMix_Scroll->GetPane(), ge::SPos(0, 0));
	
	Create_Mix_ScrollBar_Horizontal();
	
	
	tint32 iPosX = gMix.iCX -64;
	
	//--------------------------------------------
	// Master
	mpMix_Master = new CMix_Master(this, GetGUI());
	mpMix_Master_Out_Menu.iItemCount = 3;										
	mpMix_Master_Out_Menu.pItems[0] = ge::IPopupMenu::SMenuItem((const tchar*)"12",	0,	NULL);
	mpMix_Master_Out_Menu.pItems[1] = ge::IPopupMenu::SMenuItem((const tchar*)"34",	1,	NULL);
	mpMix_Master_Out_Menu.pItems[2] = ge::IPopupMenu::SMenuItem((const tchar*)"56",	2,	NULL);
	mpMix_Master->SetInfo(mpMix_Master_Out_Menu);
	mpMix_Master->Init();
	mpPane->AddControl(mpMix_Master->GetPane(), ge::SPos(iPosX, 0));
	mpMix_Master->GetPane()->StickTo(ge::IControl::PosTopRight, ge::SPos(0, 0));
	
	iPosX -= 6;
	//--------------------------------------------
	// Busses
	tint32 iBusStickToPos = -70;
	for(tint32 i=0; i<giNumber_Of_Busses; i++) {
		iPosX -= 64;
		mpMix_Busses[i] = new CMix_Buss(this, GetGUI());
		tint32 iTimerID = i + giTimerID_MixFirstBus_ScrollPane;
		mpMix_Busses[i]->SetInfo(giNumber_Of_Busses - i  - 1, iTimerID);
		mpMix_Busses[i]->Init();

		mpPane->AddControl(mpMix_Busses[i]->GetPane(), ge::SPos(iPosX, 0));
		mpMix_Busses[i]->GetPane()->StickTo(ge::IControl::PosTopRight, ge::SPos(iBusStickToPos, 0));
		iBusStickToPos -= 64;
	}
	
	//--------------------------------------------
	// Backdrop under file browser
	ge::IBitmap* pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Mix_Bottom, ge::SPos(gMix.iCX - 332, gMix.iCY - 14));
	pBmp->StickTo(ge::IControl::PosBottomRight, ge::SPos(0, 0));
	pBmp->SetSize(ge::SSize( 332 , 14  ));
	
	
	
}

void CKSMixerGUIPane::ConnectControls()
{
	mpMix_Scroll->ConnectControls();


	// Busses
	for(tint32 i=0; i<giNumber_Of_Busses; i++) {
		mpMix_Busses[i]->ConnectControls();
	}
	mpMix_Master->ConnectControls();

}

void CKSMixerGUIPane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{

/*	switch(pControl->GetID()) {
		case giMixScrollBarID:
			SMsg Msg(MsgScrollMix);
			SendMsg(&Msg);
			break;
			
	}
*/
	
}

void CKSMixerGUIPane::UpdateEngineData( tint32 iType,tint32  iExtra, tint32  iID, tint32 iValue)
{
	switch(iType){
		case giSectionGlobal:
			break;
		default: break;
	}

}

void CKSMixerGUIPane::UpdateTrackData(tint32 iID, tint32 iValue, tint32 iChannel)
{
	
	switch(iID) {
		default: break;
		
		case giParam_ChSolo:
		case giParam_ChMute:
		{ 
			break;
		}
	
		case giParam_Track_Info_Show_Inserts:{
			STrackData sData;
			sData.iParam_ID		=	giParam_Track_Info_Show_Inserts;
			sData.iVal			=	iValue;
			sData.iTrack		=	iChannel;
			SMsg Msg			=	Msg_Track_Data;
			Msg.pDataIn			=	(void*)&sData;
			SendMsg(&Msg);
			break;
		}

		case giParam_Ch_Insert1GUIOpen:
		case giParam_Ch_Insert2GUIOpen:
		case giParam_Ch_Insert3GUIOpen:
		case giParam_Ch_Insert4GUIOpen:
		case giParam_Ch_Insert5GUIOpen:
		case giParam_Ch_Insert6GUIOpen:
		case giParam_Ch_Insert7GUIOpen:
		case giParam_Ch_Insert8GUIOpen:
			{
				tint iInsert = iID - giParam_Ch_Insert1GUIOpen;
			}
			break;
	}
}

void CKSMixerGUIPane::UpdateGUIData(tint32 iID, tint32 iValue)
{
	if (iID == giParamID_Show_Mix_Window) {
		if (iValue == 0) {
			mbIsVisible = false;
		}
		else {
			mbFirstMeterSet = true;
			mbIsVisible = true;
		}
		return;
	}
	
	

	SGUIData sData;
	sData.iID	= iID;
	sData.iVal	= iValue;
	SMsg Msg;
	
	Msg = MsgGUIData;
	Msg.pDataIn	= (void*)&sData;
	SendMsg(&Msg);
	
}

void CKSMixerGUIPane::UpdateCommandData(tint32 iID, tint32 iValue)
{

/*	SGUIData sData;
	sData.iID	= iID;
	sData.iVal	= iValue;
	SMsg Msg;
	
	Msg = MsgGUIData;
	Msg.pDataIn	= (void*)&sData;
	SendMsg(&Msg);
*/
	
}

void CKSMixerGUIPane::UpdateBussData(tint32 iID, tint32 iValue, tint32 iBuss)
{
		switch(iID) {
		default: break;
		
		case giParam_ChSolo:
		case giParam_ChMute:
		{ 
			break;
		}
	
		case giParam_Buss_Info_Show_Inserts:{
			STrackData sData;
			sData.iParam_ID		=	giParam_Buss_Info_Show_Inserts;
			sData.iVal			=	iValue;
			sData.iTrack		=	iBuss;
			SMsg Msg			=	Msg_Buss_Data;
			Msg.pDataIn			=	(void*)&sData;
			SendMsg(&Msg);
			break;
		}
	}

}

void CKSMixerGUIPane::UpdateMasterData(tint32 iID, tint32 iValue)
{
}
/*
void CKSMixerGUIPane::UpdateAuxMonitorData(tint32 iID, tint32 iValue)
{
//	mpAuxMonitor_GUI->UpdateData( iID,  iValue);
}

*/
void CKSMixerGUIPane::OnTimer(tint32 iTimerID)
{
	// Lasse, out-commented 2008-06-25 - no longer running on this timer
	/*
	// Lasse, added 2008-06-17:
	switch (iTimerID) {
		case giTimerID_Meters_Mixer:
		{
			if (!mbIsVisible) {
				// Just remove values
				dynamic_cast<CKSPlugIn*>(mpPlugIn)->ClearAllMeters();
			}
			else {
				// Calc elapsed time since previous meter set
				tuint32 uiTimeMS_Now = ITime::GetTimeMS();
				tuint32 uiTimeMS_Elapsed = uiTimeMS_Now - miuTimeMS_Prev;
				miuTimeMS_Prev = uiTimeMS_Now;
				void* pMeters = NULL;
				if (mbFirstMeterSet) {
					// Set all meters to 1.0, to make them fall-off
					mbFirstMeterSet = false;
				}
				else {
					// Get meter values and decay factor
					dynamic_cast<CKSPlugIn*>(mpPlugIn)->GetAllMeters_MaybeClear(mpvoid_sMeters_All, uiTimeMS_Elapsed, true);
					pMeters = mpvoid_sMeters_All;
				}
				// Send pointer to struct with all meter values to all tracks and let them decide if they need it
				mpMix_Scroll->Update_Tracks_Meters(pMeters);
				for (tint32 iBus = 0; iBus < giNumber_Of_Busses; iBus++) {
					mpMix_Busses[iBus]->UpdateMeters(pMeters);
				}
				mpMix_Master->UpdateMeters(pMeters);
			}
		}
	}
	// .. Lasse
	*/
	// .. Lasse
}

void CKSMixerGUIPane::Resize(ge::SSize SizeNew)
{
	// Lasse, test
	SMsg Msg(Msg_Resize);
	//SendMsg(&Msg);
	mpMix_Scroll->HandleMsg(&Msg);

	ge::SScrollPos scrollposMix;
	mpMix_Scroll->GetScrollPane()->GetScrollPos(scrollposMix);
	ge::SScrollPos scrollposBar;
	mpScrollBarHorizontal->GetScrollPos(scrollposBar);
	scrollposBar.VisibleRect.iCX = scrollposMix.VisibleRect.iCX;
	scrollposBar.VisibleRect.iX = scrollposMix.VisibleRect.iX;
	scrollposBar.AreaSize.iCX = scrollposMix.AreaSize.iCX;
	mpScrollBarHorizontal->SetScrollPos(scrollposBar);

	//SendMsg(&Msg);
	CBasePane::Resize(SizeNew);


}

void CKSMixerGUIPane::SendMsgFromTop(SMsg* pMsg)
{
	switch(pMsg->iOpcode) 
	{
		case Msg_Track_Data:
	//	case Msg_Add_Track:
	//	case Msg_Delete_Track:
		case Msg_Select_Track:
		case Msg_Stack_Tracks: 
		{
			
			SendMsg(pMsg);
			break;
		}

		case Msg_Set_All_Meters:
			Update_All_Meters(pMsg->pDataIn);
			break;
	}
	
}

void CKSMixerGUIPane::Create_Mix_ScrollBar_Horizontal()
{

	//--------------------------------------------
	// Track_Player Scrollbar and backdrop for horizontal scrolling
	//-------------------------------
	
	// Scrollbar Backdrop Center part
	ge::IBitmap*  pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Horizontal_Center,  ge::SPos(0,gMix.iCY -giScrollBarSize));
	pBmp->SetSize(ge::SSize( gMix_Scroll_Default.iCX  , giScrollBarSize  ));
	pBmp->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteX);
	pBmp->StickTo(ge::IControl::PosBottomLeft, ge::SPos(23,0));
	//-----
	// Scrollbar Backdrop Left part
	pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Left, ge::SPos(14,gMix.iCY -giScrollBarSize));
	pBmp->StickTo(ge::IControl::PosBottomLeft, ge::SPos(14,0));

	
	// Scrollbar Backdrop Right part
	pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Right, ge::SPos(gMix_Scroll_Default.iCX - 37, gMix.iCY - giScrollBarSize));
	pBmp->StickTo(ge::IControl::PosBottomRight, ge::SPos(-(5*64)-6-18,0));
	//-----
	// Scroll pos
	ge::SScrollPos ScrollPos;
	ScrollPos.AreaSize = ge::SSize(gMix_Scroll_Default.iCX, gMix.iCY -giScrollBarSize);
	ScrollPos.VisibleRect = ge::SRect(ge::SPos(gMix_Scroll_Default.iCX + 14, 0), ge::SSize(gMix_Scroll_Default.iCX + giScrollBarSize , gMix.iCY-giScrollBarSize));
	
	// ScrollBar resources
	tint32 piIDs[ge::IScrollBar::BitmapCount];
	piIDs[ge::IScrollBar::BitmapLeftTop]			= IDB_Button_Scroll_Left;
	piIDs[ge::IScrollBar::BitmapRightDown]			= IDB_Button_Scroll_Right;
	piIDs[ge::IScrollBar::BitmapLeftTopHandle]		= IDB_Scroll_Bar_Handle_Horizontal_Left;
	piIDs[ge::IScrollBar::BitmapRightDownHandle]	= IDB_Scroll_Bar_Handle_Horizontal_Right;
	piIDs[ge::IScrollBar::BitmapCenterHandle]		= IDB_Scroll_Bar_Handle_Horizontal_Center;

	//-----
	// Create scrollbar
	mpScrollBarHorizontal = ge::IScrollBar::Create();
	mpScrollBarHorizontal->Init(giChC_Mix_ScrollBar_Horizontal,
		ge::IScrollBar::TypeHorizontal,
		GetWindow(),
		GetResMan(),
		piIDs,
		mpMix_Scroll->GetScrollPane(),
		ScrollPos);
	mpScrollBarHorizontal->ExpandAreaSize();
	//-----
	// Add ScrollBar
	//-----
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpScrollBarHorizontal),ge::SPos(0, gMix.iCY - giScrollBarSize));

	//-----
	// Glue scrollbar to Bottom Left.iCX;
	mpScrollBarHorizontal->StickTo(ge::IControl::PosBottomLeft, ge::SPos(0,0));
	// Make it resize with main pane
	mpScrollBarHorizontal->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteX);
	mpMix_Scroll->GetScrollPane()->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteX);
	mpMix_Scroll->GetScrollPane()->SetScrollPos(ScrollPos, false);


	
}


void CKSMixerGUIPane::UpdateChannelName(tint32 iTrack, const std::string& sName)
{
	mpMix_Scroll->UpdateChannelName( iTrack, sName);
}


void CKSMixerGUIPane::Update_All_Meters(void* pvoid_sMeters_All)
{
	if (mbIsVisible) {
		if (mbFirstMeterSet) {
			// Set all meters to 1.0, to make them fall-off
			pvoid_sMeters_All = NULL;
			mbFirstMeterSet = false;
		}
		// Send pointer to struct with all meter values to all tracks and let them decide if they need it
		mpMix_Scroll->Update_Tracks_Meters(pvoid_sMeters_All);
		for (tint32 iBus = 0; iBus < giNumber_Of_Busses; iBus++) {
			mpMix_Busses[iBus]->UpdateMeters(pvoid_sMeters_All);
		}
		mpMix_Master->UpdateMeters(pvoid_sMeters_All);
	}
}