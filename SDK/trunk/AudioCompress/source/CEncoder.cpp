
//! brief: class CEncoder
// Created by Lasse Steen Bohnstedt, 2008-04-xx

#include "acInternalOS.h"


CEncoder::CEncoder()
{
	miBytesToRead = 0;
	mfNormalizationFactor = mfNormalizationFactor_Inverse = 1.0f;

	miOutputChannels = 0;
	miOutputSampleFreq = -1;
	miOutputSamplesTotal = 0;
	
	mpfOutputStream = NULL;
	mbInitDone = false;

	mpcInputBuff1 = NULL;
	mpcInputBuff2 = NULL;
	mpcPreProcessBuff1 = NULL;
	mpcPreProcessBuff2 = NULL;
	mpcPostProcessBuff1 = NULL;
	mpcPostProcessBuff2 = NULL;
	mpcOutputBuff = NULL;

	muiBytesTotalOutput = 0;

	// Default: no qualities possible (real encoders must override)
	miLowestPossibleQualityNb = 0;
	miHighestPossibleQualityNb = -1;
	miDefaultQualityNb = -1;

	// Set name of encoder (real encoders must override)
	mpszEncoderName = "Undefined Encoder (no real functionality)";
	meAudioCodec = geAudioCodecUndefined;
	meQuality = geQualityUnknown;
	meQualityInfoToSave = geQualityUnknown;

	mbProcessInProgress = false;
	mbIgnoreOutputFileNull = false;
	mbIsInputRawData = false;

	// Set file extension and description (real encoders must override)
	//mpszDefaultExt = ".*";
	//mpszFileTypeDesc = "All Files (*.*)";
}

CEncoder::~CEncoder()
{
	if (mbInitDone) {
		Finalize();
	}
}


IEncoder* IEncoder::Create()
{
	return Create(geAudioCodecVorbis);
} // Create(void)


IEncoder* IEncoder::Create(EAudioCodec eAudioCodec, tchar* pszErrBuff /*= NULL*/, tint32 iErrBuffSize /*= 0*/)
{
	IEncoder* pEncoder = NULL;
	std::string sErr = "";

	switch (eAudioCodec) {
		case geAudioCodecWave:
			pEncoder = CWaveEncoder::Create();
			break;

		case geAudioCodecVorbis:
			pEncoder = CVorbisEncoder::Create();
			break;

		case geAudioCodecLame:
			pEncoder = CLameEncoder::Create();
			break;

		default:
			{
				tchar pszErr[512];
				sprintf(pszErr, "Can't create IEncoder for format %d", eAudioCodec);
				sErr = pszErr;
			}
	}

	if ((sErr.length()) && (pszErrBuff) && (iErrBuffSize > 0)) {
		strncpy(pszErrBuff, sErr.c_str(), iErrBuffSize - 1);
		pszErrBuff[iErrBuffSize - 1] = '\0';
	}
	return pEncoder;
} // Create(EAudioCodec, tchar*, tint32)


void CEncoder::Destroy()
{
	delete dynamic_cast<CEncoder*>(this);
}


tbool CEncoder::Init(IFile* pfOutputStream, EQuality eQuality /*= geQualityDefault*/, tint iChannels /*= -1*/)
{
	if (mbInitDone) {
		// Finish previous stream
		Finalize();
	}

	msErrMsg = "";
	msWarnMsg = "";
	miOutputSamplesTotal = 0;
	muiBytesTotalOutput = 0;

	mbIsInputRawData = false;
	
	tbool bError = false;
	
	if ((!mbIgnoreOutputFileNull) && (pfOutputStream == NULL)) {
		AppendErrMsg("Pointer to ouput compressed stream/file is NULL");
		bError = true;
	}
	mpfOutputStream = pfOutputStream;

	if (eQuality == geQualityDefault)
		eQuality = (EQuality)miDefaultQualityNb;

	if (eQuality >= geNbOfQualities) {
		tchar psz[128];
		sprintf(psz, "Invalid quality number %d", eQuality);
		AppendErrMsg(psz);
		bError = true;
	}
	else if ((eQuality < miLowestPossibleQualityNb) || (eQuality > miHighestPossibleQualityNb)) {
		tchar psz[128];
		sprintf(psz, "'%s' quality is impossible for %s", GetQualityName(eQuality), mpszEncoderName);
		AppendErrMsg(psz);
		bError = true;
	}
	meQuality = eQuality;
	meQualityInfoToSave = eQuality;

	if ((iChannels != 1) && (iChannels != 2) && (iChannels != -1)) {
		tchar psz[128];
		sprintf(psz, "Can only encode mono or stereo streams, not %d channels", iChannels);
		AppendErrMsg(psz);
		bError = true;
	}
	miOutputChannels = iChannels;

	if ((!bError) && (!Init_Descendant(eQuality)))
		bError = true;
	
	mbInitDone = !bError;
	return mbInitDone;
} // Init


void CEncoder::SetQualityOfOriginal(EQuality eQualityOfOriginal)
{
	if (eQualityOfOriginal < meQualityInfoToSave)
		meQualityInfoToSave = eQualityOfOriginal;
} // SetQualityOfOriginal


