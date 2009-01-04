#include "KSOS.h"

CMix_Master::CMix_Master(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

}

CMix_Master::~CMix_Master()
{
}

void CMix_Master::SetInfo(const ge::IPopupMenu::SMenuItemList& Mix_Out_Menu)
{

	mpMix_Out_Menu = Mix_Out_Menu;


}

void CMix_Master::Init()
{
	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(gMix_Master);
//	mpPane->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteX );
	mpPane->SetBackgroundColour(ge::SRGB(200,200,200));
	
	// Bitmap
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Mix_Master_Lable, ge::SPos(0, 0));
	
	//--------------------------------------------
	// Text label
	ge::IText* pText = CreateDisplay(	giCtr_Mix_Master_Text, 
												ge::SPos(8, 24),  
												ge::SSize(48, 12), 
												CreateFont(Generic128, IDB_Font_Minix_Tight, ge::SRGB(204, 204, 204)));

	pText->SetHorzAlignment(ge::IText::HorzAlignLeft);
	pText->SetClicksOutsideAsSuccess();
	
	char psz[32];
	sprintf(psz, "Master");
	pText->SetText(psz);
	
	//--------------------------------------------
	// Track Aux/ Insert setup
	mpMix_Master_AUX_Insert = new CMix_Master_AUX_Insert(this, GetGUI());
	mpMix_Master_AUX_Insert->SetInfo(giCtr_Mix_Master_ID, giTimerID_MixMaster_ScrollPane);
	mpMix_Master_AUX_Insert->Init();
	mpPane->AddControl(mpMix_Master_AUX_Insert->GetPane(), ge::SPos(0, 54));
	
	// Buttom part with slider
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Mix_Master_Volume, ge::SPos(0, 184));
	
//	mpSelected_Back_Drop	= CreateBitmap(ge::IControl::giNoID, IDB_Back_Track_Info_Big_Selected, ge::SPos(0, 0));
//	mpSelected_Back_Drop->SetVisible(false);

	//--------------------------------------------
	// Show insert button
	Create2StateButton(giCtr_Mix_Master_Show_Insert_Button,	IDB_Button_AUX_Insert,	ge::SPos(36, 0), true);
	
	// Output selection popup
	//CreatePop(giCtrl_Mix_Master_Out_Popup, IDB_Button_Master_Popup, mpMix_Out_Menu, ge::SPos(3, 254));
	CreatePop(giCtrl_Mix_Master_Out_Popup, IDB_Button_Master_Popup, mpMix_Out_Menu, ge::SPos(3, 3));
																		
	// Volume slider & display
	CreateSlider(giCtr_Mix_Master_Volume_Slider, ge::SPos(88-64 , 21+184),ge::SSize(16,110), IDB_Slider_Handle_Volume);
	CreateDisplay( giCtr_Mix_Master_Volume_Slider_Display, ge::SPos(4, 350),  ge::SSize(56, 12), CreateFont(Generic128, IDB_Font_Axel_Tight, ge::SRGB(0, 255, 0)));
	
	// Output meter, Left
	mpMeterOutL = CreatePeakMeter(ge::IControl::giNoID, ge::SPos(46, 215), 120, ge::IPeakMeter::Up);
	FindPane(mpMeterOutL)->CreateBitmap(ge::IControl::giNoID, IDB_Meter_Off_Left, ge::SPos(0, 0));
	// Output meter, Right
	mpMeterOutR = CreatePeakMeter(ge::IControl::giNoID, ge::SPos(54, 215), 120, ge::IPeakMeter::Up);
	FindPane(mpMeterOutR)->CreateBitmap(ge::IControl::giNoID, IDB_Meter_Off_Right, ge::SPos(0, 0));
	
	mpPan = CreatePanner(giCtrl_Lopp_Start, ge::IControl::giNoID, NULL, IDB_Button_Pan_Handle, ge::SPos(3, 188), ge::SPos(0, 0), ge::SSize(59, 0), NULL);
	mpPan->SetDirectMouseXlat();
	
}


void CMix_Master::ConnectControls()
{
	// Volume slider
	RegisterGlobalControlWithX3Translation(giSection_Master, giCtr_Mix_Master_Volume_Slider, giParam_Master_Volume, 4);
	// Display
	mpParmMan->RegisterControl(	miPaneID, 
								giCtr_Mix_Master_Volume_Slider_Display, 
								giParam_Master_Volume, 
								de::IParameterManager::TypeGlobal, 
								giSection_Master);
								
	de::IDisplayConverter* pConverter  = new CX3DisplayConverter();
	ge::IControl* pControl = GetControl(giCtr_Mix_Master_Volume_Slider_Display);
	pControl->SetDisplayConverter(pConverter);
	
	pControl->SetDefaultValueConverted(mpParmMan->Get(	giParam_Master_Volume, 
														de::IParameterManager::TypeGlobal,
														giSection_Master)->GetDefaultValue());
														
//	RegisterGlobalControl(giSection_Master, giCtrl_Mix_Master_Out_Popup, giParam_Master_Out);

	mpMix_Master_AUX_Insert->ConnectControls();
}

void CMix_Master::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{

	if( pControl->GetID() == giCtr_Mix_Master_Show_Insert_Button) {
		// Send expand channel message 
		STrackData sData;
		sData.iParam_ID		=	giParam_Master_Show_Inserts;
		sData.iVal			=	iValueNew;
//		sData.iTrack		=	miID;
		SMsg Msg			=	Msg_Track_Data;
		Msg.pDataIn			=	(void*)&sData;
		SendMsgFromTop(&Msg);
	}
	
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
}

void CMix_Master::EventGeneric(ge::IControl* pControl, void* pEventData)
{
}


void CMix_Master::HandleMsg(SMsg* pMsg)
{

}


ge::IDropDownListBox* CMix_Master::CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos)
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
															List, 
															Pos,
															ge::SPos(0,0),
															ge::SSize(12,12));
//	pListBox->DontSetText();
	return pListBox;
}


void CMix_Master::UpdateMeters(void* pvoid_sMeters_All)
{
	//if (mpPane->IsVisible()) {
	if (true) { 
		if (pvoid_sMeters_All == NULL) {
			mpMeterOutL->SetFloatValueWithDecay(1.0, 0.0);
			mpMeterOutR->SetFloatValueWithDecay(1.0, 0.0);
		}
		else {
			CKSDSP::SMeters_All* psMeters_All = (CKSDSP::SMeters_All*)pvoid_sMeters_All;
			tfloat32* pMetersForThisChannel = psMeters_All->pfMeters_Master;
			mpMeterOutL->SetFloatValueWithDecay(pMetersForThisChannel[0], psMeters_All->fDecayFactor);
			mpMeterOutR->SetFloatValueWithDecay(pMetersForThisChannel[1], psMeters_All->fDecayFactor);
		}
	}
} // UpdateMeters


