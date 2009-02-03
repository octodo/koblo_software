


#include "VST2KSPIOS.h"



CVST2KSPIGUI::CVST2KSPIGUI(CVST2KSPIPlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan),
	mpTopPane(NULL),
	mbDemoDialogShown(false), mbUnauthorizedDialogShown(false),
	CBasePane(NULL, this),
	mpVSTEffect(NULL)
{
}

CVST2KSPIGUI::~CVST2KSPIGUI()
{
	mpVSTEffect->dispatcher(mpVSTEffect, effEditClose, 0, 0, 0, 0.0);

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
	if (mpVSTEffect == NULL) {
		mpVSTEffect = dynamic_cast<CVST2KSPIPlugIn*>(GetPlugIn())->GetEffect();
	}

	ERect* pRect;
	mpVSTEffect->dispatcher(mpVSTEffect, effEditGetRect, 0, 0, &pRect, 0.0);

	*piSizeX = pRect->right;
	*piSizeY = pRect->bottom;
}

void CVST2KSPIGUI::MakeWindow(void* p)
{
	if (mpVSTEffect == NULL) {
		mpVSTEffect = dynamic_cast<CVST2KSPIPlugIn*>(GetPlugIn())->GetEffect();
	}

	try {
		PreCreate();
		
		// Create the window
		mpWindow = ge::IWindow::Create();
		// Tell the window what context it's in
		mpWindow->SetContext(mpContext);
		
#ifdef WIN32
		mhWndParent = (HWND)p;
		mpVSTEffect->dispatcher(mpVSTEffect, effEditOpen, 0, 0, p, 0.0);
#endif
#ifdef _Mac
		/*void**/ mhWndParent = p;
		mpVSTEffect->dispatcher(mpVSTEffect, effEditOpen, 0, 0, p, 0.0);
#endif
		
		tuint32 iSizeX;
		tuint32 iSizeY;
		GetSize(&iSizeX, &iSizeY);
		
		ge::SRect rct(0, 0, iSizeX, iSizeY);
//		mpWindow->MakeWindow(mhWndParent, &rct, false);
		
		Initialize();
		
		// Make the window visible
//		mpWindow->ShowWindow();
		
//		mpWindow->InstallKeyboardHook();
		
//		mpWindow->AcquireKeyboardInput();
	}
	catch(IException* pException) {
		const tchar* psz = pException->GetFullDescription();
		
//		DoMsgBox(psz);
		
		pException->Destroy();
	}
	catch(...) {
//		DoMsgBox("Unknown Exception");
	}
}

void CVST2KSPIGUI::Initialize()
{
	if (mpVSTEffect == NULL) {
		mpVSTEffect = dynamic_cast<CVST2KSPIPlugIn*>(GetPlugIn())->GetEffect();
	}
	
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
//	CBaseGUI::OnTimer();
	mpVSTEffect->dispatcher(mpVSTEffect, effEditIdle, 0, 0, 0, 0.0);
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

