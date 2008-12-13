
#include "KSOS.h"




CKSXML::CKSXML(CKSPlugIn * pKSPlugIn)
{
	
	mpKSPlugIn = pKSPlugIn;
	
}

CKSXML::~CKSXML()
{
	
}

void CKSXML::ReadOnlineXML()
{

	tchar* pszBuff = NULL;
	tint32 iOutLen = 0;
	ine::IINetUtil::GetWebFile(NULL, "koblo.com", "/projects/16/branches/21.xml", &iOutLen, &pszBuff);
	if ((pszBuff) && (iOutLen > 0)) {

		// create TinyXml document
		TiXmlDocument doc("koblo_studio project");
		// parse XML file in tp TinyXml object tree
		doc.Parse(pszBuff);
		// parse values from project tree in to KS data system
		Parse_Root( &doc );
	}
	ine::IINetUtil::ReleaseBuffer(&pszBuff);
}


void CKSXML::Parse_Root( TiXmlNode* pParent )
{
	// if file is empty return
	if ( !pParent ) return;
	
	TiXmlNode* pChild;
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		if(pChild->Type() == TiXmlNode::ELEMENT)
			Set_Project(pChild);
	}	
}


void CKSXML::Set_Project(TiXmlNode* pParent)
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



void CKSXML::Parse_Project_Childs(TiXmlNode* pParent)
{
	if (stricmp("branch", pParent->Value()) == 0) {
		printf("--------------------------  BRANCH ----------------------------\n");
		Set_Branch(pParent->ToElement());
	}
	else if (stricmp("settings", pParent->Value()) == 0) {
		printf("--------------------------  SETTINGS ----------------------------\n");
		Set_Settings(pParent->ToElement());
	}
	else if (stricmp("editing", pParent->Value()) == 0) {
		printf("--------------------------  EDDITING ----------------------------\n");
		Set_Edditing(pParent->ToElement());
	}
	else if (stricmp("sample", pParent->Value()) == 0) {
		printf("--------------------------  SAMPLE ----------------------------\n");
		Set_Sample(pParent->ToElement());
	}
	else
		Ignore_Tag( pParent);
}

void CKSXML::Ignore_Tag(TiXmlNode* pParent)
{
//	printf(".\n");
	if ( !pParent ) return;
	
	TiXmlNode* pChild;
	
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		Ignore_Tag( pChild );
	}
}



void CKSXML::Set_Branch(TiXmlElement* pElement)
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
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			
			else if (stricmp("description", pChild->Value()) == 0) {
				printf( "branch description:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, 0);
				printf( "\n" );
			}
			
			else if (stricmp("revision", pChild->Value()) == 0){
				printf( "branch revision:  ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, 0);
				printf( "\n" );
			}
		}
	}
}





