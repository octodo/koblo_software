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

//! Called when a buffer of samples needs to be filled
/*!
 \param iID [in]: ID of callback as when SetCallback was called.
 \param pfOut [out]: Buffer to be filled
 \param iCount [in]: Number of samples to fill in buffer
 */

/*! \class CKSXML_Write_Project
 * \brief class that parse the XML data in to the internal KS DataEngine
 based on TinyXml www.sourceforge.net/projects/tinyxml
 */


class CSample_Data;
class CTake_Data;
class CPreset_Data;

class CKSXML_Write_Project
{
	
public:
	
	CKSXML_Write_Project();
	
	~CKSXML_Write_Project();
	
	//! get the internal data as a xml text string
	std::string Get_Internal_Data_As_XML();
	
	//! write the XML file to Koblo.com
	void Upload_Project_As_XML_File_To_Koblo( tint32 iProjectID);
	
	
	
	std::string Create_License_String();
	
	tbool Save_Project_As_XML_File_To_Disk();
	
	
	
	
	
	
	
	
private:

	
	void Add_Comment(TiXmlDocument* doc, std::string str);
	void Add_Comment(TiXmlElement* pParent,std::string str);
	
	
/*! Koblo Studio music project XML format
	most ids are originally returned from the website, and are globally unique (between different projects) 
	ids are always specified as attibutes 
	all times and track positions are specified in sample points
 */
	//! Root of TinyXML DOM
	void Write_Project(TiXmlDocument* doc);
	
	//! information about the branch
	void Write_Branch(TiXmlElement* pParent);
	
	//! overall project settings
	void Write_Settings(TiXmlElement* pParent);
	
	void Write_Signature(TiXmlElement* pParent);
	
	void Write_License(TiXmlElement* pParent);
	
	//! editing settings. these are things that would typically not be moved/merged between branches
	void Write_Editing(TiXmlElement* pParent);
	
	void Write_Tool(TiXmlElement* pParent);
	
	void Write_Snap(TiXmlElement* pParent);
	
	void Write_Loop(TiXmlElement* pParent);
	
	void Write_Windows(TiXmlElement* pParent);
	
	void Write_Window(TiXmlElement* pParent, tint32 iWindow, tbool bShow = true);
	
	void Write_Window_Position(TiXmlElement* pParent, tuint uiPosX, tuint uiPosY);
	
	void Write_Window_Size(TiXmlElement* pParent, tuint uiSizeX, tuint uiSizeY);
	
	
	//! write samples
	void Write_Samples(TiXmlElement* pParent);
	
	void Write_Sample(TiXmlElement* pParent, CSample_Data* pSample_Data);
	
	
	
	
	
	//! write samples
//	void Write_Samples(TiXmlElement* pParent);
	
//	void Write_Sample(TiXmlElement* pParent, CSample_Data* pSample_Data);
	
	
	
	
	
	void Write_Take(TiXmlElement* pParent, CTake_Data* pTake_Data);
	
	//! tracks
	void Write_Tracks(TiXmlElement* pParent);
	
	void Write_Track(TiXmlElement* pParent, tuint uiTrack);
	
	void Write_Track_In(TiXmlElement* pParent, tuint uiTrack);
	
	void Write_Track_Out(TiXmlElement* pParent, tuint uiTrack);
	
	void Write_Track_Inserts(TiXmlElement* pParent, tuint uiTrack);
	
	void Write_Track_Insert(TiXmlElement* pParent, tuint uiTrack, tuint uiInsert);
	
	void Write_Track_Regions(TiXmlElement* pParent, tuint uiTrack);


	//! AUX
	void Write_AUXs(TiXmlElement* pParent, tuint uiTrack);
	
	void Write_AUX(TiXmlElement* pParent, tuint uiTrack, tuint iAux); 
	
	//! bus
	void Write_Busses(TiXmlElement* pParent);
	
	void Write_Bus(TiXmlElement* pParent, tuint uiID);
	
	void Write_Bus_Out(TiXmlElement* pParent, tuint uiID);
	
	
	//! create a new plug-in settings file
	tbool Create_Plugin_Setting(CPreset_Data* pPreset_Data);

	
	
	
	
};

