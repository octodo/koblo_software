
#include "KSOS.h"


CKSExportForWebGUI::CKSExportForWebGUI( de::IParameterManager* pParmMan)
	: CKSBaseGUI( pParmMan),
	CBaseGUI(dynamic_cast<CBasePlugIn*>(gpApplication), pParmMan)
{
}

CKSExportForWebGUI::~CKSExportForWebGUI()
{
}

void CKSExportForWebGUI::Destroy()
{
	gpApplication->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CKSExportForWebGUI*>(this);
}

void CKSExportForWebGUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	*piSizeX = 541;
	*piSizeY = 477;
}

void CKSExportForWebGUI::Initialize()
{
	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CKSExportForWebGUIPane(NULL, dynamic_cast<CBaseGUI*>(this));
	pPane->Init();
	SetPane(pPane);
	// This is the top pane, so set it in window
	mpWindow->SetPane(pPane->GetPane());

	ConnectControls();

//#ifdef WIN32
	mpContext->SetMinWindowSize(mpWindow, ge::SSize(541,477));
	mpContext->SetMaxWindowSize(mpWindow, ge::SSize(541,477));
//#endif // WIN32


}

void CKSExportForWebGUI::ConnectControls()
{
	dynamic_cast <CKSExportForWebGUIPane*>( GetPane())->ConnectControls();

}

/*
	IEventReceiver
*/

void CKSExportForWebGUI::EventValueChange(ge::IControl* pSender, tint32 iValueNew)
{
}

void CKSExportForWebGUI::EventGeneric(ge::IControl* pControl, void *pEventData)
{
}

void CKSExportForWebGUI::GetName(tchar* psz)
{
	strcpy((char*)psz, gpApplication->GetProductName().c_str());
}

void CKSExportForWebGUI::OnTimer()
{
	CBaseGUI::OnTimer();
}

void CKSExportForWebGUI::OnTimer(tint32 iTimerID)
{
/*
	CKSExportForWebGUIPane* pPane = dynamic_cast <CKSExportForWebGUIPane*>( GetPane());
	if (pPane) {
		pPane->OnTimer(iTimerID);
	}
*/
}

void CKSExportForWebGUI::UpdateGUIData(tint32 iID, tint32 iValue)
{
//	dynamic_cast <CKSExportForWebGUIPane*>( GetPane())->UpdateGUIData( iID,  iValue);
}
