#include "acInternalOS.h"


CDecoder::CDecoder()
{
	ResetAllParams();
	meAudioCodec = geAudioCodecUndefined;
} // constructor


void CDecoder::ResetAllParams()
{
	mpfOutWaveStream1 = NULL;
	mpfOutWaveStream2 = NULL;
	mbSplit = false;
	mbAllowNoOutputFile = false;
	
	mbRIFFWaveHeader = true;
	mbIsLossyCompression = true;
	miOutputBitWidth = 0;
	miOutputChannels = 0;
	//meLastInputQuality = geQualityUnknown;
	meLowestInputQuality = geQualityUnknown;
	miOutputSampleFreq = -1;
	miSamplesTotal = 0;

	msInputFilePathName = "";
	mpcInputBuff = NULL;
	muiInputFilePos = 0;
	//
	mpcIntermediateBuffL = NULL;
	mpcIntermediateBuffR = NULL;
	miSizeIntermediateBuffL = 0;
	miSizeIntermediateBuffR = 0;
	//
	mpcOutputBuffL = NULL;
	miSizeOutputBuffL = 0;
	mpcOutputBuffR = NULL;
	miSizeOutputBuffR = 0;
	//
	miSampleIxOutputBuffs = 0;
	miSamplesOutputBuffs = 0;

#ifndef AC_IDECODER_NOT_BACK_COMPAT
	miSampleFreq = -1;
	miChannels = 0;
	miBitWidth = 0;
#endif // IDECODER_BACK_COMPAT
} // ResetAllParams


CDecoder::~CDecoder()
{
	Maybe_Finalize();

	// Kill allocated buffers (if any)
	PrepareInputBuff(0);
	PrepareIntermediateBuffL(0);
	PrepareIntermediateBuffR(0);
	PrepareOutputBuffL(0);
	PrepareOutputBuffR(0);
} // destructor


void CDecoder::Maybe_Finalize()
{
	if (mpfOutWaveStream1) {
		// Finish previous stream(s)
		Finalize();
	}
} // Maybe_Finalize


