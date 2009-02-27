#include "KSOS.h"

CTrack_AUX_Insert::CTrack_AUX_Insert(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

}

CTrack_AUX_Insert::~CTrack_AUX_Insert()
{
}

void CTrack_AUX_Insert::SetInfo(tint32 iID, tint32 iTimerID)
{
	miID = iID;
	miTimerID = iTimerID;
	miCtrl_Offset	= giFirst_Track_Ctrl + ( miID * giNr_Of_Track_Ctls);
	miSection		= iID + giSection_First_Track;
	
	

}

void CTrack_AUX_Insert::Init()
{
	
	
	mpScrollPane = ge::IScrollPane::Create();
	mpScrollPane->Init(ge::IControl::giNoID, miTimerID);
	mpPane = dynamic_cast<ge::IPane*>(mpScrollPane);
	mpPane->SetSize(ge::SSize(gTrack_Aux_Insert.iCX , gTrack_Aux_Insert.iCY));
//	mpPane->SetBackgroundColour(ge::SRGB(5,242,242));
	
	// Back Drop
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Track_Info_Insert, ge::SPos(gTrack_Aux_Insert.iCX, 0));
	
	
	// Echo and Reverb send
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Track_Info_AUX, ge::SPos(0, 0));
	// Echo
	CreateKnob(giCtrl_Track_Big_AUX1_Send + miCtrl_Offset, IDB_Dial_Aux, ge::SPos(17, 20), 129);
	// Reverb
	CreateKnob(giCtrl_Track_Big_AUX2_Send + miCtrl_Offset, IDB_Dial_Aux, ge::SPos(17, 84), 129);
	
	
	// Channel In
	PreparePopups();
	CreatePop(giTrack_Info_Big_Input_Popup+ miCtrl_Offset, IDB_Button_Invisible_16_64, mpInput_Menu, ge::SPos(68, 16),ge::SSize(18, 12), ge::SRGB(0,255,0));
	
	

	CreateButton(miCtrl_Offset + giCtrl_Trak_Edit_Insert1, IDB_Button_Edit_Plugin, ge::SPos(116, 48));
	CreateButton(miCtrl_Offset + giCtrl_Trak_Edit_Insert2, IDB_Button_Edit_Plugin, ge::SPos(116, 48 + 16));
	CreateButton(miCtrl_Offset + giCtrl_Trak_Edit_Insert3, IDB_Button_Edit_Plugin, ge::SPos(116, 48 + 16 * 2));
	CreateButton(miCtrl_Offset + giCtrl_Trak_Edit_Insert4, IDB_Button_Edit_Plugin, ge::SPos(116, 48 + 16 * 3));

	// Inserts
	tint32 piIDs[ge::IPopupMenu::BitmapCount];
	piIDs[ge::IPopupMenu::BitmapFolderArrow]	= IDB_Button_Popup;
	piIDs[ge::IPopupMenu::BitmapTop]			= IDB_PopupBorder;
	piIDs[ge::IPopupMenu::BitmapCenter]			= IDB_POPUP_BODY;
	piIDs[ge::IPopupMenu::BitmapCenterInv]		= IDB_POPUP_BODY_INVERTED;
	piIDs[ge::IPopupMenu::BitmapBottom]			= IDB_PopupBorder;
	
	
	CPlugInManager* pPlugManager	= gpApplication->GetPlugInManager();
	tint32 iPlugInCount				= pPlugManager->GetNrOfPlugIns();
	ge::IPopupMenu::SMenuItemList List;
	List.iItemCount = iPlugInCount + 1;
	tint32 iPlugIn;
	for (iPlugIn = 0; iPlugIn < iPlugInCount; iPlugIn++) {
		CPlugInManager::SPlugInInfo* pPlugInInfo = pPlugManager->GetPlugInInfo(iPlugIn);
		
		
		std::string s  = pPlugInInfo->sProductName;
		tint32* pi = new tint32[2];
		pi[0] = pPlugInInfo->uiCompanyID;
		pi[1] = pPlugInInfo->uiProductID;
		List.pItems[iPlugIn + 1] = ge::IPopupMenu::SMenuItem(s.c_str(), iPlugIn + 1, NULL, NULL, -1, 0, (void*)pi);
	}
	tint32* pi = new tint32[2];
	pi[0] = 0;
	pi[1] = 0;
	List.pItems[0] = ge::IPopupMenu::SMenuItem("None", 0, NULL, NULL, -1, 0, (void*)pi);
	
	
