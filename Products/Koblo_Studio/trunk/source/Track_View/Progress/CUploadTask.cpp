
#include "KSOS.h"


CUploadTask::CUploadTask()
{
	miActionOrder = 0;

	mpfileCommitXML = NULL;

	mpDownloader_VerifySample = NULL;
	mpfileReply_VerifySample = NULL;

	mpDownloader_VerifyTake = NULL;
	mpfileReply_VerifyTake = NULL;

	mpUploader = NULL;
	mpfileReply_Uploader = NULL;

	mpTakeCurr = NULL;
	mpfileMp3 = NULL;
	mpfileOgg = NULL;
} // constructor


CUploadTask::~CUploadTask()
{
	if (mpUploader) {
		delete mpUploader;
		mpUploader = NULL;
	}
	if (mpfileReply_Uploader) {
		delete mpfileReply_Uploader;
		mpfileReply_Uploader = NULL;
	}
	//
	if (mpDownloader_VerifySample) {
		delete mpDownloader_VerifySample;
		mpDownloader_VerifySample = NULL;
	}
	if (mpfileReply_VerifySample) {
		delete mpfileReply_VerifySample;
		mpfileReply_VerifySample = NULL;
	}
	//
	if (mpDownloader_VerifyTake) {
		delete mpDownloader_VerifyTake;
		mpDownloader_VerifyTake = NULL;
	}
	if (mpfileReply_VerifyTake) {
		delete mpfileReply_VerifyTake;
		mpfileReply_VerifyTake = NULL;
	}
	//
	std::list<CTake_Data*>::iterator it = mlistpTakes.begin();
	for ( ; it != mlistpTakes.end(); it++) {
		CTake_Data* pTake = *it;
		delete pTake;
		mlistpTakes.pop_back();
	}
	if (mpTakeCurr) {
		delete mpTakeCurr;
		mpTakeCurr = NULL;
	}
	if (mpfileMp3) {
		delete mpfileMp3;
		mpfileMp3 = NULL;
	}
	if (mpfileOgg) {
		delete mpfileOgg;
		mpfileOgg = NULL;
	}
} // destructor


void CUploadTask::Destroy()
{
	delete dynamic_cast<CUploadTask*>(this);
} // Destroy


tbool CUploadTask::Init_NewProject(
	const tchar* pszUser, const tchar* pszPassword,
	CKSUUID* pProjUUID, const tchar* pszProjName, const tchar* pszProjDesc, const tchar* pszProjLicenseCode,
	CKSUUID* pBranchUUID,
	CKSUUID* pCommitUUID, IFile* pfileCommit,
	std::list<CTake_Data*>* plistpTakes)
{
	if (miActionOrder != 0) {
		msExtendedError = "Double initialization";
		return false;
	}

	msUser					= pszUser;
	msPassword				= pszPassword;

	msProjectUUID			= pProjUUID->Get_UUID();
	msProjectName			= pszProjName;
	msProjectDescription	= pszProjDesc;
	msProjectLicense		= pszProjLicenseCode;

	// not used: msBranchUUID_Old		= pBranchUUID_Original->Get_UUID();
	msBranchUUID			= pBranchUUID->Get_UUID();
	// not used: msBranchName			= pszBranchName;
	// not used: msBranchDescription		= pszBranchDesc;
	// not used: msBranchLicense			= pszBranchLicenseCode;

	msCommitUUID			= pCommitUUID->Get_UUID();
	mpfileCommitXML			= pfileCommit;
	msCommitDescription		= "Initial Version";

	if (!Init_Helper(plistpTakes))
		return false;

	miActionOrder = geUpload_NewProject_Before;
	return true;
} // Init_NewProject


