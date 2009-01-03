
#include "KSOS.h"

CKS_TrackEditorPane::CKS_TrackEditorPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI), mbInited(false)
{
	CBaseDSPEngine* pD = GetPlugIn()->GetDSPEngine();
	mpDSP = dynamic_cast<CKSDSP*>(pD);
	mpKSPlugIn = dynamic_cast<CKSPlugIn*>(GetPlugIn());
	miMouseOverTrack = -1;
	miSelectedTrack = -1;
	mpMouseTrap = NULL;
//	mbPlaying	=	false;

	// Init meters arrays
	mpvoid_sMeters_All = (void*)new CKSDSP::SMeters_All();
	mbFirstMeterSet = true;
	mbIsVisible = false;
	mfGoToPos = 0.0;
	
	mbZoomFlipFlop = false;

	mbDisplayStutterWarning_Armed = true;
}

CKS_TrackEditorPane::~CKS_TrackEditorPane()
{
	if (mpMouseTrap) {
		delete mpMouseTrap;
		mpMouseTrap = NULL;
	}

	// Delete meters arrays
	delete ((CKSDSP::SMeters_All*)mpvoid_sMeters_All);
}

void CKS_TrackEditorPane::Init()
{
	//!!!Max added some git test code here
	
	
	mpScrollBarHorizontal = NULL;
	mpScrollBarVertical = NULL;
	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetBackgroundColour(ge::SRGB(177, 177, 177));
	mpPane->SetSize(ge::SSize(gTrack_Editor.iCX, gTrack_Editor.iCY));
	
	//--------------------------------------------
	// Track_Top
	// Holds the tools and the buttons
	mpTrack_Top = new CTrack_Top(this, GetGUI());
	mpTrack_Top->SetInfo();
	mpTrack_Top->Init();
	mpPane->AddControl(mpTrack_Top->GetPane(), ge::SPos(0, 0));
		
	//--------------------------------------------
	// Track_Side
	// Shows the track setup, solo, mute, arm, volume, etc...
	mpTrack_Side = new CTrack_Side(this, GetGUI());
	mpTrack_Side->SetInfo();
	mpTrack_Side->Init();
	mpPane->AddControl(mpTrack_Side->GetPane(), ge::SPos(0, gTrack_Top.iCY));

	//--------------------------------------------
	// Track_Player
	// This is a container holding the audio files, it scrools when KS plays
	mpTrack_Player = new CTrack_Player(this, GetGUI());
	mpTrack_Player->SetInfo();
	mpTrack_Player->Init();
	mpPane->AddControl(mpTrack_Player->GetPane(), ge::SPos(gTrack_Side.iCX, gTrack_Top.iCY));
	
	//--------------------------------------------
	// Track_File_Browser
	// Lasse, what's with the comment? Fix later
	// This is a container that scrools when KS plays 
	mpTrack_File_Browser = new CTrack_File_Browser(this, GetGUI());
	mpTrack_File_Browser->SetInfo();
	mpTrack_File_Browser->Init();
	mpPane->AddControl(mpTrack_File_Browser->GetPane(), ge::SPos(gTrack_Side.iCX + gTrack_Player.iCX + giScrollBarSize, gTrack_Top.iCY));
	
	//--------------------------------------------
	// Backdrop and button for showing and hiding of File_Browser
	ge::SPos BitMapPos(gTrack_Side.iCX + gTrack_Player.iCX , gTrack_Top.iCY );
	ge::IBitmap* pBmp = CreateBitmap(	ge::IControl::giNoID, IDB_Back_Show_Hide_Files, BitMapPos);
	pBmp->StickTo(ge::IControl::PosTopRight, ge::SPos(- gTrack_File_Browser.iCX,gTrack_Top.iCY ));
	
	//--------------------------------------------
	// Backdrop under file browser
	pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_File_Buttom, ge::SPos(0, 0));
	pBmp->StickTo(ge::IControl::PosBottomRight, ge::SPos(0, 0));
	
	
	//--------------------------------------------
	// System Inf0
	// Lasse, what's with the comment? Fix later
	// This is a container holding the audio files, it scrools when KS plays
//	mpTrack_System_Info = new CTrack_System_Info(this, GetGUI());
//	mpTrack_System_Info->SetInfo();
//	mpTrack_System_Info->Init();
//	mpPane->AddControl(mpTrack_System_Info->GetPane(), ge::SPos(200, 0));
//	mpTrack_System_Info->GetPane()->SetVisible(false);
	
