//
//  main.c
//  Koblo Studio
//
//  Created by mo on 11/29/05.
//  Copyright Koblo 2008. All rights reserved.
//

#include "KSOS.h"

// Lasse:
#ifdef WIN32
#include "projects\win32\resource.h"
#endif // WIN32
// .. Lasse

static void HandleNew_Audio();
static void HandleNew_MIDI();
static void HandleNew_Menu();







volatile bool gbClosingDown = false;
volatile bool gbInAudioEngine = false;

CKSApplication* gpPlugIn = NULL;
CDSP* gpDSP = NULL;

class CAppHost : public virtual kspi::IHost
{
public:
	CAppHost();

	virtual void KSPI_CALL OnAutomationUpdate(kspi::IPlugIn* pCaller, tint32 iParameterIndex, tint32 iValue) {}
	virtual tfloat32 KSPI_CALL GetCurrentBPM() {return 0.0f;}
	virtual kspi::IHost::EType KSPI_CALL GetType() {return kspi::IHost::HostTypeStandAlone;}
	virtual void KSPI_CALL ActivateWindow(tint32 iIndex);
	virtual void KSPI_CALL HideWindow(tint32 iIndex);
	virtual tint32 KSPI_CALL IsWindowVisible(tint32 iIndex);
	virtual void KSPI_CALL RedrawWindow(kspi::IGUI* pCaller, tint32 iX, tint32 iY, tint32 iCX, tint32 iCY);
	virtual tint32 KSPI_CALL DoesWindowHaveToolbar(tint32 iIndex);
};

CAppHost::CAppHost()
{
}

void CAppHost::RedrawWindow(kspi::IGUI* pCaller, tint32 iX, tint32 iY, tint32 iCX, tint32 iCY)
{
}

CAppHost* gpHost = 0;

class CGUI
{
public:
	CGUI(kspi::IGUI* pGUI);

	virtual void Idle();

protected:
	kspi::IGUI* mpGUI;
};

CGUI::CGUI(kspi::IGUI* pGUI) : mpGUI(pGUI)
{
}

void CGUI::Idle()
{
	mpGUI->OnTimer();
}

#ifdef _Mac
CFRunLoopTimerRef gTimer;
	
// Undocumented system call to get the main runloop
extern "C" CFRunLoopRef CFRunLoopGetMain(void);

typedef void (*CFRunLoopTimerCallBack) (CFRunLoopTimerRef timer,  void *info);

void timerCallbackGUI(CFRunLoopTimerRef timer, void *info);
#endif	// _Mac


//enum EInputOutput { keBothInputAndOutput, keInputOnly, keOutputOnly };


class CCallback : public virtual aioe::IDeviceCallback, public virtual mioe::IIDeviceCallback
{
public:
	CCallback();
	CCallback(tbool bEnableInput, tbool bEnableOutput);
	//CCallback(EInputOutput eIO);
	virtual ~CCallback();

	//! aioe::IDeviceCallback override
	virtual void ProcessAudio(tfloat32** ppOut, const tfloat32** ppIn, tint iSamples);

	//! mioe::IIDeviceCallback override
	virtual void OnMIDI(tuchar ucStatus, tuchar ucDB1, tuchar ucDB2);
	//! mioe::IIDeviceCallback override
	virtual void OnMIDISysEx(const void* p, tint iSize);
	
	//! (lasse): ? not used?
	void SetAudioDevice(aioe::IDevice* pAudioDevice) { mpAudioDevice = pAudioDevice; };

protected:
	aioe::IDevice* mpAudioDevice;
	tbool mbInput, mbOutput, mbBoth;

	//! List of MIDI events to be send at next processing interval
	std::list<kspi::SMIDIEvent> mMIDIEvents;

	//! Mutex used for audio/MIDI
	CMutex mMutexAudio;
};

CCallback::CCallback() : mpAudioDevice(NULL), mbInput(true), mbOutput(true), mbBoth(true)
{
}


CCallback::CCallback(tbool bEnableInput, tbool bEnableOutput) : mpAudioDevice(NULL)
{
	mbInput = bEnableInput;
	mbOutput = bEnableOutput;
	mbBoth = (mbInput && mbOutput);
}

CCallback::~CCallback()
{
}

void CCallback::ProcessAudio(tfloat32** ppOut, const tfloat32** ppIn, tint iSamples)
{
	if (gbClosingDown == true) {
		return;
	}

	gbInAudioEngine = true;

	// Deliver the MIDI events
	{
		CAutoLock Lock(mMutexAudio);

		tuint iEvent;
		tuint iNrOfEvents = mMIDIEvents.size();
		std::list<kspi::SMIDIEvent>::iterator it = mMIDIEvents.begin();
		for (iEvent = 0; iEvent < iNrOfEvents; iEvent++, it++) {
			kspi::SMIDIEvent Event = *it;
//			mpPlugIn->InputMIDI(&Event, 1);
		}

		// Clear the event buffer
		mMIDIEvents.clear();
	}

	// 200Hz sine. Can be used to test output
/*	tfloat32* pfLeft = ppOut[0];
	tfloat32* pfRight = ppOut[1];
	static float fIndex = 0;
	tint32 iSample;
	tfloat fHz = 200.0f;
	for (iSample = 0; iSample < iSamples; iSample++) {
		float f = sin(fIndex * 3.14 * 2);
		pfLeft[iSample] = f;
		pfRight[iSample] = f;

		fIndex += fHz / 44100;
		if (fIndex >= 1) {
			fIndex -= 1;
		}
	}*/

	//gpPlugIn->ProcessNonInPlace(ppOut, ppIn, iSamples);
/*	tint i;
	tfloat32* pOutL = ppOut[0];
	tfloat32* pOutR = ppOut[1];
	for (i = 0; i < iSamples; i++) {
		pOutL[i] = 0;
		pOutR[i] = 0;
	}*/

	if (mbBoth)
		gpPlugIn->ProcessNonInPlace(ppOut, ppIn, iSamples);
	else if (mbInput)
		gpPlugIn->ProcessNonInPlace_InputOnly(ppIn, iSamples);
	else // if (mbOutput)
		gpPlugIn->ProcessNonInPlace_OutputOnly(ppOut, iSamples);

	gbInAudioEngine = false;
}

