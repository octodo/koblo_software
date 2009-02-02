
#include "KSOS.h"


CKSXML_Read_Project::CKSXML_Read_Project()
{
	
//	mpTinyXMLDoc = new TiXmlDocument;
	
}

CKSXML_Read_Project::~CKSXML_Read_Project()
{
//	delete mpTinyXMLDoc;
}

void CKSXML_Read_Project::Read_Project_From_Disk(std::string sFile)
{

	// read project in to 
	CAutoDelete<IFile> pfile(IFile::Create());
	if (pfile->Open(sFile.c_str(), IFile::FileRead)) {
		// reset/ erase the current DAW project
		Reset_Project();
		// read project in to char buffer
		tuint iSize = pfile->GetSizeWhenOpened();
		char pszProjectXML[iSize];
		pfile->Read(pszProjectXML, iSize);
		// parse pszProjectXML in to TinyXML document
		mpTinyXMLDoc->Parse(pszProjectXML);
		// Iterate mpTinyXMLDoc in To DAW data structure
		Pass_The_Project_Tag( mpTinyXMLDoc );
		
		// 
		Prepare_Samples();
		
		// download takes
		Download_Takes();
		// decompress takes
		Decompress_Takes();
			
	}
	
}
/*
void CKSXML_Read_Project::Reset_Project()
{
	miTrack_ID	=	0;
	gpApplication->CleanProject(0);
	mpTinyXMLDoc->Clear();
	
}
*/
void CKSXML_Read_Project::Read_Latest_Revision_From_Koblo(tint32 iProjectID )
{
	// cleanup old tinyxml dom
	mpTinyXMLDoc->Clear();
	
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
		mpTinyXMLDoc->Parse(pszBuff);
		
		//printf(pszBuff);
	}
	ine::IINetUtil::ReleaseBuffer(&pszBuff);
	
}


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
	// set project uuid
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
				pTake_Data->Name( Sample_Data.Name() );
				
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
/*

void CKSXML_Read_Project::Read_Track_Aux(TiXmlNode* pParent, tint32 iTrackID)
{
	TiXmlNode* pChild = pParent->FirstChild();
	
	TiXmlAttribute* pAttrib	=	pChild->FirstAttribute();
		tint32 iAux = -1;
	
	// aux id
	if (pAttrib && pAttrib->QueryIntValue(&iAux)!=TIXML_SUCCESS)    
		return;
	

	Set_DAW_Parameter(pElement, giTinyXml_Type_Float, giParam_ChAUX1 + iAux, giSection_First_Track + iTrackID, 10000.0f);

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

				Set_DAW_Parameter(pChild, giTinyXml_Type_Int, 0, 0);

			}
			else if (stricmp("start", pChild->Value()) == 0) {

				Set_DAW_Parameter(pChild, giTinyXml_Type_Int, 0, 0);

			}
			else if (stricmp("end", pChild->Value()) == 0) {

				Set_DAW_Parameter(pChild, giTinyXml_Type_Int, 0, 0);

			}
			else if (stricmp("volume", pChild->Value()) == 0) {

				Set_DAW_Parameter(pChild, giTinyXml_Type_Float, 0, 0);

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

				Set_DAW_Parameter(pChild, giTinyXml_Type_Int, 0, 0);

			}
			else if (stricmp("out", pChild->Value()) == 0) {

				Set_DAW_Parameter(pChild, giTinyXml_Type_Int, 0, 0);

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
		Import_Takes( (*it) );
	}
	
	
}




void CKSXML_Read_Project::Import_Takes(CSample_Data pSample_Data)
{
	
	//! notice only one take pr sample supported at the moment	
	CTake_Data* pTake_Data = pSample_Data.Get_Take_Data();
	Import_Take( pTake_Data );
	
}

void CKSXML_Read_Project::Import_Take(CTake_Data* pTake)
{
	
	// check if take is in Wave folder
	if( In_Wave_Files(pTake) ) 
		mInsert_Que.push_back(pTake);
	
	// if the take is in the "OGG Folder"
	else if( In_OGG_Files(pTake) )
		mDecompress_Que.push_back(pTake);

	// add take to the download que
	else
		mDownload_Que.push_back(pTake);

	
}

tbool CKSXML_Read_Project::In_Wave_Files(CTake_Data* pTake_Data)
{
	std::string sFolder = gpApplication->Wave_File_Folder();
	return In_Folder(	pTake_Data , sFolder, ".wav" );
	
}


tbool CKSXML_Read_Project::In_OGG_Files(CTake_Data* pTake_Data)
{
	std::string sFolder = gpApplication->OGG_File_Folder();
	return In_Folder(	pTake_Data , sFolder, ".ogg" );

}

tbool CKSXML_Read_Project::In_Folder(CTake_Data* pTake_Data, std::string sFolder, std::string sExtencion)
{
	tuint32 uiChannels = 0;
	
	std::string sMode = pTake_Data->Mode();
	
	if(stricmp( 	sMode.c_str(),	"mono"	) == 0)
		uiChannels = 1;
	
	else if(stricmp( sMode.c_str(),"stereo") == 0)
		uiChannels = 2;
	
	
	switch(uiChannels) {
		case 1:
		{	// mono 
			return gpApplication->Readable_Audio(sFolder + pTake_Data->Name(1));
		}
		case 2:
		{	// files are split in two
			tuint In_Wave_Files = 0;
			In_Wave_Files += gpApplication->Readable_Audio(sFolder + pTake_Data->Name(1) + sExtencion);
			In_Wave_Files += gpApplication->Readable_Audio(sFolder + pTake_Data->Name(2) + sExtencion);
			return In_Wave_Files == 2 ? true: false;
		}
		default: break;
	}
	return false;
}








void CKSXML_Read_Project::Download_Takes()
{
	std::list<CTake_Data*>::iterator it = mDownload_Que.begin();
	for (; it != mDownload_Que.end(); it++) {
		Download_Take( (*it) );
	}
}


void CKSXML_Read_Project::Download_Take(CTake_Data* Take_Data)
{
	std::string sURL = Take_Data->URL();
	// do download here
	// download to "OGG Files" folder
	// when compleated add files to decompressing que:
	// mDecompress_Que.push_back(pTake_Data);
}

void CKSXML_Read_Project::Decompress_Takes()
{
	std::list<CTake_Data*>::iterator it = mDecompress_Que.begin();
	for (; it != mDecompress_Que.end(); it++) {
		Decompress_Take( (*it) );
	}
}


void CKSXML_Read_Project::Decompress_Take(CTake_Data* Take_Data)
{
	std::string sURL = Take_Data->URL();
	// do decompression here
	// decompress to "Wave Files" folder
	// when compleated add files to insert que:
	// mInsert_Que.push_back(pTake_Data);
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
















/*
tbool CKSXML_Read_Project::Insert_Sample()
{
	if (gpApplication->IsPlayingOrRecording())  gpApplication->PlaybackStop();
	
	
	CImportAudioTask* pImportAudioTask = new CImportAudioTask();
	
	CImportAudioTask::EStereoBehavior eBehave = (bAlwaysKeepStereo) ? CImportAudioTask::geStereoDoKeep : CImportAudioTask::geStereoDoAsk;
	
	tbool bSuccess = pImportAudioTask->Init( File_Item.Source_Path(), false, eBehave, false);
	
	if (bSuccess) {
		gpApplication->mpProgressTasks->Add(pImportAudioTask);
		gpApplication->Playback_InProgressTask();
	}
	else {
		gpApplication->Extended_Error(pImportAudioTask->GetError());
		pImportAudioTask->Destroy();
	}
	
	return bSuccess;
}
*/








