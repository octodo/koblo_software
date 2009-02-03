#include "KSOS.h"

CTrack_DSP::CTrack_DSP(CDSP* pDSP, tint32 iChannelNumber, tbool bIsBusOrMix, CTrack_DSP** ppAUXes)
	: mpDSPTools(NULL), mpDSP(pDSP), miTrack(iChannelNumber), // mpDezipperVolume(0),
	miDestinationNumberOfChannels(2), miNumberOfChannelsForPanner(2),
	mfPanningLeft(0.71),mfPanningRight(0.71), mfPanningFrontBack(0.0),
	mbIsBusOrMix(bIsBusOrMix), miModeChannelCount(0), miInputChannelCount(2),
	mppAUXes(ppAUXes), mfVolumeSlider(1.0),
	mbArmed(false), mpFileRecording(NULL)
{
	mpbInsertBypass[0] = mpbInsertBypass[1] = mpbInsertBypass[2] = mpbInsertBypass[3] = false;

	if (mbIsBusOrMix) {
		miTrack += 1024;
	}

	mpBuffer = new CBuffer();
	


	mbLimitDataFromStream = false;

	muiFirstInput = 0;
	mbMeterBeforeEffects = false;
	
	mpDezipperPreAmp = NULL;
	for (tint i=0; i<8; i++) {
		mapDezipperOutAmp[0] = NULL;
	}	

	_CalcSetNumberOfChannelsForPanner();

	mitRegionsInfo = mRegionInfoList.begin();

	mppInsert[0] = NULL;
	mppInsert[1] = NULL;
	mppInsert[2] = NULL;
	mppInsert[3] = NULL;

	// Lasse, fix 2008-08-26
	// Avoid violent noise caused by Echo and KVerb volumes being default set at -400 millions (windows debug version only)
	SetAUXVolume(0, 0.0f);
	SetAUXVolume(1, 0.0f);
	
}

CTrack_DSP::~CTrack_DSP()
{
	// Lasse, out-comment 2008-06-20 - guess we don't need this..
	//CAutoLock lock(mInProcessMutex);
	// .. Lasse
	
	delete mpBuffer;
}

void CTrack_DSP::Initialize()
{
	mpDSPTools = st::IDSPTools::Create();

	for (tint i=0; i<8; i++) {
		mapDezipperOutAmp[i] = new CBaseDezipper2(0.0, 4.0, 1.0);
		mapDezipperOutAmp[i]->SetTarget(1.0);
	}
	
	mpDezipperPreAmp = new CBaseDezipper2(0.0, 4.0, 1.0);
	mpDezipperPreAmp->SetTarget(1.0);
	
	_CalcSetVolumeDezippers();
	_KillPeakVolume();
}

void CTrack_DSP::DeInitialize()
{
	//mpDezipperVolume->Destroy();
	for (tint i=0; i<8; i++)
		mapDezipperOutAmp[i]->Destroy();
	
	mpDezipperPreAmp->Destroy();

	mpDSPTools->Destroy();
	mpDSPTools = NULL;
}

void CTrack_DSP::SetSongPosition(tuint64 uiPosNew)
{
	// (mo) Note: Currently the sound position is set, which will flush its buffer, leading to decreased performance each time this method is called

 	muiSongPos = uiPosNew;

	// Locate next region to play
	mitRegionsInfo = mRegionInfoList.begin();
	while (mitRegionsInfo != mRegionInfoList.end()) {
	
		tint64 uiTrack_Pos	=	(*mitRegionsInfo)->uiTrack_Pos;
		
		if (uiTrack_Pos > muiSongPos) {
			// Next region is later than position
			// Reset next region position
			(*mitRegionsInfo)->pRegion->SetPos(0);
			break;
		}

		CRegion_DSP* pRegion = (*mitRegionsInfo)->pRegion;
		tint64 iLength = pRegion->Get_Duration();
		if (uiTrack_Pos + iLength - 1 < uiPosNew) {
			// Position is beyond end of region
			mitRegionsInfo++;
		}
		else {
			// Position is "within" region

			// Calculate the sound position and set it
			tuint64 uiTrackPosOffset = uiPosNew - uiTrack_Pos;
			//(*mitRegions)->pRegion->SetSampleOffSet((tuint32)uiTrackPosOffset); SetStartPos
			// Error sample offset is the clipping og the region
			(*mitRegionsInfo)->pRegion->SetPos((tuint32)uiTrackPosOffset);
			break;
		}
	}
}

void CTrack_DSP::AddInsert(tint32 iInsert, tuint32 uiCompanyID, tuint32 uiProductID, tuint32 uiProductID2)
{
	CPlugInManager* pPlugManager = gpApplication->GetPlugInManager();
	
	if (mppInsert[iInsert]) {
		pPlugManager->UnloadPlugIn(mhInserts[iInsert], miTrack, iInsert);
	}
	
	if (uiCompanyID == 0 && uiProductID == 0) {
		mppInsert[iInsert] = NULL;
	}
	else {
		// Stop updating of meters
		gpApplication->All_Plugins_Created(false);
		mhInserts[iInsert] = pPlugManager->LoadPlugIn(uiCompanyID, uiProductID, miTrack, iInsert);
		
		if (mhInserts[iInsert] == CPlugInManager::mInvalidHandleValue) {
			mhInserts[iInsert] = pPlugManager->LoadPlugIn(2, uiProductID2, miTrack, iInsert);
		}
		if (mhInserts[iInsert] == CPlugInManager::mInvalidHandleValue) {
			mppInsert[iInsert] = NULL;
		}
		else {
			mppInsert[iInsert] = pPlugManager->GetPlugInFromHandle(mhInserts[iInsert]);
		}
		// Restart meter updat
		gpApplication->All_Plugins_Created(true);
	}
}

