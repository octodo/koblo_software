#include "KSOS.h"

CTrack_Top::CTrack_Top(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;
	miSecond	=	-1;
	miMin		=	-1;
	miHour		=	-1;
}

CTrack_Top::~CTrack_Top()
{
}

void CTrack_Top::SetInfo()
{
	miZoomLevel		=	gi_Default_Zoom_Level;

	mbPlaying		=	false;
	mbFFPressed		=	false;
	mbRewindPressed	=	false;
	



}

void CTrack_Top::Init()
{
	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(gTrack_Top.iCX, gTrack_Top.iCY));
	mpPane->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteX);

	//Back drop
	ge::IBitmap* pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Track_Top, ge::SPos(0, 0));
	pBmp->SetSize(ge::SSize(gTrack_Top.iCX, gTrack_Top.iCY));
	pBmp->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteX);

	//-------------------------------
	// Transporter Buttons
	tint32	iPosX	=	8;
	tint32	iPosY	=	6;
	
	CreateButton(giControlID_Go_To_Start, IDB_Button_GoToStart, ge::SPos(iPosX, iPosY), false); iPosX		+= 28;
	// FF
	ge::IButton* pButton = CreateButton(giControlID_Fast_Back, IDB_Button_FastBack, ge::SPos(iPosX, iPosY), false); iPosX		+= 27;
	pButton->SetEventMouseUpDown();
	// FF
	pButton = CreateButton(giControlID_Forward, IDB_Button_Fast_Forward, ge::SPos(iPosX, iPosY), false);iPosX		+= 27; 
	pButton->SetEventMouseUpDown();
	
	CreateButton(giControlID_Go_To_End, IDB_Button_GoToEnd, ge::SPos(iPosX, iPosY), false); iPosX			+= 27;
	
	iPosX			=	8;
	iPosY			=	33;
	Create2StateButton(giControlID_Record, IDB_Button_Record, ge::SPos(iPosX, iPosY), true); iPosX		+= 28;
	Create2StateButton(giControlID_Play, IDB_Button_Play, ge::SPos(iPosX, iPosY), true); iPosX			+= 27;
	
	CreateButton(giControlID_Stop, IDB_Button_Stop, ge::SPos(iPosX, iPosY), false); iPosX					+= 27;
	Create2StateButton(giControlID_Loop, IDB_Button_Loop, ge::SPos(iPosX, iPosY), true); 
	
	//-------------------------------
	iPosX			= 130;
	// Time Display background
	pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Track_TimeDisplay, ge::SPos(iPosX, 0));iPosX		+= 15;


	// Tempo display Hour
	mpTextHour = CreateDisplay( giControlID_Hour_Display, ge::SPos(iPosX, 14),  ge::SSize(32, 28), CreateFont(Generic128, IDB_Time_Display_Numbers, ge::SRGB(0, 255, 0)));
	mpTextHour->SetHorzAlignment(ge::IText::HorzAlignRight);
	mpTextHour->SetVertAlignment(ge::IText::VertAlignBottom);
	mpTextHour->EnableTextLimiting(false);
	mpTextHour->SpeedOptimize_RedrawUsingFirstNonZeroSize();
	iPosX += 42;
	// Tempo display Min
	mpTextMinute = CreateDisplay( giControlID_Min_Display, ge::SPos(iPosX, 14),  ge::SSize(32, 28), CreateFont(Generic128, IDB_Time_Display_Numbers, ge::SRGB(0, 255, 0)));
	mpTextMinute->SetHorzAlignment(ge::IText::HorzAlignLeft);
	mpTextMinute->SetVertAlignment(ge::IText::VertAlignBottom);
	mpTextMinute->EnableTextLimiting(false);
	mpTextMinute->SpeedOptimize_RedrawUsingFirstNonZeroSize();
	iPosX += 42;
	// Tempo display Sec
	mpTextSecond = CreateDisplay( giControlID_Sec_Display, ge::SPos(iPosX, 14),  ge::SSize(32, 28), CreateFont(Generic128, IDB_Time_Display_Numbers, ge::SRGB(0, 255, 0)));
	mpTextSecond->SetHorzAlignment(ge::IText::HorzAlignLeft);
	mpTextSecond->SetVertAlignment(ge::IText::VertAlignBottom);
	mpTextSecond->EnableTextLimiting(false);
	mpTextSecond->SpeedOptimize_RedrawUsingFirstNonZeroSize();
	iPosX += 42;
	// Tempo display mSec
	mpTextMilliSecs = CreateDisplay( giControlID_MSec_Display, ge::SPos(iPosX, 14),  ge::SSize(48, 28), CreateFont(Generic128, IDB_Time_Display_Numbers, ge::SRGB(0, 255, 0)));
	mpTextMilliSecs->SetHorzAlignment(ge::IText::HorzAlignLeft);
	mpTextMilliSecs->SetVertAlignment(ge::IText::VertAlignBottom);
	mpTextMilliSecs->EnableTextLimiting(false);
	mpTextMilliSecs->SpeedOptimize_RedrawUsingFirstNonZeroSize();
	
	// Sample Start
	iPosX += 94;
	iPosY = 12;
	mpText_Sample_Start = CreateDisplay( giControlID_Sample_Start_Display, ge::SPos(iPosX, iPosY),  ge::SSize(82, 10), CreateFont(Generic128, IDB_Font_Minix_Tight, ge::SRGB(0, 255, 0)));
	mpText_Sample_Start->SetHorzAlignment(ge::IText::HorzAlignLeft);
	mpText_Sample_Start->SetVertAlignment(ge::IText::VertAlignBottom);
	mpText_Sample_Start->EnableTextLimiting(false);
	mpText_Sample_Start->SpeedOptimize_RedrawUsingFirstNonZeroSize();
	iPosY += 14;
	// Sample End
	mpText_Sample_End = CreateDisplay( giControlID_Sample_Start_Display, ge::SPos(iPosX, iPosY),  ge::SSize(82, 10), CreateFont(Generic128, IDB_Font_Minix_Tight, ge::SRGB(0, 255, 0)));
	mpText_Sample_End->SetHorzAlignment(ge::IText::HorzAlignLeft);
	mpText_Sample_End->SetVertAlignment(ge::IText::VertAlignBottom);
	mpText_Sample_End->EnableTextLimiting(false);
	mpText_Sample_End->SpeedOptimize_RedrawUsingFirstNonZeroSize();
	iPosY += 14;
	// Sample Cursor
	mpText_Sample_Cursor = CreateDisplay( giControlID_Sample_Start_Display, ge::SPos(iPosX, iPosY),  ge::SSize(82, 10), CreateFont(Generic128, IDB_Font_Minix_Tight, ge::SRGB(0, 255, 0)));
	mpText_Sample_Cursor->SetHorzAlignment(ge::IText::HorzAlignLeft);
	mpText_Sample_Cursor->SetVertAlignment(ge::IText::VertAlignBottom);
	mpText_Sample_Cursor->EnableTextLimiting(false);
	mpText_Sample_Cursor->SpeedOptimize_RedrawUsingFirstNonZeroSize();

	//-------------------------------
	// Tools Buttons
	iPosX	+=	74;
	iPosY	=	6;
	ge::IRadioButtons* pToolRadioButton = CreateRadioButtonsGroup(giCtrl_ToolSelection, ge::SPos(0, 0));
	CreateRadioButton(ge::IControl::giNoID, IDB_Button_Hand, ge::SPos(iPosX, iPosY), pToolRadioButton, true);   iPosX += 30;
	CreateRadioButton(ge::IControl::giNoID, IDB_Button_Trim, ge::SPos(iPosX, iPosY), pToolRadioButton, true);   iPosX += 29;
	CreateRadioButton(ge::IControl::giNoID, IDB_Button_Select, ge::SPos(iPosX, iPosY), pToolRadioButton, true); iPosX += 29;
	CreateRadioButton(ge::IControl::giNoID, IDB_Button_Cut, ge::SPos(iPosX, iPosY), pToolRadioButton, true); iPosX	+= 40;
	// Fade buttons
	Create2StateButton(giCtrl_Show_Fade, IDB_Button_Fade, ge::SPos(iPosX, 6), true);iPosX	+= 30;
	// Show Waveform
	Create2StateButton(giCtrl_Show_Waveform, IDB_Button_Waveform, ge::SPos(iPosX, 6), true);iPosX	+= 29;
	// Grid button
	Create2StateButton(giCtrl_Grid, IDB_Button_Grid, ge::SPos(iPosX, 6), true);
	//-------------------------------
	
	
	// Time Display background
	pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_BPM_Mesure_Grid, ge::SPos(442, 39));
	
	// Tempo display
	ge::IText* pText =  CreateDisplay( giChC_Tempo_Display, ge::SPos(464, 41),  ge::SSize(42, 14), CreateFont(Generic128, IDF_Font_Fuxley_712, ge::SRGB(0, 255, 0)),true);
	pText->SetClicksOutsideAsSuccess();

	// Time signature selection popup
	PreparePopups();
	CreatePop(giControlID_Timesignatur_Popup, IDB_Button_Invisible_16_64, mpTime_Signature_Menu, ge::SPos(544, 40));
	// Snap to grid popup
	CreatePop(giControlID_Snap_To_Popup, IDB_Button_Invisible_16_64, mpSnapTo_Menu, ge::SPos(610, 40));


	// AUX button
	ge::IButton*  p2Button = CreateButton(giControlID_View_AUX_Button, IDB_Track_Button_AUX, ge::SPos(117+46, 12), false);;
	 p2Button->StickTo(ge::IControl::PosTopRight, ge::SPos(-16, 12));
