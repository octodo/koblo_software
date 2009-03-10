#include "KSOS.h"

CKSUUID::CKSUUID()
{

	msUUID_X	=	"";

}

CKSUUID::~CKSUUID()
{
	
}

CKSUUID::CKSUUID(const CKSUUID& Data_Old)
{

}

void CKSUUID::Check_UUID()
{
	if(msUUID_X.size() == 0)
		Generate_UUID();
}


std::string CKSUUID::Get_UUID()
{
	Check_UUID();
	return msUUID_X;
}

void CKSUUID::Generate_UUID()
{
	
	tchar* pszUUID = new tchar[128];
	*pszUUID = '\0';
	tuint32 uiBuffer_Size = 128;
	
	Gen_UUID(pszUUID, uiBuffer_Size);
	msUUID_X = pszUUID;
	
}

tbool CKSUUID::UUID_Is_Set()
{
	return  msUUID_X.size() != 0;
}

std::string CKSUUID::Get_New_UUID()
{
	Clear_UUID();
	return Get_UUID();
	 
}