//	mpTrack_System_Info->GetPane()->StickTo(ge::IControl::PosTopRight, ge::SPos(-200, 0));
	
	
	//--------------------------------------------
	Create_Track_Player_ScrollBar_Horizontal();
	Create_Track_Player_ScrollBar_Vertical();
	
	miNr_Of_Tracks	= 0;
	miSelectedTrack	= -1;
	
	// Track Solo Mute
	mpTrackSoloMuteContainer = CSoloMuteContainer::Create();
	mpTrackSoloMuteContainer->Init(this);
	
	for(tint32 i=0; i<giNumber_Of_Tracks; i++){
		mpTrackSoloMuteContainer->AddChannel(giParam_ChMute, giParam_ChSolo, giSection_First_Track+i);
	}
	
	
	// Busses Solo Mute
	mpBussesSoloMuteContainer = CSoloMuteContainer::Create();
	mpBussesSoloMuteContainer->Init(this);
	
	for(tint32 i=0; i<giNumber_Of_Busses; i++){
		mpBussesSoloMuteContainer->AddChannel(giParam_Buss_Mute, giParam_Buss_Solo, giSection_First_Buss+i);
	}

	
	// Lasse, added 2008-05-13
	// Link left-side scrollpane to mid-section scrollpane
	ge::IScrollPane* pSPPlayer2		= mpTrack_Player->GetScrollPane2();
	ge::IScrollPane* pSPSideSetup	= pSPSideSetup = mpTrack_Side->GetSetupScrollPane();
	pSPPlayer2->AddSlaveScrollPane(pSPSideSetup, ge::IControl::ResizeAbsoluteY);
	// .. Lasse
	
	// Chase pointer to mpTrack_Player
	mpTrack_Player->SetTrackTopPtr(mpTrack_Top);
	
	
	
}

void CKS_TrackEditorPane::ConnectControls()
{
	mpTrack_Top->ConnectControls();
	mpTrack_Side->ConnectControls();
//	mpTrack_System_Info->ConnectControls();
	
	
	
	mbInited = true;
}

void CKS_TrackEditorPane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
/*	switch(pControl->GetID()) {
		case giControlID_Play:
			{
				mbPlaying = (iValueNew != 0);
			}
			break;
	}*/
} // EventValueChange


void CKS_TrackEditorPane::EventGeneric(ge::IControl* pControl, void* pEventData)
{
}

	
void CKS_TrackEditorPane::UpdateGlobalData( tint32 iType,tint32  iExtra, tint32  iID, tint32 iValue)
{
	
	switch(iType){
		case giParamID_KS_Time_Signature:
		case giParamID_KS_Tempo:{
			SGlobalData sData;
			sData.iExtra	= iExtra;
			sData.iID		= iID;
			sData.iVal		= iValue;
			SMsg Msg;
			Msg = MsgGlobalData;
			Msg.pDataIn	= (void*)&sData;
			SendMsg(&Msg);
			break;
		}
		case giParamID_Info_Radio_Button: {
			SGlobalData sData;
			sData.iExtra	= iExtra;
			sData.iID		= Msg_Show_Setup_Screen;
			sData.iVal		= iValue;
			SMsg Msg;
			Msg = MsgGlobalData;
			Msg.pDataIn	= (void*)&sData;
			SendMsg(&Msg);
			break;
		}
		case giParamID_Info_Verify_Rights: {
			SGlobalData sData;
			sData.iExtra	= iExtra;
			sData.iID		= Msg_Verify_Rights;
			sData.iVal		= iValue;
			SMsg Msg;
			Msg = MsgGlobalData;
			Msg.pDataIn	= (void*)&sData;
			SendMsg(&Msg);
			break;
		}
		case giParamID_Loop_On:{
			
			mbLoopOn = (iValue != 0);

			break;
		}
		case giParamID_KS_Snap_To:{
			
			SMsg Msg = Msg_Set_Grid;
			SendMsg(&Msg);
			break;
		}
		default: break;
	}
}

void CKS_TrackEditorPane::UpdateTrackData(tint32 iID, tint32 iValue, tint32 iChannel)
{

/*
	mpChannels[iChannel]->UpdateChannelData(iID, iValue);
*/	
	switch(iID) {
		default: break;
		
		case giParam_ChSolo:
		case giParam_ChMute:
		{ 
			mpTrackSoloMuteContainer->UpdateChannelData( iID, iValue, iChannel);
			break;
		}
		case giParam_Track_Info_SizeY:{
			// Can't be handled by the messaage system. 
			// sending 128 paremeters to 128 chanels is to slow
		//	if(iChannel <= miNr_Of_Tracks){
				mpTrack_Side->Set_Track_Size(iChannel, iValue);
				mpTrack_Player->Set_Track_Size(iChannel, iValue);
				mpKSPlugIn->Stack_Tracks();

		//	}
			break;
		}
		case giParam_Track_Info_Show_Inserts:{
			STrackData sData;
			sData.iParam_ID	=	giParam_Track_Info_Show_Inserts;
			sData.iVal			=	iValue;
			sData.iTrack		=	iChannel;
			SMsg Msg			=	Msg_Track_Data;
			Msg.pDataIn			=	(void*)&sData;
			SendMsg(&Msg);
			break;
		}	
	}			
}

