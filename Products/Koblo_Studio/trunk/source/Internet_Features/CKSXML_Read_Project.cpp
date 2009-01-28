
#include "KSOS.h"


CKSXML_Read_Project::CKSXML_Read_Project()
{
	
	mpDoc = new TiXmlDocument;
	
}

CKSXML_Read_Project::~CKSXML_Read_Project()
{
	delete mpDoc;
}

void CKSXML_Read_Project::Read_Project_From_Disk()
{
	
	
	std::string sProject_Folder = gpApplication->Project_Folder();
	std::string sProject_Name	= gpApplication->Project_Name();
	
	/*
	for(tint i = 0; i != std::string::npos; i = sProject_Folder.find(":")) {
		
		sProject_Folder.erase(i,1);
		sProject_Folder.insert(i,"/");
	}
	 */
	

	std::string sRead			= sProject_Folder +  sProject_Name ;
	
	
	CAutoDelete<IFile> pfile(IFile::Create());
	if (pfile->Open(sRead.c_str(), IFile::FileRead)) {
		
		Reset_Project();
		tuint iSize = pfile->GetSizeWhenOpened();
		char psz[iSize];
		
		pfile->Read(psz, iSize);
		mpDoc->Parse(psz);
		Parse_Project( mpDoc );
	}
	
}

void CKSXML_Read_Project::Reset_Project()
{
	miTrack_ID	=	0;
	gpApplication->CleanProject(0);
	
	mpDoc->Clear();
	
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

	// (lasse) very very temporary code: download directly from koblo.com
	CAutoDelete<ine::IDownloader> pDownloader(ine::IDownloader::Create());
	if (pDownloader->Init("assets.koblo.com", "/mp3s/7/short2.mp3")) {
		pDownloader->SetReplyMIMEType(ine::MIME_TYPE_MP3);
		CAutoDelete<IFile> pfTest(IFile::Create());
#ifdef _WIN32
		tchar* pszTestFile = "C:\\testhest.mp3";
#endif // _WIN32
#ifdef _Mac
		tchar* pszTestFile = "/testhest.mp3";
#endif // _Mac
		if (pfTest->Open(pszTestFile, IFile::FileCreate)) {
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
			if (pDownloader->IsFailed()) {
				tchar pszErr[1024];
				pDownloader->GetLatestError(pszErr, 1024);
				ge::IWindow::ShowMessageBox(pszErr, "Downloader Error");
			}
		}
	}
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
	
	TiXmlAttribute* pAttrib	=	pParent->ToElement()->FirstAttribute();
	// project uuid
	gpApplication->Set_Project_UUID( pAttrib->Value() );
	
/*
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
			Parse_Project_Childs(pChild);
	}

}

void CKSXML_Read_Project::Parse_Project_Childs(TiXmlNode* pParent)
{
	if (stricmp("name", pParent->Value()) == 0)
		Read_Project_Name(pParent);
	
	else if (stricmp("branch", pParent->Value()) == 0) {
		Read_Branch(pParent->ToElement());
	}
	else if (stricmp("settings", pParent->Value()) == 0) {
		Read_Settings(pParent->ToElement());
	}
	else if (stricmp("editing", pParent->Value()) == 0) {
		Read_Edditing(pParent->ToElement());
	}
	else if (stricmp("sample", pParent->Value()) == 0) {
		Read_Sample(pParent->ToElement());
	}
	/*
	else if (stricmp("insert", pParent->Value()) == 0) {
		Read_Insert(pParent->ToElement());
	}
	 */
	else if (stricmp("track", pParent->Value()) == 0) {
		Read_Track(pParent->ToElement());
	}
	else if (stricmp("bus", pParent->Value()) == 0) {
		Read_Bus(pParent->ToElement());
	}
	else if (stricmp("master", pParent->Value()) == 0) {
		Read_Master(pParent->ToElement());
	}
	/*
	else
		Ignore_Tag( pParent);
	*/
}
void CKSXML_Read_Project::Read_Project_Name(TiXmlNode* pParent)
{
	if ( !pParent ) return;
	
	TiXmlNode* pChild = pParent->FirstChild();
	
	if ( pChild ){
		TiXmlText* pText;
		pText = pChild->ToText();
		gpApplication->Project_Name(pText->Value());
	}
}

