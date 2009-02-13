
#include "KSOS.h"


CKSExportForWebGUIPane::CKSExportForWebGUIPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{

	mbIsVisible = false;

	miLicenseNb = 0;
}

CKSExportForWebGUIPane::~CKSExportForWebGUIPane()
{
	
}

void CKSExportForWebGUIPane::Init()
{
//	mpPlugIn = gpApplication;

	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(541,582));
	
	//-------------------------------
	// Backdrop for Export
	CreateBitmap(ge::IControl::giNoID, IDB_Back_License, ge::SPos(0, 0));
	
	
	ge::SPos pos(160,30);
	
/*	
	// URL
	mpTextUrl = CreateDisplay(	giCtrl_CC_License_URL_Txt, 
												pos,  
												ge::SSize(344, 12), 
												CreateFont(Generic128, IDB_Font_Minix_Tight, ge::SRGB(10, 10, 10)),
												true);
	 
	mpTextUrl->SetHorzAlignment(ge::IText::HorzAlignLeft);
	mpTextUrl->SetClicksOutsideAsSuccess();
	
	// Author
	pos.iY += 20;
	mpTextAuthor = CreateDisplay(	giCtrl_CC_License_Author_Txt, 
												pos,  
												ge::SSize(344, 12), 
												CreateFont(Generic128, IDB_Font_Minix_Tight, ge::SRGB(10, 10, 10)),
												true);

	mpTextAuthor->SetHorzAlignment(ge::IText::HorzAlignLeft);
	mpTextAuthor->SetClicksOutsideAsSuccess();
 
	
	// Status
	pos.iY += 20;
	mpTextStatus = CreateDisplay(	giCtrl_CC_License_Status_Txt, 
												pos,  
												ge::SSize(344, 12), 
												CreateFont(Generic128, IDB_Font_Minix_Tight, ge::SRGB(10, 10, 10)),
												true);

	mpTextStatus->SetHorzAlignment(ge::IText::HorzAlignLeft);
	mpTextStatus->SetClicksOutsideAsSuccess();
	
	
//	Create2StateButton(giCtrl_Delay_Power, IDB_Button_Power, ge::SPos(25, 36), true);

*/
	pos.iX = 32;
	pos.iY = 132-74; 

	mpSetup_Radio_Button = CreateRadioButtonsGroup(giCtrl_CC_License_Radio_Button, ge::SPos(0,0) );
	// Account button			
	CreateRadioButton(ge::IControl::giNoID, IDB_Button_Radio, pos, mpSetup_Radio_Button, true); pos.iY += 84;
	CreateRadioButton(ge::IControl::giNoID, IDB_Button_Radio, pos, mpSetup_Radio_Button, true); pos.iY += 84;
	CreateRadioButton(ge::IControl::giNoID, IDB_Button_Radio, pos, mpSetup_Radio_Button, true); pos.iY = 132-74; pos.iX = 226;
	CreateRadioButton(ge::IControl::giNoID, IDB_Button_Radio, pos, mpSetup_Radio_Button, true); pos.iY += 84;
//	CreateRadioButton(ge::IControl::giNoID, IDB_Button_Radio, pos, mpSetup_Radio_Button, true); pos.iY += 84;
	CreateRadioButton(ge::IControl::giNoID, IDB_Button_Radio, pos, mpSetup_Radio_Button, true);
	

	pos.iY = 162-74; pos.iX = 60;
	ge::SSize UrlSize(200, 18);
	// BY
	mpBY_URL	=	ge::IURL::Create();
	mpBY_URL->Init(ge::IControl::giNoID, gpApplication->mapszLicenseUrls[0]);
	
	mpBY_URL->SetSize(UrlSize);
//	mpBY_URL->ActivateHandCursor();
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpBY_URL), pos); pos.iY += 84;
	
	//BY SA
	mpBY_SA_URL	=	ge::IURL::Create();
	mpBY_SA_URL->Init(ge::IControl::giNoID, gpApplication->mapszLicenseUrls[1]);
	mpBY_SA_URL->SetSize(UrlSize);
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpBY_SA_URL), pos); pos.iY += 84;
	
	//BY ND
	mpBY_SA_URL	=	ge::IURL::Create();
	mpBY_SA_URL->Init(ge::IControl::giNoID, gpApplication->mapszLicenseUrls[2]);
	mpBY_SA_URL->SetSize(UrlSize);
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpBY_SA_URL), pos);
	
	
	pos.iY = 162-74; pos.iX = 250;
	
	// BY NC
	mpBY_URL	=	ge::IURL::Create();
	mpBY_URL->Init(ge::IControl::giNoID, gpApplication->mapszLicenseUrls[3]);
	
	mpBY_URL->SetSize(UrlSize);
