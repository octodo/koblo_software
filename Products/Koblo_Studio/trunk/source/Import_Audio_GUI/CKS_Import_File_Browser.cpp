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

	mpDirectory_Popup = CreateListBox(giCtrl_File_List,
		ge::SPos(4, 0), ge::SSize(SizeThis.iCX-4, SizeThis.iCY),
		CreateFont(Generic128, 
		IDB_Font_Minix_Tight, 
		ge::SRGB(10, 10, 10)));
	mpDirectory_Popup->EnableExtraMouseEvent(ge::LeftButtonDblClk, -2);
}

void CKS_Import_File_Browser::Build_File_List(const std::string& sPath)
{
	CAutoDelete<ge::IWaitCursor> pWaitCursor(ge::IWaitCursor::Create());

	std::string sEnum;
	// clear list of file items
	mFile_Items.clear();

	tbool bWasOldMyComputer = (strcmp(msFolder_Path.c_str(), ":")	== 0);
	tbool bIsNewMyComputer = (strcmp(sPath.c_str(), ":")	== 0);

	msFolder_Path = sPath;

	if (bIsNewMyComputer) {
		// List "My Computer" - a.k.a. disk volumes
		tchar pszEnumVolumes[1024];
		if (IFile::GetDriveNames(pszEnumVolumes, sizeof(pszEnumVolumes), '@', true)) {
			std::list<std::string> listVolumes;
			PszEnumString2List(pszEnumVolumes, '@', listVolumes);
			std::list<std::string>::iterator itVolumes = listVolumes.begin();
			for ( ; itVolumes != listVolumes.end(); itVolumes++) {
				CKSFile_Item File_Item;
				File_Item.Disk_Name(*itVolumes);
				File_Item.Is_A_Dir(true);
				
				mFile_Items.push_back(File_Item);
			}
			sEnum = pszEnumVolumes;
		}
	}
	else if (bWasOldMyComputer) {
#ifdef _WIN32
		// For windows we must use only the drive letter part of the path
		tint32 iPosSpace = msFolder_Path.find_first_of(' ');
		if (iPosSpace > -1) {
			msFolder_Path.erase(iPosSpace);
			msFolder_Path += ":\\";
		}
#endif // _WIN32
	}
	// prepare check if files in folder
	CAutoDelete<IFileSearch> pSearch(IFileSearch::Create());
	std::string sSearchPathName(msFolder_Path);
	std::string sTest(msFolder_Path);
	
	sSearchPathName += "*";
	pSearch->Init2(sSearchPathName.c_str());

	tbool bFirst = true;
	tchar pszName[1024];
	tbool bDir;
	
	// check all files and folders in directory
	while (pSearch->GetNext(pszName, bDir)) {
		
		std::string sName;
		std::string sFolder_Path = msFolder_Path + pszName;
		tbool bFirstOld = bFirst;
		if (bFirst == false) {
			// Put a "@" tag on root 
			sName = "@";
		}
		bFirst = false;


		sName += std::string(pszName);
		
		CKSFile_Item File_Item;
		
		if (bDir) {
			// directory
			sName						+= " (dir)";

			sFolder_Path				+=	":";
			File_Item.Is_A_Dir(true);
			File_Item.Source_Path(sFolder_Path );
			mFile_Items.push_back(File_Item);
			sEnum += sName;
		}
		else {
			// file
			std::string sPathName(msFolder_Path);
			sPathName += std::string(pszName);
			if(gpApplication->Readable_Audio(sPathName) ){
				File_Item.Import(std::string(sPathName)); 
				mFile_Items.push_back(File_Item);
				sEnum += sName;
			}
			else 
				bFirst = bFirstOld;
			 
			
			 
		}
		
	}
	
	Update_Interface(sEnum);
	
} // SetPath

void CKS_Import_File_Browser::Update_Interface(std::string sMenu_Items)
{
	
	mpDirectory_Popup->SetText(sMenu_Items.c_str(), '@');
	
	tint32 iTextHeight = mFile_Items.size() * 14;
	
	ge::SSize SizeScroll(200 - giScrollBarSize, iTextHeight);
	ge::SScrollPos ScrollPos;
	mpScrollPane->GetScrollPos(ScrollPos);
	ScrollPos.AreaSize = SizeScroll;
	if (ScrollPos.AreaSize.iCY < ScrollPos.VisibleRect.iCY) {
		ScrollPos.AreaSize.iCY = ScrollPos.VisibleRect.iCY;
	}
	mpScrollPane->SetScrollPos(ScrollPos);
	
	ge::SSize SizeListBox(SizeScroll - ge::SSize(4, 0));
	mpDirectory_Popup->SetSize(SizeListBox);
	
}

void CKS_Import_File_Browser::ConnectControls()
{
}


void CKS_Import_File_Browser::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);

	switch(pControl->GetID()) {
			
		// Click in filebrowswe
		case giCtrl_File_List:
			{	
				tbool bDoubleClick = (iValueNew == -2);
				
				if (iValueNew >= -1) {
					// selection to be highlighted 
					miLatestGenuineIndex = iValueNew; 
				}
				
				tint32 iIndex = miLatestGenuineIndex; 
				
				std::list<CKSFile_Item>::const_iterator it = mFile_Items.begin();
				
				// move list iterator to selected item 
				while (iIndex > 0) {
					iIndex--;
					it++;
				}
				CKSFile_Item File_Item = *it;
				if (File_Item.Is_A_Dir()) {
					mpImportGUI->PreviewStop();
					miLatestSelectedFile = -1;

					std::string sPath;
					sPath = File_Item.Source_Path();
					sPath += File_Item.Disk_Name();
					
					if (bDoubleClick) {
						mpImportGUI->BrowseToDir(sPath);
					}
				}
				else {
					tbool bSameFile = (miLatestGenuineIndex == miLatestSelectedFile);

					if (bSameFile) {
						// add selected file to the import list
						if (bDoubleClick) {
							mpImportGUI->AddFile(File_Item.Source_Path());
						}
					}
					else {
						// present file info on the gui
						Display_File_Info(File_Item.Disk_Name() + File_Item.Extencion());
					}
				}
			}
			break;
	}
} // EventValueChange


void CKS_Import_File_Browser::Display_File_Info(std::string sName)
{
	// New file - read its info
	// First we need to stop previewing
	mpImportGUI->PreviewStop();
	
	// Get info
	miLatestSelectedFile = miLatestGenuineIndex;
	
	std::string sPathName(msFolder_Path);
	sPathName += sName;
	
	
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



std::string CKS_Import_File_Browser::GetSelectedFile()
{
	tint32 iIndex = miLatestSelectedFile; //mpDirectory_Popup->GetValue();
	if (iIndex < 0) {
		return "";
	}

	std::list<CKSFile_Item>::const_iterator it = mFile_Items.begin();
	while (iIndex) {
		iIndex--;
		it++;
	}
	CKSFile_Item File_Item = *it;
	if (File_Item.Is_A_Dir()) {
		return "";
	}
	else {
		/*
		std::string sPathName(msFolder_Path);
		sPathName += File_Item.Name();
		sPathName += File_Item.Extencion();
		return sPathName;
		 */ 
		return File_Item.Source_Path();
	}
}

void CKS_Import_File_Browser::EventGeneric(ge::IControl* pControl, void* pEventData)
{
}


void CKS_Import_File_Browser::HandleMsg(SMsg* pMsg)
{

}


