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

	geUpload_Take_Before,
	geUpload_Take_Action,
	geUpload_Take_After,

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
		CKSUUID* pProjUUID, const tchar* pszProjName, const tchar* pszProjDesc, const tchar* pszProjLicenseCode,
		CKSUUID* pBranchUUID,
		IFile* pfileProj,
		std::list<CTake_Data*> listpTakes);

	tbool Init_Commit(
		CKSUUID* pProjUUID,
		CKSUUID* pBranchUUID,
		IFile* pfileProj, const tchar* pszCommitDesc,
		std::list<CTake_Data*> listpTakes);

	tbool Init_NewBranch(
		CKSUUID* pProjUUID,
		CKSUUID* pBranchUUID, const tchar* pszBranchName, const tchar* pszBranchDesc, const tchar* pszBranchLicenseCode,
		IFile* pfileProj,
		std::list<CTake_Data*> listpTakes);

	virtual tbool DoWork();
	virtual tbool IsDone();

protected:
	tbool DoNewProject_Before() { return true; };
	tbool DoNewProject_Action() { return true; };
	tbool DoNewProject_After() { return true; };

	tbool DoNewBranch_Before() { return true; };
	tbool DoNewBranch_Action() { return true; };
	tbool DoNewBranch_After() { return true; };

	tbool DoCommit_Before() { return true; };
	tbool DoCommit_Action() { return true; };
	tbool DoCommit_After() { return true; };

	tbool DoTake_Before() { return true; };
	tbool DoTake_Action() { return true; };
	tbool DoTake_After() { return true; };

}; // CExportClipTask
