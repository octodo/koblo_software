#include "KSOS.h"

CTake_Data::CTake_Data()
{
	msDescription			=		"";
	msURL					=		"";
	msLeft_Wave_File_Path	=		"";
	msRight_Wave_File_Path	=		"";
	msLeft_Peak_File_Path	=		"";
	msRight_Peak_File_Path	=		"";


}

CTake_Data::~CTake_Data()
{
	
}

std::string CTake_Data::Disk_Name( tuint32 uiChannel)
{

	std::string sDisk_Name;
	sDisk_Name				=	Get_UUID();

	// channel
	char Char_Buffer[8];
	if(uiChannel > 99)  uiChannel = 99;
	sprintf(Char_Buffer, "%02d", uiChannel ); 	
	
	sDisk_Name	+= std::string("_");
	sDisk_Name	+= std::string(Char_Buffer);

	return sDisk_Name; 

}