tbool CUploadTask::Init_NewBranch(
	const tchar* pszUser, const tchar* pszPassword,
	CKSUUID* pProjUUID,
	CKSUUID* pBranchUUID_Original,
	CKSUUID* pBranchUUID, const tchar* pszBranchName, const tchar* pszBranchDesc, const tchar* pszBranchLicenseCode,
	CKSUUID* pCommitUUID, IFile* pfileCommit,
	std::list<CTake_Data*>* plistpTakes)
{
	if (miActionOrder != 0) {
		msExtendedError = "Double initialization";
		return false;
	}

	msUser					= pszUser;
	msPassword				= pszPassword;

	msProjectUUID			= pProjUUID->Get_UUID();
	// not used: msProjectName			= pszProjName;
	// not used: msProjectDescription	= pszProjDesc;
	// not used: msProjectLicense		= pszProjLicenseCode;

	msBranchUUID_Old		= pBranchUUID_Original->Get_UUID();
	msBranchUUID			= pBranchUUID->Get_UUID();
	msBranchName			= pszBranchName;
	msBranchDescription		= pszBranchDesc;
	msBranchLicense			= pszBranchLicenseCode;

	msCommitUUID			= pCommitUUID->Get_UUID();
	mpfileCommitXML			= pfileCommit;
	msCommitDescription		= "Initial Copy";

	if (!Init_Helper(plistpTakes))
		return false;

	miActionOrder = geUpload_NewBranch_Before;
	return true;
} // Init_NewBranch


tbool CUploadTask::Init_Commit(
	const tchar* pszUser, const tchar* pszPassword,
	CKSUUID* pProjUUID,
	CKSUUID* pBranchUUID,
	CKSUUID* pCommitUUID, IFile* pfileCommit, const tchar* pszCommitDesc,
	std::list<CTake_Data*>* plistpTakes)
{
	if (miActionOrder != 0) {
		msExtendedError = "Double initialization";
		return false;
	}

	msUser					= pszUser;
	msPassword				= pszPassword;

	msProjectUUID			= pProjUUID->Get_UUID();
	// not used: msProjectName			= pszProjName;
	// not used: msProjectDescription	= pszProjDesc;
	// not used: msProjectLicense		= pszProjLicenseCode;

	// not used: msBranchUUID_Old		= pBranchUUID_Original->Get_UUID();
	msBranchUUID			= pBranchUUID->Get_UUID();
	// not used: msBranchName			= pszBranchName;
	// not used: msBranchDescription		= pszBranchDesc;
	// not used: msBranchLicense			= pszBranchLicenseCode;

	msCommitUUID			= pCommitUUID->Get_UUID();
	mpfileCommitXML			= pfileCommit;
	msCommitDescription		= pszCommitDesc;

	if (!Init_Helper(plistpTakes))
		return false;

	miActionOrder = geUpload_Commit_Before;
	return true;
} // Init_Commit


tbool CUploadTask::Init_Helper(std::list<CTake_Data*>* plistpTakes)
{
	std::list<CTake_Data*>::iterator it = plistpTakes->begin();
	for ( ; it != plistpTakes->end(); it++) {
		CTake_Data* pTake = *it;
		CTake_Data* pTake_Copy = new CTake_Data(*pTake);
		mlistpTakes.push_back(pTake_Copy);
	}

	if (mpUploader == NULL) {
		mpUploader = ine::IUploader::Create();
	}

	return (mpUploader != NULL);
} // Init_Helper


