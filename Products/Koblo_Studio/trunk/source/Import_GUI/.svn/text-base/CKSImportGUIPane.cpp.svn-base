
#include "KSOS.h"

const tint32 giPreviewBuffSmall = 1024;



CKSImportGUIPane::CKSImportGUIPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mbPreviewing = false;

	mpfPreview_Input = NULL;
	mpdecPreview = NULL;
	for (tint32 i = 0; i < kiNbOfBuffsInChain; i++) {
		mapfPreview_OutputL[i] = NULL;
		mapfPreview_OutputR[i] = NULL;
	}

	miInFillChain_Level = 0;
	mpTimer = NULL;
} // constructor


CKSImportGUIPane::~CKSImportGUIPane()
{
	if (mpTimer) {
		mpTimer->Destroy();
		mpTimer = NULL;
	}

	PreviewStop();
} // destructor


void CKSImportGUIPane::Init()
{
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(424,424));
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Import, ge::SPos(0, 0));

	mpPop = NULL;

	//-------------------------------------------------
	// Scroll pane viewing the content in a "Folder"
	mpImport_File_Browser = new CKS_Import_File_Browser(this, GetGUI());
	mpImport_File_Browser->SetInfo();
	mpImport_File_Browser->Init();
	mpPane->AddControl(mpImport_File_Browser->GetPane(), ge::SPos(8, 39));
	Create_File_Browser_ScrollBar();
	
	//-------------------------------------------------
	// Scroll pane viewing a list of files to add to te project
	mpImport_Files = new CKS_Import_Files(this, GetGUI());
	mpImport_Files->SetInfo();
	mpImport_Files->Init();
	mpPane->AddControl(mpImport_Files->GetPane(), ge::SPos(216, 39));
	Create_File_ScrollBar();


	CreateButton(giCtrl_Remove_Import, IDB_Button_Move, ge::SPos(195, 345), false);
	CreateButton(giCtrl_Add_Import, IDB_Button_Add, ge::SPos(195, 369), false);
	// Stop
	CreateButton(giCtrl_Stop_Import, IDB_Button_Stop, ge::SPos(195, 369+24), false);
	// Play
	Create2StateButton(giCtrl_Play_Import, IDB_Button_Play, ge::SPos(195+28, 369+24), true);
	
	
	// Cancel / Export
	CreateButton(giCtrl_Cancel_Import, IDB_Button_Cancel, ge::SPos(292, 400), false);
	CreateButton(giCtrl_Import, IDB_Button_Import, ge::SPos(292 + 64, 400), false);

	tchar psz[1024];
	CKSPlugIn* pPlugIn = dynamic_cast<CKSPlugIn*>(GetPlugIn());
	//IFile::GetSystemDirectory(IFile::SystemDirDesktop, psz);
	pPlugIn->GetDefaultProjectFolder(psz);
	BrowseToDir(std::string(psz));

	dynamic_cast<CKSPlugIn*>(GetGUI()->GetPlugIn())->SetPreviewCallback(dynamic_cast<CPreviewCallback*>(this));

	ge::IText* pText = CreateDisplay(giControlType, ge::SPos(96, 333+22), ge::SSize(100, 12), CreateFont(Generic128, IDB_Minix_Tight, ge::SRGB(10, 10, 10)), false);
	pText->SetHorzAlignment(ge::IText::HorzAlignLeft);
	pText = CreateDisplay(giControlBitDepth, ge::SPos(96, 347+22), ge::SSize(100, 12), CreateFont(Generic128, IDB_Minix_Tight, ge::SRGB(10, 10, 10)), false);
	pText->SetHorzAlignment(ge::IText::HorzAlignLeft);
	pText = CreateDisplay(giControlSampleRate, ge::SPos(96, 361+22), ge::SSize(100, 12), CreateFont(Generic128, IDB_Minix_Tight, ge::SRGB(10, 10, 10)), false);
	pText->SetHorzAlignment(ge::IText::HorzAlignLeft);
	pText = CreateDisplay(giControlChannels, ge::SPos(96, 375+22), ge::SSize(100, 12), CreateFont(Generic128, IDB_Minix_Tight, ge::SRGB(10, 10, 10)), false);
	pText->SetHorzAlignment(ge::IText::HorzAlignLeft);
} // Init


