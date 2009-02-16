
#include "KSOS.h"


CKSXML_Write_Project::CKSXML_Write_Project()
{
	
	
}

CKSXML_Write_Project::~CKSXML_Write_Project()
{
	
}

std::string CKSXML_Write_Project::Get_Internal_Data_As_XML()
{
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	TiXmlDocument *pDoc		=	new TiXmlDocument("koblo_doc");
	pDoc->LinkEndChild( decl );
	
	Write_Project(pDoc);
	
	// convert pDoc to a std::string
	TiXmlPrinter printer;
	pDoc->Accept(&printer);
	std::string xml_str =  printer.CStr();
	
	return xml_str;
}

tbool CKSXML_Write_Project::Save_Project_As_XML_File_To_Disk()
{
	TiXmlDeclaration* decl	=	new TiXmlDeclaration( "1.0", "UTF-8", "" );
	TiXmlDocument *pDoc		=	new TiXmlDocument("koblo_doc");
	pDoc->LinkEndChild( decl );
	
	Write_Project(pDoc);
	
	// convert pDoc to a std::string
	TiXmlPrinter printer;
	pDoc->Accept(&printer);
	std::string xml_str =  printer.CStr();
	
	// missing code 
//	std::string sFileName = gpApplication->Project_Name() + ".xml";
	
	printf("Save_Project_As_XML_File_To_Disk()\n");
	std::string sProject_Name	= gpApplication->Project_Name();
	std::string sProject_Folder = gpApplication->Project_Folder();
	std::string sProject		=  sProject_Folder + sProject_Name + ".xml";
	
	printf("sProject_Name = %s \n", sProject_Name.c_str());
	printf("sProject_Folder = %s \n", sProject_Folder.c_str());
	printf("sProject = %s \n", sProject.c_str());
	
	CAutoDelete<IFile> pfile(IFile::Create());
	if (pfile->Open(sProject.c_str(), IFile::FileWrite)) {
		pfile->Write(xml_str.c_str(), xml_str.length());
		return true;
	}
	return false;
}

void CKSXML_Write_Project::Upload_Project_As_XML_File_To_Koblo( tint32 iProjectID)
{
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	TiXmlDocument *pDoc		=	new TiXmlDocument("koblo_doc");
	pDoc->LinkEndChild( decl );
	

	Write_Project(pDoc);
	
	// convert pDoc to a std::string
	TiXmlPrinter printer;
	pDoc->Accept(&printer);
	std::string xml_str =  printer.CStr();
	
	//------------------- DUMMY CODE ----------------------
	
	// print to console
//	printf(xml_str.c_str());
	
	// write file to disk
	pDoc->SaveFile("xml_doc.xml");
	
	//------------------- REAL CODE INCOMPLETE ----------------------
	
	//Get Project ID
	tint32 iProject_ID = gpApplication->GetGlobalParm(giParamID_Project_ID, giSectionGlobal);
	
	char psz[128];
	sprintf(psz, "branches/%d/revisions", iProject_ID);
	std::string str = psz;
	/*
	 curl -u "user@koblo.com:password" \
	 -F "commit[description]= Track names added" \
	 -F "commit[markup]=</Users/maxgronlund/Documents/Koblo_Software/Products/Koblo_Studio/trunk/build/Debug/xml_doc.xml" \
	 http://koblo.com/branches/1/revisions.xml
	 */
/*
	tchar* pszBuff = NULL;
	tint32 iOutLen = 0;
	ine::IINetUtil::GetWebFile("userid=name.com&password=pswrd", "koblo.com", str.c_str(), &iOutLen, &pszBuff);
*/

}

void CKSXML_Write_Project::Write_Project(TiXmlDocument* pDoc)
{
	
	Add_Comment(pDoc, "Koblo Studio music project XML format");
	Add_Comment(pDoc, "most ids are originally returned from the website, and are globally unique (between different projects)");
	Add_Comment(pDoc, "ids are always specified as attributes");
	Add_Comment(pDoc, "all times and track positions are specified in sample points");
	
	TiXmlElement* pProject = new TiXmlElement( "project" );

	
	pProject->SetAttribute("uuid", gpApplication->Get_Project_UUID().c_str());
/*	
	pProject->SetAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
	pProject->SetAttribute("xsi:noNamespaceSchemaLocation","http://koblo.com/schemas/koblo_project.xsd");
 */
	pDoc->LinkEndChild( pProject );
	
	// name
	TiXmlElement* pName = new TiXmlElement( "name" );
	TiXmlText* pNameTxt = new TiXmlText(gpApplication->Project_Name().c_str());
	pName->LinkEndChild( pNameTxt );
	pProject->LinkEndChild( pName );
	
/*	// description
	TiXmlElement* pDescription = new TiXmlElement( "description" );
	TiXmlText* pDescriptionTxt = new TiXmlText(gpApplication->Get_Branch_Description().c_str());
	pDescription->LinkEndChild( pDescriptionTxt );
	pBranch->LinkEndChild( pDescription );
*/	
	Write_Branch( pProject);
	Write_Settings( pProject);
	Write_Editing( pProject);
	Write_Samples(pProject);
	Write_Tracks(pProject);
	Write_Busses(pProject);
	
}