void CTrack_DSP::Process(tint32 iSamples)
{
	if (mbIsBusOrMix == true) {
		mpBuffer->SetChannelsNoConvert(2);
	}
	else
		mpBuffer->SetChannelsNoConvert(1);
		
	if (mbIsBusOrMix == false) {
		mpBuffer->Clear();
	}

	// (mo) Note: Currently if 2 sounds overlap, the second will be played out of sync

	// We use this temporary copy, so we can change it in the loop
	tuint64 uiSongPosTemp = muiSongPos;
	tuint64 iSamplesLeft = iSamples;
	tuint64 iBufferIndex = 0;
	while (mitRegionsInfo != mRegionInfoList.end() &&
			iSamplesLeft) {
		// Still region(s) to play
		CRegion_DSP* pRegion = (*mitRegionsInfo)->pRegion;
		tuint64 uiPosStart = (*mitRegionsInfo)->uiTrack_Pos;
		tuint64 uiPosEnd = uiPosStart + pRegion->Get_Duration() - 1;

		tint32 iChannels = pRegion->GetChannels();
		mpBuffer->SetChannels(iChannels);

		if (uiPosStart <= uiSongPosTemp) {
			if (uiPosEnd >= uiSongPosTemp) {
				tint32 iSamplesToGet = (tint32)(uiPosEnd - uiPosStart + 1);
				if (iSamplesToGet > iSamplesLeft) {
					iSamplesToGet = (tint32)iSamplesLeft;
				}
				if (iSamplesToGet > uiPosEnd - uiSongPosTemp + 1) {
					iSamplesToGet = (tint32)(uiPosEnd - uiSongPosTemp + 1);
				}

				if (iChannels == 1) {
					tfloat32* pfData = mpBuffer->GetData(0);
					tfloat32* ppfData[1];
					ppfData[0] = pfData + iBufferIndex;
					pRegion->GetSamples(ppfData, iSamplesToGet);
					if (mbLimitDataFromStream) {
						// Maybe silence some samples at start and/or end of portion

						// Silence at start?
						tint64 iSilentSamples_Start = miFirstStreamIx_Limited - uiSongPosTemp;
						if (iSilentSamples_Start > 0) {
							// We need to silence some samples at start of portion
							if (iSilentSamples_Start > iSamplesToGet) {
								iSilentSamples_Start = iSamplesToGet;
							}
							mpDSPTools->Clear(ppfData[0], iSilentSamples_Start);
						}

						// Silence at end?
						tint64 iSilentSamples_EndIx = miFinalStreamIx_Limited - uiSongPosTemp;
						if (iSilentSamples_EndIx < iSamplesToGet) {
							// We need to silence some samples at end of portion
							if (iSilentSamples_EndIx < 0) {
								iSilentSamples_EndIx = 0;
							}
							tint32 iSamplesToSilence = iSamplesToGet - iSilentSamples_EndIx;
							tfloat32* pf0Ix = ((tfloat32*)ppfData[0]) + iSilentSamples_EndIx;
							mpDSPTools->Clear(pf0Ix, iSamplesToSilence);
						}
					}
				}
				else {
					tfloat32* pfData1 = mpBuffer->GetData(0);
					tfloat32* pfData2 = mpBuffer->GetData(1);
					tfloat32* ppfData[2];
					ppfData[0] = pfData1 + iBufferIndex;
					ppfData[1] = pfData2 + iBufferIndex;
					pRegion->GetSamples(ppfData, iSamplesToGet);
					if (mbLimitDataFromStream) {
						// Maybe silence some samples at start and/or end of portion

						// Silence at start?
						tint64 iSilentSamples_Start = miFirstStreamIx_Limited - uiSongPosTemp;
						if (iSilentSamples_Start > 0) {
							// We need to silence some samples at start of portion
							if (iSilentSamples_Start > iSamplesToGet) {
								iSilentSamples_Start = iSamplesToGet;
							}
							mpDSPTools->Clear(ppfData[0], iSilentSamples_Start);
							mpDSPTools->Clear(ppfData[1], iSilentSamples_Start);
						}

						// Silence at end?
						tint64 iSilentSamples_EndIx = miFinalStreamIx_Limited - uiSongPosTemp;
						if (iSilentSamples_EndIx < iSamplesToGet) {
							// We need to silence some samples at end of portion
							if (iSilentSamples_EndIx < 0) {
								iSilentSamples_EndIx = 0;
							}
							tint32 iSamplesToSilence = iSamplesToGet - iSilentSamples_EndIx;
							tfloat32* pf0Ix = ((tfloat32*)ppfData[0]) + iSilentSamples_EndIx;
							tfloat32* pf1Ix = ((tfloat32*)ppfData[1]) + iSilentSamples_EndIx;
							mpDSPTools->Clear(pf0Ix, iSamplesToSilence);
							mpDSPTools->Clear(pf1Ix, iSamplesToSilence);
						}
					}
				}

				iSamplesLeft -= iSamplesToGet;
				uiSongPosTemp += iSamplesToGet;
				iBufferIndex += iSamplesToGet;
			}
			else {
				// Skip to next region
				mitRegionsInfo++;

				if (mitRegionsInfo != mRegionInfoList.end()) {
					(*mitRegionsInfo)->pRegion->SetPos(0);

					if ((*mitRegionsInfo)->pRegion->GetChannels() > iChannels) {
						iChannels = (*mitRegionsInfo)->pRegion->GetChannels();
					}
				}
			}
		}
		else {
			// Region not started yet
			tuint64 uiSamplesToClear = uiPosStart - uiSongPosTemp;
			if (uiSamplesToClear > iSamplesLeft) {
				uiSamplesToClear = iSamplesLeft;
			}

			tfloat32* pfData = mpBuffer->GetData(0);
			mpDSPTools->Clear(pfData + iBufferIndex, (tuint32)uiSamplesToClear);

			iSamplesLeft -= (tuint32)uiSamplesToClear;
			uiSongPosTemp += (tuint32)uiSamplesToClear;
			iBufferIndex += (tuint32)uiSamplesToClear;
		}
	}

/*	mpBuffer->SetChannels(2);
	muiSongPos += iSamples;
	return;*/

	if (mbArmed) {
		tint32 iInputChannel = miInputChannel;

		// (lasse) Huh? What's this for?
		tint32 iChannels = 1;
		if (miInputChannel >= 1000) {
			iChannels = 2;
			iInputChannel -= 1000;
		}

		mpBuffer->SetChannels(iChannels);

		mpDSP->GetInput(mpBuffer, iChannels, iInputChannel, iSamples);

		if (miRecordingChannels == -1) {
			// (lasse) This is a HACK - need a way to determine
			miRecordingChannels = 2;//iChannels;
			// .. (lasse)

			iChannels = miRecordingChannels;
		}

		if (mpFileRecording) {
			SetInputChannelCount(miRecordingChannels);
			SetTrackMode(miRecordingChannels);

			if (miRecordingChannels == 1) {
				tfloat32* pf = mpBuffer->GetData(0);
				for (tint32 iPeak = iSamples; iPeak > 0; iPeak--) {
					tfloat32 f = fabs(*pf++);
					if (f > mfPeak)	mfPeak = f;
				}
				mpFileRecording->Write((const tchar*)mpBuffer->GetData(0), iSamples * sizeof(tfloat32));
			}
			else {
				tfloat32* pfL = mpBuffer->GetData(0);
				tfloat32* pfR = mpBuffer->GetData(1);
				for (tint32 iPeak = iSamples; iPeak > 0; iPeak--) {
					tfloat32 fL = *pfL++;
					tfloat32 fR = *pfR++;
					tfloat32 fMono = fL + fR;
					fL = fabs(fL);
					fR = fabs(fR);
					fMono = fabs(fMono);
					if (fL > mfPeak)		mfPeak = fL;
					if (fR > mfPeak)		mfPeak = fR;
					if (fMono > mfPeakMono)	mfPeakMono = fMono;
				}
				mpFileRecording->Write((const tchar*)mpBuffer->GetData(0), iSamples * sizeof(tfloat32));
				mpFileRecording->Write((const tchar*)mpBuffer->GetData(1), iSamples * sizeof(tfloat32));
			}
		}
	}

	// Tell the buffer how many channels it has from outset
	mpBuffer->SetChannels(miInputChannelCount);

	// If the bus mode has more channels than the raw input we haven't done the up-mix yet - do it now
	if (miModeChannelCount > miInputChannelCount) {
		// Mix up
		mpBuffer->SetChannels(miModeChannelCount);
	}

	// Apply effects
	tint32 iInsert;
	for (iInsert = 0; iInsert < giNumber_Of_Inserts; iInsert++) {
		if (mppInsert[iInsert] && mpbInsertBypass[iInsert] == false) {
			tfloat32* ppfBuffersOut[2];
			ppfBuffersOut[0] = mpfBufferTmp1;
			ppfBuffersOut[1] = mpfBufferTmp2;
			tfloat32* pfData1 = mpBuffer->GetData(0);
			tfloat32* pfData2 = mpBuffer->GetData(1);
			tfloat* ppfData[2];
			ppfData[0] = pfData1;
			ppfData[1] = pfData2;
			mppInsert[iInsert]->ProcessNonInPlace(ppfBuffersOut, (const tfloat**)ppfData, iSamples);
			memcpy(pfData1, ppfBuffersOut[0], iSamples * sizeof(tfloat32));
			memcpy(pfData2, ppfBuffersOut[1], iSamples * sizeof(tfloat32));
		}
	}

	// Prepare for panning
	mpBuffer->SetChannels(miNumberOfChannelsForPanner);
	
	// Apply out amp
	for (tuint iChannel=0; iChannel<mpBuffer->GetChannels(); iChannel++) {
		CBaseDezipper2* pDezipper = mapDezipperOutAmp[iChannel];
			
		//tfloat32 fTest, fPeak;
			
		//tint32 iSamplesAlign8 = (iSamples % 8) * 8;
		tfloat32* pfData = mpBuffer->GetData(iChannel);
		//tfloat32* pfDataStop = pfData + iSamples;
		//tfloat32* pfDataStopAlign8 = pfData + iSamplesAlign8;
			
		if (pDezipper->IsDezipNeeded()) {
			// Out-amp is changing - do a de-zip
			tfloat afDezip[1024];
			pDezipper->DezipSamples(afDezip, iSamples, 44100);
			tfloat32* pfMul = afDezip;
				
			// Don't keep old peaks (if there) when amp is changing
			mbKillDecay = true;
				
			mpDSPTools->Mul(pfData, pfMul, iSamples);
			/*
			// Apply and collect 8 aligned
			while (pfData < pfDataStopAlign8) {
				// Apply amp and collect meter value
				fTest = *pfData++ *= *pfMul++;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//1
				fTest = *pfData++ *= *pfMul++;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//2
				fTest = *pfData++ *= *pfMul++;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//3
				fTest = *pfData++ *= *pfMul++;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//4
				fTest = *pfData++ *= *pfMul++;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//5
				fTest = *pfData++ *= *pfMul++;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//6
				fTest = *pfData++ *= *pfMul++;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//7
				fTest = *pfData++ *= *pfMul++;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//8
			}
				
			// Apply and collect remainder
			while (pfData < pfDataStop) {
				// Apply amp and collect meter value
				fTest = *pfData++ *= *pfMul++;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//1
			}
			*/
		}
		else {
			// Out-amp factor is the same for all samples - just multiply
			tfloat fOutAmp = pDezipper->GetCurrent();
				
			// Keep old peaks (if not yet read) because amp isn't changing
			//fPeak = mafPeakVolumes[iChannel];
				
			if ((fOutAmp < 0.99999) || (fOutAmp > 1.00001)) {

				mpDSPTools->Mul(pfData, fOutAmp, iSamples);
				/*
				// Apply and collect 8 aligned
				while (pfData < pfDataStopAlign8) {
					// Apply amp and collect meter value
					fTest = *pfData++ *= fOutAmp;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//1
					fTest = *pfData++ *= fOutAmp;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//2
					fTest = *pfData++ *= fOutAmp;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//3
					fTest = *pfData++ *= fOutAmp;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//4
					fTest = *pfData++ *= fOutAmp;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//5
					fTest = *pfData++ *= fOutAmp;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//6
					fTest = *pfData++ *= fOutAmp;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//7
					fTest = *pfData++ *= fOutAmp;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//8
				}
				
				// Apply and collect remainder
				while (pfData < pfDataStop) {
					// Apply amp and collect meter value
					fTest = *pfData++ *= fOutAmp;		fTest = fabsf(fTest);		if (fTest > fPeak) fPeak = fTest;	//1
				}
				 */
			}
			else {
				/*
				// No output amplification - just collect meter values
										
				// Collect alligned to 8
				while (pfData < pfDataStopAlign8) {
					// Collect meter value
					fTest = fabsf(*pfData++);		if (fTest > fPeak) fPeak = fTest;	//1
					fTest = fabsf(*pfData++);		if (fTest > fPeak) fPeak = fTest;	//2
					fTest = fabsf(*pfData++);		if (fTest > fPeak) fPeak = fTest;	//3
					fTest = fabsf(*pfData++);		if (fTest > fPeak) fPeak = fTest;	//4
					fTest = fabsf(*pfData++);		if (fTest > fPeak) fPeak = fTest;	//5
					fTest = fabsf(*pfData++);		if (fTest > fPeak) fPeak = fTest;	//6
					fTest = fabsf(*pfData++);		if (fTest > fPeak) fPeak = fTest;	//7
					fTest = fabsf(*pfData++);		if (fTest > fPeak) fPeak = fTest;	//8
				}

				// Collect any remainder
				while (pfData < pfDataStop) {
					// Collect meter value
					fTest = fabsf(*pfData++);		if (fTest > fPeak) fPeak = fTest;	//1
				}
				*/
			}
		}
			
		//mafPeakVolumes[iChannel] = fPeak;
	}

	tint32 iAUX;
	for (iAUX = 0; iAUX < 2; iAUX++) {
		if (mpfAUXVolume[iAUX] != 0) {
			CBuffer* pBufferAUX = mppAUXes[iAUX]->GetBuffer();

//			*pBufferAUX += *mpBuffer;
			pBufferAUX->Accumulate(*mpBuffer, mpfAUXVolume[iAUX]);
		}
	}

	// Maybe do down or up mix for destination
	mpBuffer->SetChannels(miDestinationNumberOfChannels);

	muiSongPos += iSamples;
} // Process