void CCallback::OnMIDI(tuchar ucStatus, tuchar ucDB1, tuchar ucDB2)
{
//	WindowPtr pFront = FrontWindow();
//	WindowPtr pThisWindow = gpNexsynWindow;

	//if(window != FrontWindow())
//		SelectWindow(gpNexsynWindow);
//		BeginUpdate(window);
//		SetPortWindowPort(window);
//		EndUpdate(window);

	CAutoLock Lock(mMutexAudio);
	


	// Timestamp the MIDI
	tuint32 uiTimeStamp = 0;
	if (mpAudioDevice) {
		uiTimeStamp = mpAudioDevice->GetTimeStamp();

		// Just to be on the safe side
		if (uiTimeStamp >= mpAudioDevice->GetBufferSize()) {
			uiTimeStamp = mpAudioDevice->GetBufferSize();
		}
	}

	// Create MIDI Event
	kspi::SMIDIEvent Event;
	Event.iTimeStamp = uiTimeStamp;
	Event.pcData[0] = ucStatus;
	Event.pcData[1] = ucDB1;
	Event.pcData[2] = ucDB2;
	Event.pcData[3] = 0;

	// The event is ready, but the timestamp may be off.
	// This is because there's an interval from the time the audio engine resets its "start-of-buffer-time" to the time the processing is actually started.
	// If a MIDI event occurs in this interval it will receive a very low timestamp (typically 0 I believe) because the audio engine believes it belongs to the next buffer
	// The most important thing is to avoid hanging notes, so what we do is to detect the case where hanging notes would occur, i.e. when a note-off would occur before a note-on
	// So, this means that if the current queue is empty we will accept the jitter, but if the queue is not empty we should be able to detect the hanging-notes case by comparing the timestamp of our last event in the buffer.
	// If the current timestamp is less than the timestamp of the last event we know this has occured, and hanging notes is a possibility.
	// What we do then is to give it the timestamp of the last event.
	// A better approach would be to keep a seperate event list for the next buffer. But at least we don't get hanging notes.
	std::list<kspi::SMIDIEvent>::iterator it = mMIDIEvents.end();
	it--;
	if (it != mMIDIEvents.end()) {
		tuint32 uiTimeStampEvent = it->iTimeStamp;
		if (Event.iTimeStamp < uiTimeStampEvent) {
			Event.iTimeStamp = uiTimeStampEvent;
		}
	}
	// Put it in MIDI queue
	mMIDIEvents.insert(mMIDIEvents.end(), Event);
	
}

void CCallback::OnMIDISysEx(const void* /*p*/, tint /*iSize*/)
{
}


void ShowMessageBox(const tchar* pszMessage, const tchar* pszHeader)
{
	CKSApplication::ShowMessageBox(pszMessage, pszHeader);
}

//aioe::IDevice* pAudio = NULL;

int giNrOfAudioDevices = 0;
int giNrOfMIDIDevices = 0;
int giNrOfBufferSizes = 0;

CMainApplication* gpMainApplication = NULL;

void* gWndMain;
void* gWndSplash;
void* gWndMixer;
void* gWndAUX;
void* gWndExport;
void* gWndExportForWeb;
void* gWndImport;
void* gWndSetup;
void* gWndProjectID;
void* gWndSignIn;


//tbool gbShowZoom;
//tbool gbTrackEditInFront;


CTrack_Editor_View* gpGUITrackEdit;
CKSSplashScreen* gpSplashGUI;
CKSMixerGUI* gpGUIMixer;
CKSAUXGUI* gpGUIAUX;
CKSExportGUI* gpGUIExport;
CKSExportForWebGUI* gpGUIExportForWeb;
CKSImportGUI* gpGUIImport;
CKSSetupGUI* gpGUISetup;
CKSProject_ID_GUI* gpGUIProjectID;
CKSUsername_And_Password_View* gpGUISignIn;

#ifdef WIN32
#include "afxres.h"
#endif // #ifdef WIN32

class CAppCallback : public virtual ge::IApplicationCallback
{
public:
	virtual void OnMenuEvent(const tchar* pszString);

	virtual void SizeChanged(void* pWnd, ge::SSize SizeNew);

	virtual void WindowClosed(void* pWnd);

	virtual tbool OnInit(tint32 iRunIndex);

protected:
};

void CAppCallback::OnMenuEvent(const tchar* pszString)
{
	gpPlugIn->OnMenuEvent(pszString);
}

