
#include "KSOS.h"


CKSExportGUI::CKSExportGUI(CKSPlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CKSBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan),
	CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan)
{
}

CKSExportGUI::~CKSExportGUI()
{
}

void CKSExportGUI::Destroy()
{
	gpApplication->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CKSExportGUI*>(this);
}

void CKSExportGUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	*piSizeX = 386;
	*piSizeY = 242;
}

void CKSExportGUI::Initialize()
{
	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CKSExportGUIPane(NULL, dynamic_cast<CBaseGUI*>(this));
	pPane->Init();
	SetPane(pPane);
	// This is the top pane, so set it in window
	mpWindow->SetPane(pPane->GetPane());

	ConnectControls();

//#ifdef WIN32
	mpContext->SetMinWindowSize(mpWindow, ge::SSize(386,242));
	mpContext->SetMaxWindowSize(mpWindow, ge::SSize(386,242));
//#endif // WIN32


}

void CKSExportGUI::ConnectControls()
{
	dynamic_cast <CKSExportGUIPane*>( GetPane())->ConnectControls();

}

/*
	IEventReceiver
*/

void CKSExportGUI::EventValueChange(ge::IControl* pSender, tint32 iValueNew)
{
}

void CKSExportGUI::EventGeneric(ge::IControl* pControl, void *pEventData)
{
}

void CKSExportGUI::GetName(tchar* psz)
{
	strcpy((char*)psz, gpApplication->GetProductName().c_str());
}

void CKSExportGUI::OnTimer()
{
	CBaseGUI::OnTimer();
}

void CKSExportGUI::OnTimer(tint32 iTimerID)
{
/*
	CKSExportGUIPane* pPane = dynamic_cast <CKSExportGUIPane*>( GetPane());
	if (pPane) {
		pPane->OnTimer(iTimerID);
	}
*/
}

void CKSExportGUI::UpdateGUIData(tint32 iID, tint32 iValue)
{
//	dynamic_cast <CKSExportGUIPane*>( GetPane())->UpdateGUIData( iID,  iValue);
}
