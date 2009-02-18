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

/*! \class CImport_TinyXML
 * \brief class that parse the TinyXML DOM in to the DAW's data structure
 based on TinyXml www.sourceforge.net/projects/tinyxml
 */

class CKSApplication;
class CSample_Data;
class CTake_Data;

class CImport_TinyXML 
{
	



public:
	
	CImport_TinyXML();
	~CImport_TinyXML();
	
	
	/*! reset the DAW's internal project structure
	 */
	void Reset_Project();
	

	
	/*! pass the TinyXML DOM in to the KS data structure
	*/
	virtual void CKSXML_Parse_DOM_To_DAW();
	
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
	virtual void Read_Project_Name(TiXmlNode* pParent);
	

	
	/*! read the <settings> TinyXML DOM 
	 \param TiXmlElement [in]: TiXmlElement
	 */
	virtual void Read_Settings_Object(TiXmlElement* pElement);
	
	/*! read the <signature> tag from TinyXML DOM 
	 \param TiXmlElement [in]: TiXmlElement
	 */
	virtual void Read_Signature_Object(TiXmlElement* pElement);
	
	/*! parse the <editing> TinyXML DOM 
	 \param TiXmlElement [in]: TiXmlElement.
	 */
	virtual void Parse_Edditing_Object(TiXmlElement* pElement);
	
	
	
	
	
	virtual void Parse_Windows_Object(TiXmlElement* pElement);
	virtual void Read_Window(TiXmlNode* pChild, tint32 iId);
	virtual void Open_Window(std::string sOn_Off, tint32 iWindow);
	virtual void Read_Window_Pos(TiXmlNode* pChild, tint32 iId);
	virtual void Read_Window_Size(TiXmlNode* pChild, tint32 iId);
	virtual void Read_Dialogs(TiXmlElement* pElement);
	virtual void Read_Export_Dialog(TiXmlElement* pElement);
	
	virtual void Read_Insert(TiXmlElement* pElement);
	virtual void Read_Insert_Settings(TiXmlElement* pElement);
	virtual void Read_Insert_Parameter(TiXmlElement* pElement);

	

	
	/*! Iterates the list of samples
	 \ each sample holds a list of takes with minimum one take
	 \ determin witch take has to be decompressed / downloaded
	 \ resoult are stored in the tree lists 
	 \ mInsert_Que, mDownload_Que and mDecompress_Que
	 */
	void Import_Samples();
	
	/*! iterates the list of takes
	 \ takes are inspected and downloaded / decompressed if needed
	 */
	void Inspect_Takes(CSample_Data Sample_Data);
	
	//! inspect one take
	void Inspect_Take( CTake_Data* pTake_Data);
	
	//! check if take is in the "wave file" folder
	tbool In_Wave_Files( CTake_Data* pTake_Data);
	
	//! check if take is in the" download:OGG Files" folder
	tbool In_OGG_Files( CTake_Data* pTake_Data);
	
	//! check if take is in the folder
	tbool In_Folder( CTake_Data* pTake_Data, std::string sFolder);
	
	
	
	/*! add take to download que
	\ if a take file isn't in the "wave file" folder or in the "ogg folder"
	\ it will be added to the download que
	*/
//	void Add_Take_To_Download_Que( CTake_Data* pTake_Data);
	
	/*! add take to decompression que
	\ this que is excecuted when all missing take files are downloaded
	*/
//	void Add_Take_To_Decompress_Que( CTake_Data* pTake_Data);
	
	/*! add take to import que
	\ this que is executed when all takes are in the "wave file" folder
	\ takes from this que is inserted to the list of samples
	*/
//	void Add_Take_To_Insert_Que( CTake_Data* pTake_Data);
	

	
	/*! Set a parameter in the DAW
	 \ param TiXmlNode [in]: TiXmlNode.
	 \ param uiType [in]: defined in ETinyXmlTypes.
	 \ param iParamID [in]: parameter id.
	 \ param uiSection [in]: chunck / section
	 \ param fFactor [in]: factor used to multiply / devide internal DAW values
	 */
	void Set_DAW_Parameter( TiXmlNode* pParent, tuint uiType = 0, tuint32 uiParamID= 0, tint32 uiSection = 0, tfloat fFactor = 1.0f );
	
	
	
	
	tbool Check_For_Newer_Revision(tint32 iProject_ID);
	//! itterate true the project XML file from the root level
	void Check_Project( TiXmlNode* pParent );
	
	void Check_Project_Childs(TiXmlNode* pParent);
	
	void Check_Branch(TiXmlNode* pParent);
	
	void Check_Revision(TiXmlElement* pElement);
	
	//  TinyXml document
	TiXmlDocument* mpTinyXMLDoc;
	

private:
	
	
	tint32 miTrack_ID;
	
	
	
	tbool mbNew_Revision;
	
	tuint32 muiNummerator;
	tuint32 muiDominator;
	
	/*! updates the signature
	\ from the Nummerator and the Dominator
	*/
	void Update_Signature();
	
	/*! read the <tool> TinyXML DOM 
	 \param TiXmlElement [in]: TiXmlElement.
	 */
	void Read_Tool_Object(std::string sTool);
	
	/*! read the <grid> TinyXML DOM 
	 \param TiXmlElement [in]: TiXmlElement.
	 */
	void Read_Grid(std::string sGrid);
	
	/*! read the <snap> TinyXML DOM 
	 \param TiXmlElement [in]: TiXmlElement.
	 */
	void Read_Snap(std::string sSnap);
	
	/*! read the <waves> TinyXML DOM 
	 \param TiXmlElement [in]: TiXmlElement.
	 */
	void Read_View_Waves(std::string sWaves);
	
	/*! read the <fades> TinyXML DOM 
	 \param TiXmlElement [in]: TiXmlElement.
	 */
	void Read_View_Fades(std::string sFades);
	
	/*! read the <loop> TinyXML DOM 
	 \param TiXmlElement [in]: TiXmlElement.
	 */
	void Read_Loop_Object(TiXmlElement* pElement);
	
	
	/*
	
	// list of sample files to load
	std::list<CSample_Data> mSample_Data_List;
	
	// list of takes files to load
	std::list<CTake_Data*> mDecompress_Que;
	
	// list of takes files to load
	std::list<CTake_Data*> mDownload_Que;
	
	// list of takes files to load
	std::list<CTake_Data*> mInsert_Que;
	 */
	
};

