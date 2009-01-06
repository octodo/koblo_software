
#include "KSOS.h"


CKSExportGUIPane::CKSExportGUIPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{


	mpDropDownAudioCodecs = NULL;
	mpDropDownQualities = NULL;
	meAudioCodec = ac::geAudioCodecUndefined;
	miAudioCodecQuality = -1;
	miTailMS = 200;//1000;
	miChannels = 0;
	mbNormalize = true;

	mbIsVisible = false;
}

CKSExportGUIPane::~CKSExportGUIPane()
{
	
}

void CKSExportGUIPane::Init()
{
	
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(386,242));
	
	//-------------------------------
	// Backdrop for Export
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Export_Audio, ge::SPos(0, 0));
	
	
//	Create2StateButton(giCtrl_Delay_Power, IDB_Button_Power, ge::SPos(25, 36), true);

	ge::SPos pos(24,36);

	ge::IRadioButtons* pSetup_Radio_Button = CreateRadioButtonsGroup(giCtrl_Export_Selection,ge::SPos(0,0) );
	// Account button			
	CreateRadioButton(ge::IControl::giNoID, IDB_Button_Radio, pos, pSetup_Radio_Button, true); pos.iY += 22;
	CreateRadioButton(ge::IControl::giNoID, IDB_Button_Radio, pos, pSetup_Radio_Button, true); pos.iY += 22;
	CreateRadioButton(ge::IControl::giNoID, IDB_Button_Radio, pos, pSetup_Radio_Button, true); pos.iY += 22;
	CreateRadioButton(ge::IControl::giNoID, IDB_Button_Radio, pos, pSetup_Radio_Button, true); pos.iY += 22;
	CreateRadioButton(ge::IControl::giNoID, IDB_Button_Radio, pos, pSetup_Radio_Button, true); pos.iY += 22;
	
	Prepare_Popups();
	
	// Normalize
	ge::IMultiStateButton* pBtnNorm = Create2StateButton(giCtrl_Normalize_On_Export, IDB_Button_Normalize, pos, false);
	
	// Type
	mpDropDownAudioCodecs = CreatePop( giCtrl_Wave_Type_Popup, IDB_Button_Popup,  mpType_Popup, ge::SPos(192, 45), mpPane);
	mpDropDownAudioCodecs->SetValue(0);
	
	// Compression quality
	mpDropDownQualities = CreatePop( giCtrl_Compression_Popup, IDB_Button_Popup,  mpQuality_Popup, ge::SPos(192, 87), mpPane);
	mpDropDownQualities->SetValue(miAudioCodecQuality);
	
	// Channels
	mpDropDownChannels = CreatePop( giCtrl_Channels_Popup, IDB_Button_Popup,  mpChannels_Popup, ge::SPos(192, 128), mpPane);
	mpDropDownChannels->SetValue(miChannels);
	
	
	//

	// Set compression type anew to force quality drop-down to update list
	mpDropDownAudioCodecs->SetValue(1);

	//--------------------------------------------
	// Tail
	ge::IText* pText =  CreateDisplay( giCtrl_Export_Tail, ge::SPos(172, 168),  ge::SSize(106, 14), CreateFont(Generic128, IDB_Font_Minix_Tight, ge::SRGB(0, 0, 0)),true);
	pText->SetClicksOutsideAsSuccess();
	
												
												
	// Cancel / Export
	CreateButton(giCtrl_Cancel_Export, IDB_Button_Cancel, ge::SPos(249, 212), false);
	CreateButton(giCtrl_Export, IDB_Button_Export, ge::SPos(317, 212), false);

}


