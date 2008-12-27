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

/*! \class CKSXML_Read
 * \brief class that parse the XML data in to the internal KS DataEngine
 based on TinyXml www.sourceforge.net/projects/tinyxml
 */

class CKSPlugIn;


class CKSXML_Read 
{
	
private:
	CKSPlugIn* mpKSPlugIn;

public:
	
	CKSXML_Read(CKSPlugIn * pKSPlugIn);
	~CKSXML_Read();
	
	//! read the XML file from Koblo.com
	void ReadOnlineXML(std::string str);

	//! itterate true the XML file from the root level
	void Parse_Root( TiXmlNode* pParent );
	
	//! handle unknown tags
	void Ignore_Tag(TiXmlNode* pParent);
	
	//! set projet parameters from XML tag <project> in to KS
	/*!
	 \param TiXmlNode [in]: TiXmlNode.
	 */
	void Set_Project(TiXmlNode* pParent);
	
	//! handle the different ellements at the root level of the xml file sort out comments and the like
	/*!
	 \param TiXmlNode [in]: TiXmlNode.
	 */
	void Parse_Project_Childs(TiXmlNode* pParent);
	
	//! set branch parameters from XML tag <branch> in to KS
	/*!
	 \param TiXmlElement [in]: TiXmlElement.
	 */
	void Set_Branch(TiXmlElement* pElement);
	void Set_Settings(TiXmlElement* pElement);
	void Set_Signature(TiXmlElement* pElement);
	void Update_Signature(TiXmlElement* pElement);
	void Set_Edditing(TiXmlElement* pElement);
	void Set_Sample(TiXmlElement* pElement);
	void Set_Sample_Take(TiXmlElement* pElement);
	void Set_Loop(TiXmlElement* pElement);
	void Set_Windows(TiXmlElement* pElement);
	void Set_Window(TiXmlNode* pChild, tint32 iId);
	void Set_Window_Pos(TiXmlNode* pChild, tint32 iId);
	void Set_Window_Size(TiXmlNode* pChild, tint32 iId);
	void Set_Dialogs(TiXmlElement* pElement);
	void Set_Export_Dialog(TiXmlElement* pElement);
	void Set_Insert(TiXmlElement* pElement);
	void Set_Insert_Settings(TiXmlElement* pElement);
	void Set_Insert_Parameter(TiXmlElement* pElement);
	void Set_Track(TiXmlElement* pElement);
	void Set_Track_In(TiXmlElement* pElement);
	void Set_Track_Out(TiXmlElement* pElement);
	void Set_Track_Aux(TiXmlElement* pElement);
	void Set_Track_Insert(TiXmlElement* pElement, tint32 uTrack);
	void Set_Track_Region(TiXmlElement* pElement);
	void Set_Track_Region_Fade(TiXmlElement* pElement);
	
	void Set_Bus(TiXmlElement* pElement);
	void Set_Bus_Out(TiXmlElement* pElement);
	void Set_Bus_Aux(TiXmlElement* pElement);
	void Set_Bus_Insert(TiXmlElement* pElement);
	
	void Set_Master(TiXmlElement* pElement);
	void Set_Master_Out(TiXmlElement* pElement);
	void Set_Master_Insert(TiXmlElement* pElement);
	void Set_Master_Aux_Return(TiXmlElement* pElement);

	
	void Set_Param( TiXmlNode* pParent, tuint uiType, tuint32 uiID, tint32 Section, tfloat fFactor = 1.0f );

	
	
};

