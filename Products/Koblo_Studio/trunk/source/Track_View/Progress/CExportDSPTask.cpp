
#include "KSOS.h"


// Define this if:
// ... Unfortunately normalization isn't working for some reason
//#define NORMALIZE_DONT_WORK


#ifdef _DEBUG

	// Debug mode - smaller portions
	/*
	const tint32 kiPeakPortionSize	= 32 * 1024;

	const tint32 kiPortionSizeWav	= 16 * 1024;
	const tint32 kiPortionSizeOgg	=  8 * 1024;
	const tint32 kiPortionSizeMp3	=  4 * 1024;
	*/
	const tint32 kiPeakPortionSize	= 1 * 1024;

	const tint32 kiPortionSizeWav	= 1 * 1024;
	const tint32 kiPortionSizeOgg	= 1 * 1024;
	const tint32 kiPortionSizeMp3	= 1 * 1024;

#else _DEBUG

	// Release mode - larger portions
	/*
	const tint32 kiPeakPortionSize	= 64 * 1024;

	const tint32 kiPortionSizeWav	= 32 * 1024;
	const tint32 kiPortionSizeOgg	= 16 * 1024;
	const tint32 kiPortionSizeMp3	=  8 * 1024;
	*/

	const tint32 kiPeakPortionSize	= 1 * 1024;

	const tint32 kiPortionSizeWav	= 1 * 1024;
	const tint32 kiPortionSizeOgg	= 1 * 1024;
	const tint32 kiPortionSizeMp3	= 1 * 1024;

#endif // _DEBUG


	const tint32 kiStopAfterProcessingMS = 50;



CExportDSPTask::CExportDSPTask(CKSPlugIn* pKSPlugIn,
	tint32 iTrackNb, const tchar* pszTrackName,
	ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels, tint32 iTailMS
	)
{
	mpKSPlugIn = pKSPlugIn;

	miTrack = iTrackNb;
	msTrackName = pszTrackName;
	meCodec = eCodec;
	meQuality = eQuality;
	miChannels = iChannels;
	
	miFirstSample = 0;
	miFinalSample = -1;
	mbRemoveInitialSilence = false;
	miTailMS = iTailMS;

	// Arm for peak search
	mfNormalizationFactor = 0.0f;
	// Default: We will go ahead and encode after peak search
	mbSkipFileOutput = false;

	muiPosSaved = 0;
	mpfIntermediateFiles[0] = NULL;
	mpfIntermediateFiles[1] = NULL;
	mpfOutput = NULL;
	mpEncoder = NULL;

	switch (eCodec) {
		// RIFF Wave
		case ac::geAudioCodecWave:		miPortionSize = kiPortionSizeWav;	break;
		// Ogg Vorbis
		case ac::geAudioCodecVorbis:	miPortionSize = kiPortionSizeOgg;	break;
		// LAME mp3 (and default worst case scenario)
		case ac::geAudioCodecLame:
		default:						miPortionSize = kiPortionSizeMp3;	break;
	}
	miBytesPerPortion = miPortionSize * sizeof(tfloat32);

	ppfInputs[0] = NULL;
	ppfInputs[1] = NULL;
	ppfOutputs[0] = NULL;
	ppfOutputs[1] = NULL;

	miActionOrder = 0;
}; // constructor


CExportDSPTask::~CExportDSPTask()
{
	tbool bAbort = false;
	if (mpEncoder) {
		bAbort = true;
		mpEncoder->Destroy();
		mpEncoder = NULL;
	}

	if (mpfOutput) {
		if (bAbort) {
			DeleteDstFile(mpfOutput);
		}

		mpfOutput->Destroy();
		mpfOutput = NULL;
	}
	if (mpfIntermediateFiles[0]) {
		mpfIntermediateFiles[0]->Destroy();
		mpfIntermediateFiles[0] = NULL;
	}
	if (mpfIntermediateFiles[1]) {
		mpfIntermediateFiles[1]->Destroy();
		mpfIntermediateFiles[1] = NULL;
	}
	if (msIntermediateFiles[0].length()) {
		IFile::DeleteFile(msIntermediateFiles[0].c_str());
	}
	if (msIntermediateFiles[1].length()) {
		IFile::DeleteFile(msIntermediateFiles[1].c_str());
	}

	AllocBuffers(0);
} // destructor


void CExportDSPTask::Destroy()
{
	delete dynamic_cast<CExportDSPTask*>(this);
} // Destroy