void CKSImportGUIPane::BrowseToDir(const std::string& sPath)
{
	msPathNameCur = sPath;
	mpImport_File_Browser->SetPath(msPathNameCur);

	if (mpPop) {
		mpPane->RemoveControl(dynamic_cast<ge::IControl*>(mpPop));
		// (mo) We need to put this in a list and destroy it later
//		mpPop->Destroy();
	}
	Prepare_Popups();
	// Type
	mpPop = CreatePop(giCtrl_Browser_Popup, IDB_Button_Popup,  mpFile_Browser_Popup, ge::SPos(28, 13), mpPane);
} // BrowseToDir


void CKSImportGUIPane::ConnectControls()
{

} // ConnectControls


void CKSImportGUIPane::OnTimer(tint32 iTimerID)
{
	if (iTimerID == giTimerID_ImportAudioPreview) {
		FillChainPart();
	}
} // OnTimer


void CKSImportGUIPane::SendMsgFromTop(SMsg* pMsg)
{
} // SendMsgFromTop


void CKSImportGUIPane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	switch(pControl->GetID()) {
		case giCtrl_Import:{
			if (mpImport_Files->GetAddedFiles() == 0) {
				mpGUI->GetWindow()->ShowMessageBox("You must add some files\nbefore clicking import", "Sorry");
			}
			else {
				mpImport_Files->ImportFiles();
				GetPlugIn()->SetGlobalParm(giParamID_Show_Import_Window, 0, giSectionGUI);
				PreviewStop();
			}
			break;
		}

		case giCtrl_Cancel_Import:{
			mpImport_Files->ClearFiles();
			GetPlugIn()->SetGlobalParm(giParamID_Show_Import_Window, 0, giSectionGUI);
			PreviewStop();
			break;
		}

		case giCtrl_Browser_Popup:
		{
			if (iValueNew != mpFile_Browser_Popup.iItemCount - 1) {
				PreviewStop();

				std::string s(":");
				tint32 iIndex;
				for (iIndex = 1; iIndex <= iValueNew; iIndex++) {
					s += std::string((const char*)(mpFile_Browser_Popup.pItems[iIndex].pszText));
					s += ":";
				}

				BrowseToDir(s);
			}
		}
		break;

		case giCtrl_Play_Import:
			if (iValueNew == 1) {
				if (!PreviewStart()) {
					GetControl(giCtrl_Play_Import)->SetValue(0);
				}
			}
			else {
				PreviewStop();
			}
			break;

		case giCtrl_Stop_Import:
			PreviewStop();
			break;

		case giCtrl_Add_Import:
			{
				std::string sPathName = mpImport_File_Browser->GetSelectedFile();
				AddFile(sPathName);
			}
			break;

		case giCtrl_Remove_Import:
			mpImport_Files->RemoveFile();
			break;
	}

	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
} // EventValueChange


tbool CKSImportGUIPane::AddFile(std::string sPathName)
{
	if (sPathName.size() == 0)
		return false;

	return mpImport_Files->AddFile(sPathName);
} // AddFile


void CKSImportGUIPane::UpdateGUIData(tint32 iID, tint32 iValue)
{

} // UpdateGUIData


void CKSImportGUIPane::Prepare_Popups()
{
	std::string s(msPathNameCur);

	s = s.substr(1);

	std::string psStrings[64];
	psStrings[0] = "My Computer";
	tint32 iStrings = 1;

	while (s.find_first_of(':') != std::string::npos) {
		tint32 iPos = s.find_first_of(':');

		psStrings[iStrings] = s.substr(0, iPos);
		s = s.substr(iPos + 1);
		iStrings++;
	}

	mpFile_Browser_Popup.iItemCount = iStrings;
	tint32 iString;
	for (iString = 0; iString < iStrings; iString++) {
		mpFile_Browser_Popup.pItems[iString] = ge::IPopupMenu::SMenuItem(psStrings[iString].c_str(), iString, NULL);
	}
} // Prepare_Popups


