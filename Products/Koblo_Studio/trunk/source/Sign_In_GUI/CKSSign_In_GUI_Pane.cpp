
#include "KSOS.h"


CKSSign_In_GUI_Pane::CKSSign_In_GUI_Pane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI),
	mbMenuInited(false)
{
//	CBaseDSPEngine* pD = GetPlugIn()->GetDSPEngine();
//	mpDSP = dynamic_cast<CKSDSP*>(pD);
}

CKSSign_In_GUI_Pane::~CKSSign_In_GUI_Pane()
{
	if (mpPopupMenuAudioDevices) {
		mpPopupMenuAudioDevices->Destroy();
		mpPopupMenuAudioDevices = NULL;
	}
	if (mpPopupMenuAudioRates) {
		mpPopupMenuAudioRates->Destroy();
		mpPopupMenuAudioRates = NULL;
	}
	if (mpPopupMenuAudioBuffs) {
		mpPopupMenuAudioBuffs->Destroy();
		mpPopupMenuAudioBuffs = NULL;
	}
}

void CKSSign_In_GUI_Pane::Init()
{
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(320,245));
	
	tint32 iPosY = 0;
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Setup, ge::SPos(0, 0)); iPosY += 60;
	
	mpDevice_BMP				= CreateBitmap(ge::IControl::giNoID, IDB_Back_Setup_Device, ge::SPos(0, 54));
	mpDevice_Asio_BMP			= CreateBitmap(ge::IControl::giNoID, IDB_Back_Setup_Device_Asio, ge::SPos(0, 54)); 


	
	// Take lock - no questions
	miBlockSetAudioDevice_volatile = 1;

//	Prepare_Popups();

	
	// Audio In devices
	ge::IPopupMenu::SMenuItemList list;
	list.iItemCount = 0;
	mpPopupMenuAudioIn = CreatePop( giCtrl_Sound_Card_In_Popup, IDB_Button_Popup, list, ge::SPos(47, 69), mpPane);
	// Audio devices/ out
	//ge::IPopupMenu::SMenuItemList list;
	//list.iItemCount = 0;
	mpPopupMenuAudioDevices = CreatePop( giCtrl_Sound_Card_Popup, IDB_Button_Popup, list, ge::SPos(47, 107), mpPane);

	// Audio frequency - giCtrl_Sample_Rate_Popup
	list.iItemCount = 0;
	mpPopupMenuAudioRates = CreatePop( giCtrl_Sample_Rate_Popup, IDB_Button_Popup, list, ge::SPos(47, 107+38), mpPane);

	// Audio buffer - giCtrl_Buffer_Size_Popup
	list.iItemCount = 0;
	mpPopupMenuAudioBuffs = CreatePop( giCtrl_Buffer_Size_Popup, IDB_Button_Popup, list, ge::SPos(47, 107+(38*2)), mpPane);
	

	// ASIO Setup
	mpASIO_Button = CreateButton(giCtrl_Asio_Setup, IDB_Button_Close, ge::SPos(59, 57), false);
	
	
	// Close
	CreateButton(giCtrl_Close_Audio_Setup, IDB_Button_Close, ge::SPos(232, 220), false);

	InitAudioDevicePopupValues();
	InitAudioDevicePopupValuesInput();

	//!!! Remove this
	SetupIO();

	// Release lock
	miBlockSetAudioDevice_volatile--;
}

void CKSSign_In_GUI_Pane::ConnectControls()
{
//	RegisterGlobalControl(giSectionGUI, giCtrl_Export_Selection,   giParamID_Export_Type);
	
//	RegisterGlobalControlWithDisplayTranslater(giSectionGUI, giCtrl_Export_Tail, giParamID_Export_Tail, 0, 6, 0, "  m.Sec");
}

void CKSSign_In_GUI_Pane::OnTimer(tint32 iTimerID)
{
}

void CKSSign_In_GUI_Pane::SendMsgFromTop(SMsg* pMsg)
{
	switch(pMsg->iOpcode) 
	{
		case MsgGUIData:{
			SGUIData sData = *(SGUIData*)pMsg->pDataIn;
			switch( sData.iID ){
				case Msg_Show_Info: {
				InitAudioDevicePopupValues();
				break;
				}
			default: break;
			}
		}
	}
}


void CKSSign_In_GUI_Pane::ThatDidntWork()
{
	tchar* pszHeader = "Audio Device Error";
	tchar pszMessage[1024];
	sprintf(pszMessage, "Hm.. That didn't work.");

	tbool bInput = (gpDSPEngine->mPrefs_2.cbInputDevice_LastOpenSuccess != 0);
	tbool bOutput = (gpDSPEngine->mPrefs_1.cbAudioDevice_LastOpenSuccess != 0);
	if ((!bInput) && (!bOutput)) {
		if (gpDSPEngine->IsAudioDeviceDuplex()) {
			strcat(pszMessage, "\n\nFull duplex audio device isn't running.");
		}
		else {
			strcat(pszMessage, "\n\nInput and output audio devices aren't running.");
		}
	}
	else {
		if (!bInput) {
			strcat(pszMessage, "\n\nInput isn't running.");
		}
		else if (!bOutput) {
			strcat(pszMessage, "\n\nOutput isn't running.");
		}
	}

	GetWindow()->ShowMessageBox(pszMessage, pszHeader, ge::IWindow::MsgBoxOK);
} // ThatDidntWork


