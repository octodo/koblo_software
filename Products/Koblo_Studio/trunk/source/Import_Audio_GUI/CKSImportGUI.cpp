
#include "KSOS.h"


CKSImportGUI::CKSImportGUI(CKSPlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CKSBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan),
	CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan)
{
}

CKSImportGUI::~CKSImportGUI()
{
}

void CKSImportGUI::Destroy()
{
	gpApplication->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CKSImportGUI*>(this);
}

void CKSImportGUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	*piSizeX = 424;
	*piSizeY = 424;
}

void CKSImportGUI::Initialize()
{
	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CKSImportGUIPane(NULL, dynamic_cast<CBaseGUI*>(this));
	pPane->Init();
	SetPane(pPane);
	// This is the top pane, so set it in window
	mpWindow->SetPane(pPane->GetPane());

	ConnectControls();

//#ifdef WIN32
	mpContext->SetMinWindowSize(mpWindow, ge::SSize(424,424));
	mpContext->SetMaxWindowSize(mpWindow, ge::SSize(424,424));
//#endif // WIN32


}

void CKSImportGUI::ConnectControls()
{
	dynamic_cast <CKSImportGUIPane*>( GetPane())->ConnectControls();

}

/*
	IEventReceiver
*/

void CKSImportGUI::EventValueChange(ge::IControl* pSender, tint32 iValueNew)
{
}

void CKSImportGUI::EventGeneric(ge::IControl* pControl, void *pEventData)
{
}

void CKSImportGUI::GetName(tchar* psz)
{
	strcpy((char*)psz, gpApplication->GetProductName().c_str());
}

void CKSImportGUI::OnTimer()
{
	CBaseGUI::OnTimer();
}

void CKSImportGUI::OnTimer(tint32 iTimerID)
{
/*
	CKSImportGUIPane* pPane = dynamic_cast <CKSImportGUIPane*>( GetPane());
	if (pPane) {
		pPane->OnTimer(iTimerID);
	}
*/
}

void CKSImportGUI::UpdateGUIData(tint32 iID, tint32 iValue)
{
//	dynamic_cast <CKSImportGUIPane*>( GetPane())->UpdateGUIData( iID,  iValue);
}
