
#include "KSOS.h"

CSample_Data::CSample_Data()
{
	mpTake_Data = new(CTake_Data);
	
//	mpUUID	=	new(CKSUUID);
} 

CSample_Data::~CSample_Data()
{
	delete mpTake_Data;
} 