/*
	CPlugInManager* pPlugManager = gpApplication->GetPlugInManager();
	tint32 iPlugInCount = pPlugManager->GetNrOfPlugIns();
	ge::IPopupMenu::SMenuItemList List;
	List.iItemCount = iPlugInCount + 1;
	tint32 iPlugIn;
	for (iPlugIn = 0; iPlugIn < iPlugInCount; iPlugIn++) {
		CPlugInManager::SPlugInInfo* pInfo = pPlugManager->GetPlugInInfo(iPlugIn);
		
		std::string s = pInfo->sProductName;
		tint32* pi = new tint32[2];
		pi[0] = pInfo->uiCompanyID;
		pi[1] = pInfo->uiProductID;
		List.pItems[iPlugIn + 1] = ge::IPopupMenu::SMenuItem(s.c_str(), iPlugIn + 1, NULL, NULL, -1, 0, (void*)pi);
	}
	tint32* pi = new tint32[2];
	pi[0] = 0;
	pi[1] = 0;
	List.pItems[0] = ge::IPopupMenu::SMenuItem("None", 0, NULL, NULL, -1, 0, (void*)pi);
*/	
	CreatePop(giCtrl_Track_Insert_Pop1 + miCtrl_Offset, IDB_Button_Invisible_16_64, List, ge::SPos(2 + 64, 48), ge::SSize(61, 15), ge::SRGB(204,204,204));
	CreatePop(giCtrl_Track_Insert_Pop2 + miCtrl_Offset, IDB_Button_Invisible_16_64, List, ge::SPos(2 + 64, 48+ 16), ge::SSize(61, 15), ge::SRGB(204,204,204));
	CreatePop(giCtrl_Track_Insert_Pop3 + miCtrl_Offset, IDB_Button_Invisible_16_64, List, ge::SPos(2 + 64, 48 + 16 * 2), ge::SSize(61, 15), ge::SRGB(204,204,204));
	CreatePop(giCtrl_Track_Insert_Pop4 + miCtrl_Offset, IDB_Button_Invisible_16_64, List, ge::SPos(2 + 64, 48 + 16 * 3), ge::SSize(61, 15), ge::SRGB(204,204,204));
}



void CTrack_AUX_Insert::ConnectControls()
{
	RegisterGlobalControl(miSection, giTrack_Info_Big_Input_Popup + miCtrl_Offset , giParam_ChIn);
	
	RegisterGlobalControlWithX3Translation(miSection, giCtrl_Track_Big_AUX1_Send + miCtrl_Offset, giParam_ChAUX1, 4);
	RegisterGlobalControlWithX3Translation(miSection, giCtrl_Track_Big_AUX2_Send + miCtrl_Offset, giParam_ChAUX2, 4);

	RegisterGlobalControl(miSection, giCtrl_Track_Insert_Pop1 + miCtrl_Offset, giParam_ChInsert1);
	RegisterGlobalControl(miSection, giCtrl_Track_Insert_Pop2 + miCtrl_Offset, giParam_ChInsert2);
	RegisterGlobalControl(miSection, giCtrl_Track_Insert_Pop3 + miCtrl_Offset, giParam_ChInsert3);
	RegisterGlobalControl(miSection, giCtrl_Track_Insert_Pop4 + miCtrl_Offset, giParam_ChInsert4);
}

