
#include "KSOS.h"


CKSXML_Read_Project::CKSXML_Read_Project()
{
	
	mpDoc = new TiXmlDocument;
	
}

CKSXML_Read_Project::~CKSXML_Read_Project()
{
	delete mpDoc;
}

void CKSXML_Read_Project::Reset_Project()
{
	miTrack_ID	=	0;
	gpApplication->Set_Selected_Track(-1);
}

void CKSXML_Read_Project::Read_Project_XML_To_DOM(tint32 iProjectID )
{
	// cleanup old tinyxml dom
	mpDoc->Clear();
	
	// read latst revision
	std::string str;
	char psz[128];
	sprintf(psz, "/branches/%d/revisions/latest.xml", iProjectID);
	str = psz;
	
	tchar* pszBuff = NULL;
	tint32 iOutLen = 0;
	ine::IINetUtil::GetWebFile(NULL, "koblo.com", str.c_str(), &iOutLen, &pszBuff);
	
	if ((pszBuff) && (iOutLen > 0)) {

		// parse XML file in to TinyXml object tree
		mpDoc->Parse(pszBuff);
		
		//printf(pszBuff);
	}
	ine::IINetUtil::ReleaseBuffer(&pszBuff);
	
}

void CKSXML_Read_Project::CKSXML_Parse_DOM_To_Preset()
{
	// parse values from project tree in to KS data system
	Parse_Project( mpDoc );


#if (0)
	// (lasse) very very temporary code: download directly from koblo.com

	// These must be set or it won't work
#define TEMPUSR "xxx@xxx.com"
#define TEMPPWD "xxx"

	tchar pszUUID_Proj[128];
	Gen_UUID(pszUUID_Proj, 128);
	tchar pszUUID_Branch[128];
	Gen_UUID(pszUUID_Branch, 128);
	tchar pszUUID_Sample[128];
	Gen_UUID(pszUUID_Sample, 128);
	tchar pszUUID_Take[128];
	Gen_UUID(pszUUID_Take, 128);

	tbool bProjOK = false;
	CAutoDelete<ine::IDownloader> pDownloader(ine::IDownloader::Create());
	if (pDownloader->Init("koblo.com", "/projects.xml", 80, TEMPUSR, TEMPPWD)) {
		pDownloader->SetReplyMIMEType(ine::MIME_TYPE_XML);
		pDownloader->AddParam("project[name]", "Short-lived Test 123 (lasse)", -1);
		pDownloader->AddParam("project[description]", "Temporary project for test only", -1);
		pDownloader->AddParam("project[license]", "by", -1);
		pDownloader->AddParam("project[uuid]", pszUUID_Proj, -1);
		pDownloader->AddParam("branch[uuid]", pszUUID_Branch, -1);
		CAutoDelete<IFile> pfTest(IFile::Create());
#ifdef _WIN32
		tchar* pszTestFile = "C:\\_create-proj-reply.xml";
#endif // _WIN32
#ifdef _Mac
		tchar* pszTestFile = "/_create-proj-reply.xml";
#endif // _Mac
		if (pfTest->Open(pszTestFile, IFile::FileCreate)) {
			if (pDownloader->Start()) {
				tchar pszBuffer[1024];
				tint32 iPortionSize = 0;
				tbool bSuccess, bError;
				do {
					ITime::SleepMS(5);
					pDownloader->GetReplyPortion(pszBuffer, 1024, &iPortionSize);
					if (iPortionSize > 0) {
						pfTest->Write(pszBuffer, iPortionSize);
					}
					bSuccess = pDownloader->IsDone();
					bError = pDownloader->IsFailed();
				} while ((!bSuccess) && (!bError));
			}
			if (pDownloader->IsDone()) {
				bProjOK = true;
			}
		}
		if (pDownloader->IsFailed()) {
			tchar pszErr[1024];
			pDownloader->GetError(pszErr, 1024);
			ge::IWindow::ShowMessageBox(pszErr, "Downloader Error");
		}
	}

	if (bProjOK) {
		CAutoDelete<ine::IUploader> pUploader(ine::IUploader::Create());
		std::string sPage = std::string("/projects/") + pszUUID_Proj;
		sPage += "/samples.xml";
		CAutoDelete<IFile> pfMp3(IFile::Create());
#ifdef _WIN32
		tchar* pszMp3File = "C:\\_testhest.mp3";
#endif // _WIN32
#ifdef _Mac
		tchar* pszMp3File = "/_testhest.mp3";
#endif // _Mac
		if (!pfMp3->Open(pszMp3File, IFile::FileRead))
			return;
		if (pUploader->Init("koblo.com", sPage.c_str(), pfMp3, "mp3[uploaded_data]", 80, TEMPUSR, TEMPPWD)) {
			pUploader->SetReplyMIMEType(ine::MIME_TYPE_XML);
			pUploader->SetSpecificVerb(ine::VERB_POST);
			pUploader->AddParam("sample[uuid]", pszUUID_Sample, -1);
			pUploader->AddParam("sample[name]", "Short-lived Samle for Test 123 (lasse)", -1);
			pUploader->AddParam("take[uuid]", pszUUID_Take, -1);
			pUploader->AddParam("take[description]", "Temporary sample for test only", -1);
			CAutoDelete<IFile> pfReply(IFile::Create());
#ifdef _WIN32
			tchar* pszReplyFile = "C:\\_up-file-reply.xml";
#endif // _WIN32
#ifdef _Mac
			tchar* pszReplyFile = "/_up-file-reply.xml";
#endif // _Mac
			if (pfReply->Open(pszReplyFile, IFile::FileCreate)) {
				if (pUploader->Start(pfReply)) {
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
		}
		if (pUploader->IsFailed()) {
			tchar pszErr[1024];
			pUploader->GetError(pszErr, 1024);
			ge::IWindow::ShowMessageBox(pszErr, "Downloader Error");
		}
	}
#endif 

#if (0)
	// (lasse) very very temporary code: download directly from koblo.com
	CAutoDelete<ine::IDownloader> pDownloader(ine::IDownloader::Create());
	if (pDownloader->Init("koblo.com", "/projects/13/branches/1")) {
		pDownloader->SetReplyMIMEType(ine::MIME_TYPE_XML);
		//pDownloader->AddParam("hl", "da", -1);
		//pDownloader->AddParam("q", "hest", -1);
		//pDownloader->AddParam("btnG", NULL, 0);
		CAutoDelete<IFile> pfTest(IFile::Create());
#ifdef _WIN32
		tchar* pszTestFile = "C:\\_testhest.html";
#endif // _WIN32
#ifdef _Mac
		tchar* pszTestFile = "/_testhest.html";
#endif // _Mac
		if (pfTest->Open(pszTestFile, IFile::FileCreate)) {
			if (pDownloader->Start()) {
				tchar pszBuffer[1024];
				tint32 iPortionSize = 0;
				tbool bSuccess, bError;
				do {
					pDownloader->GetReplyPortion(pszBuffer, 1024, &iPortionSize);
					if (iPortionSize > 0) {
						pfTest->Write(pszBuffer, iPortionSize);
					}
					bSuccess = pDownloader->IsDone();
					bError = pDownloader->IsFailed();
				} while ((!bSuccess) && (!bError));
			}
			if (pDownloader->IsFailed()) {
				tchar pszErr[1024];
				pDownloader->GetError(pszErr, 1024);
				ge::IWindow::ShowMessageBox(pszErr, "Downloader Error");
			}
		}
	}
#endif // (0)

#if (0)
	// (lasse) very very temporary code: download directly from koblo.com
	CAutoDelete<ine::IDownloader> pDownloader(ine::IDownloader::Create());
	if (pDownloader->Init("assets.koblo.com", "/mp3s/7/short2.mp3")) {
		pDownloader->SetReplyMIMEType(ine::MIME_TYPE_MP3);
		CAutoDelete<IFile> pfTest(IFile::Create());
#ifdef _WIN32
		tchar* pszTestFile = "C:\\_testhest.mp3";
#endif // _WIN32
#ifdef _Mac
		tchar* pszTestFile = "/_testhest.mp3";
#endif // _Mac
		if (pfTest->Open(pszTestFile, IFile::FileCreate)) {
			/*
			tchar pszBuffer[1024];
			tint32 iPortionSize = 0;
			tuint64 iTotalSize = 0;
			while (pDownloader->DownloadPortion(pszBuffer, 1024, &iPortionSize, &iTotalSize)) {
				if (pDownloader->IsDone()) {
					// Done
					break;
				}
				if (iPortionSize > 0) {
					pfTest->Write(pszBuffer, iPortionSize);
				}
			}
			*/
			if (pDownloader->Start(pfTest)) {
				tbool bSuccess, bError;
				do {
					bSuccess = pDownloader->IsDone();
					bError = pDownloader->IsFailed();
				} while ((!bSuccess) && (!bError));
			}
			if (pDownloader->IsFailed()) {
				tchar pszErr[1024];
				pDownloader->GetError(pszErr, 1024);
				ge::IWindow::ShowMessageBox(pszErr, "Downloader Error");
			}
		}
	}
#endif
}

void CKSXML_Read_Project::Parse_Project( TiXmlNode* pParent )
{
	// if file is empty return
	if ( !pParent ) return;
	
	TiXmlNode* pChild;
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		if(pChild->Type() == TiXmlNode::ELEMENT)
			Read_Project(pChild);
	}	
}

void CKSXML_Read_Project::Read_Project(TiXmlNode* pParent)
{
	if ( !pParent ) return ;
	printf("--------------------------  PROJECT ----------------------------\n");
	TiXmlAttribute* pAttrib	=	pParent->ToElement()->FirstAttribute();
	tint32 ival;
	
	// project ID
	if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
		printf( "project id =  %d", ival);
	printf( "\n" );
	// scema
	if(pAttrib=pAttrib->Next())
		printf( "%s: %s", pAttrib->Name(), pAttrib->Value());
	printf( "\n" );
	// schema Location 
	if(pAttrib=pAttrib->Next())
		printf( "%s: %s", pAttrib->Name(), pAttrib->Value());
	printf( "\n" );
	
	// parse all childs
	TiXmlNode* pChild;
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		if(pChild->Type() == TiXmlNode::ELEMENT)
			Parse_Project_Childs(pChild);
	}

}

