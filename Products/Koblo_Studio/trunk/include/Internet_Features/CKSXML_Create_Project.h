// Copyright 2004, 2005,2006,2007,2008, 2009 Koblo http://koblo.com
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











/*! \class CKSXML_Create_Project
  \brief class that creates an empty project on koblo.com 
 \ koblo.com returns a xml document with the project id that KS needs to upload the project.xml
 \ based on TinyXml www.sourceforge.net/projects/tinyxml
 */




class CKSXML_Create_Project
{

	
public:
	
	CKSXML_Create_Project();
	~CKSXML_Create_Project();
	
	//! creates a new "empty" project on koblo.com
	/*! 
	 \ use user id and password
	 \ pasrses the XML handshake in to the internal data structure
	 */ 
	tbool Create_Project_Handshake();
	
private:

	

	//! parse the returne xml file in to KS
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
	void Write_Project_Info(TiXmlNode* pParent);
	//! set project name
	/*!
	 \param TiXmlNode [in]: TiXmlElement.
	 */
	void Write_Project_Name(TiXmlElement* pElement);
	//! set project description
	/*!
	 \param TiXmlNode [in]: TiXmlElement.
	 */
	void Write_Project_Description(TiXmlElement* pElement);
	//! set project license
	/*!
	 \param TiXmlNode [in]: TiXmlElement.
	 */
	void Write_Project_License(TiXmlElement* pElement);
	
	
	
	
	

	
	
};

