
#include "KSOS.h"


CKSSetupGUI::CKSSetupGUI(CKSPlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CKSBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan),
	CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan)
{
}

CKSSetupGUI::~CKSSetupGUI()
{
}

void CKSSetupGUI::Destroy()
{
	gpApplication->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CKSSetupGUI*>(this);
}

void CKSSetupGUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	*piSizeX = 320;
	*piSizeY = 245;
}

void CKSSetupGUI::Initialize()
{
	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CKSSetupGUIPane(NULL, dynamic_cast<CBaseGUI*>(this));
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

void CKSSetupGUI::ConnectControls()
{
	dynamic_cast <CKSSetupGUIPane*>( GetPane())->ConnectControls();

}

/*
	IEventReceiver
*/

void CKSSetupGUI::EventValueChange(ge::IControl* pSender, tint32 iValueNew)
{
}

void CKSSetupGUI::EventGeneric(ge::IControl* pControl, void *pEventData)
{
}

void CKSSetupGUI::GetName(tchar* psz)
{
	strcpy((char*)psz, gpApplication->GetProductName().c_str());
}

void CKSSetupGUI::OnTimer()
{
	CBaseGUI::OnTimer();
}

void CKSSetupGUI::OnTimer(tint32 iTimerID)
{
/*
	CKSSetupGUIPane* pPane = dynamic_cast <CKSSetupGUIPane*>( GetPane());
	if (pPane) {
		pPane->OnTimer(iTimerID);
	}
*/
}

void CKSSetupGUI::UpdateGUIData(tint32 iID, tint32 iValue)
{
//	dynamic_cast <CKSSetupGUIPane*>( GetPane())->UpdateGUIData( iID,  iValue);
}
