#include "KSOS.h"

CMix_Buss::CMix_Buss(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

}

CMix_Buss::~CMix_Buss()
{
}

void CMix_Buss::SetInfo(tint32 iID, tint32 iTimerID)
{
	miID = iID;
	miTimerID = iTimerID;
	miCtrl_Offset	= giFirst_Buss_Ctrl + ( miID * giNr_Of_Buss_Ctls);
	miSection		= iID + giSection_First_Buss;

}

void CMix_Buss::Init()
{
	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(gMix_Bus);
	mpPane->SetBackgroundColour(ge::SRGB(200,200,200));
	
	// Top label
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Mix_Buss_Lable, ge::SPos(0, 0));
	
	//--------------------------------------------
	// Text label
	ge::IText* pText = CreateDisplay(	giCtr_Mix_Buss_Text+ miCtrl_Offset, 
												ge::SPos(8, 24),  
												ge::SSize(48, 12), 
												CreateFont(Generic128, IDB_Font_Minix_Tight, ge::SRGB(204, 204, 204)), true);

	pText->SetHorzAlignment(ge::IText::HorzAlignLeft);
	pText->SetClicksOutsideAsSuccess();
	
	char psz[32];
	sprintf(psz, "Bus %d", miID+1);
	pText->SetText(psz);
	
	
	//--------------------------------------------
	// Track Aux/ Insert setup
	mpMix_Buss_AUX_Insert = new CMix_Buss_AUX_Insert(this, GetGUI());
	mpMix_Buss_AUX_Insert->SetInfo(miID, miTimerID);
	mpMix_Buss_AUX_Insert->Init();
	mpPane->AddControl(mpMix_Buss_AUX_Insert->GetPane(), ge::SPos(0, 54));
	
	// Byttom part with slider
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Mix_Buss_Volume, ge::SPos(0, 184));
	
//	mpSelected_Back_Drop	= CreateBitmap(ge::IControl::giNoID, IDB_Back_Track_Info_Big_Selected, ge::SPos(0, 0));
//	mpSelected_Back_Drop->SetVisible(false);


	// Show insert button
	Create2StateButton(giCtr_Mix_Buss_Show_Insert_Button	+ miCtrl_Offset,	IDB_Button_AUX_Insert,	ge::SPos(36, 0), true);

	PreparePopups();
	CreatePop(giCtr_Mix_Buss_Output_Popup+ miCtrl_Offset, IDB_Button_Invisible_16_64, mpOutput_Menu, ge::SPos(3, 255));
	
	// Volume slider & display
	CreateSlider(giCtr_Mix_Buss_Volume_Slider				+ miCtrl_Offset, ge::SPos(88-64 , 21+184),ge::SSize(16,110), IDB_Slider_Handle_Volume);
	CreateDisplay( giCtr_Mix_Buss_Volume_Slider_Display		+ miCtrl_Offset, ge::SPos(4, 350),  ge::SSize(56, 12), CreateFont(Generic128, IDB_Font_Axel_Tight, ge::SRGB(0, 255, 0)));
	
	
	Create2StateButton(giCtr_Mix_Buss_Arm_Button			+ miCtrl_Offset, IDB_Button_Arm_Vertical, ge::SPos(3, 102+184), true);
	Create2StateButton(giCtr_Mix_Buss_Mute_Button			+ miCtrl_Offset, IDB_Button_Mute_Vertical, ge::SPos(3,102+16+184), true);
	Create2StateButton(giCtr_Mix_Buss_Solo_Button			+ miCtrl_Offset, IDB_Button_Solo_Vertical, ge::SPos(3,102+16+15+184), true); 
	
	// Output meter, Left
	mpMeterOutL = CreatePeakMeter(ge::IControl::giNoID, ge::SPos(46, 215), 120, ge::IPeakMeter::Up);
	FindPane(mpMeterOutL)->CreateBitmap(ge::IControl::giNoID, IDB_Meter_Off_Left, ge::SPos(0, 0));
	// Output meter, Right
	mpMeterOutR = CreatePeakMeter(ge::IControl::giNoID, ge::SPos(54, 215), 120, ge::IPeakMeter::Up);
	FindPane(mpMeterOutR)->CreateBitmap(ge::IControl::giNoID, IDB_Meter_Off_Right, ge::SPos(0, 0));

	//mpPan = CreatePanner(giCtrl_Lopp_Start, ge::IControl::giNoID, NULL, IDB_Button_Pan_Handle, ge::SPos(3, 188), ge::SPos(0, 0), ge::SSize(59, 0), NULL);
	//mpPan->SetDirectMouseXlat();
	
	// Panner 
	CreatePanner(
		giCtr_Mix_Buss_Pan  + miCtrl_Offset, ge::IControl::giNoID,
		NULL, IDB_Button_Pan_Handle,
		ge::SPos(3, 188), 
		ge::SPos(0, 0),
		ge::SSize(59, 0),
		NULL
	);
//	pPanner->SetDirectMouseXlat(true);
//	pPanner->SetJumpToMouseClickPos(true);
	
}