ge::IDropDownListBox* CKSImportGUIPane::CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos, ge::IPane* pPane)
{
	// Resources
	tint32 piIDs[ge::IPopupMenu::BitmapCount];
	piIDs[ge::IPopupMenu::BitmapFolderArrow]	= iBitmapID;
	piIDs[ge::IPopupMenu::BitmapTop]			= IDB_PopupBorder_Big;
	piIDs[ge::IPopupMenu::BitmapCenter]			= IDB_Popup_Boddy_Big;
	piIDs[ge::IPopupMenu::BitmapCenterInv]		= IDB_PopupBoddyInverted_Big;
	piIDs[ge::IPopupMenu::BitmapBottom]			= IDB_PopupBorder_Big;
	ge::IDropDownListBox* pListBox = CreateDropDownListBox( iControlID, 
															piIDs, 
															Generic128, 
															IDF_Fuxley_712, 
															iBitmapID, 
															List, Pos, 
															ge::SPos(-20,-2),
															ge::SSize(160,12));
//	pListBox->DontSetText();

	pListBox->SetValue(mpFile_Browser_Popup.iItemCount - 1);

	return pListBox;
} // CreatePop


void CKSImportGUIPane::Create_File_Browser_ScrollBar()
{
	// Position and Stick To Podition
	ge::SPos Pos(207 -giScrollBarSize, 38+18);
	// Scrollbar Backdrop Top
	ge::IBitmap* pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Top, ge::SPos(193,52));
	// Scrollbar Backdrop Center
	pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Vertical_Center, ge::SPos(193,61));
	pBmp->SetSize(ge::SSize(giScrollBarSize, 254  ));
	// Scrollbar Backdrop Bottom
	pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Bottom, ge::SPos(193,250+65));

	// Scroll pos
	ge::SScrollPos ScrollPos;
	ge::SSize size(200-giScrollBarSize, 298);
	ScrollPos.AreaSize = ge::SSize(200-giScrollBarSize, 600);
	ScrollPos.VisibleRect = ge::SRect(ge::SPos(0, 0), size);
	// ScrollBar resources
	tint32 piIDs[ge::IScrollBar::BitmapCount];
	piIDs[ge::IScrollBar::BitmapLeftTop]			= IDB_Button_Scroll_Up;
	piIDs[ge::IScrollBar::BitmapRightDown]			= IDB_Button_Scroll_Down;
	piIDs[ge::IScrollBar::BitmapLeftTopHandle]		= IDB_Scroll_Bar_Handle_Vertical_Top;
	piIDs[ge::IScrollBar::BitmapRightDownHandle]	= IDB_Scroll_Bar_Handle_Vertical_Bottom;
	piIDs[ge::IScrollBar::BitmapCenterHandle]		= IDB_Scroll_Bar_Handle_Vertical_Center;

	//-----
	// Create scrollbar
	mpScrollBarFileBrowser = ge::IScrollBar::Create();
	mpScrollBarFileBrowser->Init(giChC_Track_Player_ScrollBar_Vertical,
		ge::IScrollBar::TypeVertical,
		GetWindow(),
		GetResMan(),
		piIDs,
		mpImport_File_Browser->GetScrollPane(),
		ScrollPos);
	//-----
	// Add ScrollBar
	//-----
	Pos.iX =	193;
	Pos.iY =	39;
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpScrollBarFileBrowser),Pos);
	
	mpImport_File_Browser->GetScrollPane()->SetScrollPos(ScrollPos, false);

} // Create_File_Browser_ScrollBar


