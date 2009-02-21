
#include "KSOS.h"


CKSXML_Read_Project::CKSXML_Read_Project()
{
	

	
}

CKSXML_Read_Project::~CKSXML_Read_Project()
{

}

void CKSXML_Read_Project::Prepare_For_XML()
{
	mSample_Data_List.clear();
	mDecompress_Que.clear();
	mDownload_Que.clear();
	mInsert_Que.clear();
	mCreate_Pict_File_Que.clear();
	mRegion_Data_List.clear();
	
	muiTrack	=	0;
	
	mpTinyXMLDoc->Clear();
}

void CKSXML_Read_Project::Read_Project_From_Disk(std::string sFile)
{
	
	// read project in to 
	CAutoDelete<IFile> pFile(IFile::Create());
	if (pFile->Open(sFile.c_str(), IFile::FileRead)) {
		
		gpApplication->Stop_Timer();
		
		// reset/ erase the current DAW project
		Reset_Project();
		
		Prepare_For_XML();
		
		// read project in to char buffer
		tuint iSize = pFile->GetSizeWhenOpened();
		CAutoBuffer<char> pszProjectXML;
		pszProjectXML.Allocate(iSize);

		pFile->Read(pszProjectXML, iSize);
		
		// parse pszProjectXML in to a TinyXML DOM
		mpTinyXMLDoc->Parse(pszProjectXML);
		
		// pass the TinyXML DOM in to the DAW data structure
		Pass_The_Project_Tag( mpTinyXMLDoc );

		// Make sure folders are there
	//	gpApplication->Create_Folders();
		
		// 
		Prepare_Samples();
		
		// decompress takes
		Decompress_Takes();
		
		// download takes
		Download_Takes();
		
		// set up the track editor with regions
//		Setup_Track_Editor();
		
		/*
		// insert regions in track editor
		if( !bDownloading_Takes)
			Setup_Track_Editor();
		*/
		
		
		
		gpApplication->Start_Timer();
			
		//gpApplication->UpdateGUIFileList();
	}
	
}




void CKSXML_Read_Project::Setup_Track_Editor()
{
	
//	gpApplication->Stop_Timer();
	// insert takes 
	Insert_Takes();
	
	// insert regions
	Insert_Regions();
	
	CBasePane::SMsg Msg;
	Msg = Msg_Deselect_Regions;
	gpApplication->Send_Msg_To_All_Panes(&Msg);
	
	gpApplication->UpdateGUIFileList();
	
//	gpApplication->Start_Timer();
	
}
/*
void CKSXML_Read_Project::Reset_Project()
{
	miTrack_ID	=	0;
	gpApplication->CleanProject(0);
	mpTinyXMLDoc->Clear();
	
}
*/
void CKSXML_Read_Project::Read_Latest_Revision_From_Koblo(std::string sProject_UUID )
{
	
	mbOpen_Dialog = false;
	
	Reset_Project();
	Prepare_For_XML();
	
	// read latst revision
	std::string sProject;
	char psz[256];
	
	sprintf(psz, "/projects/%s/trunk/latest/markup.xml", sProject_UUID.c_str() );
	sProject = psz;
	
	Read_Project_From_Koblo( sProject );
	
	
}	
	
void CKSXML_Read_Project::Read_Latest_Revision_From_Koblo(tint32 iProjectID )
{
	
	mbOpen_Dialog = true;
	
	Reset_Project();
	Prepare_For_XML();
	
	// read latst revision
	std::string sProject;
	char psz[256];
	
	//if the project is uploaded
	gpApplication->Get_Project_UUID().c_str();
	
	sprintf(psz, "/projects/%d/trunk/latest/markup.xml", iProjectID);
	sProject = psz;
	
	Read_Project_From_Koblo( sProject );
	
		
	
	
}

