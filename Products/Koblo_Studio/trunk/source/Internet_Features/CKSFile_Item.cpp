
#include "KSOS.h"

CKSFile_Item::CKSFile_Item()
{
	Get_UUID();
	
} 

CKSFile_Item::~CKSFile_Item()
{

} 


tbool CKSFile_Item::Import(std::string sFull_Path)
{
	msExtencion		=	msSource_Path	=	sFull_Path;
	mbIs_A_Dir		=	false;
	
	// Extract the file name from path
	tint iPos		=	sFull_Path.find_last_of(":");
	msName			=	sFull_Path.substr(iPos + 1, sFull_Path.length());
	msPath			=	sFull_Path.substr(0, iPos + 1);
	iPos			=	msExtencion.find_last_of(".");
	msExtencion		=	msExtencion.substr(iPos, msExtencion.length());
	
	iPos			=	msName.find_last_of(".");
	msName			=	msName.substr(0, iPos);
	
	// can not import in to it self
	if (stricmp( gpApplication->Project_Folder().c_str(), msPath.c_str()) == 0)
		return false;
	
	return gpApplication->Readable_Audio(msSource_Path);
	
	
}

void CKSFile_Item::Load(std::string sName)
{
	msName = sName;
	

}

std::string  CKSFile_Item::Destination_Path()
{
	return gpApplication->Project_Folder() + "Wave Files:";
}

std::string CKSFile_Item::Left_Path()
{
	std::string sPathL = gpApplication->Wave_File_Folder() + Get_UUID() + "_01.wav";
	return sPathL;
}

std::string  CKSFile_Item::Right_Path()
{
	std::string sPathR = gpApplication->Wave_File_Folder() + Get_UUID() + "_02.wav";
	return sPathR;
}


std::string CKSFile_Item::Left_Name()
{
	std::string sNameL = Get_UUID() + "_01";
	return sNameL;
}

std::string  CKSFile_Item::Right_Name()
{
	std::string sNameR =  Get_UUID() + "_02";
	return sNameR;
}



std::string  CKSFile_Item::Left_Peak_File_Path()
{
	std::string sPath = gpApplication->Pict_File_Folder()  + Left_Name();
	return sPath;
}
//! get peak file name for right side
std::string  CKSFile_Item::Right_Peak_File_Path()
{
	if(mbStereo)
		return  gpApplication->Pict_File_Folder() + Right_Name();
	else 
		return "";
}