IDecoder* IDecoder::Create(IFile* pSrcFile, tchar* pszErrMsgBuff /*= NULL*/, tint32 iErrMsgBuffSize /*= -1*/)
{
	IDecoder* pDecoder = NULL;

	std::string sPathNameAndExt = "";
	std::string sExt = "";
	std::string sErr = "";

	if (pSrcFile == NULL) {
		sErr = "Internal Error: pSrcFile == NULL";
	}
	else {
		if (1) {
			tchar pszPathNameAndExt[1024];
			*pszPathNameAndExt = '\0';
			pSrcFile->GetPathName(pszPathNameAndExt);
			if (pszPathNameAndExt) {
				sPathNameAndExt = pszPathNameAndExt;

				tchar* pszName = pszPathNameAndExt;
				tchar* pszLastColon = strrchr(pszPathNameAndExt, ':');
				if (pszLastColon) {
					pszName = pszLastColon + 1;
				}
				tchar* pszExt = strrchr(pszName, '.');
				if (pszExt) {
					sExt = pszExt;
				}
			}
		}

		tuint64 iFilePos_Save = pSrcFile->GetCurrentFilePosition();
		try {
			EAudioCodec eCodec = geAudioCodecUndefined;

			tchar pszFileType[5];
			memset(pszFileType, '\0', 5);
			tint64 iActuallyRead = pSrcFile->Read(pszFileType, 4);
			pSrcFile->Seek(iFilePos_Save);

			if (iActuallyRead == 4) {
				if (strncmp(pszFileType, "RIFF", 4) == 0) {
					eCodec = geAudioCodecWave;
				}
				else if (strncmp(pszFileType, "Ogg", 3) == 0) {
					eCodec = geAudioCodecVorbis;
				}
				else if (strncmp(pszFileType, "ID3", 3) == 0) {
					eCodec = geAudioCodecLame;
				}
			}

			if (eCodec == geAudioCodecUndefined) {
				if (sExt.length()) {
					const tchar* pszExt = sExt.c_str();
					if (stricmp(pszExt, ".wav") == 0)
						eCodec = geAudioCodecWave;
					else if (stricmp(pszExt, ".ogg") == 0)
						eCodec = geAudioCodecVorbis;
					else if (stricmp(pszExt, ".mp3") == 0)
						eCodec = geAudioCodecLame;
				}
			}

			if (eCodec == geAudioCodecUndefined) {
				sErr = "Unable to determine file type";
			}
			else {
				if (pszErrMsgBuff)
					*pszErrMsgBuff = '\0';
				pDecoder = Create(eCodec, pszErrMsgBuff, iErrMsgBuffSize);
				if (pDecoder == NULL) {
					if (pszErrMsgBuff)
						sErr = pszErrMsgBuff;
					else
						sErr = "Unknown error";
				}
				else {
					// BUG-FIX: We mustn't do a TestFile here!
					// If the result is a CLameDecoder (mp3) and another is already running
					// this would fail, since LAME isn't thread safe.
					// Do nothing here
					/*
					if (!pDecoder->TestFile(pSrcFile)) {
						tchar pszErrBuff[1024];
						if (pDecoder->GetErrMsg(pszErrBuff, 1024))
							sErr = pszErrBuff;
						pDecoder->Destroy();
						pDecoder = NULL;
					}
					*/
				}
			}
		}
		catch (...) {
			sErr = "Internal error: Caught exception";
		}

		try {
			// Attempt to move file pointer to previous position
			pSrcFile->Seek(iFilePos_Save);
		}
		catch (...) {}
	}

	if ((pszErrMsgBuff) && (sErr.length())) {
		if (iErrMsgBuffSize < 0) {
			tchar* pszAfter = pszErrMsgBuff + sprintf(pszErrMsgBuff, sErr.c_str());
			if (sPathNameAndExt.length()) {
				sprintf(pszAfter, "\nFile: %s", sPathNameAndExt.c_str());
			}
		}
		else {
			strncpy(pszErrMsgBuff, sErr.c_str(), iErrMsgBuffSize - 1);
			tchar* pszAfter = pszErrMsgBuff + sErr.length();
			if (sPathNameAndExt.length()) {
				tint32 iSpaceLeft = iErrMsgBuffSize - sErr.length() - 1;
				if (iSpaceLeft > 0) {
					tchar* pszFileText = "\nFile: ";
					strncpy(pszAfter, pszFileText, iSpaceLeft);
					tint32 iLenFileText = strlen(pszFileText);
					iSpaceLeft -= iLenFileText;
					tchar* pszAfter2 = pszAfter + iLenFileText;
					if (iSpaceLeft > 0) {
						strncpy(pszAfter2, sPathNameAndExt.c_str(), iSpaceLeft);
					}
				}
			}
			pszErrMsgBuff[iErrMsgBuffSize - 1] = '\0';
		}
	}
	return pDecoder;
} // Create(IFile*, ...)


IDecoder* IDecoder::Create(EAudioCodec eAudioCodec, tchar* pszErrMsgBuff /*= NULL*/, tint32 iErrMsgBuffSize /*= -1*/)
{
	std::string sErr = "";

	IDecoder* pDecoder = NULL;
	switch (eAudioCodec) {
		case geAudioCodecWave:
			pDecoder = CWaveDecoder::Create();
			break;

		case geAudioCodecVorbis:
			pDecoder = CVorbisDecoder::Create();
			break;

		case geAudioCodecLame:
			pDecoder = CLameDecoder::Create();
			break;

		default:
			sErr = "IDecoder::Create: Unknown codec";
			break;
	}

	if ((pDecoder == NULL) && (sErr.empty())) {
		sErr = "IDecoder::Create: Unknown error (out of memory?)";
	}

	if ((sErr.length()) && (pszErrMsgBuff != NULL)) {
		if (iErrMsgBuffSize < 0)
			sprintf(pszErrMsgBuff, sErr.c_str());
		else {
			strncpy(pszErrMsgBuff, sErr.c_str(), (iErrMsgBuffSize - 1));
			pszErrMsgBuff[iErrMsgBuffSize - 1] = '\0';
		}
	}

	return pDecoder;
} // Create(EAudioCodec, ...)


void CDecoder::Destroy()
{
	delete dynamic_cast<CDecoder*>(this);
} // Destroy


