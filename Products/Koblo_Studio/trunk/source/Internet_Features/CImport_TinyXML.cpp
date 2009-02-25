
#include "KSOS.h"


CImport_TinyXML::CImport_TinyXML()
{
	
	mpTinyXMLDoc = new TiXmlDocument;
	
}

CImport_TinyXML::~CImport_TinyXML()
{
	delete mpTinyXMLDoc;
}


void CImport_TinyXML::Reset_Project()
{
	miTrack_ID	=	0;
	
	gpApplication->Clean_Project(0);
	
	mpTinyXMLDoc->Clear();
	
}

void CImport_TinyXML::CKSXML_Parse_DOM_To_DAW()
{
	//	parse values from project tree in to KS data system
	//	Parse_Project( mpTinyXMLDoc );

}

void CImport_TinyXML::Read_Project_Name(TiXmlNode* pParent)
{
	if ( !pParent ) return;
	
	TiXmlNode* pChild = pParent->FirstChild();
	
	if ( pChild ){
		TiXmlText* pText;
		pText = pChild->ToText();
		gpApplication->Online_Project_Name(pText->Value());
	}
}

void CImport_TinyXML::Read_Settings_Object(TiXmlElement* pElement)
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
				Set_DAW_Parameter(pChild, giTinyXml_Type_Float, giParamID_KS_Tempo , giSectionGlobal, 10.0f);
			
			else if (stricmp("signature", pChild->Value()) == 0){
				Read_Signature_Object(pChild->ToElement());
				Update_Signature();
			}	
			
			else if (stricmp("license", pChild->Value()) == 0){
				//!!! TO DO do somthing here
				Set_DAW_Parameter(pChild, 0, giTinyXml_Type_String, -1);

			}
		}
	}
}

void CImport_TinyXML::Read_Signature_Object(TiXmlElement* pElement)
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

void CImport_TinyXML::Update_Signature()
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

void CImport_TinyXML::Parse_Edditing_Object(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("record", pChild->Value()) == 0) 
				gpApplication->SetRecord(false);
			
			else if (stricmp("tool", pChild->Value()) == 0) {
				
				TiXmlNode* pTool = pChild->FirstChild();
				if(pTool) {
					std::string sTool = pTool->Value();
					Read_Tool_Object(sTool);
				}
			}
			
			else if (stricmp("zoom", pChild->Value()) == 0)
				Set_DAW_Parameter(pChild, giTinyXml_Type_Int, giParamID_Zoom , giSectionGUI );
			
			
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
				Read_Loop_Object(pChild->ToElement());
			
			else if (stricmp("windows", pChild->Value()) == 0)
				Parse_Windows_Object(pChild->ToElement());
			
			else if (stricmp("dialogs", pChild->Value()) == 0)
				Read_Dialogs(pChild->ToElement());
			
		}
	}
}
void CImport_TinyXML::Parse_Preset_File_Object(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	TiXmlAttribute* pAttrib		=	pElement->FirstAttribute();
	if(pAttrib)		
		gpApplication->Set_Insert_File_UUID(pAttrib->Value());
	
	
}

void CImport_TinyXML::Read_Tool_Object(std::string sTool)
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

void CImport_TinyXML::Read_Grid(std::string sGrid)
{
	if (stricmp("off", sGrid.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Grid, 0,  giSectionGUI);
	
	else if (stricmp("on", sGrid.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Grid, 1,  giSectionGUI);
	
}

void CImport_TinyXML::Read_Snap(std::string sSnap)
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

void CImport_TinyXML::Read_View_Waves(std::string sWaves)
{
	
	if (stricmp("off", sWaves.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Show_Waveform, 0,  giSectionGUI);
	
	else if (stricmp("on", sWaves.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Show_Waveform, 1,  giSectionGUI);
	

}

void CImport_TinyXML::Read_View_Fades(std::string sFades)
{
	if (stricmp("off", sFades.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Show_Fade, 0,  giSectionGUI);
	
	else if (stricmp("on", sFades.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Show_Fade, 1,  giSectionGUI);
	
}



void CImport_TinyXML::Read_Loop_Object(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("active", pChild->Value()) == 0) {
				TiXmlNode* pActive = pChild->FirstChild();
				if(pActive) {
					std::string sActive = pActive->Value();
					
					if (stricmp("off", sActive.c_str()) == 0) 
						gpApplication->SetGlobalParm(giParamID_Loop_On, 0,  giSectionGlobal);
					
					else if (stricmp("on", sActive.c_str()) == 0) 
						gpApplication->SetGlobalParm(giParamID_Loop_On, 1,  giSectionGlobal);
					
				}
			}
			
			else if (stricmp("start", pChild->Value()) == 0) 
				Set_DAW_Parameter(pChild, giTinyXml_Type_Int,giParamID_Loop_Start , giSectionGlobal );

			else if (stricmp("end", pChild->Value()) == 0) 
				Set_DAW_Parameter(pChild, giTinyXml_Type_Int,giParamID_Loop_End , giSectionGlobal );

		}
	}
}



void CImport_TinyXML::Parse_Windows_Object(TiXmlElement* pElement)
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

void CImport_TinyXML::Read_Window(TiXmlNode* pParrent, tint32 iWindow)
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
				//;Set_DAW_Parameter(pChild, giTinyXml_Type_Int, 0 , -1);
			}
			
		}
	}
}