void CAppCallback::SizeChanged(void* p, ge::SSize SizeNew)
{
	if (p == gWndMain) {
		// Track window
		gpGUITrackEdit->Resize(SizeNew.iCX, SizeNew.iCY);
	}
	else if (p == gWndMixer) {
		// Mix window
		gpGUIMixer->Resize(SizeNew.iCX, SizeNew.iCY);
	}
	else if (p == gWndAUX) {
		gpGUIAUX->Resize(SizeNew.iCX, SizeNew.iCY);
	}
}

void CAppCallback::WindowClosed(void* pWnd)
{
	gpPlugIn->WindowClosed(pWnd);
}

ge::IContext* pContext = NULL;

tbool CAppCallback::OnInit(tint32 iRunIndex)
{
	switch(iRunIndex) {
		case 0:
			gpSplashGUI->SetText("Creating Tracks Window...");
			gpHost->ActivateWindow(giSplash_Window);
			break;

		case 1:
			{
#ifdef WIN32
				gpGUITrackEdit->MakeWindow(gWndMain);//, IDC_KS_MENU);
#else WIN32
				gpGUITrackEdit->MakeWindow(gWndMain);
#endif // #else WIN32
				gpGUITrackEdit->Paint();

#ifdef WIN32
				// Nice Windows behaviour: We want the application to shutdown when the main window closes
				pContext->SetClosingBehaviour(gWndMain, ge::IContext::giClosingQuitsApp);
#endif // #ifdef WIN32

#ifdef _Mac
				// Nice OSX behaviour: We want the application to stay open when the main window closes
				pContext->SetClosingBehaviour(gWndMain, ge::IContext::giClosingOnlyHides);
#endif // #ifdef _Mac
				//gpHost->HideWindow(7);
			}
			break;

		case 2:
			gpSplashGUI->SetText("Creating Mix Window...");
			gpHost->ActivateWindow(giSplash_Window);
			break;

		case 3:
			{
				gpGUIMixer->MakeWindow(gWndMixer);
				gpGUIMixer->Paint();
				pContext->SetClosingBehaviour(gWndMixer, ge::IContext::giClosingOnlyHides);
			}
			break;

		case 4:
			gpSplashGUI->SetText("Creating Audio Export Dialog...");
			gpHost->ActivateWindow(giSplash_Window);
			break;

		case 5:
			{
				gpGUIExport->MakeWindow(gWndExport);
				gpGUIExport->Paint();
				pContext->SetClosingBehaviour(gWndExport, ge::IContext::giClosingOnlyHides);
			}
			break;

		case 6:
			gpSplashGUI->SetText("Creating License Setup Dialog...");
			gpHost->ActivateWindow(giSplash_Window);
			break;

		case 7:
			{
				gpGUIExportForWeb->MakeWindow(gWndExportForWeb);
				gpGUIExportForWeb->Paint();
				pContext->SetClosingBehaviour(gWndExportForWeb, ge::IContext::giClosingOnlyHides);
			}
			break;
		case 8:
			gpSplashGUI->SetText("Creating Import Audio Dialog...");
			gpHost->ActivateWindow(giSplash_Window);
			break;
		case 9:
			{
				gpGUIImport->MakeWindow(gWndImport);
				gpGUIImport->Paint();
				pContext->SetClosingBehaviour(gWndImport, ge::IContext::giClosingOnlyHides);
			}
			break;
		case 10:
			gpSplashGUI->SetText("Creating Audio Setup Dialog...");
			gpHost->ActivateWindow(giSplash_Window);
			break;
		case 11:
		{
			gpGUISetup->MakeWindow(gWndSetup);
			gpGUISetup->Paint();
			pContext->SetClosingBehaviour(gWndSetup, ge::IContext::giClosingOnlyHides);
			break;
		}
		
		case 12:
		{
			gpGUIProjectID->MakeWindow(gWndProjectID);
			gpGUIProjectID->Paint();
			pContext->SetClosingBehaviour(gWndProjectID, ge::IContext::giClosingOnlyHides);
			break;
		}
		case 13:
			gpSplashGUI->SetText("Init Sign In Dialog...");
			gpHost->ActivateWindow(giSplash_Window);
			break;
			
		case 14:
		{
			gpGUISignIn->MakeWindow(gWndSignIn);
			gpGUISignIn->Paint();
			pContext->SetClosingBehaviour(gWndSignIn, ge::IContext::giClosingOnlyHides);
			break;
		}

		case 15:
			gpSplashGUI->SetText("Init Rack Window...");
			gpHost->ActivateWindow(giSplash_Window);
			break;

		case 16:
			{
				gpGUIAUX->MakeWindow(gWndAUX);
				gpGUIAUX->Paint();
				pContext->SetClosingBehaviour(gWndAUX, ge::IContext::giClosingOnlyHides);
			}
			break;

		case 17:
			gpSplashGUI->SetText("Init Audio Setup Dialog...");
			gpHost->ActivateWindow(giSplash_Window);
			break;

		case 18:
			{
				HandleNew_Audio();
			}
			break;

		case 19:
			gpSplashGUI->SetText("Init MIDI...");
			gpHost->ActivateWindow(giSplash_Window);
			break;

		case 20:
			{
				HandleNew_MIDI();
			}
			break;

		case 21:
			gpSplashGUI->SetText("Init Menu...");
			gpHost->ActivateWindow(giSplash_Window);
			break;

		case 22:
			{
				HandleNew_Menu();
			}
			break;
		
		case 23:
			gpSplashGUI->SetText("Start Audio...");
			gpHost->ActivateWindow(giSplash_Window);
			break;

		case 24:
			{
				gpDSP->StartAudioDevices();
			}
			break;

		case 25:
			gpSplashGUI->SetText("Done");
			gpHost->ActivateWindow(giSplash_Window);
			break;

		case 26:
			{
				pContext->ShowWindow(gWndMain);
			}
			break;

		case 27:
			{
				pContext->ShowWindow(gWndSplash, false);
			}
			break;
		

		default:
			// Allow for stack-tracks again
			gpPlugIn->SetGUIsReady(true);
			// Do a stack-tracks
			gpPlugIn->Stack_Tracks();
			// No more - true => quit
			return true;
	}

	// Message handled - false => continue
	return false;
}

