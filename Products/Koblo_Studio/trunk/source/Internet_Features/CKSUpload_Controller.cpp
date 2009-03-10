


#include "KSOS.h"

CKSUpload_Controller::CKSUpload_Controller()
{

}

CKSUpload_Controller::~CKSUpload_Controller()
{
	
}

void CKSUpload_Controller::Upload_Project()
{
	if(gpApplication->No_Project_Is_Loaded()) {
		return;
	}
	
	// stop playback
	gpApplication->PlaybackStop();
	
	// clear que's
	mOGG_Compress_Que.clear();
	mMp3_Compress_Que.clear();
	mUpload_Que.clear();
	
	gpApplication->Set_Commit_UUID();
	
	// give preset file new UUID
	gpApplication->Get_New_Insert_File_UUID();
	
	// save project to secure resent edits is stored
	gpApplication->Save_Project_As_XML_File_To_Disk();
	

	std::list<CSample_Data>::iterator  itSample_Data = gpApplication->Get_Sample_Data_List_Begin();
	
	for (; itSample_Data != gpApplication->Get_Sample_Data_List_End(); itSample_Data++) {
		Prepare_Sampel_For_Upload( &(*itSample_Data) );
	}
	
	// propare ogg and mp3 files
	gpApplication->Compress_OGG_File();
	gpApplication->Compress_MP3_File();
	

	
	// if the project has a UUID it has been uploaded to koblo.com
	gpApplication->Project_Is_Uploaded() ? Write_Branch() : Upload_New_Project();

	// sample and take url's has been passed back form koblo now save them
	gpApplication->Save_Project_As_XML_File_To_Disk();
	

}

void CKSUpload_Controller::Upload_New_Project()
{
	
	// get path to project file
	std::string sProjectXmlFile		=	gpApplication->Project_Folder();
	sProjectXmlFile					+=	gpApplication->Project_Name(); 
	sProjectXmlFile					+=  ".xml";

	gpApplication->Branch_Revision(1);
	
	
	
	// Make task for first-time upload
	CUploadTask* pUploadTask = new CUploadTask();
	pUploadTask->Init_NewProject(
								 gpApplication->Get_User_Name().c_str(),
								 gpApplication->Get_Password().c_str(),
								 gpApplication->Project_UUID().c_str(),
								 (gpApplication->Project_Name() ).c_str(),
								 "no description", // project description
								 "by", // license
								 gpApplication->Get_Branch_UUID().c_str(),
								 gpApplication->Get_Commit_UUID().c_str(), 
								 sProjectXmlFile.c_str(),
								 &mUpload_Que);
	
	//make full path to preset file
	std::string sFile_And_Path	=	gpApplication->Plugin_Settings_Folder() + gpApplication->Get_Insert_File_UUID() + ".plugindata";
	
	printf("\n");
	printf(sFile_And_Path.c_str());
	
//	pUploadTask->Add_PresetData( (gpApplication->Get_Insert_File_UUID()).c_str(), sFile_And_Path.c_str() );
	
	
	// Add task to task list
	CAutoLock Lock(gpApplication->mMutex_Progress);
	gpApplication->mpProgressTasks->Add(pUploadTask);
	gpApplication->Playback_InProgressTask();
}


void CKSUpload_Controller::Write_Branch()
{
	/*
	 
	 //!!! Crashes
	
	std::string sURI = std::string("/branches/") + gpApplication->Get_Branch_UUID() + "/rights.xml";
	sURI +=  " -u";// + " \"" ;//+ gpApplication->Get_User_Name() + ":" + gpApplication->Get_Password() + "\"";
	sURI +=  " \"";
	sURI +=  gpApplication->Get_User_Name();
	sURI +=  ":";
	sURI += gpApplication->Get_Password();
	sURI +=  "\"";
	
	// read xml file
	tchar* pszBuff = NULL;
	tint32 iOutLen = 0;
	ine::IINetUtil::GetWebFile(NULL, "koblo.com", sURI.c_str(), &iOutLen, &pszBuff);
	
	gpApplication->Pass_Branch_Revision(pszBuff);
	 */
	
	
	
	// Make task for upload
	CUploadTask* pUploadTask = new CUploadTask();
	
	pUploadTask->Init_Check_Rights_To_Branch();
	
	// Add task to task list
	CAutoLock Lock(gpApplication->mMutex_Progress);
	gpApplication->mpProgressTasks->Add(pUploadTask);
	gpApplication->Playback_InProgressTask();
	 
}


