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

//! Called when a buffer of samples needs to be filled
/*!
 \param iID [in]: ID of callback as when SetCallback was called.
 \param pfOut [out]: Buffer to be filled
 \param iCount [in]: Number of samples to fill in buffer
 */

/*! \class CKSXML_Read_Project
 * \brief class that parse the XML data in to the internal KS DataEngine
 based on TinyXml www.sourceforge.net/projects/tinyxml
 */

class CKSApplication;
class CSample_Data;
class CTake_Data;
//class TiXmlDocument;


class CKSXML_Read_Project 
{
	



public:
	
	CKSXML_Read_Project();
	~CKSXML_Read_Project();
	
	
	//! reset project
	void Reset_Project();
	
	//! read a olde binary KSProject file
	void Read_Project_From_Disk();
	
	//! read a project XML file from Koblo.comin to the tiny xml DOM
	void Read_Project_XML_To_DOM(tint32 iProjectID);
	
	//! pass the DOM in to the KS data structure
	void CKSXML_Parse_DOM_To_Preset();

	//! itterate true the project XML file from the root level
	void Parse_Project( TiXmlNode* pParent );
	
	//! handle unknown tags
//	void Ignore_Tag(TiXmlNode* pParent);
	
	//! set projet parameters from XML tag <project> in to KS
	/*!
	 \param TiXmlNode [in]: TiXmlNode.
	 */
	void Read_Project(TiXmlNode* pParent);
	
	//! handle the different ellements at the root level of the xml file sort out comments and the like
	/*!
	 \param TiXmlNode [in]: TiXmlNode.
	 */
	void Parse_Project_Childs(TiXmlNode* pParent);
	
	//! handle the different ellements at the root level of the xml file sort out comments and the like
	/*!
	 \param TiXmlNode [in]: TiXmlNode.
	 */
	void Read_Project_Name(TiXmlNode* pParent);
	
	//! set branch parameters from XML tag <branch> in to KS
	/*!
	 \param TiXmlElement [in]: TiXmlElement.
	 */
	void Read_Branch(TiXmlElement* pElement);
	void Read_Settings(TiXmlElement* pElement);
	void Read_Signature(TiXmlElement* pElement);
	void Update_Signature();
	void Read_Edditing(TiXmlElement* pElement);
	void Read_Tool(std::string sTool);
	
	void Read_Grid(std::string sGrid);
	void Read_Snap(std::string sSnap);
	void Read_View_Waves(std::string sWaves);
	void Read_View_Fades(std::string sFades);
	
	
	
	
	void Read_Sample(TiXmlElement* pElement);
	void Read_Sample_Take(TiXmlElement* pElement, CSample_Data* Sample_Data);
	void Read_Loop(TiXmlElement* pElement);
	void Loop_Active(std::string sFades);
	void Read_Windows(TiXmlElement* pElement);
	void Read_Window(TiXmlNode* pChild, tint32 iId);
	void Open_Window(std::string sOn_Off, tint32 iWindow);
	void Read_Window_Pos(TiXmlNode* pChild, tint32 iId);
	void Read_Window_Size(TiXmlNode* pChild, tint32 iId);
	void Read_Dialogs(TiXmlElement* pElement);
	void Read_Export_Dialog(TiXmlElement* pElement);
	void Read_Insert(TiXmlElement* pElement);
	void Read_Insert_Settings(TiXmlElement* pElement);
	void Read_Insert_Parameter(TiXmlElement* pElement);
	void Read_Track(TiXmlElement* pElement);
	void Read_Track_Name(TiXmlElement* pElement, tint32 iTrackID);
	void Read_Track_In(TiXmlElement* pElement, tint32 iTrackID);
	void Read_Track_Out(TiXmlElement* pElement, tint32 iTrackID);
	void Read_Track_Regions(TiXmlElement* pElement, tint32 iTrackID);
	
	void Read_Track_Solo(TiXmlNode* pParent, tint32 iTrackID);
	void Read_Track_Mute(TiXmlNode* pParent, tint32 iTrackID);
	void Read_Track_Arm(TiXmlNode* pParent, tint32 iTrackID);
//	void Read_Track_Aux(TiXmlNode* pParent, tint32 iTrackID, tint32 iAux);
	
	//void Read_Track_Aux(TiXmlElement* pElement, tint32 iTrackID);
	void Read_Track_Insert(TiXmlElement* pElement, tint32 iTrackID);
	void Read_Track_Region(TiXmlElement* pElement, tint32 iTrackID);
	void Read_Track_Region_Fade(TiXmlElement* pElement, tint32 iTrackID);
	
	void Read_Bus(TiXmlElement* pElement);
	void Read_Bus_Out(TiXmlElement* pElement);
	void Read_Bus_Aux(TiXmlElement* pElement);
	void Read_Bus_Insert(TiXmlElement* pElement);
	
	void Read_Master(TiXmlElement* pElement);
	void Read_Master_Out(TiXmlElement* pElement);
	void Read_Master_Insert(TiXmlElement* pElement);
	void Read_Master_Aux_Return(TiXmlElement* pElement);
	
	/*! iterates the list of samples
	\ each sample holds minimum one take
	\ takes are inspected and downloaded / decompressed if needed
	*/
	void Insert_Samples();
	//! check if take is in the "wave file" folder
	tbool Is_In_Wave_Files(CTake_Data* pTake_Data);
	//! check if take is in the" download:OGG Files" folder
	tbool Is_In_OGG_Files(CTake_Data* pTake_Data);
	
	
	
	/*! add take to download que
	\ if a take file isn't in the "wave file" folder or in the "ogg folder"
	\ it will be added to the download que
	*/
	void Add_Take_To_Download_Que(CTake_Data* pTake_Data);
	
	/*! add take to decompression que
	\ this que is excecuted when all missing take files are downloaded
	*/
	void Add_Take_To_Decompress_Que(CTake_Data* pTake_Data);
	
	/*! add take to import que
	\ this que is executed when all takes are in the "wave file" folder
	\ takes from this que is inserted to the list of samples
	*/
	void Add_Take_To_Insert_Que(CTake_Data* pTake_Data);
	

	
	void Set_Param( TiXmlNode* pParent, tuint uiType, tuint32 uiID, tint32 Section, tfloat fFactor = 1.0f );
	
	
	
	
	
	
	tbool Check_For_Newer_Revision(tint32 iProject_ID);
	//! itterate true the project XML file from the root level
	void Check_Project( TiXmlNode* pParent );
	
	void Check_Project_Childs(TiXmlNode* pParent);
	
	void Check_Branch(TiXmlNode* pParent);
	
	void Check_Revision(TiXmlElement* pElement);
	
	
	

private:
	
	
	tint32 miTrack_ID;
	
	// create TinyXml document
	TiXmlDocument* mpDoc;
	
	tbool mbNew_Revision;
	
	tuint32 muiNummerator;
	tuint32 muiDominator;
	
	
	
	// list of sample files to load
	std::list<CSample_Data> mSample_Data_List;
	
	// list of takes files to load
	std::list<CTake_Data> mTake_Download_Que;
	
	// list of takes files to load
	std::list<CTake_Data> mTake_Decompress_Que;
	
	// list of takes files to load
	std::list<CTake_Data> mTake_Insert_Que;
	
};