tbool CExportDSPTask::DoWork()
{
	tbool bError = false;

	switch (miActionOrder) {
		case geExportDSP_Start:
			{
				if (miChannels < 1) {
					// The Channels drop-down was left at "unchanged"
					// This always means stereo for DSP exports,
					// since all channels are processed stereo
					miChannels = 2;
				}

				// Save position
				muiPosSaved = mpKSPlugIn->GetSongPos();

				// Find end position + tail
				tint32 iTailFrames = Float2Int((miTailMS * mpKSPlugIn->GetSampleRate()) / 1000.0f);
				tint64 iLastSample = miFinalSample;
				if (iLastSample < 0) {
					if (IsOutMix()) {
						iLastSample = gpDSPEngine->Get_Session_End_Sample();
					}
					else {
						tint64 iFirstSample_Ignored;
						if (!gpDSPEngine->CalcTrackDuration(miTrack, &iFirstSample_Ignored, &iLastSample)) {
							// Fal back to full length
							iLastSample = gpDSPEngine->Get_Session_End_Sample();
						}
					}
				}
				muiEndPosPlusTail = iLastSample + iTailFrames;

				muiProgressTarget = 0;

				if (mfNormalizationFactor != 0.0f) {
					// We have a predefined normalization factor - don't do peak search
					miActionOrder = geExportDSP_Peak_After;
					miActionOrder++;
				}
				else {
					// Go to next task (peak search)
					miActionOrder++;
				}
			}
			break;

		case geExportDSP_Peak_Before:
			{
				mpKSPlugIn->Playback_InProgressTask(miFirstSample);
				if (IsOutMix())
					mpKSPlugIn->Playback_ExportOutMix(miFirstSample);
				else
					mpKSPlugIn->Playback_ExportTrack(miTrack, miFirstSample, miFinalSample);

				msProgress = "Peak search '";
				msProgress += msTrackName;

				msProgress += "'";
				muiProgressIx = 0;
				muiProgressTarget = muiEndPosPlusTail - miFirstSample;

				AllocBuffers(kiPeakPortionSize);

				mfPeak = 0.001f;
				muiPortionSize = kiPeakPortionSize;
				uiWholePortions = (muiEndPosPlusTail - miFirstSample) / kiPeakPortionSize;
				uiLastPortionSize = muiEndPosPlusTail % kiPeakPortionSize;

				if (!mbSkipFileOutput) {
					// Prepare for intermediate buffer file
					msIntermediateFiles[0] = sDestFolder + sDestNameAndExt + ".raw0";
					msIntermediateFiles[1] = sDestFolder + sDestNameAndExt + ".raw1";
					mpfIntermediateFiles[0] = IFile::Create();
					mpfIntermediateFiles[1] = IFile::Create();
					if ((mpfIntermediateFiles[0] == NULL) || (mpfIntermediateFiles[1] == NULL)) {
						msIntermediateFiles[0] = "";
						msIntermediateFiles[1] = "";
					}
					else {
						if (
							(!mpfIntermediateFiles[0]->Open(msIntermediateFiles[0].c_str(), IFile::FileCreate))
							||
							(!mpfIntermediateFiles[1]->Open(msIntermediateFiles[1].c_str(), IFile::FileCreate))
							)
						{
							msIntermediateFiles[0] = "";
							msIntermediateFiles[1] = "";
							mpfIntermediateFiles[0]->Destroy();
							mpfIntermediateFiles[0] = NULL;
							mpfIntermediateFiles[1]->Destroy();
							mpfIntermediateFiles[1] = NULL;
						}
						else if ((miFirstSample > 0) && (!mbRemoveInitialSilence)) {
							// Dump silence into file
							const tint32 kiChunkSamples = 1024;
							tint64 iSilentChunks = (miFirstSample / kiChunkSamples);
							tint64 iAfterChunks_Samples = miFirstSample - (iSilentChunks * kiChunkSamples);

							// Initialize a memory portion with silence
							tfloat32 afSilentChunk[kiChunkSamples];
							tchar* pacSilentChunk = (tchar*)afSilentChunk;
							memset(pacSilentChunk, '\0', kiChunkSamples * sizeof(tfloat32));

							// Write whole chunks
							for ( ;iSilentChunks > 0; iSilentChunks--) {
								// Since these are temporary files we ignore MSB / LSB questions
								mpfIntermediateFiles[0]->Write(pacSilentChunk, kiChunkSamples * sizeof(tfloat32));
								mpfIntermediateFiles[1]->Write(pacSilentChunk, kiChunkSamples * sizeof(tfloat32));
							}
							// Write extra samples after whole chunks
							if (iAfterChunks_Samples > 0) {
								// Since these are temporary files we ignore MSB / LSB questions
								mpfIntermediateFiles[0]->Write(pacSilentChunk, iAfterChunks_Samples * sizeof(tfloat32));
								mpfIntermediateFiles[1]->Write(pacSilentChunk, iAfterChunks_Samples * sizeof(tfloat32));
							}
						}
					}
				}

				miActionOrder++;
			}
			break;

		case geExportDSP_Peak_Action:
			{
				bError = !DoEncode_InALoop(false);
			}
			break;

		case geExportDSP_Peak_After:
			{
				msProgress = "Peak search done";
				muiProgressIx = muiProgressTarget = 1;

				mfNormalizationFactor = 1.0f;
				mfNormalizationFactor /= mfPeak;

				if (mpfIntermediateFiles[0]) {
					mpfIntermediateFiles[0]->Close();
				}
				if (mpfIntermediateFiles[1]) {
					mpfIntermediateFiles[1]->Close();
				}

				if (mbSkipFileOutput) {
					miActionOrder = geExportDSP_After;
				}
				miActionOrder++;
			}
			break;

		case geExportDSP_Before:
			{
				if (mbSkipFileOutput) {
					// Skip encoding
					mpKSPlugIn->Playback_InProgressTask(muiPosSaved);
					msProgress = "";
					muiProgressIx = muiProgressTarget = 0;

					miActionOrder = geExportDSP_After;
					miActionOrder++;
				}
				else {
					// Prepare for encoding
					tint64 iPosToStart = (mbRemoveInitialSilence) ? miFirstSample : 0;
					if (msIntermediateFiles[0].length()) {
						// Load from intermediate files
						mpfIntermediateFiles[0]->Open(msIntermediateFiles[0].c_str(), IFile::FileRead);
						mpfIntermediateFiles[1]->Open(msIntermediateFiles[1].c_str(), IFile::FileRead);
					}
					else {
						if (IsOutMix())
							mpKSPlugIn->Playback_ExportOutMix(iPosToStart);
						else
							mpKSPlugIn->Playback_ExportTrack(miTrack, iPosToStart, miFinalSample);
					}

					msProgress = "Exporting '";
					msProgress += sDestNameAndExt;

					msProgress += "'";
					muiProgressIx = 0;
					muiProgressTarget = (muiEndPosPlusTail - iPosToStart);

					AllocBuffers(miPortionSize);

					muiPortionSize = miPortionSize;
					uiWholePortions = (muiEndPosPlusTail - iPosToStart) / miPortionSize;
					uiLastPortionSize = muiEndPosPlusTail % miPortionSize;

					miActionOrder++;
				}
			}
			break;

		case geExportDSP_Action:
			{
				if (mpfOutput == NULL) {
					bError = !DoEncode_FirstTimeHere();
				}

				if (!bError) {
					bError = !DoEncode_InALoop(true);
				}
			}
			break;

		case geExportDSP_After:
			{
				mpKSPlugIn->Playback_InProgressTask(muiPosSaved);
				msProgress = "Export done";
				muiProgressIx = muiProgressTarget = 1;

				miActionOrder++;
			}
			break;

		default:
			// Why are we here?
			bError = true;
			break;

	}

	if (bError) {
		miActionOrder = geExportDSP_Done;
	}
	return !bError;
} // DoWork


