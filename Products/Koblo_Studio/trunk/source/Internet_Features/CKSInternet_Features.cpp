
#include "KSOS.h"



CKSInternet_Features::CKSInternet_Features(CKSPlugIn * pKSPlugIn)
{
	
	mpKSPlugIn = pKSPlugIn;
	
}

CKSInternet_Features::~CKSInternet_Features()
{
	
}


void CKSInternet_Features::Download_Project()
{
	
	Upload_Project(); return;
	
	
	
	mpKSPlugIn->CleanProject(0);
	
	tint32 iProject_ID = mpKSPlugIn->GetGlobalParm(giParamID_Project_ID, giSectionGlobal);
	std::string str = "";
	char psz[128];
	sprintf(psz, "branches/%d/revisions/latest", iProject_ID);
	str = psz;
	
	
	mpKSPlugIn->ReadOnlineXML(str);
	
//	printf(str.c_str());
	
}

void CKSInternet_Features::Update_Project()
{
	
}

void CKSInternet_Features::Upload_Project()
{
	
	Commit_Project();
	
}

void CKSInternet_Features::Commit_Project()
{
	tint32 iProject_ID = mpKSPlugIn->GetGlobalParm(giParamID_Project_ID, giSectionGlobal);	
	mpKSPlugIn->Write_XML( iProject_ID);
}
