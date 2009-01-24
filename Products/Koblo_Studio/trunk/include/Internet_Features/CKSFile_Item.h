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

class CKSFile_Item
{
	
public:
	//! Constructor
	CKSFile_Item();
	
	//! Destructor
	virtual ~CKSFile_Item();
	
	//! set name
	void Name(std::string sName){msName = sName;};
	
	//! get name
	std::string Name(){ return msName;};
	
	// ! set path
	void Path(std::string sPath){msPath = sPath;};
	
	//! get path
	std::string Path(){ return msPath;};
	
	//! set type file or directory
	void Is_A_Dir(tbool bIs_A_Dir){mbIs_A_Dir = bIs_A_Dir;};
	
	//! get type file or directory
	tbool Is_A_Dir(){ return mbIs_A_Dir;};

	
protected:
	
	std::string msName;
	std::string msPath;
	tbool mbIs_A_Dir;
	
	
};