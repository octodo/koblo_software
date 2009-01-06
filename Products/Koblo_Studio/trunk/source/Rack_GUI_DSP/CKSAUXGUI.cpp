
#include "KSOS.h"

//===================================================================================================
//===================================================================================================
//===================================================================================================
//
//
//
//
//
//===================================================================================================
//===================================================================================================
//===================================================================================================



CKSAUXGUI::CKSAUXGUI( de::IParameterManager* pParmMan)
	: CKSBaseGUI( pParmMan),
	CBaseGUI(dynamic_cast<CBasePlugIn*>(gpApplication), pParmMan)
{
}

CKSAUXGUI::~CKSAUXGUI()
{
}

void CKSAUXGUI::Destroy()
{
	gpApplication->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CKSAUXGUI*>(this);
}

void CKSAUXGUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	*piSizeX = gAUX.iCX;
	*piSizeY = gAUX.iCY;
}

void CKSAUXGUI::Initialize()
{
	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CKSAUXGUIPane(NULL, dynamic_cast<CBaseGUI*>(this));
	pPane->Init();
	SetPane(pPane);
	// This is the top pane, so set it in window
	mpWindow->SetPane(pPane->GetPane());
	
	
	
	

	ConnectControls();

//#ifdef WIN32
	mpContext->SetMinWindowSize(mpWindow, gAUX_Min);
	mpContext->SetMaxWindowSize(mpWindow, gAUX_Max);
//#endif // WIN32

	//mpTimerMeters = ITimer::Create();
	//mpTimerMeters->Init(giTimerID_Meters_AUXs, this, 50);

}

void CKSAUXGUI::ConnectControls()
{
	dynamic_cast <CKSAUXGUIPane*>( GetPane())->ConnectControls();

	mpParmMan->ControlUpdateAll();
	mpParmMan->ParameterUpdateAll();
}

/*
	IEventReceiver
*/

void CKSAUXGUI::EventValueChange(ge::IControl* pSender, tint32 iValueNew)
{
}

void CKSAUXGUI::EventGeneric(ge::IControl* pControl, void *pEventData)
{
}

void CKSAUXGUI::GetName(tchar* psz)
{
	strcpy((char*)psz, gpApplication->GetProductName().c_str());
}

void CKSAUXGUI::OnTimer()
{
	CBaseGUI::OnTimer();
}

void CKSAUXGUI::OnTimer(tint32 iTimerID)
{
	CKSAUXGUIPane* pPane = dynamic_cast <CKSAUXGUIPane*>( GetPane());
	if (pPane) {
		pPane->OnTimer(iTimerID);
	}
}

void CKSAUXGUI::UpdateGUIData(tint32 iID, tint32 iValue)
{
	dynamic_cast <CKSAUXGUIPane*>( GetPane())->UpdateGUIData( iID,  iValue);
}


void CKSAUXGUI::UpdateAUX1Data(tint32 iID, tint32 iValue)
{

	dynamic_cast <CKSAUXGUIPane*>( GetPane())->UpdateAUX1Data( iID,  iValue);
}

void CKSAUXGUI::UpdateAUX2Data(tint32 iID, tint32 iValue)
{

	dynamic_cast <CKSAUXGUIPane*>( GetPane())->UpdateAUX2Data( iID,  iValue);
}