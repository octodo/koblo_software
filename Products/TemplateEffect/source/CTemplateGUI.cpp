


#include "TemplateOS.h"



const ge::SSize gSizeVU(5, 143);

CTemplateGUI::CTemplateGUI(CTemplatePlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan)
{
}

CTemplateGUI::~CTemplateGUI()
{
}

void CTemplateGUI::Destroy()
{
	GetPlugIn()->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CTemplateGUI*>(this);
}

void CTemplateGUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	// TODO: Change the X and Y size of your GUI to fit the background image wou want
	*piSizeX = 640;
	*piSizeY = 430;
}

void CTemplateGUI::Initialize()
{
	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CTemplatePane(NULL, dynamic_cast<CBaseGUI*>(this));
	pPane->Init();
	SetPane(pPane);
	// This is the top pane, so set it in window
	mpWindow->SetPane(pPane->GetPane());
	
	dynamic_cast<CTemplatePane*>(pPane)->ConnectControls();

//	ConnectControls();
}

void CTemplateGUI::ConnectControls()
{

	mpParmMan->ControlUpdateAll();

	mpParmMan->ParameterUpdateAll();
}

void CTemplateGUI::UpdateEngineData(tint iType, tint iExtra, tint32 iID, tint32 iValue)
{
}

/*
	IEventReceiver
*/

void CTemplateGUI::EventValueChange(ge::IControl *pSender, tint32 iValueNew)
{
	// Notify the parm manager that a control has changed
//	mpParmMan->ControlUpdate(pSender->GetID(), iValueNew);
}

void CTemplateGUI::EventGeneric(ge::IControl *pSender, void *pEventData)
{
}

void CTemplateGUI::GetName(tchar* psz)
{
	strcpy((char*)psz, ((CBasePlugIn*)GetPlugIn())->GetProductName().c_str());
}

void CTemplateGUI::OnTimer()
{
	CBaseGUI::OnTimer();
}