void CKSXML_Read_Project::Parse_Project_Childs(TiXmlNode* pParent)
{
	if (stricmp("branch", pParent->Value()) == 0) {
		printf("--------------------------  BRANCH ------------------------------\n");
		Read_Branch(pParent->ToElement());
	}
	else if (stricmp("settings", pParent->Value()) == 0) {
		printf("-------------------------  SETTINGS -----------------------------\n");
		Read_Settings(pParent->ToElement());
	}
	else if (stricmp("editing", pParent->Value()) == 0) {
		printf("--------------------------  EDDITING ---------------------------\n");
		Read_Edditing(pParent->ToElement());
	}
	else if (stricmp("sample", pParent->Value()) == 0) {
		printf("--------------------------  SAMPLE ----------------------------\n");
		Read_Sample(pParent->ToElement());
	}
	/*
	else if (stricmp("insert", pParent->Value()) == 0) {
		printf("--------------------------  INSERT ----------------------------\n");
		Read_Insert(pParent->ToElement());
	}
	 */
	else if (stricmp("track", pParent->Value()) == 0) {
		printf("-------------------------  TRACK ------------------------------\n");
		Read_Track(pParent->ToElement());
	}
	else if (stricmp("bus", pParent->Value()) == 0) {
		printf("--------------------------  BUS -------------------------------\n");
		Read_Bus(pParent->ToElement());
	}
	else if (stricmp("master", pParent->Value()) == 0) {
		printf("------------------------  MASTER ------------------------------\n");
		Read_Master(pParent->ToElement());
	}
	else
		Ignore_Tag( pParent);
}

