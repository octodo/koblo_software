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


	//! Prepare upload of a new project to koblo.com
	/*!
		\param pszUser [in]: User for authentication
		\param pszPassword [in]: Password for authentication
		\param pszProjUUID [in]: UUID for project "container" to create
		\param pszProjName [in]: Name for project
		\param pszProjDesc [in]: Description of project
		\param pszProjLicenseCode [in]: License for project
		\param pszBranchUUID [in]: UUID for initial branch (a.k.a. Trunk)
		\param pszCommitUUID [in]: UUID for first commit of project xml
		\param pszfileCommit [in]: Path to file of project xml
		\param plistpTakes [in]: List of takes to upload. May be NULL or empty.
		\return tbool: True on success. If false error text can be extracted with GetError(..) method
	*/
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


	//! Clone a project branch at koblo.com and create a new branch
	/*!
		\param pszUser [in]: User for authentication
		\param pszPassword [in]: Password for authentication
		\param pszProjUUID [in]: UUID for existing project "container" where new branch will be created
		\param pszBranchUUID_Original [in]: UUID for the branch we wish to clone
		\param pszBranchUUID [in]: UUID for the new branch
		\param pszBranchName [in]: Name for the new branch
		\param pszBranchDesc [in]: Description of the new branch
		\param pszCommitUUID [in]: UUID for first commit to branch of project xml
		\param pszfileCommit [in]: Path to file of project xml
		\param plistpTakes [in]: List of takes to upload. May be NULL or empty.
		\return tbool: True on success. If false error text can be extracted with GetError(..) method
	*/
	tbool Init_NewBranch(
	//	const tchar* pszUser, 
	//	const tchar* pszPassword,
	//	const tchar* pszProjUUID,
		const tchar* pszBranchUUID_Original,
	//	const tchar* pszBranchUUID, 
	//	const tchar* pszBranchName, 
		const tchar* pszBranchDesc,
		const tchar* pszBranchLicenseCode,
		//const tchar* pszCommitUUID, 
		const tchar* pszfileCommit,
		std::list<CTake_Data*>* plistpTakes);


	//! Commit to an existing project & branch at koblo.com
	/*!
		\param pszUser [in]: User for authentication
		\param pszPassword [in]: Password for authentication
		\param pszProjUUID [in]: UUID for existing project "container" where new branch will be created
		\param pszBranchUUID [in]: UUID for existing branch
		\param pszCommitUUID [in]: UUID for commit to branch of project xml
		\param pszfileCommit [in]: Path to file of project xml
		\param pszOnline_fileCommit [in]:  //!!!TODO: What's this???
		\param plistpTakes [in]: List of takes to upload. May be NULL or empty.
		\return tbool: True on success. If false error text can be extracted with GetError(..) method
	*/
	tbool Init_Commit(
	//	const tchar* pszUser, 
	//	const tchar* pszPassword,
	//	const tchar* pszProjUUID,
	//	const tchar* pszBranchUUID,
	//	const tchar* pszCommitUUID, 
		const tchar* pszfileCommit,
		const tchar* pszOnline_fileCommit,
		const tchar* pszCommitDesc,
		std::list<CTake_Data*>* plistpTakes);


	//! Add a preset-data chunk file to next upload. Must be called after any of the Init_xxx methods.
	/*!
		\param pszPresetDataUUID [in]: UUID for this version of preset-data
		\param pszFilePresetData [in]: Path to file of preset-data
		\return tbool: True on success. If false error text can be extracted with GetError(..) method
	*/
	tbool Add_PresetData(const tchar* pszPresetDataUUID, const tchar* pszFilePresetData);


	virtual tbool DoWork();
	virtual tbool IsDone();
	
	void Init_Check_Rights_To_Branch();
	
	

