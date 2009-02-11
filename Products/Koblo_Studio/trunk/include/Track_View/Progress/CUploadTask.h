// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
//
// This file is part of the Koblo SDK.
//
// the Koblo SDK is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo SDK is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo SDK. If not, see <http://www.gnu.org/licenses/>.


enum EUploadOrder {
	geUpload_Uninitialized,

	geUpload_NewProject_Before,
	geUpload_NewProject_Action,
	geUpload_NewProject_After,

	geUpload_NewBranch_Before,
	geUpload_NewBranch_Action,
	geUpload_NewBranch_After,

	geUpload_Commit_Before,
	geUpload_Commit_Action,
	geUpload_Commit_After,

	geUpload_Take_Verify_Before,
	geUpload_Take_Verify_Action,
	geUpload_Take_Verify_After,

	geUpload_Take_Upload_Before,
	geUpload_Take_Upload_Action,
	geUpload_Take_Upload_After,

	geUpload_Done
}; // EUploadOrder


class CUploadTask : public CProgressTask {
public:
	tbool bSuccess;

	tint32 miActionOrder;

	CUploadTask();
	virtual ~CUploadTask();
	virtual void Destroy();

	tbool Init_NewProject(
						  const tchar* pszUser, 
						  const tchar* pszPassword,
						  CKSUUID* pProjUUID, 
						  const tchar* pszProjName, 
						  const tchar* pszProjDesc, 
						  const tchar* pszProjLicenseCode,
						  CKSUUID* pBranchUUID,
						  CKSUUID* pCommitUUID, 
						  IFile* pFileCommit,
						  std::list<CTake_Data*>* plistpTakes);
	
	tbool Init_NewProject( std::list<CTake_Data*>* pUpload_Que);

	tbool Init_NewBranch(
		const tchar* pszUser, const tchar* pszPassword,
		CKSUUID* pProjUUID,
		CKSUUID* pBranchUUID_Original,
		CKSUUID* pBranchUUID, const tchar* pszBranchName, const tchar* pszBranchDesc, const tchar* pszBranchLicenseCode,
		CKSUUID* pCommitUUID, IFile* pfileCommit,
		std::list<CTake_Data*>* plistpTakes);

	tbool Init_Commit(
		const tchar* pszUser, const tchar* pszPassword,
		CKSUUID* pProjUUID,
		CKSUUID* pBranchUUID,
		CKSUUID* pCommitUUID, IFile* pfileCommit, const tchar* pszCommitDesc,
		std::list<CTake_Data*>* plistpTakes);

	virtual tbool DoWork();
	virtual tbool IsDone();

protected:
	tbool DoNewProject_Before();
	tbool DoNewProject_Action(tbool* pbDone);
	tbool DoNewProject_After();

	tbool DoNewBranch_Before();
	tbool DoNewBranch_Action(tbool* pbDone);
	tbool DoNewBranch_After();

	tbool DoCommit_Before();
	tbool DoCommit_Action(tbool* pbDone);
	tbool DoCommit_After(tbool* pbDone);

	tbool DoTake_Verify_Before();
	tbool DoTake_Verify_Action(tbool* pbDone);
	tbool DoTake_Verify_After(tbool* pbSkipThis, tbool* pbAllDone);

	tbool DoTake_Upload_Before();
	tbool DoTake_Upload_Action(tbool* pbDone);
	tbool DoTake_Upload_After(tbool* pbAllDone);

	std::string msUser;
	std::string msPassword;

	std::string msProjectUUID;
	std::string msProjectName;
	std::string msProjectDescription;
	std::string msProjectLicense;

	std::string msBranchUUID_Old;
	std::string msBranchUUID;
	std::string msBranchName;
	std::string msBranchDescription;
	std::string msBranchLicense;

	std::string msCommitUUID;
	IFile* mpFileCommitXML;
	std::string msCommitDescription;
	//
	tint32 miCommit_ReplyNb;

	CTake_Data* mpTakeCurr;
	IFile* mpfileMp3;
	IFile* mpfileOgg;
	std::list<CTake_Data*> mlistpTakes;
	tbool Init_Helper(std::list<CTake_Data*>* plistpTakes);

	ine::IUploader* mpUploader;
	IFileMemory* mpfileReply_Uploader;

	ine::IDownloader* mpDownloader_VerifySample;
	IFileMemory* mpfileReply_VerifySample;
	ine::IDownloader* mpDownloader_VerifyTake;
	IFileMemory* mpfileReply_VerifyTake;
}; // CExportClipTask
