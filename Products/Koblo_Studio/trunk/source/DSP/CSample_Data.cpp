
#include "KSOS.h"

CSample_Data::CSample_Data()
{
	mpTake_Data = new(CTake_Data);
} 

CSample_Data::~CSample_Data()
{
	delete mpTake_Data;
} 

CSample_Data::CSample_Data(const CSample_Data& DataOld)
{

	sName			=	DataOld.sName;
	mpTake_Data		=	new CTake_Data;
	*mpTake_Data	=	*(DataOld.mpTake_Data);
//	mpTake_Data = new CTake_Data(*(DataOld.mpTake_Data));;

	
}



