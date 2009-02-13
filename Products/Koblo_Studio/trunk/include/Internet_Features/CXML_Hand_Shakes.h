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

//!!! TO ASK is there always only one channel in a sound?
//#define MAX_CHANNELS_IN_SOUND 0

/*! \brief CSample process on CWave_File \
 *  trim start and end \
 */

class CXML_Hand_Shakes:
public virtual CKSUUID
{
public:
	CXML_Hand_Shakes();
	
	virtual ~CXML_Hand_Shakes();
	
	tbool Set_Take_URL(std::string sTake_Url);


	

protected:

	//  TinyXml document
	TiXmlDocument* mpTinyXMLDoc;
	
};