void CKSExportGUIPane::GenerateQualitiesDropDown()
{
	mpQuality_Popup.iItemCount = 0;

	tint32 iFirstQ, iLastQ, iDefaultQ;
	CAutoDelete<ac::IEncoder> pEnc(ac::IEncoder::Create(meAudioCodec));
	if (pEnc == NULL)
		return;

	pEnc->GetQualityRange(&iFirstQ, &iLastQ, &iDefaultQ);
	tint32 iQualities = (iLastQ - iFirstQ) + 1;

	if (miAudioCodecQuality < 0)
		miAudioCodecQuality = iDefaultQ;

	// Compression quality list
	mpQuality_Popup.iItemCount = iQualities;
	tint32 iIx = 0;
	for (tint32 iQ = iFirstQ; iQ <= iLastQ; iQ++) {
		tchar pszQName[128];
		pEnc->GetQualityName(iQ, pszQName, 128);

		tint32 iKBPS_Mono = 0;
		tint32 iKBPS_Stereo = 0;
		pEnc->GetQualityApproximateBitRate(iQ, &iKBPS_Mono, &iKBPS_Stereo);

		// Work-around for unrealistic mono rates
		if (iKBPS_Mono < iKBPS_Stereo / 2)
			iKBPS_Mono = iKBPS_Stereo / 2;

		tchar pszLine[256];
		sprintf(pszLine, "%d/%d kbps (%s)", iKBPS_Stereo, iKBPS_Mono, pszQName);
		mpQuality_Popup.pItems[iIx++] = ge::IPopupMenu::SMenuItem(pszLine, iQ, NULL);
	}
}


void CKSExportGUIPane::ConnectControls()
{
	
	RegisterGlobalControl(giSectionGUI, giCtrl_Export_Selection,   giParamID_Export_Type);
	RegisterGlobalControl(giSectionGUI, giCtrl_Normalize_On_Export,   giParamID_Normalize_On_Export);
	
	RegisterGlobalControlWithDisplayTranslater(giSectionGUI, giCtrl_Export_Tail, giParamID_Export_Tail, 0, 6, 0, "  m.Sec");
	
	
	
	
}

void CKSExportGUIPane::OnTimer(tint32 iTimerID)
{
	
}


void CKSExportGUIPane::SendMsgFromTop(SMsg* pMsg)
{
	

	switch(pMsg->iOpcode) 
	{
		case MsgGUIData: {
			SGUIData sData = *(SGUIData*)pMsg->pDataIn;
			if(sData.iID == Msg_Show_NormalizeAndTail) {
				
				switch(sData.iVal ) {
					case 0:
					case 1:
					case 2:
						GetPane()->GetControl(giCtrl_Normalize_On_Export)->SetVisible(true);
						GetPane()->GetControl(giCtrl_Export_Tail)->SetVisible(true);
						break;
					default:
						GetPane()->GetControl(giCtrl_Normalize_On_Export)->SetVisible(false);
						GetPane()->GetControl(giCtrl_Export_Tail)->SetVisible(false);
						break;
				}
			}
			break;
		}
	}
	//
	
	
	/*
	 SGUIData sData;
	 sData.iID	= Msg_Show_Info;
	 sData.iVal	= !bTest;
	 CBasePane::SMsg Msg;
	 Msg = MsgGUIData;
	 Msg.pDataIn	= (void*)&sData;
	 */
		

}



void CKSExportGUIPane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	switch(pControl->GetID()) {
		case giCtrl_Export:{
			
			GetPlugIn()->SetGlobalParm(giParamID_Show_Export_Window, 0, giSectionGUI);
			
			dynamic_cast<CKSPlugIn*>(GetPlugIn())->Export(meAudioCodec, miAudioCodecQuality, miChannels, miTailMS, mbNormalize);
			break;
		}
		case giCtrl_Cancel_Export:{
			
			GetPlugIn()->SetGlobalParm(giParamID_Show_Export_Window, 0, giSectionGUI);
			
			break;
		}

		case giCtrl_Export_Selection:
			{
				switch (iValueNew) {
					// Default for mix down is mp3
					case 0: mpDropDownAudioCodecs->SetValue(1); break;
				}
			}
			break;

		case giCtrl_Wave_Type_Popup:
			{
				switch (iValueNew) {
					case 0: meAudioCodec = ac::geAudioCodecWave; break;
					case 1: meAudioCodec = ac::geAudioCodecLame; break;
					case 2: meAudioCodec = ac::geAudioCodecVorbis; break;
				}
				UpdateQualitiesDropDown();
			}
			break;

		case giCtrl_Compression_Popup:
			{
				if (meAudioCodec == ac::geAudioCodecUndefined) {
					meAudioCodec = ac::geAudioCodecWave;		
					UpdateQualitiesDropDown();
				}
				miAudioCodecQuality = iValueNew;
			}
			break;

		case giCtrl_Channels_Popup:
			{
				miChannels = iValueNew;
			}
			break;

		case giCtrl_Normalize_On_Export:
			{
				mbNormalize = (iValueNew != 0);
			}
			break;
			
		case giCtrl_Export_Tail:
			{
				miTailMS = iValueNew;
			}
			break;
	}
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
}