void CTrack_DSP::Start()
{
	tint32 iInsert;
	for (iInsert = 0; iInsert < giNumber_Of_Inserts; iInsert++) {
		if (mppInsert[iInsert]) {
			mppInsert[iInsert]->Start();
		}
	}

	ASSERT(mpFileRecording == NULL);

	if (mbArmed && gpApplication->IsRecording()) {
		std::string sChannelName = gpApplication->GetChannelName(miTrack);
		if (sChannelName.size() == 0) {
			tchar psz[32];
			sprintf(psz, "Track %d", miTrack + 1);
			sChannelName = std::string(psz);
		}

		std::string sPathNameDest = gpApplication->GetProjDir_Audio();
		sPathNameDest += "Clips:";
		sPathNameDest += "Recording_";
		sPathNameDest += sChannelName;
		sPathNameDest += "_";

		std::string sPathName = gpApplication->GetProjDir_Audio();
		sPathName += "Recording_";
		sPathName += sChannelName;
		sPathName += "_";
		
		tint32 iIndex = 0;
		tbool bFoundIt = false;
		while (bFoundIt == false) {
			std::string s		= sPathName;
			std::string sDest	= sPathNameDest;
			std::string sLeft	= sPathNameDest;
			tchar psz[32];
			sprintf(psz, "%d.wav", iIndex);
			tchar pszLeft[32];
			sprintf(pszLeft, "%d_0000-1.wav", iIndex);
			s += std::string(psz);
			sDest += std::string(psz);
			sLeft += std::string(pszLeft);
			CAutoDelete<IFile> pFile(IFile::Create());
			if (pFile->Open(sDest.c_str(), IFile::FileRead) == false) {
				if (pFile->Open(sLeft.c_str(), IFile::FileRead) == false) {
					// File didn't exist, so use file name
					bFoundIt = true;
					sPathName = s;
					sPathNameDest = sDest;
				}
			}

			iIndex++;
		}

		msRecordingName = sPathName;
		msRecordingNameDest = sPathNameDest;
		sPathName += ".tmp";
		mpFileRecording = IFile::Create();
		mpFileRecording->Open(sPathName.c_str(), IFile::FileCreate);
		mfPeakMono = mfPeak = 1.0f / 32;

		// (lasse) This is a HACK - need a way to determine
		miRecordingChannels = 2;//-1;
		// .. (lasse)

		miRecordingSongPos = gpApplication->GetSongPos();
	}
} // Start


