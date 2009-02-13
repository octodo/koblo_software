
#include "KSOS.h"

CXML_Hand_Shakes::CXML_Hand_Shakes()
{
	mpXMLHand_Shake = new TiXmlDocument;
} 

CXML_Hand_Shakes::~CXML_Hand_Shakes()
{
	delete mpXMLHand_Shake;
} 

tbool CXML_Hand_Shakes::Set_Take_URL(std::string sTake_Info)
{
	// clear TinyXML document
	mpXMLHand_Shake->Clear();
	
	
	// read sTake_Info in to char buffer
	tuint iSize = sTake_Info.size();
	CAutoBuffer<char> pszsTake_Info;
	pszsTake_Info.Allocate(iSize);
	
	// parse pszsTake_Info  in to a TinyXML DOM
	mpXMLHand_Shake->Parse(pszsTake_Info);
	
	// pass the TinyXML DOM in to the DAW data structure
	Pass_Take_URL( mpXMLHand_Shake );
	
	
	
	return true;
}

void CXML_Hand_Shakes::Pass_Take_URL( TiXmlNode* pParent )
{
	// if file is empty return
	if ( !pParent ) return;
	
	// if there is multiply <project> tags only read the first one
	tbool read = true;
	
	TiXmlNode* pChild;
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		if(pChild->Type() == TiXmlNode::ELEMENT && read){
			
			read = false;
			
		}
	}
}






/*

<?xml version="1.0" encoding="UTF-8"?>
 <take uuid="131">
	<sample uuid="af012d9d-0eeb-437e-b3a1-20751c09e542"/>
	<description>No description</description>
	<url>http://assets.koblo.com/mp3s/132/8861a75a-6cfe-442b-8a5d-e618ffb32b0a.mp3</url>
	
	<author id="16">
		<name>Coffee Z-Z-Zombie</name>
	</author>
 </take>




*/