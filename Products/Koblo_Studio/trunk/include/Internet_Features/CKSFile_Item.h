// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
//
// This file is part of the Koblo SDK.
//
// the Koblo SDK is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo SDK is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.



/*! \brief Operations related to import of Audio \
 *  holds sample info 
 */



class CKSFile_Item:  public virtual CKSUUID
{
	
public:
	//! Constructor
	CKSFile_Item();
	
	//! Destructor
	virtual ~CKSFile_Item();
	
	//! init set name path and extencion from full path
	tbool Import(std::string sFull_Path);
	
	//! init set name path and extencion name
	void Load(std::string sName);
	
	//! set name
	void Name(std::string sName){msName = sName;};
	
	//! get name
	std::string Name(){ return msName;};
	
	// ! set path
	void Path(std::string sPath){msPath = sPath;};
	
	//! get path
	std::string Path(){ return msPath;};
	
	// ! set source path
	void Source_Path(std::string sPath){msSource_Path = sPath;};
	
	//! get source path
	std::string Source_Path(){ return msSource_Path;};
	
	// ! set destination path
//	void Destination_Path(std::string sPath){msDestination_Path = sPath;};
	
	//! get destination path
	std::string Destination_Path();
	
	
	// !set  extencion
	void Extencion(std::string sExtencion){msExtencion = sExtencion;};
	
	//! get extencion
	std::string Extencion(){ return msExtencion;};
	
	//! set type file or directory
	void Is_A_Dir(tbool bIs_A_Dir){mbIs_A_Dir = bIs_A_Dir;};
	
	//! get type file or directory
	tbool Is_A_Dir(){ return mbIs_A_Dir;};
	
	//! get file name and path for left side
	std::string  Left_Path();
	//! get file name and path for right side
	std::string  Right_Path();
	
	//! get file name and path for left side
	std::string  Left_Name();
	//! get file name and path for right side
	std::string  Right_Name();
	
	//! get peak file name for left side
	std::string  Left_Peak_File_Path();
	//! get peak file name for right side
	std::string  Right_Peak_File_Path();
	
	//! set stereo
	void Stereo(tbool bStereo){ mbStereo =  bStereo;};
	
	//! get stereo
	tbool Stereo(){ return mbStereo;};
	

	
protected:
	
	
	std::string msName;
	std::string msPath;
	std::string msExtencion;
	std::string msSource_Path;
//	std::string msDestination_Path;
	tbool mbIs_A_Dir;
	tbool mbStereo;
	
	
	
	
	
};