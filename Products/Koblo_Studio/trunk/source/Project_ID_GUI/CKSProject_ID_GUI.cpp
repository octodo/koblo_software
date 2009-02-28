
#include "KSOS.h"


CKSProject_ID_GUI::CKSProject_ID_GUI( de::IParameterManager* pParmMan)
	: CKSBaseGUI( pParmMan),
	CBaseGUI(dynamic_cast<CBasePlugIn*>(gpApplication), pParmMan)
{
}

CKSProject_ID_GUI::~CKSProject_ID_GUI()
{
}

void CKSProject_ID_GUI::Destroy()
{
	gpApplication->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CKSProject_ID_GUI*>(this);
}

void CKSProject_ID_GUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	*piSizeX = 345;
	*piSizeY = 103;
}

void CKSProject_ID_GUI::Initialize()
{
	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CKSProject_ID_GUI_Pane* pPane = new CKSProject_ID_GUI_Pane(NULL, dynamic_cast<CBaseGUI*>(this));
	pPane->Init();
	SetPane(pPane);
	// This is the top pane, so set it in window
	mpWindow->SetPane(pPane->GetPane());

	ConnectControls();

//#ifdef WIN32
	mpContext->SetMinWindowSize(mpWindow, ge::SSize(345,103));
	mpContext->SetMaxWindowSize(mpWindow, ge::SSize(345,103));
//#endif // WIN32


}

void CKSProject_ID_GUI::ConnectControls()
{
	dynamic_cast <CKSProject_ID_GUI_Pane*>( GetPane())->ConnectControls();
	

}

/*
	IEventReceiver
*/

void CKSProject_ID_GUI::EventValueChange(ge::IControl* pSender, tint32 iValueNew)
{
}

void CKSProject_ID_GUI::EventGeneric(ge::IControl* pControl, void *pEventData)
{
}

void CKSProject_ID_GUI::GetName(tchar* psz)
{
	strcpy((char*)psz, gpApplication->GetProductName().c_str());
}

void CKSProject_ID_GUI::OnTimer()
{
	CBaseGUI::OnTimer();
}

void CKSProject_ID_GUI::OnTimer(tint32 iTimerID)
{
/*
	CKSProject_ID_GUI_Pane* pPane = dynamic_cast <CKSProject_ID_GUI_Pane*>( GetPane());
	if (pPane) {
		pPane->OnTimer(iTimerID);
	}
*/
}

void CKSProject_ID_GUI::UpdateGUIData(tint32 iID, tint32 iValue)
{
//	dynamic_cast <CKSProject_ID_GUI_PaneCKSProject_ID_GUIPane*>( GetPane())->UpdateGUIData( iID,  iValue);
}