void CKS_TrackEditorPane::UpdateGUIData(tint32 iID, tint32 iValue)
{
	SGUIData sData;
	sData.iID	= iID;
	sData.iVal	= iValue;
	
	
	switch(iID){
		case giParamID_Show_Mix_Window:{
			if(iValue == 0 )
				GetPlugIn()->GetModule()->GetHost()->HideWindow(giMix_Window);
			else
				GetPlugIn()->GetModule()->GetHost()->ActivateWindow(giMix_Window);
			break;
		}
		case giParamID_Show_AUX_Window:{
			if(iValue == 0 )
				GetPlugIn()->GetModule()->GetHost()->HideWindow(giRack_Window);
			else
				GetPlugIn()->GetModule()->GetHost()->ActivateWindow(giRack_Window);
			break;
		}
		case giParamID_Show_Export_Window:{
			if(iValue == 0 )
				GetPlugIn()->GetModule()->GetHost()->HideWindow(giExport_Audio_Window);
			else
				GetPlugIn()->GetModule()->GetHost()->ActivateWindow(giExport_Audio_Window);
			break;
		}
		case giParamID_Show_Export_For_Web_Window:{
			if(iValue == 0 )
				GetPlugIn()->GetModule()->GetHost()->HideWindow(giExport_For_Web_Window);
			else
				GetPlugIn()->GetModule()->GetHost()->ActivateWindow(giExport_For_Web_Window);
			break;
		}
		case giParamID_Show_Import_Window:{
			if(iValue == 0 )
				GetPlugIn()->GetModule()->GetHost()->HideWindow(giImport_Audio_Window);
			else
				GetPlugIn()->GetModule()->GetHost()->ActivateWindow(giImport_Audio_Window);
			break;
		}
		case giParamID_Show_Projec_ID_Window:{
			if(iValue == 0 )
				GetPlugIn()->GetModule()->GetHost()->HideWindow(giProject_ID_Window);
			else
				GetPlugIn()->GetModule()->GetHost()->ActivateWindow(giProject_ID_Window);
			break;
		}
		case giParamID_Show_Sign_In_Window:{
			if(iValue == 0 )
				GetPlugIn()->GetModule()->GetHost()->HideWindow(giSign_In_Window);
			else
				GetPlugIn()->GetModule()->GetHost()->ActivateWindow(giSign_In_Window);
			break;
		}
		case giParamID_Audio_Setup_Window:{
			if(iValue == 0 )
				GetPlugIn()->GetModule()->GetHost()->HideWindow(giAudio_Setup_Window);
			else
				GetPlugIn()->GetModule()->GetHost()->ActivateWindow(giAudio_Setup_Window);
			break;
		}
		case giParamID_Zoom:{
		
			miZoom = iValue;
			SMsg Msg = MsgGUIData;
			Msg.pDataIn	= (void*)&sData;
			SendMsg(&Msg);
		
			SetScrollPosToPlayHead();
			
			break;

		}

		default:{
			SMsg Msg = MsgGUIData;
			Msg.pDataIn	= (void*)&sData;
			SendMsg(&Msg);
			break;
		}
	}	
}

void CKS_TrackEditorPane::UpdateCommandData(tint32 iID, tint32 iValue)
{

	
}

void CKS_TrackEditorPane::UpdateBussData(tint32 iID, tint32 iValue, tint32 iBuss)
{
	switch(iID) {
		default: break;
		
		case giParam_ChSolo:
		case giParam_ChMute:
		{ 
			mpBussesSoloMuteContainer->UpdateChannelData( iID, iValue, iBuss);
			break;
		}
	}	
}

void CKS_TrackEditorPane::UpdateMasterData(tint32 iID, tint32 iValue)
{
}
/*
void CKS_TrackEditorPane::UpdateAuxMonitorData(tint32 iID, tint32 iValue)
{
//	mpAuxMonitor_GUI->UpdateData( iID,  iValue);
}

*/
void CKS_TrackEditorPane::OnTimer(tint32 iTimerID)
{

	if (mbInited == false) {
		return;
	}

	if (iTimerID == giTimerID_Meters_Track) {
		CKSPlugIn::EPlaybackState eState = mpKSPlugIn->GetPlaybackState();

		// Only do things if not exporting
		if (!mpKSPlugIn->IsInProgressTaskState()) {
			tint64 iSample_Pos = mpKSPlugIn->GetSongPos();
			mpTrack_Player->Set_Play_Head(iSample_Pos);
			mpTrack_Top->Update_Time_Display(iSample_Pos);

			// Only advance scroll-pos if playing and not zoomed to much
			if((miZoom < 20) && (mpKSPlugIn->IsPlayingOrRecording()))
				SetScrollPosHorizontal(iSample_Pos);

			// Calc elapsed time since previous meter get
			tuint32 uiTimeMS_Now = ITime::GetTimeMS();
			tuint32 uiTimeMS_Elapsed = uiTimeMS_Now - miuTimeMS_Prev;
			miuTimeMS_Prev = uiTimeMS_Now;

			// Get meter values and decay factor
			mpKSPlugIn->GetAllMeters_MaybeClear(mpvoid_sMeters_All, uiTimeMS_Elapsed, true);
		//	CKSDSP::SMeters_All* psMeters_All = (CKSDSP::SMeters_All*)mpvoid_sMeters_All;

			// Update main window (track view) meters
			if (mbIsVisible) {
				if (mbFirstMeterSet) {
					// Set all meters to 1.0, to make them fall-off
					mbFirstMeterSet = false;
					mpTrack_Side->Update_Tracks_Meters(NULL);
				}
				else {
					// Send pointer to struct with all meter values to all tracks and let them decide if they need it
					mpTrack_Side->Update_Tracks_Meters(mpvoid_sMeters_All);
				}
			}

			// Maybe update mixer and AUX meters
			mpKSPlugIn->Set_All_Meters(mpvoid_sMeters_All);

			// Maybe display stutter warning - only once
			if ((mbDisplayStutterWarning_Armed) && (gpDSPEngine->GetStutterCounter() > 1)) {
				// More than 1 (start of playback) stutter - that's bad
								
				mbDisplayStutterWarning_Armed = false;

				mpKSPlugIn->ShowMessageBox_NonModal(
					"It seems your system is too slow to play back this project\n"
					"in real time.\n"
					"\n"
					"Most probable causes:\n"
					"- slow harddrive\n"
					"- low memory\n"
					"- high CPU load\n"
					"\n"
					"You may still export a mix down and listen to that;\n"
					"go to the menu File -> Export.\n"
					"\n"
					"Or: If you think this is a temporary error you could\n"
					"just keep playing. This warning message won't reappear.",
					"Sorry"
					);
			}
		}
	}
} // OnTimer