void CKSImportGUIPane::Create_File_ScrollBar()
{

	tint32 iX	=	208;
	// Position and Stick To Podition
	ge::SPos Pos( iX + 207 -giScrollBarSize, 38+18);
	// Scrollbar Backdrop Top
	ge::IBitmap* pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Top, ge::SPos(iX + 193,52));
	// Scrollbar Backdrop Center
	pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Vertical_Center, ge::SPos(iX + 193,61));
	pBmp->SetSize(ge::SSize(giScrollBarSize, 254  ));
	// Scrollbar Backdrop Bottom
	pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Scroll_Bar_Bottom, ge::SPos(iX + 193,250+65));

	// Scroll pos
	ge::SScrollPos ScrollPos;
	ge::SSize size(200-giScrollBarSize, 298);
	ScrollPos.AreaSize = ge::SSize(200-giScrollBarSize, 600);
	ScrollPos.VisibleRect = ge::SRect(ge::SPos(0, 0), size);
	// ScrollBar resources
	tint32 piIDs[ge::IScrollBar::BitmapCount];
	piIDs[ge::IScrollBar::BitmapLeftTop]			= IDB_Button_Scroll_Up;
	piIDs[ge::IScrollBar::BitmapRightDown]			= IDB_Button_Scroll_Down;
	piIDs[ge::IScrollBar::BitmapLeftTopHandle]		= IDB_Scroll_Bar_Handle_Vertical_Top;
	piIDs[ge::IScrollBar::BitmapRightDownHandle]	= IDB_Scroll_Bar_Handle_Vertical_Bottom;
	piIDs[ge::IScrollBar::BitmapCenterHandle]		= IDB_Scroll_Bar_Handle_Vertical_Center;

	//-----
	// Create scrollbar
	mpScrollBarFiles = ge::IScrollBar::Create();
	mpScrollBarFiles->Init(giChC_Track_Player_ScrollBar_Vertical,
		ge::IScrollBar::TypeVertical,
		GetWindow(),
		GetResMan(),
		piIDs,
		mpImport_Files->GetScrollPane(),
		ScrollPos);
	//-----
	// Add ScrollBar
	//-----
	Pos.iX =	iX + 193;
	Pos.iY =	39;
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpScrollBarFiles),Pos);
	
	mpImport_Files->GetScrollPane()->SetScrollPos(ScrollPos, false);
} // Create_File_ScrollBar


void CKSImportGUIPane::SetInfo(ac::EQuality eQuality, tint32 iSampleRate, tint32 iChannels)
{
	tchar psz[32];

	/*
	if (iBitDepth == -1) {
		dynamic_cast<ge::IText*>(GetControl(giControlBitDepth))->SetText("n/a");
	}
	else {
		sprintf(psz, "%d", iBitDepth);
		dynamic_cast<ge::IText*>(GetControl(giControlBitDepth))->SetText(psz);
	}
	*/
	dynamic_cast<ge::IText*>(GetControl(giControlBitDepth))->SetText(ac::IEncoder::GetQualityName(eQuality));

	if (iSampleRate == -1) {
		dynamic_cast<ge::IText*>(GetControl(giControlSampleRate))->SetText("Error");
	}
	else {
		sprintf(psz, "%d", iSampleRate);
		dynamic_cast<ge::IText*>(GetControl(giControlSampleRate))->SetText(psz);
	}

	if (iChannels < 1) {
		dynamic_cast<ge::IText*>(GetControl(giControlChannels))->SetText("Error");
	}
	else {
		sprintf(psz, "%d", iChannels);
		dynamic_cast<ge::IText*>(GetControl(giControlChannels))->SetText(psz);
	}
} // SetInfo


void CKSImportGUIPane::SetType(tint32 iType)
{
	switch(iType) {
		case 0:
			dynamic_cast<ge::IText*>(GetControl(giControlType))->SetText("RIFF Wave");
			break;

		case 1:
			dynamic_cast<ge::IText*>(GetControl(giControlType))->SetText("Ogg Vorbis");
			break;

		case 2:
			dynamic_cast<ge::IText*>(GetControl(giControlType))->SetText("Mp3");
			break;
	}
} // SetType