void CEncoder::SetNormalizationFactor(tfloat32 fNormalizationFactor)
{
	mfNormalizationFactor = fNormalizationFactor;
	mfNormalizationFactor_Inverse = 1.0f / fNormalizationFactor;
} // SetNormalizationFactor


tbool CEncoder::Init_Descendant(EQuality /*eQuality*/)
{
	AppendErrMsg("Internal error: CEncoder::Init_Descendant should be overloaded");
	return false;
} // Init_Descendant


tbool CEncoder::WriteOutput(tchar* pcOutputBuff, tint32 iOutputBytes)
{
	if (mbIgnoreOutputFileNull) {
		// Pretend to have written bytes
		muiBytesTotalOutput += iOutputBytes;
	}
	else {
		if (mpfOutputStream == NULL) {
			AppendErrMsg("Internal error, WriteOutput: no output file");
			return false;
		}
		tuint64 uiWritten = mpfOutputStream->Write(pcOutputBuff, iOutputBytes);
		muiBytesTotalOutput += uiWritten;
		if (uiWritten != (tuint64)iOutputBytes) {
			AppendErrMsg("WriteOutput: Write operation failed");
			return false;
		}
	}

	return true;
} // WriteOutput


tbool CEncoder::SetRawMode(tbool bInputRawData, tint16 iChannels, tbool bIsInterleavedStereo, tint16 iBitWidth, tint32 iFreqHz)
{
	if (bInputRawData) {
		if ((iChannels != 1) && (iChannels != 2)) {
			tchar pszErr[512];
			sprintf(pszErr, "SetRawMode expects iChannels of 1 or 2, but got %d", iChannels);
			AppendErrMsg(pszErr);
			return false;
		}

		if ((iBitWidth != 16) && (iBitWidth != 24) && (iBitWidth != 32)) {
			tchar pszErr[512];
			sprintf(pszErr, "SetRawMode expects iBitWidth of 16, 24 or 32, but got %d", iBitWidth);
			AppendErrMsg(pszErr);
			return false;
		}

		if ((iFreqHz != 44100) && (iFreqHz != 48000) && (iFreqHz != 88200) && (iFreqHz != 96000)) {
			tchar pszErr[512];
			sprintf(pszErr, "SetRawMode expects iFreqHz of 44100, 48000, 88200 or 96000, but got %d", iFreqHz);
			AppendErrMsg(pszErr);
			return false;
		}

		if (bIsInterleavedStereo) {
			if (iChannels != 2) {
				tchar pszErr[512];
				sprintf(pszErr, "SetRawMode expects iChannels = 2 for interleaved stereo, but got %d", iChannels);
				AppendErrMsg(pszErr);
				return false;
			}
			if (iBitWidth > 24) {
				AppendErrMsg("SetRawMode: Interleaved stereo isn't possible for 32 bit floats");
				return false;
			}
		}

		// Verify new frequency compared to old
		if ((miOutputSampleFreq > -1) && (miOutputSampleFreq != iFreqHz)) {
			tchar psz[128];
			sprintf(psz, "Sample freq of new input (%d Hz) differs from first input freq (%d Hz)", iFreqHz, miOutputSampleFreq);
			AppendErrMsg(psz);
			return false;
		}
		else {
			miOutputSampleFreq = iFreqHz;
		}

		// Params input
		miInputChannels = iChannels;
		miInputSampleFreq = iFreqHz;
		miInputBitWidth = iBitWidth;
		mbIsInputInterleavedStereo = bIsInterleavedStereo;

		// Calculated
		miInputByteRate = miInputBitWidth / 8;
		miInputBlockAlign = mbIsInputInterleavedStereo ? miInputByteRate * miInputChannels : miInputByteRate;

		// Can't initialize (yet)
		miInputMetaSize = -1;
		miInputDataSize = -1;
		miInputSamples = -1;
		miInputSamples_IncludingSkipped = -1;

		// Ignored
		miInputSecsTotal = -1;
		miInputSeconds = -1;
		miInputHours = -1;
		miInputMinutes = -1;

		// Maybe set output channels
		if (miOutputChannels < 0)
			miOutputChannels = miInputChannels;

		if (!Process_Init())
			return false;
	}

	mbIsInputRawData = bInputRawData;
	return true;
}


tuint64 CEncoder::ReadInput(IFile* pf, tuint64 uiBytes, tchar** ppcBuff, tint32* piAllocated)
{
	tint32 iBytes = (tint32)(((tuint32)1 << 31) - 1);
	if (uiBytes > iBytes)
		uiBytes = iBytes;
	else
		iBytes = (tint32)uiBytes;

	tchar* pcInput = PrepareWhateverBuff(ppcBuff, piAllocated, iBytes);
	return pf->Read(pcInput, uiBytes);
} // ReadInput



