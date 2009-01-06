
#include "KSOS.h"

CKSPlugInGUI::CKSPlugInGUI(CKSPlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CKSBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan),
	CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan)
{
}

CKSPlugInGUI::~CKSPlugInGUI()
{
}

void CKSPlugInGUI::Destroy()
{
	gpApplication->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CKSPlugInGUI*>(this);
}

void CKSPlugInGUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	// More or less arbitrary numbers
	*piSizeX = 600;
	*piSizeY = 400;
}

void CKSPlugInGUI::Initialize()
{
	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CKSPlugInGUIPane(NULL, dynamic_cast<CBaseGUI*>(this));
	pPane->Init();
	SetPane(pPane);
	// This is the top pane, so set it in window
	mpWindow->SetPane(pPane->GetPane());

	ConnectControls();
}

void CKSPlugInGUI::ConnectControls()
{
	dynamic_cast <CKSPlugInGUIPane*>( GetPane())->ConnectControls();
}

void CKSPlugInGUI::SetGUI(kspi::IGUI* pGUI)
{
	dynamic_cast<CKSPlugInGUIPane*>(mpPane)->SetGUI(pGUI);
}

/*
	IEventReceiver
*/

void CKSPlugInGUI::EventValueChange(ge::IControl* pSender, tint32 iValueNew)
{
}

void CKSPlugInGUI::EventGeneric(ge::IControl* pControl, void *pEventData)
{
}

void CKSPlugInGUI::GetName(tchar* psz)
{
	strcpy((char*)psz, gpApplication->GetProductName().c_str());
}

void CKSPlugInGUI::OnTimer()
{
	CBaseGUI::OnTimer();
}

tbool CKSPlugInGUI::WindowClosed(void* pWnd)
{
	if (mhWndParent != pWnd) {
		return false;
	}

	dynamic_cast<CKSPlugInGUIPane*>(GetPane())->OnCloseGUI();

#ifdef _Mac
	::DisposeWindow((WindowRef)mhWndParent);
#endif	// _Mac

	Destroy();

//	GetParmMan()->Set(true, 0, giParam_Ch_Insert1GUIOpen, de::IParameterManager::TypeGlobal, section);

	return true;
}

void CKSPlugInGUI::CloseWindow()
{
	gpApplication->CloseWindow(mhWndParent);
}

void CKSPlugInGUI::UpdateChannelData(tint32 iID, tint32 iValue, tint32 iChannel)
{
	dynamic_cast<CKSPlugInGUIPane*>(GetPane())->UpdateChannelData(iID, iValue, iChannel);
}

void CKSPlugInGUI::SetInfo(tint32 iSectionID, tint32 iInsertIndex)
{
	dynamic_cast<CKSPlugInGUIPane*>(GetPane())->SetInfo(iSectionID, iInsertIndex);
}

void CKSPlugInGUI::UpdateChannelName(tint32 iChannel, const std::string& sName)
{
	dynamic_cast<CKSPlugInGUIPane*>(GetPane())->UpdateChannelName(iChannel, sName);
}