//	 p2Button->SetEventMouseUp();
	

	// Mix button
	p2Button = CreateButton(giControlID_Open_Mix_Button, IDB_Button_Mix, ge::SPos(117+46+45, 12), false);
	p2Button->StickTo(ge::IControl::PosTopRight, ge::SPos(-16-45, 12));
//	p2Button->SetEventMouseUp();
	

	//---------------------------------
	// Progress
	//Back drop
	mpProgress_Back = CreateBitmap(ge::IControl::giNoID, IDB_Back_Processing, ge::SPos(135, 11));
	mpProgress_Bar = CreateBitmap(ge::IControl::giNoID, IDB_Back_Progres_Bar, ge::SPos(144, 38));

	mpProgress_Text = CreateText(
		ge::IControl::giNoID,
		ge::SPos(144, 28), ge::SSize(270, 12),
		CreateFont(Generic128, IDF_Font_Fuxley_712, ge::SRGB(0, 255, 0)),
		"Please wait",
		ge::IText::HorzAlignLeft,
		ge::IText::VertAlignCenter);
	mpProgress_Text->EnableTextLimiting();
	mpProgress_Text->MakeEditable();


	Show_Progress(false);
	Set_Progress(0);
	
	
}


void CTrack_Top::ConnectControls()
{
	
	RegisterGlobalControl(	giSectionGUI, giCtrl_ToolSelection,		giParamID_Tool_Selected);
//	RegisterGlobalControlWithLinearTranslation(	giSectionGUI, giChC_Zoom_Slider,giParamID_Zoom);
	RegisterGlobalControl(	giSectionGUI, giCtrl_Grid,	giParamID_Grid);
	
	
	RegisterGlobalControlWithDisplayTranslater(giSectionGlobal, giChC_Tempo_Display, giParamID_KS_Tempo, 1, 5, 1);
	RegisterGlobalControl(giSectionGlobal, giControlID_Timesignatur_Popup, giParamID_KS_Time_Signature);
	RegisterGlobalControl(giSectionGlobal, giControlID_Snap_To_Popup, giParamID_KS_Snap_To);

	RegisterGlobalControl(	giSectionGlobal, giControlID_Loop, giParamID_Loop_On);
	RegisterGlobalControl(	giSectionGUI, giCtrl_Show_Fade,	giParamID_Show_Fade);
	RegisterGlobalControl(	giSectionGUI, giCtrl_Show_Waveform,	giParamID_Show_Waveform);
 

}