void CKSXML_Read_Project::Read_Project_From_Koblo(std::string sProject )
{
	
	tchar* pszBuff = NULL;
	tint32 iOutLen = 0;
	ine::IINetUtil::GetWebFile(NULL, "koblo.com", sProject.c_str(), &iOutLen, &pszBuff);
	
	
	
	if ((pszBuff) && (iOutLen > 0)) {
		
		// parse XML file in to TinyXml object tree
		mpTinyXMLDoc->Parse(pszBuff);
		
		// pass the TinyXML DOM in to the DAW data structure
		Pass_The_Project_Tag( mpTinyXMLDoc );
		
		if(mbOpen_Dialog){
		
			tchar pszDefaultFolder[1024];
			gpApplication->GetDefaultProjectFolder(pszDefaultFolder);
			
			// get default folder
			tchar pszDefault_Folder[1024];
			gpApplication->GetDefaultProjectFolder(pszDefault_Folder);
			
			std::string sProject_Name = gpApplication->Project_Name();
			
			// open new project dialog
			tchar pszProject_Folder[1024];
			CAutoDelete<ge::ISaveAsDialog> pDlg(ge::ISaveAsDialog::Create());

			// Don't browse into OS X bundles (special kind of folders)
			pDlg->SetBundleBehaviour(1);

			pDlg->DoDialog(pszProject_Folder, pszDefault_Folder, "", "", sProject_Name.c_str() );
			std::string sProject_Folder = pszProject_Folder;
			
			// user canceled operation
			if (pszProject_Folder[0] == 0){
				// User cancelled - clean
				Reset_Project();
				Prepare_For_XML();
			}
			// continue
			else {
				// update path's and names for files and folders
				gpApplication->Update_Project_Name(sProject_Folder);

			//	tbool bExists = IFile::Exists(sProject.c_str() , &bIsFolder);
				
				// create new folders for the project
				gpApplication->Create_Folders();
					
				// create a new project file
				gpApplication->Create_Project_File();
				
				// save project 
				gpApplication->Write_XML_File_To_Disk(pszBuff);
				
				// prepare samples for download and decompression
				Prepare_Samples();
				
				// decompress takes
				Decompress_Takes();
				
				// download takes
				Download_Takes();

			}
		}
		else{
			// save project 
			gpApplication->Write_XML_File_To_Disk(pszBuff);
			
			// prepare samples for download and decompression
			Prepare_Samples();
						
			// decompress takes
			Decompress_Takes();

			// download takes
			Download_Takes();
		}
			
			
	}
	ine::IINetUtil::ReleaseBuffer(&pszBuff);
	
	

}

void CKSXML_Read_Project::Takes_Downloaded()
{
// (lasse) no - 	bInsert_Regions = true;
	
	// decompress takes
//	Decompress_Takes();
	

}

void CKSXML_Read_Project::Takes_Decompressed()
{
	/* (lasse) no:
	if(bInsert_Regions)
		Insert_Regions();
	
	bInsert_Regions = false;
	*/
}

/*
void CKSXML_Read_Project::Import_Regions()
{
	
	
	std::list<CRegion_Data>::iterator itRegion_Data = mRegion_Data_List.begin();
	for (; itRegion_Data != mRegion_Data_List.end(); itRegion_Data++) {
		
		std::string uuid = (*itRegion_Data).Take_UUID() ;
		std::string sName = Get_Take_Screen_Name( uuid );
		
	//	if(sName.size() ) {
			
			
			CRegion_Data* pRegion_Data;
			
			pRegion_Data = &(*itRegion_Data);
			CImportAudioTask* pTask = new CImportAudioTask();
			
			pTask->Init_InsertAsRegionAfterImport(pRegion_Data);
			
			
			CAutoLock Lock(gpApplication->mMutex_Progress);
			gpApplication->mpProgressTasks->Add(pTask);
			gpApplication->Playback_InProgressTask();
			
	//	}
	}
			
	
}

*/


void CKSXML_Read_Project::Pass_The_Project_Tag( TiXmlNode* pParent )
{
	// if file is empty return
	if ( !pParent ) return;
	
	// if there is multiply <project> tags only read the first one
	tbool read = true;
	
	TiXmlNode* pChild;
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		if(pChild->Type() == TiXmlNode::ELEMENT && read){
			Read_Project_Object(pChild);
			read = false;
			
		}
	}
}

