
#include "KSOS.h"

CSample_Data::CSample_Data()
{
	mpTake_Data = new(CTake_Data);
} 

CSample_Data::~CSample_Data()
{
	delete mpTake_Data;
} 

CSample_Data::CSample_Data(const CSample_Data& Data_Old)
{

	msName			=	Data_Old.msName;
	mpTake_Data		=	new CTake_Data;
	*mpTake_Data	=	*(Data_Old.mpTake_Data);

	
}