tbool CEncoder::Process_InitFile(IFile* pfInputStream1, IFile* pfInputStream2, tint32 iSampleIxToProcess, tint32* piNbSamplesToOutput)
{
	if (pfInputStream1 == NULL) {
		AppendErrMsg("Input file is NULL.");
		return false;
	}

	// Determine if we should be continuing with a partially processed file
	tchar pszPathName1[1024];
	tchar pszPathName2[1024];
	pfInputStream1->GetPathName(pszPathName1);
	if (pfInputStream2 == NULL)
		*pszPathName2 = '\0';
	else
		pfInputStream2->GetPathName(pszPathName2);
	if (strcmp(pszPathName1, msFullPath_InputFile1_Latest.c_str()) == 0) {
		// File 1 is the same
		if (
			// No file 2 - just like before
			((pfInputStream2 == NULL) && (msFullPath_InputFile2_Latest.empty()))
			||
			// .. or same file 2 as before
			(strcmp(pszPathName2, msFullPath_InputFile2_Latest.c_str()) == 0)
		) {

			// Continue with same file(s)

			// Maybe jump some samples
			tint64 iSampleDiff = 0;
			if (iSampleIxToProcess < 0) {
				// We just continue from where we last ended
				// Do nothing here...
			}
			else {
				// We have been told where to continue from
				iSampleDiff = iSampleIxToProcess - miSampleIx_InputFiles_Latest;
			}
			tint64 iByteDiff = iSampleDiff * miInputBlockAlign;
			tuint64 uiFilePos_Now = pfInputStream1->GetCurrentFilePosition();
			tuint64 uiFilePos_Dst = muiFilePos_InputFile1_Latest + iByteDiff;
			if (uiFilePos_Now != uiFilePos_Dst)
				pfInputStream1->Seek(uiFilePos_Dst);
			if (pfInputStream2) {
				uiFilePos_Now = pfInputStream2->GetCurrentFilePosition();
				uiFilePos_Dst = muiFilePos_InputFile2_Latest + iByteDiff;
				if (uiFilePos_Now != uiFilePos_Dst)
					pfInputStream2->Seek(uiFilePos_Dst);
			}

			return true;
		}
	}

	// It's a new set of files - forget any old
	msFullPath_InputFile1_Latest = "";
	msFullPath_InputFile2_Latest = "";
	muiFilePos_InputFile1_Latest = 0;
	muiFilePos_InputFile2_Latest = 0;
	miSampleIx_InputFiles_Latest = 0;

	tbool bSuccess = false;
	if (mbIsInputRawData) 
		bSuccess = Process_InitFile_RawPCM(pfInputStream1, pfInputStream2, iSampleIxToProcess, piNbSamplesToOutput);
	else
		bSuccess = Process_InitFile_RiffWave(pfInputStream1, pfInputStream2, iSampleIxToProcess, piNbSamplesToOutput);

	if (bSuccess) {
		if (!Process_Init())
			bSuccess = false;
	}

	if (bSuccess) {
		// Remember these filenames
		msFullPath_InputFile1_Latest = pszPathName1;
		msFullPath_InputFile2_Latest = pszPathName2;

		// Prepare input buffer(s) for read of 1024 sample portions
		tint32 iRequiredSize = 1024 * miInputBlockAlign;
		PrepareInputBuff1(iRequiredSize);
		miBytesToRead = iRequiredSize;
		if (pfInputStream2) {
			PrepareInputBuff2(iRequiredSize);
		}
		else {
			// De-allocate buffer 2
			PrepareInputBuff2(0);
		}

		muiFilePos_InputFile1_Latest = pfInputStream1->GetCurrentFilePosition();
		if (pfInputStream2)
			muiFilePos_InputFile2_Latest = pfInputStream2->GetCurrentFilePosition();

		// Maybe skip some samples
		if (iSampleIxToProcess > 0) {
			muiFilePos_InputFile1_Latest += iSampleIxToProcess * miInputBlockAlign;
			pfInputStream1->Seek(muiFilePos_InputFile1_Latest);
			if (pfInputStream2) {
				muiFilePos_InputFile2_Latest += iSampleIxToProcess * miInputBlockAlign;
				pfInputStream2->Seek(muiFilePos_InputFile2_Latest);
			}
		}
		miSampleIx_InputFiles_Latest = iSampleIxToProcess;
	}

	return bSuccess;
} // Process_InitFile

tbool CEncoder::Process_InitFile_RawPCM(IFile* pfInputStream1, IFile* pfInputStream2, tint32 iSampleIxToProcess, tint32* piNbSamplesToOutput)
{
	// Calculate the remainder of file as data size
	tuint64 ui64Remainder = 0;
	tuint64 ui64Pos1 = pfInputStream1->GetCurrentFilePosition();
	tuint64 ui64Size1 = pfInputStream1->GetSizeWhenOpened();
	tuint64 ui64Remainder1 = ui64Size1 - ui64Pos1;
	if (ui64Remainder1 >= ((tuint64)1 << 31)) {
		AppendErrMsg("Raw PCM 1 is simply to large");
		return false;
	}
	if (pfInputStream2 == NULL)
		ui64Remainder = ui64Remainder1;
	else {
		tuint64 ui64Pos2 = pfInputStream1->GetCurrentFilePosition();
		tuint64 ui64Size2 = pfInputStream1->GetSizeWhenOpened();
		tuint64 ui64Remainder2 = ui64Size2 - ui64Pos2;
		if (ui64Remainder2 >= ((tuint64)1 << 31)) {
			AppendErrMsg("Raw PCM 2 is simply to large");
			return false;
		}

		ui64Remainder = min(ui64Remainder1, ui64Remainder2);
	}
	tint32 iRemainder = (tint32)ui64Remainder;

	// Verify block-alignment
	if (iRemainder % miInputBlockAlign) {
		AppendErrMsg("Raw PCM is incorrectly block alligned");
		return false;
	}

	// Calculate samples
	miInputSamples_IncludingSkipped = miInputSamples = iRemainder / miInputBlockAlign;
	if (iSampleIxToProcess > 0)
		miInputSamples -= iSampleIxToProcess;
	if (miInputSamples < 0)
		miInputSamples = 0;

	if (piNbSamplesToOutput) {
		if (*piNbSamplesToOutput < 0)
			*piNbSamplesToOutput = miInputSamples;
		else if (miInputSamples > *piNbSamplesToOutput)
			miInputSamples = *piNbSamplesToOutput;
	}

	return true;
} // Process_InitFile_RawPCM

