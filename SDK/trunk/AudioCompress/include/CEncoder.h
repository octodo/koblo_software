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

#ifndef _ac_c_encoder_h
#define _ac_c_encoder_h
// Created by Lasse Steen Bohnstedt, 2008-04-xx

class CEncoder : public virtual IEncoder
{
	// Allow full access for CWaveDecoder.cpp - it needs some of the protected things because it's in reality both a decoder and an encoder.
	friend class CWaveDecoder;

public:
	CEncoder();
	virtual ~CEncoder();

	//! IDestructable override
	virtual void Destroy();
	
	//! IEncoder implementation
	virtual tbool Init(IFile* pfOutputStream, EQuality eQuality = geQualityDefault, tint iChannels = -1);
	//! IEncoder implementation
	virtual void SetQualityOfOriginal(EQuality eQualityOfOriginal);
	//! IEncoder implementation
	virtual tbool SetRawMode(tbool bInputRawMode, tint16 iChannels, tbool bIsInterleavedStereo, tint16 iBitWidth, tint32 iFreqHz);
	//! IEncoder implementation
	virtual void SetNormalizationFactor(tfloat32 fNormalizationFactor);
	//! IEncoder implementation
	virtual tbool Process(IFile* pfInputWaveStream1, IFile* pfInputWaveStream2, tint32 iSampleIxToProcess = 0, tint32 iNbSamplesToProcess = -1, tint64* piActualSamplesProcessed = NULL, tbool* pbDone = NULL, tint32 iMaxTimeMS = -1, tint64* piAccumOverflows = NULL);
	//! IEncoder implementation
	virtual tbool Process(IFile* pfInputWaveStream1, tint32 iSampleIxToProcess = 0, tint32 iNbSamplesToProcess = -1, tint64* piActualSamplesProcessed = NULL)
	{ return Process(pfInputWaveStream1, NULL, iSampleIxToProcess, iNbSamplesToProcess, piActualSamplesProcessed); };
	//! IEncoder implementation
	virtual tbool ProcessRaw(tchar* pcInput1, tchar* pcInput2, tint32 iInputBytes, tint64* piAccumOverflows, tint64* piSamples = NULL);
	//! IEncoder implementation
	virtual tbool Finalize();
	//! IEncoder implementation
	virtual tbool GetErrMsg(tchar* pszErrMsgBuff, tint iBuffSize, tbool bClearMsg = false);
	//! IEncoder implementation
	virtual tbool GetWarnMsg(tchar* pszWarnMsgBuff, tint iBuffSize, tbool bClearMsg = false);
	//! IEncoder implementation
	virtual void GetQualityRange(tint32* piFirstQ, tint32* piLastQ, tint32* piDefaultQ = NULL);

	// IEncoder dummy implementation (will be overridden by descendants)
	virtual tbool GetQualityApproximateBitRate(tint32 iQualityNb, tint32* piKBPS_Mono, tint32* piKBPS_Stereo);
	

protected:
	//! The amount of bytes to read from file(s) per encoder loop
	tint64 miBytesToRead;

	//! Callback for descendants to get output written
	tbool WriteOutput(tchar* pcOutputBuff, tint32 iOutputBytes);

	//tbool miIsInputRawPCM;
	tbool mbIgnoreOutputFileNull;
	tbool Process_InitFile(IFile* pfInputStream1, IFile* pfInputStream2, tint32 iSampleIxToProcess, tint32* piNbSamplesToProcess);
	tbool Process_InitFile_RawPCM(IFile* pfInputStream1, IFile* pfInputStream2, tint32 iSampleIxToProcess, tint32* piNbSamplesToProcess);
	tbool Process_InitFile_RiffWave(IFile* pfInputStream1, IFile* pfInputStream2, tint32 iSampleIxToProcess, tint32* piNbSamplesToProcess);

	//! Performs various house-keeping. Is called from either Process_InitFile or SetRawMode
	tbool Process_Init();
	//! Performs various house-keeping. Is called when mbProcessInProgress == true from either: (1) Finalize when the CEncoder object is closed or destroyed, or (2) from Process_Init before opening a new process
	tbool Process_Exit();
	//! True after Process_Init has been called, until Process_Exit is called
	tbool mbProcessInProgress;

	//! Must be implemented by descendants of CEncoder!<br/>Handles codec specific house-keeping. Is called from Init
	virtual tbool Init_Descendant(EQuality eQuality);

