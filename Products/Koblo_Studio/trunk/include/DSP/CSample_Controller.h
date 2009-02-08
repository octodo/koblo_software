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
// along with the Koblo SDK. If not, see <http://www.gnu.org/licenses/>.



/*! \brief CSample controller object \
 *  handles sample logic 
 */

class CSample_Controller{
	
public:
	//! Constructor
	CSample_Controller();
	
	//! Destructor
	virtual ~CSample_Controller();
	
	tuint64 Get_Sample_Duration_From_Name( const tchar* pszListName);
	
	void Set_Wave_Path( CTake_Data* pTake_Info, 
					   const tchar* pszListName, 
					   std::string& rsWavePathNameL, 
					   std::string& rsWavePathNameR ) const;
	
	CSample_Data* Get_Sample_Data_From_Name(const tchar* pszListName) const;
	
protected:
	
	
	
};