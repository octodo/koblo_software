
#include "KSOS.h"



CKSInternet_Features::CKSInternet_Features( ): 
CKSXML_Create_Project(),
CKSXML_Write_Project(),
CKSXML_Read_Project(),
CKSUsername_And_Password_Model(),
mbUpload_Project(false)
{

	mpProject_UUID = new(CKSUUID);
	mpBranch_UUID = new(CKSUUID);
	mpCommit_UUID = new CKSUUID;
	mpInsert_File_UUID  = new CKSUUID;

}

CKSInternet_Features::~CKSInternet_Features()
{
	
}

void CKSInternet_Features::On_Menu_Download_Project()
{
	
	// if project is open
	if(gpApplication->Project_Name().length() != 0 ) {

		//  project has a uuid so i has been uploaded
		if (gpApplication->Project_UUID_Is_Set() ) 
		{
			// ask for overwriting project
			if( gpApplication->Overwrite_Project() ){
				// download latest commit in branch
				gpApplication->Get_Latest_Commit_In_Branch( gpApplication->Get_Branch_UUID() );
			}

		}
		else
		{
			// close project
			if( gpApplication->Close_Project() != giUser_Canceld_Save ) {
				Ask_For_Commit_UUID();
			}	
		}
	}
	else {
		// no project is open ask for a project id
		Ask_For_Commit_UUID();
	}
		
} 

void CKSInternet_Features::Ask_For_Commit_UUID()
{
	// Open the project id dialog
	tbool bTest = (gpApplication->GetGlobalParm(giParamID_Show_Projec_ID_Window, giSectionGUI) != 0);
	if(!bTest){
		gpApplication->SetGlobalParm(giParamID_Show_Projec_ID_Window,true, giSectionGUI);
	}
	else
		gpApplication->GetModule()->GetHost()->ActivateWindow(giProject_ID_Window);
}


void CKSInternet_Features::On_Menu_Upload_Project()
{
	
	
	
	if(Get_User_Name_And_Password()){
		
		gpApplication->Upload_Project();
	}
	else{
		// after pressing OK the project will be uploaded
		mbUpload_Project = true;
		Open_Username_And_Password_Dialog();
	}
	
	
}

void CKSInternet_Features::On_Menu_Upload_New_Branch()
{
	
	if(gpApplication->Project_Name().length() != 0 ) {
		
		std::string sLink= "http://koblo.com/projects/";
		sLink += gpApplication->Project_UUID();
		OpenBrowser(sLink.c_str() );
	}
	else {

		// Warning dialog
		tchar pszMsg[1024];
		sprintf( pszMsg, "Please open/ create a project first'\n" );
		ge::IWindow::EMsgBoxReturn eRet;
		eRet = ge::IWindow::ShowMessageBox(pszMsg, "!No Project loaded", ge::IWindow::MsgBoxOK);
			

		
	}
	
	
}



void CKSInternet_Features::Download_Project(tint32 iProjectID)
{
	// read the project from koblo.com
//	gpApplication->Read_Latest_Revision_From_Koblo(iProjectID);

	
}

void CKSInternet_Features::Update_Project(tint32 iProjectID)
{
	// read the project from koblo.com
//	gpApplication->Read_Latest_Revision_From_Koblo(iProjectID);

	/*
	// check version
	if(gpApplication->Check_For_Newer_Revision(iProjectID))
	{
		// load the project
		Load_Project( iProjectID);
	}
	else{
		
		gpApplication->LoadSaveErrDlg("No newer revision on server!");
	}
	 */
	
}

void CKSInternet_Features::Load_Project(tint32 iProjectID)
{
	
	// reset the project
	gpApplication->Reset_Project();
	
	// set all internal parameters fro DOM 
//	gpApplication->CKSXML_Parse_DOM_To_DAW();
}


/*
void CKSInternet_Features::CKSUsername_And_Password_Controller()
{
	tint32 iProject_ID = gpApplication->GetGlobalParm(giParamID_Project_ID, giSectionGlobal);	
	gpApplication->Upload_Project_As_XML_File_To_Koblo( iProject_ID);
}
*/
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
/*
void CKSInternet_Features::SetProjectName( const std::string& sName)
{ 
	msProjectName = sName;
	gpApplication->Project_Name(sName);
}
*/
void CKSInternet_Features::Set_Branch_Name( const std::string& sName)
{
	CAutoLock Lock(mMutex_ForUpAndDownloadTask_CallBack);
	msBranchName = sName;
}
	
std::string CKSInternet_Features::Get_Branch_Name()
{
	CAutoLock Lock(mMutex_ForUpAndDownloadTask_CallBack);
	return msBranchName;
}
	
void CKSInternet_Features::Branch_Revision( tuint32 uiBranch_Revision )
{
	CAutoLock Lock(mMutex_ForUpAndDownloadTask_CallBack);
	muiBranch_Revision = uiBranch_Revision;
}

void CKSInternet_Features::Branch_Revision( std::string sBranch_Revision )
{
	CAutoLock Lock(mMutex_ForUpAndDownloadTask_CallBack);
	gpApplication->Pass_Branch_Revision(sBranch_Revision);
	
}
	
tuint32 CKSInternet_Features::Branch_Revision( )
{
	CAutoLock Lock(mMutex_ForUpAndDownloadTask_CallBack);
	return muiBranch_Revision;
}


void CKSInternet_Features::Set_User_Name( const std::string& sName)
{
	msUser_Name = sName;
	strncpy(gpDSPEngine->mPrefs_3.mpszUser, sName.c_str(), 511);
	//gpDSPEngine->SavePrefs();
}
	

void CKSInternet_Features::Set_Password( const std::string& sPassword)
{
	msPassword = sPassword;
	strncpy(gpDSPEngine->mPrefs_3.mpszPass, sPassword.c_str(), 511);
	//gpDSPEngine->SavePrefs();
}


void CKSInternet_Features::Set_Remember_Me(tbool bRemember_Me)
{
	mbRemember_Me = bRemember_Me;
	gpDSPEngine->mPrefs_3.mcbKeepInfo = (tchar)bRemember_Me;
	//gpDSPEngine->SavePrefs();
}

//! read permission
tbool CKSInternet_Features::Read_Permission(std::string sXML_Hand_Shake)
{
	return true;
}

//! write permission
tbool CKSInternet_Features::Write_Permission(std::string sXML_Hand_Shake)
{
	return true;
}