void CKSXML_Read_Project::Read_Project_Object(TiXmlNode* pParent)
{
	if ( !pParent ) return ;
	
	TiXmlAttribute* pAttrib	=	pParent->ToElement()->FirstAttribute();
	//  project uuid
	if(pAttrib) {
		std::string sProjectUUID =  pAttrib->Value() ;
		gpApplication->Set_Project_UUID(sProjectUUID);
	}
		
	// parse all childs
	TiXmlNode* pChild;
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		if(pChild->Type() == TiXmlNode::ELEMENT)
			Parse_Project_Object(pChild);
	}
}

void CKSXML_Read_Project::Parse_Project_Object(TiXmlNode* pParent)
{
	
	if (stricmp("name", pParent->Value()) == 0)
		Read_Project_Name(pParent);
	
	else if (stricmp("branch", pParent->Value()) == 0) {
		Read_Branch_Object(pParent->ToElement());
	}
	else if (stricmp("settings", pParent->Value()) == 0) {
		Read_Settings_Object(pParent->ToElement());
	}
	else if (stricmp("editing", pParent->Value()) == 0) {
		Parse_Edditing_Object(pParent->ToElement());
	}
	else if (stricmp("sample", pParent->Value()) == 0) {
		Read_Sample_Object(pParent->ToElement());
	}
	/*
	else if (stricmp("insert", pParent->Value()) == 0) {
		Read_Insert(pParent->ToElement());
	}
	 */
	
	else if (stricmp("track", pParent->Value()) == 0) {
		Parse_Track_Object(pParent->ToElement());
		
	}
	else if (stricmp("bus", pParent->Value()) == 0) {
		Read_Bus(pParent->ToElement());
	}
	else if (stricmp("master", pParent->Value()) == 0) {
		Read_Master(pParent->ToElement());
	}
	 
}

void CKSXML_Read_Project::Read_Branch_Object(TiXmlElement* pElement)
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

void CKSXML_Read_Project::Read_Sample_Object(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();
	CSample_Data Sample_Data;
	
	// set sample uuid
	if(pAttrib)		
		Sample_Data.Set_UUID(pAttrib->Value());
	
	TiXmlNode* pChild;
	TiXmlText* pText;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("name", pChild->Value()) == 0) {

				TiXmlNode* pName = pChild->FirstChild();
				if(pName) {
					pText = pName->ToText();
					if(pText)
						Sample_Data.Name(pText->Value());
				}
			}
			else if (stricmp("take", pChild->Value()) == 0) {
				
				CTake_Data* pTake_Data		=	Sample_Data.Get_Take_Data();
				// set take name
				pTake_Data->Screen_Name( Sample_Data.Name() );
				
				Read_Take_Object(pChild->ToElement(), pTake_Data);

			}
		}
	}
	
	// store sample data in list
	mSample_Data_List.push_back(Sample_Data);
}

void CKSXML_Read_Project::Read_Take_Object(TiXmlElement* pElement, CTake_Data* pTake_Data)
{

	if ( !pElement ) return ;
	
	// set take uuid
	TiXmlAttribute* pAttrib		=	pElement->FirstAttribute();
	if(pAttrib)		
		pTake_Data->Set_UUID(pAttrib->Value());   

	TiXmlNode* pChild;
	std::string s;
	
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			s = "";
			if (stricmp("description", pChild->Value()) == 0) {
				
				TiXmlNode* pTake_Description = pChild->FirstChild();
				if(pTake_Description) {
					TiXmlText* pText = pTake_Description->ToText();
					if(pText){
						s = pText->Value();
						pTake_Data->Set_Description(s);
					}
				}
			}
			else if (stricmp("mode", pChild->Value()) == 0) {
				
				TiXmlNode* pTake_Mode = pChild->FirstChild();
				if(pTake_Mode) {
					TiXmlText* pText = pTake_Mode->ToText();
					if(pText){
						s = pText->Value();
						pTake_Data->Mode(s);
					}
				}
			}
			else if (stricmp("url", pChild->Value()) == 0) {
				
				TiXmlNode* pUrl = pChild->FirstChild();
				if(pUrl) {
					TiXmlText* pText = pUrl->ToText();
					if(pText){
						s = pText->Value();
						pTake_Data->URL(s);
					}
				}
			}
			
			else if (stricmp("mp3", pChild->Value()) == 0) {
				
				TiXmlNode* pMp3_Url = pChild->FirstChild();
				if(pMp3_Url) {
					TiXmlText* pText = pMp3_Url->ToText();
					if(pText){
						s = pText->Value();
						pTake_Data->MP3_URL(s);
					}
				}
			}
		}
	}
}


