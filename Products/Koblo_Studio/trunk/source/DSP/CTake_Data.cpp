#include "KSOS.h"

CTake_Data::CTake_Data()
{
	msDescription			=		"";
	msURL					=		"";
	msLeft_Wave_File_Path	=		"";
	msRight_Wave_File_Path	=		"";
	msLeft_Peak_File_Path	=		"";
	msRight_Peak_File_Path	=		"";

	mpUUID_ForSample = NULL;
}

CTake_Data::~CTake_Data()
{
	
}

/*
CTake_Data CTake_Data::operator=(const CTake_Data& DataNew)
{
	return *this;
}
*/


void CTake_Data::Mode(const std::string& sMode) 
{	
	msMode = sMode;
	
	muiChannels = Get_Channels_From_Mode();
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

tbool CTake_Data::In_Folder(std::string sFolder, std::string sExtencion)
{
	tbool bOK = false;
	
	
	switch(muiChannels) {
		case 1:
		{	// mono 
			msLeft_Wave_File_Path = sFolder + Disk_Name(1) + sExtencion;
			bOK =  gpApplication->Readable_Audio(msLeft_Wave_File_Path);
			break;
		}
		case 2:
		{	// stereo
			msLeft_Wave_File_Path	= sFolder + Disk_Name(1) + sExtencion;
			msRight_Wave_File_Path	= sFolder + Disk_Name(2) + sExtencion;
			
			tuint In_Wave_Files		= 0;
			
			In_Wave_Files			+= gpApplication->Readable_Audio(msLeft_Wave_File_Path);
			In_Wave_Files			+= gpApplication->Readable_Audio(msRight_Wave_File_Path);
			
			bOK = In_Wave_Files == 2 ? true: false;
			break;
		}
		default: break;
	}
	if(!bOK){
		
		msLeft_Wave_File_Path	= "";
		msRight_Wave_File_Path	= "";
		
	}
	return bOK;
}


std::string CTake_Data::GetOggFilePath()
{
	return gpApplication->Project_Ogg_Folder() + Get_UUID() + ".ogg";
} // GetOggFilePath

std::string CTake_Data::GetMp3FilePath()
{
	return gpApplication->Project_Mp3_Folder() + Get_UUID() + ".mp3";
} // GetMp3FilePath


tbool CTake_Data::Needs_Pict_Files()
{

// peak files
#ifdef _Mac_PowerPC
	std::string sExtencion1		=	".kspk1024_ppc";
	std::string sExtencion2		=	".kspk64_ppc";
#else _Mac_PowerPC
	std::string sExtencion1		=	".kspk1024";
	std::string sExtencion2		=	".kspk64";
#endif // _Mac_PowerPC
	
	
	tbool bOK			= true;
	tuint uiTest;
	
	
	switch(  Channels() ) {
		case 1:
		{	// mono 
			msLeft_Peak_File_Path = gpApplication->Pict_File_Folder() + Disk_Name(1);
			uiTest	=	gpApplication->Is_A_File(msLeft_Peak_File_Path + sExtencion1);
			uiTest	+=  gpApplication->Is_A_File(msLeft_Peak_File_Path + sExtencion2);
			
			bOK = uiTest == 2 ? false: true;
			break;
		}
		case 2:
		{	// stereo
			msLeft_Peak_File_Path = gpApplication->Pict_File_Folder() + Disk_Name(1);
			uiTest	=	gpApplication->Is_A_File(msLeft_Peak_File_Path + sExtencion1);
			uiTest	+=  gpApplication->Is_A_File(msLeft_Peak_File_Path + sExtencion2);
			
			msRight_Peak_File_Path = gpApplication->Pict_File_Folder() + Disk_Name(2);
			uiTest	+=  gpApplication->Is_A_File(msRight_Peak_File_Path + sExtencion1);
			uiTest	+=  gpApplication->Is_A_File(msRight_Peak_File_Path + sExtencion2);
			
			bOK = uiTest == 4 ? false: true;
			break;
		}
		default: break;
	}
	return bOK;
}

tuint32 CTake_Data::Get_Channels_From_Mode()
{
	tuint32 uiChannels = 0;
	if(stricmp( 	msMode.c_str(),	"mono"	) == 0)
		uiChannels = 1;
	
	else if(stricmp( msMode.c_str(),"stereo") == 0)
		uiChannels = 2;
	
	return uiChannels;
	
}