tbool CKSUpload_Controller::Write_To_My_Branch()
{
	std::string sProjectXmlFile				= gpApplication->Project_Folder();
	std::string sOnline_ProjectXmlFile		= gpApplication->Project_Folder();
	sProjectXmlFile							+= gpApplication->Project_Name() + ".xml";
	sOnline_ProjectXmlFile					+= gpApplication->Online_Project_Name() + ".xml";
	
	
	// Make task for upload
	CUploadTask* pUploadTask = new CUploadTask();
	pUploadTask->Init_Commit(
							 sProjectXmlFile.c_str(),
							 sOnline_ProjectXmlFile.c_str(),
							 "next commit",
							 &mUpload_Que);
	
	
//	pUploadTask->Add_PresetData( (gpApplication->Get_Insert_File_UUID()).c_str(), gpApplication->Plugin_Settings_Folder().c_str() );
	
	// Add task to task list
	CAutoLock Lock(gpApplication->mMutex_Progress);
	gpApplication->mpProgressTasks->Add(pUploadTask);
	gpApplication->Playback_InProgressTask();
	
	return true;
}


tbool CKSUpload_Controller::Write_To_New_Branch()
{
	std::string sProjectXmlFile				= gpApplication->Project_Folder();
	std::string sOnline_ProjectXmlFile		= gpApplication->Project_Folder();
	sProjectXmlFile							+= gpApplication->Project_Name() + ".xml";
	sOnline_ProjectXmlFile					+= gpApplication->Online_Project_Name() + ".xml";
	
	std::string sOriginal_UUID				= gpApplication->Get_Branch_UUID();
	// Set new branch UUID
	gpApplication->Set_Branch_UUID();
	
	
	// Make task for upload
	CUploadTask* pUploadTask = new CUploadTask();
	pUploadTask->Init_NewBranch( sOriginal_UUID.c_str(),
								"Initial commit of new branch",
								"by", // license
								
								sProjectXmlFile.c_str(),
								&mUpload_Que);
	
	
//	pUploadTask->Add_PresetData( (gpApplication->Get_Insert_File_UUID() + ".plugindata").c_str(), gpApplication->Plugin_Settings_Folder().c_str() );
	
	// Add task to task list
	CAutoLock Lock(gpApplication->mMutex_Progress);
	gpApplication->mpProgressTasks->Add(pUploadTask);
	gpApplication->Playback_InProgressTask();

	
	
	return true;
}


void CKSUpload_Controller::Upload_To_My_Branch()
{
	
	std::string sProjectXmlFile				= gpApplication->Project_Folder();
	std::string sOnline_ProjectXmlFile		= gpApplication->Project_Folder();
	sProjectXmlFile							+= gpApplication->Project_Name() + ".xml";
	sOnline_ProjectXmlFile					+= gpApplication->Online_Project_Name() + ".xml";
	
	
	// Make task for upload
	CUploadTask* pUploadTask = new CUploadTask();
	pUploadTask->Init_Commit(
							 sProjectXmlFile.c_str(),
							 sOnline_ProjectXmlFile.c_str(),
							 "next commit",
							 &mUpload_Que);
	
	
//	pUploadTask->Add_PresetData( (gpApplication->Get_Insert_File_UUID() + ".plugindata").c_str(), gpApplication->Plugin_Settings_Folder().c_str() );
	
	// Add task to task list
	CAutoLock Lock(gpApplication->mMutex_Progress);
	gpApplication->mpProgressTasks->Add(pUploadTask);
	gpApplication->Playback_InProgressTask();
	
}



void CKSUpload_Controller::Online_Project_Upload_Failed()
{
	// Take lock (thread-safe)
	CAutoLock Lock(gpApplication->mMutex_ForUpAndDownloadTask_CallBack);

	gpApplication->Clear_Project_UUID();
	gpApplication->Save_Project_As_XML_File_To_Disk();
}