void CTrack_Top::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	switch(pControl->GetID()) {
	
		case giControlID_Record:
			gpApplication->SetRecord(iValueNew ? true : false);
			break;
		case giControlID_Play:{
			if (iValueNew == 1) {
				gpApplication->PlaybackStart();
				mbPlaying = true;
			}
			else {
				gpApplication->PlaybackStop();
				mbPlaying = false;
			}
			break;
		}
		case giControlID_Stop:{
			SetControlValue(GetPaneID(), giControlID_Play, 0);
			gpApplication->PlaybackStop();
			mbPlaying = false;
			break;
		}
		case giControlID_Forward:{
			if (iValueNew) {
				gpApplication->PlaybackFF();
				mbFFPressed = true;
			}
			else {
				mbFFPressed = false;
			}
			break;
		}
		case giControlID_Fast_Back:{
			if (iValueNew) {
				gpApplication->PlaybackRewind();
				mbRewindPressed = true;
			}
			else {
				mbRewindPressed = false;
			}
			break;
		}
		case giControlID_Go_To_Start:{
			gpApplication->PlaybackGoToStart();
			
			
			SGUIData sData;
			sData.iID	= Msg_Go_To_Start;
	
			SMsg Msg;
			Msg = MsgGUIData;
			Msg.pDataIn	= (void*)&sData;
			SendMsgFromTop(&Msg);

			break;
		}
		case giControlID_Go_To_End:{
			gpApplication->PlaybackGoToEnd();
			break;
		}
		case giControlID_Open_Mix_Button:{
			if (iValueNew == 0) {
				// Show/ hide Mix Window
				tbool bTest = (gpApplication->GetGlobalParm(giParamID_Show_Mix_Window, giSectionGUI) != 0);
				gpApplication->GetParmMan()->Set(true, !bTest, giParamID_Show_Mix_Window, de::IParameterManager::TypeGlobal, giSectionGUI);
			}
			break;
		}
		case giControlID_View_AUX_Button:{
			if (iValueNew == 0) {
				// Show/ hide Mix Window
				tbool bTest = (gpApplication->GetGlobalParm(giParamID_Show_AUX_Window, giSectionGUI) != 0);
				tbool bReallyVisible = gpApplication->GetModule()->GetHost()->IsWindowVisible(giRack_Window) == 0 ? false : true;
				if (bTest != bReallyVisible) {
					gpApplication->SetGlobalParm(giParamID_Show_AUX_Window, !bTest, giSectionGUI);
				}
				gpApplication->GetParmMan()->Set(true, !bReallyVisible, giParamID_Show_AUX_Window, de::IParameterManager::TypeGlobal, giSectionGUI);
			}
			break;
		}
		
		case giCtrl_Show_Fade:{
			//gpDSPEngine->Fade(giFade_In);
			break;
		}
		case giCtrl_Show_Waveform:{
			//gpDSPEngine->Fade(giFade_Out);	
			break;
		}
	}		
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
}