/*
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
*/
void CKSXML_Read_Project::Read_Branch(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	 // branch uuid
	TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();
	 if(pAttrib)
		 gpApplication->Set_Branch_UUID( pAttrib->Value() );
	 
	TiXmlNode* pChild;
	TiXmlText* pText;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("name", pChild->Value()) == 0) {
				
				TiXmlNode* pName = pChild->FirstChild();
				if(pName) {
					pText = pName->ToText();
					if(pText)
						gpApplication->Set_Branch_Name(pText->Value());
				}
			}
			
			else if (stricmp("description", pChild->Value()) == 0) {
				
				
				TiXmlNode* pSet_Branch_Description = pChild->FirstChild();
				if(pSet_Branch_Description) {
					pText = pSet_Branch_Description->ToText();
					if(pText)
						gpApplication->Set_Branch_Description(pText->Value());
				}
			}
			
			else if (stricmp("revision", pChild->Value()) == 0){
				
				TiXmlNode* pSet_Branch_Revision = pChild->FirstChild();
				if(pSet_Branch_Revision) {
					std::string sBranch_Revision = pSet_Branch_Revision->Value();
					gpApplication->Branch_Revision(atoi(sBranch_Revision.c_str()));
				}
			}
		}
	}
}


void CKSXML_Read_Project::Read_Settings(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
//	TiXmlNode* pNote;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("samplerate", pChild->Value()) == 0){
				
				
				TiXmlNode* pNote = pChild->FirstChild();
				if(pNote) {
					std::string sSample_Rate = pNote->Value();
					// gpApplication->SetSampleRate(atoi(sSample_Rate.c_str()));
				}
				
			}
			else if (stricmp("tempo", pChild->Value()) == 0)
				Set_Param(pChild, giTinyXml_Type_Float, giParamID_KS_Tempo , giSectionGlobal, 10.0f);
			
			else if (stricmp("signature", pChild->Value()) == 0){
				Read_Signature(pChild->ToElement());
				Update_Signature();
			}	
			
			else if (stricmp("license", pChild->Value()) == 0){
				//!!! TO DO do somthing here
				Set_Param(pChild, 0, giTinyXml_Type_String, -1);

			}
		}
	}
}

void CKSXML_Read_Project::Read_Signature(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	TiXmlNode* pNote;
	std::string sValue;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("numerator", pChild->Value()) == 0) {
				
				pNote				= pChild->FirstChild();
				if(pNote) {
					sValue			=	pNote->Value();
					muiNummerator	=	(atoi(sValue.c_str()));
				}
				
			}
			if (stricmp("denominator", pChild->Value()) == 0) {
				
				pNote				= pChild->FirstChild();
				if(pNote) {
					sValue			=	pNote->Value();
					muiDominator	=	(atoi(sValue.c_str()));
				}
				
			}
		}
	}

}

void CKSXML_Read_Project::Update_Signature()
{

	tuint32 iSignature;
	tuint32 uiValue		=	muiNummerator * 100;
	uiValue				+=	muiDominator;
			
	switch(uiValue){
		case 204:
			iSignature = giSignature2_4;
			break;
		case 304:
			iSignature = giSignature3_4;
			break;
		case 404:
			iSignature = giSignature4_4;
			break;
		case 608:
			iSignature = giSignature6_8;
			break;
		case 908:
			iSignature = giSignature9_8;
			break;
		case 1208:
			iSignature = giSignature12_8;
			break;
		default:
			iSignature = giSignature4_4;
				
				
	}
	gpApplication->SetGlobalParm(giParamID_KS_Time_Signature, iSignature, giSectionGlobal);

}


void CKSXML_Read_Project::Read_Edditing(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("record", pChild->Value()) == 0) 
				// never start recording upon load
				gpApplication->SetRecord(false);

			else if (stricmp("tool", pChild->Value()) == 0) {

				TiXmlNode* pTool = pChild->FirstChild();
				if(pTool) {
					std::string sTool = pTool->Value();
					Read_Tool(sTool);
				}
			}

			else if (stricmp("zoom", pChild->Value()) == 0)
				Set_Param(pChild, giTinyXml_Type_Int,giParamID_Zoom , giSectionGUI );
			
			
			else if (stricmp("grid", pChild->Value()) == 0) {
				TiXmlNode* pGrid = pChild->FirstChild();
				if(pGrid) {
					std::string sGrid = pGrid->Value();
					Read_Grid(sGrid);
				}
			}
				
			else if (stricmp("snap", pChild->Value()) == 0){
				TiXmlNode* pSnap = pChild->FirstChild();
				if(pSnap) {
					std::string sSnap = pSnap->Value();
					Read_Snap(sSnap);
				}
			}
			
			
			else if (stricmp("waves", pChild->Value()) == 0){
				
				TiXmlNode* pWaves = pChild->FirstChild();
				if(pWaves) {
					std::string sWaves = pWaves->Value();
					Read_View_Waves(sWaves);
				}
			}
				
			else if (stricmp("fades", pChild->Value()) == 0){
				
				TiXmlNode* pFades = pChild->FirstChild();
				if(pFades) {
					std::string sFades = pFades->Value();
					Read_View_Fades(sFades);
				}
			}
			
			else if (stricmp("loop", pChild->Value()) == 0)
				Read_Loop(pChild->ToElement());

			else if (stricmp("windows", pChild->Value()) == 0)
				Read_Windows(pChild->ToElement());

			else if (stricmp("dialogs", pChild->Value()) == 0)
				Read_Dialogs(pChild->ToElement());

		}
	}
}