protected:
	
	// Private enum for state machine
	enum EUploadOrder {
		geUpload_Uninitialized,
		
		geUpload_Branch_PreVerify_Before,
		geUpload_Branch_PreVerify_Action,
		geUpload_Branch_PreVerify_After,
		
		geUpload_NewProject_Before,
		geUpload_NewProject_Action,
		geUpload_NewProject_After,
		
		geUpload_NewBranch_Before,
		geUpload_NewBranch_Action,
		geUpload_NewBranch_After,
		
		geUpload_Take_PreVerify_Before,
		geUpload_Take_PreVerify_Action,
		geUpload_Take_PreVerify_After,
		
		geUpload_Take_Upload_Before,
		geUpload_Take_Upload_Action,
		geUpload_Take_Upload_After,
		
		geUpload_PresetSettings_PreVerify_Before,
		geUpload_PresetSettings_PreVerify_Action,
		geUpload_PresetSettings_PreVerify_After,
		
		geUpload_PresetSettings_Upload_Before,
		geUpload_PresetSettings_Upload_Action,
		geUpload_PresetSettings_Upload_After,
		
		geUpload_Commit_PreVerify_Before,
		geUpload_Commit_PreVerify_Action,
		geUpload_Commit_PreVerify_After,
		
		geUpload_Commit_Upload_Before,
		geUpload_Commit_Upload_Action,
		geUpload_Commit_Upload_After,
		
		geLookup_Permissions_To_Branch,
		geRead_Permissions_To_Branch,
		geSet_Permissions_To_Branch,
		
		geUpload_Done
	}; // EUploadOrder
	
	
	
	
	tbool DoBranch_PreVerify_Before();
	tbool DoBranch_PreVerify_Action(tbool* pbActionDone);
	tbool DoBranch_PreVerify_After(tbool* pbAlreadyThere, tbool* pbMoreTakes);

	tint32 miAction_Branch_PreVerify_WhereToGoNow;
	tbool mbRequiresWriteAccess;

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

	tbool DoPresetSettings_PreVerify_Before(tbool* pbAlreadyThere);
	tbool DoPresetSettings_PreVerify_Action(tbool* pbActionDone);
	
	//! verify if the file is uploaded
	tbool DoPresetSettings_PreVerify_After(tbool* pbAlreadyThere);

	tbool DoPresetSettings_Upload_Before();
	tbool DoPresetSettings_Upload_Action(tbool* pbActionDone);
	tbool DoPresetSettings_Upload_After();

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

	std::string msPresetDataUUID;
	IFile* mpfilePresetData;

	std::string msCommitUUID;
	std::string msFileCommitXML;
	std::string msOnline_FileCommitXML;
	IFile* mpfileCommitXML;
	std::string msCommitXML;
	std::string msCommitDescription;

	std::list<CTake_Data*> mlistpTakes;
	tbool Init_Helper(std::list<CTake_Data*>* plistpTakes);
	CTake_Data* mpTakeCurr;
	IFile* mpfileMp3;
	IFile* mpfileOgg;
	
	//! uploader
	ine::IUploader* mpUploader;
	IFileMemory* mpfileReply_Uploader;
	
	//! sample downloader
	ine::IDownloader* mpDownloader_VerifySample;
	IFileMemory* mpfileReply_VerifySample;
	
	//! take downloader
	ine::IDownloader* mpDownloader_VerifyTake;
	IFileMemory* mpfileReply_VerifyTake;
	
	
	//! verify mich downloader
	ine::IDownloader* mpDownloader_VerifyMisc;
	IFileMemory* mpfileReply_VerifyMisc;

	tbool SetTakeURL(IFileMemory* pfile);
	
	
	
	enum EUpload_Stage {
		geUpload_Branch_Stage_1,
		geUpload_Branch_Stage_2,
		geUpload_Branch_Stage_3,
		
		geUpload_Branch
	};
	
	//! verify mich downloader
	ine::IDownloader* mpBranch_Downloader;
	IFileMemory* mpBranch_Reply_File;
	
	// lookup permissions
	tbool Lookup_Permissions_To_Branch();
	// read permissions
	tbool Read_Permissions_To_Branch(tbool* pbActionDone);
	//! pass permission xml file
	tbool Set_Permissions_To_Branch();
	
	tbool More_Takes(){ return mlistpTakes.size() == 0;};

	
}; // CExportClipTask
