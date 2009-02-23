
#include "KSOS.h"

CXML_Hand_Shakes::CXML_Hand_Shakes()
{
	mpXMLHand_Shake = new TiXmlDocument;
} 

CXML_Hand_Shakes::~CXML_Hand_Shakes()
{
	delete mpXMLHand_Shake;
} 

tbool CXML_Hand_Shakes::Set_Take_Handshake(std::string sTake_Info)
{
	CAutoLock Lock(mHands_Shake_Mutex);
	
	// clear TinyXML document
	mpXMLHand_Shake->Clear();
	
	// clear take uuid
	msTake_UUID.clear();
	msTake_Audio_URL.clear();
	msTake_MP3_URL.clear();
	
	// read sTake_Info in to char buffer
//	tuint iSize = sTake_Info.size();
//	CAutoBuffer<char> pszsTake_Info;
//	pszsTake_Info.Allocate(iSize);
	
	// parse pszsTake_Info  in to a TinyXML DOM
	mpXMLHand_Shake->Parse(sTake_Info.c_str());
	
	// pass the TinyXML DOM in to the DAW data structure
	Pass_Take_Tag( mpXMLHand_Shake );
	

	Update_Take_Data();
	
	gpApplication->Save_Project_As_XML_File_To_Disk();
	
	
	return true;
}


void CXML_Hand_Shakes::Pass_Take_Tag( TiXmlNode* pParent )
{
	// if file is empty return
	if ( !pParent ) return;
	

	
	TiXmlNode* pChild;
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		if(pChild->Type() == TiXmlNode::ELEMENT){
			
			Pass_Take_Tags(pChild);	

		}
	}
}

void CXML_Hand_Shakes::Pass_Take_Tags( TiXmlNode* pParent )
{
	
	if ( !pParent ) return;
	
	TiXmlAttribute* pAttrib	=	pParent->ToElement()->FirstAttribute();
	// take uuid
	if(pAttrib) 
		msTake_UUID =  pAttrib->Value() ;

	// parse all childs
	TiXmlNode* pChild;
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		if(pChild->Type() == TiXmlNode::ELEMENT)
		{
			if (stricmp("sample", pChild->Value()) == 0)
				Pass_Take_Sample(pChild);
			
			else if (stricmp("description", pChild->Value()) == 0) 
				Pass_Take_Description(pParent);
			
			else if (stricmp("audio", pChild->Value()) == 0) 
				Pass_Take_Audio(pChild);
			
			else if (stricmp("mp3", pChild->Value()) == 0) 
				Pass_Take_MP3(pChild);
			
			else if (stricmp("author", pChild->Value()) == 0) 
				Pass_Take_Author(pChild);
		}
	}
	
}
/*
void CXML_Hand_Shakes::Pass_Take_Tag( TiXmlNode* pParent )
{
	
	if (stricmp("sample", pParent->Value()) == 0)
		Pass_Take_Sample(pParent);
	
	else if (stricmp("description", pParent->Value()) == 0) 
		Pass_Take_Description(pParent);
	
	else if (stricmp("audio", pParent->Value()) == 0) 
		Pass_Take_Audio(pParent);
	
	else if (stricmp("mp3", pParent->Value()) == 0) 
		Pass_Take_MP3(pParent);
	
	else if (stricmp("author", pParent->Value()) == 0) 
		Pass_Take_Author(pParent);
	
	
}
 */
void CXML_Hand_Shakes::Pass_Take_Sample( TiXmlNode* pParent )
{
	TiXmlAttribute* pAttrib	=	pParent->ToElement()->FirstAttribute();
	
	// sample uuid
	if(pAttrib) 
		msSampleUUID =  pAttrib->Value() ;

}


void CXML_Hand_Shakes::Pass_Take_Description( TiXmlNode* pParent )
{
	TiXmlNode* pContent = pParent->FirstChild();
	if ( !pContent ) 
		return;
	if (  pContent->Type() == TiXmlNode::TEXT )
	{
		TiXmlText* pText;
		pText = pContent->ToText();
		std::string s = pText->Value();
		/*
		 tint32 iNew_Revision = atoi(s.c_str());
		 if(iNew_Revision > gpApplication->GetGlobalParm(giParamID_Revision_Nr, giSectionGlobal))
		 mbNew_Revision = true;
		 */
	}
	
}