void CKSXML_Read_Project::Parse_Track_Object(TiXmlElement* pElement)
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
				Set_DAW_Parameter(pChild, giTinyXml_Type_Int, giParam_Track_Info_SizeY, giSection_First_Track + iTrackID);
			}
			
			else if (stricmp("in", pChild->Value()) == 0) {
				Read_Track_In(pChild->ToElement(),  iTrackID);
			//	printf( "\n" );
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
				Set_DAW_Parameter(pElement, giTinyXml_Type_Float, giParam_ChAUX1 + iAux, giSection_First_Track + iTrackID, 10000.0f);

			}
			else if (stricmp("insert", pChild->Value() ) == 0) {
				Read_Track_Insert(pChild->ToElement(), iTrackID);
				
			}
			else if (stricmp("region", pChild->Value()) == 0) {
				Read_Track_Region(pChild->ToElement(),  iTrackID);
				
			}
		}
	}
	
	muiTrack++;

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

				Set_DAW_Parameter(pChild, giTinyXml_Type_Int, 0, 0);

			}
			else if (stricmp("mode", pChild->Value()) == 0) {

				Set_DAW_Parameter(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("gain", pChild->Value()) == 0) {
		//		printf( "input gain:  ");
		//		Set_DAW_Parameter(pChild, giTinyXml_Type_Float, giParam_ChOut, giSection_First_Track + iTrackID);
				
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
				Set_DAW_Parameter(pChild, giTinyXml_Type_Int, giParam_ChOut, giSection_First_Track + iTrackID);
			
			else if (stricmp("mode", pChild->Value()) == 0) 
				Set_DAW_Parameter(pChild, giTinyXml_Type_Int, giParam_ChMode, giSection_First_Track + iTrackID);
			
			else if (stricmp("volume", pChild->Value()) == 0) 
				Set_DAW_Parameter(pChild, giTinyXml_Type_Float, giParam_ChVol, giSection_First_Track + iTrackID, 10000.0f);
			
			else if (stricmp("pan", pChild->Value()) == 0) 
				Set_DAW_Parameter(pChild, giTinyXml_Type_Float, giParam_ChPannerLeftRight, giSection_First_Track + iTrackID, 10000.0f);
			
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

				Set_DAW_Parameter(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("product", pChild->Value()) == 0) {

				Set_DAW_Parameter(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("url", pChild->Value()) == 0) {

				Set_DAW_Parameter(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("bypass", pChild->Value()) == 0) {

				Set_DAW_Parameter(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("settings", pChild->Value()) == 0) {
				Read_Insert_Settings(pChild->ToElement());

			}
		}
	}
}



void CKSXML_Read_Project::Read_Track_Region(TiXmlElement* pElement, tint32 iTrackID)
{
	if ( !pElement ) return ;
	
	CRegion_Data Region_Data;
	
	Region_Data.Track_ID(muiTrack);
	
	TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();

	// set region uuid
	if(pAttrib)	{
		Region_Data.Set_UUID(pAttrib->Value());
	}
	

	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("sample", pChild->Value()) == 0) {
				
				TiXmlNode* pValue = pChild->FirstChild();
				
				if(pValue) {
					Region_Data.Sample_UUID(pValue->Value());
				}
			}
			
			else if(stricmp("take", pChild->Value()) == 0) {
				
				TiXmlNode* pValue = pChild->FirstChild();
				
				if(pValue) {
					Region_Data.Take_UUID(pValue->Value());
				}
			}
			
			else if(stricmp("position", pChild->Value()) == 0) {
				
				TiXmlNode* pValue = pChild->FirstChild();
				
				if(pValue) {
					std::string sValue = pValue->Value();
					tuint uiValue = atoi(sValue.c_str());
					Region_Data.Possition(uiValue);
				}
			}
			
			else if(stricmp("offset", pChild->Value()) == 0) {

				TiXmlNode* pValue = pChild->FirstChild();
				
				if(pValue) {
					std::string sValue = pValue->Value();
					tuint uiValue = atoi(sValue.c_str());
					Region_Data.Sample_Offset(uiValue);
				}

			}
			else if(stricmp("duration", pChild->Value()) == 0) {

				TiXmlNode* pValue = pChild->FirstChild();
				
				if(pValue) {
					std::string sValue = pValue->Value();
					tuint uiValue = atoi(sValue.c_str());
					Region_Data.Sample_Duration(uiValue);
				}

			}
			else if (stricmp("volume", pChild->Value()) == 0) {
				
				TiXmlNode* pValue = pChild->FirstChild();
				
				if(pValue) {
					std::string sValue = pValue->Value();
					tfloat32 fValue = atof(sValue.c_str());
					Region_Data.Volume(fValue);
				}
				
			}
			else if (stricmp("fade", pChild->Value()) == 0) {
				
				Read_Track_Region_Fade(pChild->ToElement(), &Region_Data);
				
			}
		}
	}
	
	mRegion_Data_List.push_back(Region_Data);
}

