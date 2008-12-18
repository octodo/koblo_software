
#include "KSOS.h"


CKSXML_Write::CKSXML_Write(CKSPlugIn * pKSPlugIn)
{
	
	mpKSPlugIn = pKSPlugIn;
	
}

CKSXML_Write::~CKSXML_Write()
{
	
}
void CKSXML_Write::Write_XML(std::string str)
{
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	
	TiXmlDocument *pDoc		=	new TiXmlDocument("koblo_doc");
	pDoc->LinkEndChild( decl );
	
	
	Add_Comment(pDoc, "Koblo Studio music project XML format");
	Add_Comment(pDoc, "most ids are originally returned from the website, and are globally unique (between different projects)");
	Add_Comment(pDoc, "ids are always specified as attributes");
	Add_Comment(pDoc, "all times and track positions are specified in sample points");
	
	Write_Project(pDoc);

	pDoc->SaveFile( str.c_str() );
	
	


	
}

void CKSXML_Write::Write_Project(TiXmlDocument* pDoc)
{
	
	TiXmlElement* pProject = new TiXmlElement( "project" );
//	TiXmlText * text = new TiXmlText( "World" );
//	pProject->LinkEndChild( text );
	
	pProject->SetAttribute("id",123);
	pProject->SetAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
	pProject->SetAttribute("xsi:noNamespaceSchemaLocation","http://koblo.com/schemas/koblo_project.xsd");
	
	
	pDoc->LinkEndChild( pProject );
	
	Write_Branch( pProject);
	Write_Settings( pProject);
	Write_Editing( pProject);
	
}

void CKSXML_Write::Write_Branch(TiXmlElement* pParent)
{
	
	Add_Comment(pParent, "information about the branch");
	
	TiXmlElement* pBranch = new TiXmlElement( "branch" );
	// ID
	pBranch->SetAttribute("id",123);
	pParent->LinkEndChild( pBranch );
	
	
	// name
	TiXmlElement* pName = new TiXmlElement( "name" );
	TiXmlText* pNameTxt = new TiXmlText("Resonance Chamber Edition");
	pName->LinkEndChild( pNameTxt );
	pBranch->LinkEndChild( pName );
	
	// description
	TiXmlElement* pDescription = new TiXmlElement( "description" );
	TiXmlText* pDescriptionTxt = new TiXmlText("A branch with extra high levels of resonance");
	pDescription->LinkEndChild( pDescriptionTxt );
	pBranch->LinkEndChild( pDescription );
	
	// revision
	TiXmlElement* pRevision = new TiXmlElement( "revision" );
	TiXmlText* pRevisionTxt = new TiXmlText("67");
	pRevision->LinkEndChild( pRevisionTxt );
	pBranch->LinkEndChild( pRevision );
	
	
}

