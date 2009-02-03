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

class CImport_Auido_Controller
{
	
public:
	//! Constructor
	CImport_Auido_Controller();
	
	//! Destructor
	virtual ~CImport_Auido_Controller();
	
	
	struct SFile_Item {
		std::string sName;
		std::string sPath;
		tbool bIs_A_Dir;
	};
	
	
	

	
protected:
	
	std::list<SFile_Item > mlFile_Item;
	
};