tbool CUploadTask::DoWork()
{
	tbool bSuccess = false;
	tbool bContinue = false;
	tbool bSkip = false;
	tbool bAllDone = false;

	switch (miActionOrder) {
		case geUpload_Uninitialized:
			msExtendedError = "Internal error: Uninitialized";
			break;

		case geUpload_NewProject_Before:
			bSuccess = DoNewProject_Before();
			miActionOrder++;
			break;
		case geUpload_NewProject_Action:
			bSuccess = DoNewProject_Action(&bContinue);
			if (bContinue) miActionOrder++;
			break;
		case geUpload_NewProject_After:
			bSuccess = DoNewProject_After();
			// Jump to commit
			miActionOrder = geUpload_Commit_Before;
			break;

		case geUpload_NewBranch_Before:
			bSuccess = DoNewBranch_Before();
			miActionOrder++;
			break;
		case geUpload_NewBranch_Action:
			bSuccess = DoNewBranch_Action(&bContinue);
			if (bContinue) miActionOrder++;
			break;
		case geUpload_NewBranch_After:
			//!!! TODO: Important!!!! Remember that we've created branch!
			bSuccess = DoNewBranch_After();
			// Jump to commit
			miActionOrder = geUpload_Commit_Before;
			break;

		case geUpload_Commit_Before:
			bSuccess = DoCommit_Before();
			miActionOrder++;
			break;
		case geUpload_Commit_Action:
			bSuccess = DoCommit_Action(&bContinue);
			if (bContinue) miActionOrder++;
			break;
		case geUpload_Commit_After:
			//!!! TODO: Important!!!! Remember that we've created branch!
			bSuccess = DoCommit_After(&bAllDone);
			// Where to go?
			if (bAllDone) {
				// No takes to upload - we're done
				miActionOrder = geUpload_Done;
			}
			else {
				// Jump to takes
				miActionOrder = geUpload_Take_Verify_Before;
			}
			break;

		case geUpload_Take_Verify_Before:
			bSuccess = DoTake_Verify_Before();
			miActionOrder++;
			break;
		case geUpload_Take_Verify_Action:
			bSuccess = DoTake_Verify_Action(&bContinue);
			if (bContinue) miActionOrder++;
			break;
		case geUpload_Take_Verify_After:
			bSuccess = DoTake_Verify_After(&bSkip, &bAllDone);
			if (!bSkip) {
				// This take is brand new - continue upload
				miActionOrder++;
			}
			else {
				if (bAllDone) {
					// No more takes
					miActionOrder = geUpload_Take_Upload_After + 1;
				}
				else {
					// Go to next take
					miActionOrder = geUpload_Take_Verify_Before;
				}
			}
			break;

		case geUpload_Take_Upload_Before:
			bSuccess = DoTake_Upload_Before();
			miActionOrder++;
			break;
		case geUpload_Take_Upload_Action:
			bSuccess = DoTake_Upload_Action(&bContinue);
			if (bContinue) miActionOrder++;
			break;
		case geUpload_Take_Upload_After:
			//!!! TODO: Important!!!! Remember that we've got a url for take!
			bSuccess = DoTake_Upload_After(&bAllDone);
			if (bAllDone) {
				// Done
				miActionOrder++;
			}
			else {
				// Go to next take
				miActionOrder = geUpload_Take_Verify_Before;
			}
			break;

		default:
			// Why are we here?
			break;
	}

	if (!bSuccess) {
		miActionOrder = geUpload_Done;
	}
	return bSuccess;
} // DoWork


tbool CUploadTask::IsDone()
{
	return (miActionOrder >= geUpload_Done);
} // IsDone


tbool CUploadTask::DoNewProject_Before()
{
	if (mpfileReply_Uploader) {
		// Previously used - close
		delete mpfileReply_Uploader;
		mpfileReply_Uploader = NULL;
	}
	mpfileReply_Uploader = IFileMemory::Create();

	if (
		(!mpUploader->Init("koblo.com", "/projects.xml", 80, msUser.c_str(), msPassword.c_str()))
		||
		(!mpUploader->AddParam("project[uuid]", msProjectUUID.c_str(), -1))
		||
		(!mpUploader->AddParam("project[name]", msProjectName.c_str(), -1))
		||
		(!mpUploader->AddParam("project[description]", msProjectDescription.c_str(), -1))
		||
		(!mpUploader->AddParam("project[license]", msProjectLicense.c_str(), -1))
		||
		(!mpUploader->AddParam("branch[uuid]", msBranchUUID.c_str(), -1))
		||
		(!mpUploader->Start(mpfileReply_Uploader))
	) {
		tchar pszErr[1024];
		mpUploader->GetError(pszErr, 1024);
		msExtendedError = "Init+start of create project failed:\n";
		msExtendedError += pszErr;
		return false;
	}
	return true;
} // DoNewProject_Before

