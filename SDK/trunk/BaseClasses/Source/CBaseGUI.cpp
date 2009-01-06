

#ifdef WIN32

// Base Utilities
#include "bu.h"

// Graphical Engine
#include "ge.h"

// KSPI
#include "kspi.h"
#include "kspiC.h"

// Base Classes
#include "BaseControlIDs.h"
#include "FontSizes.h"
#include "IBaseDezipperCallback.h"
#include "CBaseMIDICCEvent.h"
#include "CBaseDezipper.h"
#include "CBaseDezipper2.h"
#include "CBaseDSPEngine.h"
#include "CBaseModule.h"
#include "CBasePlugIn.h"
#include "CBasePane.h"
#include "CBasePaneStd.h"
#include "CBaseGUI.h"
#include "CBaseTopPane.h"

#endif	// WIN32

CBaseGUI::CBaseGUI(CBasePlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: mpContext(NULL), mpResMan(NULL),
	mpPlugIn(pPlugIn), mpWindow(NULL), mpParmMan(pParmMan), mpPresetBackground(NULL),
	mbMIDILearn(false), mbMIDIUnLearn(false), mbBufferedDrawing(false)
#ifdef WIN32
, mhWndParent(NULL)
#endif	// WIN32
{
	mpMIDICCCallback = new CBaseMIDICCEvent(this);

	mbMIDIAutomaticTurnOff = true;
}

CBaseGUI::~CBaseGUI()
{
	if (mpMIDICCCallback) {
		delete mpMIDICCCallback;
	}

	if (mpPresetBackground) {
		mpPresetBackground->Destroy();
	}

	if (mpWindow) {
		mpWindow->Destroy();
	}
	if (mpResMan) {
		mpResMan->Destroy();
	}
	if (mpContext) {
		mpContext->Destroy();
	}

//	delete GetPane();
}

void CBaseGUI::PreCreate()
{
	// Create context
	mpContext = ge::IContext::Create();
	// Set context callback
	mpContext->SetCallback(dynamic_cast<ge::IContextCallback*>(this));
	// Pass it the instance handle
	mpContext->SetInstanceData((void*)mpPlugIn->GetModule()->GetInstanceHandle());
	// Initialize context
	mpContext->Init();

	// Create preset set
	mpPresetBackground = IChunkFileMemory::Create();
	tint32 iVersionNumber = 1;
	mpPresetBackground->Open(IFile::FileCreate, iVersionNumber);

	OnSavePreset(dynamic_cast<IChunkFile*>(mpPresetBackground));

	CreateResourceManager();
}


// Lasse, added 2008-06-03
#ifdef _RESOURCE_FILE_HARDLINKED
	#include "Source/ResourcesInc.cpp"
#endif // #ifdef _RESOURCE_FILE_HARDLINKED
// .. Lasse


void CBaseGUI::CreateResourceManager()
{
#ifdef _RESOURCE_FILE_HARDLINKED
	// Lasse, added 2008-06-03
	// New functionality, no .k2s file needs loading, instead link data in directly
	mpResMan = ge::IResourceManager::Create();
	IChunkFileMemory* pResFile = IChunkFileMemory::Create();
	tint32 iFileVersion;
	pResFile->Open(IFile::FileRead, iFileVersion, gp_res_k2s, giSize_res_k2s);
	mpResMan->AddResourceFile(pResFile);
	// .. Lasse

#else _RESOURCE_FILE_HARDLINKED
#ifdef WIN32
	std::string sPathName = GetApplicationFolder();
#endif	// WIN32
#ifdef _Mac
	std::string sPathName = GetResourceFolder();
#endif	// _Mac

	sPathName += mpPlugIn->GetProductName();
	sPathName += ".k2s";
	
//	std::cerr << sPathName.c_str();

	// We have the resource file name, so use it to create and initialize the resource manager
	mpResMan = ge::IResourceManager::Create();
	IChunkFile* pResFile = IChunkFile::Create();
	tint32 iFileVersion;
	if (pResFile->Open((const tchar*)sPathName.c_str(), IFile::FileRead, iFileVersion) == false) {
		tchar psz[512];
		sprintf(psz, "Resource file was not found at location %s", sPathName.c_str());
#ifdef _Mac
		// On OSX we can only show 255 characters in a dialogue box, so we need to limit the string
		std::string s(psz);
		if (s.size() > 128) {
			s = s.substr(0, 128);
			strcpy(psz, s.c_str());
		}
#endif	// _Mac
		throw IException::Create(IException::TypeFile,
			IException::ReasonFileCannotOpen,
			EXCEPTION_INFO,
			psz);
	}
	mpResMan->AddResourceFile(pResFile);

#endif // #else _RESOURCE_FILE_HARDLINKED

/*	tchar pszErr[512];
	pszErr[0] = '\0';
#ifdef WIN32
	std::string sPathName = GetApplicationFolder();
	tint iRetries = 0;
#endif	// WIN32
#ifdef _Mac
	//std::string sPathName = GetResourceFolder();
	std::string sPathName = std::string("/Library/Application Support/") + mpPlugIn->GetCompanyName() + "/";
	sPathName += mpPlugIn->GetProductName() + "/";
	tint iRetries = 1;
#endif	// _Mac

	do
	{
		sPathName += mpPlugIn->GetProductName();
		sPathName += ".k2s";

		// We have the resource file name, so use it to create and initialize the resource manager
		mpResMan = ge::IResourceManager::Create();
		IChunkFile* pResFile = IChunkFile::Create();
		tint32 iFileVersion;
		if (pResFile->Open((const tchar*)sPathName.c_str(), IFile::FileRead, iFileVersion)) {
			// The file opened - now load it and exit
			mpResMan->AddResourceFile(pResFile);
			return;
		}
		else {
			if (pszErr[0] == '\0')
				sprintf(pszErr, "Resource file was not found at location %s", sPathName.c_str());
#ifdef _Mac
			switch (iRetries) {
				case 1:	sPathName = GetResourceFolder(); break;
			}
#endif // #ifdef _Mac
		}
	}
	while (iRetries--);

	// No more retries - report error
#ifdef _Mac
	// On OSX we can only show 255 characters in a dialogue box, so we need to limit the string
	std::string s(pszErr);
	if (s.size() > 128) {
		s = s.substr(0, 128);
		strcpy(pszErr, s.c_str());
	}
#endif	// _Mac
	throw IException::Create(IException::TypeFile,
		IException::ReasonFileCannotOpen,
		EXCEPTION_INFO,
		pszErr);*/
}


#ifdef _Mac
void DoMsgBox(const tchar* psz) {
	std::string s(psz);
#ifdef _Mac
	// On OSX we can only show 255 characters in a dialogue box, so we need to limit the string
	if (s.size() > 240) {
		s = s.substr(0, 240);
	}
#endif	// _Mac

	Str255 String;
	// Convert to pascal string
	String[0] = s.size();
	strcpy((char*)&(String[1]), s.c_str());

	AlertStdAlertParamRec ParamRec;
	ParamRec.movable = 1;
	ParamRec.helpButton = 0;
	ParamRec.filterProc = nil;
	ParamRec.defaultButton = kAlertStdAlertOKButton;
	ParamRec.cancelButton = kAlertStdAlertCancelButton;
	ParamRec.position = kWindowAlertPositionParentWindowScreen;
	ParamRec.defaultText = "\pOK";
	ParamRec.cancelText = nil;
	ParamRec.otherText = nil;

	SInt16 iItemHit;
	::StandardAlert(kAlertPlainAlert,
		String,
		NULL,
		&ParamRec,
		&iItemHit);
}
#endif	// _Mac

#ifdef WIN32
void DoMsgBox(const tchar* psz) {
	::MessageBox(NULL, psz, NULL, MB_OK);
}
#endif	// WIN32

void CBaseGUI::MakeWindow(void* p)
{
	try {
		PreCreate();

		// Create the window
		mpWindow = ge::IWindow::Create();
		// Tell the window what context it's in
		mpWindow->SetContext(mpContext);

	#ifdef WIN32
		mhWndParent = (HWND)p;
	#endif
	#ifdef _Mac
		/*void**/ mhWndParent = p;
	#endif

		tuint32 iSizeX;
		tuint32 iSizeY;
		GetSize(&iSizeX, &iSizeY);

		ge::SRect rct(0, 0, iSizeX, iSizeY);
		mpWindow->MakeWindow(mhWndParent, &rct);

		Initialize();

		// Make the window visible
		mpWindow->ShowWindow();

		mpWindow->InstallKeyboardHook();

		mpWindow->AcquireKeyboardInput();
	}
	catch(IException* pException) {
		const tchar* psz = pException->GetFullDescription();
		
		DoMsgBox(psz);
		
		pException->Destroy();
	}
	catch(...) {
		DoMsgBox("Unknown Exception");
	}
}

void CBaseGUI::PrepareBuffer()
{
	mbBufferedDrawing = true;

	PreCreate();

	// Create the window
	mpWindow = ge::IWindow::Create();
	// Tell the window what context it's in
	mpWindow->SetContext(mpContext);

	#ifdef WIN32
		mhWndParent = (HWND)NULL;
	#endif

	tuint32 iSizeX;
	tuint32 iSizeY;
	GetSize(&iSizeX, &iSizeY);

	ge::SRect rct(0, 0, iSizeX, iSizeY);
	mpWindow->PrepareBuffer(&rct);

	Initialize();
}

void CBaseGUI::DisconnectControls()
{
	mpParmMan->DeregisterAllControls();
}

void CBaseGUI::Paint()
{
	if (mpWindow) {
		ge::SSize Size;
		mpWindow->GetSize(Size);
		ge::SRect Rect(ge::SPos(0, 0), Size);
		mpWindow->Redraw(Rect);
	}
}

void CBaseGUI::OnDraw(tuchar* puchBuffer, tuint32 uiInterleave, tint32 iUpdateX, tint32 iUpdateY, tint32 iUpdateCX, tint32 iUpdateCY)
{
	ge::SRect RectUpdate(iUpdateX, iUpdateY, iUpdateCX, iUpdateCY);

	if (mpWindow) {
		mpWindow->PaintToBuffer(puchBuffer, uiInterleave, RectUpdate);
	}
}

void CBaseGUI::OnMouseDown(tuint32 iPosX, tuint32 iPosY)
{
	if (mpWindow) {
		mpWindow->MouseDown(ge::SPos(iPosX, iPosY));
	}
}

void CBaseGUI::OnMouseLeftButtonDown(tint32 iPosX, tint32 iPosY)
{
	if (mpWindow) {
		ge::SPos Pos(iPosX, iPosY);
		mpWindow->RelayMouse(ge::LeftButtonDown, Pos);

		tint32 iX, iY, iCX, iCY;
		mpWindow->GetInvalidatedRect(iX, iY, iCX, iCY);
		ge::SRect Rect(iX, iY, iCX, iCY);
		if (iCX != 0 && iCY != 0) {
			// The rect has a size, so it is valid
			gpApplication->GetModule()->GetHost()->RedrawWindow(dynamic_cast<IGUI*>(this), iX, iY, iCX, iCY);
		}
	}
}

void CBaseGUI::OnMouseLeftButtonUp(tint32 iPosX, tint32 iPosY)
{
	if (mpWindow) {
		ge::SPos Pos(iPosX, iPosY);
		mpWindow->RelayMouse(ge::LeftButtonUp, Pos);

		tint32 iX, iY, iCX, iCY;
		mpWindow->GetInvalidatedRect(iX, iY, iCX, iCY);
		ge::SRect Rect(iX, iY, iCX, iCY);
		if (iCX != 0 && iCY != 0) {
			// The rect has a size, so it is valid
			gpApplication->GetModule()->GetHost()->RedrawWindow(dynamic_cast<IGUI*>(this), iX, iY, iCX, iCY);
		}
	}
}

void CBaseGUI::OnMouseMove(tint32 iPosX, tint32 iPosY)
{
	if (mpWindow) {
		ge::SPos Pos(iPosX, iPosY);
		mpWindow->RelayMouse(ge::MouseMove, Pos);

		tint32 iX, iY, iCX, iCY;
		mpWindow->GetInvalidatedRect(iX, iY, iCX, iCY);
		ge::SRect Rect(iX, iY, iCX, iCY);
		if (iCX != 0 && iCY != 0) {
			// The rect has a size, so it is valid
			gpApplication->GetModule()->GetHost()->RedrawWindow(dynamic_cast<IGUI*>(this), iX, iY, iCX, iCY);
		}
	}
}

void CBaseGUI::OnMouseRightButtonDown(tint32 iPosX, tint32 iPosY)
{
	if (mpWindow) {
		ge::SPos Pos(iPosX, iPosY);
		mpWindow->RelayMouse(ge::RightButtonDown, Pos);

		tint32 iX, iY, iCX, iCY;
		mpWindow->GetInvalidatedRect(iX, iY, iCX, iCY);
		ge::SRect Rect(iX, iY, iCX, iCY);
		if (iCX != 0 && iCY != 0) {
			// The rect has a size, so it is valid
			gpApplication->GetModule()->GetHost()->RedrawWindow(dynamic_cast<IGUI*>(this), iX, iY, iCX, iCY);
		}
	}
}

void CBaseGUI::OnMouseRightButtonUp(tint32 iPosX, tint32 iPosY)
{
	if (mpWindow) {
		ge::SPos Pos(iPosX, iPosY);
		mpWindow->RelayMouse(ge::RightButtonUp, Pos);

		tint32 iX, iY, iCX, iCY;
		mpWindow->GetInvalidatedRect(iX, iY, iCX, iCY);
		ge::SRect Rect(iX, iY, iCX, iCY);
		if (iCX != 0 && iCY != 0) {
			// The rect has a size, so it is valid
			gpApplication->GetModule()->GetHost()->RedrawWindow(dynamic_cast<IGUI*>(this), iX, iY, iCX, iCY);
		}
	}
}

void CBaseGUI::OnMouseLeftButtonDoubleClick(tint32 iPosX, tint32 iPosY)
{
	if (mpWindow) {
		ge::SPos Pos(iPosX, iPosY);
		mpWindow->RelayMouse(ge::LeftButtonDblClk, Pos);

		tint32 iX, iY, iCX, iCY;
		mpWindow->GetInvalidatedRect(iX, iY, iCX, iCY);
		ge::SRect Rect(iX, iY, iCX, iCY);
		if (iCX != 0 && iCY != 0) {
			// The rect has a size, so it is valid
			gpApplication->GetModule()->GetHost()->RedrawWindow(dynamic_cast<IGUI*>(this), iX, iY, iCX, iCY);
		}
	}
}

void CBaseGUI::OnKeyDown(tuint32 iKeyCode)
{
	if (mpWindow) {
		mpWindow->KeyDown(iKeyCode);
	}
}

void CBaseGUI::OnTimer()
{
	if (mpWindow) {
		if (mbBufferedDrawing) {
			tint32 iX, iY, iCX, iCY;
			mpWindow->GetInvalidatedRect(iX, iY, iCX, iCY);
			ge::SRect Rect(iX, iY, iCX, iCY);
			if (iCX != 0 && iCY != 0) {
				// The rect has a size, so it is valid
				gpApplication->GetModule()->GetHost()->RedrawWindow(dynamic_cast<IGUI*>(this), iX, iY, iCX, iCY);
			}
		}
		else {
			// Ask the window to paint any dirty rectangle
			mpWindow->Paint();

#ifdef _Mac
			mpWindow->OnTimer();
#endif
		}
	}
}

void CBaseGUI::SetControlValue(tint32 iPaneID, tint32 iControlID, tint32 iValue)
{
//	if (mpWindow) {
//		mpWindow->SetControlValue(iControlID, iValue);
//	}
	if (mpPane) {
		mpPane->SetControlValue(iPaneID, iControlID, iValue);
	}
}

void CBaseGUI::SetBypass(tbool bBypass)
{
	// Notify the parm manager that a control has changed
	// (mo) You cannot do this, without knowing which pane the bypass is in. As such the approach may not be the best, and one should consider reacting on parameter changes instead
//	mpParmMan->ControlUpdate(giBaseControlIDTopPaneBypass, bBypass ? 1 : 0);
}

void CBaseGUI::SetPane(CBasePane* pPane)
{
	mpPane = pPane;
}

void CBaseGUI::CopyPreset()
{
	// Destroy current background set
	mpPresetBackground->Destroy();

	// Create preset set
	mpPresetBackground = IChunkFileMemory::Create();
	tint32 iVersionNumber = 1;
	mpPresetBackground->Open(IFile::FileCreate, iVersionNumber);

	// Read current preset into background preset set
	OnSavePreset(dynamic_cast<IChunkFile*>(mpPresetBackground));
}

void CBaseGUI::ABPreset()
{
	// Create new background set
	IChunkFileMemory* pPresetBackgroundNew = IChunkFileMemory::Create();
	tint32 iVersionNumber = 1;
	pPresetBackgroundNew->Open(IFile::FileCreate, iVersionNumber);

	// Read current preset into background preset set
	OnSavePreset(dynamic_cast<IChunkFile*>(pPresetBackgroundNew));

	// Get the memory from the current background set
	tint64 iMemSize = mpPresetBackground->GetMemorySize();
	void* pMem = (void*)new char[(tuint)iMemSize];
	mpPresetBackground->GetMemory(pMem);

	// Delete the current background set
	mpPresetBackground->Destroy();

	// Switch, so the current preset is the new background
	mpPresetBackground = pPresetBackgroundNew;

	// Create chunk with old background
	CAutoDelete<IChunkFileMemory> pFile(IChunkFileMemory::Create());
	pFile->Open(IFile::FileRead, iVersionNumber, pMem, iMemSize);

	// Set the preset
	OnLoadPreset(pFile);

	// Delete the memory
	delete[] pMem;
}

void CBaseGUI::MIDICCLearn(tint32 iCtrlID)
{
/*	if (mbMIDILearn || mbMIDIUnLearn) {
		de::IParameter* pParm = mpParmMan->GetParameterFromControl(iCtrlID);
		tint32 iParamId = pParm->GetID();
		tint32 iMinVal = pParm->GetMinValue();
		tint32 iMaxVal = pParm->GetMaxValue();

		const de::IConverter* pConverter = GetWindow()->GetControl(iCtrlID)->GetConverter();
		kspi::SParameterInfo::ECurveType CurveType;
		tint32 iStrength = 0;
		if (pConverter) {
			switch(pConverter->GetType()) {
				case de::IConverter::TypeLinear:
					// Converter is linear
					CurveType = kspi::SParameterInfo::CurveTypeLinear;
					break;

				case de::IConverter::TypeLog:
					// Converter is log
					CurveType = kspi::SParameterInfo::CurveTypeLog;
					iStrength = pConverter->GetStrength();
					break;

				case de::IConverter::TypeTrueLog:
					// Converter is log
					CurveType = kspi::SParameterInfo::CurveTypeTrueLog;
					iStrength = pConverter->GetStrength();
					break;
			}
		}
		else {
			// No converter
			CurveType = kspi::SParameterInfo::CurveTypeLinear;
		}

		if (mbMIDILearn)
			// Update mpPlugIn->mTempMIDICCInfo
			gpApplication->MIDICCMapLearn(iParamId, iMinVal, iMaxVal, iStrength, CurveType);
		else {
			if (mbMIDIUnLearn) {
				gpApplication->MIDICCMapUnlearn(iParamId);
			}
		}

		if (mbMIDIAutomaticTurnOff) {
			// Turn off MIDI learn
			mbMIDILearn = false;
			mbMIDIUnLearn = false;
		}
	}*/
}

ge::IEventReceiver* CBaseGUI::GetMIDICCReceiver()
{
	return dynamic_cast<ge::IEventReceiver*>(mpMIDICCCallback);
}

void CBaseGUI::DisableMIDIAutomaticTurnOff()
{
	mbMIDIAutomaticTurnOff = false;
}

std::string CBaseGUI::GetApplicationFolder()
{
	return mpPlugIn->GetApplicationFolder();
}

std::string CBaseGUI::GetCompanyFolder()
{
	return mpPlugIn->GetCompanyFolder();
}

std::string CBaseGUI::GetFilesFolder()
{
	return mpPlugIn->GetFilesFolder();
}

std::string CBaseGUI::GetPresetFolder()
{
	return mpPlugIn->GetPresetFolder();
}

std::string CBaseGUI::GetResourceFolder()
{
	return mpPlugIn->GetResourceFolder();
}

std::string CBaseGUI::GetPreferenceFolder()
{
	return mpPlugIn->GetPreferenceFolder();
}

std::string CBaseGUI::GetCompanyPreferenceFolder()
{
	return mpPlugIn->GetCompanyPreferenceFolder();
}

void CBaseGUI::OnLoadPreset(IChunkFile* pFile, tint32 iProductID)
{
	mpPlugIn->OnLoadPreset(pFile, iProductID);
}

void CBaseGUI::OnSavePreset(IChunkFile* pFile, tint32 iProductID)
{
	mpPlugIn->OnSavePreset(pFile, iProductID);
}