void CXML_Hand_Shakes::Pass_Take_Audio( TiXmlNode* pParent )
{
	if ( !pParent ) return;
	
	TiXmlNode* pChild = pParent->FirstChild();
	
	if ( pChild ){
		TiXmlText* pText;
		pText = pChild->ToText();
		msTake_Audio_URL = pText->Value();
	}
	
}

void CXML_Hand_Shakes::Pass_Take_MP3( TiXmlNode* pParent )
{
	if ( !pParent ) return;
	
	TiXmlNode* pChild = pParent->FirstChild();
	
	if ( pChild ){
		TiXmlText* pText;
		pText = pChild->ToText();
		msTake_MP3_URL = pText->Value();
	}	
}


void CXML_Hand_Shakes::Pass_Take_Author( TiXmlNode* pParent )
{
	

}


void CXML_Hand_Shakes::Update_Take_Data()
{
	if(msTake_UUID.size()){
		
		CTake_Data* pTake_Data	=	gpApplication->Get_Take(msTake_UUID);
		if(pTake_Data){
			
			pTake_Data->URL(msTake_Audio_URL);
			pTake_Data->MP3_URL(msTake_MP3_URL);
			
			
		}
	}
}








void CXML_Hand_Shakes::Pass_Branch_Revision(std::string sBranch_Revision )
{
	//	std::string sTest("<?xml version=1.0 encoding=UTF-8?> \n <revision>9</revision>");
	//	mpXMLHand_Shake->Parse(sTest.c_str());
	
	// clear TinyXML document
	mpXMLHand_Shake->Clear();
	
	// parse pszsTake_Info  in to a TinyXML DOM
	mpXMLHand_Shake->Parse(sBranch_Revision.c_str());
	
	// Pass document in to DAW structure
	Read_Branch_Revision_Tag(mpXMLHand_Shake);
	
}


void CXML_Hand_Shakes::Read_Branch_Revision_Tag(TiXmlNode* pParent)
{
	// if file is empty return
	if ( !pParent ) return;
	
	TiXmlNode* pChild;
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		// filter out comments
		if(pChild->Type() == TiXmlNode::ELEMENT){
			if (stricmp("revision", pChild->Value()) == 0){
				
				
				TiXmlNode* pSet_Branch_Revision = pChild->FirstChild();
				if(pSet_Branch_Revision) {
					std::string sBranch_Revision = pSet_Branch_Revision->Value();
					gpApplication->Branch_Revision(atoi(sBranch_Revision.c_str()));
				}
				
				
			}
			Read_Branch_Revision_Object(pChild);

		}
	}
	
	
}

void CXML_Hand_Shakes::Read_Branch_Revision_Object(TiXmlNode* pParent)
{
	
	TiXmlNode* pChild = pParent->FirstChild();
	
	if ( !pChild ) return;
	
	if (stricmp("revision", pChild->Value()) == 0){
		
		
		
		
		
		
		
		
		
	}
	
}







void CXML_Hand_Shakes::Test_Hand_Shakes()
{
	// clear TinyXML document
	mpXMLHand_Shake->Clear();
	
	// clear take uuid
	msTake_UUID.clear();
	msTake_Audio_URL.clear();
	msTake_MP3_URL.clear();
	
	std::string sPath = gpApplication->Project_Folder() + "takeinfo.xml";
	
	// read project in to 
	CAutoDelete<IFile> pFile(IFile::Create());
	if (pFile->Open(sPath.c_str(), IFile::FileRead)) {
		
		// read project in to char buffer
		tuint iSize = pFile->GetSizeWhenOpened();
		CAutoBuffer<char> pszsTake_Info;
		pszsTake_Info.Allocate(iSize);
		
		pFile->Read(pszsTake_Info, iSize);


		std::string str(pszsTake_Info);
		
		Set_Take_Handshake(str);
		

		
	}

}