tbool CKSImportGUIPane::PreviewStart()
{
	CAutoLock Lock(&mMutex);
	tbool bSuccess = false;

	// Not here!
	//CAutoDelete<ge::IWaitCursor> pWait(ge::IWaitCursor::Create());

	mbPreviewing = false;
	if (mpfPreview_Input) {
		mpfPreview_Input->Destroy();
		mpfPreview_Input = NULL;
	}

	std::string sPathName = mpImport_File_Browser->GetSelectedFile();
	if (sPathName.size() == 0) {
		PreviewStop_NoLock();
		return false;
	}

	mpfPreview_Input = IFile::Create();
	if (mpfPreview_Input->Open(sPathName.c_str(), IFile::FileRead)) {
		mpdecPreview = ac::IDecoder::Create(mpfPreview_Input);
		if (mpdecPreview) {
			CAutoDelete<ge::IWaitCursor> pWait(ge::IWaitCursor::Create());
			
			// Test file - if this fails we can't play it
			if (!mpdecPreview->TestFile(mpfPreview_Input)) {
				// Testing file failed - do nothing
				//bError = true;
			}
			else {
				tbool bStereo = (mpdecPreview->miLastInputChannels > 1);
				tbool bError = false;
				for (tint32 iChainIx = 0; iChainIx < kiNbOfBuffsInChain; iChainIx++) {
					mapfPreview_OutputL[iChainIx] = IFileMemory::Create();
					if (mapfPreview_OutputL[iChainIx] == NULL)
						bError = true;
					else
						mapfPreview_OutputL[iChainIx]->Open(IFile::FileCreate);

					if (!bStereo) {
						mapfPreview_OutputR[iChainIx] = NULL;
					}
					else {
						mapfPreview_OutputR[iChainIx] = IFileMemory::Create();
						if (mapfPreview_OutputR[iChainIx] == NULL)
							bError = true;
						else
							mapfPreview_OutputR[iChainIx]->Open(IFile::FileCreate);
					}
				}
				if (!bError) {
					if (mpdecPreview->Init(
						dynamic_cast<IFile*>(mapfPreview_OutputL[0]),
						false, 24, 1,
						dynamic_cast<IFile*>(mapfPreview_OutputR[0])))
					{
						miPreview_FileSampleIx = 0;
						miPreview_BuffSampleIx = 0;
						miChainHead = miChainTail = (kiNbOfBuffsInChain - 1);
						mfPreviewLastSampleL = mfPreviewLastSampleR = 0.0f;
						mbPreviewing = true;
						FillChainPart();
						bSuccess = true;
						if (GetControl(giCtrl_Play_Import)->GetValue() == 0) {
							GetControl(giCtrl_Play_Import)->SetValue(1);
						}
					}
				}
			}
		}

		if (!bSuccess) {
			// Get error message
			tchar pszErr[1024];
			mpdecPreview->GetErrMsg(pszErr, 1024);
			// Abort playback
			PreviewStop_NoLock();
			// Display error
			mpGUI->GetWindow()->ShowMessageBox(pszErr, "Preview failed");
		}
	}

	if (bSuccess) {
		if (mpTimer == NULL) {
			mpTimer = ITimer::Create();
			mpTimer->Init(giTimerID_ImportAudioPreview, this, 5);
		}
	}
	return bSuccess;
} // PreviewStart


void CKSImportGUIPane::PreviewStop()
{
	CAutoLock Lock(&mMutex);

	if (mbPreviewing) {
		PreviewStop_NoLock();
	}
} // PreviewStop


void CKSImportGUIPane::PreviewStop_NoLock()
{
	// Stop timer first - this will ensure that we don't crash when destroying the other objects
	if (mpTimer) {
		mpTimer->Destroy();
		mpTimer = NULL;
	}

	if (mpdecPreview) {
		mpdecPreview->Destroy();
		mpdecPreview = NULL;
	}

	if (mpfPreview_Input) {
		mpfPreview_Input->Destroy();
		mpfPreview_Input = NULL;
	}
	for (tint32 iChainIx = 0; iChainIx < kiNbOfBuffsInChain; iChainIx++) {
		if (mapfPreview_OutputL[iChainIx]) {
			mapfPreview_OutputL[iChainIx]->Destroy();
			mapfPreview_OutputL[iChainIx] = NULL;
		}
		if (mapfPreview_OutputR[iChainIx]) {
			mapfPreview_OutputR[iChainIx]->Destroy();
			mapfPreview_OutputR[iChainIx] = NULL;
		}
	}

	mbPreviewing = false;

	if (GetControl(giCtrl_Play_Import)->GetValue() != 0) {
		GetControl(giCtrl_Play_Import)->SetValue(0);
	}
} // PreviewStop_NoLock