class CContextCallback : public virtual ge::IContextCallback
{
public:
	//! IContextCallback override
	virtual void GetName(tchar* psz) {strcpy(psz, "Koblo Studio");}

	//! IContextCallback override
	virtual tbool OnKeyDown(ge::EKey Key) {return false;}
};

int main(int argc, char* argv[])
{
	//tint32 i0 = ITime::GetTimeMS();

//	CAutoDelete<ge::IContext> pContext(ge::IContext::Create());
	pContext = ge::IContext::Create();
	pContext->SetCallback(new CContextCallback());
	pContext->Init();
	/*
	pContext->SetApplicationCallback(new CAppCallback());
	gWndMain = pContext->CreateMainWindow(ge::SSize(1000, 680));
	pContext->ShowWindow(gWndMain, false);
	*/

	gpHost = new CAppHost();

	gpMainApplication = new CMainApplication();
	

	//tint32 i1 = ITime::GetTimeMS();

	CKSModule* pModule = new CKSModule(gpHost);

	pContext->SetInstanceData((void*)pModule->GetInstanceHandle());
	pContext->SetApplicationCallback(new CAppCallback());
#ifdef WIN32
	gWndMain = pContext->CreateMainWindow(ge::SSize(1000, 680), IDC_KS_MENU, IDI_KS);
#else WIN32
	gWndMain = pContext->CreateMainWindow(ge::SSize(1000, 680));
#endif // #else WIN32
	pContext->ShowWindow(gWndMain, false);

	gpPlugIn = dynamic_cast<CKSApplication*>(pModule->CreateProcess(0));
	gpPlugIn->PreInitialize();
	gpPlugIn->SetMaxBufferSize(512);
	gpPlugIn->SetSampleRate(44100);
	gpPlugIn->SetChannelConfiguration(0, 2, 0);
	gpPlugIn->Initialize();
	gpPlugIn->Start();

	//tint32 i2 = ITime::GetTimeMS();

	// Make windows
#ifdef _Mac
	gWndSplash = pContext->CreateExtraWindow((void*)CFSTR("SplashWnd"), ge::SSize(624, 375));
#endif
#ifdef WIN32
	gWndSplash = pContext->CreateExtraWindow((void*)"Welcome", ge::SSize(624, 375));
#endif
	pContext->ShowWindow(gWndSplash);

	gpGUITrackEdit = dynamic_cast<CTrack_Editor_View*>(gpPlugIn->CreateGUI(giMain_Window));

	gpSplashGUI = dynamic_cast<CKSSplashScreen*>(gpPlugIn->CreateGUI(giSplash_Window));
	gpSplashGUI->MakeWindow(gWndSplash);
	gpSplashGUI->Paint();
/*	InstallWindowEventHandler( gWndSplash, GetWindowEventHandlerUPP(),
		GetEventTypeCount( kWindowEvents ), kWindowEvents,
		 gWndSplash, NULL );*/

/*    static const EventTypeSpec    kAppEvents[] =
    {
        { kEventClassCommand, kEventCommandProcess }
    };*/
//------------------------
#ifdef _Mac
	gWndMixer = pContext->CreateExtraWindow((void*)CFSTR("MixerWnd"), ge::SSize(1000, 382));
#endif
#ifdef WIN32
	gWndMixer = pContext->CreateExtraWindow((void*)"Koblo Studio Mixer", ge::SSize(1000, 382));
#endif
	gpGUIMixer = dynamic_cast<CKSMixerGUI*>(gpPlugIn->CreateGUI(giMix_Window));
	pContext->ShowWindow(gWndMixer, false);
//------------------------	
#ifdef _Mac
	gWndAUX = pContext->CreateExtraWindow((void*)CFSTR("AUXWnd"), ge::SSize(578, 700));
#endif
#ifdef WIN32
	gWndAUX = pContext->CreateExtraWindow((void*)"Koblo Studio AUX Rack", ge::SSize(578, 700));
#endif
	gpGUIAUX = dynamic_cast<CKSAUXGUI*>(gpPlugIn->CreateGUI(giRack_Window));
	pContext->ShowWindow(gWndAUX, false);
//------------------------	
#ifdef _Mac
	gWndExport = pContext->CreateExtraWindow((void*)CFSTR("ExportWnd"), ge::SSize(386, 242));
#endif
#ifdef WIN32
	gWndExport = pContext->CreateExtraWindow((void*)"Koblo Studio Export", ge::SSize(386, 242));
#endif

	gpGUIExport = dynamic_cast<CKSExportGUI*>(gpPlugIn->CreateGUI(giExport_Audio_Window));
	pContext->ShowWindow(gWndExport, false);
//------------------------
#ifdef _Mac
	gWndExportForWeb = pContext->CreateExtraWindow((void*)CFSTR("ExportForWebWnd"), ge::SSize(541, 582));
#endif
#ifdef WIN32
	gWndExportForWeb = pContext->CreateExtraWindow((void*)"Koblo Studio Export For Web", ge::SSize(541, 582));
#endif
	
	gpGUIExportForWeb = dynamic_cast<CKSExportForWebGUI*>(gpPlugIn->CreateGUI(giExport_For_Web_Window));
	pContext->ShowWindow(gWndExportForWeb, false);
//------------------------
#ifdef _Mac
	gWndImport = pContext->CreateExtraWindow((void*)CFSTR("ImportWnd"), ge::SSize(423, 424));
#endif
#ifdef WIN32
	gWndImport = pContext->CreateExtraWindow((void*)"Koblo Studio Import", ge::SSize(423, 424));
#endif
	
	gpGUIImport = dynamic_cast<CKSImportGUI*>(gpPlugIn->CreateGUI(giImport_Audio_Window));
	pContext->ShowWindow(gWndImport, false);
	
//------------------------
#ifdef _Mac
	gWndSetup = pContext->CreateExtraWindow((void*)CFSTR("SetupWnd"), ge::SSize(320, 218));
#endif
#ifdef WIN32
	gWndSetup = pContext->CreateExtraWindow((void*)"Koblo Studio Setup", ge::SSize(320, 218));
#endif
	
	gpGUISetup = dynamic_cast<CKSSetupGUI*>(gpPlugIn->CreateGUI(giAudio_Setup_Window));
	pContext->ShowWindow(gWndSetup, false);
	
	//------------------------
#ifdef _Mac
	gWndProjectID = pContext->CreateExtraWindow((void*)CFSTR("ProjectIDWnd"), ge::SSize(244, 102));
#endif
#ifdef WIN32
	gWndProjectID = pContext->CreateExtraWindow((void*)"Koblo Studio Setup", ge::SSize(245, 103));
#endif
	
	gpGUIProjectID = dynamic_cast<CKSProject_ID_GUI*>(gpPlugIn->CreateGUI(giProject_ID_Window));
	pContext->ShowWindow(gWndProjectID, false);
	
	
	//------------------------
#ifdef _Mac
	gWndSignIn = pContext->CreateExtraWindow((void*)CFSTR("SignInWnd"), ge::SSize(244, 182));
#endif
#ifdef WIN32
	gWndSignIn = pContext->CreateExtraWindow((void*)"Sign In", ge::SSize(245, 182));
#endif
	
	gpGUISignIn = dynamic_cast<CKSUsername_And_Password_View*>(gpPlugIn->CreateGUI(giSign_In_Window));
	pContext->ShowWindow(gWndSignIn, false);



	// Notefy PlugIn that the gui is created
	gpPlugIn->GUI_Created();
	// Notefy PlugIn that PlugIn windows created
	gpPlugIn->All_Plugins_Created(true);

	//tint32 i3 = ITime::GetTimeMS();

#ifdef _Mac
	CFRunLoopTimerContext ctxt;
	ctxt.version = 0;
	ctxt.info = (void*)gpPlugIn;
	ctxt.retain = 0;
	ctxt.release = 0;
	ctxt.copyDescription = 0;
	gTimer = ::CFRunLoopTimerCreate(kCFAllocatorDefault, CFAbsoluteTimeGetCurrent() + 0.05f, 0.05f, 0, 0, timerCallbackGUI, &ctxt);

	::CFRunLoopAddTimer(CFRunLoopGetMain(), gTimer, kCFRunLoopCommonModes);
#endif	// _Mac

//	CStartUpThread* pThread = new CStartUpThread();

	//tint32 i4 = ITime::GetTimeMS();

	//tint32 i1_0 = i1 - i0;
	//tint32 i2_1 = i2 - i1;
	//tint32 i3_2 = i3 - i2;
	//tint32 i4_3 = i4 - i3;
	//tchar pszMsg[512];
	//sprintf(pszMsg, "i1_0=%d, i2_1=%d, i3_2=%d, i43=%d", i1_0, i2_1, i3_2, i4_3);
	//ShowMessageBox(pszMsg);

	pContext->RunMainLoop();

	gbClosingDown = true;
	while (gbInAudioEngine) {
	}

#ifdef WIN32
	// (lasse) for Windows it's necessary to close the audio-device before exit to release resources
	// Maybe that's not so for OS X?
	gpDSPEngine->CloseAudioDevice_Duplex();
	gpDSPEngine->CloseAudioDevice_Input();
	gpDSPEngine->CloseAudioDevice_Output();
#endif // WIN32

	gpGUITrackEdit->Destroy();
	gpSplashGUI->Destroy();
	gpGUIMixer->Destroy();
	gpGUIAUX->Destroy();
	gpGUIExport->Destroy();
	gpGUIExportForWeb->Destroy();
	gpGUIImport->Destroy();
	gpGUISetup->Destroy();
	gpGUIProjectID->Destroy();
	gpGUISignIn->Destroy();
	gpPlugIn->Destroy();

	return 0;
//    return err;
}

