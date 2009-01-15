#include "KSOS.h"

CMix_Master_AUX_Insert::CMix_Master_AUX_Insert(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

}

CMix_Master_AUX_Insert::~CMix_Master_AUX_Insert()
{
}

void CMix_Master_AUX_Insert::SetInfo(tint32 iID, tint32 iTimerID)
{

}

void CMix_Master_AUX_Insert::Init()
{
	mpScrollPane = ge::IScrollPane::Create();
	mpScrollPane->Init(ge::IControl::giNoID, giCtr_Mix_Master_Aux_ScrollPane_Timer);
	mpPane = dynamic_cast<ge::IPane*>(mpScrollPane);
	mpPane->SetSize(ge::SSize(gTrack_Aux_Insert.iCX , gTrack_Aux_Insert.iCY));
	mpPane->SetBackgroundColour(ge::SRGB(5,242,242));
	
	
	// Back Drop
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Mix_Master_AUX_Insert, ge::SPos(0, 0));
	

	// Echo
	CreateKnob(giCtrl_Master_AUX1_Return, IDB_Dial_Aux, ge::SPos(17, 20), 129);
	// Reverb
	CreateKnob(giCtrl_Master_AUX2_Return, IDB_Dial_Aux, ge::SPos(17, 84), 129);

	CreateButton(giCtrl_Master_Edit_Insert1, IDB_Button_Edit_Plugin, ge::SPos(116, 48));
	CreateButton(giCtrl_Master_Edit_Insert2, IDB_Button_Edit_Plugin, ge::SPos(116, 48 + 16));
	CreateButton(giCtrl_Master_Edit_Insert3, IDB_Button_Edit_Plugin, ge::SPos(116, 48 + 16 * 2));
	CreateButton(giCtrl_Master_Edit_Insert4, IDB_Button_Edit_Plugin, ge::SPos(116, 48 + 16 * 3));

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
	
	CreatePop(giCtrl_Master_Insert_Pop1, IDB_Button_Invisible_16_64, List, ge::SPos(64, 48), ge::SSize(61, 15), ge::SRGB(204,204,204));
	CreatePop(giCtrl_Master_Insert_Pop2, IDB_Button_Invisible_16_64, List, ge::SPos(64, 48 + 16), ge::SSize(61, 15), ge::SRGB(204,204,204));
	CreatePop(giCtrl_Master_Insert_Pop3, IDB_Button_Invisible_16_64, List, ge::SPos(64, 48 + 16 * 2), ge::SSize(61, 15), ge::SRGB(204,204,204));
	CreatePop(giCtrl_Master_Insert_Pop4, IDB_Button_Invisible_16_64, List, ge::SPos(64, 48 + 16 * 3), ge::SSize(61, 15), ge::SRGB(204,204,204));
}


void CMix_Master_AUX_Insert::ConnectControls()
{
	RegisterGlobalControl(giSection_Master, giCtrl_Master_Insert_Pop1, giParam_Master_Insert1);
	RegisterGlobalControl(giSection_Master, giCtrl_Master_Insert_Pop2, giParam_Master_Insert2);
	RegisterGlobalControl(giSection_Master, giCtrl_Master_Insert_Pop3, giParam_Master_Insert3);
	RegisterGlobalControl(giSection_Master, giCtrl_Master_Insert_Pop4, giParam_Master_Insert4);
	
	RegisterGlobalControlWithX3Translation(giSection_Master, giCtrl_Master_AUX1_Return, giParam_Master_AUX1_Return, 4);
	RegisterGlobalControlWithX3Translation(giSection_Master, giCtrl_Master_AUX2_Return, giParam_Master_AUX2_Return, 4);

}

void CMix_Master_AUX_Insert::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
	if (pControl->GetID() == giCtrl_Master_Edit_Insert1) {
//		GetParmMan()->Set(true, 1, giParam_Ch_Insert1GUIOpen, de::IParameterManager::TypeGlobal, miSection);
		gpApplication->GetPlugInManager()->OpenGUI(2048, 0);
	}
	else if (pControl->GetID() == giCtrl_Master_Edit_Insert2) {
//		GetParmMan()->Set(true, 1, giParam_Ch_Insert2GUIOpen, de::IParameterManager::TypeGlobal, miSection);
		gpApplication->GetPlugInManager()->OpenGUI(2048, 1);
	}
	else if (pControl->GetID() == giCtrl_Master_Edit_Insert3) {
//		GetParmMan()->Set(true, 1, giParam_Ch_Insert3GUIOpen, de::IParameterManager::TypeGlobal, miSection);
		gpApplication->GetPlugInManager()->OpenGUI(2048, 2);
	}
	else if (pControl->GetID() == giCtrl_Master_Edit_Insert4) {
//		GetParmMan()->Set(true, 1, giParam_Ch_Insert4GUIOpen, de::IParameterManager::TypeGlobal, miSection);
		gpApplication->GetPlugInManager()->OpenGUI(2048, 3);
	}
}

void CMix_Master_AUX_Insert::EventGeneric(ge::IControl* pControl, void* pEventData)
{
}


void CMix_Master_AUX_Insert::HandleMsg(SMsg* pMsg)
{
	switch(pMsg->iOpcode) {
		case Msg_Track_Data:{
			STrackData sData = *(STrackData*)pMsg->pDataIn;
			switch( sData.iParam_ID){
				case giParam_Master_Show_Inserts: {
					
					sData.iVal ?  SlideToInsertsView() : SlideToIOView() ;
					
					break;
				}
			default: break;
			}
		}
		break;  
	}


}


void CMix_Master_AUX_Insert::Scroll(tint32 iXLeft)
{
	mpScrollPane->SlideTo(ge::SPos(iXLeft, 0), 0, 15, 3);
}


void CMix_Master_AUX_Insert::SlideToIOView()
{
		Scroll(0);
}

void CMix_Master_AUX_Insert::SlideToInsertsView()
{
		Scroll(gTrack_Aux_Insert.iCX);
}

ge::IDropDownListBox* CMix_Master_AUX_Insert::CreatePop( tint32 iControlID, 
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
