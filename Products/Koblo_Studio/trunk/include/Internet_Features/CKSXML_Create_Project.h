// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
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

//! Called when a buffer of samples needs to be filled
/*!
 \param iID [in]: ID of callback as when SetCallback was called.
 \param pfOut [out]: Buffer to be filled
 \param iCount [in]: Number of samples to fill in buffer
 */

/*! \class CKSXML_Create_Project
 * \brief class that parse the XML data in to the internal KS DataEngine
 based on TinyXml www.sourceforge.net/projects/tinyxml
 */

class CKSPlugIn;


class CKSXML_Create_Project
{
	
private:
	CKSPlugIn* mpKSPlugIn;
	

	
	
/*! Koblo Studio music project XML format
	ids are originally returned from the website, and are globally unique (between different projects) 
	ids are always specified as attibutes 
	all times and track positions are specified in sample points
 */
	virtual void Parse_XML( TiXmlNode* pParent );
	
	//! parse projet info xml file returned from koblo.com 
	/*!
	 \param TiXmlNode [in]: TiXmlNode.
	 */
	void Parse_Project_Info(TiXmlNode* pParent);
	
	//! set projet parameters
	/*!
	 \param TiXmlNode [in]: TiXmlNode.
	 */
	void Set_Project_Info(TiXmlNode* pParent);
	//! set project name
	void Set_Project_Name(TiXmlElement* pElement);
	//! set project description
	void Set_Project_Description(TiXmlElement* pElement);
	//! set project license
	void Set_Project_License(TiXmlElement* pElement);
	
	
	
	
	
	
public:
	
	CKSXML_Create_Project(CKSPlugIn * pKSPlugIn);
	~CKSXML_Create_Project();
	
	//! create a project on koblo.com
	/*! 
	 \ use user id and password
	 \ return XML and pass it on to the XML passer
	 */ 
	tbool Create_Project_On_Koblo();
	
	
};

