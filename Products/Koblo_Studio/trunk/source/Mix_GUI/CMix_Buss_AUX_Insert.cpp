#include "KSOS.h"

CMix_Buss_AUX_Insert::CMix_Buss_AUX_Insert(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

}

CMix_Buss_AUX_Insert::~CMix_Buss_AUX_Insert()
{
}

void CMix_Buss_AUX_Insert::SetInfo(tint32 iID, tint32 iTimerID)
{
	miID			=	iID;
	miTimerID		= iTimerID;
	miCtrl_Offset	= giFirst_Buss_Ctrl + ( miID * giNr_Of_Buss_Ctls);
	
}

void CMix_Buss_AUX_Insert::Init()
{
	mpScrollPane = ge::IScrollPane::Create();
	//mpScrollPane->Init(ge::IControl::giNoID, giCtr_Mix_Buss_Aux_ScrollPane_Timer + miCtrl_Offset);
	mpScrollPane->Init(ge::IControl::giNoID, miTimerID);
	mpPane = dynamic_cast<ge::IPane*>(mpScrollPane);
	mpPane->SetSize(ge::SSize(gTrack_Aux_Insert.iCX , gTrack_Aux_Insert.iCY));
	mpPane->SetBackgroundColour(ge::SRGB(5,242,242));
	
	
	// Back Drop
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Mix_Buss_AUX_Insert, ge::SPos(0, 0));
	

	// Echo
	CreateKnob(giCtr_Mix_Buss_AUX1Send + miCtrl_Offset, IDB_Dial_Aux, ge::SPos(17, 20), 129);
	// Reverb
	CreateKnob(giCtr_Mix_Buss_AUX2Send + miCtrl_Offset, IDB_Dial_Aux, ge::SPos(17, 84), 129);

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

	CPlugInManager* pPlugManager = gpApplication->GetPlugInManager();
	tint32 iPlugInCount = pPlugManager->GetNrOfPlugIns();
	ge::IPopupMenu::SMenuItemList List;
	List.iItemCount = iPlugInCount + 1;
	tint32 iPlugIn;
	for (iPlugIn = 0; iPlugIn < iPlugInCount; iPlugIn++) {
		CPlugInManager::SPlugInInfo* pInfo = pPlugManager->GetPlugInInfo(iPlugIn);
		/*
		 std::string s = pInfo->sCompanyName;
		 s += std::string(": ");
		 s += pInfo->sProductName;
		 */
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
	
	CreatePop(giCtr_Mix_Buss_Edit_Insert1 + miCtrl_Offset, IDB_Button_Invisible_16_64, List, ge::SPos(64, 48), ge::SSize(61, 15),ge::SRGB(204,204,204));
	CreatePop(giCtr_Mix_Buss_Edit_Insert2 + miCtrl_Offset, IDB_Button_Invisible_16_64, List, ge::SPos(64, 48 + 16), ge::SSize(61, 15),ge::SRGB(204,204,204));
	CreatePop(giCtr_Mix_Buss_Edit_Insert3 + miCtrl_Offset, IDB_Button_Invisible_16_64, List, ge::SPos(64, 48 + 16 * 2), ge::SSize(61, 15),ge::SRGB(204,204,204));
	CreatePop(giCtr_Mix_Buss_Edit_Insert4 + miCtrl_Offset, IDB_Button_Invisible_16_64, List, ge::SPos(64, 48 + 16 * 3), ge::SSize(61, 15),ge::SRGB(204,204,204));
}


void CMix_Buss_AUX_Insert::ConnectControls()
{
	
	RegisterGlobalControlWithX3Translation(giSection_First_Buss + miID, giCtr_Mix_Buss_AUX1Send + miCtrl_Offset, giParam_Buss_AUX1, 4);
	RegisterGlobalControlWithX3Translation(giSection_First_Buss + miID, giCtr_Mix_Buss_AUX2Send + miCtrl_Offset, giParam_Buss_AUX2, 4);
	RegisterGlobalControl(giSection_First_Buss + miID, giCtr_Mix_Buss_Edit_Insert1 + miCtrl_Offset, giParam_Buss_Insert1);
	RegisterGlobalControl(giSection_First_Buss + miID, giCtr_Mix_Buss_Edit_Insert2 + miCtrl_Offset, giParam_Buss_Insert2);
	RegisterGlobalControl(giSection_First_Buss + miID, giCtr_Mix_Buss_Edit_Insert3 + miCtrl_Offset, giParam_Buss_Insert3);
	RegisterGlobalControl(giSection_First_Buss + miID, giCtr_Mix_Buss_Edit_Insert4 + miCtrl_Offset, giParam_Buss_Insert4);
	
	
}

void CMix_Buss_AUX_Insert::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	
	if (pControl->GetID() == giCtr_Mix_Buss_Insert_Pop1 + miCtrl_Offset ||
		pControl->GetID() == giCtr_Mix_Buss_Insert_Pop2 + miCtrl_Offset ||
		pControl->GetID() == giCtr_Mix_Buss_Insert_Pop3 + miCtrl_Offset ||
		pControl->GetID() == giCtr_Mix_Buss_Insert_Pop4 + miCtrl_Offset) {
		if (iValueNew != 0) {
			//			tint32 iInsert = pControl->GetID() - (giCtrl_Mix_Insert_Pop1 + miCtrl_Offset);
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
	
	if (pControl->GetID() == miCtrl_Offset + giCtr_Mix_Buss_Insert_Pop1) {

		gpApplication->GetPlugInManager()->OpenGUI(miID + 1024, 0);
	}
	else if (pControl->GetID() == miCtrl_Offset + giCtr_Mix_Buss_Insert_Pop2) {

		gpApplication->GetPlugInManager()->OpenGUI(miID + 1024, 1);
	}
	else if (pControl->GetID() == miCtrl_Offset + giCtr_Mix_Buss_Insert_Pop3) {

		gpApplication->GetPlugInManager()->OpenGUI(miID + 1024, 2);
	}
	else if (pControl->GetID() == miCtrl_Offset + giCtr_Mix_Buss_Insert_Pop4) {

		gpApplication->GetPlugInManager()->OpenGUI(miID + 1024, 3);
	}
}

void CMix_Buss_AUX_Insert::EventGeneric(ge::IControl* pControl, void* pEventData)
{
}


void CMix_Buss_AUX_Insert::HandleMsg(SMsg* pMsg)
{
	switch(pMsg->iOpcode) {
		case Msg_Buss_Data:{
			SBussData sData = *(SBussData*)pMsg->pDataIn;
			switch( sData.iParam_ID ){
				case giParam_Buss_Info_Show_Inserts: {
					if(sData.iBuss == miID)
						sData.iVal ?  SlideToInsertsView() : SlideToIOView() ;
					
					break;
				}
			default: break;
			}
		}
		break;  
	}


}


void CMix_Buss_AUX_Insert::Scroll(tint32 iXLeft)
{
	mpScrollPane->SlideTo(ge::SPos(iXLeft, 0), 0, 15, 3);
}


void CMix_Buss_AUX_Insert::SlideToIOView()
{
		Scroll(0);
}

void CMix_Buss_AUX_Insert::SlideToInsertsView()
{
		Scroll(gTrack_Aux_Insert.iCX);
}

ge::IDropDownListBox* CMix_Buss_AUX_Insert::CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos, ge::SSize Size, ge::SRGB sRGB)
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
