
#include "KSOS.h"


CKSXML_Create_Sample::CKSXML_Create_Sample(CKSPlugIn * pKSPlugIn)
{
	
	mpKSPlugIn = pKSPlugIn;
	
}

CKSXML_Create_Sample::~CKSXML_Create_Sample()
{
	
}

void CKSXML_Create_Sample::Create_Sample_On_Koblo()
{
	std::string str;
	char psz[128];
	sprintf(psz, "/projects.xml");
	str = psz;
	
	
	tchar* pszBuff = NULL;
	tint32 iOutLen = 0;
	
	
	// shell script upload new sample on koblo.com
	/*
	 curl -i \
	 -u "usere@mail.com:password \
	 -F "sample[name]=bebop" \
	 -F "take[description]=first take" \
	 -F "mp3[uploaded_data]=@/Users/name/Desktop/short1.mp3" \
	 http://koblo.com/projects/1/samples.xml
	*/
	
	// username and password has to bein the http autincication header
	// Returns a .xml file
	//ine::IINetUtil::GetWebFile(NULL, "koblo.com", str.c_str(), &iOutLen, &pszBuff);
	
	// Read dummy XML file from HD here
	
	
	if ((pszBuff) && (iOutLen > 0)) {
		
		// create TinyXml document
		TiXmlDocument doc("koblo_studio project");
		// parse XML file in to TinyXml object tree
		doc.Parse(pszBuff);
		
		printf(pszBuff);
		
	}
	ine::IINetUtil::ReleaseBuffer(&pszBuff);
	
}
