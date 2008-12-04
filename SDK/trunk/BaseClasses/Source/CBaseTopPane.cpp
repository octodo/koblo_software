

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

CBaseTopPane::CBaseTopPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI),
	mbHasBypass(false)
{
	// Create preset set
	mpPresetBackground = IChunkFileMemory::Create();
	tint32 iVersionNumber = 1;
	mpPresetBackground->Open(IFile::FileCreate, iVersionNumber);

	// Read current preset into background preset set
	mpGUI->OnSavePreset(dynamic_cast<IChunkFile*>(mpPresetBackground));

	// Current set is A
	miPresetSetCurrent = 0;
}

CBaseTopPane::~CBaseTopPane()
{
}

void CBaseTopPane::Init()
{
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(10000, 10000));
}

ge::IButton* CBaseTopPane::InsertLoadButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames)
{
	ge::IButton* pButton =CreateButton(giBaseControlIDTopPaneLoad,
		uiResourceID,
		Pos,
		bMouseStateFrames);
//	pButton->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	return pButton;
}

ge::IButton* CBaseTopPane::InsertSaveButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames)
{
	// Save
	ge::IButton* pButton = CreateButton(giBaseControlIDTopPaneSave,
		uiResourceID,
		Pos,
		bMouseStateFrames);
//	pButton->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	return pButton;
}

ge::IButton* CBaseTopPane::InsertSaveAsButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames)
{
	// Save As
	ge::IButton* pButton = CreateButton(giBaseControlIDTopPaneSaveAs,
		uiResourceID,
		Pos,
		bMouseStateFrames);
//	pButton->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	return pButton;
}

ge::IRadioButtons* CBaseTopPane::InsertABButtons(tuint32 uiResourceIDA, const ge::SPos& PosA, tuint32 uiResourceIDB, const ge::SPos& PosB, tbool bMouseStateFrames)
{
/*	// Not implemented yet
	ASSERT(bMouseStateFrames == false);

	// A/B Radio buttons group
	ge::IRadioButtons* pRadioButtonGroup = mpGUI->CreateRadioButtonsGroup(giBaseControlIDTopPaneABGroup,
		ge::SPos(0, 0),
		mpPane);
//	pRadioButtonGroup->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	// A
	ge::IMultiStateButton* pMultiStateButton;
	pMultiStateButton = mpGUI->CreateRadioButton(giBaseControlIDTopPaneA,
		uiResourceIDA,
		PosA,
		pRadioButtonGroup);
	pMultiStateButton->SetEventMouseUp();

	// B
	pMultiStateButton = mpGUI->CreateRadioButton(giBaseControlIDTopPaneB,
		uiResourceIDB,
		PosB,
		pRadioButtonGroup);
	pMultiStateButton->SetEventMouseUp();

	// Set the radio button
	pRadioButtonGroup->SetValue(miPresetSetCurrent);

	return pRadioButtonGroup;*/
	return NULL;
}

ge::IButton* CBaseTopPane::InsertAToBButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames)
{
	// AB
	ge::IButton* pButton = CreateButton(giBaseControlIDTopPaneAB,
		uiResourceID,
		Pos,
		bMouseStateFrames);
//	pButton->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	return pButton;
}

ge::IButton* CBaseTopPane::InsertBToAButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames)
{
	// BA
	ge::IButton* pButton = CreateButton(giBaseControlIDTopPaneBA,
		uiResourceID,
		Pos,
		bMouseStateFrames);
//	pButton->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	return pButton;
}

ge::IMultiStateButton* CBaseTopPane::InsertMIDILearnButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames)
{
	mpGUI->DisableMIDIAutomaticTurnOff();

	ge::IMultiStateButton* pMultiStateButton = Create2StateButton(giBaseControlIDTopPaneMIDILearn,
		uiResourceID,
		Pos,
		bMouseStateFrames);
	pMultiStateButton->SetEventMouseUp();
//	pMultiStateButton->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	return pMultiStateButton;
}