tbool CUploadTask::DoNewProject_Action(tbool* pbDone)
{
	if (mpUploader->IsFailed()) {
		tchar pszErr[1024];
		mpUploader->GetError(pszErr, 1024);
		msExtendedError = std::string("Create project failed:\n") + pszErr;
		tuint64 uiReplySize = mpfileReply_Uploader->GetMemorySize();
		if (uiReplySize > 1) {
			msExtendedError += "\n\n";
			msExtendedError += (tchar*)(mpfileReply_Uploader->GetMemoryPtr());
		}
		return false;
	}

	tint64 iUp_Progress = 0;
	tint64 iUp_Total = 1;
	tint64 iReply_Progress = 0;
	tint64 iReply_Total = 1;
	mpUploader->GetProgress(&iUp_Progress, &iUp_Total, &iReply_Progress, &iReply_Total);
	muiProgressIx = (tuint64)(iUp_Progress + iReply_Progress);
	muiProgressTarget = (tuint64)(iUp_Total + iReply_Total);

	if (mpUploader->IsDone())
		*pbDone = true;

	return true;
} // DoNewProject_Action

tbool CUploadTask::DoNewProject_After()
{
	// Nothing here now
	return true;
} // DoNewProject_After


tbool CUploadTask::DoNewBranch_Before()
{
	if (mpfileReply_Uploader) {
		// Previously used - close
		delete mpfileReply_Uploader;
		mpfileReply_Uploader = NULL;
	}
	mpfileReply_Uploader = IFileMemory::Create();

	std::string sURI = std::string("/branches/") + msBranchUUID_Old + "branch.xml";
	if (
		(!mpUploader->Init("koblo.com", sURI.c_str(), 80, msUser.c_str(), msPassword.c_str()))
		||
		(!mpUploader->AddParam("branch[uuid]", msBranchUUID.c_str(), -1))
		||
		(!mpUploader->AddParam("branch[name]", msBranchName.c_str(), -1))
		||
		(!mpUploader->AddParam("branch[description]", msBranchDescription.c_str(), -1))
		||
		(!mpUploader->AddParam("branch[license]", msBranchLicense.c_str(), -1))
		||
		(!mpUploader->Start(mpfileReply_Uploader))
	) {
		tchar pszErr[1024];
		mpUploader->GetError(pszErr, 1024);
		msExtendedError = "Init+start of create branch failed:\n";
		msExtendedError += pszErr;
		return false;
	}
	return true;
} // DoNewBranch_Before

tbool CUploadTask::DoNewBranch_Action(tbool* pbDone)
{
	if (mpUploader->IsFailed()) {
		tchar pszErr[1024];
		mpUploader->GetError(pszErr, 1024);
		msExtendedError = std::string("Create branch failed:\n") + pszErr;
		tuint64 uiReplySize = mpfileReply_Uploader->GetMemorySize();
		if (uiReplySize > 1) {
			msExtendedError += "\n\n";
			msExtendedError += (tchar*)(mpfileReply_Uploader->GetMemoryPtr());
		}
		return false;
	}

	tint64 iUp_Progress = 0;
	tint64 iUp_Total = 1;
	tint64 iReply_Progress = 0;
	tint64 iReply_Total = 1;
	mpUploader->GetProgress(&iUp_Progress, &iUp_Total, &iReply_Progress, &iReply_Total);
	muiProgressIx = (tuint64)(iUp_Progress + iReply_Progress);
	muiProgressTarget = (tuint64)(iUp_Total + iReply_Total);

	if (mpUploader->IsDone())
		*pbDone = true;

	return true;
} // DoNewBranch_Action

tbool CUploadTask::DoNewBranch_After()
{
	// Nothing here now
	return true;
} // DoNewBranch_After


tbool CUploadTask::DoCommit_Before()
{
	if (mpfileReply_Uploader) {
		// Previously used - close
		delete mpfileReply_Uploader;
		mpfileReply_Uploader = NULL;
	}
	mpfileReply_Uploader = IFileMemory::Create();

	std::string sURI = std::string("/branches/") + msBranchUUID + "commits.xml";
	if (
		(!mpUploader->Init("koblo.com", sURI.c_str(), 80, msUser.c_str(), msPassword.c_str()))
		||
		(!mpUploader->AddParam("commit[uuid]", msCommitUUID.c_str(), -1))
		||
		(!mpUploader->AddParam("commit[description]", msCommitDescription.c_str(), -1))
		||
		(!mpUploader->AddFileParam("commit[markup]", mpfileCommitXML))
		||
		(!mpUploader->Start(mpfileReply_Uploader))
	) {
		tchar pszErr[1024];
		mpUploader->GetError(pszErr, 1024);
		msExtendedError = "Init+start of commit project file failed:\n";
		msExtendedError += pszErr;
		return false;
	}
	return true;
} // DoCommit_Before

