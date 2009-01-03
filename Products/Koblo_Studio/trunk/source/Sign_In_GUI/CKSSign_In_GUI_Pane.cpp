
#include "KSOS.h"


CKSSign_In_GUI_Pane::CKSSign_In_GUI_Pane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
//	CBaseDSPEngine* pD = GetPlugIn()->GetDSPEngine();
//	mpDSP = dynamic_cast<CKSDSP*>(pD);
	
}

CKSSign_In_GUI_Pane::~CKSSign_In_GUI_Pane()
{
	
}

void CKSSign_In_GUI_Pane::Init()
{
	mpKSPlugIn = dynamic_cast<CKSPlugIn*>(GetPlugIn());
	
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(320,245));
	
	tint32 iPosY = 0;
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Sign_In, ge::SPos(0, 0)); iPosY += 60;
	
	
	
	
	
	// user name
	mpUser_Name_Txt = CreateDisplay(giCtrl_Sign_In_User_Name_Display, 
									ge::SPos(23, 32),  
									ge::SSize(197, 16), 
									CreateFont(Generic128, IDF_Fuxley_712, ge::SRGB(10, 10, 10)),
									true);
	
	mpUser_Name_Txt->SetHorzAlignment(ge::IText::HorzAlignLeft);
	mpUser_Name_Txt->SetClicksOutsideAsSuccess();
	
	
	// password
	mpPassword_Txt = CreateDisplay(giCtrl_Sign_In_Password_Display, 
									ge::SPos(23, 32+42),  
									ge::SSize(197, 16), 
									CreateFont(Generic128, IDF_Fuxley_712, ge::SRGB(10, 10, 10)),
									true);
	
	mpPassword_Txt->SetHorzAlignment(ge::IText::HorzAlignLeft);
	mpPassword_Txt->SetClicksOutsideAsSuccess();
	mpPassword_Txt->SetPassword();
	
	
	
	
	
	
	
	
	// remember username and password
	mpRemember_Me_Button = Create2StateButton(giCtrl_Sign_In_Remember_Me, IDB_Button_Remember_Me, ge::SPos(23, 105), false);
	
	// cancel button
	CreateButton(giCtrl_Sign_In_Cancel, IDB_Button_Cancel, ge::SPos(119, 149), false);
	
	// ok button
	CreateButton(giCtrl_Sign_In_OK, IDB_Button_OK, ge::SPos(180, 149), false);
	
	
	
	
}

void CKSSign_In_GUI_Pane::ConnectControls()
{
//	RegisterGlobalControl(giSectionGUI, giCtrl_Export_Selection,   giParamID_Export_Type);
	
//	RegisterGlobalControlWithDisplayTranslater(giSectionGUI, giCtrl_Export_Tail, giParamID_Export_Tail, 0, 6, 0, "  m.Sec");
	
//	RegisterGlobalControl(giSectionGUI, giCtrl_Sign_In_Remember_Me,   giParamID_Remember_Username_and_Password);
	
}

void CKSSign_In_GUI_Pane::UpdateGUIData(tint32 iID, tint32 iValue)
{
}

void CKSSign_In_GUI_Pane::OnTimer(tint32 iTimerID)
{
}


void CKSSign_In_GUI_Pane::SendMsgFromTop(SMsg* pMsg)
{
	switch(pMsg->iOpcode) 
	{
		case MsgSign_Out:
		{	// delete values
			mpRemember_Me_Button->SetValue(0);
			mpUser_Name_Txt->SetText("");
			mpPassword_Txt->SetText("");
			break;
		}
	}
}


void CKSSign_In_GUI_Pane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{

	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
	
	switch (pControl->GetID()) {
		// cancel	
		case giCtrl_Sign_In_Cancel:{
			// close sign in window
			GetPlugIn()->SetGlobalParm(giParamID_Show_Sign_In_Window, 0, giSectionGUI);
			// restore remember me
			mpRemember_Me_Button->SetValue(mpKSPlugIn->Get_Remember_Me());
			// restore username text
			tchar pszTxt[1024];
			sprintf(pszTxt, mpKSPlugIn->Get_User_Name().c_str());
			mpUser_Name_Txt->SetText(pszTxt);
			// restore password text
			sprintf(pszTxt, mpKSPlugIn->Get_Password().c_str());
			mpPassword_Txt->SetText(pszTxt);
			
			break;
		}
		// ok	
		case giCtrl_Sign_In_OK:{
			// close sign in window
			GetPlugIn()->SetGlobalParm(giParamID_Show_Sign_In_Window, 0, giSectionGUI);
			// set remember me
			mpKSPlugIn->Set_Remember_Me( mpRemember_Me_Button->GetValue());
			// user name
			tchar pszTxt[1024];
			mpUser_Name_Txt->GetText(pszTxt);
			mpKSPlugIn->Set_User_Name(pszTxt);
			// password
			mpPassword_Txt->GetText(pszTxt);
			mpKSPlugIn->Set_Password(pszTxt);
			// sign in
			mpKSPlugIn->Sign_In();
			// save sign in info
			mpKSPlugIn->Save_User_Name_And_Password();
			
			break;
		}
	}
}