void CTrack_DSP::Stop()
{
	tint32 iInsert;
	for (iInsert = 0; iInsert < giNumber_Of_Inserts; iInsert++) {
		if (mppInsert[iInsert]) {
			mppInsert[iInsert]->Stop();
		}
	}

	if (mpFileRecording) {
		mpFileRecording->Destroy();
		mpFileRecording = NULL;

		tbool bSuccess = true;
		tbool bAbortOperation = false;
		tchar pszErrMsg[1024];
		*pszErrMsg = '\0';
		tint64 iSamples = 0;
		ac::EQuality eQuality = ac::geQualityUnknown;
		tint64 iAccumOverflows = 0;
		tfloat32 fNormalizationFactor = 1.0f / mfPeak;
		tfloat32 fNormalizationFactorMono = 1.0f / mfPeakMono;

		{
			CAutoDelete<IFile> pFileSrc(IFile::Create());
			CAutoDelete<IFile> pFileDest(IFile::Create());

			bSuccess &= pFileSrc->Open((msRecordingName + std::string(".tmp")).c_str(), IFile::FileRead);
			bSuccess &= pFileDest->Open(msRecordingName.c_str(), IFile::FileCreate);

			if (!bSuccess) {
				sprintf(pszErrMsg, "File open operation(s) failed");
			}
			else {
				tint64 iDataBytes = pFileSrc->GetSizeWhenOpened();
				iSamples = (iDataBytes / sizeof(tfloat32)) / miRecordingChannels;
				if (iSamples <= 0)
					bSuccess = false;
			}

			if (bSuccess) {
				CAutoDelete<ac::IEncoder> pEncoder(ac::IEncoder::Create(ac::geAudioCodecWave, pszErrMsg, 1024));
				if (!pEncoder) {
					bSuccess = false;
				}
				else {
					if (bSuccess) {
						std::string sMsg =
							std::string("Do you want to import the recorded part as 24 bit Wave?\n")
							+
							//sName
							//+
							//"\n"
							"\n"
							"- Yes:  \t24 bits = no quality loss\n"
							"- No:   \t16 bits = maybe quality loss\n"
							"- Cancel:\tAbort";

						ge::IWindow::EMsgBoxReturn eRc = ge::IWindow::ShowMessageBox(
							sMsg.c_str(), "Select bit depth", ge::IWindow::MsgBoxYesNoCancel);
						//bAskedAboutBits = true;
						switch (eRc) {
							case ge::IWindow::MsgBoxRetYes: eQuality = ac::geQualityLossless24; break;
							case ge::IWindow::MsgBoxRetNo: eQuality = ac::geQualityLossless16; break;
							default: bAbortOperation = true; break;
						}
					}

					tbool bDownMix = false;
					if (bSuccess && !bAbortOperation) {
						tchar pszMsg[] =
							"Do you want to import as stereo?\n"
							"\n"
							"- Yes:  \tStereo\n"
							"- No:   \tDownmix to mono\n"
							"- Cancel:\tAbort";
						ge::IWindow::EMsgBoxReturn eRc = ge::IWindow::ShowMessageBox(
							pszMsg, "Import as stereo", ge::IWindow::MsgBoxYesNoCancel);
						switch (eRc) {
							case ge::IWindow::MsgBoxRetYes: break;
							case ge::IWindow::MsgBoxRetNo: bDownMix = true; break;
							default: bAbortOperation = true; break;
						}
					}

					if (bSuccess && !bAbortOperation) {
						tint32 iDestChannels = (bDownMix) ? 1 : miRecordingChannels;
						if (
							(!pEncoder->Init(pFileDest, eQuality))
							||
							(!pEncoder->SetRawMode(true, iDestChannels, false, 32, gpDSPEngine->GetSampleRate()))
						) {
							bSuccess = false;
						}
					}

					if (bSuccess && !bAbortOperation) {
						tint64 iSkipExtraLatencySamples = gpApplication->AudioInput_IntermediateBuffer_CalcExtraLatency();
						pEncoder->SetNormalizationFactor((bDownMix) ? fNormalizationFactorMono : fNormalizationFactor);

						if (miRecordingChannels == 1) {
							// Mono recording
							tint32 iSamplesLeft = iSamples;
							while ((bSuccess) && (iSamplesLeft > 0)) {
								tfloat32 pSrc[32];
								tint32 iBytesRead = (tint32)pFileSrc->Read((tchar*)pSrc, 32 * sizeof(tfloat32));
								tint32 iSamplesRead = iBytesRead / sizeof(tfloat32);
								if (iSkipExtraLatencySamples > 0) {
									if (iSamplesRead <= iSkipExtraLatencySamples) {
										// Latency - eat them all
										iSkipExtraLatencySamples -= iSamplesRead;
										iSamplesRead = 0;
									}
									else {
										// Partial latency - eat some
										iSamplesRead -= iSkipExtraLatencySamples;
										iBytesRead = iSamplesRead * sizeof(tfloat32);
										tfloat32* pfAfterLatency = pSrc + iSkipExtraLatencySamples;
										iSkipExtraLatencySamples = 0;
										if (!pEncoder->ProcessRaw((tchar*)pfAfterLatency, NULL, iBytesRead, &iAccumOverflows)) {
											bSuccess = false;
										}
									}
								}
								else {
									if (!pEncoder->ProcessRaw((tchar*)pSrc, NULL, iBytesRead, &iAccumOverflows)) {
										bSuccess = false;
									}
								}

								iSamplesLeft -= 32;
							}
						}
						else {
							// Stereo recording (maybe it gets down-mixed to mono)
							tint32 iSamplesLeft = iSamples;
							while ((bSuccess) && (iSamplesLeft > 0)) {
								tfloat32 pSrc[2 * 32];
								tint32 iBytesRead = (tint32)pFileSrc->Read((tchar*)pSrc, 2 * 32 * sizeof(tfloat32));
								iBytesRead /= 2;
								tint32 iSamplesRead = iBytesRead / sizeof(tfloat32);
								tfloat32* pSrcR = pSrc + iSamplesRead;

								if (iSkipExtraLatencySamples > 0) {
									if (iSamplesRead <= iSkipExtraLatencySamples) {
										// Latency - eat them all
										iSkipExtraLatencySamples -= iSamplesRead;
										iSamplesRead = 0;
									}
									else {
										// Partial latency - eat some
										iSamplesRead -= iSkipExtraLatencySamples;
										iBytesRead = iSamplesRead * sizeof(tfloat32);
										tfloat32* pfAfterLatencyL = pSrc + iSkipExtraLatencySamples;
										tfloat32* pfAfterLatencyR = pSrcR + iSkipExtraLatencySamples;
										iSkipExtraLatencySamples = 0;
										if (!pEncoder->ProcessRaw((tchar*)pfAfterLatencyL, (tchar*)pfAfterLatencyR, iBytesRead, &iAccumOverflows)) {
											bSuccess = false;
										}
									}
								}
								else {
									if (!pEncoder->ProcessRaw((tchar*)pSrc, (tchar*)pSrcR, iBytesRead, &iAccumOverflows)) {
										bSuccess = false;
									}
								}

								iSamplesLeft -= 32;
							}
						}
					}

					if (!bSuccess) {
						pEncoder->GetErrMsg(pszErrMsg, 1024);
					}
				}
			}
		}

		IFile::DeleteFile((msRecordingName + std::string(".tmp")).c_str());

		if (!bSuccess) {
			gpApplication->ShowMessageBox_NonModal(pszErrMsg, "Error recording");
		}
		else if (!bAbortOperation) {
			gpApplication->QueueAudioFileImport(msRecordingName.c_str(), true, miTrack, miRecordingSongPos);
		}

		/* must wait for queued operation to complete before doing these operations

		IFile::DeleteFile(msRecordingName.c_str());

		std::string sName = msRecordingName.substr(0, msRecordingName.size() - 4);
		tint iPos = sName.rfind(':');
		sName = sName.substr(iPos + 1);
		mpDSP->CreateRegion(sName, miTrack, miRecordingSongPos, 0);
		*/
	}
} // Stop


