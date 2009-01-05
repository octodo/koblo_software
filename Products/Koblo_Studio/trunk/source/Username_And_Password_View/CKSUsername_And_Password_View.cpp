
#include "KSOS.h"


CKSUsername_And_Password_View::CKSUsername_And_Password_View(CKSPlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CKSBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan),
	CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan)
{
}

CKSUsername_And_Password_View::~CKSUsername_And_Password_View()
{
}

void CKSUsername_And_Password_View::Destroy()
{
	GetPlugIn()->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CKSUsername_And_Password_View*>(this);
}

void CKSUsername_And_Password_View::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	*piSizeX = 320;
	*piSizeY = 245;
}

void CKSUsername_And_Password_View::Initialize()
{
	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CKSUsername_And_Password_Pane(NULL, dynamic_cast<CBaseGUI*>(this));
	pPane->Init();
	SetPane(pPane);
	// This is the top pane, so set it in window
	mpWindow->SetPane(pPane->GetPane());

	ConnectControls();

//#ifdef WIN32
	mpContext->SetMinWindowSize(mpWindow, ge::SSize(320,245));
	mpContext->SetMaxWindowSize(mpWindow, ge::SSize(230,245));
//#endif // WIN32


}

void CKSUsername_And_Password_View::ConnectControls()
{
	dynamic_cast <CKSUsername_And_Password_Pane*>( GetPane())->ConnectControls();

}

/*
	IEventReceiver
*/

void CKSUsername_And_Password_View::EventValueChange(ge::IControl* pSender, tint32 iValueNew)
{
}

void CKSUsername_And_Password_View::EventGeneric(ge::IControl* pControl, void *pEventData)
{
}

void CKSUsername_And_Password_View::GetName(tchar* psz)
{
	strcpy((char*)psz, GetPlugIn()->GetProductName().c_str());
}

void CKSUsername_And_Password_View::OnTimer()
{
	CBaseGUI::OnTimer();
}

void CKSUsername_And_Password_View::OnTimer(tint32 iTimerID)
{
/*
	CKSUsername_And_Password_ViewPane* pPane = dynamic_cast <CKSUsername_And_Password_ViewPane*>( GetPane());
	if (pPane) {
		pPane->OnTimer(iTimerID);
	}
*/
}

void CKSUsername_And_Password_View::UpdateGUIData(tint32 iID, tint32 iValue)
{
//	dynamic_cast <CKSUsername_And_Password_ViewPane*>( GetPane())->UpdateGUIData( iID,  iValue);
}