ge::IMultiStateButton* CBaseTopPane::InsertMIDIUnlearnButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames)
{
	ge::IMultiStateButton* pMultiStateButton = Create2StateButton(giBaseControlIDTopPaneMIDIUnlearn,
		uiResourceID,
		Pos,
		bMouseStateFrames);
	pMultiStateButton->SetEventMouseUp();
//	pMultiStateButton->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	return pMultiStateButton;
}

ge::IButton* CBaseTopPane::InsertMIDIResetButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames)
{
	ge::IButton* pButton = CreateButton(giBaseControlIDTopPaneMIDIReset,
		uiResourceID,
		Pos,
		bMouseStateFrames);
//	pButton->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	return pButton;
}

ge::IMultiStateButton* CBaseTopPane::InsertBypassButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames)
{
	ge::IMultiStateButton* pMultiStateButton = Create2StateButton(giBaseControlIDTopPaneBypass,
		uiResourceID,
		Pos,
		bMouseStateFrames);
	pMultiStateButton->SetEventMouseUp();
//	pMultiStateButton->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	mbHasBypass = true;

	return pMultiStateButton;
}

void CBaseTopPane::RegisterControls()
{
/*	if (mbHasBypass) {
		mpGUI->RegisterGlobalControl(0,
			giBaseControlIDTopPaneBypass,
			0);
	}*/
}

void CBaseTopPane::EventValueChange(ge::IControl *pSender, tint32 iValueNew)
{
	switch(pSender->GetID()) {
		case giBaseControlIDTopPaneLoad:
			ActionLoad();
			break;

		case giBaseControlIDTopPaneSave:
#ifdef _DEMO_
			mpGUI->GetWindow()->ShowMessageBox("This feature is disabled in the demo version", "Info");
#else	// _DEMO_
			ActionSave();
#endif
			break;

		case giBaseControlIDTopPaneSaveAs:
#ifdef _DEMO_
			mpGUI->GetWindow()->ShowMessageBox("This feature is disabled in the demo version", "Info");
#else	// _DEMO_
			ActionSaveAs();
#endif
			break;

		case giBaseControlIDTopPaneABGroup:
			ActionAB(iValueNew);
			break;

		case giBaseControlIDTopPaneAB:
			ActionAToB();
			break;

		case giBaseControlIDTopPaneBA:
			ActionBToA();
			break;

		case giBaseControlIDTopPaneBypass:
			ActionBypass(iValueNew == 0 ? false : true);
			break;

		case giBaseControlIDTopPaneMIDILearn:
#ifdef _DEMO_
			mpGUI->GetWindow()->ShowMessageBox("This feature is disabled in the demo version", "Info");
#else	// _DEMO_
			// Set flag for MIDI Learn process
			mpGUI->SetMIDILearn(iValueNew != 0);

			// Maybe abort MIDI Unlearn process
			if (mpGUI->GetMIDILearn()) {
				mpGUI->SetMIDIUnlearn(false);
			}

			// Sanitize blinking buttons
			MIDICyclingOnOff();
#endif	// _DEMO_
			break;

		case giBaseControlIDTopPaneMIDIUnlearn:
#ifdef _DEMO_
			mpGUI->GetWindow()->ShowMessageBox("This feature is disabled in the demo version", "Info");
#else	// _DEMO_
			// Set flag for MIDI Unlearn process
			mpGUI->SetMIDIUnlearn(iValueNew != 0);

			// Maybe abort MIDI Learn process
			if (mpGUI->GetMIDIUnlearn()) {
				mpGUI->SetMIDILearn(false);
			}

			// Sanitize blinking buttons
			MIDICyclingOnOff();
#endif	// _DEMO_
			break;

		case giBaseControlIDTopPaneMIDIReset:
#ifdef _DEMO_
			mpGUI->GetWindow()->ShowMessageBox("This feature is disabled in the demo version", "Info");
#else	// _DEMO_
			MIDIReset();
#endif	// _DEMO_
			break;
	}
}

