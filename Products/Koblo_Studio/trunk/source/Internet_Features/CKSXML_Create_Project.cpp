
#include "KSOS.h"


CKSXML_Create_Project::CKSXML_Create_Project()
{
	

	
}

CKSXML_Create_Project::~CKSXML_Create_Project()
{
	
}

tbool CKSXML_Create_Project::Create_Project_Handshake()
{
	
//-------------- DUMMY CODE HERE --------------------
	// Read dummy XML file from HD here
	TiXmlDocument *pDoc		=	new TiXmlDocument();
	bool loadOkay = pDoc->LoadFile("Project_Created_OK.xml");
	
	
	if (loadOkay)
	{
		// convert pDoc to a std::string
		TiXmlPrinter printer;
		pDoc->Accept(&printer);
		std::string xml_str =  printer.CStr();
		
		// print to console
		printf(xml_str.c_str());
		
		Parse_XML(pDoc);
		return true;
	
	}
	else
	{
		printf("Failed to load return code xml file");
		return false;
	}
	
	// shell script to create new project on koblo.com
	/*
	 curl http://koblo.com/projects.xml -u \
	 "usere@mail.com:password" \
	 -F "project[name]=Gazzell blond dear" \
	 -F "project[description]=a animal tribute" \
	 -F "project[license]=by-sa" \
	 -i
	 */
	
	
//-------------- REAL CODE HERE --------------------
	
/*	
	
	tbool bReturnValue = false;
 
	std::string sLicense = gpApplication->Create_License_String();
 
 
	 std::string str;
	 char psz[128];
	 sprintf(psz, "/projects.xml");
	 str = psz;
	 
	tchar* pszBuff = NULL;
	tint32 iOutLen = 0;
	// username and password has to bein the http autincication header
	// Returns a .xml file
	ine::IINetUtil::GetWebFile(NULL, "koblo.com", str.c_str(), &iOutLen, &pszBuff);
	
	if ((pszBuff) && (iOutLen > 0)) {
		
		// create TinyXml document
		TiXmlDocument doc("koblo_studio project");
		// parse XML file in to TinyXml object tree
		doc.Parse(pszBuff);
		
		printf(pszBuff);
 
		// parse values from project tree in to KS data system
		Parse_XML( &doc );
	bReturnValue = true;
		
	}
	else printf("Failed to load xml file returned from koblo.com");
	
	ine::IINetUtil::ReleaseBuffer(&pszBuff);
 
	return bReturnValue;
*/ 
	
}

void CKSXML_Create_Project::Parse_XML( TiXmlNode* pParent )
{
	// if file is empty return
	if ( !pParent ) return;
	
	TiXmlNode* pChild;
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		if(pChild->Type() == TiXmlNode::ELEMENT)
			Parse_Project_Info(pChild);
	}	
}

void CKSXML_Create_Project::Parse_Project_Info(TiXmlNode* pParent)
{
	if ( !pParent ) return ;

	
	TiXmlAttribute* pAttrib	=	pParent->ToElement()->FirstAttribute();
	tint32 ival;
	
	// set project ID
	if (pAttrib && pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)  {
		
		gpApplication->SetGlobalParm(giParamID_Project_ID, ival, giSectionGlobal);
		
	}
		
	
	// parse all childs
	TiXmlNode* pChild;
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		if(pChild->Type() == TiXmlNode::ELEMENT)
			Write_Project_Info(pChild);
	}
}


void CKSXML_Create_Project::Write_Project_Info(TiXmlNode* pParent)
{
	if (stricmp("name", pParent->Value()) == 0) {
		Write_Project_Name( pParent->ToElement());
		
	}
	else if (stricmp("description", pParent->Value()) == 0) {
		//-------------------------  DESCRIPTION -----------------------------
		Write_Project_Description(pParent->ToElement());
	}
	else if (stricmp("license", pParent->Value()) == 0) {
		//--------------------------  LICENSE ---------------------------
		Write_Project_License(pParent->ToElement());
	}
	
}

void CKSXML_Create_Project::Write_Project_Name(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	TiXmlNode* pChild = pElement->FirstChild();
	gpApplication->Set_Project_Name( pChild->Value());
}

void CKSXML_Create_Project::Write_Project_Description(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	TiXmlNode* pChild = pElement->FirstChild();
	gpApplication->Set_Project_Description( pChild->Value());
}


void CKSXML_Create_Project::Write_Project_License(TiXmlElement* pElement)
{
	if ( !pElement ) return ;
	
	if (stricmp("by-sa", pElement->Value()) == 0) {
		//!ignore this we already got it
	}
}