void CKSXML_Read_Project::Read_Tool(std::string sTool)
{

			
	if (stricmp("hand", sTool.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Tool_Selected, giTool_Hand,  giSectionGUI);
	
	else if (stricmp("trim", sTool.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Tool_Selected, giTool_Trim,  giSectionGUI);
	
	else if (stricmp("select", sTool.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Tool_Selected, giTool_Select,  giSectionGUI);
	
	else if (stricmp("cut", sTool.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Tool_Selected, giTool_Cut,  giSectionGUI);
			
	
}

void CKSXML_Read_Project::Read_Grid(std::string sGrid)
{
	if (stricmp("off", sGrid.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Grid, 0,  giSectionGUI);
	
	else if (stricmp("on", sGrid.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Grid, 1,  giSectionGUI);
	
}

void CKSXML_Read_Project::Read_Snap(std::string sSnap)
{
	if (stricmp("1/1", sSnap.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_KS_Snap_To, 32,  giSectionGlobal);
	
	else if (stricmp("1/2", sSnap.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_KS_Snap_To, 16,  giSectionGlobal);
	
	else if (stricmp("1/4", sSnap.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_KS_Snap_To, 8,  giSectionGlobal);
	
	else if (stricmp("1/8", sSnap.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_KS_Snap_To, 4,  giSectionGlobal);
	
	else if (stricmp("1/16", sSnap.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_KS_Snap_To, 2,  giSectionGlobal);
	
	else if (stricmp("1/32", sSnap.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_KS_Snap_To, 1,  giSectionGlobal);
	
	

}

void CKSXML_Read_Project::Read_View_Waves(std::string sWaves)
{
	
	if (stricmp("off", sWaves.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Show_Waveform, 0,  giSectionGUI);
	
	else if (stricmp("on", sWaves.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Show_Waveform, 1,  giSectionGUI);
	

}

void CKSXML_Read_Project::Read_View_Fades(std::string sFades)
{
	if (stricmp("off", sFades.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Show_Fade, 0,  giSectionGUI);
	
	else if (stricmp("on", sFades.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Show_Fade, 1,  giSectionGUI);
	
}



void CKSXML_Read_Project::Read_Loop(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("active", pChild->Value()) == 0) {
				TiXmlNode* pActive = pChild->FirstChild();
				if(pActive) {
					std::string sActive = pActive->Value();
					Loop_Active(sActive);
				}
			}
			
			else if (stricmp("start", pChild->Value()) == 0) 
				Set_Param(pChild, giTinyXml_Type_Int,giParamID_Loop_Start , giSectionGUI );

			else if (stricmp("end", pChild->Value()) == 0) 
				Set_Param(pChild, giTinyXml_Type_Int,giParamID_Loop_End , giSectionGUI );

		}
	}
}

void CKSXML_Read_Project::Loop_Active(std::string sGrid)
{
	if (stricmp("off", sGrid.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Loop_On, 0,  giSectionGlobal);
	
	else if (stricmp("on", sGrid.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Loop_On, 1,  giSectionGlobal);
	
}

void CKSXML_Read_Project::Read_Windows(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("edit", pChild->Value()) == 0){
				Read_Window(pChild, giParamID_Show_Track_Window);
			}
			if (stricmp("mix", pChild->Value()) == 0){
				Read_Window(pChild, giParamID_Show_Mix_Window);
			}
			if (stricmp("rack", pChild->Value()) == 0){
				Read_Window(pChild, giParamID_Show_AUX_Window);
			}
		}
	}
}

void CKSXML_Read_Project::Read_Window(TiXmlNode* pParrent, tint32 iWindow)
{
	TiXmlElement* pElement = pParrent->ToElement();
	
	if ( !pElement ) return ;
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("show", pChild->Value()) == 0) {
				TiXmlNode* pOn_Off = pChild->FirstChild();
				if(pOn_Off) {
					std::string  sOn_Off = pOn_Off->Value();
					Open_Window( sOn_Off, iWindow);
				}
			}
			else if (stricmp("position", pChild->Value()) == 0){
				Read_Window_Pos(pChild, iWindow);
			}
			else if (stricmp("size", pChild->Value()) == 0){
				Read_Window_Size(pChild, iWindow);
			}
			if (stricmp("layer", pChild->Value()) == 0) {
				//;Set_Param(pChild, giTinyXml_Type_Int, 0 , -1);
			}
			
		}
	}
}

void CKSXML_Read_Project::Open_Window(std::string sOn_Off, tint32 iWindow)
{
	
	if (stricmp("off", sOn_Off.c_str()) == 0) 
		gpApplication->SetGlobalParm(iWindow, 0,  giSectionGlobal);
	
	else if (stricmp("on", sOn_Off.c_str()) == 0) 
		gpApplication->SetGlobalParm(iWindow, 1,  giSectionGlobal);
	

}

void CKSXML_Read_Project::Read_Window_Pos(TiXmlNode* pParrent, tint32 iId)
{
	TiXmlElement* pElement = pParrent->ToElement();
	
	if ( !pElement ) return ;
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("x", pChild->Value()) == 0) {
				Set_Param(pChild, 0, giTinyXml_Type_Int, iId);
			}
			else if (stricmp("y", pChild->Value()) == 0) {
				Set_Param(pChild, 0, giTinyXml_Type_Int, iId);
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
				Set_Param(pChild, 0, giTinyXml_Type_Int, iId);
			}
			else if (stricmp("y", pChild->Value()) == 0) {
				Set_Param(pChild, 0, giTinyXml_Type_Int, iId);
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

				Set_Param(pChild, 0, giTinyXml_Type_String, 0);
			}
			else if (stricmp("compression", pChild->Value()) == 0) {
				Set_Param(pChild, 0, giTinyXml_Type_Int, 0);
			}
			else if (stricmp("tail", pChild->Value()) == 0) {
				Set_Param(pChild, 0, giTinyXml_Type_Int, 0);
			}
			else if (stricmp("normalize", pChild->Value()) == 0) {
				Set_Param(pChild, 0, giTinyXml_Type_String, 0);
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
		;	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("name", pChild->Value()) == 0) {
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("take", pChild->Value()) == 0) {
				Read_Sample_Take(pChild->ToElement());

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
		;
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("description", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("url", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_Take, 0, 0);

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
		;
	
	
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

				Set_Param(pChild, giTinyXml_Type_Int, 0, 0);

			}
			else if (stricmp("mode", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_String, 0, 0);

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
		//printf( "insert:%d,  ", iSlot);
		// plug-in id
		if (pAttrib->QueryIntValue(&iPluginId)==TIXML_SUCCESS) {
			//printf( "plug-in id:%d,  ", iPluginId);
			gpApplication->SetGlobalParm(giParam_ChInsert1 + iSlot, iPluginId, giSection_First_Track + uTrack);
		}
	}
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("vendor", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("product", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("url", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("bypass", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("settings", pChild->Value()) == 0) {
				Read_Insert_Settings(pChild->ToElement());

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

			}
			else if (stricmp("chunk", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_String, 0, 0);

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

				Set_Param(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("default", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);

			}
			else if (stricmp("value", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);
			}
		}
	}
}

void CKSXML_Read_Project::Read_Track_Region(TiXmlElement* pElement, tint32 iTrackID)
{
	if ( !pElement ) return ;
	
	TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();
	tint32 ival;
	// aux id
	if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
		;//	printf( "region id:%d   ", ival);
	
	if(pAttrib=pAttrib->Next())
		
		// aux id
		if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
			;//printf( "   take:%d \n", ival);
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("position", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_Int, 0, 0);

			}
			else if (stricmp("start", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_Int, 0, 0);

			}
			else if (stricmp("end", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_Int, 0, 0);

			}
			else if (stricmp("volume", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);

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

				Set_Param(pChild, giTinyXml_Type_Int, 0, 0);

			}
			else if (stricmp("out", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_Int, 0, 0);

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
		;//printf( "bus id =  %d \n", ival);
	
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

				Set_Param(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("mode", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("volume", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);

			}
			else if (stricmp("pan", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);

			}
			else if (stricmp("solo", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("mute", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("arm", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_String, 0, 0);

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
		;//printf( "AUX %d ", ival);
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("send", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);

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
		;//printf( "insert slot;%d,  ", ival);
	
	if(pAttrib=pAttrib->Next())
		
		// aux id
		if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
			;//printf( "id:%d \n", ival);
	
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
		;//printf( "AUX %d return: ", ival);
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("volume", pChild->Value()) == 0) {
				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);

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
		;//printf( "insert slot;%d,  ", ival);
	
	if(pAttrib=pAttrib->Next())
		
		// aux id
		if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
			;//printf( "id:%d \n", ival);
	
}

void CKSXML_Read_Project::Read_Master_Out(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("volume", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);

			}
			else if (stricmp("pan", pChild->Value()) == 0) {

				Set_Param(pChild, giTinyXml_Type_Float, 0, 0);

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
			//	printf( "%s", pText->Value() );
				break;
				
			case giTinyXml_Type_Int:{
				
				pText = pChild->ToText();
				std::string s = pText->Value();
				tfloat fValue = atoi(s.c_str());
			//	printf( "%f", fValue );
				
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
			//	printf( "%s", pText->Value() );
				
				break;
			}
			default : break;
		}
	}
	
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