CRegion_DSP* CTrack_DSP::CreateRegion(tint32 iUniqueID, 
										const std::string& sSample_Name, 
										tuint64 uiTrack_Pos, 
										tuint64 uiSample_Offset, 
										tuint64& ruiSample_Duration,
										tuint64 uiFadeInLength,
										tuint64	uiFadeOutLength,
										tfloat fRegionVolume)
{
	gpApplication->Stop_Timer();
	
	
	CSample_Data* pSample_Data	= gpDSPEngine->Get_Sample_Data_From_Name(sSample_Name.c_str());
	CTake_Data* pTake_Data		= pSample_Data->Get_Take_Data();
	
	if(pSample_Data == NULL) return NULL;
	
	std::string sWavePathNameL;
	std::string sWavePathNameR;
	gpDSPEngine->Set_Wave_Path( pTake_Data, 
							   sSample_Name.c_str(),  
							   sWavePathNameL, 
							   sWavePathNameR);

	//! TODO: 1) If iWaveFiles == 2 it's a stereo list item; sWavePathR will be valid, and channel should run in stereo mode
	CRegion_DSP* pRegion	=  new CRegion_DSP(iUniqueID, 
											  // sWavePathNameL, 
											  // sWavePathNameR, 
											  // sSample_Name, 
											   pSample_Data,
											   pTake_Data,
											   uiSample_Offset, 
											   ruiSample_Duration);
//	pRegion->Check_Region_UUID();
	
	if(ruiSample_Duration == (tuint64)-1)
		ruiSample_Duration = pRegion->Get_Sample_Duration();
		
	pRegion->Set_Fade_In_Duration(uiFadeInLength);
	pRegion->Set_Fade_Out_Duration(uiFadeOutLength);
	pRegion->Set_Volume(fRegionVolume);
		
	Delete_Selection(giTrim,uiTrack_Pos, ruiSample_Duration);
	
	SChannelRegionInfo* pRegionInfo		=	new SChannelRegionInfo();
	pRegionInfo->pRegion				=	pRegion;
	pRegionInfo->uiTrack_Pos			=	uiTrack_Pos;
	pRegionInfo->uiRegionID				=	iUniqueID;
	
	Insert_Region_Info(pRegionInfo) ;
	Update_Regions_For_Playback();
	
	gpApplication->Start_Timer();
	return pRegion;
}