	//! Must be implemented by descendants of CEncoder!<br/>Handles codec specific house-keeping. May write to output. Is called from Process_Init
	/*!
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool Process_Init_Descendant();

	//! Must be implemented by descendants of CEncoder!<br/>Handles codec specific house-keeping. Is called from Process_Exit
	virtual tbool Process_Exit_Descendant();

	//! Must be implemented by descendants of CEncoder!<br/>Encode a portion of data (PCM or tfloat32 array) and either copy to a buffer or do in-place convertion. This buffer will then be written to previously opened output file (through WriteOutput() member)
	/*!
		\param pcInput1 [in]: Source data for encoding. Either left channel or interleaved stereo (in which case only PCM is supported)
		\param pcInput2 [in]: Source data for encoding. Right channel, may be NULL
		\param iInputBytes [in]: Number of bytes (note! not samples!) in input(s). If two buffers are submitted then the total amount of bytes is double
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool ProcessRaw_Descendant(tchar* pcInput1, tchar* pcInput2, tint32 iInputBytes, tint32 iSamples, tint64* piAccumOverflows);

	//! Must be implemented by descendants of CEncoder!<br/>Closes encoder engine after flushing it. This may result in some extra bytes written to output file
	/*!
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool Finalize_Descendant();

	//! IEncoder implementation
	virtual EAudioCodec GetAudioCodec() const
	{ return meAudioCodec; };

	EAudioCodec meAudioCodec;

	virtual const tchar* GetDefaultExt()
	{ return IEncoder::GetDefaultExt(meAudioCodec); };
	virtual const tchar* GetFileTypeDesc()
	{ return IEncoder::GetFileTypeDesc(meAudioCodec); };

	//tchar* mpszDefaultExt;
	//tchar* mpszFileTypeDesc;

	std::string msFullPath_InputFile1_Latest;
	std::string msFullPath_InputFile2_Latest;
	tuint64 muiFilePos_InputFile1_Latest;
	tuint64 muiFilePos_InputFile2_Latest;
	tint64 miSampleIx_InputFiles_Latest;

	//tint16 miInputChannels;
	//tint32 miInputSampleRate;
	tint32 miInputByteRate;
	tint16 miInputBlockAlign;
	//tint16 miInputBits;
	tint32 miInputMetaSize;
	tint32 miInputDataSize;
	//tint32 miInputSamples;
	tint32 miInputSecsTotal;
	tint32 miInputSeconds;
	tint32 miInputHours;
	tint32 miInputMinutes;
	tint64 miAccumOverflows;
	tfloat mfNormalizationFactor;
	tfloat mfNormalizationFactor_Inverse;
	void Process_ReportChannels();

	// Buffers
	tint32 miInputBuff1Size;
	tchar* mpcInputBuff1;
	tchar* PrepareInputBuff1(int iRequiredSize) { return PrepareWhateverBuff(&mpcInputBuff1, &miInputBuff1Size, iRequiredSize); };
	tint32 miInputBuff2Size;
	tchar* mpcInputBuff2;
	tchar* PrepareInputBuff2(int iRequiredSize) { return PrepareWhateverBuff(&mpcInputBuff2, &miInputBuff2Size, iRequiredSize); };

	tint32 miPreProcessBuff1Size;
	tchar* mpcPreProcessBuff1;
	tchar* PreparePreProcessBuff1(int iRequiredSize) { return PrepareWhateverBuff(&mpcPreProcessBuff1, &miPreProcessBuff1Size, iRequiredSize); };
	tint32 miPreProcessBuff2Size;
	tchar* mpcPreProcessBuff2;
	tchar* PreparePreProcessBuff2(int iRequiredSize) { return PrepareWhateverBuff(&mpcPreProcessBuff2, &miPreProcessBuff2Size, iRequiredSize); };

	tint32 miPostProcessBuff1Size;
	tchar* mpcPostProcessBuff1;
	tchar* PreparePostProcessBuff1(int iRequiredSize) { return PrepareWhateverBuff(&mpcPostProcessBuff1, &miPostProcessBuff1Size, iRequiredSize); };
	tint32 miPostProcessBuff2Size;
	tchar* mpcPostProcessBuff2;
	tchar* PreparePostProcessBuff2(int iRequiredSize) { return PrepareWhateverBuff(&mpcPostProcessBuff2, &miPostProcessBuff2Size, iRequiredSize); };

	tint32 miOutputBuffSize;
	tchar* mpcOutputBuff;
	tchar* PrepareOutputBuff(int iRequiredSize) { return PrepareWhateverBuff(&mpcOutputBuff, &miOutputBuffSize, iRequiredSize); };

	tuint64 ReadInput(IFile* pf, tuint64 uiBytes, tchar** ppcBuff, tint32* piAllocated);
	tuint64 ReadInput1(IFile* pf1, tuint64 uiBytes)
	{ return ReadInput(pf1, uiBytes, &mpcInputBuff1, &miInputBuff1Size); };
	tuint64 ReadInput2(IFile* pf2, tuint64 uiBytes)
	{ return ReadInput(pf2, uiBytes, &mpcInputBuff2, &miInputBuff2Size); };

	tbool mbInitDone;
	IFile* mpfOutputStream;
	
	std::string msErrMsg;
	std::string msWarnMsg;
	
	void AppendErrMsg(tchar* psz);
	void AppendWarnMsg(tchar* psz);

	void my_warningf_hook(const char *format, va_list ap);
	void my_errorf_hook(const char *format, va_list ap);

	//! How many bytes were output
	tuint64 muiBytesTotalOutput;
	
	//! Set name of encoder (real encoders must override)
	tchar* mpszEncoderName;

	//! Default: no qualities possible (real encoders must override)
	tint32 miLowestPossibleQualityNb;
	tint32 miHighestPossibleQualityNb;
	tint32 miDefaultQualityNb;
};

#endif //#infdef _ac_c_encoder_h
