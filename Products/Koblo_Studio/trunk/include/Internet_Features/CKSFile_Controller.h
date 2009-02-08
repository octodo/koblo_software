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

class CMIDI_Sequencer;
class CSample_Data;
class CTake_Data;



class CKSFile_Controller
{
	
public:
	//! Constructor
	CKSFile_Controller();

	//! Destructor
	virtual ~CKSFile_Controller();
	
	//! open project
	virtual tbool Open_Project();

	/*! called from the "File:New" menu
	 \ returns an ESaveMsg
	 */
	tint32 New_Project();
	
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
	
	
	
	//! get project folder
	std::string Project_Folder(){ return msProject_Folder;};
	//! set project name
	void Project_Folder(std::string sFolder){  msProject_Folder = sFolder;};
	
	std::string Project_Name(){ return msProject_Name;};
	//! set project name
	void Project_Name(std::string sName){  msProject_Name = sName;};
	
	//! update project name and folder path
	//!!! TO DO make private when old fileformat no longer is supported
	void Update_Project_Name(std::string sNew_Name);
/*	
	// Check if the file path points to an audio file
	tbool Is_A_Audio_File(std::string sFile);
	
	// check if extencion is reconized
	tbool Check_Extencion(std::string sFile);
*/	
	std::string  Get_Extencion(std::string sFile);
	
	//! opens a audio file at the given path and check if it's readable 
	tbool Readable_Audio(std::string sFile);
	
	//! opens a audio file at the given path and check if it's readable 
	tbool Is_A_File(std::string sFile_Path);
	
	// get wave file folder
	std::string Wave_File_Folder(){ return msProject_Folder + "Wave Files:";};
	
	// get ogg file folder
	std::string OGG_File_Folder(){ return msProject_Folder + "OGG Files:";};
	
	// get mp3 file folder
	std::string MP3_File_Folder(){ return msProject_Folder + "MP3 Files:";};

	
	// get ogg file folder
	std::string Pict_File_Folder(){ return msProject_Folder + "Wave Picts:";};
	
	// get plug-in settings folder
	std::string Plugin_Settings_Folder(){ return msProject_Folder + "Plug-In's Settings:";};
	
	
	
	
	
	//! itterate all samples in the mSample_Data_List
	virtual void Prepare_Sampels_For_Upload();
	
	//! check one sample from the mSample_Data_List
	virtual void Prepare_Sampel_For_Upload(CSample_Data* pSample_Data);
	
	/*! find the oog and mp3 file in the "OGG Files" and the "MP3 Files" folders 
	\ if there is no ogg / mp3 file the name of the file is stored in the mOGG_Compress_Que
	 */
	virtual void Prepare_Take_For_Upload(CTake_Data* Take_Data);
	
	//! secure all mp3 files are in the ogg file folder
	virtual void Compress_OGG_File();
	
	//! secure all mp3 files are in the ogg file folder
	virtual void Compress_MP3_File();
	
	tbool Validate_Files_For_Upload();
	
	// validate that a take acutually is readable auido file/ files
	tbool Validate_Take(CTake_Data* Take_Data);

	
private:	
	
	//! project folder
	std::string msProject_Folder;
	
	//! project name
	std::string msProject_Name;
	
	//! create a new project folder
	tbool Create_Project_Folder();
	
	//! create a new sample folder inside the project folder
	tbool Create_Audio_Folders();
	
	//! create a new midi folder inside the project folder
	tbool Create_Midi_Folder();
	
	//! create a new plug-in data folder inside the project folder
	tbool Create_Plugin_Data_Folder();
	
	//! create a new download folder inside the project folder
	tbool Create_OGG_Folder();
	
	//! create a new uplaod folder inside the project folder
	tbool Create_MP3_Folder();
	
	//! create a new "wave pict" folder inside the project folder
	tbool Create_Wave_Picts_Folder();
	
	//! create a new "plug-in settings" folder inside the project folder
	tbool Create_Plugin_Settings_Folder();
	
	//! create a new project file
	tbool Create_Project_File();
	
	// copy samples to project folder
	tbool Copy_Samples(); 
	
	//! create folder for project files
	tbool Create_Folders();
	
	//! takes care of not owerwriting a project withour a warning
	tint32 Save_Before_Close();
	
	//! project folder to copy from during save as
	std::string msCopy_From_Project_Folder;
	
	//! project name to copy from during save as
	std::string msCopy_From_Project_Name;
	
	// copy files from one project to another one 
	tbool Copy_Project();
	
	// copy waves from old KSProject file
	tbool Copy_KSProject_Waves();
	
	CMIDI_Sequencer* mpMIDI_Sequencer;
	 
	 
	// list of takes files to compress as OGG
	std::list<CTake_Data*> mOGG_Compress_Que;
	
	// list of takes files to compress as mp3
	std::list<CTake_Data*> mMp3_Compress_Que;
	
	//! missing files
	tuint32 muiMissing_Files;
	
	
	
	
};