void CKS_TrackEditorPane::Resize(ge::SSize SizeNew)
{
	CBasePane::Resize(SizeNew);

	SMsg Msg(Msg_Resize);
	SendMsg(&Msg);
}

void CKS_TrackEditorPane::UpdateChannelName(tint32 iTrack, const std::string& sName)
{
	mpTrack_Side->UpdateChannelName( iTrack, sName);
}


void CKS_TrackEditorPane::Create_Track_Player_ScrollBar_Vertical()
{
	//--------------------------------------------
	// Track_Player Scrollbar and backdrop for vertical scrolling
	//-------------------------------
	// Scrollbar possition
	// Position and Stick To Podition
	ge::SPos Pos(gTrack_Side.iCX + gTrack_Player.iCX , gTrack_Top.iCY + gTrack_Time_Pos.iCY);
	ge::SPos StickToPos(- gTrack_File_Browser.iCX, gTrack_Top.iCY + gTrack_Time_Pos.iCY );
	//-----
	// Scrollbar Backdrop left top
	Pos.iY			+= 14;
	StickToPos.iY	+= 14;
	ge::IBitmap* pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Top, Pos);
	pBmp->StickTo(ge::IControl::PosTopRight, StickToPos);
	//-----
	// Scrollbar Backdrop Center part
	Pos.iY			+= 9;
	StickToPos.iY	+= 9;
	pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Vertical_Center, Pos);
	pBmp->SetSize(ge::SSize(giScrollBarSize, gTrack_Editor.iCY - gTrack_Top.iCY - gTrack_Time_Pos.iCY - 50 ));
	pBmp->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteY);
	pBmp->StickTo(ge::IControl::PosTopRight, StickToPos);
	//-----
	// Scrollbar Backdrop Bottom part
	Pos.iY = gTrack_Editor.iCY - giScrollBarSize - 9 - 14;
	StickToPos.iY = -giScrollBarSize - 14;
	pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Bottom, Pos);
	pBmp->StickTo(ge::IControl::PosBottomRight, StickToPos);
	
	//-----
	// Scroll pos
	ge::SScrollPos ScrollPos;
	ge::SSize size(gTrack_Scroll_Editor.iCX, gTrack_Player.iCY-gTrack_Time_Pos.iCY);
	ScrollPos.AreaSize = ge::SSize(gTrack_Scroll_Editor.iCX, gTrack_Scroll_Editor.iCY);
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
		mpTrack_Player->GetScrollPane2(),
		ScrollPos);
	//-----
	// Add ScrollBar
	//-----
	Pos.iX =	gTrack_Side.iCX + gTrack_Player.iCX ;
	Pos.iY =	gTrack_Top.iCY + gTrack_Time_Pos.iCY;
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpScrollBarVertical),Pos);
	
	//-----
	// Glue scrollbar to Top Right
	StickToPos.iX = - gTrack_File_Browser.iCX;
	StickToPos.iY = gTrack_Top.iCY + gTrack_Time_Pos.iCY;
	//ge::SPos StickToPos(- gTrack_File_Browser.iCX, gTrack_Top.iCY + gTrack_Time_Pos.iCY );
	mpScrollBarVertical->StickTo(ge::IControl::PosTopRight, StickToPos);
	// Make it resize with main pane
	mpScrollBarVertical->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteY);
	
	// Lasse, mod 2008-05-09 - make player2 auto-resize with player1
	/*
	mpTrack_Player->GetScrollPane2()->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteY);
	*/
	// Make player2 fill all of player1 - and then move down below top
	mpTrack_Player->GetScrollPane2()->CloneParentRect();
	mpTrack_Player->GetScrollPane2()->GetScrollPos(ScrollPos);
	ScrollPos.AreaSize.iCY -= giTime_Pos_SizeY;
	ScrollPos.VisibleRect.iCY -= giTime_Pos_SizeY;
	ge::SPos posPlayer2(0, giTime_Pos_SizeY);
	mpTrack_Player->GetScrollPane2()->SetRelPos(posPlayer2);
	mpTrack_Player->GetScrollPane2()->SetScrollPos(ScrollPos, false);
	// Resizing
	mpTrack_Player->GetScrollPane2()->SetAutomaticResize(true, ge::IControl::ResizeAbsolute);//ResizeAbsoluteX);
	mpTrack_Player->GetScrollPane2()->SetAutomaticResizeOfSubPane(true, ge::IControl::ResizeAbsoluteX);
	// .. Lasse

	// Lasse, added 2008-05-09
	// Update scrollbar sizes
	mpScrollBarVertical->SetScrollPos(ScrollPos);
	// .. Lasse
	
	// Lasse, added 2008-04-17, 04-30
	mpScrollBarVertical->AddListener(this);
	mpScrollBarHorizontal->AddListener(this);
	// .. Lasse
}

