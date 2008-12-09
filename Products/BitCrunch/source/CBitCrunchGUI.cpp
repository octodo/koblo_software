


#include "BitCrunchOS.h"



const ge::SSize gSizeVU(5, 143);

CBitCrunchGUI::CBitCrunchGUI(CBitCrunchPlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan)
{
}

CBitCrunchGUI::~CBitCrunchGUI()
{
}

void CBitCrunchGUI::Destroy()
{
	GetPlugIn()->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CBitCrunchGUI*>(this);
}

void CBitCrunchGUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	// TODO: Change the X and Y size of your GUI to fit the background image wou want
	*piSizeX = 222;
	*piSizeY = 106;
}

void CBitCrunchGUI::Initialize()
{
	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CBitCrunchPane(NULL, dynamic_cast<CBaseGUI*>(this));
	pPane->Init();
	SetPane(pPane);
	// This is the top pane, so set it in window
	mpWindow->SetPane(pPane->GetPane());

	dynamic_cast<CBitCrunchPane*>(pPane)->ConnectControls();
//	ConnectControls();
}

void CBitCrunchGUI::ConnectControls()
{

	mpParmMan->ControlUpdateAll();

	mpParmMan->ParameterUpdateAll();
}

void CBitCrunchGUI::UpdateEngineData(tint iType, tint iExtra, tint32 iID, tint32 iValue)
{
}

/*
	IEventReceiver
*/

void CBitCrunchGUI::EventValueChange(ge::IControl *pSender, tint32 iValueNew)
{
	// Notify the parm manager that a control has changed
//	mpParmMan->ControlUpdate(pSender->GetID(), iValueNew);
}

void CBitCrunchGUI::EventGeneric(ge::IControl *pSender, void *pEventData)
{
}

void CBitCrunchGUI::GetName(tchar* psz)
{
	strcpy((char*)psz, ((CBasePlugIn*)GetPlugIn())->GetProductName().c_str());
}

void CBitCrunchGUI::OnTimer()
{
	CBaseGUI::OnTimer();
}