void CTrack_DSP::Insert_Region_Info( SChannelRegionInfo* pRegionInfo) 
{
	if(mRegionInfoList.empty()){
		mRegionInfoList.push_back(pRegionInfo);
		return;
	}
	std::list<SChannelRegionInfo*>::iterator it = mRegionInfoList.begin();
	for (; it != mRegionInfoList.end(); it++) {
		// if the start position for the next object it after this?
		if((*it)->uiTrack_Pos > pRegionInfo->uiTrack_Pos){
			// insert this one
			mRegionInfoList.insert(it, pRegionInfo);
			return;
		}
	}
	// else put it in the end
	mRegionInfoList.push_back(pRegionInfo);
}

void CTrack_DSP::GetChannelRegionInfo(tuint32 iRegion, SChannelRegionInfo info)
{

	
	std::list<SChannelRegionInfo*>::iterator it = mRegionInfoList.begin();
	for (; it != mRegionInfoList.end(); it++) {
		// if the start position for the next object it after this?
		if((*it)->uiRegionID == iRegion){
			info = *(*it);
			return;
		}
	}

}


void CTrack_DSP::Update_Regions_For_Playback()
{
	SetSongPosition(muiSongPos);
}

void CTrack_DSP::DeleteRegion(tint32 iID)
{
	std::list<SChannelRegionInfo*>::iterator it = mRegionInfoList.begin();
	for (; it != mRegionInfoList.end(); it++) {
		if ((*it)->pRegion->GetID() == iID) {
			// Save pointer
			CRegion_DSP* pRegionDSP = (*it)->pRegion;
			// Remove entry
			mRegionInfoList.erase(it);
			SetSongPosition(muiSongPos);
			// Relase pointer
			delete pRegionDSP;
			return;
		}
	}

	// We didn't find any. Should we throw an exception?
}

CRegion_DSP* CTrack_DSP::GetRegion_DSP(tuint32 uiID)
{
	std::list<SChannelRegionInfo*>::iterator it = mRegionInfoList.begin();
	for (; it != mRegionInfoList.end(); it++) {
		if ((*it)->pRegion->GetID() == uiID) {
			return (*it)->pRegion;
		}
	}

	// We didn't find any. Should we throw an exception?
	return NULL;
}

tuint64 CTrack_DSP::Get_Region_Pos(tuint32 uiRegionID)
{
	std::list<SChannelRegionInfo*>::iterator it = mRegionInfoList.begin();
	for (; it != mRegionInfoList.end(); it++) {
		if ((*it)->pRegion->GetID() == uiRegionID) {
			return (*it)->uiTrack_Pos;
		}
	}

	// We didn't find any. Should we throw an exception?
	return 0;

}

