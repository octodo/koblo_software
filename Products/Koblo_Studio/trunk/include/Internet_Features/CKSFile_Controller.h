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

class CMIDI_Sequencer;

class CKSFile_Controller
{
	
public:
	//! Constructor
	CKSFile_Controller();

	//! Destructor
	virtual ~CKSFile_Controller();

	/*! called from the "File:New" menu
	 \ returns true if sucessfull
	 */
	tbool New_Project();
	
	/*! called from the "File:Save project" menu 
	 \ and from New_Project()
	 \ returns true if sucessfull
	 */
	tbool Save_Project();
	
	/*! called from the "File:Save As" menu 
	 \ and from New_Project()
	 \ returns true if sucessfull
	 */
	tbool Save_As();
	
	
	
	// get project folder
	std::string Get_Project_Folder(){ return msProject_Folder;};
	//! set project name
	void Set_Project_Folder(std::string sFolder){  msProject_Folder = sFolder;};
	
	std::string Get_Project_Name(){ return msProject_Name;};
	//! set project name
	void Set_Project_Name(std::string sName){  msProject_Name = sName;};
	
	//! update project name and folder path
	//!!! TO DO make private when old fileformat no longer is supported
	void Update_Project_Name(std::string sNew_Name);
	
private:	
	
	//! project folder
	std::string msProject_Folder;
	
	//! project name
	std::string msProject_Name;
	
	//! create a new project folder
	tbool Create_Project_Folder();
	
	//! create a new sample folder inside the project folder
	tbool Create_Audio_Folder();
	
	//! create a new midi folder inside the project folder
	tbool Create_Midi_Folder();
	
	//! create a new plug-in data folder inside the project folder
	tbool Create_Plugin_Data_Folder();
	
	//! create a new download folder inside the project folder
	tbool Create_Download_Folder();
	
	//! create a new uplaod folder inside the project folder
	tbool Create_Upload_Folder();
	
	//! create a new project file
	tbool Create_Project_File();
	
	// copy samples to project folder
	tbool Copy_Samples(std::string sSource_Path, std::string sDestination_Path); 
	
	//! create folder for project files
	tbool Create_Folders();
	
	//! takes care of not owerwriting a project withour a warning
	tbool Leave_Old_Project();
	
	//! project folder to copy from during save as
	std::string msCopy_From_Project_Folder;
	
	//! project name to copy from during save as
	std::string msCopy_From_Project_Name;
	
	// copy files from one project to another one 
	tbool Copy_Project();
	
	// copy sample Wave from one project to another one 
	tbool Copy_Waves();
	
	CMIDI_Sequencer* mpMIDI_Sequencer;
	
	
	
};