//	mpBY_URL->ActivateHandCursor();
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpBY_URL), pos); pos.iY += 84;
	
	//BY NC SA
	mpBY_SA_URL	=	ge::IURL::Create();
	mpBY_SA_URL->Init(ge::IControl::giNoID, gpApplication->mapszLicenseUrls[4]);
	mpBY_SA_URL->SetSize(UrlSize);
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpBY_SA_URL), pos); pos.iY += 84;
	
	//BY NC ND
	mpBY_SA_URL	=	ge::IURL::Create();
	mpBY_SA_URL->Init(ge::IControl::giNoID, gpApplication->mapszLicenseUrls[5]);
	mpBY_SA_URL->SetSize(UrlSize);
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpBY_SA_URL), pos);
	
	pos.iY = 563-74; pos.iX = 45;
	//Creative Commons
	mpBY_SA_URL	=	ge::IURL::Create();	
	mpBY_SA_URL->Init(ge::IControl::giNoID,(tchar*)"http://creativecommons.org/");
	mpBY_SA_URL->SetSize(UrlSize);
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpBY_SA_URL), pos);
	

	Prepare_Popups();
	
	// Compression
//	ge::IDropDownListBox* pPop = CreatePop( giCtrl_Export_Compression_Popup, IDB_Button_Popup,  mpCompression_Popup, ge::SPos(364, 521-74), mpPane);
//	pPop->SetValue(meQuality);
												
	// Cancel / Export
	CreateButton(giCtrl_Cancel_ExportForWeb, IDB_Button_Cancel, ge::SPos(402, 447), false);
	CreateButton(giCtrl_ExportForWeb, IDB_Button_Export, ge::SPos(402+68, 447), false);
	
	mpLicense =CreateBitmapVU(ge::IControl::giNoID, IDB_License, ge::SPos(27, 368-74), 6);
	mpLicense->SetFrame(0);
} // Init


void CKSExportForWebGUIPane::ConnectControls()
{
	
	RegisterGlobalControl(giSectionGUI, giCtrl_CC_License_Radio_Button,   giParamID_CC_License_Type);
	
//	RegisterGlobalControlWithDisplayTranslater(giSectionGUI, giCtrl_Export_Tail, giParamID_Export_Tail, 0, 6, 0, "  m.Sec");
} // ConnectControl


void CKSExportForWebGUIPane::OnTimer(tint32 iTimerID)
{
	
} // OnTimer(tint32)


void CKSExportForWebGUIPane::SendMsgFromTop(SMsg* pMsg)
{
	HandleMsg(pMsg);
} // SendMsgFromTop



void CKSExportForWebGUIPane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{

	switch(pControl->GetID()) {
		case giCtrl_ExportForWeb:{
			/*
			std::string sViolations = "";
			if (!mbMayExport) {
				sViolations += " - This project must not be re-exported.\n";
			}
			if ((!mbMayChangeLicense) && (miLicenseNb != mpPlugIn->miPreviousLicenseNb)) {
				sViolations += " - You are not allowed to alter the license.\n";
			}
			if ((!mbMayBeCommercial) && (miLicenseNb < 3)) {
				sViolations +=
					" - This project has been previouly released\n"
					" under a non-commercial license, so you can't\n"
					" re-export it as a commercial project.\n";
			}
			if (sViolations.length() > 0) {
				ShowSorryDialog(sViolations);
			}
			else*/ {
				// Close the window
				gpApplication->SetGlobalParm(giParamID_Show_Export_For_Web_Window, 0, giSectionGUI);
				// Set 
		/*		tchar pszAuthor[1024];
				tchar pszUrl[1024];
				tchar pszStatus[1024];
				mpTextAuthor->GetText(pszAuthor);
				mpTextUrl->GetText(pszUrl);
				mpTextStatus->GetText(pszStatus);
				
				gpApplication->Export_For_Web(meQuality, miLicenseNb, pszAuthor, pszUrl, pszStatus);
		 
		 */
			}
			break;
		}
		case giCtrl_Cancel_ExportForWeb:{
			
			gpApplication->SetGlobalParm(giParamID_Show_Export_For_Web_Window, 0, giSectionGUI);
			
			break;
		}
		case giCtrl_CC_License_Radio_Button:{
			miLicenseNb = iValueNew;
			mpLicense->SetFrame(iValueNew);
			break;
		}
			/*
		case giCtrl_Export_Compression_Popup:{
				
			meQuality = (ac::EQuality)iValueNew;
			break;
		}
			 */
			
	}
	
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
} // EventValueChange


