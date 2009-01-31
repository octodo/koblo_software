
#include "KSOS.h"

CKSFile_Item::CKSFile_Item()
{
	
} 

CKSFile_Item::~CKSFile_Item()
{

} 


tbool CKSFile_Item::Import(std::string sFull_Path)
{
	msExtencion		=	msSource_Path	=	sFull_Path;
	mbIs_A_Dir		=	false;
	
	
	// Build destination path
	msDestination_Path = gpApplication->Project_Folder() + ":Wave Files:";
	
	// Extract the file name from path
	tint iPos		=	sFull_Path.find_last_of(":");
	msName			=	sFull_Path.substr(iPos + 1, sFull_Path.length());
	msPath			=	sFull_Path.substr(0, iPos + 1);
	iPos			=	msExtencion.find_last_of(".");
	msExtencion		=	msExtencion.substr(iPos, msExtencion.length());
	
	iPos			=	msName.find_last_of(".");
	msName			=	msName.substr(0, iPos);
	
	// can not import in to it self
	if (stricmp( msDestination_Path.c_str(), msPath.c_str()) == 0)
		return false;
	
	return gpApplication->Readable_Audio(msSource_Path);
	
	
}



std::string CKSFile_Item::Left_Path()
{
	// format xxxx-n xxxx =  take nr   n = channel
	std::string sNameL = msDestination_Path + msName + "_0000-1.wav";
	return sNameL;
}

std::string  CKSFile_Item::Right_Path()
{
	// format xxxx-n xxxx =  take nr   n = channel
	std::string sNameR = msDestination_Path + msName + "_0000-2.wav";
	return sNameR;
}


std::string CKSFile_Item::Left_Name()
{
	std::string sNameL = msName + "_0000-1";
	return sNameL;
}

std::string  CKSFile_Item::Right_Name()
{
	std::string sNameR =  msName + "_0000-2";
	return sNameR;
}



std::string  CKSFile_Item::Left_Peak_File_Path()
{
	std::string sNameR = gpApplication->Project_Folder() + ":Wave Picts:" + msName + "_0000-1";
	return sNameR;
}
//! get peak file name for right side
std::string  CKSFile_Item::Right_Peak_File_Path()
{
	std::string sNameR = gpApplication->Project_Folder() + ":Wave Picts:" + msName + "_0000-2";
	return sNameR;
}