tbool CUploadTask::DoCommit_Action(tbool* pbDone)
{
	if (mpUploader->IsFailed()) {
		tchar pszErr[1024];
		mpUploader->GetError(pszErr, 1024);
		msExtendedError = std::string("Commit of project file failed:\n") + pszErr;
		tuint64 uiReplySize = mpfileReply_Uploader->GetMemorySize();
		if (uiReplySize > 1) {
			msExtendedError += "\n\n";
			msExtendedError += (tchar*)(mpfileReply_Uploader->GetMemoryPtr());
		}
		return false;
	}

	tint64 iUp_Progress = 0;
	tint64 iUp_Total = 1;
	tint64 iReply_Progress = 0;
	tint64 iReply_Total = 1;
	mpUploader->GetProgress(&iUp_Progress, &iUp_Total, &iReply_Progress, &iReply_Total);
	muiProgressIx = (tuint64)(iUp_Progress + iReply_Progress);
	muiProgressTarget = (tuint64)(iUp_Total + iReply_Total);

	if (mpUploader->IsDone())
		*pbDone = true;

	return true;
} // DoCommit_Action

tbool CUploadTask::DoCommit_After(tbool* pbDone)
{
	if (mlistpTakes.size() == 0) {
		// No takes to upload - we're done
		*pbDone = true;
	}

	return true;
} // DoCommit_After


tbool CUploadTask::DoTake_Verify_Before()
{
	if (mlistpTakes.size() == 0) {
		msExtendedError = "Internal error: No upload takes in list";
		return false;
	}
	if (mpTakeCurr) {
		// Release this take - it has already been handled
		delete mpTakeCurr;
		mpTakeCurr = NULL;
	}
	mpTakeCurr = mlistpTakes.front();
	mlistpTakes.pop_front();

	if (mpfileReply_VerifySample) {
		// Release file - it was from previous transfer
		delete mpfileReply_VerifySample;
		mpfileReply_VerifySample = NULL;
	}
	if (mpfileReply_VerifyTake) {
		// Release file - it was from previous transfer
		delete mpfileReply_VerifyTake;
		mpfileReply_VerifyTake = NULL;
	}
	mpfileReply_VerifySample = IFileMemory::Create();
	mpfileReply_VerifyTake = IFileMemory::Create();
	if ((mpfileReply_VerifySample == NULL) || (mpfileReply_VerifyTake == NULL))
		return false;

	if (mpDownloader_VerifySample == NULL)
		mpDownloader_VerifySample = ine::IDownloader::Create();
	if (mpDownloader_VerifyTake == NULL)
		mpDownloader_VerifyTake = ine::IDownloader::Create();
	if ((mpDownloader_VerifySample == NULL) || (mpDownloader_VerifyTake == NULL))
		return false;

	std::string sURI_VerifySample = std::string("/samples/") + mpTakeCurr->GetUUID_ForOwningSample();
	sURI_VerifySample += ".xml";
	if ((!mpDownloader_VerifySample->Init("koblo.com", sURI_VerifySample.c_str(), 80, msUser.c_str(), msPassword.c_str()))
		||
		(!mpDownloader_VerifySample->Start(mpfileReply_VerifySample))
	) {
		tchar pszErr[1024];
		mpDownloader_VerifySample->GetError(pszErr, 1024);
		msExtendedError = std::string("Init+start of verify-sample failed:\n") + pszErr;
		return false;
	}
	std::string sURI_VerifyTake = std::string("/takes/") + mpTakeCurr->Get_UUID();
	sURI_VerifyTake += ".xml";
	if ((!mpDownloader_VerifyTake->Init("koblo.com", sURI_VerifyTake.c_str(), 80, msUser.c_str(), msPassword.c_str()))
		||
		(!mpDownloader_VerifyTake->Start(mpfileReply_VerifyTake))
	) {
		tchar pszErr[1024];
		mpDownloader_VerifyTake->GetError(pszErr, 1024);
		msExtendedError = std::string("Init+start of verify-take failed:\n") + pszErr;
		return false;
	}

	muiProgressIx = 0;
	muiProgressTarget = 1;
	msProgress = std::string("Looking up ") + mpTakeCurr->Screen_Name();

	return true;
} // DoTake_Verify_Before