void CKSXML_Write_Project::Write_Branch(TiXmlElement* pParent)
{
	
	Add_Comment(pParent, "information about the branch");
	
	TiXmlElement* pBranch = new TiXmlElement( "branch" );
	// Branch ID
//	tint32 iBranch_ID = gpApplication->GetGlobalParm(giParamID_Branch_ID, giSectionGlobal);
	pBranch->SetAttribute("uuid",gpApplication->Get_Branch_UUID().c_str());
	pParent->LinkEndChild( pBranch );
	
	// name
	TiXmlElement* pName = new TiXmlElement( "name" );
	TiXmlText* pNameTxt = new TiXmlText(gpApplication->Get_Branch_Name().c_str());
	pName->LinkEndChild( pNameTxt );
	pBranch->LinkEndChild( pName );
	
	// description
	TiXmlElement* pDescription = new TiXmlElement( "description" );
	TiXmlText* pDescriptionTxt = new TiXmlText(gpApplication->Get_Branch_Description().c_str());
	pDescription->LinkEndChild( pDescriptionTxt );
	pBranch->LinkEndChild( pDescription );
	
	// revision
	char pszBuff [64];
	sprintf(pszBuff, "%d", gpApplication->Branch_Revision());
	TiXmlElement* pRevision = new TiXmlElement( "revision" );
	TiXmlText* pRevisionTxt = new TiXmlText(pszBuff);
	pRevision->LinkEndChild( pRevisionTxt );
	pBranch->LinkEndChild( pRevision );
	
	
}

//----------------------------------------------------------------
// settings
//----------------------------------------------------------------

void CKSXML_Write_Project::Write_Settings(TiXmlElement* pParent)
{
	Add_Comment(pParent, "overall project settings");
	
	TiXmlElement* pSettings = new TiXmlElement( "settings" );
	pParent->LinkEndChild( pSettings );
	

	char pszBuff [64];
	
	// Samplerate
	sprintf(pszBuff, "%d", gpApplication->GetSampleRate());
	TiXmlElement* pSampleRate = new TiXmlElement( "samplerate" );
	TiXmlText* pSampleRateTxt = new TiXmlText(pszBuff);
	pSampleRate->LinkEndChild( pSampleRateTxt );
	pSettings->LinkEndChild( pSampleRate );
	
	//Tempo
	tfloat32 fTempo = (tfloat32)gpApplication->GetGlobalParm(giParamID_KS_Tempo, giSectionGlobal) / 10.0f;
	sprintf(pszBuff, "%f", fTempo);
	TiXmlElement* pTempo = new TiXmlElement( "tempo" );
	TiXmlText* pTempoTxt = new TiXmlText(pszBuff);
	pTempo->LinkEndChild( pTempoTxt );
	pSettings->LinkEndChild( pTempo );
	
	TiXmlElement* pSignatur = new TiXmlElement( "signature" );;
	pSettings->LinkEndChild( pSignatur );
	Write_Signature(pSignatur);
	
	TiXmlElement* pLicense = new TiXmlElement( "license" );;
	pSettings->LinkEndChild( pLicense );
	Write_License(pLicense);
	
	
}

void CKSXML_Write_Project::Write_Signature(TiXmlElement* pParent)
{
	tuint32 uiSignatur = gpApplication->GetGlobalParm(giParamID_KS_Time_Signature, giSectionGlobal);
	tuint uiNumerator;
	tuint uiDenominator;
	
	switch(uiSignatur){
			
		case giSignature2_4:{
			uiNumerator			= 2;
			uiDenominator		= 4;
			break;
		}
		case giSignature3_4:{
			uiNumerator			= 3;
			uiDenominator		= 4;
			break;
		}
		default:
		case giSignature4_4:{
			uiNumerator			= 4;
			uiDenominator		= 4;
			break;
		}
		case giSignature6_8:{
			uiNumerator			= 6;
			uiDenominator		= 8;
			break;
		}
		case giSignature9_8:{
			uiNumerator			= 9;
			uiDenominator		= 8;
			break;
		}
		case giSignature12_8:{
			uiNumerator			= 12;
			uiDenominator		= 8;
			break;
		}
	}
			
		
	char pszBuff [64];
	// numerator
	sprintf(pszBuff, "%d", uiNumerator);
	TiXmlElement* pNumerator = new TiXmlElement( "numerator" );
	TiXmlText* pNumeratorTxt = new TiXmlText(pszBuff);
	pNumerator->LinkEndChild( pNumeratorTxt );
	pParent->LinkEndChild( pNumerator );
	
	//Tempo
	sprintf(pszBuff, "%d", uiDenominator);
	TiXmlElement* pDenominator = new TiXmlElement( "denominator" );
	TiXmlText* pDenominatorTxt = new TiXmlText(pszBuff);
	pDenominator->LinkEndChild( pDenominatorTxt );
	pParent->LinkEndChild( pDenominator );
	
}

void CKSXML_Write_Project::Write_License(TiXmlElement* pParent)
{
	
	std::string s = Create_License_String();
	TiXmlText* pLicenseTxt = new TiXmlText(s.c_str());
	pParent->LinkEndChild( pLicenseTxt );
	
	
	
}

std::string CKSXML_Write_Project::Create_License_String()
{
	char pszBuff [64];
	tuint uiTest	=	 gpApplication->GetGlobalParm(giParamID_CC_License_Type, giSectionGUI);
	
	switch(uiTest){
			
		case giCC_By:{
			sprintf(pszBuff, "by");
			break;
		}
		case giCC_By_Sa:{
			sprintf(pszBuff, "by-sa");
			break;
		}
		case giCC_By_Nd:{
			sprintf(pszBuff, "by-nd");
			break;
		}
			
		case giCC_By_Nc:{
			sprintf(pszBuff, "by-nc");
			break;
		}
		case giCC_By_Nc_Sa:{
			sprintf(pszBuff, "by-nc-sa");
			break;
		}
		case giCC_By_Nc_Nd:{
			sprintf(pszBuff, "by-nc-nd");
			break;
		}
	}
	
	
	return pszBuff;
	
}

//----------------------------------------------------------------
// editing
//----------------------------------------------------------------

