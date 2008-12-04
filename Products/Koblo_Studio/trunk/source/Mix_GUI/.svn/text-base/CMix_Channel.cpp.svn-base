#include "KSOS.h"

CMix_Channel::CMix_Channel(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

}

CMix_Channel::~CMix_Channel()
{
}

void CMix_Channel::SetInfo(tint32 iID, tint32 iTimerID)
{
	miID = iID;
	miTimerID = iTimerID;
	miCtrl_Offset	= giFirst_Track_Ctrl + ( miID * giNr_Of_Track_Ctls);
	miSection		= iID + giSection_First_Track;

}

void CMix_Channel::Init()
{
	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(gMix_Channel);
//	mpPane->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteX );
	mpPane->SetBackgroundColour(ge::SRGB(200,200,200));
	
	// Top label
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Chanel_Info_Lable, ge::SPos(0, 0));
	
	// Buttom part with slider
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Mix_Volume, ge::SPos(0, 184));
	
	// Mouse-trap for activation of track 
	mpMouseTrap = ge::IMouseTrap::Create();
	// Only link into chain, but don't remove message (the third param = false)
	mpMouseTrap->EnableOneTrap(ge::LeftButtonDown, true, false);
	mpPane->AddControl(mpMouseTrap, ge::SPos(0, 0));
	mpMouseTrap->PlaceOnTopOf(GetPane());
	mpMouseTrap->AddListener(this);
	
	
	
	//--------------------------------------------
	// Text label
	ge::IText* pText = CreateDisplay(	giCtr_Mix_Channel_Text + miCtrl_Offset, 
												ge::SPos(8, 24),  
												ge::SSize(48, 12), 
												CreateFont(Generic128, IDB_Minix_Tight, ge::SRGB(204, 204, 204)),
												true);

	pText->SetHorzAlignment(ge::IText::HorzAlignLeft);
	pText->SetClicksOutsideAsSuccess();
		
	//--------------------------------------------
	// Track Aux/ Insert setup
	mpMix_AUX_Insert = new CMix_AUX_Insert(this, GetGUI());
	mpMix_AUX_Insert->SetInfo(miID, miTimerID);
	mpMix_AUX_Insert->Init();
	mpPane->AddControl(mpMix_AUX_Insert->GetPane(), ge::SPos(0, 54));
	
	//--------------------------------------------
	// Bitmap for selection
	mpSelected_Back_Drop = CreateBitmap(ge::IControl::giNoID, IDB_Back_Mix_Selected, ge::SPos(0, 0));
	mpSelected_Back_Drop->SetVisible(false);
	
	// Show insert button
	Create2StateButton(giChannel_Show_Insert_Button	+ miCtrl_Offset,	IDB_Button_AUX_Insert,	ge::SPos(36, 0), true);
	
	PreparePopups();
	CreatePop(giChannel_Output_Popup+ miCtrl_Offset, IDB_Invisible_Button_16_64, mpOutput_Menu, ge::SPos(3, 255));

	// Volume slider & display
	CreateSlider(giChannel_Volume_Slider				+ miCtrl_Offset, ge::SPos(24 , 21+184),ge::SSize(16,110), IDB_Slider_Handle_Volume);
	CreateDisplay( giChannel_Volume_Slider_Display		+ miCtrl_Offset, ge::SPos(4, 350),  ge::SSize(56, 12), CreateFont(Generic128, IDB_AxelTight, ge::SRGB(0, 255, 0)));


	Create2StateButton(giChannel_Arm_Button + miCtrl_Offset, IDB_Button_Arm_Vertical, ge::SPos(3, 102+184), true);
	Create2StateButton(giChannel_Mute_Button + miCtrl_Offset, IDB_Button_Mute_Vertical, ge::SPos(3,102+16+184), true);
	Create2StateButton(giChannel_Solo_Button + miCtrl_Offset, IDB_Button_Solo_Vertical, ge::SPos(3,102+16+15+184), true); 
	
	// Output meter, Left
	mpMeterOutL = CreatePeakMeter(ge::IControl::giNoID, ge::SPos(46, 215), 120, ge::IPeakMeter::Up);
	FindPane(mpMeterOutL)->CreateBitmap(ge::IControl::giNoID, IDB_Meter_Off_Left, ge::SPos(0, 0));
	// Output meter, Right
	mpMeterOutR = CreatePeakMeter(ge::IControl::giNoID, ge::SPos(54, 215), 120, ge::IPeakMeter::Up);
	FindPane(mpMeterOutR)->CreateBitmap(ge::IControl::giNoID, IDB_Meter_Off_Right, ge::SPos(0, 0));
	
	
	// Panner 
	ge::IPanner* pPanner = CreatePanner(
		giChannel_Pan  + miCtrl_Offset, ge::IControl::giNoID,
		NULL, IDB_Button_Pan_Handle,
		ge::SPos(3, 188), 
		ge::SPos(0, 0),
		ge::SSize(59, 0),
		NULL
	);
//	pPanner->SetDirectMouseXlat(true);
//	pPanner->SetJumpToMouseClickPos(true);
	
}