tbool CEncoder::Process_InitFile_RiffWave(IFile* pfInputStream1, IFile* pfInputStream2, tint32 iSampleIxToProcess, tint32* piNbSamplesToOutput)
{
	// We must find, read and verify the RIFF/WAVE chunk-headers
	tbool bError = false;

	// Prepare an input buffer for file read of header
//	PrepareInputBuff1(1024);
	
	// Read META header and first sub-chunk ID
	tuint64 uiFileReadIx = pfInputStream1->GetCurrentFilePosition();
	tuint64 uiActuallyRead = ReadInput1(pfInputStream1, 20);
	tint32 iNextSubChunkIDIx = 12;
	if (uiActuallyRead != 20) {
		AppendErrMsg("RIFF/WAVE file META header read error (file < 20 bytes)");
		bError = true;		
	}
	else {
		if (strncmp((char*)mpcInputBuff1, "RIFF", 4) != 0) {
			AppendErrMsg("Expected 'RIFF' at index 0");
			bError = true;
		}
		miInputMetaSize = CPSwap(*((tint32*)(mpcInputBuff1+4)));
		if (strncmp((char*)mpcInputBuff1+8, "WAVE", 4) != 0) {
			AppendErrMsg("Expected 'WAVE' at index 8");
			bError = true;
		}
		if (!bError) {
			if (miInputMetaSize + 8 > pfInputStream1->GetSizeWhenOpened()) {
				AppendErrMsg("RIFF meta chunk size doesn't match WAVE file size - end of file missing?");
				bError = true;
			}
			else if (miInputMetaSize + 8 < pfInputStream1->GetSizeWhenOpened()) {
				AppendErrMsg("RIFF meta chunk size doesn't match WAVE file size - file corrupt?");
				bError = true;
			}
		}
	}
	
	// Search for 'fmt ', 'KBLO' and 'data' chunks
	tbool bSubChunkFmt = false;
	tbool bSubChunkData = false;
	
	do {
		tchar* pszSubChunkID = (char*)mpcInputBuff1 + iNextSubChunkIDIx;
		tint32 iSubChunkSize = CPSwap(*((tint32*)(pszSubChunkID + 4)));

		if (strncmp(pszSubChunkID, "data", 4) == 0) {
			std::cerr << "Found 'data' sub-chunk at file index " << (uiFileReadIx + iNextSubChunkIDIx) << ".\n";
			bSubChunkData = true;
			if (!bSubChunkFmt) {
				AppendErrMsg("Found 'data' sub-chunk before 'fmt '.");
				bError = true;
			}
			else {
				miInputDataSize = iSubChunkSize;
				tuint64 uiRestOfFile = miInputMetaSize - (uiFileReadIx + iNextSubChunkIDIx);
				if (miInputDataSize > uiRestOfFile) {
					AppendErrMsg("Sub-chunk 'data' is too big - corrupted file");
					bError = true;
				}				
				else if (miInputDataSize < uiRestOfFile) {
					tuint64 uiSurplus = uiRestOfFile - miInputDataSize;
					tchar psz[128];
					sprintf(psz, "There are %u bytes surplus in WAVe file - non-standard chunks?", (tuint32)uiSurplus);
					AppendWarnMsg(psz);
				}
			}
		}
		else if (strncmp(pszSubChunkID, "KBLO", 4) == 0) {
			std::cerr << "Found 'KBLO' sub-chunk at file index " << (uiFileReadIx + iNextSubChunkIDIx) << ".\n";
			tuint64 uiWantToRead = iSubChunkSize + 8;
			iNextSubChunkIDIx = iSubChunkSize;
			uiActuallyRead = ReadInput1(pfInputStream1, uiWantToRead);
			if (uiActuallyRead != uiWantToRead) {
				AppendErrMsg("Sub-chunk 'KBLO' read error (end of file)");
				bError = true;
			}
			else {
				const tchar* kpszEncQual = "KOBLO_ENC_QUAL=";
				if (strnicmp(mpcInputBuff1, kpszEncQual, sizeof(kpszEncQual)) == 0) {
					const tchar* pszEncQual = mpcInputBuff1 + strlen(kpszEncQual);
					tint32 iQuality = IEncoder::GetQualityNb(pszEncQual);
					if (iQuality >= 0) {
						EQuality eQuality = (EQuality)iQuality;
						if (eQuality < meQualityInfoToSave) {
							meQualityInfoToSave = eQuality;
						}
					}
				}
			}
		}
		else if (strncmp(pszSubChunkID, "fmt ", 4) == 0) {
			std::cerr << "Found 'fmt ' sub-chunk at file index " << (uiFileReadIx + iNextSubChunkIDIx) << ".\n";
			if (bSubChunkFmt) {
				AppendErrMsg("More than one 'fmt ' header");
				bError = true;
			}
			else {
				bSubChunkFmt = true;
				if (iSubChunkSize < 16) {
					tchar psz[128];
					sprintf(psz, "Expected 'fmt ' header size to be 16; found size: %d", iSubChunkSize);
					AppendErrMsg(psz);
					bError = true;
				}
				else {
					if (iSubChunkSize > 16) {
						tchar psz[128];
						sprintf(psz, "Expected 'fmt ' header size to be 16; found size: %d", iSubChunkSize);
						AppendWarnMsg(psz);
					}

					// Read rest of sub-chunk (and ID and size of next chunk)
					uiFileReadIx = pfInputStream1->GetCurrentFilePosition();
					tuint64 uiWantToRead = iSubChunkSize + 8;
					iNextSubChunkIDIx = iSubChunkSize;
					uiActuallyRead = ReadInput1(pfInputStream1, uiWantToRead);
					if (uiActuallyRead != uiWantToRead) {
						AppendErrMsg("Sub-chunk 'fmt ' read error (end of file)");
						bError = true;
					}
					else {
						tint32 iAudioFormat = CPSwap(*((tint16*)mpcInputBuff1));
						if (iAudioFormat != 1) {
							tchar psz[128];
							sprintf(psz, "Expected audio format 1 = PCM, found %d", iAudioFormat);
							AppendErrMsg(psz);
							bError = true;
						}
						miInputChannels = CPSwap(*((tint16*)(mpcInputBuff1 + 2)));
						miInputSampleFreq = CPSwap(*((tint32*)(mpcInputBuff1 + 4)));
						miInputByteRate = CPSwap(*((tint32*)(mpcInputBuff1 + 8)));
						miInputBlockAlign = CPSwap(*((tint16*)(mpcInputBuff1 + 12)));
						miInputBitWidth = CPSwap(*((tint16*)(mpcInputBuff1 + 14)));
						if (miInputByteRate != miInputSampleFreq * miInputChannels * miInputBitWidth / 8) {
							AppendWarnMsg("Fixing byte-rate mismatch in WAVE file header.");
							miInputByteRate = miInputSampleFreq * miInputChannels * miInputBitWidth / 8;
							//bError = true;
						}
						if (miInputBlockAlign != miInputChannels * miInputBitWidth / 8) {
							AppendWarnMsg("Fixing block-align mismatch in WAVE file header.");
							miInputBlockAlign = miInputChannels * miInputBitWidth / 8;
							//bError = true;
						}
						if ((miInputChannels != 1) && (miInputChannels != 2)) {
							tchar psz[128];
							sprintf(psz, "Can only read mono or stereo WAVes, not %d channels", miInputChannels);
							AppendErrMsg(psz);
							bError = true;
						}
						
						if ((miInputBitWidth != 16) && (miInputBitWidth != 24)) {
							tchar psz[128];
							sprintf(psz, "Only support for 16 and 24 bits (WAVE is %d bits)", miInputBitWidth);
							AppendErrMsg(psz);
							bError = true;
						}
						
					}
				}
			}
		}
		else {
			tchar psz[128];
			sprintf(psz, "Skipping non-standard chunk at index %u", (tuint32)uiFileReadIx + iNextSubChunkIDIx);

			AppendWarnMsg(psz);
			uiFileReadIx = pfInputStream1->GetCurrentFilePosition();
			tuint64 uiWantToRead = iSubChunkSize + 8;
			tuint64 uiPosAfterSkip = uiFileReadIx + uiWantToRead;
			if ((uiPosAfterSkip < uiFileReadIx) || (uiPosAfterSkip >= pfInputStream1->GetSizeWhenOpened())) {
				sprintf(psz, "Can't skip broken non-standard chunk at index %u", (tuint32)uiFileReadIx + iNextSubChunkIDIx);
				AppendErrMsg(psz);
				bError = true;
			}
			else {
				iNextSubChunkIDIx = iSubChunkSize;
				uiActuallyRead = ReadInput1(pfInputStream1, uiWantToRead);
				if (uiActuallyRead != uiWantToRead) {
					AppendErrMsg("Non-standard sub-chunk read error (end of file)");
					bError = true;
				}
			}
		}
	}
	while ((!bError) && ((!bSubChunkFmt) || (!bSubChunkData)));

	if (bError) {
		AppendErrMsg("We can't encode this WAVe file");
		return false;
	}

	mbIsInputInterleavedStereo = (miInputChannels == 2);
	
	
	miInputSamples = miInputDataSize / miInputBlockAlign;
	miInputSecsTotal = (miInputSamples + (miInputSampleFreq >> 1)) / miInputSampleFreq;
	miInputSeconds = miInputSecsTotal;
	miInputHours = miInputSeconds / (60*60);
	miInputSeconds -= miInputHours * 60*60;
	miInputMinutes = miInputSeconds / 60;
	miInputSeconds -= miInputMinutes * 60;
	
	if ((miOutputSampleFreq > -1) && (miOutputSampleFreq != miInputSampleFreq)) {
		tchar psz[128];
		sprintf(psz, "Sample freq of new WAVE (%d Hz) differs from first WAVE freq (%d Hz)", miInputSampleFreq, miOutputSampleFreq);
		AppendErrMsg(psz);
		bError = true;
	}
	else {
		miOutputSampleFreq = miInputSampleFreq;
	}
	
	// Verify any second wave file has same format as first
	tint32 iInputSamples_File2 = -1;
	if (pfInputStream2 != NULL) {
		tbool bDualMonoOk = false;

		if (miInputChannels > 1) {
			AppendErrMsg("Process_InitFile: File 1 is stereo but there's a file 2 as well.\n"
				"Can only handle either (a) One stereo file, or (b) two mono files");
		}

		IEncoder* pIEncoder2_Temp = IEncoder::Create(geAudioCodecWave);
		if (pIEncoder2_Temp) {
			CEncoder* pCEncoder2_Temp = dynamic_cast<CEncoder*>(pIEncoder2_Temp);
			if (pCEncoder2_Temp) {
				pCEncoder2_Temp->mbIgnoreOutputFileNull = true;
				pCEncoder2_Temp->Init(NULL);
				if (pCEncoder2_Temp->Process_InitFile(pfInputStream2, NULL, 0, &iInputSamples_File2)) {
					if (pCEncoder2_Temp->miInputBitWidth != miInputBitWidth) {
						AppendErrMsg("Process_InitFile: The two input files are different bit-width");
					}
					else if (pCEncoder2_Temp->miInputChannels > 1) {
						AppendErrMsg("Process_InitFile: File 2 is stereo.\n"
							"Can only handle either (a) One stereo file, or (b) two mono files");
					}
					else if (pCEncoder2_Temp->miInputSampleFreq != miInputSampleFreq) {
						tchar pszErr[512];
						sprintf(pszErr,
							"Process_InitFile: Sample rates differ between dual mono inputs.\n"
							"File 1 is %d Hz, file 2 is %d Hz",
							miInputSampleFreq, pCEncoder2_Temp->miInputSampleFreq
							);
						AppendErrMsg(pszErr);
					}
					else {
						// File 2 is ok - do nothing here
						bDualMonoOk = true;

						miInputChannels = 2;
						mbIsInputInterleavedStereo = false;
						miOutputChannels = 2;
					}
				}
			}
			pIEncoder2_Temp->Destroy();
		}

		if (!bDualMonoOk) {
			if (msErrMsg.empty())
				msErrMsg = "Process_InitFile: Unknown error handling file 2";
			bError = true;
		}
	}

	if (bError) {
		return false;
	}
	
	if (miOutputChannels == -1) {
		miOutputChannels = miInputChannels;
	}
	
	if (iInputSamples_File2 >= 0) {
		miInputSamples = min(miInputSamples, iInputSamples_File2);
	}
	miInputSamples_IncludingSkipped = miInputSamples;
	if (iSampleIxToProcess > 0)
		miInputSamples -= iSampleIxToProcess;
	if (miInputSamples < 0)
		miInputSamples = 0;

	if (*piNbSamplesToOutput == -1) {
		*piNbSamplesToOutput = miInputSamples;
	}
	else if (miInputSamples > *piNbSamplesToOutput)
		miInputSamples = *piNbSamplesToOutput;

	std::cout << "chnls:" << miInputChannels << ", freq:" << miInputSampleFreq << "Hz, width:" << miInputBitWidth << " bits\n";
	std::cout << "bytes/sample:" << miInputBlockAlign << ", bytes/frame:" << miInputBlockAlign*miInputSampleFreq << "\n";
	char pszTime[32];
	if (miInputHours > 0)
		sprintf(pszTime, "%02d:%02d:%02d", miInputHours, miInputMinutes, miInputSeconds);
	else
		sprintf(pszTime, "%02d:%02d", miInputMinutes, miInputSeconds);
	std::cout << "length: " << miInputSamples << " samples, time: " << miInputSecsTotal << " seconds == " << pszTime << "\n";

	return true;
} // Process_InitFile_RiffWave


