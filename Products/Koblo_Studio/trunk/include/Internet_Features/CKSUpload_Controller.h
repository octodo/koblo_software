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








/*! \class CKSUpload_Controller
	\brief class that creates prepares audio files and uploat a project to koblo.com 
	
*/




class CMIDI_Sequencer;
class CSample_Data;
class CTake_Data;



class CKSUpload_Controller
{
	
public:
	//! Constructor
	CKSUpload_Controller();

	//! Destructor
	virtual ~CKSUpload_Controller();
	
	//! upload project
	virtual void Upload_Project();
	
	//! upload new project
	virtual void Upload_New_Project();
	
	//! upload excisting project
	virtual void Upload_Existing_Project();
	
	//! check one sample from the mSample_Data_List
	virtual void Prepare_Sampel_For_Upload(CSample_Data* pSample_Data);
	
	/*! find  oog and mp3 takes
	 \if there is no ogg / mp3 stor the name of the take in th mOGG_Compress_Que / mOMp3_Compress_Que
	 \add the take to the upload_que
	 */
	virtual void Prepare_Take_For_Upload(CTake_Data* Take_Data);
	
	//! secure all mp3 files are in the ogg file folder
	virtual void Compress_OGG_File();
	
	//! secure all mp3 files are in the ogg file folder
	virtual void Compress_MP3_File();
	
	// validate that a take acutually is readable auido file/ files
	tbool Validate_Take(CTake_Data* pTake_Data);
	
	// called when the a new project is created on koblo.com
	void Online_Project_Created();
	
	// called if the project upload to koblo.com failde
	void Online_Project_Upload_Failed();
	
	
	
	

	
private:	
	
	//! check if a take alreaddy has an url. if it does it alreaddy lives on Koblo.com
	tbool Take_Is_Uploaded(CTake_Data* pTake_Data);
	
	//! add the take to the upload que
	void Add_Take_To_Upload_Ques(CTake_Data* pTake_Data);
	

	 
	// list of takes files to compress as OGG
	std::list<CTake_Data*> mOGG_Compress_Que;
	
	// list of takes files to compress as mp3
	std::list<CTake_Data*> mMp3_Compress_Que;
	
	// list of mp3 takes ready to upload to Koblo
	std::list<CTake_Data*> mUpload_Que;


};