void CKSExportForWebGUIPane::Resize(ge::SSize SizeNew)
{
} // Resize


void CKSExportForWebGUIPane::HandleMsg(SMsg* pMsg)
{
	/*
	if (pMsg->iOpcode == Msg_Init_ExportForWeb) {
		mpTextAuthor->SetText(mpPlugIn->msExportForWeb.sUserName.c_str());
		mpTextUrl->SetText(mpPlugIn->msExportForWeb.sUserUrl.c_str());
		mpTextStatus->SetText(mpPlugIn->msExportForWeb.sUserStatus.c_str());
		miLicenseNb = mpPlugIn->miPreviousLicenseNb;
		mbHasPreviousLicense = (miLicenseNb >= 0);
		mbMayBeCommercial = ((!mbHasPreviousLicense) || (miLicenseNb < 3));
		mbMayChangeLicense = ((!mbHasPreviousLicense) || ((miLicenseNb == 0) || (miLicenseNb == 3)));
		mbMayExport = ((!mbHasPreviousLicense) || (((miLicenseNb != 2) && (miLicenseNb != 5))));
		if (!mbHasPreviousLicense) miLicenseNb = 0;

		mpSetup_Radio_Button->SetValue(miLicenseNb);
		mpTextAuthor->MakeEditable(!mbHasPreviousLicense);
		mpTextUrl->MakeEditable(!mbHasPreviousLicense);
		mpTextStatus->MakeEditable(!mbHasPreviousLicense);
	}
	 */
} // HandleMsg


void CKSExportForWebGUIPane::ShowSorryDialog(std::string sViolation)
{
	std::string sMsg =
		"Export is not possible.\n"
		"\n"
		"You are trying to export this project in a way\n"
		"that would violate the Creative Commons license\n"
		"which it has been previously assigned.\n"
		"\n"
		"Problem(s):\n\n";
	sMsg += sViolation;
	mpGUI->GetWindow()->ShowMessageBox(sMsg.c_str(), "Sorry", ge::IWindow::MsgBoxOK);

} // ShowSorryDialog


void CKSExportForWebGUIPane::UpdateGUIData(tint32 iID, tint32 iValue)
{
} // UpdateGUIData


void CKSExportForWebGUIPane::Prepare_Popups()
{
	/*
	tint32 iFirstQ, iLastQ, iDefaultQ;
	CAutoDelete<ac::IEncoder> pEnc(ac::IEncoder::Create(ac::geAudioCodecVorbis));
	pEnc->GetQualityRange(&iFirstQ, &iLastQ, &iDefaultQ);
	tint32 iQualities = (iLastQ - iFirstQ) + 1;

	meQuality = (ac::EQuality)iDefaultQ;

	// Compression list
	mpCompression_Popup.iItemCount = iQualities + 0;
	tint32 iIx = 0;
	for (tint32 iQ = iFirstQ; iQ <= iLastQ; iQ++) {
		tchar pszQName[128];
		pEnc->GetQualityName(iQ, pszQName, 128);

		tint32 iKBPS_Mono = 0;
		tint32 iKBPS_Stereo = 0;
		pEnc->GetQualityApproximateBitRate(iQ, &iKBPS_Mono, &iKBPS_Stereo);

		tchar pszLine[256];
		sprintf(pszLine, "%d/%d kbps (%s)", iKBPS_Stereo, iKBPS_Mono, pszQName);
		mpCompression_Popup.pItems[iIx++] = ge::IPopupMenu::SMenuItem(pszLine, iQ, NULL);
	}
	 */
} // Prepare_Popups


ge::IDropDownListBox* CKSExportForWebGUIPane::CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos, ge::IPane* pPane)
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
} // CreatePop