#include "KSOS.h"

CKS_Import_File_Browser::CKS_Import_File_Browser(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

	mpImportGUI = dynamic_cast<CKSImportGUIPane*>(pPaneParent);
	miLatestGenuineIndex = -1;
	miLatestSelectedFile = -1;
}

CKS_Import_File_Browser::~CKS_Import_File_Browser()
{
}

void CKS_Import_File_Browser::SetInfo()
{

}

void CKS_Import_File_Browser::Init()
{
	// Lasse, size may be wrong! fix later
	ge::SSize SizeThis(200 - giScrollBarSize, 600);
	mpScrollPane = ge::IScrollPane::Create();
	mpScrollPane->Init(ge::IControl::giNoID , giTimerID_TrackFiles_ScrollPane);
	mpPane = dynamic_cast<ge::IPane*>(mpScrollPane);
	mpPane->SetSize(SizeThis);
	mpPane->GetSize(SizeThis);
	mpPane->SetBackgroundColour(ge::SRGB(239,239,239));

	mpListBox = CreateListBox(giCtrl_File_List,
		ge::SPos(4, 0), ge::SSize(SizeThis.iCX-4, SizeThis.iCY),
		CreateFont(Generic128, 
		IDB_Font_Minix_Tight, 
		ge::SRGB(10, 10, 10)));
	mpListBox->EnableExtraMouseEvent(ge::LeftButtonDblClk, -2);
}

void CKS_Import_File_Browser::SetPath(const std::string& sPath)
{
	CAutoDelete<ge::IWaitCursor> pWaitCursor(ge::IWaitCursor::Create());

	std::string sEnum;
	mItems.clear();

	tbool bWasOldMyComputer = (strcmp(msPath.c_str(), ":") == 0);
	tbool bIsNewMyComputer = (strcmp(sPath.c_str(), ":") == 0);

	msPath = sPath;

	if (bIsNewMyComputer) {
		// List "My Computer" - a.k.a. disk volumes
		tchar pszEnumVolumes[1024];
		if (IFile::GetDriveNames(pszEnumVolumes, sizeof(pszEnumVolumes), '@', true)) {
			std::list<std::string> listVolumes;
			PszEnumString2List(pszEnumVolumes, '@', listVolumes);
			std::list<std::string>::iterator itVolumes = listVolumes.begin();
			for ( ; itVolumes != listVolumes.end(); itVolumes++) {
				SItemInfo Info;
				Info.sName = *itVolumes;
				Info.bDir = true;
				mItems.push_back(Info);
			}
			sEnum = pszEnumVolumes;
		}
	}
	else if (bWasOldMyComputer) {
#ifdef _WIN32
		// For windows we must use only the drive letter part of the path
		tint32 iPosSpace = msPath.find_first_of(' ');
		if (iPosSpace > -1) {
			msPath.erase(iPosSpace);
			msPath += ":\\";
		}
#endif // _WIN32
	}

	CAutoDelete<IFileSearch> pSearch(IFileSearch::Create());
	std::string sSearchPathName(msPath);
	sSearchPathName += "*";
	pSearch->Init2(sSearchPathName.c_str());

	tbool bFirst = true;
	tchar pszName[1024];
	tbool bDir;
	while (pSearch->GetNext(pszName, bDir)) {
		std::string sName;
		tbool bFirstOld = bFirst;
		if (bFirst == false) {
			sName = "@";
		}
		bFirst = false;

		tbool bOK = false;
		sName += std::string(pszName);
		if (bDir) {
			sName += " (dir)";
			bOK = true;
		}
		else {
			std::string sPathName(msPath);
			sPathName += std::string(pszName);
			CAutoDelete<IFile> pFile(IFile::Create());
			if (pFile->Open(sPathName.c_str(), IFile::FileRead)) {
				ac::IDecoder* pDec = ac::IDecoder::Create(pFile);
				if (pDec) {
					bOK = true;
					pDec->Destroy();
				}
			}
		}

		if (bOK) {
			sEnum += sName;

			SItemInfo Info;
			Info.sName = std::string(pszName);
			Info.bDir = bDir;
			mItems.push_back(Info);
		}
		else {
			bFirst = bFirstOld;
		}
	}

	mpListBox->SetText(sEnum.c_str(), '@');

	tint32 iTextHeight = mItems.size() * 14;

	ge::SSize SizeScroll(200 - giScrollBarSize, iTextHeight);
	ge::SScrollPos ScrollPos;
	mpScrollPane->GetScrollPos(ScrollPos);
	ScrollPos.AreaSize = SizeScroll;
	if (ScrollPos.AreaSize.iCY < ScrollPos.VisibleRect.iCY) {
		ScrollPos.AreaSize.iCY = ScrollPos.VisibleRect.iCY;
	}
	mpScrollPane->SetScrollPos(ScrollPos);

	ge::SSize SizeListBox(SizeScroll - ge::SSize(4, 0));
	mpListBox->SetSize(SizeListBox);
} // SetPath


