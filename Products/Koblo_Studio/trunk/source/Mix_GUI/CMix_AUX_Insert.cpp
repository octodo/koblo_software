#include "KSOS.h"

CMix_AUX_Insert::CMix_AUX_Insert(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

	mpInput = NULL;

	// (lasse) Crash-fix! Set number of menu items to 0
	mpInput_Menu.iItemCount = 0;

	miInPreparePopups_volatile = 0;
}

CMix_AUX_Insert::~CMix_AUX_Insert()
{
}

void CMix_AUX_Insert::SetInfo(tint32 iID, tint32 iTimerID)
{
	miID						=	iID;
	miTimerID					=	iTimerID;
	miCtrl_Offset				=	giFirst_Track_Ctrl + ( miID * giNr_Of_Track_Ctls);
	miSection					=	iID + giSection_First_Track;
	
}

void CMix_AUX_Insert::Init()
{
	mpScrollPane = ge::IScrollPane::Create();
	// Lasse, fix 2008-05-13
	//mpScrollPane->Init(ge::IControl::giNoID, giCtr_Mix_Aux_ScrollPane_Timer);
	mpScrollPane->Init(ge::IControl::giNoID, miTimerID);
	// .. Lasse
	mpPane = dynamic_cast<ge::IPane*>(mpScrollPane);
	mpPane->SetSize(ge::SSize(gTrack_Aux_Insert.iCX , gTrack_Aux_Insert.iCY));
	mpPane->SetBackgroundColour(ge::SRGB(5,242,242));
	
	
	// Back Drop
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Mix_AUX_Insert, ge::SPos(0, 0));

	//-----------------------------------------
	// Echo
	CreateKnob(giCtrl_AUX1Send + miCtrl_Offset, IDB_Dial_Aux, ge::SPos(17, 20), 129);
	// Reverb
	CreateKnob(giCtrl_AUX2Send + miCtrl_Offset, IDB_Dial_Aux, ge::SPos(17, 84), 129);
	//-----------------------------------------
	// Channel In
	PreparePopups();
	CreatePop(giChannel_Input_Popup+ miCtrl_Offset, IDB_Button_Invisible_16_64, mpInput_Menu, ge::SPos(68, 16), ge::SSize(18, 12), ge::SRGB(0,204,0));

	
	CreateButton(miCtrl_Offset + giCtrlOpenPlugEdit1, IDB_Button_Edit_Plugin, ge::SPos(116, 48));
	CreateButton(miCtrl_Offset + giCtrlOpenPlugEdit2, IDB_Button_Edit_Plugin, ge::SPos(116, 48 + 16));
	CreateButton(miCtrl_Offset + giCtrlOpenPlugEdit3, IDB_Button_Edit_Plugin, ge::SPos(116, 48 + 16 * 2));
	CreateButton(miCtrl_Offset + giCtrlOpenPlugEdit4, IDB_Button_Edit_Plugin, ge::SPos(116, 48 + 16 * 3));

	// Inserts
	tint32 piIDs[ge::IPopupMenu::BitmapCount];
	piIDs[ge::IPopupMenu::BitmapFolderArrow]	= IDB_Button_Popup;
	piIDs[ge::IPopupMenu::BitmapTop]			= IDB_PopupBorder;
	piIDs[ge::IPopupMenu::BitmapCenter]			= IDB_POPUP_BODY;
	piIDs[ge::IPopupMenu::BitmapCenterInv]		= IDB_POPUP_BODY_INVERTED;
	piIDs[ge::IPopupMenu::BitmapBottom]			= IDB_PopupBorder;

	CPlugInManager* pPlugManager = gpApplication->GetPlugInManager();
	tint32 iPlugInCount = pPlugManager->GetNrOfPlugIns();
	ge::IPopupMenu::SMenuItemList List;
	List.iItemCount = iPlugInCount + 1;
	tint32 iPlugIn;
	for (iPlugIn = 0; iPlugIn < iPlugInCount; iPlugIn++) {
		CPlugInManager::SPlugInInfo* pInfo = pPlugManager->GetPlugInInfo(iPlugIn);
		
		//	std::string s = pInfo->sCompanyName;
		//	s += std::string(": ");
		std::string s  = pInfo->sProductName;
		tint32* pi = new tint32[2];
		pi[0] = pInfo->uiCompanyID;
		pi[1] = pInfo->uiProductID;
		List.pItems[iPlugIn + 1] = ge::IPopupMenu::SMenuItem(s.c_str(), iPlugIn + 1, NULL, NULL, -1, 0, (void*)pi);
	}
	tint32* pi = new tint32[2];
	pi[0] = 0;
	pi[1] = 0;
	List.pItems[0] = ge::IPopupMenu::SMenuItem("None", 0, NULL, NULL, -1, 0, (void*)pi);
	
	CreatePop(giCtrl_InsertPop1 + miCtrl_Offset, IDB_Button_Invisible_16_64, List, ge::SPos(64, 48), ge::SSize(61, 15), ge::SRGB(204,204,204));
	CreatePop(giCtrl_InsertPop2 + miCtrl_Offset, IDB_Button_Invisible_16_64, List, ge::SPos(64, 48 + 16), ge::SSize(61, 15), ge::SRGB(204,204,204));
	CreatePop(giCtrl_InsertPop3 + miCtrl_Offset, IDB_Button_Invisible_16_64, List, ge::SPos(64, 48 + 16 * 2), ge::SSize(61, 15), ge::SRGB(204,204,204));
	CreatePop(giCtrl_InsertPop4 + miCtrl_Offset, IDB_Button_Invisible_16_64, List, ge::SPos(64, 48 + 16 * 3), ge::SSize(61, 15), ge::SRGB(204,204,204));
}