void CKSSign_In_GUI_Pane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{

	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
	
	switch (pControl->GetID()) {

		// New audio device 
		case giCtrl_Sound_Card_Popup:
			{
				// Attempt lock
				if (++miBlockSetAudioDevice_volatile != 1) {
					// We're not alone - release lock
					miBlockSetAudioDevice_volatile--;
				}
				else {
					CKSDSP* pDSP = dynamic_cast<CKSDSP*>(GetPlugIn()->GetDSPEngine());
					tbool bSuccess = pDSP->OpenAudioDevice_FromMenu_Output(iValueNew, -1, -1);
					// Fill popup with valid frequencies and buffer sizes
					InitAudioDevicePopupValues();
					if (!bSuccess) {
						ThatDidntWork();
					}
					// Release lock
					miBlockSetAudioDevice_volatile--;
				}
			}
			break;

		case giCtrl_Sound_Card_In_Popup:
			{
				// Attempt lock
				if (++miBlockSetAudioDevice_volatile != 1) {
					// We're not alone - release lock
					miBlockSetAudioDevice_volatile--;
				}
				else {
					CKSDSP* pDSP = dynamic_cast<CKSDSP*>(GetPlugIn()->GetDSPEngine());
					tbool bSuccess = pDSP->OpenAudioDevice_FromMenu_Input(iValueNew, /*-1,*/ -1);
					// Fill popup with valid frequencies and buffer sizes
					InitAudioDevicePopupValuesInput();
					if (!bSuccess) {
						ThatDidntWork();
					}
					// Release lock
					miBlockSetAudioDevice_volatile--;
				}
			}
			break;

		// New sample rate
		case giCtrl_Sample_Rate_Popup:
			{
				// Attempt lock
				if (++miBlockSetAudioDevice_volatile != 1) {
					// We're not alone - release lock
					miBlockSetAudioDevice_volatile--;
				}
				else {
					CKSDSP* pDSP = dynamic_cast<CKSDSP*>(GetPlugIn()->GetDSPEngine());
					tint32 iMgr = pDSP->mPrefs_1.iAudioManagerIx;
					tint32 iDev = pDSP->mPrefs_1.iAudioDeviceIx;
					tchar* pszDev = pDSP->mPrefs_1.pszAudioDevName;
					tint32 iFrq = iValueNew * 100; //pDSP->mPrefs_1.iAudioSampleRate;
					tint32 iBuf = pDSP->mPrefs_1.iAudioBufferSize;
					tbool bSuccess = pDSP->OpenAudioDevice_Output(iMgr, iDev, pszDev, iFrq, iBuf, true);
					if (!bSuccess) {
						ThatDidntWork();
					}
					// Release lock
					miBlockSetAudioDevice_volatile--;
				}
			}
			break;

		// New buffer size
		case giCtrl_Buffer_Size_Popup:
			{
				// Attempt lock
				if (++miBlockSetAudioDevice_volatile != 1) {
					// We're not alone - release lock
					miBlockSetAudioDevice_volatile--;
				}
				else {
					CKSDSP* pDSP = dynamic_cast<CKSDSP*>(GetPlugIn()->GetDSPEngine());
					tint32 iMgr = pDSP->mPrefs_1.iAudioManagerIx;
					tint32 iDev = pDSP->mPrefs_1.iAudioDeviceIx;
					tchar* pszDev = pDSP->mPrefs_1.pszAudioDevName;
					tint32 iFrq = pDSP->mPrefs_1.iAudioSampleRate;
					tint32 iBuf = iValueNew * 32; //pDSP->mPrefs_1.iAudioBufferSize;
					tbool bSuccess = pDSP->OpenAudioDevice_Output(iMgr, iDev, pszDev, iFrq, iBuf, true);
					if (!bSuccess) {
						ThatDidntWork();
					}
					// Release lock
					miBlockSetAudioDevice_volatile--;
				}
			}
			break;
		case giCtrl_Close_Audio_Setup:
		{
			//tbool bTest = GetPlugIn()->GetGlobalParm(giParamID_Audio_Setup_Window, giSectionGUI) ;

			GetPlugIn()->SetGlobalParm(giParamID_Audio_Setup_Window,false, giSectionGUI);
			
		}
	}

}

void CKSSign_In_GUI_Pane::UpdateGUIData(tint32 iID, tint32 iValue)
{
}

ge::IDropDownListBox* CKSSign_In_GUI_Pane::CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos, ge::IPane* pPane)
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
															IDF_Fuxley_712, 
															iBitmapID, 
															List, Pos, 
															ge::SPos(-20,-2),
															ge::SSize(160,12));
//	pListBox->DontSetText();
	return pListBox;
}