void CKSXML_Write_Project::Write_Editing(TiXmlElement* pParent)
{
	Add_Comment(pParent, "editing settings. these are things that would typically not be moved/merged between branches");
	char pszBuff [64];
	
	TiXmlElement* pEditing = new TiXmlElement( "editing" );
	pParent->LinkEndChild( pEditing );
	
	// tool
	TiXmlElement* pTool = new TiXmlElement( "tool" );;
	pEditing->LinkEndChild( pTool );
	Write_Tool(pTool);
	
	// zoom
	tint iVal = gpApplication->GetGlobalParm(giParamID_Zoom, giSectionGUI);
	sprintf(pszBuff, "%d", iVal);
	TiXmlElement* pZoom = new TiXmlElement( "zoom" );
	TiXmlText* pZoomTxt = new TiXmlText(pszBuff);
	pZoom->LinkEndChild( pZoomTxt );
	pEditing->LinkEndChild( pZoom );
	
	// grid
	iVal = gpApplication->GetGlobalParm(giParamID_Grid, giSectionGUI);
	TiXmlElement* pGrid = new TiXmlElement( "grid" );
	iVal ? sprintf(pszBuff, "on") : sprintf(pszBuff, "off");
	TiXmlText* pGridTxt = new TiXmlText(pszBuff);
	pGrid->LinkEndChild( pGridTxt );
	pEditing->LinkEndChild( pGrid );
	
	// snap
//	iVal = gpApplication->GetGlobalParm(giParamID_KS_Snap_To, giSectionGlobal);
	TiXmlElement* pSnap = new TiXmlElement( "snap" );
	pEditing->LinkEndChild( pSnap );
	Write_Snap(pSnap);
	
/*	
	iVal ? sprintf(pszBuff, "on") : sprintf(pszBuff, "off");
	TiXmlText* pSnapTxt = new TiXmlText(pszBuff);
	pSnap->LinkEndChild( pSnapTxt );
	pEditing->LinkEndChild( pSnap );
*/	
	// waves
	iVal = gpApplication->GetGlobalParm(giParamID_Show_Waveform, giSectionGUI);
	TiXmlElement* pWaves = new TiXmlElement( "waves" );
	iVal ? sprintf(pszBuff, "on") : sprintf(pszBuff, "off");
	TiXmlText* pWavesTxt = new TiXmlText(pszBuff);
	pWaves->LinkEndChild( pWavesTxt );
	pEditing->LinkEndChild( pWaves );
	
	// fades
	iVal = gpApplication->GetGlobalParm(giParamID_Show_Fade, giSectionGUI);
	TiXmlElement* pFades = new TiXmlElement( "fades" );
	iVal ? sprintf(pszBuff, "on") : sprintf(pszBuff, "off");
	TiXmlText* pFadesTxt = new TiXmlText(pszBuff);
	pFades->LinkEndChild( pFadesTxt );
	pEditing->LinkEndChild( pFades );
	
	// loop
	TiXmlElement* pLoop = new TiXmlElement( "loop" );;
	pEditing->LinkEndChild( pLoop );
	Write_Loop(pLoop);
	
	// windows
	TiXmlElement* pWindows = new TiXmlElement( "windows" );;
	pEditing->LinkEndChild( pWindows );
	Write_Windows(pWindows);
	
	
}

void CKSXML_Write_Project::Write_Tool(TiXmlElement* pParent)
{
	char pszBuff [64];
	tuint uiTest	=	 gpApplication->GetGlobalParm(giParamID_Tool_Selected, giSectionGUI);
	
	
	switch(uiTest){
			
		case giTool_Hand:{
			sprintf(pszBuff, "hand");
			break;
		}
		case giTool_Trim:{
			sprintf(pszBuff, "trim");
			break;
		}
		case giTool_Select:{
			sprintf(pszBuff, "select");
			break;
		}
		case giTool_Cut:{
			sprintf(pszBuff, "cut");
			break;
		}
	}
	
	// text
	TiXmlText* pToolTxt = new TiXmlText(pszBuff);
	pParent->LinkEndChild( pToolTxt );
}

void CKSXML_Write_Project::Write_Snap(TiXmlElement* pParent)
{
	char pszBuff [64];
	tuint uiTest = gpApplication->GetGlobalParm(giParamID_KS_Snap_To, giSectionGlobal);
	
	
	switch(uiTest){
			
		case 32:{
			sprintf(pszBuff, "1/1");
			break;
		}
		case 16:{
			sprintf(pszBuff, "1/2");
			break;
		}
		case 8:{
			sprintf(pszBuff, "1/4");
			break;
		}
		case 4:{
			sprintf(pszBuff, "1/8");
			break;
		}
		case 2:{
			sprintf(pszBuff, "1/16");
			break;
		}
		case 1:{
			sprintf(pszBuff, "1/32");
			break;
		}
		default : break;
	}
	// text
	TiXmlText* pSnapTxt = new TiXmlText(pszBuff);
	pParent->LinkEndChild( pSnapTxt );
}

void CKSXML_Write_Project::Write_Loop(TiXmlElement* pParent)
{
	char pszBuff [64];

	// on
	tint32 iVal = gpApplication->GetGlobalParm(giParamID_Loop_On, giSectionGlobal);
	TiXmlElement* pActive = new TiXmlElement( "active" );
	iVal ? sprintf(pszBuff, "on") : sprintf(pszBuff, "off");
	TiXmlText* pActiveTxt = new TiXmlText(pszBuff);
	pActive->LinkEndChild( pActiveTxt );
	pParent->LinkEndChild( pActive );
	
	// start
	iVal = gpApplication->GetGlobalParm(giParamID_Loop_Start, giSectionGlobal);
	sprintf(pszBuff, "%d", iVal);
	TiXmlElement* pStart = new TiXmlElement( "start" );
	TiXmlText* pStartTxt = new TiXmlText(pszBuff);
	pStart->LinkEndChild( pStartTxt );
	pParent->LinkEndChild( pStart );
	
	// end
	iVal = gpApplication->GetGlobalParm(giParamID_Loop_End, giSectionGlobal);
	sprintf(pszBuff, "%d", iVal);
	TiXmlElement* pEnd = new TiXmlElement( "end" );
	TiXmlText* pEndTxt = new TiXmlText(pszBuff);
	pEnd->LinkEndChild( pEndTxt );
	pParent->LinkEndChild( pEnd );
	
	
}

