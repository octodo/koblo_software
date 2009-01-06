


#include "KSOS.h"


CKSSplashScreenPane::CKSSplashScreenPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
}

CKSSplashScreenPane::~CKSSplashScreenPane()
{
}

void CKSSplashScreenPane::Init()
{
	// Create the main pane
	mpPane = ge::IPane::Create();
//	mpPane->SetSize(ge::SSize(100, 100));

	// Backdrop
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Splash_Screen, ge::SPos(0, 0));
	
	/*
	char psz[128] = "hello world";
	//sprintf(psz, "%d", iNumber);
	*/
	ge::IText* pText;
	
	pText = CreateText(		giControlIDSplashText, 
							ge::SPos(200, 311), 
							ge::SSize(220, 16), 
							CreateFont(Generic128, IDF_Font_Fuxley_712, ge::SRGB(230, 230, 230)), 
							"Setting Up Windows...", 
							ge::IText::HorzAlignCenter, 
							ge::IText::VertAlignCenter);
	
		
}

void CKSSplashScreenPane::SetText(const std::string& sText)
{
	ge::IControl* pControl = GetWindow()->GetControl(giControlIDSplashText);
	ge::IText* pText = dynamic_cast<ge::IText*>(pControl);
	if (pText) {
		pText->SetText(sText.c_str());
	}
}

CKSSplashScreen::CKSSplashScreen(CKSPlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CKSBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan),
	CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan)
{
		mpTimerMeters = NULL;
}

CKSSplashScreen::~CKSSplashScreen()
{
}

void CKSSplashScreen::Destroy()
{
	gpApplication->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CTrack_Editor_View*>(this);
}

void CKSSplashScreen::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	// TODO: Change size here
	*piSizeX = 624;
	*piSizeY = 375;
}


void CKSSplashScreen::Initialize()
{
	CBasePane* pPane = new CKSSplashScreenPane(NULL, dynamic_cast<CBaseGUI*>(this));
	pPane->Init();
	SetPane(pPane);
	// This is the top pane, so set it in window
	mpWindow->SetPane(pPane->GetPane());

	ConnectControls();
}

void CKSSplashScreen::SetText(const std::string& sText)
{
	dynamic_cast<CKSSplashScreenPane*>(GetPane())->SetText(sText);
}

void CKSSplashScreen::ConnectControls()
{
//	dynamic_cast <CKSSplashScreenPane*>( GetPane())->ConnectControls();
}

void CKSSplashScreen::UpdateEngineData(tint iType, tint iExtra, tint32 iID, tint32 iValue)
{
}

void CKSSplashScreen::UpdateChannelData(tint32 iID, tint32 iValue, tint32 iChannel)
{

//	dynamic_cast <CKSSplashScreenPane*>( GetPane())->UpdateChannelData( iID,  iValue,  iChannel);
}

void CKSSplashScreen::UpdateMasterData(tint32 iID, tint32 iValue)
{

//	mpAuxMonitor_GUI->UpdateData( iID,  iValue);
//	dynamic_cast <CAuxMonitor_GUIPane*>( GetPane())->UpdateData( iID,  iValue);
}


void CKSSplashScreen::UpdateAuxMonitorData(tint32 iID, tint32 iValue)
{

//	mpAuxMonitor_GUI->UpdateData( iID,  iValue);
//	dynamic_cast <CAuxMonitor_GUIPane*>( GetPane())->UpdateData( iID,  iValue);
//	dynamic_cast <CKSMixEditorPane*>( GetPane())->UpdateAuxMonitorData( iID,  iValue);
}


/*
	IEventReceiver
*/

void CKSSplashScreen::EventValueChange(ge::IControl* pSender, tint32 iValueNew)
{
	// Notify the parm manager that a control has changed
//	mpParmMan->ControlUpdate(pSender->GetID(), iValueNew);
}

void CKSSplashScreen::EventGeneric(ge::IControl* pControl, void *pEventData)
{
	
}

void CKSSplashScreen::GetName(tchar* psz)
{
	strcpy((char*)psz, gpApplication->GetProductName().c_str());
}

void CKSSplashScreen::OnTimer()
{
	CBaseGUI::OnTimer();
}


void CKSSplashScreen::OnTimer(tint32 iTimerID)
{

	
}