void CKS_TrackEditorPane::SetScrollPosHorizontal(tuint64 uiSample)
{

	tfloat32 fPixelPrSample		=	(tfloat32)mpKSPlugIn->GetPixelPrSample();
	tint32 iPlayHead_Pos		=	(tint32)(fPixelPrSample * (tfloat32)uiSample);
	

	ge::SScrollPos scrollposBar;
	mpScrollBarHorizontal->GetScrollPos(scrollposBar);
	// Size of window
	tint32 iVisible =	scrollposBar.VisibleRect.iCX;
	tint32 iMidt	=	iVisible / 2;
	tint32 iPos		=	scrollposBar.VisibleRect.iX;
	
	if(mbLoopOn){
		SLoop_Drawing_Info info		=	gpDSPEngine->Get_Loop_Drawing_Info();
		tint32 fEnd					=	(tint32)((tfloat32)info.uiLoop_End   * fPixelPrSample);
		
		tint32 iWindowEnd	= iPos + iVisible - 32 ;
		// If the loop endisinside the window
		if(  fEnd < iWindowEnd  )
			return;
		
			
	}
	if( iPlayHead_Pos - iPos > iMidt){
	
		mfGoToPos	=	(tfloat32)(iPlayHead_Pos - iMidt);
		scrollposBar.VisibleRect.iX = (tint32)((scrollposBar.VisibleRect.iX * 0.75) + (mfGoToPos * 0.25f));
		mpScrollBarHorizontal->SetScrollPos(scrollposBar);
	}
	// Size of inner pane
//	tint32 iSize = scrollposBar.AreaSize.iCX;

	


}

void CKS_TrackEditorPane::SetScrollPosToPlayHead()
{
	tint32 iPlayHead_Pos	=	(tint32)(mpKSPlugIn->GetPixelPrSample() * mpKSPlugIn->GetSongPos());

	ge::SScrollPos scrollposBar;
	mpScrollBarHorizontal->GetScrollPos(scrollposBar);
	// Size of window
	tint32 iVisible = scrollposBar.VisibleRect.iCX;
	tint32 iMidt	=	iVisible / 2;
	
	// Scrool position
	//tint32 iPos = scrollposBar.VisibleRect.iX;
	scrollposBar.VisibleRect.iX	=	iPlayHead_Pos - iMidt;
	mpScrollBarHorizontal->SetScrollPos(scrollposBar);

}
void CKS_TrackEditorPane::SetScrollPosToLoopStart()
{
	
	SLoop_Drawing_Info sInfo =	gpDSPEngine->Get_Loop_Drawing_Info();



	
 	tint32 iPlayHead_Pos	=	(tint32)(mpKSPlugIn->GetPixelPrSample() * (tfloat32)sInfo.uiLoop_Start);

	ge::SScrollPos scrollposBar;
	mpScrollBarHorizontal->GetScrollPos(scrollposBar);
	// Size of window
//	tint32 iVisible = scrollposBar.VisibleRect.iCX;
//	tint32 iMidt	=	iVisible / 2;
	
	// Scrool position
	//tint32 iPos = scrollposBar.VisibleRect.iX;
	if(scrollposBar.VisibleRect.iX > iPlayHead_Pos){
		scrollposBar.VisibleRect.iX	=	iPlayHead_Pos;
		mpScrollBarHorizontal->SetScrollPos(scrollposBar);
	}
	


}

void CKS_TrackEditorPane::SetScrollPosHorizontalToStart()
{
	ge::SScrollPos scrollposBar;
	mpScrollBarHorizontal->GetScrollPos(scrollposBar);
	scrollposBar.VisibleRect.iX =0;
	mpScrollBarHorizontal->SetScrollPos(scrollposBar);
}