void CKSXML_Read_Project::Read_Track_Region_Fade(TiXmlElement* pElement, CRegion_Data* pRegion_Data)
{
	if ( !pElement ) return ;
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("in", pChild->Value()) == 0) {
				
				TiXmlNode* pValue = pChild->FirstChild();
				
				if(pValue) {
					std::string sValue = pValue->Value();
					tuint uiValue = atoi(sValue.c_str());
					pRegion_Data->Fade_In_Duration(uiValue);
				}

			}
			else if (stricmp("out", pChild->Value()) == 0) {

				TiXmlNode* pValue = pChild->FirstChild();
				
				if(pValue) {
					std::string sValue = pValue->Value();
					tuint uiValue = atoi(sValue.c_str());
					pRegion_Data->Fade_Out_Duration(uiValue);
				}

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

				Set_DAW_Parameter(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("mode", pChild->Value()) == 0) {

				Set_DAW_Parameter(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("volume", pChild->Value()) == 0) {

				Set_DAW_Parameter(pChild, giTinyXml_Type_Float, 0, 0);

			}
			else if (stricmp("pan", pChild->Value()) == 0) {

				Set_DAW_Parameter(pChild, giTinyXml_Type_Float, 0, 0);

			}
			else if (stricmp("solo", pChild->Value()) == 0) {

				Set_DAW_Parameter(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("mute", pChild->Value()) == 0) {

				Set_DAW_Parameter(pChild, giTinyXml_Type_String, 0, 0);

			}
			else if (stricmp("arm", pChild->Value()) == 0) {

				Set_DAW_Parameter(pChild, giTinyXml_Type_String, 0, 0);

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

				Set_DAW_Parameter(pChild, giTinyXml_Type_Float, 0, 0);

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
				Set_DAW_Parameter(pChild, giTinyXml_Type_Float, 0, 0);

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

				Set_DAW_Parameter(pChild, giTinyXml_Type_Float, 0, 0);

			}
			else if (stricmp("pan", pChild->Value()) == 0) {

				Set_DAW_Parameter(pChild, giTinyXml_Type_Float, 0, 0);

			}
		}
	}
}







void CKSXML_Read_Project::Prepare_Samples()
{

	std::list<CSample_Data>::iterator it = mSample_Data_List.begin();
	for (; it != mSample_Data_List.end(); it++) {
		
		CSample_Data* pSample_Data = &(*it);
		Import_Takes( pSample_Data );
	}
}


void CKSXML_Read_Project::Import_Takes(CSample_Data* pSample_Data)
{
	
	//! notice only one take pr sample supported at the moment	
	CTake_Data* pTake_Data = pSample_Data->Get_Take_Data();
	Import_Take( pTake_Data );
	
}