void CKSXML_Read_Project::Ignore_Tag(TiXmlNode* pParent)
{
//	printf(".\n");
	if ( !pParent ) return;
	
	TiXmlNode* pChild;
	
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		Ignore_Tag( pChild );
	}
}

void CKSXML_Read_Project::Read_Branch(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();
	tint32 ival;
	
	// project ID
	if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
		printf( "branch id =  %d", ival);
	printf( "\n" );
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("name", pChild->Value()) == 0) {
				printf( "branch name:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, -1);
				printf( "\n" );
			}
			
			else if (stricmp("description", pChild->Value()) == 0) {
				printf( "branch description:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, -1);
				printf( "\n" );
			}
			
			else if (stricmp("revision", pChild->Value()) == 0){
				printf( "branch revision:  ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, -1);
				printf( "\n" );
			}
		}
	}
}

void CKSXML_Read_Project::Read_Settings(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("name", pChild->Value()) == 0) {
				printf( "project name:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, -1);
				printf( "\n" );
			}
			
			else if (stricmp("description", pChild->Value()) == 0) {
				printf( "project description:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, -1);
				printf( "\n" );
			}
			else if (stricmp("image", pChild->Value()) == 0){
				printf( "image link:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, -1);
				printf( "\n" );
			}
			else if (stricmp("samplerate", pChild->Value()) == 0){
				printf( "samplerate:  ");
				Set_Param(pChild, giTinyXml_Type_Int,0, -1);
				printf( "\n" );
			}
			else if (stricmp("tempo", pChild->Value()) == 0){
				printf( "tempo:  ");
				Set_Param(pChild, giTinyXml_Type_Float, giParamID_KS_Tempo , giSectionGlobal, 10.0f);
				printf( "\n" );
			}
			else if (stricmp("signature", pChild->Value()) == 0){
				printf( "signature:  ");
				Read_Signature(pChild->ToElement());
				printf( "\n" );
				
			}
			else if (stricmp("license", pChild->Value()) == 0){
				printf( "license:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, -1);
				printf( "\n" );
			}
		}
	}
}

void CKSXML_Read_Project::Read_Signature(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("numerator", pChild->Value()) == 0) {
				Update_Signature(pChild->ToElement());
				
			}
			if (stricmp("denominator", pChild->Value()) == 0) {
				
			}
		}
	}
}