void CKS_TrackEditorPane::Create_Track_Player_ScrollBar_Horizontal()
{
	//--------------------------------------------
	// Track_Player Scrollbar and backdrop for horizontal scrolling
	//-------------------------------
	// Scrollbar possition
	// Position and Stick To Podition
	ge::SPos Pos(gTrack_Side.iCX + 14 , gTrack_Editor.iCY - 14 );
	ge::SPos StickToPos( gTrack_Side.iCX + 14, 0 );
	//-----
	// Scrollbar Backdrop Left part
	ge::IBitmap* pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Left, Pos);
	pBmp->StickTo(ge::IControl::PosBottomLeft, StickToPos);
	//-----
	// Scrollbar Backdrop Center part
	Pos.iX			+= 9;
	StickToPos.iX	+= 9;
	pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Horizontal_Center, Pos);
	pBmp->SetSize(ge::SSize( gTrack_Editor.iCX - gTrack_Side.iCX - gTrack_File_Browser.iCX  - 46 - giScrollBarSize , giScrollBarSize  ));
	pBmp->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteX);
	pBmp->StickTo(ge::IControl::PosBottomLeft, StickToPos);
	//-----
	// Scrollbar Backdrop Right part
	Pos.iX = gTrack_Editor.iCX -  gTrack_File_Browser.iCX  - gTrack_Side.iCX - 14 ;
	StickToPos.iX = - gTrack_File_Browser.iCX - giScrollBarSize -14 ;
	pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Right, Pos);
	pBmp->StickTo(ge::IControl::PosBottomRight, StickToPos);
	
	//-----
	// Scroll pos
	ge::SScrollPos ScrollPos;
	ScrollPos.AreaSize = ge::SSize(gTrack_Scroll_Editor.iCX, gTrack_Player.iCY);//gTrack_Scroll_Editor.iCY);
	ScrollPos.VisibleRect = ge::SRect(ge::SPos(0, 0), gTrack_Player);
	
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
	mpScrollBarHorizontal->Init(giChC_Track_Player_ScrollBar_Horizontal,
		ge::IScrollBar::TypeHorizontal,
		GetWindow(),
		GetResMan(),
		piIDs,
		mpTrack_Player->GetScrollPane(),
		ScrollPos);
	//-----
	// Add ScrollBar
	//-----
	Pos.iX = gTrack_Side.iCX ;
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpScrollBarHorizontal),Pos);
	
	//-----
	// Glue scrollbar to Bottom Left.iCX;
	StickToPos.iX = gTrack_Side.iCX;
	StickToPos.iY = 0;
	mpScrollBarHorizontal->StickTo(ge::IControl::PosBottomLeft, StickToPos);
	// Make it resize with main pane
	mpScrollBarHorizontal->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteX);
	mpTrack_Player->GetScrollPane()->SetScrollPos(ScrollPos, false);
	mpTrack_Player->GetScrollPane()->SetAutomaticResize(true, ge::IControl::ResizeAbsolute);
	mpTrack_Player->GetScrollPane()->SetAutomaticResizeOfSubPane(true, ge::IControl::ResizeAbsoluteY);
	
}

void CKS_TrackEditorPane::SendMsgFromTop(SMsg* pMsg)
{
	
	switch(pMsg->iOpcode) 
	{
		case Msg_Stack_Tracks: {

			if (mpKSPlugIn->AreGUIsReady()) {
				mbIsVisible = true;
				SendMsg(pMsg);
			}
			break;
		}
		case Msg_Resize:
		case Msg_Add_Region:
		case Msg_Track_Data:
		case Msg_Track_Mouse_Data:
		case Msg_Select_Track:
		case Msg_Reset_Drag_Region_Size:
		case Msg_Reset_Update_Play_Head_Size_Y:
//		case Msg_Show_Grid:
		case Msg_Delete_All_Regions:
		case Msg_Deselect_All:
		case Msg_Track_MouseDrop:
		case Msg_Track_MouseDrag:
		case Msg_Draw_Loop:
		case Msg_Go_To_Mouse:
		case Msg_Progress:
		case Msg_Set_Grid:
		{
			SendMsg(pMsg);
			break;
		}
		case Msg_Go_To_Playhead:
		{
			SetScrollPosToLoopStart();
			break;
		}

		case Msg_Update_Track_Selection:
		{
			tint32 iTrack = (tint32)(pMsg->pDataIn);
			mpTrack_Player->mpTrack_Player2->RedrawSelection_For_Track(iTrack);
		}
		break;
		
		case Msg_Track_MouseClick_Inside: {
			miSelectedTrack = (tint32)(pMsg->pDataIn);
			SendMsg(pMsg);
			//UpdateGUI();
			break;			
		}
		
		case Msg_Track_MouseClick_Outside: {
			tint32 iOld = (tint32)(pMsg->pDataIn);
			if (iOld == miSelectedTrack) {
				miSelectedTrack = -1;
				SendMsg(pMsg);
				//UpdateGUI();
			}
			break;			
		}
		case Msg_Track_MouseMove_Inside: {
			miMouseOverTrack = (tint32)(pMsg->pDataIn);
			SendMsg(pMsg);
			//UpdateGUI();
			break;			
		}
		case Msg_Track_MouseMove_Outside: {
			tint32 iOld = (tint32)(pMsg->pDataIn);
			if (iOld == miMouseOverTrack) {
				miMouseOverTrack = -1;
				SendMsg(pMsg);
				//UpdateGUI();
			}
			break;			
		}
		case MsgGUIData: {
			SGUIData sData = *(SGUIData*)pMsg->pDataIn;
			
			switch(sData.iID){
				/*
				case Msg_Show_Info: {
					mpTrack_System_Info->SetVisible(sData.iVal != 0);
					break;
				}
				*/
				case Msg_Go_To_Start:{
					SetScrollPosHorizontalToStart();
					break;
				}
				default:{
					SendMsg(pMsg);
				}
			}
			break;
		}	
	}	
}



