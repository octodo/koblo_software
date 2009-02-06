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



/*! \brief CSample data object \
 *  holds sample info 
 */

class CSample_Data:
public virtual CKSUUID
{
	
public:
	//! Constructor
	CSample_Data();
	
	//! Destructor
	virtual ~CSample_Data();
	
	//! copy constructer
	CSample_Data(const CSample_Data& DataNew);
	
	//! Set sample name
	void Name( std::string Name){ msName = Name;};
	
	//! get sample name
	std::string Name(){ return msName;};
	
	//! get a pointer to the takes
	CTake_Data* Get_Take_Data(){ return mpTake_Data;};
	
	//! finds a take from an given UUID
	CTake_Data* Get_Take_Data(std::string sUUID);
	
	//! finds a take from an given UUID
	CTake_Data* Get_Take_Data(tuint32 uiTake);
	
	tuint32 Number_Of_Takes(){ return muiTakes;};
	
protected:
	
	//! name of clip as seen in the list
	std::string msName;
	
	//!!! TO DO turn this in to a stdlist of takes later
	CTake_Data* mpTake_Data;
	
	tuint32 muiTakes;
	
	
};