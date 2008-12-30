
#include "KSOS.h"



CKSInternet_Features::CKSInternet_Features(CKSPlugIn * pKSPlugIn)
{
	
	mpKSPlugIn = pKSPlugIn;
	
}

CKSInternet_Features::~CKSInternet_Features()
{
	
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
		mpKSPlugIn->LoadSaveErrDlg("Project ID already set");
	}
} 


void CKSInternet_Features::On_Menu_Update_Project()
{
	tint iProjectID = mpKSPlugIn->GetGlobalParm(giParamID_Project_ID, giSectionGlobal);
	// if no project id is set
	if(iProjectID == -1){
		mpKSPlugIn->LoadSaveErrDlg("No Project ID is set");
	}
	else{
		Update_Project(iProjectID);
	}
	
}

void CKSInternet_Features::On_Menu_Upload_Project()
{
	if(mpKSPlugIn->Create_Project_On_Koblo())
		Commit_Project();
}

void CKSInternet_Features::On_Menu_Commit_Project()
{
	Commit_Project();
}




void CKSInternet_Features::Download_Project(tint32 iProjectID)
{
	
	// clean the project
	mpKSPlugIn->CleanProject(0);
	
	// update the project
	Update_Project(iProjectID);
	

	
}

void CKSInternet_Features::Update_Project(tint32 iProjectID)
{
	
	// clean the project		
	mpKSPlugIn->CleanProject(0);
	 
	// read the project from koblo.com
	mpKSPlugIn->Read_Project_XML(iProjectID);
	
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









