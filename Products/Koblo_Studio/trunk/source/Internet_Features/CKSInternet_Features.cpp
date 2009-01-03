
#include "KSOS.h"



CKSInternet_Features::CKSInternet_Features(CKSPlugIn* pKSPlugIn ): 
mpKSPlugIn(pKSPlugIn),
CKSXML_Create_Project(pKSPlugIn),
CKSXML_Write_Project(pKSPlugIn),
CKSXML_Read_Project(pKSPlugIn),
CKSXML_Sign_In(pKSPlugIn),
mbUpload_Project(false)
{
	
}

CKSInternet_Features::~CKSInternet_Features()
{
	
}

void CKSInternet_Features::Open_Sign_In_Dialog()
{
	Get_User_Name_And_Password();
	
	
	tbool bTest = (mpKSPlugIn->GetGlobalParm(giSign_In_Window, giSectionGUI) != 0);
	if(!bTest){
		
		mpKSPlugIn->SetGlobalParm(giParamID_Show_Sign_In_Window,true, giSectionGUI);
	}
	else
		mpKSPlugIn->GetModule()->GetHost()->ActivateWindow(giSign_In_Window);
	
} 

void CKSInternet_Features::On_Menu_Sign_Out()
{
	Sign_Out();
}

void CKSInternet_Features::Sign_In()
{
	// Called when the user presses "Ok" in the sign in dialog
	// if we called because the user wanted to upload a project and wasent signed in
	if(mbUpload_Project && mpKSPlugIn->Create_Project_Handshake()){
		mbUpload_Project = false;
		Commit_Project();
	}
		
}


void CKSInternet_Features::On_Menu_Download_Project()
{
	tbool bNoProjectID = (mpKSPlugIn->GetGlobalParm(giParamID_Project_ID, giSectionGlobal) == -1);
	// if no project id is set
	if(bNoProjectID){
		
		// Open the project id dialog
		tbool bTest = (mpKSPlugIn->GetGlobalParm(giParamID_Show_Projec_ID_Window, giSectionGUI) != 0);
		if(!bTest){
			mpKSPlugIn->SetGlobalParm(giParamID_Show_Projec_ID_Window,true, giSectionGUI);
		}
		else
			mpKSPlugIn->GetModule()->GetHost()->ActivateWindow(giProject_ID_Window);
	}
	else{
		mpKSPlugIn->LoadSaveErrDlg("You cant overwrite this project! Try update project instead");
	}
} 


void CKSInternet_Features::On_Menu_Update_Project()
{

//	Sign_In();
	
	tint iProjectID = mpKSPlugIn->GetGlobalParm(giParamID_Project_ID, giSectionGlobal);
	// if no project id is set
	if(iProjectID == -1){
		mpKSPlugIn->LoadSaveErrDlg("No Project ID selected! Check if is project online?");
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
	   Open_Sign_In_Dialog();
	}
}

void CKSInternet_Features::On_Menu_Commit_Project()
{
	Commit_Project();
}


void CKSInternet_Features::Download_Project(tint32 iProjectID)
{
	
	// read the project from koblo.com
	mpKSPlugIn->Read_Project_XML_To_DOM(iProjectID);
	// load the project
	Load_Project( iProjectID);
	
	
}

void CKSInternet_Features::Update_Project(tint32 iProjectID)
{
	// read the project from koblo.com
	mpKSPlugIn->Read_Project_XML_To_DOM(iProjectID);
	
	// check version
	if(mpKSPlugIn->Check_For_Newer_Revision(iProjectID))
	{
		// load the project
		Load_Project( iProjectID);
	}
	else{
		
		mpKSPlugIn->LoadSaveErrDlg("No newer revision on server!");
	}
	
}

void CKSInternet_Features::Load_Project(tint32 iProjectID)
{
	// clean the project
	mpKSPlugIn->CleanProject(0);
	
	// reset the project
	mpKSPlugIn->Reset_Project();
	
	// set all internal parameters fro DOM 
	mpKSPlugIn->CKSXML_Parse_DOM_To_Preset();
}



void CKSInternet_Features::Commit_Project()
{
	tint32 iProject_ID = mpKSPlugIn->GetGlobalParm(giParamID_Project_ID, giSectionGlobal);	
	mpKSPlugIn->Upload_Project_As_XML_File_To_Koblo( iProject_ID);
}

void CKSInternet_Features::Open_Project_Edit_Page_On_Koblo()
{
	tint32 iProjectID = mpKSPlugIn->GetGlobalParm(giParamID_Project_ID, giSectionGlobal);
		
	char psz[128];
	sprintf(psz, "/http://koblo.com/projects/%d/edit", iProjectID);
	OpenBrowser(psz);
	
	
}










