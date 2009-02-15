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


class CUploadTask : public CProgressTask {
public:
	tbool bSuccess;

	// State machine "enum" (it's an int to allow for ++ )
	tint32 miActionOrder;

	CUploadTask();
	virtual ~CUploadTask();
	virtual void Destroy();

	tbool Init_NewProject(
						  const tchar* pszUser, 
						  const tchar* pszPassword,
						  const tchar* pszProjUUID, 
						  const tchar* pszProjName, 
						  const tchar* pszProjDesc, 
						  const tchar* pszProjLicenseCode,
						  const tchar* pszBranchUUID,
						  const tchar* pszCommitUUID, 
						  const tchar* pszfileCommit,
						  std::list<CTake_Data*>* plistpTakes);

	tbool Init_NewBranch(
		const tchar* pszUser, const tchar* pszPassword,
		const tchar* pszProjUUID,
		const tchar* pszBranchUUID_Original,
		const tchar* pszBranchUUID, const tchar* pszBranchName, const tchar* pszBranchDesc, const tchar* pszBranchLicenseCode,
		const tchar* pszCommitUUID, const tchar* pszfileCommit,
		std::list<CTake_Data*>* plistpTakes);

	tbool Init_Commit(
		const tchar* pszUser, const tchar* pszPassword,
		const tchar* pszProjUUID,
		const tchar* pszBranchUUID,
		const tchar* pszCommitUUID, const tchar* pszfileCommit, const tchar* pszCommitDesc,
		std::list<CTake_Data*>* plistpTakes);

	virtual tbool DoWork();
	virtual tbool IsDone();

protected:
	tbool DoBranch_PreVerify_Before();
	tbool DoBranch_PreVerify_Action(tbool* pbActionDone);
	tbool DoBranch_PreVerify_After(tbool* pbAlreadyThere, tbool* pbMoreTakes);

	tint32 miAction_Branch_PreVerify_WhereToGoNow;

	tbool DoNewProject_Before();
	tbool DoNewProject_Action(tbool* pbActionDone);
	tbool DoNewProject_After(tbool* pbNoTakes);

	tbool DoNewBranch_Before();
	tbool DoNewBranch_Action(tbool* pbActionDone);
	tbool DoNewBranch_After(tbool* pbNoTakes);

	tbool DoTake_PreVerify_Before();
	tbool DoTake_PreVerify_Action(tbool* pbActionDone);
	tbool DoTake_PreVerify_After(tbool* pbSkipThisTake, tbool* pbNoMoreTakes);

	tbool DoTake_Upload_Before();
	tbool DoTake_Upload_Action(tbool* pbActionDone);
	tbool DoTake_Upload_After(tbool* pbNoMoreTakes);

	tbool DoCommit_PreVerify_Before();
	tbool DoCommit_PreVerify_Action(tbool* pbActionDone);
	tbool DoCommit_PreVerify_After(tbool* pbAlreadyThere);

	tbool DoCommit_Upload_Before();
	tbool DoCommit_Upload_Action(tbool* pbActionDone);
	tbool DoCommit_Upload_After();

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
	std::string msFileCommitXML;
	IFile* mpfileCommitXML;
	std::string msCommitXML;
	std::string msCommitDescription;
	//
	tint32 miCommit_ReplyNb;

	std::list<CTake_Data*> mlistpTakes;
	tbool Init_Helper(std::list<CTake_Data*>* plistpTakes);
	CTake_Data* mpTakeCurr;
	IFile* mpfileMp3;
	IFile* mpfileOgg;

	ine::IUploader* mpUploader;
	IFileMemory* mpfileReply_Uploader;

	ine::IDownloader* mpDownloader_VerifySample;
	IFileMemory* mpfileReply_VerifySample;
	ine::IDownloader* mpDownloader_VerifyTake;
	IFileMemory* mpfileReply_VerifyTake;

	ine::IDownloader* mpDownloader_VerifyBranch;
	IFileMemory* mpfileReply_VerifyBranch;

	ine::IDownloader* mpDownloader_VerifyCommit;
	IFileMemory* mpfileReply_VerifyCommit;

	tbool SetTakeURL(IFileMemory* pfile);
}; // CExportClipTask
