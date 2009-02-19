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

/*! \class CKSXML_Read_Project
 * \brief class that parse the XML data in to the internal KS DataEngine
 based on TinyXml www.sourceforge.net/projects/tinyxml
 */

class CKSApplication;
class CSample_Data;
class CTake_Data;
class CRegion_Data;



class CKSXML_Read_Project:
public virtual CImport_TinyXML
{
	



public:
	
	CKSXML_Read_Project();
	~CKSXML_Read_Project();
	
	

	 /*! read DAW "Project File.xml" from disk
	 \param sFile [in]: File part and file name with extencion
	 \param iBackResID [in]: Background bitmap
	 */
	void Read_Project_From_Disk(std::string sFile);
	
	//! read a project XML file from Koblo.comin to the tiny xml DOM
	void Read_Latest_Revision_From_Koblo(tint32 iProjectID);
	
	//! read a project XML file from Koblo.comin to the tiny xml DOM
	void Read_Latest_Revision_From_Koblo(std::string sProject_UUID);
	
	//! read a project XML file from Koblo.comin to the tiny xml DOM
	void Read_Project_From_Koblo(std::string sProject);
	
	//! pass the DOM in to the KS data structure
	//!!! TO DO rename please look at content
//	virtual void CKSXML_Parse_DOM_To_DAW();

	/*! read the  TinyXML DOM file the root level
	 \ passes one <project> object
	 */
	void Pass_The_Project_Tag( TiXmlNode* pParent );
	
	//! Read the  parameters from TinyXML <project> DOM
	/*!
	 \param TiXmlNode [in]: TiXmlNode.
	 */
	void Read_Project_Object(TiXmlNode* pParent);
	
	/*! passes the different TinyXML objects from the root level 
	\ filter out comments and the like
	 \param TiXmlNode [in]: TiXmlNode.
	 */
	void Parse_Project_Object(TiXmlNode* pParent);
	
	//! Read the project name object from the TinyXml object
	/*!
	 \param TiXmlNode [in]: TiXmlNode.
	 */
//	virtual void Read_Project_Name(TiXmlNode* pParent);
	
	/*! read the <branch> TinyXML DOM 
	 \param TiXmlElement [in]: TiXmlElement.
	 */
	virtual void Read_Branch_Object(TiXmlElement* pElement);
	

	
	//! read list of sampel from TinyXML DOM
	void Read_Sample_Object(TiXmlElement* pElement);
	//! read list of sampel from TinyXML DOM
	void Read_Take_Object(TiXmlElement* pElement, CTake_Data* pTake_Data);
	
	

//	void Read_Insert(TiXmlElement* pElement);
//	void Read_Insert_Settings(TiXmlElement* pElement);
//	void Read_Insert_Parameter(TiXmlElement* pElement);
	
	
	
	void Parse_Track_Object(TiXmlElement* pElement);
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
	void Read_Track_Region_Fade(TiXmlElement* pElement, CRegion_Data* pRegion_Data);
	
	void Read_Bus(TiXmlElement* pElement);
	void Read_Bus_Out(TiXmlElement* pElement);
	void Read_Bus_Aux(TiXmlElement* pElement);
	void Read_Bus_Insert(TiXmlElement* pElement);
	
	void Read_Master(TiXmlElement* pElement);
	void Read_Master_Out(TiXmlElement* pElement);
	void Read_Master_Insert(TiXmlElement* pElement);
	void Read_Master_Aux_Return(TiXmlElement* pElement);
	
	/*! iterates the list of samples
	 \ each sample holds a list of takes with minimum one take
	 \ determin witch take has to be decompressed / downloaded
	 \ resoult are stored in the tree lists 
	 \ mInsert_Que, mDownload_Que and mDecompress_Que
	 */
	virtual void Prepare_Samples();
	
	/*! iterates the list of takes
	 \ takes are inspected and downloaded / decompressed if needed
	 */
	 virtual void Import_Takes(CSample_Data* pSample_Data);
	
	//! inspect one take
	void Import_Take( CTake_Data* pTake_Data);

	
	
	
	
	
	tbool Check_For_Newer_Revision(tint32 iProject_ID);
	//! itterate true the project XML file from the root level
	void Check_Project( TiXmlNode* pParent );
	
	void Check_Project_Childs(TiXmlNode* pParent);
	
	void Check_Branch(TiXmlNode* pParent);
	
	void Check_Revision(TiXmlElement* pElement);
	
	
	CTake_Data* Get_Take(std::string sTake_UUID);
	
	
	//! called from CDownloadTask::DoWork() when there is no more takes to download
	void Takes_Downloaded();
	
	//! decompress one take
	void Takes_Decompressed();
	
	

private:
	
	virtual void Prepare_For_XML();
	
	tbool mbNew_Revision;
	
	//! iterate the list of takes to download
	void Download_Takes();
	
	//! download one take
	void Download_Take(CTake_Data* Take_Data);
	
	//! iterate the list of takes to decompress
	void Decompress_Takes();
	
	//! decompress one take
	void Decompress_Take(CTake_Data* Take_Data);
	
	//! create pict files
	void Create_Pict_Files();
	
	//! create pict file
	void Create_Pict_File(CTake_Data* Take_Data);
	
	//! insert wave files from the mInsert_Que list
	void Insert_Takes();
	
	//! insert regions
	void Insert_Regions();
	
	//! 
//	void Import_Regions();
	
	
	//! setup track editor
	void Setup_Track_Editor();
	
	
	
	
	//! list of sample files to load
	std::list<CSample_Data> mSample_Data_List;
	
	//! list of takes files to decompress
	std::list<CTake_Data*> mDecompress_Que;
	
	//! list of takes files to download
	std::list<CTake_Data*> mDownload_Que;
	
	//! list of takes files to load
	std::list<CTake_Data*> mInsert_Que;
	
	//! list of takes files that needs pict files
	std::list<CTake_Data*> mCreate_Pict_File_Que;
	
	
	//! list of regions to insert when all wav files and pict files are readdy
	std::list<CRegion_Data> mRegion_Data_List;
	
	std::string Get_Take_Screen_Name(std::string sUUID);
	
	tuint32 muiTrack;
	
	tbool bInsert_Regions;
	
	tbool mbOpen_Dialog;
	
	
};

