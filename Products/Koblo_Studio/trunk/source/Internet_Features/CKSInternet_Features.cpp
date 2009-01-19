
#include "KSOS.h"



CKSInternet_Features::CKSInternet_Features( ): 
CKSXML_Create_Project(),
CKSXML_Write_Project(),
CKSXML_Read_Project(),
CKSUsername_And_Password_Model(),
mbUpload_Project(false)
{
	//!!!TO DO remove this dummy code
	msProject_Name = "flying pink elephants with umbrellas";
	msProject_Path	=	"";
	mpProject_UUID	= new(CKSUUID);
	mpBranch_UUID	= new(CKSUUID);
	

}

CKSInternet_Features::~CKSInternet_Features()
{
	
}





void CKSInternet_Features::On_Menu_Download_Project()
{
	tbool bNoProjectID = (gpApplication->GetGlobalParm(giParamID_Project_ID, giSectionGlobal) == -1);
	// if no project id is set
	if(bNoProjectID){
		
		// Open the project id dialog
		tbool bTest = (gpApplication->GetGlobalParm(giParamID_Show_Projec_ID_Window, giSectionGUI) != 0);
		if(!bTest){
			gpApplication->SetGlobalParm(giParamID_Show_Projec_ID_Window,true, giSectionGUI);
		}
		else
			gpApplication->GetModule()->GetHost()->ActivateWindow(giProject_ID_Window);
	}
	else{
		gpApplication->LoadSaveErrDlg("You cant overwrite this project! Try update project instead");
	}
} 


void CKSInternet_Features::On_Menu_Update_Project()
{

//	Sign_In();
	
	tint iProjectID = gpApplication->GetGlobalParm(giParamID_Project_ID, giSectionGlobal);
	// if no project id is set
	if(iProjectID == -1){
		gpApplication->LoadSaveErrDlg("No Project ID selected! Check if is project online?");
	}
	else{
		Update_Project(iProjectID);
	}
	
}

void CKSInternet_Features::On_Menu_Upload_Project()
{
	mbUpload_Project = true;
	
	if(Get_User_Name_And_Password()){
		
	}
	else{
	   Open_Username_And_Password_Dialog();
	}
}

void CKSInternet_Features::On_Menu_Commit_Project()
{
	Upload_Project();
}


void CKSInternet_Features::Download_Project(tint32 iProjectID)
{
	
	// read the project from koblo.com
	gpApplication->Read_Project_XML_To_DOM(iProjectID);
	// load the project
	Load_Project( iProjectID);
	
	
}

void CKSInternet_Features::Update_Project(tint32 iProjectID)
{
	// read the project from koblo.com
	gpApplication->Read_Project_XML_To_DOM(iProjectID);
	
	// check version
	if(gpApplication->Check_For_Newer_Revision(iProjectID))
	{
		// load the project
		Load_Project( iProjectID);
	}
	else{
		
		gpApplication->LoadSaveErrDlg("No newer revision on server!");
	}
	
}

void CKSInternet_Features::Load_Project(tint32 iProjectID)
{
	// clean the project
	gpApplication->CleanProject(0);
	
	// reset the project
	gpApplication->Reset_Project();
	
	// set all internal parameters fro DOM 
	gpApplication->CKSXML_Parse_DOM_To_Preset();
}



void CKSInternet_Features::Upload_Project()
{
	tint32 iProject_ID = gpApplication->GetGlobalParm(giParamID_Project_ID, giSectionGlobal);	
	gpApplication->Upload_Project_As_XML_File_To_Koblo( iProject_ID);
}

void CKSInternet_Features::Open_Project_Edit_Page_On_Koblo()
{
	tint32 iProjectID = gpApplication->GetGlobalParm(giParamID_Project_ID, giSectionGlobal);
		
	char psz[128];
	sprintf(psz, "/http://koblo.com/projects/%d/edit", iProjectID);
	OpenBrowser(psz);
	
	
}

tbool CKSInternet_Features::Project_Has_Changed()
{
	// if no file on disk return true to force the save dialog to open
	// 
	std::string sInternal	=	Get_Internal_Data_As_XML();
	std::string sOn_Disk	=	"";
	
	return sInternal.compare(sOn_Disk) != 0 ; 

	
}

std::string CKSInternet_Features::Get_Project_UUID()
{ 
	return mpProject_UUID->Get_UUID();
}

void CKSInternet_Features::Set_Project_UUID()
{ 
	mpProject_UUID->Set_UUID();
}

void CKSInternet_Features::Set_Project_UUID(std::string  sUUID )
{ 
	mpProject_UUID->Set_UUID(sUUID);
}

std::string CKSInternet_Features::Get_Branch_UUID()
{ 
	return mpBranch_UUID->Get_UUID();
}

void CKSInternet_Features::Set_Branch_UUID()
{ 
	mpBranch_UUID->Set_UUID();
}

void CKSInternet_Features::Set_Branch_UUID(std::string  sUUID )
{ 
	mpBranch_UUID->Set_UUID(sUUID);
}