void CTrack_DSP::Resize_Region(tuint32 uiID, tuint64 iTrack_Pos, tuint64 iSample_Offset, tint64 iSample_Duration)
{
	
	std::list<SChannelRegionInfo*>::iterator it = mRegionInfoList.begin();
	for (; it != mRegionInfoList.end(); it++) {
		
		if ((*it)->pRegion->GetID() == uiID) {
		
			tuint64 uiFadeInLength	=	(*it)->pRegion->Get_Fade_In_Duration();
			tuint64 uiFadeOutLength	=	(*it)->pRegion->Get_Fade_Out_Duration();
			tuint64 uiOldDuration	=	(*it)->pRegion->Get_Duration();
			tuint64 uiNewDuration	=	iSample_Duration;
			//---------------------
			// Trim end
			if((*it)->uiTrack_Pos == iTrack_Pos){
			
				tint64 iChange			=	uiOldDuration - uiNewDuration;
				// Fade out
				if(uiFadeOutLength < iChange)
					uiFadeOutLength		=	0;
				else
					uiFadeOutLength		-=	iChange;
				// Fade in
				if( uiFadeInLength > uiNewDuration)
					uiFadeInLength = uiNewDuration;
			}
			//---------------------
			// Trim start
			else{
				tint64 iChange	=	iTrack_Pos - (*it)->uiTrack_Pos;
				
				// Fade in
				if(uiFadeInLength < iChange)
					uiFadeInLength		=	0;
				else
					uiFadeInLength		-=	iChange;
				// Fade out
				if( uiFadeOutLength > uiNewDuration)
					uiFadeOutLength = uiNewDuration;
			
			}
			
			(*it)->pRegion->SetStartPos(iSample_Offset);
			(*it)->pRegion->SetDuration(uiNewDuration);
			(*it)->pRegion->Set_Fade_In_Duration(uiFadeInLength);
			(*it)->pRegion->Set_Fade_Out_Duration(uiFadeOutLength);
			(*it)->uiTrack_Pos = iTrack_Pos;
			Update_Regions_For_Playback();
			return;
		}
	}

	// We didn't find any. Should we throw an exception?
	return;

}


void CTrack_DSP::Set_Volume(tfloat32 fVolume)
{
	// Lasse, out-comment 2008-06-20 - guess we don't need this..
	//CAutoLock lock(mInProcessMutex);
	// .. Lasse
	
	mfVolumeSlider = fVolume;

	_CalcSetVolumeDezippers();
	_KillPeakVolume();
}

void CTrack_DSP::SetAUXVolume(tint32 iAUX, tfloat32 fVolume)
{
	mpfAUXVolume[iAUX] = fVolume;
}

void CTrack_DSP::ResetAllEffectsTails()
{
	tint32 iInsert;
	for (iInsert = 0; iInsert < giNumber_Of_Inserts; iInsert++) {
		if (mppInsert[iInsert] && mpbInsertBypass[iInsert] == false) {
			// Kill buffered data in effect
			mppInsert[iInsert]->Stop();
			mppInsert[iInsert]->Start();
		}
	}

	for (tint32 iAUX = 0; iAUX < 2; iAUX++) {
		mppAUXes[iAUX]->ResetAllEffectsTails();
	}
}

void CTrack_DSP::SetPanningLeftRight(tfloat32 fLeftRight)
{
	// Lasse, out-comment 2008-06-20 - guess we don't need this..
	//CAutoLock lock(mInProcessMutex);
	// .. Lasse
	
	tfloat32 fLeft		= fLeftRight;
	tfloat32 fRight		= 1.0f- fLeftRight;
	
	mfPanningLeft	=	1.0f - (fLeft  * fLeft);
	mfPanningRight	=	1.0f - (fRight * fRight);
	
	_CalcSetVolumeDezippers();
	_KillPeakVolume();
}

void CTrack_DSP::_KillPeakVolume()
{
	// Avoid "hanging" peak volumes when panning or sliding volume
	//memset(mafPeakVolumes, 0, 8 * sizeof(tfloat32));
	mbKillDecay = true;
}

 
void CTrack_DSP::SetPanningFrontBack(tfloat32 fFrontBack)
{
	// Lasse, out-comment 2008-06-20 - guess we don't need this..
	//CAutoLock lock(mInProcessMutex);
	// .. Lasse

//	mfPanningFrontBack = fFrontBack;

//	_CalcSetVolumeDezippers();
//	_KillPeakVolume();
}

void CTrack_DSP::_CalcSetVolumeDezippers()
{
	switch (miNumberOfChannelsForPanner) {
		case 1: {	// Mono - only apply output amp
			mapDezipperOutAmp[0]->SetTarget(mfVolumeSlider);
		}
		break;
		
		case 2: {	// Stereo - apply volume panning and output amp
		
			
			tfloat32 fLeft	= mfPanningLeft  * mfVolumeSlider;
			tfloat32 fRight = mfPanningRight * mfVolumeSlider;
			// Set targets
			mapDezipperOutAmp[0]->SetTarget(fLeft);
			mapDezipperOutAmp[1]->SetTarget(fRight);

		}
		break;
		/*
		case 4: {	// Quad - apply volume panning and output amp
			// Calc nearness for outputs
			tfloat32 faNearness[4];
			faNearness[0] = (1.0f - mfPanningLeftRight) * (mfPanningFrontBack + 1.0f) / 2.0f;
			faNearness[1] = (mfPanningLeftRight + 1.0f) * (mfPanningFrontBack + 1.0f) / 2.0f;
			faNearness[2] = (1.0f - mfPanningLeftRight) * (1.0f - mfPanningFrontBack) / 2.0f;
			faNearness[3] = (mfPanningLeftRight + 1.0f) * (1.0f - mfPanningFrontBack) / 2.0f;
			
			// Calc amplitude factors for output and set targets
			for (tint i=0; i<4; i++) {
				tfloat fAmp;
				tfloat fNearness = faNearness[i];
				if (fNearness < 0.0)
					fAmp = 0.0;
				else
					fAmp = (fNearness*fNearness) * mfVolumeSlider;
				mapDezipperOutAmp[i]->SetTarget(fAmp);
			}
		}
		break;
		*/
	}

	// Silence unused channels
	for (tint32 iSilentChannels = miNumberOfChannelsForPanner; iSilentChannels<8; iSilentChannels++) {
		// Silence these channels ...
		mapDezipperOutAmp[iSilentChannels]->SetTarget(0.0);
		// ... and effectuate immediately
		mapDezipperOutAmp[iSilentChannels]->Reset();				
	}	
}

void CTrack_DSP::SetInputChannelCount(tuint32 iChannels)
{
	// Lasse, mod 2008-01-16
	//miInputChannelCount = iChannels;

	// Lasse, out-comment 2008-06-20 - guess we don't need this..
	//CAutoLock lock(mInProcessMutex);
	// .. Lasse

	miInputChannelCount = iChannels;
	// .. Lasse

	_KillPeakVolume();
}

void CTrack_DSP::SetFirstInput(tuint32 uiFirstInput)
{
	// Lasse, out-comment 2008-06-20 - guess we don't need this..
	//CAutoLock lock(mInProcessMutex);
	// .. Lasse
	
	muiFirstInput = uiFirstInput;

	_KillPeakVolume();
}