void CKSXML_Read_Project::Import_Take(CTake_Data* pTake)
{
//	std::string sUUID = pTake->Get_UUID();
	
	// check if take is in Wave folder
	if( pTake->In_Folder(gpApplication->Wave_File_Folder(), ".wav") ){
		// add take to insert que
		mInsert_Que.push_back(pTake);
		// check if the pict files is in the folder
		if(pTake->Needs_Pict_Files() )
			mCreate_Pict_File_Que.push_back(pTake);
			
	}
	// check if the take is in the "OGG Folder"
	else if( pTake->In_Folder(gpApplication->OGG_File_Folder(), ".ogg") )
		mDecompress_Que.push_back(pTake);

	// add take to the download que
	else {
		mDownload_Que.push_back(pTake);
		// (lasse) we can't add to decompress queue yet - download task will do that
		/*
		// ... and to decompress queue
		mDecompress_Que.push_back(pTake);
		*/
		// .. (lasse)
	}

	
}



void CKSXML_Read_Project::Download_Takes()
{
	CDownloadTask* pTask = new CDownloadTask();
	tbool bInitOK = pTask->Init_Update(&mDownload_Que);
	if (bInitOK) {
		CAutoLock Lock(gpApplication->mMutex_Progress);
		gpApplication->mpProgressTasks->Add(pTask);
		gpApplication->Playback_InProgressTask();
	}
	else {
	
		std::string sReason = pTask->GetError();
		pTask->Destroy();
		std::string sErr = "Unable to queue download of takes.\n";
		sErr += "Reason:\  ";
		sErr += sReason;
		gpApplication->ShowMessageBox_NonModal(sErr.c_str(), "Error downloading");
	//	Setup_Track_Editor();
	}

}


void CKSXML_Read_Project::Download_Take(CTake_Data* Take_Data)
{
	/* (lasse) not needed
	std::string sURL = Take_Data->URL();
	// do download here
	// download to "OGG Files" folder
	// when compleated add files to decompressing que:
	// mDecompress_Que.push_back(pTake_Data);
	*/
}

void CKSXML_Read_Project::Decompress_Takes()
{
	std::list<CTake_Data*>::iterator it = mDecompress_Que.begin();
	for (; it != mDecompress_Que.end(); it++) {
		if (!Decompress_Take( (*it) )) {
			std::string sErr = gpApplication->Extended_Error();
			ge::IWindow::ShowMessageBox(sErr.c_str(), "Error");
		}
	}

	// (lasse) Not here, must be only once and very last - queued from CDownloadTask
	/*
	// Queue insertion of regions
	CInsertRegionsTask* pInsertRegionsTask = new CInsertRegionsTask();
	pInsertRegionsTask->Init();
	{
		CAutoLock Lock(gpApplication->mMutex_Progress);
		gpApplication->mpProgressTasks->Add(pInsertRegionsTask);
		gpApplication->Playback_InProgressTask();
	}
	*/
	// .. (lasse)
}


tbool CKSXML_Read_Project::Decompress_Take(CTake_Data* pTake_Data)
{
	// (lasse) no - std::string sURL = Take_Data->URL();
	// do decompression here
	// decompress to "Wave Files" folder
	// when compleated add files to insert que:
	// mDecompress_Que.push_back(pTake_Data);
	
	std::string sFull_Path	=	gpApplication->OGG_File_Folder();
	sFull_Path				+=	pTake_Data->Get_UUID();
	sFull_Path				+= ".ogg";
	
	CKSFile_Item File_Item;
	File_Item.Import(sFull_Path);
	File_Item.Screen_Name(pTake_Data->Screen_Name());
	File_Item.Set_UUID( pTake_Data->Get_UUID() );

/*
	std::string sUUID = pTake_Data->Get_UUID();
	std::string sOgg = gpApplication->OGG_File_Folder() + sUUID + ".ogg";
	CKSFile_Item* pItem = new CKSFile_Item();
	pItem->Import(sOgg);
	pItem->Screen_Name(pTake_Data->Screen_Name());
*/
	CImportAudioTask* pTask = new CImportAudioTask();
//<<<<<<< HEAD:Products/Koblo_Studio/trunk/source/Internet_Features/CKSXML_Read_Project.cpp
	tbool bInitOK = pTask->Init(&File_Item);
//=======

//	tbool bInitOK = pTask->Init(pItem);
//>>>>>>> 03875535f10ddd4155266eac2be39959a9d16b99:Products/Koblo_Studio/trunk/source/Internet_Features/CKSXML_Read_Project.cpp
	if (bInitOK) {
		CAutoLock Lock(gpApplication->mMutex_Progress);
		gpApplication->mpProgressTasks->Add(pTask);
		gpApplication->Playback_InProgressTask();
		return true;
	}
	else {
		/*
<<<<<<< HEAD:Products/Koblo_Studio/trunk/source/Internet_Features/CKSXML_Read_Project.cpp
	//	std::string sErr = pTask->GetError();
	//	ge::IWindow::ShowMessageBox(sErr.c_str(), "Error");
	//	gpApplication->Extended_Error(sErr);
		// if nothing to decompress continue
=======
		 */
		std::string sErr = pTask->GetError();
//>>>>>>> 03875535f10ddd4155266eac2be39959a9d16b99:Products/Koblo_Studio/trunk/source/Internet_Features/CKSXML_Read_Project.cpp
		pTask->Destroy();
		gpApplication->Extended_Error(sErr);
		//ge::IWindow::ShowMessageBox(sErr.c_str(), "Error");
		return false;
	}
}



