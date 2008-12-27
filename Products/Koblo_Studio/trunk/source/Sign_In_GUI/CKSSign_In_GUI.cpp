
#include "KSOS.h"


CKSSign_In_GUI::CKSSign_In_GUI(CKSPlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CKSBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan),
	CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan)
{
}

CKSSign_In_GUI::~CKSSign_In_GUI()
{
}

void CKSSign_In_GUI::Destroy()
{
	GetPlugIn()->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CKSSign_In_GUI*>(this);
}

void CKSSign_In_GUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	*piSizeX = 320;
	*piSizeY = 245;
}

void CKSSign_In_GUI::Initialize()
{
	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CKSSign_In_GUIPane(NULL, dynamic_cast<CBaseGUI*>(this));
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

void CKSSign_In_GUI::ConnectControls()
{
	dynamic_cast <CKSSign_In_GUIPane*>( GetPane())->ConnectControls();

}

/*
	IEventReceiver
*/

void CKSSign_In_GUI::EventValueChange(ge::IControl* pSender, tint32 iValueNew)
{
}

void CKSSign_In_GUI::EventGeneric(ge::IControl* pControl, void *pEventData)
{
}

void CKSSign_In_GUI::GetName(tchar* psz)
{
	strcpy((char*)psz, GetPlugIn()->GetProductName().c_str());
}

void CKSSign_In_GUI::OnTimer()
{
	CBaseGUI::OnTimer();
}

void CKSSign_In_GUI::OnTimer(tint32 iTimerID)
{
/*
	CKSSign_In_GUIPane* pPane = dynamic_cast <CKSSign_In_GUIPane*>( GetPane());
	if (pPane) {
		pPane->OnTimer(iTimerID);
	}
*/
}

void CKSSign_In_GUI::UpdateGUIData(tint32 iID, tint32 iValue)
{
//	dynamic_cast <CKSSign_In_GUIPane*>( GetPane())->UpdateGUIData( iID,  iValue);
}
