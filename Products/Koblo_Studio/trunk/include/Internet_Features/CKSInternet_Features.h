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
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.

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



class CKSInternet_Features 
: 
public virtual CKSXML_Create_Project,
public virtual CKSXML_Write_Project,
public virtual CKSXML_Read_Project,
public virtual CKSUsername_And_Password_Model

//public virtual CKSXML_Create_Sample
{

public:
	
	CKSInternet_Features();
	~CKSInternet_Features();
	
	//! called from the menu and opens the project id dialog 
	virtual void On_Menu_Download_Project();
	
	
	
	//! called from the file menu
	virtual void On_Menu_Update_Project();
	
	//! called from the file menu
	virtual void On_Menu_Upload_Project();
	
	//! called from the file menu
	virtual void On_Menu_Commit_Project();
	
	//! clean project and read it from koblo.com
	void Download_Project(tint32 iProjectID);
	//! read project from koblo.com
	void Update_Project(tint32 iProjectID);
	// Load project
	void Load_Project(tint32 iProjectID);
	//! upload project to koblo.com
	void Upload_Project();
	
	//! prepare wave files for upload
	void Compress_Wave_File(std::string sSample);
	//!
	void Upload_Sample(std::string sSample);
	//!
	void Download_Take();
	
	//! upload compressed audio file and mp3 preview.
	void Upload_OGG_And_MP3_File();
	
	//!!! To Do call this
	void SetProjectName( const std::string& sName){ msProject_Name = sName;}
	
	//! get the project name
	std::string GetProjectName(){ return msProject_Name;}
	
	
	
	//! set project path
	void SetProjectPath( const std::string& sName){ msProject_Path = sName;}
	
	std::string GetProjectPath(){ return msProject_Path;}

	
	void SetProjectDescription( const std::string& sDescription){ msProjectDescription = sDescription;}
	
	std::string Get_Project_Description(){ return msBranchDescription;}
	
	void Set_Branch_Name( const std::string& sName){ msBranchName = sName;}
	
	std::string Get_Branch_Name(){ return msBranchName;}
	
	void Set_Branch_Description( const std::string& sName){ msBranchDescription = sName;}
	
	std::string Get_Branch_Description(){ return msBranchDescription;}
	
	void Set_User_Name( const std::string& sName){ msUser_Name = sName;}
	
	std::string Get_User_Name(){ return msUser_Name;}
	
	void Set_Password( const std::string& sPassword){ msPassword = sPassword;}
	
	std::string Get_Password(){ return msPassword;}
	
	void Set_Remember_Me(tbool bRemember_Me){ mbRemember_Me = bRemember_Me;}
	
	tbool Get_Remember_Me(){ return mbRemember_Me;}
	
	tbool mbUpload_Project;
	
	//! compares the internal data structure to the xml file stroed on disk
	tbool Project_Has_Changed();
	
	//! set project uuid
	tbool Set_Project_UUID();
	
	//! set project uuid
	tbool Set_Project_UUID(std::string sProject_UUID){msProject_UUID = msProject_UUID;};
	
	//! clear project uuid
	tbool Clear_Project_UUID(){ msProject_UUID.erase(0, msProject_UUID.length());};
	
	//! get the project uuid
	std::string Get_Project_UUID();
	
	
	
	//! set project uuid
	tbool Set_Branch_UUID();
	
	//! set project uuid
	tbool Set_Branch_UUID(std::string sBranch_UUID){msBranch_UUID = msBranch_UUID;};
	
	//! clear project uuid
	tbool Clear_Branch_UUID(){ msBranch_UUID.erase(0, msBranch_UUID.length());};
	
	//! get the project uuid
	std::string Get_Branch_UUID();
	
	
	
	
	
	//! generates a UUID
	std::string Get_UUID();

	
private:

	
	virtual void Open_Project_Edit_Page_On_Koblo();
	
	//! project name
	std::string msProject_Name;
	
	//! project uuid
	std::string msProject_UUID;
	
	//! project uuid
	std::string msBranch_UUID;
	
	//! project name
	std::string msProject_Path;
	
	//! project description
	std::string msProjectDescription;
	
	//! branch name
	std::string msBranchName;
	
	//! branch description
	std::string msBranchDescription;
	
	//! if true the users sign in informations is stored in the pref file
	tbool mbRemember_Me;
	
	//! user name
	std::string msUser_Name;
	
	//! password description
	std::string msPassword;
		
		
};

