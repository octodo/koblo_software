#include "KSOS.h"

CTake_Data::CTake_Data()
{
	msDescription			=		"";
	msURL					=		"";
	muiTake_Nr				=		0;
//	muiChannels				=		0;
	sWavePathNameL			=		"";
	sWavePathNameR			=		"";
	sLeft_Peak_File_Path	=		"";
	sRight_Peak_File_Path	=		"";

}

CTake_Data::~CTake_Data()
{
	
}
/*
void CTake_Data::Name(std::string sName, tuint uiChannels)
{
	
	msName			=	sName;
	muiChannels		=	uiChannels;

}
*/
std::string CTake_Data::Name( tuint32 uiChannel)
{

	std::string sName;
	sName				=	msName;
	
	
	// take
	if(muiTake_Nr > 9999)  muiTake_Nr = 9999;
	char Char_Buffer[8];
//	sprintf(Char_Buffer, "%04d", muiTake_Nr );
 	sprintf(Char_Buffer, "%04d", 0 ); // only one take supported for now
	sName	+= std::string("_");
	sName	+= std::string(Char_Buffer);
	

	// channel
	if(uiChannel > 99)  uiChannel = 99;
	sprintf(Char_Buffer, "%02d", uiChannel ); 	
	
	sName	+= std::string("-");
	sName	+= std::string(Char_Buffer);

	return sName; 

}