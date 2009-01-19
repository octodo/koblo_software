
#include "KSOS.h"

CSample_Data::CSample_Data()
{
	mpTake_Data = new(CTake_Data);
	mpTake_Data->Generate_UUID();
	
	msUUID	=	"";
	Generate_UUID();
} 

CSample_Data::~CSample_Data()
{
	delete mpTake_Data;
} 



void CSample_Data::Generate_UUID()
{

	if(msUUID.size() == 0)
		msUUID = gpApplication->Get_UUID();

}