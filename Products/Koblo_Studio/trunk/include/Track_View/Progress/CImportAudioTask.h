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


//! tasks shown in the progress bar at the top o the endit window

enum EAudioImportOrder {
	geAudioImport_Start,

	geAudioImport_Decode_Before,
	geAudioImport_Decode_Action,
	geAudioImport_Decode_After,

	geAudioImport_Copy_Before,
	geAudioImport_Copy_Action,
	geAudioImport_Copy_After,

	geAudioImport_Peak_Before,
	geAudioImport_Peak_Action,
	geAudioImport_Peak_After,

	geAudioImport_Region_Before,
	geAudioImport_Region_Action,
	geAudioImport_Region_After,

	geAudioImport_Done
}; // EAudioImportOrder


class CImportAudioTask : public CProgressTask {
public:
	
	CImportAudioTask();
	
	virtual ~CImportAudioTask();


	enum EStereoBehavior { geStereoDoAsk, geStereoDoSplit, geStereoDoKeep };


	ac::EAudioCodec meCodec;
	ac::EQuality meSrcQuality;
	tbool mbSrcLossyCompressed;
//	tbool mbStereo;
//	tbool mbSplit;
	tint32 miBitWidth;

	IFile* mpSrc_File;
	IDecoder* mpDecoder;
	tint32 miDecodeIx;
	IFile* mpAudio_File_L;
	IFile* mpAudio_File_R;

	tbool mbDstIsAlreadyThere;
	tbool mbCopyLossyOriginal;

//	tint32 miRegion_TrackID;
//	tint64 miRegion_TrackPos;
	tbool mbDeleteNonLossyOriginal;

	tint32 miAudioImportOrder;

	

	virtual void Destroy();

	tbool Init( CKSFile_Item* pFile_Item
			/*	tbool bDoesWaveAlreadyExist
				EStereoBehavior eStereoBehavior 
				tbool bForceOriginalIsLossy */);
	
//	void Init_InsertAsRegionAfterImport(tint32 iTrackID, tint64 iTrackPos);

	tbool IsOpened() { return (mpAudio_File_L != NULL); };
	tbool Open();

	virtual tbool DoWork();
	virtual tbool IsDone();

	tbool Stereo(){ return mFile_Item.Stereo();};
	void Stereo( tbool bStereo) { mFile_Item.Stereo(bStereo);};
	
	//! take name
	std::string Disk_Name(){ return mFile_Item.Disk_Name();};
	
	//! left take name
	std::string Left_Name(){ return mFile_Item.Left_Name();};
	//! right take name
	std::string Right_Name(){ return mFile_Item.Stereo() ? mFile_Item.Right_Name(): "";};
	
	
	//! left take name
	std::string Left_Path(){ return mFile_Item.Left_Path();};
	//! right take name
	std::string Right_Path(){ return mFile_Item.Stereo() ? mFile_Item.Right_Path(): "";};
	
	//! left peek file path
	std::string Left_Peak_File_Path(){ return mFile_Item.Left_Peak_File_Path();};
	//! right peek file path
	std::string Right_Peak_File_Path(){ return mFile_Item.Stereo() ? mFile_Item.Right_Peak_File_Path(): "";};
	
	//! get uuid
	std::string Get_UUID(){ return mFile_Item.Get_UUID(); };
	
	//! get screen name
	std::string Screen_Name(){ return mFile_Item.Screen_Name(); };
	
	//! set screen name
	void Screen_Name(std::string sScreen_Name){ mFile_Item.Screen_Name(sScreen_Name); };
	
	
	
protected:
	
//	std::string msImported_File_NameL;
//	std::string msImported_File_NameR;
	
//	std::string msSource_File;
	
	CKSFile_Item mFile_Item;
	
//	tint32 miRegion_TrackID;
//	tint64 miRegion_TrackPos;
	
	
//	virtual std::string Get_File_Name_From_Path(std::string sFile_Path);
};