void CKS_TrackEditorPane::AddTrack(tbool bRedraw /*= true*/)
{

	return;
	{
		miNr_Of_Tracks++;
		
		// Check for max nr tracks
		if(miNr_Of_Tracks >= giNumber_Of_Tracks) 
			miNr_Of_Tracks = giNumber_Of_Tracks-1;
		else if (bRedraw) {
			// Lasse, 2008-05-13 - speed optimized, no longer needed
			/*
			// Lasse, 2008-05-13 - we really need this!!! DON'T JUST DELETE
			// Should be speed optimized
			SMsg msg(Msg_Stack_Tracks);
			mpTrack_Player->mpTrack_Player2->HandleMsg(&msg);
			// .. Lasse
			*/
			// .. Lasse

			// Only update GUI if a new track is added
		//	UpdateGUI();
		}
	}
	
}

void CKS_TrackEditorPane::DeleteTrack(tbool bRedraw /*= true*/)
{

}

tbool CKS_TrackEditorPane::DoKeyDown(ge::EKey Key)
{

	switch(Key){
		case ge::KeySpace: {
			if (mpTrack_Top->mbPlaying) {
				GetPane()->GetControl(giControlID_Play)->SetValue(0);
				//mpTrack_Top->mbPlaying = false;
			}
			else {
				GetPane()->GetControl(giControlID_Play)->SetValue(1);
				//mpTrack_Top->mbPlaying = true;
			}
			return true;
		}
		case ge::KeyReturn:{
			// Go back to start
			GetPane()->GetControl(giControlID_Go_To_Start)->SetValue(1);
			SetScrollPosHorizontalToStart();
			return true;
		}
		case ge::KeyEqual:{
			
			// Show/hide Mix Window
			tbool bTest = (GetPlugIn()->GetGlobalParm(giParamID_Show_Mix_Window, giSectionGUI) != 0);
			tbool bReallyVisible = GetPlugIn()->GetModule()->GetHost()->IsWindowVisible(giMix_Window) == 0 ? false : true;
			if (bTest != bReallyVisible) {
				//GetPlugIn()->SetGlobalParm(giParamID_Show_Mix_Window, !bTest, giSectionGUI);
				mpKSPlugIn->GetParmMan()->Set(true, !bTest, giParamID_Show_Mix_Window, de::IParameterManager::TypeGlobal, giSectionGUI);
			}
			mpKSPlugIn->GetParmMan()->Set(true, !bReallyVisible, giParamID_Show_Mix_Window, de::IParameterManager::TypeGlobal, giSectionGUI);
			//GetPlugIn()->SetGlobalParm(giParamID_Show_Mix_Window, !bReallyVisible, giSectionGUI);
			return true;
		}
		/*
		case ge::Keya:{
		//!!! For test only
			SetScrollPosToPlayHead();
			break;
		}
		*/
		case ge::KeySlash:{
		
		// Show/hide AUX Window
			tbool bTest = (GetPlugIn()->GetGlobalParm(giParamID_Show_AUX_Window, giSectionGUI) != 0);
			tbool bReallyVisible = (GetPlugIn()->GetModule()->GetHost()->IsWindowVisible(giRack_Window) != 0);
			if (bTest != bReallyVisible) {
				mpKSPlugIn->GetParmMan()->Set(true, !bTest, giParamID_Show_AUX_Window, de::IParameterManager::TypeGlobal, giSectionGUI);
			}
			mpKSPlugIn->GetParmMan()->Set(true, !bReallyVisible, giParamID_Show_AUX_Window, de::IParameterManager::TypeGlobal, giSectionGUI);
			return true;
			
		}
		case ge::KeyBackSpace:
		case ge::KeyDelete:{
			Do_Delete();
			
			break;
		}
		case ge::Key1: {
			GetPlugIn()->SetGlobalParm(giParamID_Tool_Selected,giTool_Hand, giSectionGUI);
			break;
		}
		
		case ge::Key2: {
			GetPlugIn()->SetGlobalParm(giParamID_Tool_Selected,giTool_Trim, giSectionGUI);
			break;
		}
		
		case ge::Key3: {
			GetPlugIn()->SetGlobalParm(giParamID_Tool_Selected,giTool_Select, giSectionGUI);
			break;
		}
		case ge::Key4: {
			GetPlugIn()->SetGlobalParm(giParamID_Tool_Selected,giTool_Cut, giSectionGUI);
			break;
		}
		case ge::Keyd: {
			gpDSPEngine->DuplicateRegion();
			break;
		}
		case ge::Keyf: {
			tbool bTest = (GetPlugIn()->GetGlobalParm(giParamID_Show_Fade, giSectionGUI) != 0);
			mpKSPlugIn->GetParmMan()->Set(true, !bTest, giParamID_Show_Fade, de::IParameterManager::TypeGlobal, giSectionGUI);
			//GetPlugIn()->SetGlobalParm(giParamID_Show_Fade,!bTest, giSectionGUI);
			break;
		}
		case ge::Keyg: {
			tbool bTest = (GetPlugIn()->GetGlobalParm(giParamID_Grid, giSectionGUI) != 0);
			mpKSPlugIn->GetParmMan()->Set(true, !bTest, giParamID_Grid, de::IParameterManager::TypeGlobal, giSectionGUI);
			//GetPlugIn()->SetGlobalParm(giParamID_Grid,!bTest, giSectionGUI);
			break;
		}
		case ge::Keyk: {
		
			gpDSPEngine->Set_Loop_To_Selection();
			break;
		}
		case ge::Keyl: {
			tbool bTest = (GetPlugIn()->GetGlobalParm(giParamID_Loop_On, giSectionGlobal) != 0);
			mpKSPlugIn->GetParmMan()->Set(true, !bTest, giParamID_Loop_On, de::IParameterManager::TypeGlobal, giSectionGlobal);
			//GetPlugIn()->SetGlobalParm(giParamID_Loop_On,!bTest, giSectionGlobal);
			break;
		}
		case ge::Keyo: {
			
			mbZoomFlipFlop = !mbZoomFlipFlop;
			if(mbZoomFlipFlop){
				miStoredZoom = GetPlugIn()->GetGlobalParm(giParamID_Zoom, giSectionGUI);
				mpKSPlugIn->GetParmMan()->Set(true, giZoom_Levels - 10, giParamID_Zoom, de::IParameterManager::TypeGlobal, giSectionGUI);
			}
			else{
				mpKSPlugIn->GetParmMan()->Set(true, miStoredZoom, giParamID_Zoom, de::IParameterManager::TypeGlobal, giSectionGUI);
				
			}
			break;
		}
		case ge::Keyp: {
			CBasePane::SMsg Msg(Msg_Go_To_Mouse);
			SendMsgFromTop(&Msg);
			break;
		}
		case ge::Keyw: {
			tbool bTest = (GetPlugIn()->GetGlobalParm(giParamID_Show_Waveform, giSectionGUI) != 0);
			mpKSPlugIn->GetParmMan()->Set(true, bTest, giParamID_Show_Waveform, de::IParameterManager::TypeGlobal, giSectionGUI);
			//GetPlugIn()->SetGlobalParm(giParamID_Show_Waveform,!bTest, giSectionGUI);
			break;
		}
		/*
		case ge::Keyx: {
		
			//!!! Lasse remove this when done
			static tint32 iTest = 0;
			iTest += 10;
			if(iTest > 100) iTest = 0;
			CKSPlugIn* pPlugIn	=	dynamic_cast<CKSPlugIn*>(GetPlugIn());
			pPlugIn->Set_Progress(iTest != 0, iTest);

			break;
		}
		*/
		case ge::KeyMinus: {
			tint32 iTest = GetPlugIn()->GetGlobalParm(giParamID_Zoom, giSectionGUI) -1;
			mpKSPlugIn->GetParmMan()->Set(true, iTest, giParamID_Zoom, de::IParameterManager::TypeGlobal, giSectionGUI);
			break;
		}
		case ge::KeyPlus: {
			tint32 iTest = GetPlugIn()->GetGlobalParm(giParamID_Zoom, giSectionGUI) +1;
			mpKSPlugIn->GetParmMan()->Set(true, iTest, giParamID_Zoom, de::IParameterManager::TypeGlobal, giSectionGUI);
			break;
		}
		
		case ge::KeyLeft: {
			mpKSPlugIn->PlaybackRewind();
			break;
		}
		case ge::KeyRight: {
		
			mpKSPlugIn->PlaybackFF();
			break;
		}
		
		
		
		
	}

	return false;
}

void CKS_TrackEditorPane::Do_Delete()
{
//	tint32 iTool = GetPlugIn()->GetGlobalParm(giParamID_Tool_Selected, giSectionGUI);
	
	gpDSPEngine->Delete_Selection();
	
}

tbool CKS_TrackEditorPane::DoKeyUp(ge::EKey Key)
{
	switch(Key){
		
		
		case ge::KeyLeft: {
			mpKSPlugIn->PlaybackRewind();
			break;
		}
		case ge::KeyRight: {
		
			mpKSPlugIn->PlaybackFF();
			break;
		}
		
	}



	return false;
}


