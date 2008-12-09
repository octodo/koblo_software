


#include "VST2KSPIOS.h"



CVST2KSPIGUI::CVST2KSPIGUI(CVST2KSPIPlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan),
	mpTopPane(NULL),
	mbDemoDialogShown(false), mbUnauthorizedDialogShown(false),
	CBasePane(NULL, this)
{
}

CVST2KSPIGUI::~CVST2KSPIGUI()
{
	if (mpTopPane) {
		delete mpTopPane;
	}
}

void CVST2KSPIGUI::Destroy()
{
	GetPlugIn()->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CVST2KSPIGUI*>(this);
}

void CVST2KSPIGUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	*piSizeX = 664;
	*piSizeY = 170;
}

void CVST2KSPIGUI::Initialize()
{
	// Kindof a hack needed to make it possible to override from both gui and pane
	CBasePane::mpResMan = CBaseGUI::GetResMan();

	// Create the main pane
	CBasePane::mpPane = ge::IPane::Create();

	ConnectControls();
}

void CVST2KSPIGUI::ConnectControls()
{
	CBaseGUI::mpParmMan->ControlUpdateAll();
}

/*
	IContextCallback
*/

void CVST2KSPIGUI::EventValueChange(ge::IControl *pSender, tint32 iValueNew)
{
	// Notify the parm manager that a control has changed
	CBaseGUI::mpParmMan->ControlUpdate(miPaneID, pSender->GetID(), iValueNew);
}

void CVST2KSPIGUI::EventGeneric(ge::IControl *pSender, void *pEventData)
{
}

void CVST2KSPIGUI::GetName(tchar* psz)
{
	std::string sName = CBaseGUI::mpPlugIn->GetProductName();

	strcpy((char*)psz, sName.c_str());
}

void CVST2KSPIGUI::OnTimer()
{
	CBaseGUI::OnTimer();
}

void CVST2KSPIGUI::OnAbout()
{
	tbool bIsRegistered = GetPlugIn()->IsRegistered();

	tchar psz[256];
	sprintf(psz, "Essential Compressor by KeyToSound, www.keytosound.com\nVersion 0.9b1\n%s",
		bIsRegistered ? "Registered" : "Not registered");

	CBaseGUI::GetWindow()->ShowMessageBox(psz, "Info");
}

void CVST2KSPIGUI::OnManual()
{
	std::string sPathName = GetResourceFolder();
	sPathName += "Essential Compressor manual.pdf";

	::OpenDocument(sPathName.c_str());
}