void CKSXML_Write_Project::Write_Windows(TiXmlElement* pParent)
{
	tbool bShow;
	
	// edit
	TiXmlElement* pEdit = new TiXmlElement( "edit" );
	pParent->LinkEndChild( pEdit );
	bShow = (gpApplication->GetModule()->GetHost()->IsWindowVisible(giMain_Window) != 0);
	Write_Window( pEdit, giParamID_Show_Track_Window, bShow);
	
	// mix
	TiXmlElement* pMix = new TiXmlElement( "mix" );
	pParent->LinkEndChild( pMix );
	bShow = (gpApplication->GetModule()->GetHost()->IsWindowVisible(giMix_Window) != 0);
	Write_Window( pMix, giParamID_Show_Mix_Window, bShow);
	
	// rack
	TiXmlElement* pRack = new TiXmlElement( "rack" );
	pParent->LinkEndChild( pRack );
	bShow = (gpApplication->GetModule()->GetHost()->IsWindowVisible(giRack_Window) != 0);
	Write_Window( pRack, giParamID_Show_AUX_Window, bShow);
}

void CKSXML_Write_Project::Write_Window(TiXmlElement* pParent, tint32 iWindow, tbool bShow)
{
	char pszBuff [64];
	tint32 iVal;
	
	// show
	std::string sText		=	bShow? "on": "off";
	TiXmlElement* pShow		=	new TiXmlElement( "show");
	TiXmlText* pShowTxt		=	new TiXmlText(sText.c_str() );
	pShow->LinkEndChild( pShowTxt );
	pParent->LinkEndChild( pShow );
	
	
	// layer
	
	iVal = 1;//gpApplication->GetGlobalParm(iWindow, giSectionGUI);
	sprintf(pszBuff, "%d", iVal);
	TiXmlElement* pLayer = new TiXmlElement( "layer" );
	TiXmlText* pLayerTxt = new TiXmlText(pszBuff);
	pLayer->LinkEndChild( pLayerTxt );
	pParent->LinkEndChild( pLayer );
	
	// position
	TiXmlElement* pPosition = new TiXmlElement( "position" );;
	pParent->LinkEndChild( pPosition );
	Write_Window_Position(pPosition, 100, 100);
	
	// size
	TiXmlElement* pSize = new TiXmlElement( "size" );;
	pParent->LinkEndChild( pSize );
	Write_Window_Size(pSize, 800, 600);
//	Add_Comment(pParent, "the order of tracks is considered an editing setting, and is specified here");
	
}


void CKSXML_Write_Project::Write_Window_Position(TiXmlElement* pParent, tuint uiPosX, tuint uiPosY)
{
	char pszBuff [64];
	
	// x
	sprintf(pszBuff, "%d", uiPosX);
	TiXmlElement* pX = new TiXmlElement( "x" );
	TiXmlText* pXTxt = new TiXmlText(pszBuff);
	pX->LinkEndChild( pXTxt );
	pParent->LinkEndChild( pX );
	
	// y
	sprintf(pszBuff, "%d", uiPosY);
	TiXmlElement* pY = new TiXmlElement( "y" );
	TiXmlText* pYTxt = new TiXmlText(pszBuff);
	pY->LinkEndChild( pYTxt );
	pParent->LinkEndChild( pY );
	
}
	
void CKSXML_Write_Project::Write_Window_Size(TiXmlElement* pParent, tuint uiSizeX, tuint uiSizeY)
{
	char pszBuff [64];
	
	// x
	sprintf(pszBuff, "%d", uiSizeX);
	TiXmlElement* pX = new TiXmlElement( "x" );
	TiXmlText* pXTxt = new TiXmlText(pszBuff);
	pX->LinkEndChild( pXTxt );
	pParent->LinkEndChild( pX );
	
	// y
	sprintf(pszBuff, "%d", uiSizeY);
	TiXmlElement* pY = new TiXmlElement( "y" );
	TiXmlText* pYTxt = new TiXmlText(pszBuff);
	pY->LinkEndChild( pYTxt );
	pParent->LinkEndChild( pY );
	
}

//----------------------------------------------------------------
// samples
//----------------------------------------------------------------
void CKSXML_Write_Project::Write_Samples(TiXmlElement* pParent)
{
	Add_Comment(pParent, "samples and their takes. only used takes are listed. a sample can be included that's not used on any track");
	
	std::list<CSample_Data>::iterator  itSample_Data = gpApplication->Get_Sample_Data_List_Begin();
	
	for (; itSample_Data != gpApplication->Get_Sample_Data_List_End(); itSample_Data++) {
		
		// sample tag with uuid as atribute
		TiXmlElement* pSample = new TiXmlElement( "sample" );
		pSample->SetAttribute("uuid", (*itSample_Data).Get_UUID().c_str());
		pParent->LinkEndChild( pSample );
		
		Write_Sample( pSample, &(*itSample_Data));
	}
}

void CKSXML_Write_Project::Write_Sample(TiXmlElement* pParent, CSample_Data* pSample_Data)
{
	TiXmlElement* pSample = new TiXmlElement("name" );
	TiXmlText* pSampleTxt = new TiXmlText(pSample_Data->Name().c_str());
	pSample->LinkEndChild( pSampleTxt );
	pParent->LinkEndChild( pSample );

	CTake_Data* pTake_Data = pSample_Data->Get_Take_Data();
	Write_Take(pParent, pTake_Data);
}


