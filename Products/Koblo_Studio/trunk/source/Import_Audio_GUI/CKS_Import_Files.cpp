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

tbool CKS_Import_Files::Add_File( std::string sPathName)
{

	CKSFile_Item File_Item;
	
	if(!File_Item.Import(sPathName) ){
		// wrong name or type
		return false;
	}

	std::list<CKSFile_Item>::iterator it = mFile_Items.begin();
	// We can't have several files with the same name - so run thru list first
	for ( ;it != mFile_Items.end(); it++) {
		
		CKSFile_Item& rInfo = *it;
		
		if (stricmp(File_Item.Disk_Name().c_str(), rInfo.Disk_Name().c_str()) == 0)
			// Same name - that won't do
			return false;
		
	}

	mFile_Items.push_back(File_Item);

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

}


// new version
void CKS_Import_Files::Import_Audio_Files()
{
	
	std::list<CKSFile_Item>::iterator it = mFile_Items.begin();
	for (; it != mFile_Items.end(); it++) {
		
		Import_Audio_File((*it));
	}
	ClearFiles();
}

tbool CKS_Import_Files::Import_Audio_File(CKSFile_Item File_Item  )
{
	if (gpApplication->IsPlayingOrRecording())  gpApplication->PlaybackStop();
	
	
	CImportAudioTask* pImportAudioTask = new CImportAudioTask();
	
	CImportAudioTask::EStereoBehavior eBehave = CImportAudioTask::geStereoDoAsk;
	
	tbool bSuccess = pImportAudioTask->Init( File_Item.Source_Path(), false, eBehave, false);
		
	if (bSuccess) {
		gpApplication->mpProgressTasks->Add(pImportAudioTask);
		gpApplication->Playback_InProgressTask();
	}
	else {
		gpApplication->Extended_Error(pImportAudioTask->GetError());
		pImportAudioTask->Destroy();
	}

	return bSuccess;
} 


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

		sName += std::string(Info.Disk_Name());

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




