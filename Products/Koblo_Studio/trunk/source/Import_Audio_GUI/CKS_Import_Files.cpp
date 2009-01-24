#include "KSOS.h"

CKS_Import_Files::CKS_Import_Files(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

	mpImportGUI = dynamic_cast<CKSImportGUIPane*>(pPaneParent);
}

CKS_Import_Files::~CKS_Import_Files()
{
}

void CKS_Import_Files::SetInfo()
{

}

void CKS_Import_Files::Init()
{
	// Lasse, size may be wrong! fix later
	ge::SSize SizeThis(200 - giScrollBarSize, 600);
	mpScrollPane = ge::IScrollPane::Create();
	mpScrollPane->Init(ge::IControl::giNoID , giTimerID_TrackFiles_ScrollPane);
	mpPane = dynamic_cast<ge::IPane*>(mpScrollPane);
	mpPane->SetSize(SizeThis);
	mpPane->GetSize(SizeThis);
	

	mpPane->SetBackgroundColour(ge::SRGB(239,239,239));
	
	mpListBox = CreateListBox(giCtrl_File_List2,
		ge::SPos(4, 0), ge::SSize(SizeThis.iCX-4, SizeThis.iCY),
		CreateFont(Generic128, 
		IDB_Font_Minix_Tight, 
		ge::SRGB(10, 10, 10)));
}

void CKS_Import_Files::ConnectControls()
{
}

void CKS_Import_Files::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
}

void CKS_Import_Files::EventGeneric(ge::IControl* pControl, void* pEventData)
{
}


void CKS_Import_Files::HandleMsg(SMsg* pMsg)
{

}

tbool CKS_Import_Files::AddFile(const std::string& sPathName)
{
	tint32 iPos = sPathName.rfind(':');
	std::string sName = sPathName.substr(iPos + 1);

	CKSFile_Item Info;
	Info.Path(sPathName);
	Info.Name(sName);

	// Find name only of file
	std::list<CKSFile_Item>::iterator it = mFile_Items.begin();
	std::string sNameNoExt = sName;
	tint32 iDotIx = sNameNoExt.find_last_of('.');
	if (iDotIx >= 0) {
		sNameNoExt.erase(iDotIx);
		if (sNameNoExt.length() == 0) {
			// Can't add the empty string
			return false;
		}
	}

	// We can't have several files with the same name - so run thru list first
	for ( ;it != mFile_Items.end(); it++) {
		
		CKSFile_Item& rInfo = *it;
		std::string sNameNoExt_AlreadyAdded = rInfo.Name();
		tint32 iDotIx = sNameNoExt_AlreadyAdded.find_last_of('.');
		if (iDotIx >= 0) {
			sNameNoExt_AlreadyAdded.erase(iDotIx);
		}

		if (stricmp(sNameNoExt.c_str(), sNameNoExt_AlreadyAdded.c_str()) == 0) {
			// Same name - that won't do
			return false;
		}
	}

	mFile_Items.push_back(Info);

	UpdateListBox();
	return true;
}

void CKS_Import_Files::RemoveFile()
{
	tint32 iIndex = mpListBox->GetValue();
	if (iIndex == -1) {
		return;
	}

	std::list<CKSFile_Item>::iterator it = mFile_Items.begin();
	while (iIndex) {
		iIndex--;
		it++;
	}
	mFile_Items.erase(it);

	UpdateListBox();
}

// old version
void CKS_Import_Files::ImportFiles()
{
	/*
	 if (gpApplication->GetProjDir().length() == 0) {
	 gpApplication->ShowMessageBox("You must create or load a project before importing audio", "Sorry");
	 return;
	 }
	 */
	std::list<CKSFile_Item>::iterator it = mFile_Items.begin();
	for (; it != mFile_Items.end(); it++) {
		CKSFile_Item Info = *it;
		gpApplication->QueueAudioFileImport(Info.Path().c_str(), false);
	}
	ClearFiles();
}


// new version
void CKS_Import_Files::Import_Audio_Files()
{
	
	std::list<CKSFile_Item>::iterator it = mFile_Items.begin();
	for (; it != mFile_Items.end(); it++) {
		
		CKSFile_Item Audio_File = *it;
		
		Import_Audio_File(Audio_File.Path().c_str(), false);
	}
	ClearFiles();
}

tbool CKS_Import_Files::Import_Audio_File(const tchar* pszPathName, tbool bAlwaysKeepStereo, tint32 iTrackID , tint64 iTrackPos)
{
	if (gpApplication->IsPlayingOrRecording()) {
		gpApplication->PlaybackStop();
	}
	
	CImportAudioTask* pImportAudioTask = new CImportAudioTask();
	
	tbool bDoesWaveAlreadyExist = false;
	tbool bForceOriginalIsLossy = false;
	
	CImportAudioTask::EStereoBehavior eBehave = (bAlwaysKeepStereo) ? CImportAudioTask::geStereoDoKeep : CImportAudioTask::geStereoDoAsk;
	
	tbool bSuccess = pImportAudioTask->Init( pszPathName, bDoesWaveAlreadyExist, eBehave, bForceOriginalIsLossy);
	
	
	if (iTrackID >= 0) {
		pImportAudioTask->Init_InsertAsRegionAfterImport(iTrackID, iTrackPos);
	}
	
	if (bSuccess) {
		gpApplication->mpProgressTasks->Add(pImportAudioTask);
		gpApplication->Playback_InProgressTask();
		//bSuccess = DoAudioFileImport(pInfo);
	}
	
	if (!bSuccess) {
		gpApplication->Extended_Error(pImportAudioTask->GetError());
		pImportAudioTask->Destroy();
	}
	//delete pTask;
	return bSuccess;
} // QueueAudioFileImport


void CKS_Import_Files::ClearFiles()
{
	mFile_Items.clear();

	UpdateListBox();
}

void CKS_Import_Files::UpdateListBox()
{
	std::string sEnum;

	std::list<CKSFile_Item>::const_iterator it = mFile_Items.begin();
	tbool bFirst = true;
	for (; it != mFile_Items.end(); it++) {
		std::string sName;

		if(bFirst) 
			bFirst = false;
		else 
			sName = "@";
/*
		if (bFirst == false) {
			sName = "@";
		}
		bFirst = false;
*/
		CKSFile_Item Info = *it;

		sName += std::string(Info.Name());

		sEnum += sName;
	}

	mpListBox->SetText(sEnum.c_str(), '@');

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
	mpListBox->SetSize(SizeListBox);
}