void CKSXML_Write_Project::Write_Take(TiXmlElement* pParent, CTake_Data* pTake_Data)
{
	// uuid
	TiXmlElement* pTake = new TiXmlElement( "take" );
	pTake->SetAttribute("uuid", pTake_Data->Get_UUID().c_str());
	pParent->LinkEndChild( pTake );
	
	
	// description
	TiXmlElement* pDescription = new TiXmlElement( "description" );
	TiXmlText* pDescriptionTxt = new TiXmlText(pTake_Data->Get_Description().c_str());
	pDescription->LinkEndChild( pDescriptionTxt );
	pTake->LinkEndChild( pDescription );
	
	// mode mono/ stereo
	TiXmlElement* pChannels = new TiXmlElement( "mode" );
	TiXmlText* pChannelsTxt = new TiXmlText(pTake_Data->Mode().c_str());
	pChannels->LinkEndChild( pChannelsTxt );
	pTake->LinkEndChild( pChannels );
	
	// url
	TiXmlElement* pURL = new TiXmlElement( "url" );
	TiXmlText* pURLTxt = new TiXmlText(pTake_Data->URL().c_str());
	pURL->LinkEndChild( pURLTxt );
	pTake->LinkEndChild( pURL );
	
	// mp3
	TiXmlElement* pMp3_URL = new TiXmlElement( "mp3" );
	TiXmlText* pMp3_URLTxt = new TiXmlText(pTake_Data->MP3_URL().c_str());
	pMp3_URL->LinkEndChild( pMp3_URLTxt );
	pTake->LinkEndChild( pMp3_URL );

}


void CKSXML_Write_Project::Write_Tracks(TiXmlElement* pParent)
{
	tint32 iNrTracks = gpApplication->Get_Number_Of_Tracks();//gpApplication->msStack.iNr_Of_Tracks;
	
	for(tint32 i = 0; i<iNrTracks; i++){
		
		// id
		tuint uiTrack = gpApplication->Get_Track_Id(i);
		
		
		CTrack_DSP*		pTrack_DSP		=	gpDSPEngine->GetTrack(uiTrack);

		// track uuid
		TiXmlElement* pTrack = new TiXmlElement( "track" );
		pTrack->SetAttribute("uuid", pTrack_DSP->Get_UUID().c_str());
		pParent->LinkEndChild( pTrack );
		
		// write track data
		Write_Track(pTrack, uiTrack);	
	}
}

void CKSXML_Write_Project::Write_Track(TiXmlElement* pParent, tuint uiTrack)
{
	char pszBuff [64];
	
	// name
	std::string str  = gpApplication->GetChannelName(uiTrack);
	TiXmlElement* pName = new TiXmlElement( "name" );
	TiXmlText* pNameTxt = new TiXmlText(str.c_str());
	pName->LinkEndChild( pNameTxt );
	pParent->LinkEndChild( pName );
	 
	// description
	TiXmlElement* pDescription = new TiXmlElement( "description" );
	TiXmlText* pDescriptionTxt = new TiXmlText("NA");
	pDescription->LinkEndChild( pDescriptionTxt );
	pParent->LinkEndChild( pDescription );
	
	
	// size
	TiXmlElement* pSize = new TiXmlElement( "size" );
	tint32 iVal = gpApplication->GetGlobalParm(giParam_Track_Info_SizeY, giSection_First_Track + uiTrack);
	sprintf(pszBuff, "%d", iVal);
	TiXmlText* pSizeTxt = new TiXmlText(pszBuff);
	pSize->LinkEndChild( pSizeTxt );
	pParent->LinkEndChild( pSize );
	
	// in
	TiXmlElement* pIn = new TiXmlElement( "in" );
	pParent->LinkEndChild( pIn );
	Write_Track_In(pIn, uiTrack);
	
	// out
	TiXmlElement* pOut = new TiXmlElement( "out" );
	pParent->LinkEndChild( pOut );
	Write_Track_Out(pOut, uiTrack);
	
	// inserts
	Write_Track_Inserts(pParent, uiTrack);
	
	// AUXs
	Write_AUXs(pParent, uiTrack);
	
	// regions
	Write_Track_Regions( pParent, uiTrack);
	
	
}

void CKSXML_Write_Project::Write_Track_In(TiXmlElement* pParent, tuint uiTrack)
{
//	CTrack_DSP*		pTrack_DSP		=	gpDSPEngine->GetTrack(uiTrack);
	
	// input
	char pszBuff [64];
	tint32 iVal = gpApplication->GetGlobalParm(giParam_ChIn, giSection_First_Track + uiTrack);
	sprintf(pszBuff, "%d", iVal);
	TiXmlElement* pIn = new TiXmlElement( "input" );
	TiXmlText* pInTxt = new TiXmlText(pszBuff);
	pIn->LinkEndChild( pInTxt );
	pParent->LinkEndChild( pIn );
	
	// mode
	iVal = gpApplication->GetGlobalParm(giParam_ChInMode, giSection_First_Track + uiTrack);
	iVal ? sprintf(pszBuff, "mono") : sprintf(pszBuff, "stereo");
	TiXmlElement* pMode = new TiXmlElement( "in" );
	TiXmlText* pModeTxt = new TiXmlText(pszBuff);
	pMode->LinkEndChild( pModeTxt );
	pParent->LinkEndChild( pMode );
	
	// gain
	iVal = gpApplication->GetGlobalParm(giParam_ChInGain, giSection_First_Track + uiTrack);
	tfloat fGain	=	(tfloat)iVal * 0.0001f;
	sprintf(pszBuff, "%f", fGain);
	TiXmlElement* pGain = new TiXmlElement( "gain" );
	TiXmlText* pGainTxt = new TiXmlText(pszBuff);
	pGain->LinkEndChild( pGainTxt );
	pParent->LinkEndChild( pGain );
	
}

