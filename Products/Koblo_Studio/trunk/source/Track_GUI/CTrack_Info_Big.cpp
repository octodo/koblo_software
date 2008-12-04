#include "KSOS.h"

CTrack_Info_Big::CTrack_Info_Big(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

}

CTrack_Info_Big::~CTrack_Info_Big()
{
	
}

void CTrack_Info_Big::SetInfo(tint32 iID, tint32 iTimerID)
{
	miID = iID;
	miTimerID = iTimerID;
	miCtrl_Offset	= giFirst_Track_Ctrl + ( miID * giNr_Of_Track_Ctls);
	miSection		= iID + giSection_First_Track;
}

void CTrack_Info_Big::Init()
{
	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(gTrack_Info_Big);
	
	//--------------------------------------------
	// Track Aux/ Insert setup
	mpTrack_AUX_Insert = new CTrack_AUX_Insert(this, GetGUI());
	mpTrack_AUX_Insert->SetInfo(miID, miTimerID);
	mpTrack_AUX_Insert->Init();
	mpPane->AddControl(mpTrack_AUX_Insert->GetPane(), ge::SPos(0, 54));

	
	mpBack_Drop				= CreateBitmap(ge::IControl::giNoID, IDB_Back_Track_Info_Lable, ge::SPos(0, 0));
	mpBack_Drop				= CreateBitmap(ge::IControl::giNoID, IDB_Back_Track_Info_Big, ge::SPos(64, 0));
	mpSelected_Back_Drop	= CreateBitmap(ge::IControl::giNoID, IDB_Back_Track_Info_Big_Selected, ge::SPos(0, 0));
	
	mpSelected_Back_Drop->SetVisible(false);

	// Expand button
	ge::IMultiStateButton*  pButton = Create2StateButton(giTrack_Info_Big_Colaps_Button		+ miCtrl_Offset,	IDB_Track_Button_Expand, ge::SPos(2, 4), true);
	pButton->SetEventMouseUp();

	// Show insert button
	Create2StateButton(giTrack_Info_Big_Show_Insert_Button	+ miCtrl_Offset,	IDB_Button_AUX_Insert,	ge::SPos(36, 0), true);

	PreparePopups();
	CreatePop(giTrack_Info_Big_Output_Popup+ miCtrl_Offset, IDB_Invisible_Button_16_64, mpOutput_Menu, ge::SPos(3+64, 71));
	
	// Volume slider & display
	CreateSlider(giTrack_Info_Big_Volume_Slider				+ miCtrl_Offset, ge::SPos(88, 21),ge::SSize(16,110), IDB_Slider_Handle_Volume);
	CreateDisplay( giTrack_Info_Big_Volume_Slider_Display	+ miCtrl_Offset, ge::SPos(68, 350-184),  ge::SSize(56, 12), CreateFont(Generic128, IDB_AxelTight, ge::SRGB(0, 255, 0)));
	
	Create2StateButton(giTrack_Info_Big_Arm_Button + miCtrl_Offset, IDB_Button_Arm_Vertical, ge::SPos(67, 102), true);
	Create2StateButton(giTrack_Info_Big_Mute_Button + miCtrl_Offset, IDB_Button_Mute_Vertical, ge::SPos(67,102+16), true);
	Create2StateButton(giTrack_Info_Big_Solo_Button + miCtrl_Offset, IDB_Button_Solo_Vertical, ge::SPos(67,102+16+15), true); 
	
	//--------------------------------------------
	// Text label
	ge::IText* pText = CreateDisplay(	giTrack_Info_Big_Channel_Text+ miCtrl_Offset, 
												ge::SPos(8, 24),  
												ge::SSize(48, 12), 
												CreateFont(Generic128, IDB_Minix_Tight, ge::SRGB(204, 204, 204)),
												true);

	pText->SetHorzAlignment(ge::IText::HorzAlignLeft);
	pText->SetClicksOutsideAsSuccess();
	
	
	// Output meter, Left
	mpMeterOutL = CreatePeakMeter(ge::IControl::giNoID, ge::SPos(46+64, 31), 120, ge::IPeakMeter::Up);
	FindPane(mpMeterOutL)->CreateBitmap(ge::IControl::giNoID, IDB_Meter_Off_Left, ge::SPos(0, 0));
	// Output meter, Right
	mpMeterOutR = CreatePeakMeter(ge::IControl::giNoID, ge::SPos(54+64, 31), 120, ge::IPeakMeter::Up);
	FindPane(mpMeterOutR)->CreateBitmap(ge::IControl::giNoID, IDB_Meter_Off_Right, ge::SPos(0, 0));
	
	// Panner 
	ge::IPanner* pPanner =  CreatePanner(
		giTrack_Info_Big_Pan  + miCtrl_Offset, ge::IControl::giNoID,
		NULL, IDB_Button_Pan_Handle,
		ge::SPos(3+64, 4), 
		ge::SPos(0, 0),
		ge::SSize(59, 0),
		NULL
	);
	
//	pPanner->SetDirectMouseXlat();
	
	// Mouse-trap for activation of track - must be last to have effect
	mpMouseTrap = ge::IMouseTrap::Create();
	mpMouseTrap->EnableOneTrap(ge::MouseDrag, true, true);
	mpMouseTrap->EnableOneTrap(ge::MouseDrop, true, true);
	mpPane->AddControl(mpMouseTrap, ge::SPos(0, 0));
	mpMouseTrap->CloneRect(GetPane());
	mpMouseTrap->AddListener(this);
	

}


