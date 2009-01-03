
#include "KSOS.h"


CKSXML_Sign_In::CKSXML_Sign_In(CKSPlugIn* pKSPlugIn):
mpKSPlugIn(pKSPlugIn)
{
	

	
}

CKSXML_Sign_In::~CKSXML_Sign_In()
{
	
}

tbool CKSXML_Sign_In::Sign_In_Handshake()
{
	// sign in to koblo.com 
	return true;	
}



tbool CKSXML_Sign_In::Get_User_Name_And_Password()
{
	// load username and password memory 
	std::string sUser_Name	= mpKSPlugIn->Get_User_Name();
	std::string sPassword	= mpKSPlugIn->Get_Password();
	tbool bRemember_Me		= mpKSPlugIn->Get_Remember_Me();
	
	
	// Check if we need to load it from pref file from disk
	tbool bLoad_From_Disk	=	false;
	
	if(stricmp("", sUser_Name.c_str()) == 0)
		bLoad_From_Disk = true;
	else if(stricmp("", sPassword.c_str()) == 0)
		bLoad_From_Disk = true;
	// if username and passwore already loaded return true
	if(!bLoad_From_Disk)
		return true;
	
	
	// dummy code unable to load from disk
	return Load_User_Name_And_Password();
	
}

tbool CKSXML_Sign_In::Load_User_Name_And_Password()
{
	

	//!!! TO DO make load from pref file here 

	/*
	 sUser_Name =
	 sPassword	= 
	 bRemember_Me = 

	// copy to internal data structure
	mpKSPlugIn->Set_Remember_Me(bRemember_Me);
	mpKSPlugIn->Set_User_Name(sUser_Name);
	mpKSPlugIn->Set_Password(sPassword);
	 */
	
	// dummy code unable to load from pref file
	return false;
	
}

void CKSXML_Sign_In::Save_User_Name_And_Password()
{
	
	
	if(mpKSPlugIn->Get_Remember_Me()){
		// save username and password to disk
		std::string sUser_Name	= mpKSPlugIn->Get_User_Name();
		std::string sPassword	= mpKSPlugIn->Get_Password();
		
		//!!! TO DO 
		// save username, password and remember_me to pref file here
	}
	else
		Remove_Username_And_Password();
}

void CKSXML_Sign_In::Sign_Out()
{
	
	// delete internal data structure
	mpKSPlugIn->Set_Remember_Me(false);
	mpKSPlugIn->Set_User_Name("");
	mpKSPlugIn->Set_Password("");
	// send message to sign in dialog 
	CBasePane::SMsg Msg(MsgSign_Out);
	mpKSPlugIn->Send_Msg_To_All_Panes(&Msg);
	// remove from pref file
	Remove_Username_And_Password();
	
	
}

void CKSXML_Sign_In::Remove_Username_And_Password()
{
	//!!! TO DO 
	// remove username and password form pref file here 
}