void CKSXML_Read_Project::Update_Signature(TiXmlElement* pParent)
{
	if ( !pParent ) return;
		
	TiXmlNode* pChild = pParent->FirstChild();
	TiXmlText* pText;
	if ( !pChild ) return;
		
		
	if (  pChild->Type() == TiXmlNode::TEXT )
	{
					
		pText = pChild->ToText();
		std::string s = pText->Value();
		tuint uiValue = atoi(s.c_str());
			
		tuint32 iSignature; 
			
		switch(uiValue){
			case 2:
				iSignature = giSignature2_4;
				break;
			case 3:
				iSignature = giSignature3_4;
				break;
			case 4:
				iSignature = giSignature4_4;
				break;
			case 6:
				iSignature = giSignature6_8;
				break;
			case 9:
				iSignature = giSignature9_8;
				break;
			case 12:
				iSignature = giSignature12_8;
				break;
			default:
				iSignature = giSignature4_4;
					
					
		}
		gpApplication->SetGlobalParm(giParamID_KS_Time_Signature, iSignature, giSectionGlobal);
		
	}

}


void CKSXML_Read_Project::Read_Edditing(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("record", pChild->Value()) == 0) {
				printf( "record:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, -1);
				printf( "\n" );
			}
			
			else if (stricmp("tool", pChild->Value()) == 0) {
				printf( "tool:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, -1);
				printf( "\n" );
			}
			else if (stricmp("zoom", pChild->Value()) == 0){
				printf( "zoom:  ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, -1);
				printf( "\n" );
			}
			else if (stricmp("grid", pChild->Value()) == 0){
				printf( "grid:  ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, -1);
				printf( "\n" );
			}
			else if (stricmp("snap", pChild->Value()) == 0){
				printf( "snap:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, -1);
				printf( "\n" );
				
			}
			else if (stricmp("waves", pChild->Value()) == 0){
				printf( "waves:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, -1);
				printf( "\n" );
				
			}
			else if (stricmp("fades", pChild->Value()) == 0){
				printf( "fades:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, -1);
				printf( "\n" );
				
			}
			else if (stricmp("loop", pChild->Value()) == 0){
				printf( "loop:  ");
				Read_Loop(pChild->ToElement());
			}
			else if (stricmp("position", pChild->Value()) == 0){
				printf( "position:  ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, -1);
				printf( "\n" );
			}
			else if (stricmp("windows", pChild->Value()) == 0){
				//printf( "windows:  ");
				Read_Windows(pChild->ToElement());
			}
			else if (stricmp("dialogs", pChild->Value()) == 0){
				//printf( "windows:  ");
				Read_Dialogs(pChild->ToElement());
			}
		}
	}
}

void CKSXML_Read_Project::Read_Loop(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("active", pChild->Value()) == 0) {
				//printf( "active:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, -1);
				printf( "\n" );
			}
			else if (stricmp("start", pChild->Value()) == 0) {
				printf( "start:  ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, -1);
				printf( "\n" );
			}
			else if (stricmp("end", pChild->Value()) == 0) {
				printf( "end:  ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, -1);
				printf( "\n" );
			}
		}
	}
}

void CKSXML_Read_Project::Read_Windows(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("edit", pChild->Value()) == 0){
				printf( "---- EDIT WINDOW ----\n");
				Read_Window(pChild, 0);
			}
			if (stricmp("mix", pChild->Value()) == 0){
				printf( "---- MIX WINDOW ----\n");
				Read_Window(pChild, 1);
			}
			if (stricmp("rack", pChild->Value()) == 0){
				printf( "---- RACK WINDOW ----\n");
				Read_Window(pChild, 3);
			}
		}
	}
}

void CKSXML_Read_Project::Read_Window(TiXmlNode* pParrent, tint32 iId)
{
	TiXmlElement* pElement = pParrent->ToElement();
	
	if ( !pElement ) return ;
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("show", pChild->Value()) == 0) {
				printf("show: ");
				Set_Param(pChild, giTinyXml_Type_String, 0, -1);
				printf( "\n" );
			}
			else if (stricmp("position", pChild->Value()) == 0){
				printf( "position:  ");
				Read_Window_Pos(pChild, iId);
			}
			else if (stricmp("size", pChild->Value()) == 0){
				printf( "size:  ");
				Read_Window_Size(pChild, iId);
			}
			if (stricmp("layer", pChild->Value()) == 0) {
				printf("layer: ");
				Set_Param(pChild, giTinyXml_Type_Int, 0 , -1);
				printf( "\n" );
			}
			
		}
	}
}

void CKSXML_Read_Project::Read_Window_Pos(TiXmlNode* pParrent, tint32 iId)
{
	TiXmlElement* pElement = pParrent->ToElement();
	
	if ( !pElement ) return ;
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("x", pChild->Value()) == 0) {
				printf( "x: ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, iId);
				printf( "    ");
			}
			else if (stricmp("y", pChild->Value()) == 0) {
				printf( "y:");
				Set_Param(pChild, 0, giTinyXml_Type_Int, iId);
				printf( "\n" );
			}
		}
	}
}

void CKSXML_Read_Project::Read_Window_Size(TiXmlNode* pParrent, tint32 iId)
{
	TiXmlElement* pElement = pParrent->ToElement();
	
	if ( !pElement ) return ;
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("x", pChild->Value()) == 0) {
				printf( "x: ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, iId);
				printf( "    ");
			}
			else if (stricmp("y", pChild->Value()) == 0) {
				printf( "y:");
				Set_Param(pChild, 0, giTinyXml_Type_Int, iId);
				printf( "\n" );
			}
		}
	}
}

