
#include "KSOS.h"


CUploadTask::CUploadTask()
{
	miActionOrder = 0;
} // constructor


CUploadTask::~CUploadTask()
{
} // destructor


void CUploadTask::Destroy()
{
	delete dynamic_cast<CUploadTask*>(this);
} // Destroy


tbool CUploadTask::Init_NewProject(
	CKSUUID* pProjUUID, const tchar* pszProjName, const tchar* pszProjDesc, const tchar* pszProjLicenseCode,
	CKSUUID* pBranchUUID,
	IFile* pfileProj,
	std::list<CTake_Data*> listpTakes)
{
	miActionOrder = geUpload_NewProject_Before;
	return true;
} // Init_NewProject


tbool CUploadTask::Init_Commit(
	CKSUUID* pProjUUID,
	CKSUUID* pBranchUUID,
	IFile* pfileProj, const tchar* pszCommitDesc,
	std::list<CTake_Data*> listpTakes)
{
	miActionOrder = geUpload_Commit_Before;
	return true;
} // Init_Commit


tbool CUploadTask::Init_NewBranch(
	CKSUUID* pProjUUID,
	CKSUUID* pBranchUUID, const tchar* pszBranchName, const tchar* pszBranchDesc, const tchar* pszBranchLicenseCode,
	IFile* pfileProj,
	std::list<CTake_Data*> listpTakes)
{
	miActionOrder = geUpload_NewBranch_Before;
	return true;
} // Init_NewBranch


tbool CUploadTask::DoWork()
{
	tbool bSuccess = false;

	switch (miActionOrder) {
		case geUpload_Uninitialized:
			msExtendedError = "Internal error: Uninitialized";
			break;

		case geUpload_NewProject_Before:
			bSuccess = DoNewProject_Before();
			miActionOrder++;
			break;
		case geUpload_NewProject_Action:
			bSuccess = DoNewProject_Action();
			miActionOrder++;
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
			bSuccess = DoNewBranch_Action();
			miActionOrder++;
			break;
		case geUpload_NewBranch_After:
			// Important!!!! Remember that we've created branch!
			bSuccess = DoNewBranch_After();
			// Jump to commit
			miActionOrder = geUpload_Commit_Before;
			break;

		case geUpload_Commit_Before:
			bSuccess = DoCommit_Before();
			miActionOrder++;
			break;
		case geUpload_Commit_Action:
			bSuccess = DoCommit_Action();
			miActionOrder++;
			break;
		case geUpload_Commit_After:
			// Important!!!! Remember that we've created branch!
			bSuccess = DoCommit_After();
			// Jump to takes
			miActionOrder = geUpload_Take_Before;
			break;

		case geUpload_Take_Before:
			bSuccess = DoTake_Before();
			miActionOrder++;
			break;
		case geUpload_Take_Action:
			bSuccess = DoTake_Action();
			miActionOrder++;
			break;
		case geUpload_Take_After:
			// Important!!!! Remember that we've got a url for take!
			bSuccess = DoTake_After();
			// If more takes
			if (false)
				miActionOrder = geUpload_Take_Before;
			else
				miActionOrder++;
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