void CImport_TinyXML::Open_Window(std::string sOn_Off, tint32 iWindow)
{
	
	if (stricmp("off", sOn_Off.c_str()) == 0){
		gpApplication->SetGlobalParm(iWindow, 0,  giSectionGUI);
	}
	
	else if (stricmp("on", sOn_Off.c_str()) == 0) 
		gpApplication->SetGlobalParm(iWindow, 1,  giSectionGUI);
	

}

void CImport_TinyXML::Read_Window_Pos(TiXmlNode* pParrent, tint32 iId)
{
	TiXmlElement* pElement = pParrent->ToElement();
	
	if ( !pElement ) return ;
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("x", pChild->Value()) == 0) {
				Set_DAW_Parameter(pChild, 0, giTinyXml_Type_Int, iId);
			}
			else if (stricmp("y", pChild->Value()) == 0) {
				Set_DAW_Parameter(pChild, 0, giTinyXml_Type_Int, iId);
			}
		}
	}
}

void CImport_TinyXML::Read_Window_Size(TiXmlNode* pParrent, tint32 iId)
{
	TiXmlElement* pElement = pParrent->ToElement();
	
	if ( !pElement ) return ;
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("x", pChild->Value()) == 0) {
				Set_DAW_Parameter(pChild, 0, giTinyXml_Type_Int, iId);
			}
			else if (stricmp("y", pChild->Value()) == 0) {
				Set_DAW_Parameter(pChild, 0, giTinyXml_Type_Int, iId);
			}
		}
	}
}

void CImport_TinyXML::Read_Dialogs(TiXmlElement* pElement)
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

void CImport_TinyXML::Read_Export_Dialog(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("type", pChild->Value()) == 0) {

				Set_DAW_Parameter(pChild, 0, giTinyXml_Type_String, 0);
			}
			else if (stricmp("compression", pChild->Value()) == 0) {
				Set_DAW_Parameter(pChild, 0, giTinyXml_Type_Int, 0);
			}
			else if (stricmp("tail", pChild->Value()) == 0) {
				Set_DAW_Parameter(pChild, 0, giTinyXml_Type_Int, 0);
			}
			else if (stricmp("normalize", pChild->Value()) == 0) {
				Set_DAW_Parameter(pChild, 0, giTinyXml_Type_String, 0);
			}
		}
	}
}





tbool CImport_TinyXML::In_Wave_Files(CTake_Data* pTake_Data)
{
	std::string sFolder = gpApplication->Wave_File_Folder();
	return In_Folder(	pTake_Data , sFolder );
	
}


tbool CImport_TinyXML::In_OGG_Files(CTake_Data* pTake_Data)
{
	std::string sFolder = gpApplication->OGG_File_Folder();
	return In_Folder(	pTake_Data , sFolder );

}

tbool CImport_TinyXML::In_Folder(CTake_Data* pTake_Data, std::string sFolder)
{
	
	switch( pTake_Data->Channels()) {
		case 1:
		default:
		{	// mono 
			return gpApplication->Readable_Audio(sFolder + pTake_Data->Disk_Name(1));
		}
		case 2:
		{	// files are split in two
			tuint In_Wave_Files = 0;
			In_Wave_Files += gpApplication->Readable_Audio(sFolder + pTake_Data->Disk_Name(1));
			In_Wave_Files += gpApplication->Readable_Audio(sFolder + pTake_Data->Disk_Name(2));
			return In_Wave_Files == 2 ? true: false;
		}
	}
	return false;
}