void CKSXML_Read_Project::Read_Dialogs(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("export", pChild->Value()) == 0){
				printf( "---- EXPORT DIALOG ----\n");
				Read_Export_Dialog(pChild->ToElement());
			}
		}
	}
}

void CKSXML_Read_Project::Read_Export_Dialog(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("type", pChild->Value()) == 0) {
				printf( "audio type: ");
				Set_Param(pChild, 0, giTinyXml_Type_String, 0);
				printf( "\n");
			}
			else if (stricmp("compression", pChild->Value()) == 0) {
				printf( "compression: ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, 0);
				printf( "\n");
			}
			else if (stricmp("tail", pChild->Value()) == 0) {
				printf( "tail: ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, 0);
				printf( "\n");
			}
			else if (stricmp("normalize", pChild->Value()) == 0) {
				printf( "normalize: ");
				Set_Param(pChild, 0, giTinyXml_Type_String, 0);
				printf( "\n");
			}
		}
	}
}

void CKSXML_Read_Project::Read_Sample(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();
	tint32 ival;
	
	// sample id
	if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
		printf( "sample id =  %d \n", ival);
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("name", pChild->Value()) == 0) {
				printf( "sample name:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("take", pChild->Value()) == 0) {
				Read_Sample_Take(pChild->ToElement());
				printf( "\n" );
			}
		}
	}
}

void CKSXML_Read_Project::Read_Sample_Take(TiXmlElement* pElement)
{
	
	
	if ( !pElement ) return ;
	
	TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();
	tint32 ival;
	
	// sample id
	if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
		printf( "take id =  %d \n", ival);
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("description", pChild->Value()) == 0) {
				printf( "take description:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("url", pChild->Value()) == 0) {
				printf( "take url:  ");
				Set_Param(pChild, giTinyXml_Type_Take, 0, 0);
				printf( "\n" );
			}
			
		}
	}
}

void CKSXML_Read_Project::Read_Track(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();
	tint32 iTrackID;
	
	// track id
	if (pAttrib && pAttrib->QueryIntValue(&iTrackID)==TIXML_SUCCESS)   
		printf( "track id =  %d \n", iTrackID);
	
	
	// owerwrite
	iTrackID = gpApplication->AddTrack();
	
	if(iTrackID == -1) return;
		
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			
			if (stricmp("name", pChild->Value()) == 0) {
				
				Read_Track_Name( pChild->ToElement(), iTrackID);
			}
			
			
			else if (stricmp("size", pChild->Value()) == 0) {
				Set_Param(pChild, giTinyXml_Type_Int, giParam_Track_Info_SizeY, giSection_First_Track + iTrackID);
			}
			
			else if (stricmp("in", pChild->Value()) == 0) {
				Read_Track_In(pChild->ToElement(),  iTrackID);
				printf( "\n" );
			}
			else if (stricmp("out", pChild->Value()) == 0) {
				Read_Track_Out(pChild->ToElement(),  iTrackID);
				
			}
			else if (stricmp("aux", pChild->Value()) == 0) {
				
				TiXmlElement* pElement	=	pChild->ToElement();
				TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();
				tint32 iAux = -1;
				
				// aux id
				if (pAttrib && pAttrib->QueryIntValue(&iAux)!=TIXML_SUCCESS)    
					return;
				// Set value
				Set_Param(pElement, giTinyXml_Type_Float, giParam_ChAUX1 + iAux, giSection_First_Track + iTrackID, 10000.0f);

			}
			else if (stricmp("insert", pChild->Value() ) == 0) {
				Read_Track_Insert(pChild->ToElement(), iTrackID);
				
			}
			else if (stricmp("region", pChild->Value()) == 0) {
				Read_Track_Region(pChild->ToElement(),  iTrackID);
				
			}
		}
	}
	
	miTrack_ID++;
}
void CKSXML_Read_Project::Read_Track_Name(TiXmlElement* pElement, tint32 iTrackID)
{
	if ( !pElement ) return ;
	
	TiXmlNode* pChild = pElement->FirstChild();
	gpApplication->SetChannelName(iTrackID, pChild->Value());
}
void CKSXML_Read_Project::Read_Track_In(TiXmlElement* pElement, tint32 iTrackID)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("input", pChild->Value()) == 0) {
				printf( "input:  ");
				Set_Param(pChild, giTinyXml_Type_Int, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("mode", pChild->Value()) == 0) {
				printf( "input mode:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("gain", pChild->Value()) == 0) {
		//		printf( "input gain:  ");
		//		Set_Param(pChild, giTinyXml_Type_Float, giParam_ChOut, giSection_First_Track + iTrackID);
				
			}
		}
	}
}

