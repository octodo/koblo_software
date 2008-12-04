


#include "DigitalEchoOS.h"



const ge::SSize gSizeVU(5, 143);

CDigitalEchoGUI::CDigitalEchoGUI(CDigitalEchoPlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan)
{
}

CDigitalEchoGUI::~CDigitalEchoGUI()
{
}

void CDigitalEchoGUI::Destroy()
{
	GetPlugIn()->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CDigitalEchoGUI*>(this);
}

void CDigitalEchoGUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	// TODO: Change the X and Y size of your GUI to fit the background image wou want
	*piSizeX = 397;
	*piSizeY = 99;
}

void CDigitalEchoGUI::Initialize()
{
	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CDigitalEchoPane(NULL, dynamic_cast<CBaseGUI*>(this));
	pPane->Init();
	SetPane(pPane);
	// This is the top pane, so set it in window
	mpWindow->SetPane(pPane->GetPane());

	dynamic_cast<CDigitalEchoPane*>(pPane)->ConnectControls();
//	ConnectControls();
}

void CDigitalEchoGUI::ConnectControls()
{

	mpParmMan->ControlUpdateAll();

	mpParmMan->ParameterUpdateAll();
}

void CDigitalEchoGUI::UpdateEngineData(tint iType, tint iExtra, tint32 iID, tint32 iValue)
{
}

/*
	IEventReceiver
*/

void CDigitalEchoGUI::EventValueChange(ge::IControl *pSender, tint32 iValueNew)
{
	// Notify the parm manager that a control has changed
//	mpParmMan->ControlUpdate(pSender->GetID(), iValueNew);
}

void CDigitalEchoGUI::EventGeneric(ge::IControl *pSender, void *pEventData)
{
}

void CDigitalEchoGUI::GetName(tchar* psz)
{
	strcpy((char*)psz, ((CBasePlugIn*)GetPlugIn())->GetProductName().c_str());
}

void CDigitalEchoGUI::OnTimer()
{
	CBaseGUI::OnTimer();
}

