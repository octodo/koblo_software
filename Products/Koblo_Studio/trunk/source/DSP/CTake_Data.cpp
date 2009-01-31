#include "KSOS.h"

CTake_Data::CTake_Data()
{
	msDescription = "";
	msURL	=	"";
	muiTake_Nr	=	0;

}

CTake_Data::~CTake_Data()
{
	
}

void CTake_Data::Name(std::string sName, tuint uiChannels)
{
	
	msName			=	sName;
	muiChannels		=	uiChannels;

}

std::string CTake_Data::Name( tuint32 uiChannel)
{
	std::string sExencion = "";
	
	
	// take
	if(muiTake_Nr > 9999)  muiTake_Nr = 9999;
	char Char_Buffer[8];
	sprintf(Char_Buffer, "%04d", muiTake_Nr ); 	
	sExencion = "_";
	sExencion = + Char_Buffer;
	

	// channel
	if(muiTake_Nr > 9999)  muiTake_Nr = 9999;
	sprintf(Char_Buffer, "%04d", uiChannel ); 	
	
	sExencion	+= "-";
	sExencion	+= Char_Buffer;

	return sExencion; 

}