void CMix_Channel::ConnectControls()
{
	// Show / Hide Inserts
	RegisterGlobalControl(miSection, giChannel_Show_Insert_Button	+ miCtrl_Offset, giParam_Track_Info_Show_Inserts);
	// Tracck Size
	// Solo Mute
	RegisterGlobalControl(miSection, giChannel_Solo_Button + miCtrl_Offset, giParam_ChSolo);
	RegisterGlobalControl(miSection, giChannel_Mute_Button + miCtrl_Offset, giParam_ChMute);
	RegisterGlobalControl(miSection, giChannel_Arm_Button + miCtrl_Offset, giParam_ChArm);

	// Volume slider
	RegisterGlobalControlWithX3Translation(miSection, giChannel_Volume_Slider + miCtrl_Offset, giParam_ChVol, 4);
	// Display
	mpParmMan->RegisterControl(	miPaneID, 
								giChannel_Volume_Slider_Display + miCtrl_Offset, 
								giParam_ChVol, 
								de::IParameterManager::TypeGlobal, 
								miSection);
								
	de::IDisplayConverter* pConverter  = new CX3DisplayConverter();
	ge::IControl* pControl = GetControl(giChannel_Volume_Slider_Display + miCtrl_Offset);
	pControl->SetDisplayConverter(pConverter);
	
	pControl->SetDefaultValueConverted(mpParmMan->Get(	giParam_ChVol, 
														de::IParameterManager::TypeGlobal,
														miSection)->GetDefaultValue());
														
	RegisterGlobalControl(miSection, giChannel_Output_Popup + miCtrl_Offset , giParam_ChOut);
	
	mpMix_AUX_Insert->ConnectControls();
	
	
	RegisterGlobalControlWithLinearTranslation(	miSection, giChannel_Pan + miCtrl_Offset,giParam_ChPannerLeftRight);

}

void CMix_Channel::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
/*	if( pControl->GetID() == giChannel_Show_Insert_Button	+ miCtrl_Offset) {
		// Send expand channel message 
		STrackData sData;
		sData.iParam_ID		=	giParam_Track_Info_Show_Inserts;
		sData.iVal			=	iValueNew;
		sData.iTrack		=	miID;
		SMsg Msg			=	Msg_Track_Data;
		Msg.pDataIn			=	(void*)&sData;
		SendMsgFromTop(&Msg);
	}
*/	
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
}

void CMix_Channel::EventGeneric(ge::IControl* pControl, void* pEventData)
{
	tint32 iID = pControl->GetID() - miCtrl_Offset;
	
	switch(iID) {
		case giCtr_Mix_Channel_Text:
			std::string sText = std::string((const tchar*)pEventData);
			dynamic_cast<CKSPlugIn*>(mpGUI->GetPlugIn())->SetChannelName(miID, sText);
			break;
	}
	
	
	ge::SEventGeneric* pEvent = (ge::SEventGeneric*)pEventData;
	if (pEvent->eType = ge::giEventMouse) {
		// Event happened inside control
		if (pEvent->bInsideControlRect) {
			
			if (pEvent->iMsg == ge::LeftButtonDown) {
					dynamic_cast<CKSPlugIn*>(GetPlugIn())->SelectTrack(miID);

			}
		}
	}
	
}


void CMix_Channel::HandleMsg(SMsg* pMsg)
{

}

void CMix_Channel::SelectTrack( tbool bSelected)
{
	mpSelected_Back_Drop->SetVisible(bSelected);
}


void CMix_Channel::PreparePopups()
{
	
	// Out Popup
	mpOutput_Menu.iItemCount = giNumber_Of_Busses+1 ;	
	
	char psz[32];
	for(tint32 i=0; i< giNumber_Of_Busses; i++){
	
		sprintf(psz, "B%d", i+1);
		mpOutput_Menu.pItems[i] = ge::IPopupMenu::SMenuItem(psz, ParmIOOffsetBus + i ,	NULL);
	}
	
	
	mpOutput_Menu.pItems[giNumber_Of_Busses]	= ge::IPopupMenu::SMenuItem((const tchar*)"M",		ParmIOOffsetMix,			NULL);


		
}

ge::IDropDownListBox* CMix_Channel::CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos)
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
															ge::SSize(18,12),
															ge::SRGB(0,255,0));
//	pListBox->DontSetText();
	return pListBox;
}


void CMix_Channel::UpdateMeters_Maybe(void* pvoid_sMeters_All)
{
	if (mpPane->IsVisible()) {
		if (pvoid_sMeters_All == NULL) {
			mpMeterOutL->SetFloatValueWithDecay(1.0, 0.0);
			mpMeterOutR->SetFloatValueWithDecay(1.0, 0.0);
		}
		else {
			CKSDSP::SMeters_All* psMeters_All = (CKSDSP::SMeters_All*)pvoid_sMeters_All;
			tfloat32* pMetersForThisChannel = psMeters_All->ppfMeters_Tracks[miID];
			mpMeterOutL->SetFloatValueWithDecay(pMetersForThisChannel[0], psMeters_All->fDecayFactor);
			mpMeterOutR->SetFloatValueWithDecay(pMetersForThisChannel[1], psMeters_All->fDecayFactor);
		}
	}
} // UpdateMeters_Maybe


void CMix_Channel::UpdateChannelName(const std::string& sName)
{
	ge::IControl* pControl = mpPane->GetControl(giCtr_Mix_Channel_Text + miCtrl_Offset);
	ge::IText* pText = dynamic_cast<ge::IText*>(pControl);

	pText->SetText(sName.c_str());
}