#ifdef WIN32
extern HINSTANCE ghInstance;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	ghInstance = hInstance;

	main(0, NULL);
}
#endif	// WIN32

tint32 CAppHost::DoesWindowHaveToolbar(tint32 iIndex)
{
	return 1;
}

tint32 CAppHost::IsWindowVisible(tint32 iIndex)
{
	switch(iIndex) {
		case giMain_Window:
			return pContext->IsWindowVisible(gWndMain) == true ? 1 : 0;
			
		case giMix_Window:
			return pContext->IsWindowVisible(gWndMixer) == true ? 1 : 0;

		case giRack_Window:
			return pContext->IsWindowVisible(gWndAUX) == true ? 1 : 0;

		case giSplash_Window:
			return pContext->IsWindowVisible(gWndSplash) == true ? 1 : 0;
			
		case giExport_Audio_Window:
			return pContext->IsWindowVisible(gWndExport) == true ? 1 : 0;
			
		case giExport_For_Web_Window:
			return pContext->IsWindowVisible(gWndExportForWeb) == true ? 1 : 0;
			
		case giImport_Audio_Window:
			return pContext->IsWindowVisible(gWndImport) == true ? 1 : 0;
			
		case giAudio_Setup_Window:
			return pContext->IsWindowVisible(gWndSetup) == true ? 1 : 0;
			
		case giProject_ID_Window:
			return pContext->IsWindowVisible(gWndProjectID) == true ? 1 : 0;
			
		case giSign_In_Window:
			return pContext->IsWindowVisible(gWndSignIn) == true ? 1 : 0;
			
	}

	return 0;
}