tbool CEncoder::Process_Init_Descendant()
{
	AppendErrMsg("Internal error: CEncoder::Process_Init_Descendant should be overloaded");
	return false;
} // Process_Init_Descendant


void CEncoder::Process_ReportChannels()
{
	std::cout << "Current input is " << ((miInputChannels == 1) ? "mono" : "stereo");
	std::cout << ", output is " << ((miOutputChannels == 1) ? "mono" : "stereo");
	std::cout << ((miInputChannels == miOutputChannels) ? "\n" : ((miInputChannels < miOutputChannels) ? " (up-mix)\n" : " (down-mix)\n"));
}


tbool CEncoder::Process(
	IFile* pfInputStream1, IFile* pfInputStream2 /*= NULL*/,
	tint32 iSampleIxToProcess /*= 0*/, tint32 iNbSamplesToProcess /*= -1*/,
	tint64* piActualSamplesProcessed /*= NULL*/, tbool* pbDone /*= NULL*/, tint32 iMaxTimeMS /*= -1*/,
	tint64* piAccumOverflows /*= NULL*/)
{
	tuint32 uiTime0 = ITime::GetTimeMS();

	if (!Process_InitFile(pfInputStream1, pfInputStream2, iSampleIxToProcess, &iNbSamplesToProcess)) {
		return false;
	}

	Process_ReportChannels();

	tuint32 uiTime1 = ITime::GetTimeMS();

	if (piActualSamplesProcessed)
		(*piActualSamplesProcessed) = 0;

	tbool bError = false;
	tint32 iBytesToRead = (tint32)miBytesToRead;
	tint32 iNbBytesToProcess = iNbSamplesToProcess * miInputBlockAlign;
	tbool bExit = (iNbSamplesToProcess <= 0);
	if (pbDone)
		*pbDone = bExit;
	while (!bExit) {
		// Is this the last portion?
		tbool bLastPortion = false;
		if (iNbBytesToProcess <= iBytesToRead) {
			iBytesToRead = iNbBytesToProcess;
			bLastPortion = true;
		}

		if (iBytesToRead == 0)
			bExit = true;
		else {
			tint64 iBytes = ReadInput1(pfInputStream1, iBytesToRead);
			if (pfInputStream2) {
				tint64 iBytes2 = ReadInput2(pfInputStream2, iBytesToRead);
				if (iBytes2 < iBytes)
					iBytes = iBytes2;
			}

			if (iBytes == 0) {
				// No more
				bExit = true;
			}
			else {
				tint64 iSamples = 0;
				if (!ProcessRaw(mpcInputBuff1, mpcInputBuff2, (tint32)iBytes, piAccumOverflows, &iSamples)) {
					bError = true;
				}
				else {
					// Update progress
					if (piActualSamplesProcessed)
						(*piActualSamplesProcessed) += iSamples;
					iNbBytesToProcess -= (tint32)iBytes;

					// Make sure we can continue from here
					miSampleIx_InputFiles_Latest += iSamples;
					muiFilePos_InputFile1_Latest = pfInputStream1->GetCurrentFilePosition();
					if (pfInputStream2)
						muiFilePos_InputFile2_Latest = pfInputStream2->GetCurrentFilePosition();

					// Have we used up our amounted time?
					if (iMaxTimeMS > -1) {
						tuint32 uiMaxTimeMS = (tuint32)iMaxTimeMS;
						tuint32 uiTime2 = ITime::GetTimeMS();
						tuint32 uiDiffTotal = uiTime2 - uiTime0;
						if (uiDiffTotal >= uiMaxTimeMS) {
							// We've used up time
							bExit = true;
						}
						else {
							tuint32 uiDiffLast = uiTime2 - uiTime1;
							tuint32 uiDiffTotalNext_Approx = uiDiffTotal + (uiDiffLast / 2);
							if (uiDiffTotalNext_Approx > uiMaxTimeMS) {
								// We would use to much time running next loop - exit now
								bExit = true;
							}
						}
					}
				}
			}
		}

		// was this last portion?
		if (bLastPortion) {
			bExit = true;
			if (pbDone)
				*pbDone = true;
		}

		if (bError)
			bExit = true;
	}

	return !bError;
} // Process