tbool CExportDSPTask::DoEncode_FirstTimeHere()
{
	tbool bError = false;

	mpfOutput = IFile::Create();
	if (mpfOutput == NULL) {
		msExtendedError = "Internal error creating IFile object";
		bError = true;
	}
	else {
		std::string sPathName = sDestFolder + sDestNameAndExt;

		if (!mpfOutput->Open(sPathName.c_str(), IFile::FileCreate)) {
			msExtendedError = std::string("Unable to write access file:\n  ") + sPathName;
			bError = true;
		}

		if (!bError) {
			tchar pszErr[1024];
			mpEncoder = IEncoder::Create(meCodec, pszErr, 1024);
			if (mpEncoder == NULL) {
				msExtendedError = pszErr;
				bError = true;
			}
			else if (!mpEncoder->Init(mpfOutput, meQuality, miChannels)) {
				mpEncoder->GetErrMsg(pszErr, 1024, true);
				msExtendedError = std::string("IEncoder::Init(..) failed:\n") + pszErr;
				bError = true;
			}
			else if (!mpEncoder->SetRawMode(true, 2, false, 32, mpKSPlugIn->GetSampleRate())) {
				mpEncoder->GetErrMsg(pszErr, 1024, true);
				msExtendedError = std::string("IEncoder::SetRawMode(..) failed:\n") + pszErr;
				bError = true;
			}
			else {
				mpEncoder->SetNormalizationFactor(mfNormalizationFactor);
				iAccumOverflows = 0;
			}
		}
	}

	return !bError;
} // DoEncode_FirstTimeHere


