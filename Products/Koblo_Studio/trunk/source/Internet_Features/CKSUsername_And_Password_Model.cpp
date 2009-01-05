
#include "KSOS.h"


CKSUsername_And_Password_Model::CKSUsername_And_Password_Model()
{
	

	
}

CKSUsername_And_Password_Model::~CKSUsername_And_Password_Model()
{
	
}

tbool CKSUsername_And_Password_Model::Computer_Is_Online()
{
	// sign in to koblo.com 
	return true;	
}

tbool CKSUsername_And_Password_Model::Get_User_Name_And_Password()
{
	// load username and password from memory 
	std::string sUser_Name	= gpApplication->Get_User_Name();
	std::string sPassword	= gpApplication->Get_Password();
//	tbool bRemember_Me		= gpApplication->Get_Remember_Me();
	
	
	// Check if we need to load it from pref file on disk
	tbool bLoad_From_Disk	=	false;
	
	if(stricmp("", sUser_Name.c_str()) == 0)
		bLoad_From_Disk = true;
	else if(stricmp("", sPassword.c_str()) == 0)
		bLoad_From_Disk = true;
	// if username and passwore already stored in memory return true
	if(!bLoad_From_Disk)
		return true;
	
	
	// load from disk
	return Load_User_Name_And_Password();
	
}

tbool CKSUsername_And_Password_Model::Load_User_Name_And_Password()
{
	

	//!!! TO DO load from pref file here 

	/*
	 sUser_Name =
	 sPassword	= 
	 bRemember_Me = 

	// copy to internal data structure
	gpApplication->Set_Remember_Me(bRemember_Me);
	gpApplication->Set_User_Name(sUser_Name);
	gpApplication->Set_Password(sPassword);
	 */
	
	// dummy code unable to load from pref file
	return false;
	
}

void CKSUsername_And_Password_Model::Save_User_Name_And_Password()
{
	
	
	if(gpApplication->Get_Remember_Me()){
		// save username and password to disk
		std::string sUser_Name	= gpApplication->Get_User_Name();
		std::string sPassword	= gpApplication->Get_Password();
		
		//!!! TO DO 
		// save username, password and remember_me to pref file here
	}
	else
		Remove_Username_And_Password();
}

void CKSUsername_And_Password_Model::Clear_Username_And_Password()
{
	
	// delete internal data structure
	gpApplication->Set_Remember_Me(false);
	gpApplication->Set_User_Name("");
	gpApplication->Set_Password("");
	// send message to sign in dialog 
	CBasePane::SMsg Msg(MsgSign_Out);
	gpApplication->Send_Msg_To_All_Panes(&Msg);
	// remove from pref file
	Remove_Username_And_Password();
	
	
}

void CKSUsername_And_Password_Model::Remove_Username_And_Password()
{
	//!!! TO DO 
	// remove username and password form pref file here 
}

void CKSUsername_And_Password_Model::Open_Username_And_Password_Dialog()
{
	Get_User_Name_And_Password();
	
	// check if the window is open
	tbool bWindow_Is_Open = (gpApplication->GetGlobalParm(giSign_In_Window, giSectionGUI) != 0);
	if(!bWindow_Is_Open){
		// open window true internal data structure
		gpApplication->SetGlobalParm(giParamID_Show_Sign_In_Window,true, giSectionGUI);
	}
	else{
		// bring window to front
		gpApplication->GetModule()->GetHost()->ActivateWindow(giSign_In_Window);
	}
	
} 

void CKSUsername_And_Password_Model::On_Username_And_Password_OK()
{
	Save_User_Name_And_Password();
	// Called when the user presses "Ok" in the sign in dialog
	// if this function was called because the user wanted to upload a project and was not signed in
	if(gpApplication->mbUpload_Project && gpApplication->Create_Project_Handshake()){
		gpApplication->mbUpload_Project = false;
		gpApplication->Upload_Project();
	}
	
}
