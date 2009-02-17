#include "KSOS.h"

CKSUUID::CKSUUID()
{

	msUUID	=	"";

}

CKSUUID::~CKSUUID()
{
	
}
void CKSUUID::Check_UUID()
{
	if(msUUID.size() == 0)
		Generate_UUID();
}


std::string CKSUUID::Get_UUID()
{
	Check_UUID();
	return msUUID;
}

void CKSUUID::Generate_UUID()
{
	
	tchar* pszUUID = new tchar[128];
	*pszUUID = '\0';
	tuint32 uiBuffer_Size = 128;
	
	Gen_UUID(pszUUID, uiBuffer_Size);
	msUUID = pszUUID;
	
}

tbool CKSUUID::UUID_Is_Set()
{
	return  msUUID.size() != 0;
}

std::string CKSUUID::Get_New_UUID()
{
	Clear_UUID();
	return Get_UUID();
	 
}