void CBaseTopPane::EventGeneric(ge::IControl *pSender, void *pEventData)
{
}

void CBaseTopPane::ActionLoad()
{
	// Get the directory
	std::string sPresetDirectory = mpGUI->GetPresetFolder();
	
	// Get product name
	tchar pszProductName[128];
	mpGUI->GetPlugIn()->GetProductName(pszProductName);

	// Create the dialog
	CAutoDelete<ge::IOpenDialog> pDialog(ge::IOpenDialog::Create());

	// Open dialog
	tchar pszPathName[512];
	pDialog->DoDialog((tchar*)pszPathName,
		(tchar*)(sPresetDirectory.c_str()),
		(tchar*)"*.preset",
		(tchar*)(std::string(pszProductName) + " Presets (*.preset)").c_str());

	if (pszPathName[0] == 0) {
		// Error in dialog, or user pressed cancel
		return;
	}

	msPresetPathName = std::string((char*)pszPathName);

	CAutoDelete<IChunkFile> pFile(IChunkFile::Create());
	tint32 iVersionNumber;
	if (pFile->Open((const tchar*)(msPresetPathName.c_str()),
		IFile::FileRead,
		iVersionNumber)) {
		mpGUI->OnLoadPreset(pFile);
	}
}

void CBaseTopPane::ActionSave()
{
#ifndef _DEMO_
	if (msPresetPathName.size() == 0) {
		// No current preset, so do a save as and return
		ActionSaveAs();
		return;
	}

	CAutoDelete<IChunkFile> pFile(IChunkFile::Create());
	tint32 iVersionNumber = 1;
	if (pFile->Open((const tchar*)(msPresetPathName.c_str()),
		IFile::FileCreate,
		iVersionNumber)) {
		mpGUI->OnSavePreset(pFile);
	}
#endif	// _DEMO_
}

void CBaseTopPane::ActionSaveAs()
{
#ifndef _DEMO_
	// Get the directory
	std::string sPresetDirectory = mpGUI->GetPresetFolder();

	// Get product name
	tchar pszProductName[128];
	mpGUI->GetPlugIn()->GetProductName(pszProductName);
	
	// Create the dialog
	CAutoDelete<ge::ISaveAsDialog> pDialog(ge::ISaveAsDialog::Create());

	// Open dialog
	tchar pszPathName[512];
	pDialog->DoDialog((tchar*)pszPathName,
		(tchar*)(sPresetDirectory.c_str()),
		(tchar*)"*.preset",
	    (tchar*)(std::string(pszProductName) + " Presets (*.preset)").c_str(),
		NULL);

	if (pszPathName[0] == 0) {
		// Error in dialog, or user pressed cancel
		return;
	}

	msPresetPathName = std::string((char*)pszPathName);

	CAutoDelete<IChunkFile> pFile(IChunkFile::Create());
	tint32 iVersionNumber = 1;
	if (pFile->Open((const tchar*)(msPresetPathName.c_str()),
		IFile::FileCreate,
		iVersionNumber)) {
		mpGUI->OnSavePreset(pFile);
	}
#endif	// _DEMO_
}

void CBaseTopPane::ActionAB(tint iIndex)
{
	if (miPresetSetCurrent != iIndex) {
		// Switch to set not current
		CopyBackgroundPresetToForeground();

		miPresetSetCurrent = iIndex;
	}
}

void CBaseTopPane::ActionAToB()
{
	if (miPresetSetCurrent == 0) {
		CopyForegroundPresetToBackground();
	}
	else {
		CopyBackgroundPresetToForeground();
	}
}

void CBaseTopPane::ActionBToA()
{
	if (miPresetSetCurrent == 0) {
		CopyBackgroundPresetToForeground();
	}
	else {
		CopyForegroundPresetToBackground();
	}
}