void CKSUpload_Controller::Prepare_Sampel_For_Upload(CSample_Data* pSample_Data)
{
	tuint32 uiTakes =  pSample_Data->Number_Of_Takes();
	
	for(tuint32 i= 0; i< uiTakes; i++ ){
		
		CTake_Data* pTake_Data = pSample_Data->Get_Take_Data(i);
		
		if( !Take_Is_Uploaded(pTake_Data) ){
			
			// prepare ogg and mp3 files
			Prepare_Take_For_Upload( pTake_Data );
			
			// add take to upload que
			mUpload_Que.push_back(pTake_Data);

		}
	
	
	}
}

tbool CKSUpload_Controller::Take_Is_Uploaded(CTake_Data* pTake_Data)
{
	tbool bUploaded = pTake_Data->URL().size() > 0 ? true: false;	
	return bUploaded;
}

void CKSUpload_Controller::Prepare_Take_For_Upload(CTake_Data* pTake_Data)
{
	// check if file is in wav folder
	std::string sFile_Path = gpApplication->Wave_File_Folder() + pTake_Data->Get_UUID();
	
	if( Validate_Take( pTake_Data) )
	{
	
		// check if file alreaddy is in ogg folder
		sFile_Path = gpApplication->OGG_File_Folder() + pTake_Data->Get_UUID();
		if( !gpApplication->Readable_Audio(sFile_Path + ".ogg") )
			mOGG_Compress_Que.push_back(pTake_Data);
		
		// check if file alreaddy is in mp3 folder
		sFile_Path = gpApplication->MP3_File_Folder() + pTake_Data->Get_UUID();
		if( !gpApplication->Readable_Audio(sFile_Path + ".mp3") )
		   mMp3_Compress_Que.push_back(pTake_Data);

	}
}

tbool CKSUpload_Controller::Validate_Take(CTake_Data* pTake_Data)
{

	if( pTake_Data->Channels() == 1 ){
		return gpApplication->Readable_Audio(pTake_Data->Left_Wave_File_Path());
	}
	else{
		tuint uiTest =	gpApplication->Readable_Audio(pTake_Data->Left_Wave_File_Path());
		uiTest		+=	gpApplication->Readable_Audio(pTake_Data->Right_Wave_File_Path());
		return uiTest == 2 ? true: false;
	}
	
	return false;
}


void CKSUpload_Controller::Compress_OGG_File()
{
	std::list<CTake_Data*>::iterator it = mOGG_Compress_Que.begin();
	for ( ; it != mOGG_Compress_Que.end(); it++) {
		CTake_Data* pTake_Data = *it;
		std::string sOgg = gpApplication->OGG_File_Folder() + pTake_Data->Get_UUID() + ".ogg";
		
		CExportClipTask* pExportClipTask = new CExportClipTask(); //!!! when is this task deleted??
		
		if (pExportClipTask->Init(pTake_Data, sOgg.c_str(), ac::geAudioCodecVorbis, ac::geQualityDefault)) {
			// Add task to task list
			CAutoLock Lock(gpApplication->mMutex_Progress);
			gpApplication->mpProgressTasks->Add(pExportClipTask);
			gpApplication->Playback_InProgressTask();
			
		}
		else {
			// We can't add this anyway - what to do?
			delete pExportClipTask;
			pTake_Data = NULL;
		}
	}
}


void CKSUpload_Controller::Compress_MP3_File()
{
	std::list<CTake_Data*>::iterator it = mMp3_Compress_Que.begin();
	for ( ; it != mMp3_Compress_Que.end(); it++) {
		CTake_Data* pTake = *it;
		std::string sMp3 = gpApplication->MP3_File_Folder() + pTake->Get_UUID() + ".mp3";
		CExportClipTask* pTask = new CExportClipTask();
		if (pTask->Init(pTake, sMp3.c_str(), ac::geAudioCodecLame, ac::geQualityLow)) {
			// Add task to task list
			CAutoLock Lock(gpApplication->mMutex_Progress);
			gpApplication->mpProgressTasks->Add(pTask);
		}
		else {
			// We can't add this anyway - what to do?
			delete pTask;
			pTask = NULL;
		}
	}

}









