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


enum EExportClipOrder {
	geExportClip_Before,
	geExportClip_Action,
	geExportClip_After,

	geExportClip_Done
}; // EExportClipOrder


class CExportClipTask : public CProgressTask {
public:


	std::string sClipName;
	tuint64 muiStartIx;
	tuint64 muiDuration;

	tint32 iFiles;
	IFile* pfWaveL;
	IFile* pfWaveR;
	
	std::string sWaveR;
	
	tbool bIsDecompressed;
	std::string sCompressedOriginal;

	ac::EAudioCodec eCodecOfOriginal;
	ac::EQuality eQualityOfOriginal;
	ac::EQuality eQualityOfCompressed;
	tint32 iChannels;
	tint32 iSampleRate;

	tchar* mpcSilence;

	std::string sDestName_Concatenate;
	std::string sDestFolder;
	std::string sDestNameAndExt;
	tbool bDoEncode;
	tbool bDoCopy;
	tbool bSuccess;
	CExportClipTask* pConcatenateNextTask;

	tint32 miActionOrder;
	std::string sOut;
	IFile* mpfDst;
	tint32 miChannelsDst;
	ac::EAudioCodec meCodecDst;
	ac::EQuality meQualityDst;
	ac::IEncoder* mpEncoder;
	tuint64 muiCurrEncodeIx;
	tuint64 muiSamplesNeeded;

	CExportClipTask( const tchar* pszClipName, tuint64 uiStartIx, tuint64 uiDuration)
	{
		

		sClipName = pszClipName;
		muiStartIx = uiStartIx;
		muiDuration = uiDuration;

		mpcSilence = NULL;

		iFiles = 0;
		pfWaveL = IFile::Create();
		pfWaveR = IFile::Create();
		bIsDecompressed = false;

		eCodecOfOriginal = ac::geAudioCodecUndefined;
		eQualityOfOriginal = eQualityOfCompressed = ac::geQualityUnknown;
		iChannels = iSampleRate = 0;

		bDoEncode = bDoCopy = bSuccess = false;
		pConcatenateNextTask = NULL;

		miActionOrder = geExportClip_Before;
		mpfDst = NULL;
		miChannelsDst = -1;
		mpEncoder = NULL;
		meCodecDst = ac::geAudioCodecUndefined;
		meQualityDst = ac::geQualityUnknown;
		muiCurrEncodeIx = uiStartIx;
		muiSamplesNeeded = uiDuration;
	} // constructor

	virtual ~CExportClipTask();
	virtual void Destroy();

	virtual tbool DoWork();
	virtual tbool IsDone();

	tbool ConcatenateNext()
	{ return (pConcatenateNextTask != NULL); };

protected:
	tbool DoEncode();
	tbool DoEncode_FirstTimeHere();
	tbool DoCopy();

	tbool IsSilence()
	{ return (iFiles == 0); };

}; // CExportClipTask