void CBaseTopPane::CopyBackgroundPresetToForeground()
{
	// Create new background set
	IChunkFileMemory* pPresetBackgroundNew = IChunkFileMemory::Create();
	tint32 iVersionNumber = 1;
	pPresetBackgroundNew->Open(IFile::FileCreate, iVersionNumber);

	// Read current preset into background preset set
	mpGUI->OnSavePreset(dynamic_cast<IChunkFile*>(pPresetBackgroundNew));

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
	mpGUI->OnLoadPreset(pFile);

	// Delete the memory
	delete[] pMem;
}

void CBaseTopPane::CopyForegroundPresetToBackground()
{
	// Destroy current background set
	mpPresetBackground->Destroy();

	// Create preset set
	mpPresetBackground = IChunkFileMemory::Create();
	tint32 iVersionNumber = 1;
	mpPresetBackground->Open(IFile::FileCreate, iVersionNumber);

	// Read current preset into background preset set
	mpGUI->OnSavePreset(dynamic_cast<IChunkFile*>(mpPresetBackground));
}

void CBaseTopPane::ActionBypass(tbool bBypass)
{
	mpGUI->SetBypass(bBypass);
}

void CBaseTopPane::MIDILearnOrUnlearnAbort()
{
	mpGUI->SetMIDILearn(false);
	mpGUI->SetMIDIUnlearn(false);

	// Stop MIDI blinking
	MIDICyclingOnOff();
}

void CBaseTopPane::MIDICyclingOnOff()
{
	ge::IMultiStateButton* pButtonLearn = dynamic_cast<ge::IMultiStateButton*>(mpPane->GetControl(giBaseControlIDTopPaneMIDILearn));
	ge::IMultiStateButton* pButtonUnlearn = dynamic_cast<ge::IMultiStateButton*>(mpPane->GetControl(giBaseControlIDTopPaneMIDIUnlearn));

	if (pButtonLearn->IsCycling() != mpGUI->GetMIDILearn()) {
		pButtonLearn->Cycle(mpGUI->GetMIDILearn(), 500);

		tbool bMIDILearnCurrent = (pButtonLearn->GetValue()!= 0);
		if (bMIDILearnCurrent != mpGUI->GetMIDILearn()) {
			tint iNewValue = (mpGUI->GetMIDILearn()) ? 1 : 0;
			pButtonLearn->SetValue(iNewValue, false);
		}
	}
	if (pButtonUnlearn->IsCycling() != mpGUI->GetMIDIUnlearn()) {
		pButtonUnlearn->Cycle(mpGUI->GetMIDIUnlearn(), 500);

		tbool bMIDIUnLearnCurrent = (pButtonUnlearn->GetValue() != 0);
		if (bMIDIUnLearnCurrent != mpGUI->GetMIDIUnlearn()) {
			tint iNewValue = (mpGUI->GetMIDIUnlearn()) ? 1 : 0;
			pButtonUnlearn->SetValue(iNewValue, false);
		}
	}
}

void CBaseTopPane::MIDICCLearn(tint32 iCtrlID)
{
	// Stop buttons blinking
	MIDICyclingOnOff();
}

void CBaseTopPane::MIDIReset()
{
	ge::IWindow::EMsgBoxReturn Button = mpGUI->GetWindow()->ShowMessageBox((tchar*)"Your custom MIDI learn assignment will be lost and the factory default reinstated\n\nDo you wish to continue?", (tchar*)"Question", ge::IWindow::MsgBoxYesNo);
	if (Button != ge::IWindow::MsgBoxRetYes) {
		return;
	}

	// Stop any ongoing MIDI Learn or Unlearn operation
	MIDILearnOrUnlearnAbort();

	// Reset all MIDI
	mpGUI->GetPlugIn()->MIDICCReset();
}