void CTrack_Top::EventGeneric(ge::IControl* pControl, void* pEventData)
{

}


void CTrack_Top::HandleMsg(SMsg* pMsg)
{
	switch(pMsg->iOpcode) {
	
		case MsgGUIData: {
			SGUIData sData	= *(SGUIData*)pMsg->pDataIn;
			switch( sData.iID){
				case giParamID_Zoom:
				{
					miZoomLevel			= sData.iVal;
					Update_Zoom();
					break;
				}
				
				default: break;
			}
			break;
		}
		case Msg_Go_To_Mouse:{
			gpApplication->PlaybackGoToPos(muiSamplePos);
		
		
			break;
		}
		case Msg_Progress:{
			SProgressData sData	= *(SProgressData*)pMsg->pDataIn;
			Show_Progress(sData.bOn);
			Set_Progress(sData.fProgress);
			Set_ProgressText(sData.pszText);
		}
	}
}

void CTrack_Top::Update_Zoom()
{
	mfSec_Per_Pixel = 1.0f / (tfloat32)gpiPixels_Pr_Sec[miZoomLevel];
	
}

void CTrack_Top::Pixel_To_Sample(tint32 iPixel)
{
	//!!! Max Fix this
	tint64 iTest = (tint64)((tfloat64)gpApplication->GetSampleRate() * mfSec_Per_Pixel * iPixel);
	Update_Time_Display(iTest);

}

void CTrack_Top::Update_Time_Display(tint64 iSample_Point)
{
	//return;

	char psz[4];

	// Calc milli-secs and seconds
	tint64 iMilliSecsTotal = ((iSample_Point * 1000) / 44100);
	tint32 iMSec = (tint32)(iMilliSecsTotal % 1000);
	tint64 iSecsTotal = iMilliSecsTotal / 1000;
	tint32 iSec = (tint32)(iSecsTotal % 100);
	
	// Milli Sec
	sprintf(psz, "%03u", iMSec);
	mpTextMilliSecs->SetText(psz);

	// Sec
	if (miSecond != iSec) {
		miSecond = iSec;
		
		tint32 iScreenSec	=	(tint32)(iSecsTotal % 60);
		sprintf(psz, "%02u", iScreenSec);
		mpTextSecond->SetText(psz);

		// Min
		tint32 iMin = (tint32)((iSecsTotal / 60) % 60);
		if(miMin != iMin){
			miMin = iMin;
			sprintf(psz, "%02u", iMin);
			mpTextMinute->SetText(psz);

			// Hour
			tint32 iHour = (tint32)(iSecsTotal / (60*60));
			if(miHour != iHour){
				miHour	=	iHour;
				sprintf(psz, "%02u", iHour);
				mpTextHour->SetText(psz);
			}
		}
	}
	
	if(mbRewindPressed)
		gpApplication->PlaybackRewind();
	if(mbFFPressed)
		gpApplication->PlaybackFF();
		
}

