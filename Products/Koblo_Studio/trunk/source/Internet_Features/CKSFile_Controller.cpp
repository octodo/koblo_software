


#include "KSOS.h"

CKSFile_Controller::CKSFile_Controller()
{

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
	
	if (gpApplication->Get_Project_Folder().size()) {
		
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
	msCopy_From_Project_Folder	=	msProject_Folder;
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
	Copy_Waves();
		
	return true;
	
}

tbool CKSFile_Controller::Copy_Waves()
{
	std::list<CSample_Data*> pSample_Data_List = gpApplication->Get_Sample_Data_List();
	std::list<CSample_Data*>::iterator  itSample_Data = pSample_Data_List.begin();
	
	
	
	for (; itSample_Data != pSample_Data_List.end(); itSample_Data++) {
		
		CSample_Data* pSample_Data	= (*itSample_Data);
		CTake_Data*	pTake_Data		=	pSample_Data->Get_Take_Data();
		
		std::string sName = (*itSample_Data)->Get_Name().c_str();
		
		switch(pTake_Data->iOriginalChannelMask)
		{
				// Left
			case 1:
				break;
				// right	
			case 2:
				break;
				// stereo
			case 3:
				break;
		}
	}
	//CopyFile(const tchar* pszPathNameDest, const tchar* pszPathNameSrc, const tchar* pszName);
	
	return true;
	
}


tbool CKSFile_Controller::Create_Project_Folder()
{
	IFile::CreateDirectory(gpApplication->Get_Project_Folder().c_str());
	// make check here
	return true;
}

//! create a new sample folder inside the project folder
tbool CKSFile_Controller::Create_Audio_Folder()
{
	std::string sFolder = gpApplication->Get_Project_Folder() + ": Wave Files";
	IFile::CreateDirectory(sFolder.c_str());
	// make check here
	return true;
}

//! create a new midi folder inside the project folder
tbool CKSFile_Controller::Create_Midi_Folder()
{
	std::string sFolder = gpApplication->Get_Project_Folder() + ": MIDI FIles";
	IFile::CreateDirectory(gpApplication->Get_Project_Folder().c_str());
	// make check here
	return true;
}

//! create a new plug-in data folder inside the project folder
tbool CKSFile_Controller::Create_Plugin_Data_Folder()
{
	std::string sFolder = gpApplication->Get_Project_Folder() + ": Plug-In's Settings";
	IFile::CreateDirectory(sFolder.c_str());
	// make check here
	return true;
}

//! create a new download folder inside the project folder
tbool CKSFile_Controller::Create_Download_Folder()
{
	std::string sFolder = gpApplication->Get_Project_Folder() + ": Download";
	IFile::CreateDirectory(sFolder.c_str());
	// make check here
	return true;
}

//! create a new uplaod folder inside the project folder
tbool CKSFile_Controller::Create_Upload_Folder()
{
	std::string sFolder = gpApplication->Get_Project_Folder() + ": Upload";
	IFile::CreateDirectory(sFolder.c_str());
	// make check here
	return true;
}



//! create a new project file
tbool CKSFile_Controller::Create_Project_File()
{
	std::string sProject_Name	= gpApplication->Get_Project_Name();
	std::string sProject_Folder = gpApplication->Get_Project_Folder();
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

tbool CKSFile_Controller::Copy_Samples(std::string sSource_Path, std::string sDestination_Path)
{
	return true;
}

tbool CKSFile_Controller::Create_Folders()
{

	// create a new project folder
	if(Create_Project_Folder() == false ) return false;
	
	// create a new project folder
	if(Create_Audio_Folder() == false ) return false;
	
	// create a new project folder
	if(Create_Midi_Folder() == false ) return false;
	
	// create a new project folder
	if(Create_Plugin_Data_Folder() == false ) return false;
	
	// create a new project folder
	if(Create_Download_Folder() == false ) return false;
	
	// create a new project folder
	if(Create_Upload_Folder() == false ) return false;
	
	
	
	
	// create a new project file
	if( Create_Project_File() == false ) return false;

	return true;

}

void CKSFile_Controller::Update_Project_Name(std::string sNew_Name)
{
	// set project folder
	gpApplication->Set_Project_Folder(sNew_Name);
	
	// set project name
	std::string sProject_Name = sNew_Name;
	tint iPos = sProject_Name.find_last_of(':');
	sProject_Name = sProject_Name.substr(iPos + 1, sProject_Name.size());
	gpApplication->Set_Project_Name(sProject_Name);
	
}








