
#include "KSOS.h"


CKSProject_ID_GUI_Pane::CKSProject_ID_GUI_Pane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)

{
//	CBaseDSPEngine* pD = GetPlugIn()->GetDSPEngine();
//	mpDSP = dynamic_cast<CKSDSP*>(pD);
}

CKSProject_ID_GUI_Pane::~CKSProject_ID_GUI_Pane()
{
	
}

void CKSProject_ID_GUI_Pane::Init()
{
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(245,103));
	
//	tint32 iPosY = 0;
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Project_ID, ge::SPos(0, 0));
	
	// Project ID
	ge::IText* pText = CreateDisplay( giCtrl_Project_ID_Display, ge::SPos(25, 35),  ge::SSize(200, 16), CreateFont(Generic128, IDB_Font_Axel_Tight, ge::SRGB(64, 64, 64)), true);
	pText->SetHorzAlignment(ge::IText::HorzAlignLeft);
	pText->SetClicksOutsideAsSuccess();

	
	CreateButton(giCtrl_Project_ID_Cancel, IDB_Button_Cancel, ge::SPos(119, 72), false);
	CreateButton(giCtrl_Project_ID_OK, IDB_Button_OK, ge::SPos(180, 72), false);
	
	

}

void CKSProject_ID_GUI_Pane::ConnectControls()
{
	RegisterGlobalControlWithDisplayTranslater(giSectionGlobal, giCtrl_Project_ID_Display, giParamID_Project_ID, 0, 10, 0);
	
}

void CKSProject_ID_GUI_Pane::OnTimer(tint32 iTimerID)
{
}

void CKSProject_ID_GUI_Pane::SendMsgFromTop(SMsg* pMsg)
{
	/*
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
	 */
}


void CKSProject_ID_GUI_Pane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{

	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
	
	switch (pControl->GetID()) {

		case giCtrl_Project_ID_Cancel:{
			
			gpApplication->SetGlobalParm(giParamID_Show_Projec_ID_Window, 0, giSectionGUI);
			break;
		}
			
		case giCtrl_Project_ID_OK:{
			
			gpApplication->SetGlobalParm(giParamID_Show_Projec_ID_Window, 0, giSectionGUI);
			tint32 iProject_ID = gpApplication->GetGlobalParm(giParamID_Project_ID, giSectionGlobal);
			gpApplication->Download_Project(iProject_ID);
			break;
		}

	}
 
}

void CKSProject_ID_GUI_Pane::UpdateGUIData(tint32 iID, tint32 iValue)
{
}