tbool CEncoder::ProcessRaw(tchar* pcInput1, tchar* pcInput2, tint32 iInputBytes, tint64* piAccumOverflows, tint64* piSamples /*= NULL*/)
{
	if (!mbInitDone) {
		AppendErrMsg("Internal error, ProcessRaw(..) before Init(..)");
		return false;
	}

	tint32 iSamples = iInputBytes / miInputBlockAlign;
	miOutputSamplesTotal += iSamples;
	if (piSamples)
		*piSamples = iSamples;

	// Call upon descendant for actual translation of audio
	if (!ProcessRaw_Descendant(pcInput1, pcInput2, iInputBytes, iSamples, piAccumOverflows))
		return false;

	return true;
} // ProcessRaw


tbool CEncoder::ProcessRaw_Descendant(tchar* /*pcInput1*/, tchar* /*pcInput2*/, tint32 /*iInputBytes*/, tint32 /*iSamples*/, tint64* /*piAccumOverflows*/)
{
	AppendErrMsg("Internal error: CEncoder::ProcessRaw_Descendant should be overloaded");
	return false;
} // ProcessRaw_Descendant


tbool CEncoder::Process_Exit_Descendant()
{
	AppendErrMsg("Internal error: CEncoder::Process_Exit_Descendant should be overloaded");
	return false;
} // Process_Exit_Descendant