void CKSXML_Read_Project::Create_Pict_Files()
{
	std::list<CTake_Data*>::iterator it = mCreate_Pict_File_Que.begin();
	for (; it != mCreate_Pict_File_Que.end(); it++) {
		Create_Pict_File( (*it) );
	}
	
}

void CKSXML_Read_Project::Create_Pict_File(CTake_Data* Take_Data)
{
	//!!! TO DO make code that creates missing pict files here
}


void CKSXML_Read_Project::Insert_Takes()
{
	gpApplication->Stop_Timer();
	std::list<CTake_Data*>::iterator it = mInsert_Que.begin();
	for (; it != mInsert_Que.end(); it++) {
		gpApplication->AddClipToList( (*it) );
	}
	

}

void CKSXML_Read_Project::Insert_Regions()
{

	//dont update graphics until all regions are loaded
	gpApplication->Stop_Timer();
	
	std::list<CRegion_Data>::iterator itRegion_Data = mRegion_Data_List.begin();
	for (; itRegion_Data != mRegion_Data_List.end(); itRegion_Data++) {
		
		std::string uuid = (*itRegion_Data).Take_UUID() ;
		
		std::string sName = Get_Take_Screen_Name( uuid );
		
		if(sName.size() ) {
		
			
			
			gpDSPEngine->CreateRegion( sName.c_str(), 
									  (*itRegion_Data).Track_ID(),
									  (*itRegion_Data).Possition(), 
									  (*itRegion_Data).Sample_Offset(),
									  (*itRegion_Data).Sample_Duration(),
									  (*itRegion_Data).Fade_In_Duration(),
									  (*itRegion_Data).Fade_Out_Duration(),
									  (*itRegion_Data).Volume()	);
			
			
			
			
		}

	}
	gpApplication->Start_Timer();
	
}

std::string CKSXML_Read_Project::Get_Take_Screen_Name(std::string sTake_UUID)
{
	CTake_Data* pTake_Data = Get_Take(sTake_UUID);
	
	if(pTake_Data)
		return pTake_Data->Screen_Name();
	
	return "";
	
	/*
	std::list<CSample_Data>::iterator it = mSample_Data_List.begin();
	for (; it != mSample_Data_List.end(); it++) {
		
		
		CTake_Data* pTake_Data = (*it).Get_Take_Data(sUUID);
		
		if(pTake_Data)
			return pTake_Data->Screen_Name();
	}
	return "";
	 */
}

CTake_Data* CKSXML_Read_Project::Get_Take(std::string sTake_UUID)
{
	
	std::list<CSample_Data>::iterator it = gpApplication->Get_Sample_Data_List_Begin();
	for (; it != gpApplication->Get_Sample_Data_List_End(); it++) {
		
		CTake_Data* pTake_Data = (*it).Get_Take_Data(sTake_UUID);
		
		if(pTake_Data)
			return pTake_Data;
	}
	return NULL;
}

tbool CKSXML_Read_Project::Check_For_Newer_Revision(tint32 iProject_ID)
{
	mbNew_Revision = false;
	// Check DOM downloaded to see if there is a newer one
	Check_Project( mpTinyXMLDoc );
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