tbool CUploadTask::DoTake_Verify_Action(tbool* pbDone)
{
	tint64 iVerifySample_Progress = 0;
	tint64 iVerifySample_Total = 1;
	if (!mpDownloader_VerifySample->GetProgress(&iVerifySample_Progress, &iVerifySample_Total)) {
		tchar pszErr[1024];
		mpDownloader_VerifySample->GetError(pszErr, 1024);
		msExtendedError = pszErr;
		return false;
	}

	tint64 iVerifyTake_Progress = 0;
	tint64 iVerifyTake_Total = 1;
	if (!mpDownloader_VerifyTake->GetProgress(&iVerifyTake_Progress, &iVerifyTake_Total)) {
		tchar pszErr[1024];
		mpDownloader_VerifyTake->GetError(pszErr, 1024);
		msExtendedError = pszErr;
		return false;
	}

	muiProgressIx = (tuint64)(iVerifySample_Progress + iVerifyTake_Progress);
	muiProgressTarget = (tuint64)(iVerifySample_Total + iVerifyTake_Total);

	// We don't care for the actual status of the downloads - we test that later
	tint32 iFinished = 0;
	if (mpDownloader_VerifySample->IsFinished()) {
		iFinished++;
	}
	if (mpDownloader_VerifyTake->IsFinished()) {
		iFinished++;
	}

	*pbDone = (iFinished == 2);
	return true;
} // DoTake_Verify_Action

tbool CUploadTask::DoTake_Verify_After(tbool* pbSkipThis, tbool* pbAllDone)
{
	tbool bTakeThere = mpDownloader_VerifyTake->IsDone();
	if (!bTakeThere) {
		// Status 404 means "page not there" - that's ok ..
		if (mpDownloader_VerifyTake->GetHttpStatusCode() != 404) {
			// .. but this on the other hand is an unexpected error
			tchar pszErr[1024];
			mpDownloader_VerifyTake->GetError(pszErr, 1024);
			msExtendedError = std::string("Verify take failed:\n") + pszErr;
			tuint64 uiReplySize = mpfileReply_VerifyTake->GetMemorySize();
			if (uiReplySize > 1) {
				msExtendedError += "\n\n";
				msExtendedError += (tchar*)(mpfileReply_VerifyTake->GetMemoryPtr());
			}
			return false;
		}
	}

	if (bTakeThere) {
		// Take has already been uploaded
		*pbSkipThis = true;

		// We came here because we didn't have URL for take - fix that
		//!!! TODO: Important!!! We must save url of take!

		if (mlistpTakes.size() == 0) {
			// No more takes
			*pbAllDone = true;
		}
	}

	return true;
} // DoTake_Verify_After


