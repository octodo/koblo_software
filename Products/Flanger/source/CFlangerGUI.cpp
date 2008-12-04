


#include "FlangerOS.h"



const ge::SSize gSizeVU(5, 143);

CFlangerGUI::CFlangerGUI(CFlangerPlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan)
{
}

CFlangerGUI::~CFlangerGUI()
{
}

void CFlangerGUI::Destroy()
{
	GetPlugIn()->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CFlangerGUI*>(this);
}

void CFlangerGUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	// TODO: Change the X and Y size of your GUI to fit the background image wou want
	*piSizeX = 397;
	*piSizeY = 99;
}

void CFlangerGUI::Initialize()
{
	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CFlangerPane(NULL, dynamic_cast<CBaseGUI*>(this));
	pPane->Init();
	SetPane(pPane);
	// This is the top pane, so set it in window
	mpWindow->SetPane(pPane->GetPane());
	
	dynamic_cast<CFlangerPane*>(pPane)->ConnectControls();

//	ConnectControls();
}

void CFlangerGUI::ConnectControls()
{
	// TODO: Connect control IDs (for GUI use) to parm-IDs (for parameter-manager use) below here.
	// Use the CBaseGUI::RegisterGlobalControlXxx...() methods.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_g_u_i.html
	// NOTE: Remember to first add the parm-IDs to the parameter-manager (in CFlangerPlugIn::AddParameters() ).
	// NOTE: Several control-IDs may be connected to one parm-ID.
	// NOTE: Only one parm-ID may be connected to each control-ID.


	mpParmMan->ControlUpdateAll();

	mpParmMan->ParameterUpdateAll();
}

void CFlangerGUI::UpdateEngineData(tint iType, tint iExtra, tint32 iID, tint32 iValue)
{
}

/*
	IEventReceiver
*/

void CFlangerGUI::EventValueChange(ge::IControl *pSender, tint32 iValueNew)
{
	// Notify the parm manager that a control has changed
//	mpParmMan->ControlUpdate(pSender->GetID(), iValueNew);
}

void CFlangerGUI::EventGeneric(ge::IControl *pSender, void *pEventData)
{
}

void CFlangerGUI::GetName(tchar* psz)
{
	strcpy((char*)psz, ((CBasePlugIn*)GetPlugIn())->GetProductName().c_str());
}

void CFlangerGUI::OnTimer()
{
	CBaseGUI::OnTimer();
}