void CKSXML::Set_Settings(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("name", pChild->Value()) == 0) {
				printf( "project name:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			
			else if (stricmp("description", pChild->Value()) == 0) {
				printf( "project description:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, 0);
				printf( "\n" );
			}
			else if (stricmp("image", pChild->Value()) == 0){
				printf( "image link:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, 0);
				printf( "\n" );
			}
			else if (stricmp("samplerate", pChild->Value()) == 0){
				printf( "samplerate:  ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, 0);
				printf( "\n" );
			}
			else if (stricmp("tempo", pChild->Value()) == 0){
				printf( "tempo:  ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, 0);
				printf( "\n" );
			}
			else if (stricmp("signature", pChild->Value()) == 0){
				printf( "signature:  ");
				Set_Signature(pChild->ToElement());
				printf( "\n" );
				
			}
			else if (stricmp("license", pChild->Value()) == 0){
				printf( "license:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, 0);
				printf( "\n" );
			}
		}
	}
}




void CKSXML::Set_Signature(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("numerator", pChild->Value()) == 0) {
				Set_Param(pChild, giTinyXml_Type_Int, 0, 0);
				printf( ":" );
			}
			if (stricmp("denominator", pChild->Value()) == 0) {
				Set_Param(pChild, giTinyXml_Type_Int, 0, 0);
			}
		}
	}
}


void CKSXML::Set_Edditing(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("record", pChild->Value()) == 0) {
				printf( "record:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			
			else if (stricmp("tool", pChild->Value()) == 0) {
				printf( "tool:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, 0);
				printf( "\n" );
			}
			else if (stricmp("zoom", pChild->Value()) == 0){
				printf( "zoom:  ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, 0);
				printf( "\n" );
			}
			else if (stricmp("grid", pChild->Value()) == 0){
				printf( "grid:  ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, 0);
				printf( "\n" );
			}
			else if (stricmp("snap", pChild->Value()) == 0){
				printf( "snap:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, 0);
				printf( "\n" );
				
			}
			else if (stricmp("waves", pChild->Value()) == 0){
				printf( "waves:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, 0);
				printf( "\n" );
				
			}
			else if (stricmp("fades", pChild->Value()) == 0){
				printf( "fades:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, 0);
				printf( "\n" );
				
			}
			else if (stricmp("loop", pChild->Value()) == 0){
				printf( "loop:  ");
				Set_Loop(pChild->ToElement());
			}
			else if (stricmp("position", pChild->Value()) == 0){
				printf( "position:  ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, 0);
				printf( "\n" );
			}
			else if (stricmp("windows", pChild->Value()) == 0){
				//printf( "windows:  ");
				Set_Windows(pChild->ToElement());
			}
			else if (stricmp("dialogs", pChild->Value()) == 0){
				//printf( "windows:  ");
				Set_Dialogs(pChild->ToElement());
			}
		}
	}
}

void CKSXML::Set_Loop(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("active", pChild->Value()) == 0) {
				//printf( "active:  ");
				Set_Param(pChild, 0, giTinyXml_Type_String, 0);
				printf( "\n" );
			}
			else if (stricmp("start", pChild->Value()) == 0) {
				printf( "start:  ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, 0);
				printf( "\n" );
			}
			else if (stricmp("end", pChild->Value()) == 0) {
				printf( "end:  ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, 0);
				printf( "\n" );
			}
		}
	}
}

void CKSXML::Set_Windows(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("edit", pChild->Value()) == 0){
				printf( "---- EDIT WINDOW ----\n");
				Set_Window(pChild, 0);
			}
			if (stricmp("mix", pChild->Value()) == 0){
				printf( "---- MIX WINDOW ----\n");
				Set_Window(pChild, 1);
			}
			if (stricmp("rack", pChild->Value()) == 0){
				printf( "---- RACK WINDOW ----\n");
				Set_Window(pChild, 3);
			}
		}
	}
}

void CKSXML::Set_Window(TiXmlNode* pParrent, tint32 iId)
{
	TiXmlElement* pElement = pParrent->ToElement();
	
	if ( !pElement ) return ;
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("show", pChild->Value()) == 0) {
				printf("show: ");
				Set_Param(pChild, 0, giTinyXml_Type_String, iId);
				printf( "\n" );
			}
			else if (stricmp("position", pChild->Value()) == 0){
				printf( "position:  ");
				Set_Window_Pos(pChild, iId);
			}
			else if (stricmp("size", pChild->Value()) == 0){
				printf( "size:  ");
				Set_Window_Size(pChild, iId);
			}
			if (stricmp("layer", pChild->Value()) == 0) {
				printf("layer: ");
				Set_Param(pChild, 0, giTinyXml_Type_Int, iId);
				printf( "\n" );
			}
			
		}
	}
}

void CKSXML::Set_Window_Pos(TiXmlNode* pParrent, tint32 iId)
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

void CKSXML::Set_Window_Size(TiXmlNode* pParrent, tint32 iId)
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

void CKSXML::Set_Dialogs(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("export", pChild->Value()) == 0){
				printf( "---- EXPORT DIALOG ----\n");
				Set_Export_Dialog(pChild->ToElement());
			}
		}
	}
}

void CKSXML::Set_Export_Dialog(TiXmlElement* pElement)
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

void CKSXML::Set_Sample(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	TiXmlAttribute* pAttrib	=	pElement->FirstAttribute();
	tint32 ival;
	
	// sample id
	if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
		printf( "sample id =  %d \n", ival);
	printf( "\n" );
	
	TiXmlNode* pChild;
	
	for ( pChild = pElement->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("name", pChild->Value()) == 0) {
				printf( "sample name:  ");
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			else if (stricmp("take", pChild->Value()) == 0) {
				/*
				if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    
					printf( "take id =  %d \n", ival);
				*/
				Set_Sample_Take(pChild->ToElement());
				printf( "\n" );
			}
			/*
			else if (stricmp("settings", pChild->Value()) == 0){
				printf( "settings:  ");
				Set_Sample_Settings(pChild->ToElement());
				printf( "\n" );
				
			}
			 */
		}
	}
}

void CKSXML::Set_Sample_Take(TiXmlElement* pElement)
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
				Set_Param(pChild, giTinyXml_Type_String, 0, 0);
				printf( "\n" );
			}
			
		}
	}
}


void CKSXML::Set_Param( TiXmlNode* pParent, tuint uiType,tuint32 uiID, tuint uiSection )
{
	if ( !pParent ) return;
	
	TiXmlNode* pChild;
	TiXmlText* pText;
	
	if (  pParent->Type() == TiXmlNode::TEXT )
	{
		switch(uiType){
			case giTinyXml_Type_String:
				pText = pParent->ToText();
				printf( "%s", pText->Value() );
				break;
				
			case giTinyXml_Type_Int:{
				
				pText = pParent->ToText();
				std::string s = pText->Value();
				int number = atoi(s.c_str());
				printf( "%d", number );
				break;
			
			}
			default : break;
		}
	}
	
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		Set_Param( pChild, uiType, 0, 0);
	}
	
}

