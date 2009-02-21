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


enum EExportClipOrder {
	geExportClip_Before,
	geExportClip_Action,
	geExportClip_After,

	geExportClip_Done
}; // EExportClipOrder


class CExportClipTask : public CProgressTask {
public:
	std::string sScreenName;

	//CKSFile_Item mFileItem;
	tuint64 muiStartIx;
	tuint64 muiDuration;

	tint32 miFiles;
	IFile* mpLeft_Wave_File;
	IFile* mpRight_Wave_File;
	
	
	tbool bIsDecompressed;
	std::string sCompressedOriginal;

	ac::EAudioCodec eCodecOfOriginal;
	ac::EQuality eQualityOfOriginal;
	ac::EQuality eQualityOfCompressed;
	tint32 miChannels;
	tint32 miSampleRate;

	tchar* mpcSilence;

	std::string sDestName_Concatenate;
	std::string sDestFolder;
	std::string sDestNameAndExt;
	tbool bDoEncode;
	tbool bDoCopy;
	tbool bSuccess;
	CExportClipTask* pConcatenateNextTask;

	tint32 miActionOrder;
	std::string msExport_Path_and_File;
	IFile* mpfDestinatin_File;
	tint32 miChannelsDst;
	ac::EAudioCodec meCodecDst;
	ac::EQuality meQualityDst;
	ac::IEncoder* mpEncoder;
	tuint64 muiCurrEncodeIx;
	tuint64 muiSamplesNeeded;

	// Old constructor (should be deleted some time later)
	CExportClipTask(const tchar* pszClipName, tuint64 uiStartIx, tuint64 uiDuration);
	// New constructor - use with Init(..)
	CExportClipTask();

	virtual ~CExportClipTask();
	virtual void Destroy();

	tbool Init(const CTake_Data* pTake_Input, const tchar* pszFilePathDst, ac::EAudioCodec eCodecDst, ac::EQuality eQuality, tuint64 uiStartIx = 0, tuint64 uiDuration = (tuint64)-1);
	
	virtual tbool DoWork();
	virtual tbool IsDone();

	tbool ConcatenateNext()
	{ return (pConcatenateNextTask != NULL); };

protected:
	tbool DoEncode();
	tbool DoEncode_FirstTimeHere();
	tbool DoCopy();

	tbool IsSilence()
	{ return (miFiles == 0); };
	
	void constructor_helper(ac::EAudioCodec eCodecDst, tuint64 uiStartIx, tuint64 uiDuration);

}; // CExportClipTask