tbool CDecoder::Init(IFile* pfOutputWaveStream1, tbool bRIFFWaveHeader /*= true*/, tint32 iBitWidth /*= 24*/, tint32 iChannels /*= -1*/, IFile* pfOutputWaveStream2)
{
	Maybe_Finalize();	

	msErrMsg = "";
	msWarnMsg = "";
	miSamplesTotal = 0;
	
	tbool bError = false;
	
	if ((!mbAllowNoOutputFile) && (pfOutputWaveStream1 == NULL)) {
		AppendErrMsg("Input Wave stream is NULL");
		bError = true;
	}
	
	if ((iChannels != 1) && (iChannels != 2) && (iChannels != -1)) {
		tchar psz[128];
		sprintf(psz, "Can only decode to mono or stereo streams, not %d channels", iChannels);
		AppendErrMsg(psz);
		bError = true;
	}

	if ((iBitWidth != 16) && (iBitWidth != 24)) {
		tchar psz[128];
		sprintf(psz, "Can only decode to 16 or 24 bit streams, not %d bits", iBitWidth);
		AppendErrMsg(psz);
		bError = true;
	}

	if (pfOutputWaveStream2) {
		mbSplit = true;
		//if ((iBitWidth != 24) || (iChannels != 1)) {
		if (iChannels != 1) {
			//AppendErrMsg("Can only split into two mono 24 bit wave files");
			AppendErrMsg("Can only split into two mono wave files");
			bError = true;
		}
	}

	if (!bError) {
		mpfOutWaveStream1 = pfOutputWaveStream1;
		mpfOutWaveStream2 = pfOutputWaveStream2;

		mbRIFFWaveHeader = bRIFFWaveHeader;
		miOutputChannels = iChannels;
		miOutputBitWidth = iBitWidth;
		miOutputBlockAlign = -1;
#ifndef AC_IDECODER_NOT_BACK_COMPAT
		miChannels = iChannels;
		miBitWidth = iBitWidth;
#endif // AC_IDECODER_NOT_BACK_COMPAT
		
		meLowestInputQuality = (miOutputBitWidth == 16) ? geQualityLossless16 : geQualityLossless24;
	}
	
	return !bError;
} // Init


tbool CDecoder::HardcoreSwapOutputFiles(IFile* pfOutputWaveStream1, IFile* pfOutputWaveStream2 /*= NULL*/)
{
	mpfOutWaveStream1 = pfOutputWaveStream1;
	mpfOutWaveStream2 = pfOutputWaveStream2;
	return true;
} // HardcoreSwapOutputFiles


tbool CDecoder::Process(IFile* /*pfInputOggStream*/, tint32 /*iSampleIxToProcess /*= 0*/, tint32 /*iNbOfSamplesToProcess /*= -1*/, tint32* /*piAccumSamplesReturned /*= NULL*/)
{
	AppendErrMsg("CDecoder is a dummy class that cannot actually process anything");
	return false;
} // Process


tbool CDecoder::Process_Init(IFile* pfInputCompressedStream, tint32 iSizeBuffInput, tint32 iSizeBuffIntermediateL, tint32 iSizeBuffIntermediateR, tbool* pbSameFile)
{
	if (pfInputCompressedStream == NULL) {
		AppendErrMsg("Input stream is NULL.");
		return false;
	}

	PrepareInputBuff(iSizeBuffInput);
	PrepareIntermediateBuffL(iSizeBuffIntermediateL);
	PrepareIntermediateBuffR(iSizeBuffIntermediateR);

	tchar pszNewFilePathName[1024];
	pfInputCompressedStream->GetPathName(pszNewFilePathName);
	tbool bSameFile = (strcmp(msInputFilePathName.c_str(), pszNewFilePathName) == 0);

	if (!bSameFile) {
		msInputFilePathName = pszNewFilePathName;
		muiInputFilePos = pfInputCompressedStream->GetCurrentFilePosition();
		miSampleIxOutputBuffs = 0;
		miSamplesOutputBuffs = 0;
		miLastInputSamples = 0;
		miLastInputBitWidth = 0; // only valid for uncompressed input
		muiInputFileSampleIx = 0;
		miPctDisplayed = -1;
	}
	*pbSameFile = bSameFile;

	return true;
} // Process_Init