void CTrack_Top::SetStartSamples(tint64 iSample_Point)
{
	char psz[32];
	sprintf(psz, "%ld", iSample_Point);
	mpText_Sample_Start->SetText(psz);
	
}

void CTrack_Top::SetEndSamples(tint64 iSample_Point)
{
	char psz[32];
	sprintf(psz, "%ld", iSample_Point);
	mpText_Sample_End->SetText(psz);
	
}

void CTrack_Top::SetCursorInSamples(tint64 iSample_Point)
{
	muiSamplePos = iSample_Point;
	char psz[32];
	sprintf(psz, "%ld", iSample_Point);
	mpText_Sample_Cursor->SetText(psz);
	
}

void CTrack_Top::Show_Progress(tbool bOn)
{
	mpProgress_Back->SetVisible(bOn);
	mpProgress_Text->SetVisible(bOn);
	mpProgress_Bar->SetVisible(bOn);

	if (!bOn) {
		// Redraw everything in pane - doesn't work! why!!?
		ge::SRect rect;
		mpPane->GetRect(rect);
		mpPane->Redraw(rect);
	}

} // Show_Progress

void CTrack_Top::Set_Progress(tfloat32 fProgress)
{
	const tint32 kiFullWidth = 270;
	tint32 iSize = Float2Int(fProgress * kiFullWidth); //2.56f;
	iSize %= kiFullWidth;
	mpProgress_Bar->SetSize(ge::SSize(iSize, 14));
} // Set_Progress


ge::IDropDownListBox* CTrack_Top::CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos)
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
															IDF_Font_Fuxley_712, 
															iBitmapID, 
															List, Pos, 
															ge::SPos(0,0),
															ge::SSize(48,12),
															ge::SRGB(0,255,0));
//	pListBox->DontSetText();
	return pListBox;
}

void CTrack_Top::PreparePopups()
{
	// Out Popup
	mpTime_Signature_Menu.iItemCount = 6;										
	mpTime_Signature_Menu.pItems[0] = ge::IPopupMenu::SMenuItem((const tchar*)"2/4",	giSignature2_4,	NULL);
	mpTime_Signature_Menu.pItems[1] = ge::IPopupMenu::SMenuItem((const tchar*)"3/4",	giSignature3_4,	NULL);
	mpTime_Signature_Menu.pItems[2] = ge::IPopupMenu::SMenuItem((const tchar*)"4/4",	giSignature4_4,	NULL);
	mpTime_Signature_Menu.pItems[3] = ge::IPopupMenu::SMenuItem((const tchar*)"6/8",	giSignature6_8,	NULL);
	mpTime_Signature_Menu.pItems[4] = ge::IPopupMenu::SMenuItem((const tchar*)"9/8",	giSignature9_8,	NULL);
	mpTime_Signature_Menu.pItems[5] = ge::IPopupMenu::SMenuItem((const tchar*)"12/8",	giSignature12_8,NULL);
	
	
	// Out Popup
	mpSnapTo_Menu.iItemCount = 6;										
	mpSnapTo_Menu.pItems[0] = ge::IPopupMenu::SMenuItem((const tchar*)"1/1",	32,	NULL);
	mpSnapTo_Menu.pItems[1] = ge::IPopupMenu::SMenuItem((const tchar*)"1/2",	16,	NULL);
	mpSnapTo_Menu.pItems[2] = ge::IPopupMenu::SMenuItem((const tchar*)"1/4",	8,	NULL);
	mpSnapTo_Menu.pItems[3] = ge::IPopupMenu::SMenuItem((const tchar*)"1/8",	4,	NULL);
	mpSnapTo_Menu.pItems[4] = ge::IPopupMenu::SMenuItem((const tchar*)"1/16",	2,	NULL);
	mpSnapTo_Menu.pItems[5] = ge::IPopupMenu::SMenuItem((const tchar*)"1/32",	1,	NULL);

		
}

void CTrack_Top::Set_ProgressText(const tchar* pszText /*= NULL*/)
{
	if (pszText == NULL)
		mpProgress_Text->SetText("Please wait");
	else
		mpProgress_Text->SetText(pszText);
} // Set_ProgressText
