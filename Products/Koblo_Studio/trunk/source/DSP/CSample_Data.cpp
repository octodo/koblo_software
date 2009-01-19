
#include "KSOS.h"

CSample_Data::CSample_Data()
{
	mpTake_Data = &msTake_Data;
	mpTake_Data->Generate_UUID();
	
	msUUID	=	"";
	Generate_UUID();
} 

CSample_Data::~CSample_Data()
{

} // destructor



void CSample_Data::Generate_UUID()
{

	if(msUUID.size() == 0)
		msUUID = gpApplication->Get_UUID();

}