void CKSXML_Write::Write_Settings(TiXmlElement* pParent)
{
	Add_Comment(pParent, "overall project settings");
	
	TiXmlElement* pSettings = new TiXmlElement( "settings" );
	pParent->LinkEndChild( pSettings );
	
	// name
	TiXmlElement* pName = new TiXmlElement( "name" );
	TiXmlText* pNameTxt = new TiXmlText("Crystal Cave");
	pName->LinkEndChild( pNameTxt );
	pSettings->LinkEndChild( pName );
	
	// description
	TiXmlElement* pDescription = new TiXmlElement( "description" );
	TiXmlText* pDescriptionTxt = new TiXmlText("This project uses samples from underground crystal caves in Mexico.");
	pDescription->LinkEndChild( pDescriptionTxt );
	pSettings->LinkEndChild( pDescription );
	

	char pszBuff [64];
	
	// Samplerate
	sprintf(pszBuff, "%d", mpKSPlugIn->GetSampleRate());
	TiXmlElement* pSampleRate = new TiXmlElement( "samplerate" );
	TiXmlText* pSampleRateTxt = new TiXmlText(pszBuff);
	pSampleRate->LinkEndChild( pSampleRateTxt );
	pSettings->LinkEndChild( pSampleRate );
	
	//Tempo
	tfloat32 fTempo = (tfloat32)mpKSPlugIn->GetGlobalParm(giParamID_KS_Tempo, giSectionGlobal) / 10.0f;
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

void CKSXML_Write::Write_Signature(TiXmlElement* pParent)
{
	tuint32 uiSignatur = mpKSPlugIn->GetGlobalParm(giParamID_KS_Time_Signature, giSectionGlobal);
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
	TiXmlElement* pDenominator = new TiXmlElement( "tempo" );
	TiXmlText* pDenominatorTxt = new TiXmlText(pszBuff);
	pDenominator->LinkEndChild( pDenominatorTxt );
	pParent->LinkEndChild( pDenominator );
	
}

void CKSXML_Write::Write_License(TiXmlElement* pParent)
{
	char pszBuff [64];
	tuint uiTest	=	 mpKSPlugIn->GetGlobalParm(giParamID_CC_License_Type, giSectionGUI);
	
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
	TiXmlText* pLicenseTxt = new TiXmlText(pszBuff);
	pParent->LinkEndChild( pLicenseTxt );

}

void CKSXML_Write::Write_Editing(TiXmlElement* pParent)
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
	tint iVal = mpKSPlugIn->GetGlobalParm(giParamID_Zoom, giSectionGUI);
	sprintf(pszBuff, "%d", iVal);
	TiXmlElement* pZoom = new TiXmlElement( "zoom" );
	TiXmlText* pZoomTxt = new TiXmlText(pszBuff);
	pZoom->LinkEndChild( pZoomTxt );
	pEditing->LinkEndChild( pZoom );
	
	// grid
	iVal = mpKSPlugIn->GetGlobalParm(giParamID_Grid, giSectionGUI);
	TiXmlElement* pGrid = new TiXmlElement( "grid" );
	iVal ? sprintf(pszBuff, "on") : sprintf(pszBuff, "off");
	TiXmlText* pGridTxt = new TiXmlText(pszBuff);
	pGrid->LinkEndChild( pGridTxt );
	pEditing->LinkEndChild( pGrid );
	
	// snap
	iVal = mpKSPlugIn->GetGlobalParm(giParamID_KS_Snap_To, giSectionGlobal);
	TiXmlElement* pSnap = new TiXmlElement( "snap" );
	iVal ? sprintf(pszBuff, "on") : sprintf(pszBuff, "off");
	TiXmlText* pSnapTxt = new TiXmlText(pszBuff);
	pSnap->LinkEndChild( pSnapTxt );
	pEditing->LinkEndChild( pSnap );
	
	// waves
	iVal = mpKSPlugIn->GetGlobalParm(giParamID_Show_Waveform, giSectionGUI);
	TiXmlElement* pWaves = new TiXmlElement( "waves" );
	iVal ? sprintf(pszBuff, "on") : sprintf(pszBuff, "off");
	TiXmlText* pWavesTxt = new TiXmlText(pszBuff);
	pWaves->LinkEndChild( pWavesTxt );
	pEditing->LinkEndChild( pWaves );
	
	// fades
	iVal = mpKSPlugIn->GetGlobalParm(giParamID_Show_Fade, giSectionGUI);
	TiXmlElement* pFades = new TiXmlElement( "fades" );
	iVal ? sprintf(pszBuff, "on") : sprintf(pszBuff, "off");
	TiXmlText* pFadesTxt = new TiXmlText(pszBuff);
	pFades->LinkEndChild( pFadesTxt );
	pEditing->LinkEndChild( pFades );
	
	// loop
	TiXmlElement* pLoop = new TiXmlElement( "loop" );;
	pEditing->LinkEndChild( pLoop );
	Write_Loop(pLoop);
	
/*	// position
	iVal = mpKSPlugIn->GetGlobalParm(giParamID_Loop_End, giSectionGlobal);
	sprintf(pszBuff, "%d", iVal);
	TiXmlElement* pEnd = new TiXmlElement( "end" );
	TiXmlText* pEndTxt = new TiXmlText(pszBuff);
	pEnd->LinkEndChild( pEndTxt );
	pParent->LinkEndChild( pEnd );
*/	
	// windows
	TiXmlElement* pWindows = new TiXmlElement( "windows" );;
	pEditing->LinkEndChild( pWindows );
	Write_Windows(pWindows);
	
	
}

