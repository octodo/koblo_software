
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
	
	TiXmlNode* pChild;
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			if (stricmp("url", pChild->Value()) == 0){
				
				TiXmlNode* pChild = pChild->FirstChild();
				
				if ( pChild ){
					TiXmlText* pText;
					pText = pChild->ToText();
					gpApplication->Project_Name(pText->Value());
				}
			}
			
		}
	}
}


void CXML_Hand_Shakes::Test_Hand_Shakes()
{
	std::string sPath = gpApplication->Project_Folder() + "takeinfo.xml";
	
	// read project in to 
	CAutoDelete<IFile> pFile(IFile::Create());
	if (pFile->Open(sPath.c_str(), IFile::FileRead)) {
		
		// read project in to char buffer
		tuint iSize = pFile->GetSizeWhenOpened();
		CAutoBuffer<char> pszsTake_Info;
		pszsTake_Info.Allocate(iSize);
		
		pFile->Read(pszsTake_Info, iSize);
		
		// parse pszProjectXML in to a TinyXML DOM
		mpXMLHand_Shake->Parse(pszsTake_Info);
		
	}

}