void CKSXML_Write_Project::Write_Track_Out(TiXmlElement* pParent, tuint uiTrack)
{
	
	// output
	char pszBuff [64];
	tint32 iVal = gpApplication->GetGlobalParm(giParam_ChOut, giSection_First_Track + uiTrack);
	
	if(iVal < ParmIOOffsetBus) {
		sprintf(pszBuff, "mix", iVal);
	}
	else{
		sprintf(pszBuff, "bus %d", iVal-ParmIOOffsetBus);
	}
	/*
	TiXmlElement* pOut = new TiXmlElement( "out" );
	TiXmlText* pOutTxt = new TiXmlText(pszBuff);
	pOut->LinkEndChild( pOutTxt );
	pParent->LinkEndChild( pOut );
	*/
	// gain
	iVal = gpApplication->GetGlobalParm(giParam_ChVol, giSection_First_Track + uiTrack);
	tfloat fVolume	=	(tfloat)iVal * 0.0001f;
	sprintf(pszBuff, "%f", fVolume);
	TiXmlElement* pVolume = new TiXmlElement( "volume" );
	TiXmlText* pVolumeTxt = new TiXmlText(pszBuff);
	pVolume->LinkEndChild( pVolumeTxt );
	pParent->LinkEndChild( pVolume );
	
	// pan
	iVal = gpApplication->GetGlobalParm(giParam_ChPannerLeftRight, giSection_First_Track + uiTrack);
	tfloat fPan	=	(tfloat)iVal * 0.0001f;
	sprintf(pszBuff, "%f", fPan);
	TiXmlElement* pPan = new TiXmlElement( "pan" );
	TiXmlText* pPanTxt = new TiXmlText(pszBuff);
	pPan->LinkEndChild( pPanTxt );
	pParent->LinkEndChild( pPan );
	
	// solo
	iVal = gpApplication->GetGlobalParm(giParam_ChSolo, giSection_First_Track + uiTrack);
	iVal ? sprintf(pszBuff, "on") : sprintf(pszBuff, "off");
	TiXmlElement* pSolo = new TiXmlElement( "solo" );
	TiXmlText* pSoloTxt = new TiXmlText(pszBuff);
	pSolo->LinkEndChild( pSoloTxt );
	pParent->LinkEndChild( pSolo );
	
	// mute
	iVal = gpApplication->GetGlobalParm(giParam_ChMute, giSection_First_Track + uiTrack);
	iVal ? sprintf(pszBuff, "on") : sprintf(pszBuff, "off");
	TiXmlElement* pMute = new TiXmlElement( "mute" );
	TiXmlText* pMuteTxt = new TiXmlText(pszBuff);
	pMute->LinkEndChild( pMuteTxt );
	pParent->LinkEndChild( pMute );
	
	// arm
	iVal = gpApplication->GetGlobalParm(giParam_ChArm, giSection_First_Track + uiTrack);
	iVal ? sprintf(pszBuff, "on") : sprintf(pszBuff, "off");
	TiXmlElement* pArm = new TiXmlElement( "arm" );
	TiXmlText* pArmTxt = new TiXmlText(pszBuff);
	pArm->LinkEndChild( pArmTxt );
	pParent->LinkEndChild( pArm );
}

void CKSXML_Write_Project::Write_Track_Inserts(TiXmlElement* pParent, tuint uiTrack)
{
	
	for(tuint uiInsert=0; uiInsert<giNumber_Of_Inserts; uiInsert++)
	{
		Write_Track_Insert( pParent, uiTrack, uiInsert);
	}
}

void CKSXML_Write_Project::Write_Track_Insert(TiXmlElement* pParent, tuint uiTrack, tuint uiInsert)
{
	
	

	// output
//	char pszBuff [64];
	tint32 iInsertId = gpApplication->GetGlobalParm(giParam_ChInsert1 + uiInsert, giSection_First_Track + uiTrack);
	
	if(iInsertId){
		CPreset_Data Preset_Data;
		// in
		TiXmlElement* pInsert = new TiXmlElement( "insert" );
		pParent->LinkEndChild( pInsert );
		pInsert->SetAttribute("slot",uiInsert);
		pInsert->SetAttribute("id", iInsertId);
		
		CPlugInManager* pPlugManager = gpApplication->GetPlugInManager();
		
		std::string s = pPlugManager->GetPlugInVendor(iInsertId);
		std::string u = pPlugManager->GetPlugInName(iInsertId);
		
		// vendor
		TiXmlElement* pVendor = new TiXmlElement( "vendor" );
		TiXmlText* pVendorTxt = new TiXmlText(s.c_str());
		pVendor->LinkEndChild( pVendorTxt );
		pInsert->LinkEndChild( pVendor );
		
		// product
		TiXmlElement* pProduct = new TiXmlElement( "product" );
		TiXmlText* pProductTxt = new TiXmlText(u.c_str());
		pProduct->LinkEndChild( pProductTxt );
		pInsert->LinkEndChild( pProduct );
		
		Create_Plugin_Setting( &Preset_Data);
	}
	
	 
}

tbool CKSXML_Write_Project::Create_Plugin_Setting(CPreset_Data* pPreset_Data)
{

	std::string sPlug_in_Setting		=	gpApplication->Plugin_Settings_Folder() + pPreset_Data->Get_UUID() + ".ksprst";
/*
	// missing code 
	std::string sFileName			= gpApplication->GetProjectName() + ".xml";
	std::string sProject_Name		= gpApplication->Project_Name();
	std::string sProject_Folder		= gpApplication->Project_Folder();
	std::string sProject		=  sProject_Folder + sProject_Name + ".xml";
	
	CAutoDelete<IFile> pfile(IFile::Create());
	if (pfile->Open(sProject.c_str(), IFile::FileWrite)) {
		pfile->Write(xml_str.c_str(), xml_str.length());
		return true;
	}
	
	if (pfPlug_In_Setting) {
		return true;
	}
*/	
	return false;
}