void CKSXML_Read_Project::Read_Track_Out(TiXmlElement* pElement, tint32 iTrackID)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("out", pChild->Value()) == 0) 
				Set_Param(pChild, giTinyXml_Type_Int, giParam_ChOut, giSection_First_Track + iTrackID);
			
			else if (stricmp("mode", pChild->Value()) == 0) 
				Set_Param(pChild, giTinyXml_Type_Int, giParam_ChMode, giSection_First_Track + iTrackID);
			
			else if (stricmp("volume", pChild->Value()) == 0) 
				Set_Param(pChild, giTinyXml_Type_Float, giParam_ChVol, giSection_First_Track + iTrackID, 10000.0f);
			
			else if (stricmp("pan", pChild->Value()) == 0) 
				Set_Param(pChild, giTinyXml_Type_Float, giParam_ChPannerLeftRight, giSection_First_Track + iTrackID, 10000.0f);
			
			else if (stricmp("solo", pChild->Value()) == 0)
				Read_Track_Solo(pChild,   iTrackID);
			
			else if (stricmp("mute", pChild->Value()) == 0) 
				Read_Track_Mute(pChild,  iTrackID);
			
			else if (stricmp("arm", pChild->Value()) == 0)
				Read_Track_Arm(pChild,  iTrackID);
			
			
			
			
		}
	}
}


void CKSXML_Read_Project::Read_Track_Solo(TiXmlNode* pParent, tint32 iTrackID)
{
	TiXmlNode* pChild = pParent->FirstChild();

	if ( !pChild ) return;
	
	if(stricmp("on", pChild->Value()) == 0)
		gpApplication->SetGlobalParm(giParam_ChSolo, 1, giSection_First_Track+ iTrackID);
	
	else if(stricmp("off", pChild->Value()) == 0)
		gpApplication->SetGlobalParm(giParam_ChSolo, 0, giSection_First_Track+ iTrackID);
	
}

void CKSXML_Read_Project::Read_Track_Mute(TiXmlNode* pParent, tint32 iTrackID)
{
	TiXmlNode* pChild = pParent->FirstChild();
	
	if ( !pChild ) return;
	
	if(stricmp("on", pChild->Value()) == 0)
		gpApplication->SetGlobalParm(giParam_ChMute, 1, giSection_First_Track+ iTrackID);
	
	else if(stricmp("off", pChild->Value()) == 0)
		gpApplication->SetGlobalParm(giParam_ChMute, 0, giSection_First_Track+ iTrackID);
}

void CKSXML_Read_Project::Read_Track_Arm(TiXmlNode* pParent, tint32 iTrackID)
{
	TiXmlNode* pChild = pParent->FirstChild();
	
	if ( !pChild ) return;
	
	if(stricmp("on", pChild->Value()) == 0)
		gpApplication->SetGlobalParm(giParam_ChArm, 1, giSection_First_Track+ iTrackID);
	
	else if(stricmp("off", pChild->Value()) == 0)
		gpApplication->SetGlobalParm(giParam_ChArm, 0, giSection_First_Track+ iTrackID);
}
/*

void CKSXML_Read_Project::Read_Track_Aux(TiXmlNode* pParent, tint32 iTrackID)
{
	TiXmlNode* pChild = pParent->FirstChild();
	
	TiXmlAttribute* pAttrib	=	pChild->FirstAttribute();
		tint32 iAux = -1;
	
	// aux id
	if (pAttrib && pAttrib->QueryIntValue(&iAux)!=TIXML_SUCCESS)    
		return;
	

	Set_Param(pElement, giTinyXml_Type_Float, giParam_ChAUX1 + iAux, giSection_First_Track + iTrackID, 10000.0f);

}
*/

void CKSXML_Read_Project::Read_Track_Insert(TiXmlElement* pElement, tint32 uTrack)
{
	if ( !pElement ) return ;
	
	TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();
	tint32 iSlot, iPluginId;
	TiXmlNode* pChild;
	
	// slot id
	if (pAttrib->QueryIntValue(&iSlot)==TIXML_SUCCESS) {
		printf( "insert:%d,  ", iSlot);
		// plug-in id
		if (pAttrib->QueryIntValue(&iPluginId)==TIXML_SUCCESS) {
			printf( "plug-in id:%d,  ", iPluginId);
			gpApplication->SetGlobalParm(giParam_ChInsert1 + iSlot, iPluginId, giSection_First_Track + uTrack);
		}
	}
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("vendor", pChild->Value()) == 0) {
				printf( "vendor:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("product", pChild->Value()) == 0) {
				printf( "product:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("url", pChild->Value()) == 0) {
				printf( "url:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("bypass", pChild->Value()) == 0) {
				printf( "bypass:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("settings", pChild->Value()) == 0) {
				Read_Insert_Settings(pChild->ToElement());
				printf( "\n" );
			}
		}
	}
}

void CKSXML_Read_Project::Read_Insert(TiXmlElement* pElement)
{
	/*
	if ( !pElement ) return ;
	
	TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();
	tint32 ival;
	
	// sample id
	if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
		printf( "insert id =  %d \n", ival);
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("vendor", pChild->Value()) == 0) {
				printf( "vendor:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("product", pChild->Value()) == 0) {
				printf( "product:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("url", pChild->Value()) == 0) {
				printf( "url:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("bypass", pChild->Value()) == 0) {
				printf( "bypass:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("settings", pChild->Value()) == 0) {
				Set_Insert_Settings(pChild->ToElement());
				printf( "\n" );
			}
		}
	}
	 */
}

void CKSXML_Read_Project::Read_Insert_Settings(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("parameter", pChild->Value()) == 0) {
				Read_Insert_Parameter(pChild->ToElement());
				printf( "\n" );
			}
			else if (stricmp("chunk", pChild->Value()) == 0) {
				printf( "chunk url:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
		}
	}
}