void CMix_AUX_Insert::ConnectControls()
{
	RegisterGlobalControl(miSection, giChannel_Input_Popup + miCtrl_Offset , giParam_ChIn);

	RegisterGlobalControlWithX3Translation(miSection, giCtrl_AUX1Send + miCtrl_Offset, giParam_ChAUX1, 4);

	RegisterGlobalControlWithX3Translation(miSection, giCtrl_AUX2Send + miCtrl_Offset, giParam_ChAUX2, 4);

	RegisterGlobalControl(miSection, giCtrl_InsertPop1 + miCtrl_Offset, giParam_ChInsert1);
	RegisterGlobalControl(miSection, giCtrl_InsertPop2 + miCtrl_Offset, giParam_ChInsert2);
	RegisterGlobalControl(miSection, giCtrl_InsertPop3 + miCtrl_Offset, giParam_ChInsert3);
	RegisterGlobalControl(miSection, giCtrl_InsertPop4 + miCtrl_Offset, giParam_ChInsert4);
}

void CMix_AUX_Insert::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	if (pControl->GetID() == giCtrl_InsertPop1 + miCtrl_Offset ||
		pControl->GetID() == giCtrl_InsertPop2 + miCtrl_Offset ||
		pControl->GetID() == giCtrl_InsertPop3 + miCtrl_Offset ||
		pControl->GetID() == giCtrl_InsertPop4 + miCtrl_Offset) {
		if (iValueNew != 0) {
			//			tint32 iInsert = pControl->GetID() - (giCtrl_InsertPop1 + miCtrl_Offset);
			ge::IPopupMenu* pPopup = dynamic_cast<ge::IDropDownListBox*>(pControl)->GetPopup();
			tint32* pi = (tint32*)(pPopup->GetData(iValueNew));
			tint32 iCompanyID = pi[0];
			tint32 iProductID = pi[1];
			if (iCompanyID == 2) {
				iValueNew = iProductID;
			}
			else {
				iValueNew = (iCompanyID << 8) | iProductID;
			}
		}
	}

	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);

	if (pControl->GetID() == miCtrl_Offset + giCtrlOpenPlugEdit1) {
//! TODO: Determine what is right!   -- Lasse
//max:		GetParmMan()->Set(true, 1, giParam_Ch_Insert1GUIOpen, de::IParameterManager::TypeGlobal, miSection, true);
//mo:
//		GetParmMan()->Set(true, 1, giParam_Ch_Insert1GUIOpen, de::IParameterManager::TypeGlobal, miSection);
		gpApplication->GetPlugInManager()->OpenGUI(miID, 0);
	}
	else if (pControl->GetID() == miCtrl_Offset + giCtrlOpenPlugEdit2) {
//		GetParmMan()->Set(true, 1, giParam_Ch_Insert2GUIOpen, de::IParameterManager::TypeGlobal, miSection);
		gpApplication->GetPlugInManager()->OpenGUI(miID, 1);
	}
	else if (pControl->GetID() == miCtrl_Offset + giCtrlOpenPlugEdit3) {
//		GetParmMan()->Set(true, 1, giParam_Ch_Insert3GUIOpen, de::IParameterManager::TypeGlobal, miSection);
		gpApplication->GetPlugInManager()->OpenGUI(miID, 2);
	}
	else if (pControl->GetID() == miCtrl_Offset + giCtrlOpenPlugEdit4) {
//		GetParmMan()->Set(true, 1, giParam_Ch_Insert4GUIOpen, de::IParameterManager::TypeGlobal, miSection);
		gpApplication->GetPlugInManager()->OpenGUI(miID, 3);
	}
}

void CMix_AUX_Insert::EventGeneric(ge::IControl* pControl, void* pEventData)
{
}