#ifdef WIN32
#pragma pack(push, 1)
#endif	// WIN32
struct SSample24
{
	unsigned char a;
	unsigned char b;
	char c;
} PACKED;
#ifdef WIN32
#pragma pack(pop)
#endif	// WIN32

void CKSImportGUIPane::FillChainPart()
{
	if ((!mbPreviewing) || (mpfPreview_Input == NULL))
		return;

	tint32 iChainIx = GetFillingChainIx();
	if (iChainIx < 0) {
		// All are full - that's good!
		return;
	}

	// Get lock for decoding
	miInFillChain_Level++;
	if (miInFillChain_Level != 1) {
		// We've caught up with our-selves - exit before we crash
		miInFillChain_Level--;
		return;
	}

	tbool bStereo = (mapfPreview_OutputR[iChainIx] != NULL);

	// Go to file position 0
	mapfPreview_OutputL[iChainIx]->Seek(0);
	if (bStereo) {
		mapfPreview_OutputR[iChainIx]->Seek(0);
	}
	mpdecPreview->HardcoreSwapOutputFiles(mapfPreview_OutputL[iChainIx], mapfPreview_OutputR[iChainIx]);

	tint32 iSamplesLoaded = 0;
	if (!mpdecPreview->Process(mpfPreview_Input, miPreview_FileSampleIx, giPreviewBuffSmall, &iSamplesLoaded)) {
		// Error in decoding
		// Get error description
		tchar pszErr[1024];
		mpdecPreview->GetErrMsg(pszErr, 1024);
		// Abort playback
		PreviewStop_NoLock();
		// Display error
#ifdef _WIN32
		// Only for Windows - on OS X this would hang (don't know why)
		mpGUI->GetWindow()->ShowMessageBox(pszErr, "Playback failed");
#endif // _WIN32
	}
	else {
		miPreview_FileSampleIx += iSamplesLoaded;

		tint32 iMissingSamples = giPreviewBuffSmall - iSamplesLoaded;
		if (iMissingSamples > 0) {
			// This is last portion

			// Pad with zeroes after actual contents
			tint32 iMissingBytes = iMissingSamples * 3;
			tchar* pcZeroes = new tchar[iMissingBytes];
			memset(pcZeroes, '\0', iMissingBytes);
			mapfPreview_OutputL[iChainIx]->Write(pcZeroes, iMissingBytes);
			if (bStereo) {
				mapfPreview_OutputR[iChainIx]->Write(pcZeroes, iMissingBytes);
			}

			// Flag End-Of-File so we don't try to read more
			mpfPreview_Input->Destroy();
			mpfPreview_Input = NULL;
		}

		// Flag buffer as non-empty
		miChainHead = iChainIx;
	}

	// Release lock on decoding
	miInFillChain_Level--;
} // FillChainPart


