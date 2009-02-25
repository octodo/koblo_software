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

//! Called when a buffer of samples needs to be filled
/*!
 \param iID [in]: ID of callback as when SetCallback was called.
 \param pfOut [out]: Buffer to be filled
 \param iCount [in]: Number of samples to fill in buffer
 */

/*! \class CKSInternet_Features
 * \brief class that handles all Internet features
*/

class CKSApplication;
class CKSUUID;


class CKSInternet_Features
: 
public virtual CKSXML_Create_Project,
public virtual CKSXML_Write_Project,
public virtual CKSXML_Read_Project,
public virtual CKSUsername_And_Password_Model,
public virtual CXML_Hand_Shakes

//public virtual CKSXML_Create_Sample
{

public:
	
	CKSInternet_Features();
	~CKSInternet_Features();
	
	//! called from the menu and opens the project id dialog 
	virtual void On_Menu_Download_Project();
	
	
	
	//! called from the file menu
//	virtual void On_Menu_Update_Project();
	
	//! called from the file menu
	virtual void On_Menu_Upload_Project();
	
	//! clean project and read it from koblo.com
	void Download_Project(tint32 iProjectID);
	
	//! read project from koblo.com
	void Update_Project(tint32 iProjectID);
	
	// Load project
	void Load_Project(tint32 iProjectID);

	
	//! prepare wave files for upload
	void Compress_Wave_File(std::string sSample);
	//!
	void Upload_Sample(std::string sSample);
	
	//!
	void Download_Take();
	
	//! upload compressed audio file and mp3 preview.
	void Upload_OGG_And_MP3_File();
		
	
	void Set_Project_Description( const std::string& sDescription){ msProject_Description = sDescription;}
	
	std::string Get_Project_Description(){ return msBranch_Description;}
	
	void Set_Branch_Name( const std::string& sName);
	
	std::string Get_Branch_Name();
	
	void Set_Branch_Description( const std::string& sName){ msBranch_Description = sName;}
	
	std::string Get_Branch_Description(){ return msBranch_Description;}
	
	void Set_User_Name( const std::string& sName);
	
	std::string Get_User_Name(){ return msUser_Name;}
	
	void Set_Password( const std::string& sPassword);
	
	std::string Get_Password(){ return msPassword;}
	
	void Set_Remember_Me(tbool bRemember_Me);
	
	tbool Get_Remember_Me(){ return mbRemember_Me;}
	
	tbool mbUpload_Project;
	
	//! compares the internal data structure to the xml file stroed on disk
	tbool Project_Has_Changed();
	
	// set branch revision
	void Branch_Revision( tuint32 uiBranch_Revision );
	
	// set branch revision
	void Branch_Revision( std::string sBranch_Revision );
	
	//! get branch revision
	tuint32 Branch_Revision();
	
	//! get branch revision
	tbool Project_Is_Uploaded(){ return muiBranch_Revision != 0;} ;
		
	// get uuid from CKSUUID object
	std::string Get_Project_UUID(){ return mpProject_UUID->Get_UUID();};
	
	// check if project uuid is set
	tbool Project_UUID_Is_Set(){ return mpProject_UUID->UUID_Is_Set();};
	
	// check if project uuid is set
	void Clear_Project_UUID(){  mpProject_UUID->Clear_UUID();};
	
	// set a new uuid
	void Set_Project_UUID(){mpProject_UUID->Set_UUID();};
	
	// set a uuid from a string
	void Set_Project_UUID(std::string  sUUID){ mpProject_UUID->Set_UUID(sUUID);};
	
	// get uuid from CKSUUID object
	std::string Get_Branch_UUID(){ return mpBranch_UUID->Get_UUID();};
	
	// set a new uuid
	void Set_Branch_UUID(){ mpBranch_UUID->Set_UUID();};
	
	// get insert file uuid
	std::string Get_Insert_File_UUID(){ return mpInsert_File_UUID->Get_UUID();};
	
	// set insert file uuid
	void Set_Insert_File_UUID(std::string  sUUID){ mpInsert_File_UUID->Set_UUID( sUUID);};
	
	// set a new insert file uuid
	std::string Get_New_Insert_File_UUID(){ return mpInsert_File_UUID->Get_New_UUID();};
	
	// set a uuid from a string
	void Set_Branch_UUID(std::string sUUID){ mpBranch_UUID->Set_UUID(sUUID);};
	
	// set a new uuid
	void Set_Commit_UUID(){ mpCommit_UUID->Set_UUID();};
	
	// set a uuid from a string
	void Set_Commit_UUID(std::string sUUID){ mpCommit_UUID->Set_UUID(sUUID);};
	
	// get uuid from CKSUUID object
	std::string Get_Commit_UUID(){ return mpCommit_UUID->Get_UUID();};
	
	// get new uuid from CKSUUID object
	std::string Get_New_Commit_UUID(){ return mpCommit_UUID->Get_New_UUID();};
	
	// set a new commit uuid
	void Set_New_Commit_UUID(){ mpCommit_UUID->Get_New_UUID();};
	
	tbool Read_Permission(std::string sXML_Hand_Shake);
	
	//! write permission
	tbool Write_Permission(std::string sXML_Hand_Shake);
 

	// (lasse) important: mutex for callback
	//! This mutex must be taken before doing anything with variables or files that CUploadTask and CDownloadTask may change
	CMutex mMutex_ForUpAndDownloadTask_CallBack;
	
	
private:
	
	//! called from the menu and opens the project id dialog 
	virtual void Ask_For_Project_ID();

	// project uuid object
	CKSUUID* mpProject_UUID;
	
	// branch uuid object
	CKSUUID* mpBranch_UUID;
	
	// branch uuid object
	CKSUUID* mpCommit_UUID;
	
	// insert preset file uuid
	CKSUUID* mpInsert_File_UUID;
	
	virtual void Open_Project_Edit_Page_On_Koblo();
	
	//! project name
//	std::string msProject_Name;
	
	
	//! project path
//	std::string msProject_Path;

	//! project description
	std::string msProject_Description;
	
	//! branch name
	std::string msBranchName;
	
	//! branch description
	std::string msBranch_Description;
	
	//! if true the users sign in informations is stored in the pref file
	tbool mbRemember_Me;
	
	//! user name
	std::string msUser_Name;
	
	//! password description
	std::string msPassword;
	
	tuint32 muiBranch_Revision;

		
};

