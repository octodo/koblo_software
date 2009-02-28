
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
	mpPane->SetSize(ge::SSize(345,103));
	
//	tint32 iPosY = 0;
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Project_ID, ge::SPos(0, 0));
	
	
	// user name
	mpProject_UUID_Txt = CreateDisplay(giCtrl_Projert_UUID, 
									ge::SPos(25, 35),  
									ge::SSize(300, 16), 
									CreateFont(Generic128, IDF_Font_Fuxley_712, ge::SRGB(10, 10, 10)),
									true);
	
	mpProject_UUID_Txt->SetHorzAlignment(ge::IText::HorzAlignLeft);
	mpProject_UUID_Txt->SetClicksOutsideAsSuccess();
	
	/*
	
	// Project ID
	ge::IText* pText = CreateDisplay( giCtrl_Project_ID_Display, ge::SPos(25, 35),  ge::SSize(300, 16), CreateFont(Generic128, IDB_Font_Axel_Tight, ge::SRGB(64, 64, 64)), true);
	pText->SetHorzAlignment(ge::IText::HorzAlignLeft);
	pText->SetClicksOutsideAsSuccess();
	 */

	
	CreateButton(giCtrl_Project_ID_Cancel, IDB_Button_Cancel, ge::SPos(219, 72), false);
	CreateButton(giCtrl_Project_ID_OK, IDB_Button_OK, ge::SPos(280, 72), false);
	
	
	mpDownload_Help	=	ge::IURL::Create();	
	mpDownload_Help->Init(ge::IControl::giNoID,(tchar*)"http://koblo.com/");
	mpDownload_Help->SetSize(ge::SSize(28, 72));
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpDownload_Help), ge::SPos(26, 48));
	
	

}

void CKSProject_ID_GUI_Pane::ConnectControls()
{
	//RegisterGlobalControlWithDisplayTranslater(giSectionGlobal, giCtrl_Project_ID_Display, giParamID_Project_ID, 0, 10, 0);
	
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
			
			// project id 
			tchar pszTxt[1024];
			sprintf(pszTxt, "Paste UUID from Koblo here");
			mpProject_UUID_Txt->SetText(pszTxt);
			
			//gpApplication->SetGlobalParm(giParamID_Show_Projec_ID_Window, 0, giSectionGUI);
			break;
		}
			
		case giCtrl_Project_ID_OK:{
			
			gpApplication->SetGlobalParm(giParamID_Show_Projec_ID_Window, 0, giSectionGUI);
	//		tint32 iProject_ID = gpApplication->GetGlobalParm(giParamID_Project_ID, giSectionGlobal);
			
			// user name
			tchar pszTxt[1024];
			mpProject_UUID_Txt->GetText(pszTxt);
			gpApplication->Read_Latest_Revision_From_Koblo(pszTxt);
			break;
		}
			
			
			
			
	
			
			/*
			// close sign in window
			gpApplication->SetGlobalParm(giParamID_Show_Sign_In_Window, 0, giSectionGUI);
			// restore remember me
			mpRemember_Me_Button->SetValue(gpApplication->Get_Remember_Me());
			// restore username text
			tchar pszTxt[1024];
			sprintf(pszTxt, gpApplication->Get_User_Name().c_str());
			mpUser_Name_Txt->SetText(pszTxt);
			// restore password text
			sprintf(pszTxt, gpApplication->Get_Password().c_str());
			mpPassword_Txt->SetText(pszTxt);
			
			*/
			
			
			

	}
 
}

void CKSProject_ID_GUI_Pane::UpdateGUIData(tint32 iID, tint32 iValue)
{
}