tbool CExportDSPTask::DoEncode_InALoop(tbool bWrite)
{
	tbool bDone = false;
	tbool bSuccess = true;
	tuint32 uiTimeMS0 = ITime::GetTimeMS();
	tuint32 uiDiff = 0;
	tint32 iActionOrderSave = miActionOrder;
	do {
		if (!DoEncode(bWrite)) {
			bSuccess = false;
		}
		else {
			tuint32 uiDiff = ITime::GetTimeMS() - uiTimeMS0;
			if ((uiDiff >= kiStopAfterProcessingMS) || (miActionOrder != iActionOrderSave)) {
				bDone = true;
			}
		}
	} while ((bSuccess) && (!bDone));

	return bSuccess;
}


tbool CExportDSPTask::DoEncode(tbool bWrite)
{
	tbool bError = false;

	// Loop through Process(...) until end-pos + tail
	if (uiWholePortions > 0) {
		uiWholePortions--;

		if (!bWrite) {
			// Get samples
			mpKSPlugIn->ProcessNonInPlace(ppfOutputs, (const tfloat**)ppfInputs, (tuint32)muiPortionSize);
			// Find peak
			tfloat32* pfL = (tfloat32*)pcOutputL;
			tfloat32* pfR = (tfloat32*)pcOutputR;
			for (tint32 i = 0; i < muiPortionSize; i++) {
				tfloat32 f;
				f = fabsf(pfL[i]);
				if (f > mfPeak) {
					mfPeak = f;
				}
				f = fabsf(pfR[i]);
				if (f > mfPeak) {
					mfPeak = f;
				}
			}
			// Maybe write intermediate files
			if (msIntermediateFiles[0].length()) {
				// Since these are temporary files we ignore MSB / LSB questions
				tint64 iBytes = muiPortionSize * sizeof(tfloat32);
				tint64 iWritten = mpfIntermediateFiles[0]->Write(pcOutputL, iBytes);
				if (iWritten == iBytes) {
					iWritten = mpfIntermediateFiles[1]->Write(pcOutputR, iBytes);
				}
				if (iWritten != iBytes) {
					// Out of disk space writing intermediate files - stop buffering and delete them
					mpfIntermediateFiles[0]->Close();
					mpfIntermediateFiles[1]->Close();
					IFile::DeleteFile(msIntermediateFiles[0].c_str());
					IFile::DeleteFile(msIntermediateFiles[1].c_str());
					msIntermediateFiles[0] = "";
					msIntermediateFiles[1] = "";
				}
			}
		}
		else {
			if (msIntermediateFiles[0].length()) {
				// Load samples from intermediate files
				// Since these are temporary files we ignore MSB / LSB questions
				mpfIntermediateFiles[0]->Read(pcOutputL, muiPortionSize * sizeof(tfloat32));
				mpfIntermediateFiles[1]->Read(pcOutputR, muiPortionSize * sizeof(tfloat32));
			}
			else {
				// Get samples
				mpKSPlugIn->ProcessNonInPlace(ppfOutputs, (const tfloat**)ppfInputs, (tuint32)muiPortionSize);
			}
			// Export
			if (!mpEncoder->ProcessRaw(pcOutputL, pcOutputR, miBytesPerPortion, &iAccumOverflows)) {
				tchar pszErr[1024];
				mpEncoder->GetErrMsg(pszErr, 1024, true);
				msExtendedError = std::string("IEncoder::ProcessRaw(..) failed:\n") + pszErr;
				bError = true;
			}
		}

		if (!bError) {
			// Display progress
			muiProgressIx += muiPortionSize;
		}
	}
	else {
		// No more whole portions left
		
		// - any partial?
		if (uiLastPortionSize > 0) {
			tint32 iBytesInLastPortion = (tint32)uiLastPortionSize * sizeof(tfloat32);
			if (!bWrite) {
				// Get samples
				mpKSPlugIn->ProcessNonInPlace(ppfOutputs, (const tfloat**)ppfInputs, miPortionSize);
				// Find peak
				for (tint32 i = 0; i < uiLastPortionSize; i++) {
					tfloat32* pfL = (tfloat32*)pcOutputL;
					tfloat32* pfR = (tfloat32*)pcOutputR;
					tfloat32 f;
					f = fabsf(pfL[i]);
					if (f > mfPeak) {
						mfPeak = f;
					}
					f = fabsf(pfR[i]);
					if (f > mfPeak) {
						mfPeak = f;
					}
				}
				// Maybe write intermediate files
				if (msIntermediateFiles[0].length()) {
					// Since these are temporary files we ignore MSB / LSB questions
					tint64 iWritten = mpfIntermediateFiles[0]->Write(pcOutputL, iBytesInLastPortion);
					if (iWritten == iBytesInLastPortion) {
						iWritten = mpfIntermediateFiles[1]->Write(pcOutputR, iBytesInLastPortion);
					}
					if (iWritten != iBytesInLastPortion) {
						// Out of disk space writing intermediate files - stop buffering and delete them
						mpfIntermediateFiles[0]->Close();
						mpfIntermediateFiles[1]->Close();
						IFile::DeleteFile(msIntermediateFiles[0].c_str());
						IFile::DeleteFile(msIntermediateFiles[1].c_str());
						msIntermediateFiles[0] = "";
						msIntermediateFiles[1] = "";
					}
				}
			}
			else {
				if (msIntermediateFiles[0].length()) {
					// Load samples from intermediate files
					mpfIntermediateFiles[0]->Read(pcOutputL, iBytesInLastPortion);
					mpfIntermediateFiles[1]->Read(pcOutputR, iBytesInLastPortion);
				}
				else {
					// Get samples
					mpKSPlugIn->ProcessNonInPlace(ppfOutputs, (const tfloat**)ppfInputs, miPortionSize);
				}
				// Export
				if (!mpEncoder->ProcessRaw(pcOutputL, pcOutputR, iBytesInLastPortion, &iAccumOverflows)) {
					tchar pszErr[1024];
					mpEncoder->GetErrMsg(pszErr, 1024, true);
					msExtendedError = std::string("IEncoder::ProcessRaw(..) failed:\n") + pszErr;
					bError = true;
				}
			}
		}

		// Finish up
		if (!bError) {
			if (bWrite) {
				if (!mpEncoder->Finalize()) {
					tchar pszErr[1024];
					mpEncoder->GetErrMsg(pszErr, 1024, true);
					msExtendedError = std::string("IEncoder::Finalize() failed:\n") + pszErr;
					bError = true;
				}
			}
		}

		if (!bError) {
			muiProgressIx += uiLastPortionSize;

			if (bWrite) {
				// Close encoder to signal all is well
				mpEncoder->Destroy();
				mpEncoder = NULL;
			}

			miActionOrder++;
		}
	}

	return !bError;
} // DoEncode