void CImport_TinyXML::Read_Insert(TiXmlElement* pElement)
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
	 Set_DAW_Parameter(pChild, giTinyXml_Type_String, 0, 0);
	 printf( "\n" );
	 }
	 else if (stricmp("product", pChild->Value()) == 0) {
	 printf( "product:  ");
	 Set_DAW_Parameter(pChild, giTinyXml_Type_String, 0, 0);
	 printf( "\n" );
	 }
	 else if (stricmp("url", pChild->Value()) == 0) {
	 printf( "url:  ");
	 Set_DAW_Parameter(pChild, giTinyXml_Type_String, 0, 0);
	 printf( "\n" );
	 }
	 else if (stricmp("bypass", pChild->Value()) == 0) {
	 printf( "bypass:  ");
	 Set_DAW_Parameter(pChild, giTinyXml_Type_String, 0, 0);
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

void CImport_TinyXML::Read_Insert_Settings(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("parameter", pChild->Value()) == 0) {
				Read_Insert_Parameter(pChild->ToElement());
				
			}
			else if (stricmp("chunk", pChild->Value()) == 0) {
				
				Set_DAW_Parameter(pChild, giTinyXml_Type_String, 0, 0);
				
			}
		}
	}
}

void CImport_TinyXML::Read_Insert_Parameter(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("name", pChild->Value()) == 0) {
				
				Set_DAW_Parameter(pChild, giTinyXml_Type_String, 0, 0);
				
			}
			else if (stricmp("default", pChild->Value()) == 0) {
				
				Set_DAW_Parameter(pChild, giTinyXml_Type_Float, 0, 0);
				
			}
			else if (stricmp("value", pChild->Value()) == 0) {
				
				Set_DAW_Parameter(pChild, giTinyXml_Type_Float, 0, 0);
			}
		}
	}
}














void CImport_TinyXML::Set_DAW_Parameter( TiXmlNode* pParent, tuint uiType , tuint32 iParamID, tint32 iSection , tfloat fFactor )
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
					gpApplication->SetGlobalParm(iParamID, fValue * fFactor, iSection);
				
				
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












tbool CImport_TinyXML::Check_For_Newer_Revision(tint32 iProject_ID)
{
	mbNew_Revision = false;
	// Check DOM downloaded to see if there is a newer one
	Check_Project( mpTinyXMLDoc );
	// return resoult
	return mbNew_Revision;
}


void CImport_TinyXML::Check_Project( TiXmlNode* pParent )
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

void CImport_TinyXML::Check_Project_Childs(TiXmlNode* pParent)
{
	if ( !pParent ) return ;

	// parse all childs
	TiXmlNode* pChild;
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		if(pChild->Type() == TiXmlNode::ELEMENT)
			Check_Branch(pChild);
	}}

void CImport_TinyXML::Check_Branch(TiXmlNode* pParent)
{
	if (stricmp("branch", pParent->Value()) == 0) {
		Check_Revision(pParent->ToElement());
	}	
}

void CImport_TinyXML::Check_Revision(TiXmlElement* pElement)
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
/*
void CImport_TinyXML::Read_Grid(std::string sGrid)
{
	if (stricmp("off", sGrid.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Grid, 0,  giSectionGUI);
	
	else if (stricmp("on", sGrid.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Grid, 1,  giSectionGUI);
	
}

void CImport_TinyXML::Read_Snap(std::string sSnap)
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

void CImport_TinyXML::Read_View_Waves(std::string sWaves)
{
	
	if (stricmp("off", sWaves.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Show_Waveform, 0,  giSectionGUI);
	
	else if (stricmp("on", sWaves.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Show_Waveform, 1,  giSectionGUI);
	
	
}

void CImport_TinyXML::Read_View_Fades(std::string sFades)
{
	if (stricmp("off", sFades.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Show_Fade, 0,  giSectionGUI);
	
	else if (stricmp("on", sFades.c_str()) == 0) 
		gpApplication->SetGlobalParm(giParamID_Show_Fade, 1,  giSectionGUI);
	
}

*/