void CMix_AUX_Insert::HandleMsg(SMsg* pMsg)
{
	switch(pMsg->iOpcode) {
		case Msg_Track_Data:{
			STrackData sData = *(STrackData*)pMsg->pDataIn;
			switch( sData.iParam_ID ){
				case giParam_Track_Info_Show_Inserts: {
					if(sData.iTrack == miID)
						sData.iVal ?  SlideToInsertsView() : SlideToIOView() ;
					
					break;
				}
			default: break;
			}
		}
		break; 

		case MsgUpdateAudioGUI:
			PreparePopups();
			break;
	}
}


void CMix_AUX_Insert::Scroll(tint32 iXLeft)
{
	mpScrollPane->SlideTo(ge::SPos(iXLeft, 0), 0, 15, 3);
}


void CMix_AUX_Insert::SlideToIOView()
{
		Scroll(0);
}

void CMix_AUX_Insert::SlideToInsertsView()
{
		Scroll(gTrack_Aux_Insert.iCX);
}

void CMix_AUX_Insert::PreparePopups()
{
	// Attempt lock
	if (++miInPreparePopups_volatile != 1) {
		// We're not alone - release lock
		miInPreparePopups_volatile--;
		return;
	}

	tbool bUpdate = false;
	if (mpInput) {
/*		GetPane()->RemoveControl(dynamic_cast<ge::IControl*>(mpInput));

		mpInput->Destroy();*/
/*		mpInput->SetVisible(false);

		bUpdate = true;*/
	}
	
	tint32 iInputChannels = gpApplication->GetNrOfInputChannels();

	ge::IPopupMenu::SMenuItemList* pMenuMono = new ge::IPopupMenu::SMenuItemList();
	ge::IPopupMenu::SMenuItemList* pMenuStereo = new ge::IPopupMenu::SMenuItemList();
	ge::IPopupMenu::SMenuItemList* pMenu = new ge::IPopupMenu::SMenuItemList();

	pMenu->iItemCount = 2;
	pMenu->pItems[0] = ge::IPopupMenu::SMenuItem("Mono", -1, pMenuMono);
	pMenu->pItems[1] = ge::IPopupMenu::SMenuItem("Stereo", -1, pMenuStereo);

	pMenuMono->iItemCount = iInputChannels;
	pMenuStereo->iItemCount = iInputChannels / 2;
	tint i;
	for (i = 0; i < iInputChannels; i++) {
		tchar psz[32];
		sprintf(psz, "%d", i + 1);
		pMenuMono->pItems[i] = ge::IPopupMenu::SMenuItem(psz, i, NULL);
		
		if (i % 2 == 0) {
			sprintf(psz, "%d-%d", i + 1, i + 2);
			pMenuStereo->pItems[i / 2] = ge::IPopupMenu::SMenuItem(psz, i + 1000, NULL);
		}
	}

	if (mpInput == NULL) {
		mpInput = CreatePop(giChannel_Input_Popup + miCtrl_Offset, IDB_Button_Invisible_16_64, *pMenu, ge::SPos(68, 16), ge::SSize(18, 12), ge::SRGB(0,204,0));
	}
	else {
		mpInput->GetPopup()->DestroyItems();
		mpInput->GetPopup()->AppendItems(*pMenu);

		tint32 iValueCur = mpPlugIn->GetGlobalParm(giParam_ChIn, miSection);
		mpPlugIn->SetGlobalParm(giParam_ChIn, 100000, miSection);
		mpPlugIn->SetGlobalParm(giParam_ChIn, iValueCur, miSection);
	}

	// Force update of control
/*	const de::IParameter* pParameter = GetGUI()->GetParmMan()->Get(giParam_ChIn, de::IParameterManager::TypeGlobal, miSection);
	if (pParameter) {
		tint32 iValueCur = pParameter->GetValue();
		GetGUI()->GetParmMan()->ControlUpdate(miPaneID, giChannel_Input_Popup + miCtrl_Offset, 100000, true);
		GetGUI()->GetParmMan()->ControlUpdate(miPaneID, giChannel_Input_Popup + miCtrl_Offset, iValueCur, true);
	}*/
/*	if (bUpdate) {
		tint32 iValueCur = mpPlugIn->GetGlobalParm(giParam_ChIn, miSection);
		mpPlugIn->SetGlobalParm(giParam_ChIn, 100000, miSection);
		mpPlugIn->SetGlobalParm(giParam_ChIn, iValueCur, miSection);
	}*/

	// Release lock
	miInPreparePopups_volatile--;
}

ge::IDropDownListBox* CMix_AUX_Insert::CreatePop( tint32 iControlID, 
														tint32 iBitmapID,  
														const ge::IPopupMenu::SMenuItemList& List, 
														ge::SPos Pos, 
														ge::SSize Size,
														ge::SRGB rRGB)
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
														   IDB_Font_Minix_Tight, 
														   iBitmapID, 
														   List, 
														   Pos, 
														   ge::SPos(0,0),
														   Size,
														   rRGB);
	//	pListBox->DontSetText();
	return pListBox;
}