void CTrack_DSP::SetTrackMode(tuint32 iChannels)
{
	
	// Lasse, out-comment 2008-06-20 - guess we don't need this..
	//CAutoLock lock(mInProcessMutex);
	// .. Lasse
	
	miModeChannelCount = iChannels;
	if (mbIsBusOrMix) {
		// This is a bus (or master mix), so we set input width along with mode width
		miInputChannelCount = iChannels;
	}
	
	_CalcSetNumberOfChannelsForPanner();
	_CalcSetVolumeDezippers();
	_KillPeakVolume();
}

void CTrack_DSP::_CalcSetNumberOfChannelsForPanner()
{
	tint32 iNew = ((tint32)miDestinationNumberOfChannels > (tint32)miModeChannelCount) ? miDestinationNumberOfChannels : miModeChannelCount;
	if (iNew == 0)
		iNew = 1;
	
	if (iNew != miNumberOfChannelsForPanner) {
		miNumberOfChannelsForPanner = iNew;
		
		if (mbMeterBeforeEffects)
			miNumberOfChannelsForMeter = miModeChannelCount;
		else
			miNumberOfChannelsForMeter = iNew;
	}
}

void CTrack_DSP::SetOutputDestination(tuint32 uiDest, tint32 iDestNumberOfChannels)
{
	if ((uiDest != miDestination) || (iDestNumberOfChannels != miDestinationNumberOfChannels)) {
		// Lasse, out-comment 2008-06-20 - guess we don't need this..
		//CAutoLock lock(mInProcessMutex);
		// .. Lasse
		
		miDestination = uiDest;
	
		miDestinationNumberOfChannels = iDestNumberOfChannels;
		_CalcSetNumberOfChannelsForPanner();
		_CalcSetVolumeDezippers();
		_KillPeakVolume();
	}
}

void CTrack_DSP::SetInsertBypass(tint32 iInsert, tbool bBypass)
{
	mpbInsertBypass[iInsert] = bBypass;
}

void CTrack_DSP::Delete_Selection(tint32 iCmd, tuint64 uiSelection_Pos, tuint64 uiSelection_Duration)
{

	tint32 iNewRegionID	= -1;
	miFade_Pos			= 0;

	// List of regions to delete
	std::list<tuint32> lRegionsToDelete;
	
	// Look at all regions on track
	std::list<SChannelRegionInfo*>::iterator it = mRegionInfoList.begin();
	for (; it != mRegionInfoList.end(); it++) {
		
		
		CRegion_DSP* pRegion_DSP			=	(*it)->pRegion;
		tint32 iRegionID					=	(*it)->uiRegionID;
		tuint64 uiSample_Duration			=	pRegion_DSP->Get_Duration();
		std::string sSample_Name				=	pRegion_DSP->Get_Sample_Name();
		tuint64 uiRegion_Pos				=	(*it)->uiTrack_Pos;
		tuint64 uiRegion_End				=	uiRegion_Pos + uiSample_Duration - 1;
		tuint64 uiSample_Offset				=	pRegion_DSP->Get_Sample_Offset();
		tuint64 uiSelection_End				=	uiSelection_Pos + uiSelection_Duration;
		tfloat32 fVolume					=	pRegion_DSP->Get_Volume();
		tuint uiFade_Out_Duration			=	pRegion_DSP->Get_Fade_Out_Duration();
		

		// Prevent feedback on new fade in regions
		//!!! BUG? has to come up with somthing better
		//!!! Make list of regions to add
		//!!! find start sample and end sample, use it for fading
		if(iNewRegionID != iRegionID){
			
			// delete region start
			if(uiSelection_Pos <= uiRegion_Pos && uiSelection_End < uiRegion_End && uiSelection_End > uiRegion_Pos){
				
				uiSelection_Duration	-=	(uiRegion_Pos - uiSelection_Pos)-1;
				Resize_Region(iRegionID, uiRegion_Pos + uiSelection_Duration, uiSample_Offset + uiSelection_Duration, uiSample_Duration - uiSelection_Duration);
				mpDSP->Refresh_Region_GUI(iRegionID, miTrack);
				
			}
			// delete region
			else if(uiSelection_Pos <= uiRegion_Pos && uiSelection_End >= uiRegion_End){
				
				lRegionsToDelete.insert(lRegionsToDelete.begin(), iRegionID);
			}
			// delete region end
			else if(uiSelection_Pos > uiRegion_Pos && uiSelection_End > uiRegion_End && uiRegion_End > uiSelection_Pos  ){
				
				uiSample_Duration	=	uiSelection_Pos - uiRegion_Pos;
				Resize_Region(iRegionID, uiRegion_Pos , uiSample_Offset , uiSample_Duration);
				mpDSP->Refresh_Region_GUI(iRegionID, miTrack);
			}
			// delete midt of region
			else if(uiSelection_Pos > uiRegion_Pos && uiSelection_End < uiRegion_End){
				
				//delete region end
				tuint64 uiNew_Sample_Duration	=	uiSelection_Pos - uiRegion_Pos;
				Resize_Region(iRegionID, uiRegion_Pos , uiSample_Offset , uiNew_Sample_Duration);
				mpDSP->Refresh_Region_GUI(iRegionID, miTrack);
				

				// create new region
				tuint64 uiDelta		=	uiNew_Sample_Duration + uiSelection_Duration;
				mpDSP->CreateRegion(sSample_Name, miTrack, uiRegion_Pos + uiDelta, uiSample_Offset + uiDelta , uiSample_Duration - uiDelta, 0,uiFade_Out_Duration, fVolume );
				
			}
			
		}
	}
	
	// Delete Regions on Channels and GUI and erase then from list to delete
	while (lRegionsToDelete.size()) {
		tint32 iRegionID = *(lRegionsToDelete.begin());
		// Delet region
		DeleteRegion(iRegionID);
		// Delete region on GUI
		mpDSP->Delete_Region_View(miTrack, iRegionID);
		// Delete region from list of regions
		lRegionsToDelete.erase(lRegionsToDelete.begin());
	}
	
}