void CKSXML_Read_Project::Read_Insert_Parameter(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("name", pChild->Value()) == 0) {
				printf( "parameter name:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( ", " );
			}
			else if (stricmp("default", pChild->Value()) == 0) {
				printf( "default:  ");
				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);
				printf( ", " );
			}
			else if (stricmp("value", pChild->Value()) == 0) {
				printf( "value:  ");
				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);
			}
		}
	}
}

void CKSXML_Read_Project::Read_Track_Region(TiXmlElement* pElement, tint32 iTrackID)
{
	if ( !pElement ) return ;
	
	TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();
	static tint32 ival = 0;
	printf( "--------\n", ival);
	// aux id
	if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
		printf( "region id:%d   ", ival);
	
	if(pAttrib=pAttrib->Next())
		
		// aux id
		if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
			printf( "   take:%d \n", ival);
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("position", pChild->Value()) == 0) {
				printf( "position:  ");
				Set_Param(pChild, giTinyXml_Type_Int, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("start", pChild->Value()) == 0) {
				printf( "start:  ");
				Set_Param(pChild, giTinyXml_Type_Int, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("end", pChild->Value()) == 0) {
				printf( "end:  ");
				Set_Param(pChild, giTinyXml_Type_Int, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("volume", pChild->Value()) == 0) {
				printf( "volume:  ");
				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);
				printf( "\n" );
			}
		}
	}
}

void CKSXML_Read_Project::Read_Track_Region_Fade(TiXmlElement* pElement, tint32 iTrackID)
{
	if ( !pElement ) return ;
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("in", pChild->Value()) == 0) {
				printf( "fade in:  ");
				Set_Param(pChild, giTinyXml_Type_Int, 0, 0);
				//	printf( "\n" );
			}
			else if (stricmp("out", pChild->Value()) == 0) {
				printf( "fade out:  ");
				Set_Param(pChild, giTinyXml_Type_Int, 0, 0);
					printf( "\n" );
			}

		}
	}
}

void CKSXML_Read_Project::Read_Bus(TiXmlElement* pElement)
{
	return;
	if ( !pElement ) return ;
	
	TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();
	tint32 ival;
	
	// sample id
	if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
		printf( "bus id =  %d \n", ival);
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("out", pChild->Value()) == 0) {
			//	Read_Track_Out(pChild->ToElement(), ival);
				
			}
			else if (stricmp("aux", pChild->Value()) == 0) {
			//	Read_Track_Aux(pChild->ToElement(), ival);
				
			}
			/*
			else if (stricmp("insert", pChild->Value()) == 0) {
				Read_Track_Insert(pChild->ToElement());
				
			}
			 */
		}
	}
}


void CKSXML_Read_Project::Read_Bus_Out(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("out", pChild->Value()) == 0) {
				printf( "out:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("mode", pChild->Value()) == 0) {
				printf( "output mode:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("volume", pChild->Value()) == 0) {
				printf( "output volume:  ");
				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("pan", pChild->Value()) == 0) {
				printf( "pan:  ");
				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("solo", pChild->Value()) == 0) {
				printf( "solo:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("mute", pChild->Value()) == 0) {
				printf( "mute:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("arm", pChild->Value()) == 0) {
				printf( "arm:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
		}
	}
}

void CKSXML_Read_Project::Read_Bus_Aux(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();
	tint32 ival;
	
	// aux id
	if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
		printf( "AUX %d ", ival);
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("send", pChild->Value()) == 0) {
				printf( "send:  ");
				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);
				printf( "\n" );
			}
		}
	}
}

void CKSXML_Read_Project::Read_Bus_Insert(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();
	tint32 ival;
	
	// aux id
	if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
		printf( "insert slot;%d,  ", ival);
	
	if(pAttrib=pAttrib->Next())
		
		// aux id
		if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
			printf( "id:%d \n", ival);
	
}


void CKSXML_Read_Project::Read_Master(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			
			if (stricmp("out", pChild->Value()) == 0) {
				Read_Master_Out(pChild->ToElement());
				
			}
			else if (stricmp("aux_return", pChild->Value()) == 0) {
				Read_Master_Aux_Return(pChild->ToElement());
				
			}
			else if (stricmp("insert", pChild->Value()) == 0) {
				Read_Master_Insert(pChild->ToElement());
				
			}
		}
	}
}