void CKSImportGUIPane::Accumulate(tfloat32** ppfBuffer, tint32 iSamples)
{
	CAutoLock Lock(&mMutex);

	static tint32 iNothingBufferedCount = 0;
	if (mbPreviewing == false) {
		iNothingBufferedCount = 0;
		return;
	}
	if (mapfPreview_OutputL[0] == NULL) {
		// That's an error - huh?
		PreviewStop_NoLock();
		return;
	}

	tint32 iChainIx = GetPlayingChainIx();
	if (iChainIx < 0) {
		// Nothing buffered
		// Avoid click
		tfloat32* pfLeft = ppfBuffer[0];
		tfloat32* pfRight = ppfBuffer[1];
		for (tint32 iSample = 0; iSample < iSamples; iSample++) {
			// After last portion - must not click
			mfPreviewLastSampleL *= 0.95f;
			mfPreviewLastSampleR *= 0.95f;
			pfLeft[iSample] = mfPreviewLastSampleL;
			pfRight[iSample] = mfPreviewLastSampleR;
		}
		// .. and exit
		if ((mpfPreview_Input == NULL) && (++iNothingBufferedCount == 100)) {
			PreviewStop_NoLock();
		}
		return;
	}
	iNothingBufferedCount = 0;

	IFileMemory* pfileL = mapfPreview_OutputL[iChainIx];
	IFileMemory* pfileR = mapfPreview_OutputR[iChainIx];

	tbool bReleaseChainBuff = false;
	tbool bStereo = (pfileR != NULL);
	tint32 iBytes = (tint32)pfileL->GetMemorySize();
	tint32 iBuffByteIx = miPreview_BuffSampleIx * 3;
	tint32 iBytesUnused = iBytes - iBuffByteIx;
	tint32 iSamplesUnused = iBytesUnused / 3;
	tint32 iSamplesFromCurrBuffer = iSamples;
	if (iSamplesUnused <= iSamples) {
		// Can only take what's there
		iSamplesFromCurrBuffer = iSamplesUnused;
		// Now the buffer is empty
		bReleaseChainBuff = true;
	}

	tfloat32* pfL = new tfloat32[iSamples];
	tfloat32* pfR = new tfloat32[iSamples];
	tchar* pcL = (tchar*)pfL;
	tchar* pcR = (tchar*)pfR;

	// Use up current buffer first
	if (iSamplesFromCurrBuffer > 0) {
		tint32 iBytesFromCurrBuffer = iSamplesFromCurrBuffer * 3;

		tint32 iOutputBytes = 0;
		tint32 iBuffByteIx = miPreview_BuffSampleIx * 3;
		if (bStereo) {
			tchar* pcInL = ((tchar*)pfileL->GetMemoryPtr()) + iBuffByteIx;
			tchar* pcInR = ((tchar*)pfileR->GetMemoryPtr()) + iBuffByteIx;
			st::Xlat::DualPCM24Mono_DualAF32Mono(
				pcInL, pcInR, iBytesFromCurrBuffer, &iOutputBytes,
				&pcL, NULL, &pcR, NULL);
			pcL += iOutputBytes;
			pcR += iOutputBytes;
		}
		else {
			tchar* pcIn = ((tchar*)pfileL->GetMemoryPtr()) + iBuffByteIx;
			st::Xlat::PCM24Mono_DualAF32Mono(
				pcIn, iBytesFromCurrBuffer, &iOutputBytes,
				&pcL, NULL, &pcR, NULL);
			pcL += iOutputBytes;
			pcR += iOutputBytes;
		}
		miPreview_BuffSampleIx += iSamplesFromCurrBuffer;

		tfloat32* pfLeft = ppfBuffer[0];
		tfloat32* pfRight = ppfBuffer[1];
		tint32 iSample;
		for (iSample = 0; iSample < iSamplesFromCurrBuffer; iSample++) {
			pfLeft[iSample] += pfL[iSample];
			pfRight[iSample] += pfR[iSample];
		}
		mfPreviewLastSampleL = pfL[iSample - 1];
		mfPreviewLastSampleR = pfR[iSample - 1];
		for ( ; iSample < iSamples; iSample++) {
			// This was a partial last portion - must not click
			mfPreviewLastSampleL *= 0.99f;
			mfPreviewLastSampleR *= 0.99f;
			pfLeft[iSample] = mfPreviewLastSampleL;
			pfRight[iSample] = mfPreviewLastSampleR;
		}
	}

	delete[] pfL;
	delete[] pfR;

	if (bReleaseChainBuff) {
		// Advance chain tail one index
		if (++iChainIx >= kiNbOfBuffsInChain)
			iChainIx = 0;
		miChainTail = iChainIx;

		// Start at beginning of that buffer
		miPreview_BuffSampleIx = 0;
	}
} // Accumulate