tbool CDecoder::Process_MaybeWriteBufferedOutput(tbool* pbExit, tint32 iNbOfSamplesToProcess, tint32* piAccumSamplesReturned)
{
	tbool bError = false;

	tint32 iSamplesBuffered = miSamplesOutputBuffs - miSampleIxOutputBuffs;
	if (iSamplesBuffered > 0) {
		tint32 iSamplesToWriteNow = iSamplesBuffered;
		tint32 iAccumSamplesNew = *piAccumSamplesReturned + iSamplesBuffered;

		// Should we limit the output number?
		if (iNbOfSamplesToProcess < 0) {
			// No limit - write them all
			// Do nothing here
		}
		else {
			// Enforce limit
			if (iAccumSamplesNew > iNbOfSamplesToProcess) {
				// We must limit number of samples
				iSamplesToWriteNow = iNbOfSamplesToProcess - *piAccumSamplesReturned;
				iAccumSamplesNew = iNbOfSamplesToProcess;
			}
		}

		tint32 iBytesToWriteNow = iSamplesToWriteNow * miOutputBlockAlign;

		// Write the converted output
		//mpfOutWaveStream->Write((const char*)pcBufferToWriteNow, iBytesToWriteNow);
		if (!mbAllowNoOutputFile) {
			tint32 iBytesIx = miSampleIxOutputBuffs * miOutputBlockAlign;
			if (!mbSplit) {
				// Write to one (stereo or mono) wave file
				const tchar* pc = mpcBufferToWriteNowL + iBytesIx;
				mpfOutWaveStream1->Write(pc, iBytesToWriteNow);
			}
			else {
				// Write to two (mono) wave files
				const tchar* pcL = mpcBufferToWriteNowL + iBytesIx;
				const tchar* pcR = mpcBufferToWriteNowR + iBytesIx;
				mpfOutWaveStream1->Write(pcL, iBytesToWriteNow);
				mpfOutWaveStream2->Write(pcR, iBytesToWriteNow);
			}
		}
		
		miSampleIxOutputBuffs += iSamplesToWriteNow;
		miLastInputSamples += iSamplesToWriteNow;
		miSamplesTotal += iSamplesToWriteNow;
		*piAccumSamplesReturned = iAccumSamplesNew;
		
		// Lasse, allow for break if we have all we need
		if ((iNbOfSamplesToProcess > -1) && (iAccumSamplesNew >= iNbOfSamplesToProcess)) {
			*pbExit = true;
		}
	}

	return !bError;
} // Process_MaybeWriteBufferedOutput


tbool CDecoder::TestFile(IFile* pfInputCompressedStream)
{
	tbool bSuccess = false;

	tuint64 uiSavePos = pfInputCompressedStream->GetCurrentFilePosition();

	mbAllowNoOutputFile = true;
	if (Init(NULL, false, 24)) {
		if (Process(pfInputCompressedStream, 0, 1))
			bSuccess = true;
		Finalize();
	}
	mbAllowNoOutputFile = false;

	pfInputCompressedStream->Seek(uiSavePos);

	return bSuccess;
} // TestFile


const tint32 kiKobloChunkSize = 128;
const tchar* kpszKobloEncQual = "KOBLO_ENC_QUAL=";

void CDecoder::OutDummyPCMHeader(IFile* pfOut, tint32 iBitWidth, tint32 iChannels, tint32 iSampleFreq)
{
	tint32 i32;
	tint16 i16;

	// Meta chunk
	pfOut->Write("RIFF",4);
	// Placeholder for total size
	i32 = 0;						pfOut->Write((char*)&i32, 4);
	// Type specifier
	pfOut->Write("WAVE", 4);
	
#ifdef _Mac_PowerPC
	// Hm... nothing. Will do with CPSwap:
#endif
	// Format chunk
	pfOut->Write("fmt ", 4);
	// size of Format chunk
	i32 = CPSwap((tint32)16);											pfOut->Write((char*)&i32, 4);
	// PCM format
	i16 = CPSwap((tint16)1);											pfOut->Write((char*)&i16, 2);
	// channels
	i16 = CPSwap((tint16)iChannels);									pfOut->Write((char*)&i16, 2);
	// samplerate
	i32 = CPSwap((tint32)iSampleFreq);									pfOut->Write((char*)&i32, 4);
	// byterate
	i32 = CPSwap((tint32)(iSampleFreq * iChannels * (iBitWidth / 8)));	pfOut->Write((char*)&i32, 4);
	// blockalign
	i16 = CPSwap((tint16)(iChannels * (iBitWidth / 8)));				pfOut->Write((char*)&i16, 2);
	// bits
	i16 = CPSwap((tint16)iBitWidth);									pfOut->Write((char*)&i16, 2);

	// Koblo specific chunk
	pfOut->Write("KBLO", 4);
	i32 = CPSwap(kiKobloChunkSize);
	pfOut->Write((char*)&i32, 4);
	tchar pszEncQual[kiKobloChunkSize];
	memset(pszEncQual, '\0', kiKobloChunkSize);
	sprintf(pszEncQual, kpszKobloEncQual);
	pfOut->Write(pszEncQual, kiKobloChunkSize);
	
	// PCM Data chunk
	pfOut->Write("data", 4);
	// Placeholder for pcm data size
	i32 = 0;															pfOut->Write((char*)&i32, 4);
} // OutDummyPCMHeader