void CTrack_Info_Big::ConnectControls()
{
	// Show / Hide Inserts
	RegisterGlobalControl(miSection, giTrack_Info_Big_Show_Insert_Button + miCtrl_Offset, giParam_Track_Info_Show_Inserts);
	// Tracck Size
	RegisterGlobalControl(miSection, giTrack_Info_Big_Colaps_Button + miCtrl_Offset, giParam_Track_Info_SizeY);
	// Solo Mute
	RegisterGlobalControl(miSection, giTrack_Info_Big_Solo_Button + miCtrl_Offset, giParam_ChSolo);
	RegisterGlobalControl(miSection, giTrack_Info_Big_Mute_Button + miCtrl_Offset, giParam_ChMute);
	RegisterGlobalControl(miSection, giTrack_Info_Big_Arm_Button + miCtrl_Offset, giParam_ChArm);
	
	// Volume slider
	RegisterGlobalControlWithX3Translation(miSection, giTrack_Info_Big_Volume_Slider + miCtrl_Offset, giParam_ChVol, 4);
	// Display
	mpParmMan->RegisterControl(	miPaneID, 
								giTrack_Info_Big_Volume_Slider_Display + miCtrl_Offset, 
								giParam_ChVol, 
								de::IParameterManager::TypeGlobal, 
								miSection);
	de::IDisplayConverter* pConverter  = new CX3DisplayConverter();
	ge::IControl* pControl = GetControl(giTrack_Info_Big_Volume_Slider_Display + miCtrl_Offset);
	pControl->SetDisplayConverter(pConverter);
	
	pControl->SetDefaultValueConverted(mpParmMan->Get(	giParam_ChVol, 
														de::IParameterManager::TypeGlobal,
														miSection)->GetDefaultValue());
														
	RegisterGlobalControl(miSection, giTrack_Info_Big_Output_Popup + miCtrl_Offset , giParam_ChOut);
	
	RegisterGlobalControlWithLinearTranslation(	miSection, giTrack_Info_Big_Pan + miCtrl_Offset,giParam_ChPannerLeftRight);
	
	
	mpTrack_AUX_Insert->ConnectControls();

}

void CTrack_Info_Big::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{

	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
}

void CTrack_Info_Big::EventGeneric(ge::IControl* pControl, void* pEventData)
{
	tint32 iID = pControl->GetID() - miCtrl_Offset;
	
	switch(iID) {
		case giTrack_Info_Big_Channel_Text:{
			std::string sText = std::string((const tchar*)pEventData);
			dynamic_cast<CKSPlugIn*>(mpGUI->GetPlugIn())->SetChannelName(miID, sText);
			return;
		}
		
	}
	
	ge::SEventGeneric* pEvent = (ge::SEventGeneric*)pEventData;
	if (pEvent->eType = ge::giEventMouse) {
	
		switch(pEvent->iMsg){
		
			case ge::MouseDrop: {
				SendMsg(Msg_Track_MouseDrop);
				break;
			}
			case ge::MouseDrag: {
				SendMsg(Msg_Track_MouseDrag);
				break;
			}
		}
	}
}

void CTrack_Info_Big::SendMsg(tint32 iMsg)
{
	SMsg Msg			=	iMsg;
	Msg.pDataIn			=	(void*)&miID;
	SendMsgFromTop(&Msg);
}

void CTrack_Info_Big::HandleMsg(SMsg* pMsg)
{

}

void CTrack_Info_Big::SelectTrack( tbool bSelected)
{

	mpSelected_Back_Drop->SetVisible(bSelected);

}


void CTrack_Info_Big::PreparePopups()
{


	//--------------------------------
	// Out popup
	//--------------------------------
	mpOutput_Menu.iItemCount = giNumber_Of_Busses+1 ;	
	
	char psz[32];
	for(tint32 i=0; i< giNumber_Of_Busses; i++){
	
		sprintf(psz, "B%d", i+1);
		mpOutput_Menu.pItems[i] = ge::IPopupMenu::SMenuItem(psz, ParmIOOffsetBus + i ,	NULL);
	}
	mpOutput_Menu.pItems[giNumber_Of_Busses]	= ge::IPopupMenu::SMenuItem((const tchar*)"M",		ParmIOOffsetMix,			NULL);


		
}

ge::IDropDownListBox* CTrack_Info_Big::CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos)
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

void CTrack_Info_Big::UpdateTrackMeters_Maybe(void* pvoid_sMeters_All)
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
} // UpdateTrackMeters_Maybe


void CTrack_Info_Big::UpdateChannelName(const std::string& sName)
{	
	ge::IControl* pControl = mpPane->GetControl(giTrack_Info_Big_Channel_Text + miCtrl_Offset);
	ge::IText* pText = dynamic_cast<ge::IText*>(pControl);
	pText->SetText(sName.c_str());
}