tbool CUploadTask::DoTake_Upload_Before()
{
	tbool bSampleThere = mpDownloader_VerifySample->IsDone();
	if (!bSampleThere) {
		// Status 404 means "page not there" - that's ok ..
		if (mpDownloader_VerifySample->GetHttpStatusCode() != 404) {
			// .. but this on the other hand is an unexpected error
			tchar pszErr[1024];
			mpDownloader_VerifySample->GetError(pszErr, 1024);
			msExtendedError = std::string("Verify sample failed:\n") + pszErr;
			tuint64 uiReplySize = mpfileReply_VerifySample->GetMemorySize();
			if (uiReplySize > 1) {
				msExtendedError += "\n\n";
				msExtendedError += (tchar*)(mpfileReply_VerifySample->GetMemoryPtr());
			}
			return false;
		}
	}

	if (mpfileOgg) {
		delete mpfileOgg;
		mpfileOgg = NULL;
	}
	if (mpfileMp3) {
		delete mpfileMp3;
		mpfileMp3 = NULL;
	}
	mpfileOgg = IFile::Create();
	mpfileMp3 = IFile::Create();
	if (mpfileMp3 == NULL || mpfileOgg == NULL) {
		return false;
	}
	std::string sOgg = mpTakeCurr->GetOggFilePath();
	std::string sMp3 = mpTakeCurr->GetMp3FilePath();
	if (!mpfileOgg->Open(sOgg.c_str(), IFile::FileRead)) {
		msExtendedError = std::string("Can't read-open file:\n") + sOgg;
		return false;
	}
	if (!mpfileMp3->Open(sMp3.c_str(), IFile::FileRead)) {
		msExtendedError = std::string("Can't read-open file:\n") + sMp3;
		return false;
	}

	tbool bInitError = false;
	if (bSampleThere) {
		// Previously uploaded sample - just upload a new take onto it
		std::string sURI = std::string("/samples/");
		sURI += mpTakeCurr->GetUUID_ForOwningSample();
		sURI += "/takes.xml";
		if (
			(!mpUploader->Init("koblo.com", sURI.c_str(), 80, msUser.c_str(), msPassword.c_str()))
			||
			(!mpUploader->AddParam("take[uuid]", mpTakeCurr->Get_UUID().c_str(), -1))
			||
			(!mpUploader->AddParam("take[description]", mpTakeCurr->Get_Description().c_str(), -1))
			||
			(!mpUploader->AddFileParam("audio[uploaded_data]", mpfileOgg))
			||
			(!mpUploader->AddFileParam("mp3[uploaded_data]", mpfileMp3))
		) {
			bInitError = true;
		}
	}
	else {
		// All new sample - create sample + upload take
		std::string sURI = std::string("/projects/");
		sURI += msProjectUUID;
		sURI += "/samples.xml";
		if (
			(!mpUploader->Init("koblo.com", sURI.c_str(), 80, msUser.c_str(), msPassword.c_str()))
			||
			(!mpUploader->AddParam("sample[uuid]", mpTakeCurr->GetUUID_ForOwningSample().c_str(), -1))
			||
			(!mpUploader->AddParam("sample[name]", mpTakeCurr->GetName_ForOwningSample().c_str(), -1))
			||
			(!mpUploader->AddParam("take[uuid]", mpTakeCurr->Get_UUID().c_str(), -1))
			||
			(!mpUploader->AddParam("take[description]", mpTakeCurr->Get_Description().c_str(), -1))
			||
			(!mpUploader->AddFileParam("audio[uploaded_data]", mpfileOgg))
			||
			(!mpUploader->AddFileParam("mp3[uploaded_data]", mpfileMp3))
		) {
			bInitError = true;
		}
	}

	if (bInitError) {
		tchar pszErr[1024];
		mpUploader->GetError(pszErr, 1024);
		msExtendedError = "Create branch failed:\n";
		msExtendedError += pszErr;
		return false;
	}

	muiProgressIx = 0;
	muiProgressTarget = 1;
	msProgress = std::string("Uploading ") + mpTakeCurr->Screen_Name();

	return true;
} // DoTake_Upload_Before

tbool CUploadTask::DoTake_Upload_Action(tbool* pbDone)
{
	if (mpUploader->IsFailed()) {
		tchar pszErr[1024];
		mpUploader->GetError(pszErr, 1024);
		msExtendedError = std::string("Upload of take failed:\n") + pszErr;
		tuint64 uiReplySize = mpfileReply_Uploader->GetMemorySize();
		if (uiReplySize > 1) {
			msExtendedError += "\n\n";
			msExtendedError += (tchar*)(mpfileReply_Uploader->GetMemoryPtr());
		}
		return false;
	}

	tint64 iUp_Progress = 0;
	tint64 iUp_Total = 1;
	tint64 iReply_Progress = 0;
	tint64 iReply_Total = 1;
	mpUploader->GetProgress(&iUp_Progress, &iUp_Total, &iReply_Progress, &iReply_Total);
	muiProgressIx = (tuint64)(iUp_Progress + iReply_Progress);
	muiProgressTarget = (tuint64)(iUp_Total + iReply_Total);

	if (mpUploader->IsDone())
		*pbDone = true;

	return true;
} // DoTake_Upload_Action

tbool CUploadTask::DoTake_Upload_After(tbool* pbAllDone)
{
	if (mlistpTakes.size() == 0) {
		// No more takes
		*pbAllDone = true;
	}
	return true;
} // DoTake_Upload_After