tbool CDecoder::Finalize()
{
	if ((!mbAllowNoOutputFile) && (mpfOutWaveStream1 == NULL)) {
		// Close attempted before open
		AppendErrMsg("Finalize() before Init()");
		return false;
	}

	tbool bSuccess = Finalize_CodecSpecific();

	if (mbRIFFWaveHeader) {
		// Update RIFF/Wave header(s) - Fill in the correct sizes
		
		tint32 iPCMLen = miSamplesTotal * miOutputChannels * (miOutputBitWidth / 8);
		std::cout << "PCM Len : " << iPCMLen;
		std::cout << " = " << miSamplesTotal << " * " << miOutputChannels << " * (" << miOutputBitWidth << " / 8);\n";
		
		OutCorrectPCMHeader(mpfOutWaveStream1, iPCMLen, meLowestInputQuality);
		if (mpfOutWaveStream2) {
			OutCorrectPCMHeader(mpfOutWaveStream2, iPCMLen, meLowestInputQuality);
		}
	}

	std::cout << "\nDone.\n\n";

	mpfOutWaveStream1 = NULL;
	mpfOutWaveStream2 = NULL;

	msInputFilePathName = "";
	muiInputFilePos = 0;
	PrepareInputBuff(0);
	PrepareIntermediateBuffL(0);
	PrepareIntermediateBuffR(0);
	PrepareOutputBuffL(0);
	PrepareOutputBuffR(0);
	
	return bSuccess;
} // Finalize


void CDecoder::OutCorrectPCMHeader(IFile* pfOut, tint32 iPCMLen, EQuality eLowestInputQuality)
{
	tuint64 uiFilePosSave = pfOut->GetCurrentFilePosition();

	// Total size (excluding meta chunk id and size itself)
	pfOut->Seek(4);
	tint32 iMetaChunkSize = iPCMLen + 36 + 8 + kiKobloChunkSize;
	tint32 i32 = CPSwap((tint32)iMetaChunkSize);	pfOut->Write((char*)&i32, 4);

	// Koblo chunk
	pfOut->Seek(36 + 8);
	tchar pszKobloChunk[kiKobloChunkSize];
	sprintf(pszKobloChunk, "%s%s", kpszKobloEncQual, IEncoder::GetQualityName(eLowestInputQuality));
	pfOut->Write(pszKobloChunk, strlen(pszKobloChunk));
	
	// PCM data size (excluding chunk id and size itself)
	pfOut->Seek(36 + 8 + kiKobloChunkSize + 4);
	i32 = CPSwap((tint32)iPCMLen);	pfOut->Write((char*)&i32, 4);

	pfOut->Seek(uiFilePosSave);
} // OutCorrectPCMHeader


tbool CDecoder::GetErrMsg(tchar* pszErrMsgBuff, tint iBuffSize, tbool bClearMsg /*= false*/)
{
	tint iLen = msErrMsg.size();
	if (iLen >= iBuffSize)
		return false;
	strncpy(pszErrMsgBuff, msErrMsg.c_str(), iLen);
	pszErrMsgBuff[iLen] = '\0';
	if (bClearMsg)
		msErrMsg = "";
	return true;
} // GetErrMsg

tbool CDecoder::GetWarnMsg(tchar* pszWarnMsgBuff, tint iBuffSize, tbool bClearMsg /*= false*/)
{
	tint iLen = msWarnMsg.size();
	if (iLen >= iBuffSize)
		return false;
	strncpy(pszWarnMsgBuff, msWarnMsg.c_str(), iLen);
	pszWarnMsgBuff[iLen] = '\0';
	if (bClearMsg)
		msWarnMsg = "";
	return true;
} // GetWarnMsg

void CDecoder::AppendErrMsg(tchar* psz)
{
	std::cerr << "Error: " << psz << "\n";
	if (msErrMsg.size() > 0)
		msErrMsg += "\n";
	msErrMsg += psz;
} // AppendErrMsg

void CDecoder::AppendWarnMsg(tchar* psz)
{
	std::cerr << "Warning: " << psz << "\n";
	if (msWarnMsg.size() > 0)
		msWarnMsg += "\n";
	msWarnMsg += psz;
} // AppendWarnMsg