tbool CExportDSPTask::IsDone()
{
	return (miActionOrder >= geExportDSP_Done);
} // IsDone


void CExportDSPTask::AllocBuffers(tint32 iSamples)
{
	// First destroy buffers ..
	if (ppfInputs[0]) {
		delete[] ppfInputs[0];
		ppfInputs[0] = NULL;
	}
	if (ppfInputs[1]) {
		delete[] ppfInputs[1];
		ppfInputs[1] = NULL;
	}

	if (ppfOutputs[0]) {
		delete[] ppfOutputs[0];
		ppfOutputs[0] = NULL;
	}
	if (ppfOutputs[1]) {
		delete[] ppfOutputs[1];
		ppfOutputs[1] = NULL;
	}
	pcOutputL = NULL;
	pcOutputR = NULL;

	// .. then maybe create new buffers
	if (iSamples > 0) {
		ppfInputs[0] = new tfloat32[iSamples];
		memset(ppfInputs[0], '\0', iSamples * sizeof(tfloat32));
		ppfInputs[1] = new tfloat32[iSamples];
		memset(ppfInputs[1], '\0', iSamples * sizeof(tfloat32));

		ppfOutputs[0] = new tfloat32[iSamples];
		ppfOutputs[1] = new tfloat32[iSamples];
		pcOutputL = (tchar*)ppfOutputs[0];
		pcOutputR = (tchar*)ppfOutputs[1];
	}
} // AllocBuffers