tbool CEncoder::Process_Exit()
{
	if (!mbProcessInProgress) {
		AppendErrMsg("Internal error, Process_Exit: Called before Process_Init");
		return false;
	}

	tbool bSuccess = Process_Exit_Descendant();

	msFullPath_InputFile1_Latest = "";
	msFullPath_InputFile2_Latest = "";
	mbProcessInProgress = false;

	return bSuccess;
}


tbool CEncoder::Process_Init()
{
	if (mbProcessInProgress)
		Process_Exit();

	mbProcessInProgress = true;

	return Process_Init_Descendant();
}


tbool CEncoder::Finalize()
{
	if (!mbInitDone) {
		AppendErrMsg("Finalize() before Init().");
		return false;
	}

	tbool bError = false;
	if (mbProcessInProgress)
		bError = !Process_Exit();

	if (!Finalize_Descendant()) {
		bError = true;
	}

	mbInitDone = false;
	mpfOutputStream = NULL;
	mbIgnoreOutputFileNull = false;

	PrepareInputBuff1(-1);
	PrepareInputBuff2(-1);
	PreparePreProcessBuff1(-1);
	PreparePreProcessBuff2(-1);
	PreparePostProcessBuff1(-1);
	PreparePostProcessBuff2(-1);
	PrepareOutputBuff(-1);

	return !bError;
} // Finalize