void CAppHost::ActivateWindow(tint32 iIndex)
{
	switch(iIndex) {
		case giMain_Window:
			pContext->ShowWindow(gWndMain);
			pContext->SelectWindow(gWndMain);
			break;
			
		case giMix_Window:
			pContext->ShowWindow(gWndMixer);
			pContext->SelectWindow(gWndMixer);
			break;

		case giRack_Window:
			pContext->ShowWindow(gWndAUX);
			pContext->SelectWindow(gWndAUX);
			break;

		case giSplash_Window:
			pContext->ShowWindow(gWndSplash);
			pContext->SelectWindow(gWndSplash);
//			gbShowZoom = true;
			break;
			
		case giExport_Audio_Window:
			pContext->ShowWindow(gWndExport);
			pContext->SelectWindow(gWndExport);
			break;
		case giExport_For_Web_Window:
			pContext->ShowWindow(gWndExportForWeb);
			pContext->SelectWindow(gWndExportForWeb);
			break;
		case giImport_Audio_Window:
			pContext->ShowWindow(gWndImport);
			pContext->SelectWindow(gWndImport);
			break;
		case giAudio_Setup_Window:
			pContext->ShowWindow(gWndSetup);
			pContext->SelectWindow(gWndSetup);
			break;
		case giProject_ID_Window:
			pContext->ShowWindow(gWndProjectID);
			pContext->SelectWindow(gWndProjectID);
			break;
		case giSign_In_Window:
			pContext->ShowWindow(gWndSignIn);
			pContext->SelectWindow(gWndSignIn);
			break;
	}
}

void CAppHost::HideWindow(tint32 iIndex)
{
	switch(iIndex) {
		case giMain_Window:
			pContext->ShowWindow(gWndMain,false);
			break;
		case giMix_Window:
			pContext->ShowWindow(gWndMixer, false);
			break;
		case giRack_Window:
			pContext->ShowWindow(gWndAUX, false);
			break;
		case giSplash_Window:
			pContext->ShowWindow(gWndSplash, false);
			break;
		case giExport_Audio_Window:
			pContext->ShowWindow(gWndExport, false);
			break;
		case giExport_For_Web_Window:
			pContext->ShowWindow(gWndExportForWeb, false);
			break;
		case giImport_Audio_Window:
			pContext->ShowWindow(gWndImport, false);
			break;
		case giAudio_Setup_Window:
			pContext->ShowWindow(gWndSetup, false);
			break;
		case giProject_ID_Window:
			pContext->ShowWindow(gWndProjectID, false);
			break;
		case giSign_In_Window:
			pContext->ShowWindow(gWndSignIn, false);
			break;
	}
}

void OnMenuWindows(int iPopupIndex)
{
}


//--------------------------------------------------------------------------------------------
static void HandleNew_Audio()
{
	gpDSP = dynamic_cast<CDSP*>(gpPlugIn->GetDSPEngine());
	gpDSP->SetAudioCallback_Duplex(new CCallback(true, true));
	gpDSP->SetAudioCallback_Input(new CCallback(true, false));
	gpDSP->SetAudioCallback_Output(new CCallback(false, true));
	tbool bAudioDevice_LastOpenCrashed = false;
	tbool bMidiDevice_LastOpenCrashed = false;
	tbool bAudioDevice_LastOpenCrashedInput = false;

	gpDSP->OpenAudioDevice_FromPrefs_Output(&bAudioDevice_LastOpenCrashed, &bMidiDevice_LastOpenCrashed);
	if (gpDSP->mPrefs_1.cbAudioDevice_LastOpenSuccess == 0) {
		tchar pszMsg[1024];
		tint32 iAfter = 0;
		if (bAudioDevice_LastOpenCrashed) {
			iAfter = sprintf(pszMsg, "Didn't try to open audio device because it seems we crashed doing that before.");
		}
		else if (bMidiDevice_LastOpenCrashed) {
			iAfter = sprintf(pszMsg, "Didn't try to open midi device because it seems we crashed doing that before.");
		}
		else {
			iAfter = sprintf(pszMsg, "Unable to open audio output device.");
		}

		tchar* pszAfter = pszMsg + iAfter;
		sprintf(pszAfter, "\nPlease change settings in the System Setup dialog.");
	
		ShowMessageBox(pszMsg, "Sorry");
	}

	gpDSP->OpenAudioDevice_FromPrefs_Input(&bAudioDevice_LastOpenCrashedInput);
	if (gpDSP->mPrefs_2.cbInputDevice_LastOpenSuccess == 0) {
		tchar pszMsg[1024];
		tint32 iAfter = 0;
		if (bAudioDevice_LastOpenCrashedInput) {
			iAfter = sprintf(pszMsg, "Didn't try to open audio input device because it seems we crashed doing that before.");
		}
		else {
			iAfter = sprintf(pszMsg, "Unable to open audio input device.");
		}

		tchar* pszAfter = pszMsg + iAfter;
		sprintf(pszAfter, "\nPlease change settings in the System Setup dialog.");

		ShowMessageBox(pszMsg, "Sorry");
	}
}