void CKSExportGUIPane::UpdateQualitiesDropDown()
{
	if (mpDropDownQualities) {
		miAudioCodecQuality = -1;
		GenerateQualitiesDropDown();
		ge::IPopupMenu* pDropMenu = mpDropDownQualities->GetPopup();
		pDropMenu->DestroyItems();
		pDropMenu->AppendItems(mpQuality_Popup);
		mpDropDownQualities->SetValue(miAudioCodecQuality);
	}
} // UpdateQualitiesDropDown


void CKSExportGUIPane::Resize(ge::SSize SizeNew)
{
}


void CKSExportGUIPane::UpdateGUIData(tint32 iID, tint32 iValue)
{
/*	if (iID == giParamID_Show_Export_Window) {
		if (iValue == 0) {
			mbIsVisible = false;
		}
		else {
			mbIsVisible = true;
		}
		return;
	}
*/
}

void CKSExportGUIPane::Prepare_Popups()
{

	
	
	
	
	//
	mpType_Popup.iItemCount = 3;
	mpType_Popup.pItems[0] = ge::IPopupMenu::SMenuItem((const tchar*)"WAVE",		0,		NULL);
	mpType_Popup.pItems[1] = ge::IPopupMenu::SMenuItem((const tchar*)"MP3",			1,		NULL);
	mpType_Popup.pItems[2] = ge::IPopupMenu::SMenuItem((const tchar*)"Ogg/Vorbis",	2,		NULL);
	
	
	//
	mpChannels_Popup.iItemCount = 3;
	mpChannels_Popup.pItems[0] = ge::IPopupMenu::SMenuItem((const tchar*)"(unchanged)",	0,		NULL);
	mpChannels_Popup.pItems[1] = ge::IPopupMenu::SMenuItem((const tchar*)"Mono",		1,		NULL);
	mpChannels_Popup.pItems[2] = ge::IPopupMenu::SMenuItem((const tchar*)"Stereo",		2,		NULL);

	

	GenerateQualitiesDropDown();
}

ge::IDropDownListBox* CKSExportGUIPane::CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos, ge::IPane* pPane)
{
	// Resources
	tint32 piIDs[ge::IPopupMenu::BitmapCount];
	piIDs[ge::IPopupMenu::BitmapFolderArrow]	= iBitmapID;
	piIDs[ge::IPopupMenu::BitmapTop]			= IDB_PopupBorder_Big;
	piIDs[ge::IPopupMenu::BitmapCenter]			= IDB_Popup_Boddy_Big;
	piIDs[ge::IPopupMenu::BitmapCenterInv]		= IDB_PopupBoddyInverted_Big;
	piIDs[ge::IPopupMenu::BitmapBottom]			= IDB_PopupBorder_Big;
	ge::IDropDownListBox* pListBox = CreateDropDownListBox( iControlID, 
															piIDs, 
															Generic128, 
															IDF_Font_Fuxley_712, 
															iBitmapID, 
															List, Pos, 
															ge::SPos(-20,-2),
															ge::SSize(160,12));
//	pListBox->DontSetText();
	return pListBox;
}