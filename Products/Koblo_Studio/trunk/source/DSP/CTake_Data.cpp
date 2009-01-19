#include "KSOS.h"

CTake_Data::CTake_Data()
{
	msDescription = "NA";
	msUUID	=	"";
	msURL	=	"";
}

CTake_Data::~CTake_Data()
{
	
}

void CTake_Data::Generate_UUID()
{
	
	if(msUUID.size() == 0)
		msUUID = gpApplication->Get_UUID();
	
}