void CMix_Buss::ConnectControls()
{

	// Show / Hide Inserts
	RegisterGlobalControl(miSection, giCtr_Mix_Buss_Show_Insert_Button	+ miCtrl_Offset, giParam_Buss_Info_Show_Inserts);

	// Solo Mute
	RegisterGlobalControl(miSection, giCtr_Mix_Buss_Solo_Button + miCtrl_Offset, giParam_Buss_Solo);
	RegisterGlobalControl(miSection, giCtr_Mix_Buss_Mute_Button + miCtrl_Offset, giParam_Buss_Mute);
	RegisterGlobalControl(miSection, giCtr_Mix_Buss_Arm_Button + miCtrl_Offset, giParam_Buss_Arm);
	
	
	
	
	// Volume slider
	RegisterGlobalControlWithX3Translation(miSection, giCtr_Mix_Buss_Volume_Slider + miCtrl_Offset, giParam_Buss_Vol, 4);
	// Display
	mpParmMan->RegisterControl(	miPaneID, 
								giCtr_Mix_Buss_Volume_Slider_Display + miCtrl_Offset, 
								giParam_Buss_Vol, 
								de::IParameterManager::TypeGlobal, 
								miSection);
								
	de::IDisplayConverter* pConverter  = new CX3DisplayConverter();
	ge::IControl* pControl = GetControl(giCtr_Mix_Buss_Volume_Slider_Display + miCtrl_Offset);
	pControl->SetDisplayConverter(pConverter);
	
	pControl->SetDefaultValueConverted(mpParmMan->Get(	giParam_Buss_Vol, 
														de::IParameterManager::TypeGlobal,
														miSection)->GetDefaultValue());
														
	RegisterGlobalControl(miSection, giCtr_Mix_Buss_Output_Popup + miCtrl_Offset , giParam_Buss_Out);
	
	
	RegisterGlobalControlWithLinearTranslation(	miSection, giCtr_Mix_Buss_Pan + miCtrl_Offset,giParam_Buss_PannerLeftRight);
	//giCtr_Mix_Buss_Pan

	mpMix_Buss_AUX_Insert->ConnectControls();
}

void CMix_Buss::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	if( pControl->GetID() == giCtr_Mix_Buss_Show_Insert_Button	+ miCtrl_Offset) {
		// Send expand channel message 
		SBussData sData;
		sData.iParam_ID		=	giParam_Buss_Info_Show_Inserts;
		sData.iVal			=	iValueNew;
		sData.iBuss			=	miID;
		SMsg Msg			=	Msg_Buss_Data;
		Msg.pDataIn			=	(void*)&sData;
		SendMsgFromTop(&Msg);
	}
	
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
}

void CMix_Buss::EventGeneric(ge::IControl* pControl, void* pEventData)
{
	/*
	tint32 iID = pControl->GetID() - miCtrl_Offset;
	
	switch(iID) {
		case giCtr_Mix_Buss_Text:
			std::string sText = std::string((const tchar*)pEventData);
			dynamic_cast<CKSPlugIn*>(mpGUI->GetPlugIn())->SetChannelName(miID + giNumber_Of_Tracks, sText);
			break;
	}
	 */
}


void CMix_Buss::HandleMsg(SMsg* pMsg)
{

}


void CMix_Buss::UpdateMeters(void* pvoid_sMeters_All)
{
	//if (mpPane->IsVisible()) {
	if (true) { 
		if (pvoid_sMeters_All == NULL) {
			mpMeterOutL->SetFloatValueWithDecay(1.0, 0.0);
			mpMeterOutR->SetFloatValueWithDecay(1.0, 0.0);
		}
		else {
			CDSP::SMeters_All* psMeters_All = (CDSP::SMeters_All*)pvoid_sMeters_All;
			tfloat32* pMetersForThisChannel = psMeters_All->ppfMeters_Busses[miID];
			mpMeterOutL->SetFloatValueWithDecay(pMetersForThisChannel[0], psMeters_All->fDecayFactor);
			mpMeterOutR->SetFloatValueWithDecay(pMetersForThisChannel[1], psMeters_All->fDecayFactor);
		}
	}
} // UpdateMeters


void CMix_Buss::PreparePopups()
{
	
	// Out Popup
	mpOutput_Menu.iItemCount = giNumber_Of_Busses - miID ;	
	tint32 iItem = 0;									

	if(miID < 1){
		mpOutput_Menu.pItems[iItem] = ge::IPopupMenu::SMenuItem((const tchar*)"B1",	ParmIOOffsetBus + iItem ,	NULL);
		iItem++;
	}
	if(miID < 2){
		mpOutput_Menu.pItems[iItem] = ge::IPopupMenu::SMenuItem((const tchar*)"B2",	ParmIOOffsetBus + iItem,	NULL);
		iItem++;
	}
	if(miID < 3){
		mpOutput_Menu.pItems[iItem] = ge::IPopupMenu::SMenuItem((const tchar*)"B3",	ParmIOOffsetBus + iItem,	NULL);
		iItem++;
	}
	
	mpOutput_Menu.pItems[iItem]	= ge::IPopupMenu::SMenuItem((const tchar*)"M",		ParmIOOffsetMix,			NULL);


		
}

ge::IDropDownListBox* CMix_Buss::CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos)
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
															ge::SSize(18,12),
															ge::SRGB(0,255,0));
//	pListBox->DontSetText();
	return pListBox;
}