void CKSXML_Write_Project::Write_Track_Regions(TiXmlElement* pParent, tuint uiTrack)
{
	// Note
	tchar pszBuff[1024];
//	tchar* psz = pszBuff;
	CTrack_DSP*		pTrack_DSP		=	gpDSPEngine->GetTrack(uiTrack);
	
	std::list<CTrack_DSP::SChannelRegionInfo*>::const_iterator it = pTrack_DSP->GetRegionList().begin();
	
	for (; it != pTrack_DSP->GetRegionList().end(); it++) {
		
		CTrack_DSP::SChannelRegionInfo* pRegionInfo = *it;
		CRegion_DSP* pRegion_DSP = pRegionInfo->pRegion;
		
		// region id and take nr.
		TiXmlElement* pRegion = new TiXmlElement( "region" );
		
	//	tuint uiID = pRegion_DSP->Get_UUID();
		pRegion->SetAttribute("uuid", pRegion_DSP->Get_UUID().c_str() );
		//pRegion->SetAttribute("take", 0);
		pParent->LinkEndChild( pRegion );
		
		
		
		TiXmlElement* pSample_UUID = new TiXmlElement( "sample" );
		TiXmlText* pSample_UUID_Txt = new TiXmlText(pRegion_DSP->Get_Region_Sample_UUID().c_str());
		pSample_UUID->LinkEndChild( pSample_UUID_Txt );
		pRegion->LinkEndChild( pSample_UUID );
		
		TiXmlElement* pTake_UUID = new TiXmlElement( "take" );
		TiXmlText* pTake_UUID_Txt = new TiXmlText(pRegion_DSP->Get_Region_Take_UUID().c_str());
		pTake_UUID->LinkEndChild( pTake_UUID_Txt );
		pRegion->LinkEndChild( pTake_UUID );
		

		// region position
		tuint64 uiPosition = pRegionInfo->uiTrack_Pos;
		sprintf(pszBuff, "%d", uiPosition);
		
		TiXmlElement* pPosition = new TiXmlElement( "position" );
		TiXmlText* pPositionTxt = new TiXmlText(pszBuff);
		pPosition->LinkEndChild( pPositionTxt );
		pRegion->LinkEndChild( pPosition );
		
		// sample offset
		tuint64 uiSample_Offset = pRegion_DSP->Get_Sample_Offset();
		sprintf(pszBuff, "%d", uiSample_Offset);
		
		TiXmlElement* pSample_Offset = new TiXmlElement( "offset" );
		TiXmlText* pSample_OffsetTxt = new TiXmlText(pszBuff);
		pSample_Offset->LinkEndChild( pSample_OffsetTxt );
		pRegion->LinkEndChild( pSample_Offset );
		
		// sample duration
		tuint64 uiSample_Duration = pRegion_DSP->Get_Duration();
		sprintf(pszBuff, "%d", uiSample_Duration);
		
		TiXmlElement* pSample_Duration = new TiXmlElement( "duration" );
		TiXmlText* pSample_DurationTxt = new TiXmlText(pszBuff);
		pSample_Duration->LinkEndChild( pSample_DurationTxt );
		pRegion->LinkEndChild( pSample_Duration );
		
		// volume
		tfloat fVolume = pRegion_DSP->Volume();
		sprintf(pszBuff, "%f", fVolume);
		
		TiXmlElement* pVolume = new TiXmlElement( "volume" );
		TiXmlText* pVolumeTxt = new TiXmlText(pszBuff);
		pVolume->LinkEndChild( pVolumeTxt );
		pRegion->LinkEndChild( pVolume);
		
		// fades
		TiXmlElement* pFade = new TiXmlElement( "fade" );
		pRegion->LinkEndChild( pFade );
		
		// fade in
		tuint64 uiFade_In_Duration = pRegion_DSP->Fade_In_Duration();
		sprintf(pszBuff, "%d", uiFade_In_Duration);
		
		TiXmlElement* pFade_In_Duration = new TiXmlElement( "in" );
		TiXmlText* pFade_In_DurationTxt = new TiXmlText(pszBuff);
		pFade_In_Duration->LinkEndChild( pFade_In_DurationTxt );
		pFade->LinkEndChild( pFade_In_Duration );
		
		// fade out
		tuint64 uiFade_Out_Duration = pRegion_DSP->Fade_Out_Duration();
		sprintf(pszBuff, "%d", uiFade_Out_Duration);
		
		TiXmlElement* pFade_Out_Duration = new TiXmlElement( "out" );
		TiXmlText* pFade_Out_DurationTxt = new TiXmlText(pszBuff);
		pFade_Out_Duration->LinkEndChild( pFade_Out_DurationTxt );
		pFade->LinkEndChild( pFade_Out_Duration );
		
		
	}
	
	
	
}



void CKSXML_Write_Project::Write_AUXs(TiXmlElement* pParent, tuint uiTrack)
{
	// prepared for more than two AUX sends
	for(tuint iAUX=0; iAUX<giNumber_Of_AUXes; iAUX++)
	{
		Write_AUX( pParent, uiTrack, iAUX);
	}
}

