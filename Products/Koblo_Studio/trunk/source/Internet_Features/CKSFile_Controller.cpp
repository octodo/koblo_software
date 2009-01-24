


#include "KSOS.h"

CKSFile_Controller::CKSFile_Controller()
{

	
	mpMIDI_Sequencer = new(CMIDI_Sequencer);
	
	char pszInFile[1024];
	char pszOutFile[1024];
	
	
	mpMIDI_Sequencer->Init( pszInFile, pszOutFile);
}

CKSFile_Controller::~CKSFile_Controller()
{
	
}

tbool CKSFile_Controller::New_Project()
{
	// take care of not owerwriting a project withour  warning
	if(Leave_Old_Project() == false) return false;

	// get default folder
	tchar pszDefault_Folder[1024];
	gpApplication->GetDefaultProjectFolder(pszDefault_Folder);
	
	// open new project dialog
	tchar pszProject_Folder[1024];
	CAutoDelete<ge::ISaveAsDialog> pDlg(ge::ISaveAsDialog::Create());
	pDlg->DoDialog(pszProject_Folder, pszDefault_Folder, "", "", "New Project");
	
	// user canceled operation
	if (pszProject_Folder[0] == 0)  return false;
	
	// clean project
	gpApplication->CleanProject(giDefault_Number_Of_Tracks);
	
	// updaate path's and names for files and folders
	Update_Project_Name(pszProject_Folder);
	
	// create new folders for the project
	return Create_Folders();


	
}

tbool CKSFile_Controller::Leave_Old_Project()
{
	
	if (gpApplication->Project_Folder().size()) {
		
		// Warning dialog
		tchar pszMsg[1024];
		sprintf( pszMsg, "Save changes to '%s before closing'\n", Get_Project_Name().c_str() );
		ge::IWindow::EMsgBoxReturn eRet;
		eRet = ge::IWindow::ShowMessageBox(pszMsg, "!Warning", ge::IWindow::MsgBoxYesNoCancel);
		
		// return value
		switch (eRet) {
				
			case ge::IWindow::MsgBoxRetYes:{
				Save_Project();
				return true;
			}
			case ge::IWindow::MsgBoxRetNo:
				return true;
				
			case ge::IWindow::MsgBoxRetCancel: 
				return false; 
				
		}
	}
	return true;
}

tbool CKSFile_Controller::Save_Project()
{
	gpApplication->Save_Project_As_XML_File_To_Disk();
	return true;
	
}

tbool CKSFile_Controller::Save_As()
{
	// store copy from locations
	msCopy_From_Project_Folder	=	gpApplication->Project_Folder();
	msCopy_From_Project_Name	=	msProject_Name;
	
	// default folder
	tchar pszDefault_Folder[1024];
	gpApplication->GetDefaultProjectFolder(pszDefault_Folder);
	
	// open new project dialog
	tchar pszProject_Folder[1024];
	std::string sName_Copy	=	gpApplication->Get_Project_Name();
	sName_Copy				+= " Copy";
	
	CAutoDelete<ge::ISaveAsDialog> pDlg(ge::ISaveAsDialog::Create());
	pDlg->DoDialog(pszProject_Folder, pszDefault_Folder, "", "", sName_Copy.c_str());
	
	// user canceled operation
	if (pszProject_Folder[0] == 0)  return false;
	
	// update project name and path's
	Update_Project_Name(pszProject_Folder);

	// create folders for audio midi etc...
	Create_Folders();
	
	// Save settings in project file
	gpApplication->Save_Project_As_XML_File_To_Disk();
	
	// copy samples and files from old project in to the new project folder
	Copy_Project();
	
	return true;

}




tbool CKSFile_Controller::Copy_Project()
{
	if(gpApplication->Project_Is_Imported()) {
		
		Copy_KSProject_Waves();
	}
	else{
		
		Copy_Samples();
	}
		
	return true;
	
}
tbool CKSFile_Controller::Copy_KSProject_Waves()
{
	CAutoDelete<IFileSearch> pSearch(IFileSearch::Create());
	
	
	// Old project folder
	std::string sCopy_From_Folder = gpApplication->GetProjDir();
	sCopy_From_Folder += "Contents:Audio:Clips:Decompressed:";
	
	
	std::string sCopy_To_Folder = gpApplication->Project_Folder() + ":Wave Files:";
	pSearch->Init2((sCopy_From_Folder + "*.wav").c_str());
	
	
	tchar pszFile_Name[1024];
	tbool bDir;
	
	
	while (pSearch->GetNext(pszFile_Name, bDir)) {
		
		std::string sFile_Name = pszFile_Name;
		IFile::CopyFile(sCopy_To_Folder.c_str(), sCopy_From_Folder.c_str(), sFile_Name.c_str());
		
	}
	
	return true;
}