void CKSSign_In_GUI_Pane::InitAudioDevicePopupValues()
{
/*	if (mbMenuInited) {
		return;
	}*/

	tbool bScanDevices = true;

#ifdef WIN32
	if (mbMenuInited == true) {
		bScanDevices = false;
	}
#endif	// WIN32

	mbMenuInited = true;

	// Don't set DSP values - Avoid error dialog on invalid values
	//mbBlockSetAudioDevice = true;

	ge::IPopupMenu::SMenuItemList list;
	tint32 iSelectedValue;
//	CKSDSP* pDSP = dynamic_cast<CKSDSP*>(GetPlugIn()->GetDSPEngine());

	// Devices
	if (bScanDevices) {
		gpDSPEngine->GetAudioDeviceMenuItems(list, iSelectedValue);
		mpPopupMenuAudioDevices->GetPopup()->DestroyItems();
		mpPopupMenuAudioDevices->GetPopup()->AppendItems(list);
		mpPopupMenuAudioDevices->SetValue(iSelectedValue);
	}

	if (bScanDevices) {
		gpDSPEngine->GetAudioDeviceMenuItemsInput(list, iSelectedValue);
		mpPopupMenuAudioIn->GetPopup()->DestroyItems();
		mpPopupMenuAudioIn->GetPopup()->AppendItems(list);
		mpPopupMenuAudioIn->SetValue(iSelectedValue);
	}

	// Sample-rates
	gpDSPEngine->GetAudioFreqsMenuItems(list, iSelectedValue);
	mpPopupMenuAudioRates->GetPopup()->DestroyItems();
	mpPopupMenuAudioRates->GetPopup()->AppendItems(list);
	mpPopupMenuAudioRates->SetValue(iSelectedValue);

	// Buffer sizes
	gpDSPEngine->GetAudioBuffsMenuItems(list, iSelectedValue);
	mpPopupMenuAudioBuffs->GetPopup()->DestroyItems();
	mpPopupMenuAudioBuffs->GetPopup()->AppendItems(list);
	mpPopupMenuAudioBuffs->SetValue(iSelectedValue);

	//mbBlockSetAudioDevice = false;

	/*
	if (strncmp(gpDSPEngine->mAudioSettings.pszAudioDevName, "WAVE: ", 6) == 0) {
		SetupIO();
	}
	else {
		SetupDevice();
	}
	*/
}

void CKSSign_In_GUI_Pane::InitAudioDevicePopupValuesInput()
{
	// Don't set DSP values - Avoid error dialog on invalid values
	//mbBlockSetAudioDevice = true;
	
	ge::IPopupMenu::SMenuItemList list;
	tint32 iSelectedValue;
	//	CKSDSP* pDSP = dynamic_cast<CKSDSP*>(GetPlugIn()->GetDSPEngine());
	
	// Devices
	gpDSPEngine->GetAudioDeviceMenuItemsInput(list, iSelectedValue);
	mpPopupMenuAudioIn->GetPopup()->DestroyItems();
	mpPopupMenuAudioIn->GetPopup()->AppendItems(list);
	mpPopupMenuAudioIn->SetValue(iSelectedValue);

	//mbBlockSetAudioDevice = false;
}

void CKSSign_In_GUI_Pane::Prepare_Popups()
{
/*
	tint32 iFirstQ, iLastQ;
	CAutoDelete<ac::IEncoder> pEnc(ac::IEncoder::Create(ac::geAudioCodecVorbis));
	pEnc->GetQualityRange(&iFirstQ, &iLastQ);
	tint32 iQualities = (iLastQ - iFirstQ) + 1;

	// Compression list
	mpCompression_Setup.iItemCount = iQualities + 0;
	tint32 iIx = 0;
	for (tint32 iQ = iFirstQ; iQ <= iLastQ; iQ++) {
		tchar pszQName[128];
		pEnc->GetQualityName(iQ, pszQName, 128);

		tint32 iKBPS_Mono = 0;
		pEnc->GetQualityApproximateBitRate(iQ, &iKBPS_Mono, NULL);

		tchar pszLine[256];
		sprintf(pszLine, "%d kbps (%s Quality)", iKBPS_Mono, pszQName);
		mpCompression_Setup.pItems[iIx++] = ge::IPopupMenu::SMenuItem(pszLine, iQ, NULL);
	}
*/
}

void CKSSign_In_GUI_Pane::SetupDevice()
{
	mpASIO_Button->SetVisible(false);
	mpPopupMenuAudioIn->SetVisible(false);
	mpDevice_Asio_BMP->SetVisible(false);
	mpDevice_BMP->SetVisible(true);
}

void CKSSign_In_GUI_Pane::SetupDeviceAsio()
{
	mpASIO_Button->SetVisible(true);
	mpPopupMenuAudioIn->SetVisible(false);
	mpDevice_Asio_BMP->SetVisible(true);
	mpDevice_BMP->SetVisible(false);
}

void CKSSign_In_GUI_Pane::SetupIO()
{
	mpASIO_Button->SetVisible(false);
	mpPopupMenuAudioIn->SetVisible(true);
	mpDevice_Asio_BMP->SetVisible(false);
	mpDevice_BMP->SetVisible(false);
}