void CKSXML_Write_Project::Write_AUX(TiXmlElement* pParent, tuint uiTrack, tuint iAux)
{
		
	tint32 iSend = gpApplication->GetGlobalParm(giParam_ChAUX1 + iAux, giSection_First_Track + uiTrack);
	
	if(iSend){
		
		char pszBuff [64];
		// aux
		TiXmlElement* pAux = new TiXmlElement( "aux" );
		pParent->LinkEndChild( pAux );
		pAux->SetAttribute("id",iAux);
		
		// send
		tfloat fVolume	=	(tfloat)iSend * 0.0001f;
		sprintf(pszBuff, "%f", fVolume);
		TiXmlText* pAUXTxt = new TiXmlText(pszBuff);
		pAux->LinkEndChild( pAUXTxt );

				
	}
/*	
	// output
	char pszBuff [64];
	tint32 iSend = gpApplication->GetGlobalParm(giParam_ChAUX1 + iAux, giSection_First_Track + uiTrack);
	
	if(iSend){
		
		TiXmlElement* pAux = new TiXmlElement( "aux" );
		pAux->LinkEndChild( pParent );
		pAux->SetAttribute("id",iAux);
		
		// gain
		tfloat fVolume	=	(tfloat)iSend * 0.0001f;
		sprintf(pszBuff, "%f", fVolume);
		TiXmlElement* pSend = new TiXmlElement( "send" );
		TiXmlText* pSendTxt = new TiXmlText(pszBuff);
		pSend->LinkEndChild( pSendTxt );
		pAux->LinkEndChild( pSend );
		
	}
 */
}

void CKSXML_Write_Project::Write_Busses(TiXmlElement* pParent)
{
//	tint32 iNrTracks = gpApplication->Get_Number_Of_Tracks();//gpApplication->msStack.iNr_Of_Tracks;
	
	for(tint32 i = 0; i<giNumber_Of_Busses; i++){
		
		// ID
	//	tuint uiID = gpApplication->Get_Track_Id(i);
		// track
		TiXmlElement* pBus = new TiXmlElement( "bus" );
		pBus->SetAttribute("id",i);
		pParent->LinkEndChild( pBus );
		// write track data
		Write_Bus(pBus, i);	
		
	}
}

void CKSXML_Write_Project::Write_Bus(TiXmlElement* pParent, tuint uiID)
{
	// out
	TiXmlElement* pOut = new TiXmlElement( "out" );
	pParent->LinkEndChild( pOut );
	Write_Bus_Out(pOut, uiID);
	
}

void CKSXML_Write_Project::Write_Bus_Out(TiXmlElement* pParent, tuint uiID)
{
	
	// output
	char pszBuff [64];
	tint32 iVal = gpApplication->GetGlobalParm(giParam_Buss_Out, giSection_First_Buss + uiID);
	
	if(iVal < ParmIOOffsetBus) {
		sprintf(pszBuff, "mix", iVal);
	}
	else{
		sprintf(pszBuff, "bus %d", iVal-ParmIOOffsetBus);
	}
	TiXmlElement* pOut = new TiXmlElement( "out" );
	TiXmlText* pOutTxt = new TiXmlText(pszBuff);
	pOut->LinkEndChild( pOutTxt );
	pParent->LinkEndChild( pOut );
	
	// gain
	iVal = gpApplication->GetGlobalParm(giParam_Buss_Vol, giSection_First_Buss + uiID);
	tfloat fVolume	=	(tfloat)iVal * 0.0001f;
	sprintf(pszBuff, "%f", fVolume);
	TiXmlElement* pVolume = new TiXmlElement( "volume" );
	TiXmlText* pVolumeTxt = new TiXmlText(pszBuff);
	pVolume->LinkEndChild( pVolumeTxt );
	pParent->LinkEndChild( pVolume );
	
	// pan
	iVal = gpApplication->GetGlobalParm(giParam_Buss_PannerLeftRight, giSection_First_Buss + uiID);
	tfloat fPan	=	(tfloat)iVal * 0.0001f;
	sprintf(pszBuff, "%f", fPan);
	TiXmlElement* pPan = new TiXmlElement( "pan" );
	TiXmlText* pPanTxt = new TiXmlText(pszBuff);
	pPan->LinkEndChild( pPanTxt );
	pParent->LinkEndChild( pPan );
	
	// solo
	iVal = gpApplication->GetGlobalParm(giParam_Buss_Solo, giSection_First_Buss + uiID);
	iVal ? sprintf(pszBuff, "on") : sprintf(pszBuff, "off");
	TiXmlElement* pSolo = new TiXmlElement( "solo" );
	TiXmlText* pSoloTxt = new TiXmlText(pszBuff);
	pSolo->LinkEndChild( pSoloTxt );
	pParent->LinkEndChild( pSolo );
	
	// mute
	iVal = gpApplication->GetGlobalParm(giParam_Buss_Mute, giSection_First_Buss + uiID);
	iVal ? sprintf(pszBuff, "on") : sprintf(pszBuff, "off");
	TiXmlElement* pMute = new TiXmlElement( "mute" );
	TiXmlText* pMuteTxt = new TiXmlText(pszBuff);
	pMute->LinkEndChild( pMuteTxt );
	pParent->LinkEndChild( pMute );
	
}
//----------------------------------------------------------------
// comments
//----------------------------------------------------------------
void CKSXML_Write_Project::Add_Comment(TiXmlDocument* pDoc, std::string str)
{
	
	//	TiXmlDeclaration* decl = new TiXmlDeclaration( s.c_str() );
	TiXmlComment*	comment = new TiXmlComment(str.c_str() );
	pDoc->LinkEndChild( comment );
	
}

void CKSXML_Write_Project::Add_Comment( TiXmlElement* pParent, std::string str)
{
	
	//	TiXmlDeclaration* decl = new TiXmlDeclaration( s.c_str() );
	TiXmlComment*	comment = new TiXmlComment(str.c_str() );
	pParent->LinkEndChild( comment );
	
}