tbool CKSFile_Controller::Create_Project_Folder()
{
	IFile::CreateDirectory(gpApplication->Project_Folder().c_str());
	// make check here
	return true;
}

//! create a new sample folder inside the project folder
tbool CKSFile_Controller::Create_Audio_Folder()
{
	std::string sFolder = gpApplication->Project_Folder() + ": Wave Files";
	IFile::CreateDirectory(sFolder.c_str());
	// make check here
	return true;
}

//! create a new midi folder inside the project folder
tbool CKSFile_Controller::Create_Midi_Folder()
{
	std::string sFolder = gpApplication->Project_Folder() + ": MIDI FIles";
	IFile::CreateDirectory(gpApplication->Project_Folder().c_str());
	// make check here
	return true;
}

//! create a new plug-in data folder inside the project folder
tbool CKSFile_Controller::Create_Plugin_Data_Folder()
{
	std::string sFolder = gpApplication->Project_Folder() + ": Plug-In's Settings";
	IFile::CreateDirectory(sFolder.c_str());
	// make check here
	return true;
}

//! create a new download folder inside the project folder
tbool CKSFile_Controller::Create_Download_Folder()
{
	std::string sFolder = gpApplication->Project_Folder() + ": Download";
	IFile::CreateDirectory(sFolder.c_str());
	// make check here
	return true;
}

//! create a new uplaod folder inside the project folder
tbool CKSFile_Controller::Create_Upload_Folder()
{
	std::string sFolder = gpApplication->Project_Folder() + ": Upload";
	IFile::CreateDirectory(sFolder.c_str());
	// make check here
	return true;
}

//! create a new uplaod folder inside the project folder
tbool CKSFile_Controller::Create_Wave_Picts_Folder()
{
	std::string sFolder = gpApplication->Project_Folder() + ": Wave Picts";
	IFile::CreateDirectory(sFolder.c_str());
	// make check here
	return true;
}



//! create a new project file
tbool CKSFile_Controller::Create_Project_File()
{
	std::string sProject_Name	= gpApplication->Get_Project_Name();
	std::string sProject_Folder = gpApplication->Project_Folder();
	std::string sProject		=  sProject_Folder + ":" + sProject_Name + ".xml";
	
	
	//"Macintosh HD:User:maxgronlund:MusicMKoblo Studio:XML:jord:ford.xml"
	
	CAutoDelete<IFile> pfProject_File(IFile::Create());
	pfProject_File->Open(sProject.c_str(), IFile::FileCreate);
	
	//pPeakFileL->Write((const tchar*)pfPeak, iPeakSize * sizeof(tfloat32));
	
	
	if (pfProject_File) {
		return true;
	}
	return false;
}

tbool CKSFile_Controller::Copy_Samples()
{
	CAutoDelete<IFileSearch> pSearch(IFileSearch::Create());
	
	
	// copy from project folder
	std::string sCopy_From_Folder = msCopy_From_Project_Folder + ":Wave Files:";
	pSearch->Init2((sCopy_From_Folder + "*.wav").c_str());
	
	
	std::string sCopy_To_Folder = gpApplication->Project_Folder() + ":Wave Files:";
	
	
	
	tchar pszFile_Name[1024];
	tbool bDir;
	while (pSearch->GetNext(pszFile_Name, bDir)) {
		
		std::string sFile_Name = pszFile_Name;
		IFile::CopyFile(sCopy_To_Folder.c_str(), sCopy_From_Folder.c_str(), sFile_Name.c_str());
		
	}
	
	return true;
}

tbool CKSFile_Controller::Create_Folders()
{

	// create a new project folder
	if(Create_Project_Folder() == false ) return false;
	
	// create a new project folder
	if(Create_Audio_Folder() == false ) return false;
	
	// create a midi project folder
	if(Create_Midi_Folder() == false ) return false;
	
	// create a plug-in data project folder
	if(Create_Plugin_Data_Folder() == false ) return false;
	
	// create a new download folder
	if(Create_Download_Folder() == false ) return false;
	
	// create a new upload folder
	if(Create_Upload_Folder() == false ) return false;
	
	// create a new upload folder
	if(Create_Wave_Picts_Folder() == false ) return false;

	
	// create a new project file
	if( Create_Project_File() == false ) return false;

	return true;

}

void CKSFile_Controller::Update_Project_Name(std::string sNew_Name)
{
	// set project folder
	gpApplication->Project_Folder(sNew_Name);
	
	// set project name
	std::string sProject_Name = sNew_Name;
	tint iPos = sProject_Name.find_last_of(':');
	sProject_Name = sProject_Name.substr(iPos + 1, sProject_Name.size());
	gpApplication->Set_Project_Name(sProject_Name);
	

	
}