void CKS_Import_File_Browser::ConnectControls()
{
}


void CKS_Import_File_Browser::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);

	switch(pControl->GetID()) {
		case giCtrl_File_List:
			{
				tbool bIsDoubleClick = (iValueNew == -2);
				if (iValueNew >= -1) {
					miLatestGenuineIndex = iValueNew;
				}

				tint32 iIndex = miLatestGenuineIndex; //pControl->GetValue();
				std::list<SItemInfo>::const_iterator it = mItems.begin();
				while (iIndex > 0) {
					iIndex--;
					it++;
				}
				SItemInfo Info = *it;
				if (Info.bDir == true) {
					mpImportGUI->PreviewStop();
					miLatestSelectedFile = -1;

					std::string sPath(msPath);
					sPath += Info.sName;
					sPath += ":";
					if (bIsDoubleClick) {
						mpImportGUI->BrowseToDir(sPath);
					}
				}
				else {
					tbool bSameFile = (miLatestGenuineIndex == miLatestSelectedFile);

					if (bSameFile) {
						// Same file
						if (bIsDoubleClick) {
							// Add file to import list
							std::string sPathName(msPath);
							sPathName += Info.sName;
							mpImportGUI->AddFile(sPathName);
						}
					}
					else {
						// New file - read its info

						// First we need to stop previewing (may crash on read info if we don't)
						mpImportGUI->PreviewStop();

						// Get info
						miLatestSelectedFile = miLatestGenuineIndex;
						std::string sPathName(msPath);
						sPathName += Info.sName;
						CAutoDelete<IFile> pFile(IFile::Create());
						if (pFile->Open(sPathName.c_str(), IFile::FileRead)) {
							ac::IDecoder* pdec = ac::IDecoder::Create(pFile);
							if (pdec) {
								// Attempt to test file - if it fails ignore it
								pdec->TestFile(pFile);
								// Now we have tried to decipher file - we pretend it succeeded
								CAutoDelete<ac::IDecoder> pDecoder(pdec);
								ac::EAudioCodec Codec = pDecoder->GetAudioCodec();
								if (Codec == ac::geAudioCodecWave) {
									mpImportGUI->SetType(0);
									ac::EQuality eQ = ac::geQualityUnknown;
									tint32 iBitDepth = pdec->miLastInputBitWidth;
									if (iBitDepth == 16)
										eQ = ac::geQualityLossless16;
									else if (iBitDepth == 24)
										eQ = ac::geQualityLossless24;
									tint32 iSampleRate = pdec->miOutputSampleFreq;
									tint32 iChannels = pdec->miOutputChannels;
									mpImportGUI->SetInfo(eQ, iSampleRate, iChannels);
								}
								else if (Codec == geAudioCodecVorbis) {
									mpImportGUI->SetType(1);
									tint32 iSampleRate = pdec->miOutputSampleFreq;
									tint32 iChannels = pdec->miOutputChannels;
									mpImportGUI->SetInfo(pdec->meLowestInputQuality, iSampleRate, iChannels);
								}
								else {
									mpImportGUI->SetType(2);
									tint32 iSampleRate = pdec->miOutputSampleFreq;
									tint32 iChannels = pdec->miOutputChannels;
									mpImportGUI->SetInfo(pdec->meLowestInputQuality, iSampleRate, iChannels);
								}
							}
						}
					}
				}
			}
			break;
	}
} // EventValueChange


std::string CKS_Import_File_Browser::GetSelectedFile()
{
	tint32 iIndex = miLatestSelectedFile; //mpListBox->GetValue();
	if (iIndex < 0) {
		return "";
	}

	std::list<SItemInfo>::const_iterator it = mItems.begin();
	while (iIndex) {
		iIndex--;
		it++;
	}
	SItemInfo Info = *it;
	if (Info.bDir == true) {
		return "";
	}
	else {
		std::string sPathName(msPath);
		sPathName += Info.sName;
		return sPathName;
	}
}

void CKS_Import_File_Browser::EventGeneric(ge::IControl* pControl, void* pEventData)
{
}


void CKS_Import_File_Browser::HandleMsg(SMsg* pMsg)
{

}