void CKSXML_Read_Project::Read_Master_Aux_Return(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();
	tint32 ival;
	
	// aux id
	if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
		printf( "AUX %d return: ", ival);
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("volume", pChild->Value()) == 0) {
				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);
				printf( "\n" );
			}
		}
	}
}

void CKSXML_Read_Project::Read_Master_Insert(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();
	tint32 ival;
	
	// aux id
	if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
		printf( "insert slot;%d,  ", ival);
	
	if(pAttrib=pAttrib->Next())
		
		// aux id
		if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
			printf( "id:%d \n", ival);
	
}

void CKSXML_Read_Project::Read_Master_Out(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("volume", pChild->Value()) == 0) {
				printf( "volume:  ");
				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("pan", pChild->Value()) == 0) {
				printf( "pan:  ");
				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);
				printf( "\n" );
			}
		}
	}
}






void CKSXML_Read_Project::Set_Param( TiXmlNode* pParent, tuint uiType, tuint32 iParamID, tint iSection,  tfloat fFactor)
{
	if ( !pParent ) return;
	
	
	TiXmlNode* pChild = pParent->FirstChild();
	TiXmlText* pText;
	if ( !pChild ) return;
	
	
	
	if (  pChild->Type() == TiXmlNode::TEXT )
	{
		switch(uiType){
			case giTinyXml_Type_String:
				pText = pChild->ToText();
				printf( "%s", pText->Value() );
				break;
				
			case giTinyXml_Type_Int:{
				
				pText = pChild->ToText();
				std::string s = pText->Value();
				tfloat fValue = atoi(s.c_str());
				printf( "%f", fValue );
				
				if(iSection == -1) return;
					gpApplication->SetGlobalParm(iParamID, fValue*fFactor, iSection);
				
				
				break;
			}
			 case giTinyXml_Type_Float:{
			 
				 pText = pChild->ToText();
				 std::string s = pText->Value();
				 tfloat32 fValue = atof(s.c_str());
			//	 printf( "%lf", fValue );
				 
				 if(iSection == -1) return;
					gpApplication->SetGlobalParm(iParamID, tint32(fValue*fFactor), iSection);
				 break;

			 }
			case giTinyXml_Type_Take:{
				pText = pChild->ToText();
				printf( "%s", pText->Value() );
				
				break;
			}
			default : break;
		}
	}
	/*
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		Set_Param( pChild, uiType, iParamID, fFactor);
	}
	 */
	
}






tbool CKSXML_Read_Project::Check_For_Newer_Revision(tint32 iProject_ID)
{
	mbNew_Revision = false;
	// Check DOM downloaded to see if there is a newer one
	Check_Project( mpDoc );
	// return resoult
	return mbNew_Revision;
}


void CKSXML_Read_Project::Check_Project( TiXmlNode* pParent )
{
	// if file is empty return
	if ( !pParent ) return;
	
	TiXmlNode* pChild;
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		if(pChild->Type() == TiXmlNode::ELEMENT)
			Check_Project_Childs(pChild);
	}	
}

void CKSXML_Read_Project::Check_Project_Childs(TiXmlNode* pParent)
{
	if ( !pParent ) return ;
	/*
	printf("--------------------------  PROJECT ----------------------------\n");
	TiXmlAttribute* pAttrib	=	pParent->ToElement()->FirstAttribute();
	tint32 ival;
	
	// project ID
	if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
		printf( "project id =  %d", ival);
	printf( "\n" );
	// scema
	if(pAttrib=pAttrib->Next())
		printf( "%s: %s", pAttrib->Name(), pAttrib->Value());
	printf( "\n" );
	// schema Location 
	if(pAttrib=pAttrib->Next())
		printf( "%s: %s", pAttrib->Name(), pAttrib->Value());
	printf( "\n" );
	*/
	// parse all childs
	TiXmlNode* pChild;
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		if(pChild->Type() == TiXmlNode::ELEMENT)
			Check_Branch(pChild);
	}}

void CKSXML_Read_Project::Check_Branch(TiXmlNode* pParent)
{
	if (stricmp("branch", pParent->Value()) == 0) {
		Check_Revision(pParent->ToElement());
	}	
}

void CKSXML_Read_Project::Check_Revision(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
		
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			 if (stricmp("revision", pChild->Value()) == 0){
				 
				 TiXmlNode* pContent = pChild->FirstChild();
				 
				 if ( !pContent ) return;
				 if (  pContent->Type() == TiXmlNode::TEXT )
				 {
					TiXmlText* pText;
					pText = pContent->ToText();
					std::string s = pText->Value();
					tint32 iNew_Revision = atoi(s.c_str());
					 if(iNew_Revision > gpApplication->GetGlobalParm(giParamID_Revision_Nr, giSectionGlobal))
						 mbNew_Revision = true;
				 }
			 }
		}
	}
	
}
