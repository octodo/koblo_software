
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
	
	
	
	if(Get_User_Name_And_Password()){
		
		gpApplication->Upload_Project();
	}
	else{
		// after pressing OK the project will be uploaded
		mbUpload_Project = true;
		Open_Username_And_Password_Dialog();
	}
	
	

	
	// (lasse) very temporary code - forces linking of libcurl (only way to determine if it links)
#if (0)
	// (lasse) wawy wawy tempowawy code: upload to koblo.com
	
	// These must be set or it won't work
#define TEMPUSR "xxx@xxx.com"
#define TEMPPWD "xxx"
#define TEMPMP3 "_testhest.mp3"
#define TEMPXXX "Aja joepie jee.mp3"
	
	CAutoDelete<IFile> pfMp3(IFile::Create());
	CAutoDelete<IFile> pfExtra(IFile::Create());
#ifdef _WIN32
	tchar* pszMp3File = "C:\\" TEMPMP3;
	tchar* pszExtraFile = "C:\\" TEMPXXX;
#endif // _WIN32
#ifdef _Mac
	tchar* pszMp3File = "/" TEMPMP3;
	tchar* pszExtraFile = "/" TEMPXXX;
#endif // _Mac
	if (!pfMp3->Open(pszMp3File, IFile::FileRead))
		return;
	if (!pfExtra->Open(pszExtraFile, IFile::FileRead))
		return;
	
	tchar pszUUID_Proj[128];
	Gen_UUID(pszUUID_Proj, 128);
	tchar pszUUID_Branch[128];
	Gen_UUID(pszUUID_Branch, 128);
	tchar pszUUID_Sample[128];
	Gen_UUID(pszUUID_Sample, 128);
	tchar pszUUID_Take[128];
	Gen_UUID(pszUUID_Take, 128);
	
	tbool bProjOK = false;
	CAutoDelete<ine::IUploader> pUploader(ine::IUploader::Create());
	if (pUploader->Init("koblo.com", "/projects.xml", 80, TEMPUSR, TEMPPWD)) {
		pUploader->SetReplyMIMEType(ine::MIME_TYPE_XML);
		pUploader->AddParam("project[name]", "Short-lived Test 123 (lasse)", -1);
		pUploader->AddParam("project[description]", "Temporary project for test only", -1);
		pUploader->AddParam("project[license]", "by", -1);
		pUploader->AddParam("project[uuid]", pszUUID_Proj, -1);
		pUploader->AddParam("branch[uuid]", pszUUID_Branch, -1);
		CAutoDelete<IFileMemory> pfCreateReply(IFileMemory::Create());
		if (pfCreateReply->Open(IFile::FileCreate)) {
			// Call the Start() method without a reply file - this is for testing buffered reply
			if (pUploader->Start()) {
				tchar pszBuffer[1024];
				tint32 iPortionSize = 0;
				tbool bSuccess, bError;
				do {
					ITime::SleepMS(5);
					pUploader->GetReplyPortion(pszBuffer, 1024, &iPortionSize);
					if (iPortionSize > 0) {
						pfCreateReply->Write(pszBuffer, iPortionSize);
					}
					bSuccess = pUploader->IsDone();
					bError = pUploader->IsFailed();
				} while ((!bSuccess) && (!bError));
			}
			if (pUploader->IsDone()) {
				bProjOK = true;
			}
		}
		if (pUploader->IsFailed()) {
			tchar pszErr[1024];
			pUploader->GetError(pszErr, 1024);
			std::string sErr = pszErr;
			tuint64 uiSize = pfCreateReply->GetMemorySize();
			if (uiSize > 1) {
				sErr += "\n\n";
				std::string sExtra = std::string((tchar*)(pfCreateReply->GetMemoryPtr()), uiSize);
				sErr += sExtra;
			}
			ge::IWindow::ShowMessageBox(sErr.c_str(), "Uploader Error");
		}
	}
	
	if (bProjOK) {
		//CAutoDelete<ine::IUploader> pUploader(ine::IUploader::Create());
		std::string sPage = std::string("/projects/") + pszUUID_Proj;
		sPage += "/samples.xml";
		CAutoDelete<IFileMemory> pfUpReply(IFileMemory::Create());
		if (pfUpReply->Open(IFile::FileCreate)) {
			if (pUploader->Init("koblo.com", sPage.c_str(), 80, TEMPUSR, TEMPPWD)) {
				pUploader->SetReplyMIMEType(ine::MIME_TYPE_XML);
				pUploader->SetSpecificVerb(ine::VERB_POST);
				pUploader->AddParam("sample[uuid]", pszUUID_Sample, -1);
				pUploader->AddParam("sample[name]", "Short-lived Samle for Test 123 (lasse)", -1);
				pUploader->AddParam("take[uuid]", pszUUID_Take, -1);
				pUploader->AddParam("take[description]", "Temporary sample for test only", -1);
				pUploader->AddFileParam("mp3[uploaded_data]", pfMp3);
				pUploader->AddFileParam("audio[uploaded_data]", pfExtra);
				// Call Start(..) with a file - this for testing direct file reply
				if (pUploader->Start(pfUpReply)) {
					CAutoDelete<ge::IWaitCursor> pWaitCursor(ge::IWaitCursor::Create());
					
					tbool bSuccess, bError;
					do {
						ITime::SleepMS(5);
						bSuccess = pUploader->IsDone();
						bError = pUploader->IsFailed();
					} while ((!bSuccess) && (!bError));
				}
				if (pUploader->IsDone()) {
					bProjOK = true;
				}
			}
			if (pUploader->IsFailed()) {
				tchar pszErr[1024];
				pUploader->GetError(pszErr, 1024);
				std::string sErr = pszErr;
				tuint64 uiSize = pfUpReply->GetMemorySize();
				if (uiSize > 1) {
					sErr += "\n\n";
					std::string sExtra = std::string((tchar*)(pfUpReply->GetMemoryPtr()), uiSize);
					sErr += sExtra;
				}
				ge::IWindow::ShowMessageBox(sErr.c_str(), "Uploader Error");
			}
		}
	}
#endif 
}

void CKSInternet_Features::On_Menu_Commit_Project()
{
	gpApplication->Upload_Project();
}


void CKSInternet_Features::Download_Project(tint32 iProjectID)
{
	
	// read the project from koblo.com
	gpApplication->Read_Latest_Revision_From_Koblo(iProjectID);
	// load the project
	Load_Project( iProjectID);
	
	
}

void CKSInternet_Features::Update_Project(tint32 iProjectID)
{
	// read the project from koblo.com
	gpApplication->Read_Latest_Revision_From_Koblo(iProjectID);
	
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

void CKSInternet_Features::SetProjectName( const std::string& sName)
{ 
	msProjectName = sName;
	gpApplication->Project_Name(sName);
}