void CKSXML_Write::Write_Tool(TiXmlElement* pParent)
{
	char pszBuff [64];
	tuint uiTest	=	 mpKSPlugIn->GetGlobalParm(giParamID_Tool_Selected, giSectionGUI);
	
	
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

void CKSXML_Write::Write_Loop(TiXmlElement* pParent)
{
	char pszBuff [64];

	// on
	tint32 iVal = mpKSPlugIn->GetGlobalParm(giParamID_Loop_On, giSectionGlobal);
	TiXmlElement* pActive = new TiXmlElement( "active" );
	iVal ? sprintf(pszBuff, "on") : sprintf(pszBuff, "off");
	TiXmlText* pActiveTxt = new TiXmlText(pszBuff);
	pActive->LinkEndChild( pActiveTxt );
	pParent->LinkEndChild( pActive );
	
	// start
	iVal = mpKSPlugIn->GetGlobalParm(giParamID_Loop_Start, giSectionGlobal);
	sprintf(pszBuff, "%d", iVal);
	TiXmlElement* pStart = new TiXmlElement( "start" );
	TiXmlText* pStartTxt = new TiXmlText(pszBuff);
	pStart->LinkEndChild( pStartTxt );
	pParent->LinkEndChild( pStart );
	
	// end
	iVal = mpKSPlugIn->GetGlobalParm(giParamID_Loop_End, giSectionGlobal);
	sprintf(pszBuff, "%d", iVal);
	TiXmlElement* pEnd = new TiXmlElement( "end" );
	TiXmlText* pEndTxt = new TiXmlText(pszBuff);
	pEnd->LinkEndChild( pEndTxt );
	pParent->LinkEndChild( pEnd );
	
	
}

void CKSXML_Write::Write_Windows(TiXmlElement* pParent)
{
	// edit
	TiXmlElement* pEdit = new TiXmlElement( "edit" );;
	pParent->LinkEndChild( pEdit );
	Write_Window_Edit(pEdit);
	
	// mix
	TiXmlElement* pMix = new TiXmlElement( "mix" );;
	pParent->LinkEndChild( pMix );
	Write_Window_Mix(pMix);
	
	// rack
	TiXmlElement* pRack = new TiXmlElement( "rack" );;
	pParent->LinkEndChild( pRack );
	Write_Window_Rack(pRack);
}

void CKSXML_Write::Write_Window_Edit(TiXmlElement* pParent)
{
	
//	sprintf(pszBuff, "%d", iVal);
	
	// show
	TiXmlElement* pShow = new TiXmlElement( "show" );
	TiXmlText* pShowTxt = new TiXmlText("on");
	pShow->LinkEndChild( pShowTxt );
	pParent->LinkEndChild( pShow );
	
	
	// layer
	char pszBuff [64];
	tint32 iVal = 1;//mpKSPlugIn->GetGlobalParm(giParamID_Loop_On, giSectionGlobal);
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
	Add_Comment(pParent, "the order of tracks is considered an editing setting, and is specified here");
	
}

void CKSXML_Write::Write_Window_Mix(TiXmlElement* pParent)
{
	
}

void CKSXML_Write::Write_Window_Rack(TiXmlElement* pParent)
{
	
}

void CKSXML_Write::Write_Window_Position(TiXmlElement* pParent, tuint uiPosX, tuint uiPosY)
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
	
void CKSXML_Write::Write_Window_Size(TiXmlElement* pParent, tuint uiSizeX, tuint uiSizeY)
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










void CKSXML_Write::Add_Comment(TiXmlDocument* pDoc, std::string str)
{
	
	//	TiXmlDeclaration* decl = new TiXmlDeclaration( s.c_str() );
	TiXmlComment*	comment = new TiXmlComment(str.c_str() );
	pDoc->LinkEndChild( comment );
	
}

void CKSXML_Write::Add_Comment( TiXmlElement* pParent, std::string str)
{
	
	//	TiXmlDeclaration* decl = new TiXmlDeclaration( s.c_str() );
	TiXmlComment*	comment = new TiXmlComment(str.c_str() );
	pParent->LinkEndChild( comment );
	
}