void CTrack_AUX_Insert::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	
	if (pControl->GetID() == giCtrl_Track_Insert_Pop1 + miCtrl_Offset ||
		pControl->GetID() == giCtrl_Track_Insert_Pop2 + miCtrl_Offset ||
		pControl->GetID() == giCtrl_Track_Insert_Pop3 + miCtrl_Offset ||
		pControl->GetID() == giCtrl_Track_Insert_Pop4 + miCtrl_Offset) {
		if (iValueNew != 0) {
			
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
	
	tint iCrl_ID = pControl->GetID();
	
	GetParmMan()->ControlUpdate(miPaneID, iCrl_ID, iValueNew);
	
	if (pControl->GetID() == miCtrl_Offset + giCtrl_Trak_Edit_Insert1) {

		gpApplication->GetPlugInManager()->OpenGUI(miID, 0);
	}
	else if (pControl->GetID() == miCtrl_Offset + giCtrl_Trak_Edit_Insert2) {

		gpApplication->GetPlugInManager()->OpenGUI(miID, 1);
	}
	else if (pControl->GetID() == miCtrl_Offset + giCtrl_Trak_Edit_Insert3) {

		gpApplication->GetPlugInManager()->OpenGUI(miID, 2);
	}
	else if (pControl->GetID() == miCtrl_Offset + giCtrl_Trak_Edit_Insert4) {

		gpApplication->GetPlugInManager()->OpenGUI(miID, 3);
	}
}

void CTrack_AUX_Insert::EventGeneric(ge::IControl* pControl, void* pEventData)
{
}


void CTrack_AUX_Insert::HandleMsg(SMsg* pMsg)
{
	switch(pMsg->iOpcode) {
		case Msg_Track_Data:{
			STrackData sData = *(STrackData*)pMsg->pDataIn;
			switch( sData.iParam_ID){
				case giParam_Track_Info_Show_Inserts: {
					if( miID == sData.iTrack){
						sData.iVal ?  SlideToInsertsView() : SlideToIOView() ;
					}
					break;
				}
			default: break;
			}
		}
		break;  

		case MsgInsertPopup:
			{
				tint32 iPaneID = ((tint32*)pMsg->pDataIn)[0];
				tint32 iControlID = ((tint32*)pMsg->pDataIn)[1];
				tint32 iValue = ((tint32*)pMsg->pDataIn)[2];
				if (iPaneID == GetPaneID()) {
					if (iControlID == giCtrl_Track_Insert_Pop1 + miCtrl_Offset ||
						iControlID == giCtrl_Track_Insert_Pop2 + miCtrl_Offset ||
						iControlID == giCtrl_Track_Insert_Pop3 + miCtrl_Offset ||
						iControlID == giCtrl_Track_Insert_Pop4 + miCtrl_Offset) {
							ge::IControl* pControl = GetWindow()->GetControl(iControlID);
							ge::IPopupMenu* pPopup = dynamic_cast<ge::IDropDownListBox*>(pControl)->GetPopup();
							tint32 iItems = pPopup->GetItems();
							tint32 iItem;
							for (iItem = 0; iItem < iItems; iItem++) {
								tint32* pi = (tint32*)(pPopup->GetData(iItem));
								if (pi) {
									tint32 iCompanyID = pi[0];
									tint32 iProductID = pi[1];
									tint32 iValuePop = iProductID;
									if (iCompanyID != 2) {
										iValuePop = (iCompanyID << 8) | iProductID;
									}

									if (iValue == iValuePop) {
										((tint32*)pMsg->pDataIn)[2] = iItem;
										break;
									}
								}
							}
					}
				}
			}
			break;
	}

}

void CTrack_AUX_Insert::Scroll(tint32 iXLeft)
{
	mpScrollPane->SlideTo(ge::SPos(iXLeft, 0), 0, 15, 3);
}


void CTrack_AUX_Insert::SlideToIOView()
{
		Scroll(0);
}

void CTrack_AUX_Insert::SlideToInsertsView()
{
		Scroll(gTrack_Aux_Insert.iCX);
}

void CTrack_AUX_Insert::PreparePopups()
{
	//--------------------------------
	// Input Popup
	//--------------------------------
	mpInput_Menu.iItemCount = 2 ;	
	mpInput_Menu.pItems[0] = ge::IPopupMenu::SMenuItem("1-2", 0 ,	NULL);
	mpInput_Menu.pItems[1] = ge::IPopupMenu::SMenuItem("3-4", 1 ,	NULL);

			
}
/*
ge::IDropDownListBox* CTrack_AUX_Insert::CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos, ge::SSize Size)
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
															List, 
															Pos, 
															ge::SPos(0,0),
															Size,
															ge::SRGB(0,255,0));
//	pListBox->DontSetText();
	return pListBox;
}
*/
ge::IDropDownListBox* CTrack_AUX_Insert::CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos, ge::SSize Size, ge::SRGB sRGB)
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
															sRGB);
//	pListBox->DontSetText();
	return pListBox;
}