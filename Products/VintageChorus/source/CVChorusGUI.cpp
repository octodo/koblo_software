


#include "VChorusOS.h"



const ge::SSize gSizeVU(5, 143);

CVChorusGUI::CVChorusGUI(CVChorusPlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan)
{
}

CVChorusGUI::~CVChorusGUI()
{
}

void CVChorusGUI::Destroy()
{
	GetPlugIn()->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CVChorusGUI*>(this);
}

void CVChorusGUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	// TODO: Change the X and Y size of your GUI to fit the background image wou want
	*piSizeX = 397;
	*piSizeY = 99;
}

void CVChorusGUI::Initialize()
{
	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CVChorusPane(NULL, dynamic_cast<CBaseGUI*>(this));
	pPane->Init();
	SetPane(pPane);
	// This is the top pane, so set it in window
	mpWindow->SetPane(pPane->GetPane());
	
	dynamic_cast<CVChorusPane*>(pPane)->ConnectControls();

//	ConnectControls();
}

void CVChorusGUI::ConnectControls()
{

	mpParmMan->ControlUpdateAll();

	mpParmMan->ParameterUpdateAll();
}

void CVChorusGUI::UpdateEngineData(tint iType, tint iExtra, tint32 iID, tint32 iValue)
{
}

/*
	IEventReceiver
*/

void CVChorusGUI::EventValueChange(ge::IControl *pSender, tint32 iValueNew)
{
	// Notify the parm manager that a control has changed
//	mpParmMan->ControlUpdate(pSender->GetID(), iValueNew);
}

void CVChorusGUI::EventGeneric(ge::IControl *pSender, void *pEventData)
{
}

void CVChorusGUI::GetName(tchar* psz)
{
	strcpy((char*)psz, ((CBasePlugIn*)GetPlugIn())->GetProductName().c_str());
}

void CVChorusGUI::OnTimer()
{
	CBaseGUI::OnTimer();
}