tbool CEncoder::Finalize_Descendant()
{
	AppendErrMsg("Internal error: CEncoder::Finalize_Descendant should be overloaded");
	return false;
} // Finalize_Descendant
	

char gaszQualityNames[geNbOfQualities][32] = {
	"Unknown", "Poor", "Low", "Medium", "Good", "Better", "High", "Super", "Extreme", "Insane", "Insaner", "16 bit", "24 bit"
};


void CEncoder::GetQualityRange(tint32* piFirstQ, tint32* piLastQ, tint32* piDefaultQ)
{
	*piFirstQ = miLowestPossibleQualityNb;
	*piLastQ = miHighestPossibleQualityNb;
	if (piDefaultQ)
		*piDefaultQ = miDefaultQualityNb;
}

tbool CEncoder::GetQualityApproximateBitRate(tint32 /*iQualityNb*/, tint32* /*piKBPS_Mono*/, tint32* /*piKBPS_Stereo*/)
{
	return false;
}

tbool IEncoder::GetQualityName(tint32 iQualityNb, tchar* pszName, tint32 iNameBuffSize)
{
	if (iQualityNb > geNbOfQualities)
		return false;
	
	strncpy(pszName, gaszQualityNames[iQualityNb], iNameBuffSize - 1);
	pszName[iNameBuffSize - 1] = '\0';
	return true;
}

const tchar* IEncoder::GetQualityName(tint32 iQualityNb)
{
	if (iQualityNb > geNbOfQualities)
		return "";
	
	return gaszQualityNames[iQualityNb];
}


tint32 IEncoder::GetQualityNb(const tchar* pszName)
{
	for (tint32 i = 0; i < geNbOfQualities; i++) {
		if (stricmp(gaszQualityNames[i], pszName) == 0)
			return i;
	}
	
	return -1;
}


const tchar* IEncoder::GetDefaultExt(EAudioCodec eCodec)
{
	switch (eCodec) {
		case geAudioCodecWave:		return ".wav";
		case geAudioCodecVorbis:	return ".ogg";
		case geAudioCodecLame:		return ".mp3";

		default:
			return ".*";
	}
} // GetDefaultExt


const tchar* IEncoder::GetFileTypeDesc(EAudioCodec eCodec)
{
	switch (eCodec) {
		case geAudioCodecWave:		return "RIFF/Wave Files (*.wav)";
		case geAudioCodecVorbis:	return "Ogg/Vorbis Files (*.ogg)";
		case geAudioCodecLame:		return "MP3 Files (*.mp3)";

		default:
			return "All Files (*.*)";
	}
} // GetFileTypeDesc


tbool CEncoder::GetErrMsg(tchar* pszErrMsgBuff, tint iBuffSize, tbool bClearMsg /*= false*/)
{
	tint iLen = msErrMsg.size();
	if (iLen >= iBuffSize)
		return false;
	strncpy(pszErrMsgBuff, msErrMsg.c_str(), iLen);
	pszErrMsgBuff[iLen] = '\0';
	if (bClearMsg)
		msErrMsg = "";
	return true;
}
	

tbool CEncoder::GetWarnMsg(tchar* pszWarnMsgBuff, tint iBuffSize, tbool bClearMsg /*= false*/)
{
	tint iLen = msWarnMsg.size();
	if (iLen >= iBuffSize)
		return false;
	strncpy(pszWarnMsgBuff, msWarnMsg.c_str(), iLen);
	pszWarnMsgBuff[iLen] = '\0';
	if (bClearMsg)
		msWarnMsg = "";
	return true;
}

void CEncoder::AppendErrMsg(tchar* psz)
{
	std::cerr << "Error: " << psz << "\n";
	if (msErrMsg.size() > 0)
		msErrMsg += "\n";
	msErrMsg += psz;
}

void CEncoder::AppendWarnMsg(tchar* psz)
{
	std::cerr << "Warning: " << psz << "\n";
	if (msWarnMsg.size() > 0)
		msWarnMsg += "\n";
	msWarnMsg += psz;
}