static void HandleNew_MIDI()
{

	// Lasse, this shall be re-included later!
	/*
	mioe::IDeviceManager* pMIDIManager = mioe::IDeviceManager::Create();
	if (gPrefCur.iMIDIDevice >= pMIDIManager->GetNrOfInputDevices()) {
		gPrefCur.iMIDIDevice = 0;
	}
	mioe::IIDevice* pMIDI = pMIDIManager->CreateInputDevice(gPrefCur.iMIDIDevice);
	*/
}

static void HandleNew_Menu()
{

	//-----------------------------------
	// Settings menu
	ge::IContext::SMenuItemList MenuItemsSetup;
	MenuItemsSetup.uiItemCount = 7;
	MenuItemsSetup.pItems[0] = ge::IContext::SMenuItem("About Koblo Studio");
	MenuItemsSetup.pItems[1] = ge::IContext::SMenuItem("Audio Setup");
	MenuItemsSetup.pItems[2] = ge::IContext::SMenuItem("-");
	MenuItemsSetup.pItems[3] = ge::IContext::SMenuItem("Set Username and Password");
	MenuItemsSetup.pItems[4] = ge::IContext::SMenuItem("Clear Username and Password");
	MenuItemsSetup.pItems[5] = ge::IContext::SMenuItem("-");
	MenuItemsSetup.pItems[6] = ge::IContext::SMenuItem("Project License");

	//-----------------------------------
	// file menu
	ge::IContext::SMenuItemList MenuItemsFile;
	MenuItemsFile.uiItemCount = 12;
	MenuItemsFile.pItems[0] = ge::IContext::SMenuItem("New Project", false, NULL, 'N');
	MenuItemsFile.pItems[1] = ge::IContext::SMenuItem("Open Project", false, NULL, 'O');
	MenuItemsFile.pItems[2] = ge::IContext::SMenuItem("Save Project", false, NULL, 'S');
	MenuItemsFile.pItems[3] = ge::IContext::SMenuItem("Save Project As");
	MenuItemsFile.pItems[4] = ge::IContext::SMenuItem("Close Project", false, NULL, 'W');
	MenuItemsFile.pItems[5] = ge::IContext::SMenuItem("Revert to Saved", false, NULL, 'R');
	MenuItemsFile.pItems[6] = ge::IContext::SMenuItem("-");
	MenuItemsFile.pItems[7] = ge::IContext::SMenuItem("Import Audio", false, NULL, 'I');
//	MenuItemsFile.pItems[8] = ge::IContext::SMenuItem("Import KSProject");
	MenuItemsFile.pItems[8] = ge::IContext::SMenuItem("Export Audio", false, NULL, 'E');
	MenuItemsFile.pItems[9] = ge::IContext::SMenuItem("-");
	MenuItemsFile.pItems[10] = ge::IContext::SMenuItem("Download Project", false, NULL, 'T');
//	MenuItemsFile.pItems[11] = ge::IContext::SMenuItem("Update Project");
	MenuItemsFile.pItems[11] = ge::IContext::SMenuItem("Upload Project", false, NULL, 'U');
//	MenuItemsFile.pItems[13] = ge::IContext::SMenuItem("Commit Changes", false, NULL, 'Y');
	
	//-----------------------------------
	// edit menu
	ge::IContext::SMenuItemList MenuItemsTrack;
	MenuItemsTrack.uiItemCount = 12;
	MenuItemsTrack.pItems[0] = ge::IContext::SMenuItem("Undo", false, NULL, 'Z');
	MenuItemsTrack.pItems[1] = ge::IContext::SMenuItem("-");
	MenuItemsTrack.pItems[2] = ge::IContext::SMenuItem("Copy Region", false, NULL, 'C');
	MenuItemsTrack.pItems[3] = ge::IContext::SMenuItem("Cut Selection", false, NULL, 'X');
	MenuItemsTrack.pItems[4] = ge::IContext::SMenuItem("Paste Region", false, NULL, 'V');
	MenuItemsTrack.pItems[5] = ge::IContext::SMenuItem("Duplicate Region", false, NULL, 'D');
	MenuItemsTrack.pItems[6] = ge::IContext::SMenuItem("-");
	MenuItemsTrack.pItems[7] = ge::IContext::SMenuItem("Add Track", false, NULL, 'M');
	MenuItemsTrack.pItems[8] = ge::IContext::SMenuItem("Delete Track");
	MenuItemsTrack.pItems[9] = ge::IContext::SMenuItem("-");
	MenuItemsTrack.pItems[10] = ge::IContext::SMenuItem("Loop Selection", false, NULL, 'K');
	MenuItemsTrack.pItems[11] = ge::IContext::SMenuItem("Loop", false, NULL, 'L');
	
 
	//-----------------------------------
	// view menu
	ge::IContext::SMenuItemList MenuItemsWindow;
	MenuItemsWindow.uiItemCount = 13;
	MenuItemsWindow.pItems[0] = ge::IContext::SMenuItem("Mixer", false, NULL, '=');
	MenuItemsWindow.pItems[1] = ge::IContext::SMenuItem("Track Editor", false, NULL, '/');
	MenuItemsWindow.pItems[2] = ge::IContext::SMenuItem("AUX Rack", false, NULL, '*');
	MenuItemsWindow.pItems[3] = ge::IContext::SMenuItem("-");
	MenuItemsWindow.pItems[4] = ge::IContext::SMenuItem("View Waveforms", false, NULL, 'B');
	MenuItemsWindow.pItems[5] = ge::IContext::SMenuItem("View Grid", false, NULL, 'G');
	MenuItemsWindow.pItems[6] = ge::IContext::SMenuItem("View Fades", false, NULL, 'F');
	MenuItemsWindow.pItems[7] = ge::IContext::SMenuItem("-");
	MenuItemsWindow.pItems[8] = ge::IContext::SMenuItem("Zoom", false, NULL, '0');
	MenuItemsWindow.pItems[9] = ge::IContext::SMenuItem("Zoom In", false, NULL, '+');
	MenuItemsWindow.pItems[10] = ge::IContext::SMenuItem("Zoom Out", false, NULL, '-');
	MenuItemsWindow.pItems[11] = ge::IContext::SMenuItem("-");
	MenuItemsWindow.pItems[12] = ge::IContext::SMenuItem("Jump to Mouse", false, NULL, 'J');
	
	//-----------------------------------
	// tools menu
	ge::IContext::SMenuItemList MenuItemsTools;
	MenuItemsTools.uiItemCount = 4;
	MenuItemsTools.pItems[0] = ge::IContext::SMenuItem("Hand", false, NULL, '1');
	MenuItemsTools.pItems[1] = ge::IContext::SMenuItem("Trim", false, NULL, '2');
	MenuItemsTools.pItems[2] = ge::IContext::SMenuItem("Select", false, NULL, '3');
	MenuItemsTools.pItems[3] = ge::IContext::SMenuItem("Cut", false, NULL, '4');
	
	//-----------------------------------
	// help menu
	ge::IContext::SMenuItemList MenuItemsHelp;
	MenuItemsHelp.uiItemCount = 4;
	MenuItemsHelp.pItems[0] = ge::IContext::SMenuItem("Koblo.com");
	MenuItemsHelp.pItems[1] = ge::IContext::SMenuItem("Online Help", false, NULL, 'H');
	MenuItemsHelp.pItems[2] = ge::IContext::SMenuItem("Getting Started");
	MenuItemsHelp.pItems[3] = ge::IContext::SMenuItem("Short Cuts");
	
	//-----------------------------------
	// Menus
	ge::IContext::SMenuItemList MenuItems;
	MenuItems.uiItemCount = 6;
	MenuItems.pItems[0] = ge::IContext::SMenuItem("Setup", true, &MenuItemsSetup);
	MenuItems.pItems[1] = ge::IContext::SMenuItem("File", true, &MenuItemsFile);
	MenuItems.pItems[2] = ge::IContext::SMenuItem("Edit", true, &MenuItemsTrack);
	MenuItems.pItems[3] = ge::IContext::SMenuItem("View", true, &MenuItemsWindow);
	MenuItems.pItems[4] = ge::IContext::SMenuItem("Tools", true, &MenuItemsTools);
	MenuItems.pItems[5] = ge::IContext::SMenuItem("Help", true, &MenuItemsHelp);
	
	
	
	pContext->CreateMenu(&MenuItems);



#ifdef _Mac
	::InvalMenuBar();
	::DrawMenuBar();
#endif	// _Mac
}

/*
    if (mTimer) {
		::CFRunLoopTimerInvalidate(mTimer);
		mTimer = 0;
	}
*/

#ifdef _Mac
void timerCallbackGUI(CFRunLoopTimerRef timer, void *info)
{
	if (info) {
//		((CGUI*)info)->Idle();
		((CKSApplication*)info)->OnGUITimer();
	}
}
#endif	// _Mac

CBaseGUI* CMainApplication::CreateExtraWindow(tint32 iGUIIndex, void* pszResName, tbool bAlwaysOnTop)
{
	void* Wnd = pContext->CreateExtraWindow(pszResName, ge::SSize(0, 0), bAlwaysOnTop);
	CBaseGUI* pGUI = dynamic_cast<CBaseGUI*>(gpPlugIn->CreateGUI(iGUIIndex));
	pGUI->MakeWindow(Wnd);
	pGUI->Paint();

	pContext->ShowWindow(Wnd);

	return pGUI;
}

void CMainApplication::CloseWindow(void* mhWnd)
{
	pContext->CloseWindow(mhWnd);
}

/*void CMainApplication::ShowZoomTool()
{
	::ShowWindow(gWndZoom);
	::SelectWindow(gWndZoom);
}*/




