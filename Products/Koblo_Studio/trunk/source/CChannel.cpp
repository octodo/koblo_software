#include "KSOS.h"

CChannel::CChannel(CKSDSP* pDSP, tint32 iChannelNumber, tbool bIsBusOrMix, CChannel** ppAUXes)
	: mpDSPTools(NULL), mpDSP(pDSP), miChannelNumber(iChannelNumber), // mpDezipperVolume(0),
	miDestinationNumberOfChannels(2), miNumberOfChannelsForPanner(2),
	mfPanningLeft(0.71),mfPanningRight(0.71), mfPanningFrontBack(0.0),
	mbIsBusOrMix(bIsBusOrMix), miModeChannelCount(0), miInputChannelCount(2),
	mppAUXes(ppAUXes), mfVolumeSlider(1.0),
	mbArmed(false), mpFileRecording(NULL)
{
	mpbInsertBypass[0] = mpbInsertBypass[1] = mpbInsertBypass[2] = mpbInsertBypass[3] = false;

	if (mbIsBusOrMix) {
		miChannelNumber += 1024;
	}

	mpBuffer = new CBuffer();
	
	mpPlugIn = dynamic_cast<CKSPlugIn*>(mpDSP->GetPlugIn());

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
	// .. Lasse
}

CChannel::~CChannel()
{
	// Lasse, out-comment 2008-06-20 - guess we don't need this..
	//CAutoLock lock(mInProcessMutex);
	// .. Lasse
	
	delete mpBuffer;
}

void CChannel::Initialize()
{
	mpDSPTools = st::IDSPTools::Create();

	//mpDezipperVolume = new CBaseDezipper();
	//mpDezipperVolume->AddParameter(0, 0, 4, 0);
	for (tint i=0; i<8; i++) {
		mapDezipperOutAmp[i] = new CBaseDezipper2(0.0, 4.0, 1.0);
		mapDezipperOutAmp[i]->SetTarget(1.0);
	}
	
	mpDezipperPreAmp = new CBaseDezipper2(0.0, 4.0, 1.0);
	mpDezipperPreAmp->SetTarget(1.0);
	
	_CalcSetVolumeDezippers();
	_KillPeakVolume();
}

void CChannel::DeInitialize()
{
	//mpDezipperVolume->Destroy();
	for (tint i=0; i<8; i++)
		mapDezipperOutAmp[i]->Destroy();
	
	mpDezipperPreAmp->Destroy();

	mpDSPTools->Destroy();
	mpDSPTools = NULL;
}

void CChannel::SetSongPosition(tuint64 uiPosNew)
{
	// (mo) Note: Currently the sound position is set, which will flush its buffer, leading to decreased performance each time this method is called

 	muiSongPos = uiPosNew;

	// Locate next region to play
	mitRegionsInfo = mRegionInfoList.begin();
	while (mitRegionsInfo != mRegionInfoList.end()) {
	
		tint64 uiTrackPosStart	=	(*mitRegionsInfo)->uiTrackPosStart;
		
		if (uiTrackPosStart > muiSongPos) {
			// Next region is later than position
			// Reset next region position
			(*mitRegionsInfo)->pSoundObject->SetPos(0);
			break;
		}

		CSoundObject* pRegion = (*mitRegionsInfo)->pSoundObject;
		tint64 iLength = pRegion->GetDuration();
		if (uiTrackPosStart + iLength - 1 < uiPosNew) {
			// Position is beyond end of region
			mitRegionsInfo++;
		}
		else {
			// Position is "within" region

			// Calculate the sound position and set it
			tuint64 uiTrackPosOffset = uiPosNew - uiTrackPosStart;
			//(*mitRegions)->pSoundObject->SetSampleOffSet((tuint32)uiTrackPosOffset); SetStartPos
			// Error sample offset is the clipping og the region
			(*mitRegionsInfo)->pSoundObject->SetPos((tuint32)uiTrackPosOffset);
			break;
		}
	}
}

void CChannel::AddInsert(tint32 iInsert, tuint32 uiCompanyID, tuint32 uiProductID, tuint32 uiProductID2)
{
	CPlugInManager* pPlugManager = dynamic_cast<CKSPlugIn*>(mpDSP->GetPlugIn())->GetPlugInManager();
	
	if (mppInsert[iInsert]) {
		pPlugManager->UnloadPlugIn(mhInserts[iInsert], miChannelNumber, iInsert);
	}
	
	if (uiCompanyID == 0 && uiProductID == 0) {
		mppInsert[iInsert] = NULL;
	}
	else {
		// Stop updating of meters
		mpDSP->GetPlugIn()->All_Plugins_Created(false);
		mhInserts[iInsert] = pPlugManager->LoadPlugIn(uiCompanyID, uiProductID, miChannelNumber, iInsert);
		
		if (mhInserts[iInsert] == CPlugInManager::mInvalidHandleValue) {
			mhInserts[iInsert] = pPlugManager->LoadPlugIn(2, uiProductID2, miChannelNumber, iInsert);
		}
		if (mhInserts[iInsert] == CPlugInManager::mInvalidHandleValue) {
			mppInsert[iInsert] = NULL;
		}
		else {
			mppInsert[iInsert] = pPlugManager->GetPlugInFromHandle(mhInserts[iInsert]);
		}
		// Restart meter updat
		mpDSP->GetPlugIn()->All_Plugins_Created(true);
	}
}

void CChannel::Process(tint32 iSamples)
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
		CSoundObject* pRegion = (*mitRegionsInfo)->pSoundObject;
		tuint64 uiPosStart = (*mitRegionsInfo)->uiTrackPosStart;
		tuint64 uiPosEnd = uiPosStart + pRegion->GetDuration() - 1;

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
					(*mitRegionsInfo)->pSoundObject->SetPos(0);

					if ((*mitRegionsInfo)->pSoundObject->GetChannels() > iChannels) {
						iChannels = (*mitRegionsInfo)->pSoundObject->GetChannels();
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
			SetChannelMode(miRecordingChannels);

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


void CChannel::Start()
{
	tint32 iInsert;
	for (iInsert = 0; iInsert < giNumber_Of_Inserts; iInsert++) {
		if (mppInsert[iInsert]) {
			mppInsert[iInsert]->Start();
		}
	}

	ASSERT(mpFileRecording == NULL);

	if (mbArmed && mpPlugIn->IsRecording()) {
		std::string sChannelName = mpPlugIn->GetChannelName(miChannelNumber);
		if (sChannelName.size() == 0) {
			tchar psz[32];
			sprintf(psz, "Track %d", miChannelNumber + 1);
			sChannelName = std::string(psz);
		}

		std::string sPathNameDest = mpPlugIn->GetProjDir_Audio();
		sPathNameDest += "Clips:";
		sPathNameDest += "Recording_";
		sPathNameDest += sChannelName;
		sPathNameDest += "_";

		std::string sPathName = mpPlugIn->GetProjDir_Audio();
		sPathName += "Recording_";
		sPathName += sChannelName;
		sPathName += "_";
		
		tint32 iIndex = 0;
		tbool bFoundIt = false;
		while (bFoundIt == false) {
			std::string s = sPathName;
			std::string sDest = sPathNameDest;
			std::string sLeft = sPathNameDest;
			tchar psz[32];
			sprintf(psz, "%d.wav", iIndex);
			tchar pszLeft[32];
			sprintf(pszLeft, "%d-1.wav", iIndex);
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

		miRecordingSongPos = mpPlugIn->GetSongPos();
	}
} // Start


void CChannel::Stop()
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
						tint64 iSkipExtraLatencySamples = mpPlugIn->AudioInput_IntermediateBuffer_CalcExtraLatency();
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
			mpPlugIn->ShowMessageBox_NonModal(pszErrMsg, "Error recording");
		}
		else if (!bAbortOperation) {
			mpPlugIn->QueueAudioFileImport(msRecordingName.c_str(), true, miChannelNumber, miRecordingSongPos);
		}

		/* must wait for queued operation to complete before doing these operations

		IFile::DeleteFile(msRecordingName.c_str());

		std::string sName = msRecordingName.substr(0, msRecordingName.size() - 4);
		tint iPos = sName.rfind(':');
		sName = sName.substr(iPos + 1);
		mpDSP->CreateRegion(sName, miChannelNumber, miRecordingSongPos, 0);
		*/
	}
} // Stop


CSoundObject* CChannel::CreateRegion(tint32 iUniqueID, 
										const std::string& sSoundListItemName, 
										tuint64 uiTrackPosStart, 
										tuint64 uiSoundPosStart, 
										tuint64& ruiSoundPosEnd,
										tuint64 uiFadeInLength,
										tuint64	uiFadeOutLength,
										tfloat fRegionVolume)
{
	std::string sWavePathNameL;
	std::string sWavePathNameR;
	tint32 iWaveFiles = mpPlugIn->GetFromListName_ClipWavePathNames(sSoundListItemName.c_str(), sWavePathNameL, sWavePathNameR);
	if (iWaveFiles <= 0) {
		// Internal error / missing waves: Exit gracefully instead of crashing
		return NULL;
	}
	//! TODO: 1) If iWaveFiles == 2 it's a stereo list item; sWavePathR will be valid, and channel should run in stereo mode
	CSoundObject* pRegionSoundObject	= new CSoundObject(iUniqueID, sWavePathNameL, sWavePathNameR, sSoundListItemName, uiSoundPosStart, ruiSoundPosEnd);
	if(ruiSoundPosEnd == (tuint64)-1)
		ruiSoundPosEnd = pRegionSoundObject->GetEndPos();
		
	pRegionSoundObject->SetFadeInLength(uiFadeInLength);
	pRegionSoundObject->SetFadeOutLength(uiFadeOutLength);
	pRegionSoundObject->SetRegionVolume(fRegionVolume);
		
	tuint64 uiDuration = ruiSoundPosEnd - uiSoundPosStart +1;
	Edit_Selection(giTrim,uiTrackPosStart, uiDuration);
	
	SChannelRegionInfo* pRegionInfo		=	new SChannelRegionInfo();
	pRegionInfo->pSoundObject			=	pRegionSoundObject;
	pRegionInfo->uiTrackPosStart		=	uiTrackPosStart;
	pRegionInfo->uiRegionID				=	iUniqueID;
	
	Insert_Region_Info(pRegionInfo) ;
	Update_Regions_For_Playback();
	return pRegionSoundObject;
}

tint64 CChannel::GetRegionSize(tint32 iUniqueID, const std::string& sSoundListItemName, tuint64 uiSoundPosStart, tint64 uiSoundPosEnd)
{
	std::string sWavePathNameL;
	std::string sWavePathNameR_NeverMind;
	tint32 iWaveFiles = mpPlugIn->GetFromListName_ClipWavePathNames(sSoundListItemName.c_str(), sWavePathNameL, sWavePathNameR_NeverMind);
	// If iWaveFiles == 2 then sWavePathR will be valid - but that doesn't matter for length calculations

	if (iWaveFiles == 0) {
		// Exit gracefully from internal error: size = 0
		return 0;
	}

	CSoundObject* pRegionSoundObject	= new CSoundObject(iUniqueID, sWavePathNameL, "", sSoundListItemName, uiSoundPosStart, uiSoundPosEnd);
	if(uiSoundPosEnd == (tuint64)-1)
		uiSoundPosEnd = pRegionSoundObject->GetEndPos();
	
	tint64 iSize = uiSoundPosEnd-uiSoundPosStart + 1;
	delete(pRegionSoundObject);
	return iSize;

	
}

void CChannel::Insert_Region_Info( SChannelRegionInfo* pRegionInfo) 
{
	if(mRegionInfoList.empty()){
		mRegionInfoList.push_back(pRegionInfo);
		return;
	}
	std::list<SChannelRegionInfo*>::iterator it = mRegionInfoList.begin();
	for (; it != mRegionInfoList.end(); it++) {
		// if the start position for the next object it after this?
		if((*it)->uiTrackPosStart > pRegionInfo->uiTrackPosStart){
			// insert this one
			mRegionInfoList.insert(it, pRegionInfo);
			return;
		}
	}
	// else put it in the end
	mRegionInfoList.push_back(pRegionInfo);
}

void CChannel::GetChannelRegionInfo(tuint32 iRegion, SChannelRegionInfo info)
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


void CChannel::Update_Regions_For_Playback()
{
	SetSongPosition(muiSongPos);
}

void CChannel::DeleteRegion(tint32 iID)
{
	std::list<SChannelRegionInfo*>::iterator it = mRegionInfoList.begin();
	for (; it != mRegionInfoList.end(); it++) {
		if ((*it)->pSoundObject->GetID() == iID) {
			mRegionInfoList.erase(it);
			SetSongPosition(muiSongPos);
			return;
		}
	}

	// We didn't find any. Should we throw an exception?
}

CSoundObject* CChannel::GetRegionSoundObject(tuint32 uiID)
{
	std::list<SChannelRegionInfo*>::iterator it = mRegionInfoList.begin();
	for (; it != mRegionInfoList.end(); it++) {
		if ((*it)->pSoundObject->GetID() == uiID) {
			return (*it)->pSoundObject;
		}
	}

	// We didn't find any. Should we throw an exception?
	return NULL;
}

tuint64 CChannel::GetRegionPosOnTrack(tuint32 uiRegionID)
{
	std::list<SChannelRegionInfo*>::iterator it = mRegionInfoList.begin();
	for (; it != mRegionInfoList.end(); it++) {
		if ((*it)->pSoundObject->GetID() == uiRegionID) {
			return (*it)->uiTrackPosStart;
		}
	}

	// We didn't find any. Should we throw an exception?
	return 0;

}

void CChannel::TrimRegion(tuint32 uiID, tuint64 iTrackStartPos, tuint64 iSoundStartPos, tint64 iSoundEndPos)
{

	std::list<SChannelRegionInfo*>::iterator it = mRegionInfoList.begin();
	for (; it != mRegionInfoList.end(); it++) {
		if ((*it)->pSoundObject->GetID() == uiID) {
		
			tuint64 uiFadeInLength	=	(*it)->pSoundObject->GetFadeInLength();
			tuint64 uiFadeOutLength	=	(*it)->pSoundObject->GetFadeOutLength();
			tuint64 uiOldDuration	=	(*it)->pSoundObject->GetDuration();
			tuint64 uiNewDuration	=	iSoundEndPos - iSoundStartPos + 1;
			//---------------------
			// Trim end
			if((*it)->uiTrackPosStart == iTrackStartPos){
			
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
				tint64 iChange	=	iTrackStartPos - (*it)->uiTrackPosStart;
				
				// Fade in
				if(uiFadeInLength < iChange)
					uiFadeInLength		=	0;
				else
					uiFadeInLength		-=	iChange;
				// Fade out
				if( uiFadeOutLength > uiNewDuration)
					uiFadeOutLength = uiNewDuration;
			
			}
		
			(*it)->pSoundObject->SetStartPos(iSoundStartPos);
			(*it)->pSoundObject->SetDuration(uiNewDuration);
			(*it)->pSoundObject->SetFadeInLength(uiFadeInLength);
			(*it)->pSoundObject->SetFadeOutLength(uiFadeOutLength);
			(*it)->uiTrackPosStart = iTrackStartPos;
			Update_Regions_For_Playback();
			return;
		}
	}

	// We didn't find any. Should we throw an exception?
	return;

}

void CChannel::SetVolume(tfloat32 fVolume)
{
	// Lasse, out-comment 2008-06-20 - guess we don't need this..
	//CAutoLock lock(mInProcessMutex);
	// .. Lasse
	
	mfVolumeSlider = fVolume;

	_CalcSetVolumeDezippers();
	_KillPeakVolume();
}

void CChannel::SetAUXVolume(tint32 iAUX, tfloat32 fVolume)
{
	mpfAUXVolume[iAUX] = fVolume;
}

void CChannel::ResetAllEffectsTails()
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

void CChannel::SetPanningLeftRight(tfloat32 fLeftRight)
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

void CChannel::_KillPeakVolume()
{
	// Avoid "hanging" peak volumes when panning or sliding volume
	//memset(mafPeakVolumes, 0, 8 * sizeof(tfloat32));
	mbKillDecay = true;
}

 
void CChannel::SetPanningFrontBack(tfloat32 fFrontBack)
{
	// Lasse, out-comment 2008-06-20 - guess we don't need this..
	//CAutoLock lock(mInProcessMutex);
	// .. Lasse

//	mfPanningFrontBack = fFrontBack;

//	_CalcSetVolumeDezippers();
//	_KillPeakVolume();
}

void CChannel::_CalcSetVolumeDezippers()
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

void CChannel::SetInputChannelCount(tuint32 iChannels)
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

void CChannel::SetFirstInput(tuint32 uiFirstInput)
{
	// Lasse, out-comment 2008-06-20 - guess we don't need this..
	//CAutoLock lock(mInProcessMutex);
	// .. Lasse
	
	muiFirstInput = uiFirstInput;

	_KillPeakVolume();
}

void CChannel::SetChannelMode(tuint32 iChannels)
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

void CChannel::_CalcSetNumberOfChannelsForPanner()
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

void CChannel::SetOutputDestination(tuint32 uiDest, tint32 iDestNumberOfChannels)
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

void CChannel::SetInsertBypass(tint32 iInsert, tbool bBypass)
{
	mpbInsertBypass[iInsert] = bBypass;
}

void CAUXReverb::Process(tint32 iSamples)
{

	 mpBuffer->SetChannels(2);
	 
	 tfloat32* pfLeft = mpBuffer->GetData(0);
	 tfloat32* pfRight = mpBuffer->GetData(1);
	 
	 // Reverb code goes here. pfLeft and pfRight buffers can be modified. Buffer length is in iSamples
	 float* pfStereo[2];
	 pfStereo[0] = pfLeft;
	 pfStereo[1] = pfRight;
	 
	 KobloVerb.processReplace(pfStereo, pfStereo, iSamples);
	 
	 // Maybe do down or up mix for destination
	 mpBuffer->SetChannels(miDestinationNumberOfChannels);
	 muiSongPos += iSamples;
	
}

void CAUXReverb::Initialize()
{
}

void CAUXReverb::DeInitialize()
{
}

void CAUXReverb::ResetAllEffectsTails()
{
	Stop();
	Start();
}

void CAUXReverb::Start()
{
}

void CAUXReverb::Stop()
{
	KobloVerb.mute();
	KobloVerb.flush();
}

void CAUXReverb:: Set_Reverb_On(tint32 iData)
{
	KobloVerb.setReverbOn(iData ? true: false);
}

void CAUXReverb:: Set_Reverb_Size(tint32 iData)
{
	KobloVerb.setRoomSize(iData / 128.0f);
}

void CAUXReverb:: Set_Reverb_PDelay(tint32 iData)
{
//	miPDelay = iData;
	KobloVerb.setPredelay(iData);
}

void CAUXReverb:: Set_Reverb_Damp(tint32 iData)
{
//	miDamp = iData;
	KobloVerb.setDamp(iData / 128.0f);
}

void CAUXReverb:: Set_Reverb_Panorama(tint32 iData)
{
//	miPanorama = iData;
	// notImpl
}

void CAUXReverb:: Set_Reverb_Color1(tint32 iData)
{
//	miColor1 = iData;
	KobloVerb.setHP(iData / 600.0f);
}

void CAUXReverb:: Set_Reverb_Color2(tint32 iData)
{

	KobloVerb.setLP(iData / 600.0f);
}

void CAUXReverb:: Set_Reverb_Mix(tint32 iData)
{
	miMix = iData;
	
	//KobloVerb.setWet(miWet / 128.0f);
	//KobloVerb.setDry(miDry / 128.0f);
}


void CAUXEcho::Process(tint32 iSamples)
{
	mpBuffer->SetChannelsNoConvert(2);
	
	if(miPower){

		tfloat32* pfL = mpBuffer->GetData(0);
		tfloat32* pfR = mpBuffer->GetData(1);

		// Start of echo
		tfloat* pfBuffer1 = mpfBuffer1;
		tfloat* pfBuffer2 = mpfBuffer2;
		tint32 iPos = miPos;

		for (tuint uiSample = 0; (tint)uiSample < iSamples; uiSample++) {
			// Move writing position forward
			iPos++;
			// Wrap (if neccasary)
			iPos &= (EFFECT_ECHO_BUFFER_SIZE - 1);

			// Find reading tap
			tint32 iTap = iPos - miDelayTime;
			iTap &= (EFFECT_ECHO_BUFFER_SIZE - 1);

			// Read from buffer
			tfloat f1 = pfBuffer1[iTap];
			tfloat f2 = pfBuffer2[iTap];

			// Apply feedback
			f1 *= mfFeedback;
			f2 *= mfFeedback;

			// Apply LP
			f1 = mpfLPHistory[0] = gfAnti_denormal + mpfLPHistory[0] + (f1 - mpfLPHistory[0]) * mfDamping;
			f2 = mpfLPHistory[1] = gfAnti_denormal + mpfLPHistory[1] + (f2 - mpfLPHistory[1]) * mfDamping;

			// Apply HP
			mpfHPHistory[0] = gfAnti_denormal + mpfHPHistory[0] + (f1 - mpfHPHistory[0]) * mfHPDamping;
			f1 = f1 - mpfHPHistory[0];
			mpfHPHistory[1] = gfAnti_denormal + mpfHPHistory[1] + (f2 - mpfHPHistory[1]) * mfHPDamping;
			f2 = f2 - mpfHPHistory[1];

			// Write to circular buffer
			pfBuffer1[iPos] = f1 + pfL[uiSample];
			pfBuffer2[iPos] = f2 + pfR[uiSample];

			// Write output
			pfL[uiSample] = f1;
			pfR[uiSample] = f2;
			
			miPos = iPos;
			// End of echo
			muiSongPos += iSamples;
		}
	}
	else{
		for (tuint uiSample = 0; (tint)uiSample < iSamples; uiSample++) {
			tfloat32* pfL = mpBuffer->GetData(0);
			tfloat32* pfR = mpBuffer->GetData(1);
			// Write output
			pfL[uiSample] = 0.0f;
			pfR[uiSample] = 0.0f;
		}
		// End of echo
		muiSongPos += iSamples;
	
	}

	
}

void CAUXEcho::Initialize()
{
 	mpfBuffer1 = new tfloat[EFFECT_ECHO_BUFFER_SIZE];
	mpfBuffer2 = new tfloat[EFFECT_ECHO_BUFFER_SIZE];
	
	miDelayTime		=	22500;
	mfFeedback		=	0.64f;
	
	mfDamping		=	0.13605442177f; //!!! Change this
	mfHPDamping		=	0.18140589569f; //!!! Change this
	
	Start();
}

void CAUXEcho::DeInitialize()
{
	delete[] mpfBuffer1;
	mpfBuffer1 = NULL;

	delete[] mpfBuffer2;
	mpfBuffer2 = NULL;
}

void CAUXEcho::ResetAllEffectsTails()
{
	Stop();
	Start();
}

void CAUXEcho::Start()
{
	memset(mpfBuffer1, 0, EFFECT_ECHO_BUFFER_SIZE * sizeof(tfloat));
	memset(mpfBuffer2, 0, EFFECT_ECHO_BUFFER_SIZE * sizeof(tfloat));

	miPos = 0;

	mpfLPHistory[0] = mpfLPHistory[1] = 0;
	mpfHPHistory[0] = mpfHPHistory[1] = 0;
}

void CAUXEcho::Stop()
{
}

void CAUXEcho::Set_Delay_Time(tint32 iMSec)
{
	miMSec = iMSec;
	
	Set_Delay_in_Samples();
	
//	miInterFaceDelayTime = iValue * GetSampleRate() / 1000;
//	miDelayTime = mbSyncToHost ? miHostDelayTime: miInterFaceDelayTime;
}

void CAUXEcho::Set_Delay_Power(tbool bPower)
{
	miPower = bPower;
	if(!bPower){
	// Reset buffers
		Start();
	}
}

void CAUXEcho:: Set_Feedback(tint32 iFeedback)
{
	mfFeedback = iFeedback * 0.008f;
}

void CAUXEcho::Set_LF_Damping(tint32 iLF_Damping)
{
	if (iLF_Damping == 16001) {
		mfDamping = 1;
	}
	else {
		mfDamping = (float)iLF_Damping / (44100 / 2);
	}
}

void CAUXEcho::Set_HP_Damping(tint32 iHP_Damping)
{
	if (iHP_Damping == 31) {
		mfHPDamping = 0;
	}
	else {
		mfHPDamping = (float)iHP_Damping / (44100 / 2);
	}
}

void CAUXEcho::Set_Sync_To_Tempo(tbool bSync)
{
	mbSyncToHost = bSync;
	
//	Set_Delay_in_Samples();
}

void CAUXEcho::Set_Delay_in_Samples()
{
	tint32 iSampleRate = mpDSP->GetPlugIn()->GetSampleRate();
	miDelayTime = miMSec * iSampleRate / 1000;
}

void CChannel::Edit_Selection(tint32 iCmd, tuint64 uiSelection_Pos, tuint64 uiSelection_Duration)
{

	tint32 iNewRegionID	= -1;
	miFade_Pos			= 0;

	// List of regions to delete
	std::list<tuint32> lRegionsToDelete;
	
	// Look at all regions on track
	std::list<SChannelRegionInfo*>::iterator it = mRegionInfoList.begin();
	for (; it != mRegionInfoList.end(); it++) {
		tint32 iRegionID					=	(*it)->uiRegionID;
		tuint64 uiRegionDuration			=	(*it)->pSoundObject->GetDuration();
		std::string sClipName				=	(*it)->pSoundObject->GetSoundListItemName();
		tuint64 uiRegionStart				=	(*it)->uiTrackPosStart;
		tuint64 uiRegionEnd					=	uiRegionStart + uiRegionDuration - 1;
		tuint64 uiSoundStart				=	(*it)->pSoundObject->GetSoundStartPos();
		tuint64 uiSoundEnd					=	(*it)->pSoundObject->GetEndPos();
		tuint64 uiSelectionStart			=	uiSelection_Pos;
		tuint64 uiSelectionEnd				=	uiSelectionStart + uiSelection_Duration;
		
		// Prevent feedback on new fade in regions
		//!!! BUG has to come up with somthing better
		//!!! Make list of regions to add
		//!!! find start sample and end sample, use it for fading
		if(iNewRegionID != iRegionID){
		// Make sure the region is inside the selection
			if(uiSelectionEnd >= uiRegionStart && uiSelectionStart <= uiRegionEnd) 
			{
				//-----------------------------------------------------------
				// Selection start on region
				if(uiSelectionStart <= uiRegionStart)
					uiSelectionStart		=	0;
				else
					uiSelectionStart		=	uiSelectionStart - uiRegionStart;
				// Selection end on region	
				if( uiSelectionEnd > uiRegionEnd)
					uiSelectionEnd		=	uiRegionDuration;
				else
					uiSelectionEnd		=	uiSelectionEnd - uiRegionStart;
					
				tuint64 uiSelectionDuration	=	uiSelectionEnd - uiSelectionStart + 1;

				//-----------------------------------------------------------	
				// Select entire region
				if( uiSelectionStart == 0 ) {
					// If the entire region is inside the selection
					if( uiRegionDuration <= uiSelectionDuration){
						
						// CREATE FADE REGION
						iNewRegionID = Create_Fade_Region(	iCmd,
															sClipName, 
															uiRegionStart, 
															uiSoundStart, 
															uiSoundEnd,
															uiSelection_Duration);
						
						lRegionsToDelete.insert(lRegionsToDelete.begin(), iRegionID);
						
					}
				//-------------------------------------------------------
				// Select beginning of region
					else if(uiSelectionEnd < uiRegionEnd ){
						
						// DELETE BEGINNING OF REGION
						TrimRegion(iRegionID, uiRegionStart + uiSelectionDuration, uiSoundStart + uiSelectionDuration, uiSoundEnd);
						
						// CREATE FADE  REGION
						uiSoundEnd = uiSoundStart + uiSelectionDuration - 2;
						iNewRegionID = Create_Fade_Region(	iCmd,
															sClipName, 
															uiRegionStart, 
															uiSoundStart, 
															uiSoundEnd,
															uiSelection_Duration);		
											
						// Update GUI
						mpDSP->Refresh_Region_GUI(iRegionID, miChannelNumber);
					}
				}
				//-----------------------------------------------------------
				// Select end of region
				else if(uiSelectionStart <= uiRegionEnd){
					
					if(uiSelectionEnd == uiRegionDuration){
						// DELETE END OF REGION
						TrimRegion(iRegionID, uiRegionStart, uiSoundStart, uiSoundEnd - uiSelectionDuration);
						
						// CREATE FADE IN REGION
						uiRegionStart	+= uiSelectionStart;
						uiSoundStart	+= uiSelectionStart;
						iNewRegionID = Create_Fade_Region(	iCmd,
															sClipName, 
															uiRegionStart, 
															uiSoundStart, 
															uiSoundEnd,
															uiSelection_Duration);	
						
						// Update GUI
						mpDSP->Refresh_Region_GUI(iRegionID, miChannelNumber);
					}
				//-----------------------------------------------------------
				// Select midt of region
					else{
						// Trim end of region
						
						tuint64 uiSStart		=	uiSoundStart;
						tuint64 uiSEnd			=	uiSStart + uiSelectionStart - 2;
						tuint64 uiSDuration		=	uiSEnd - uiSStart + 1;
						tuint64 uiRStart		=	uiRegionStart;
						tuint64 uiREnd			=	uiRStart + uiSDuration;
						TrimRegion(iRegionID, uiRStart, uiSStart, uiSEnd );
						
						
						// CREATE FADE IN REGION
						uiRStart		=	uiREnd+1;
						uiSStart		+=	uiSDuration + 1;
						uiSEnd			=	uiSStart + uiSelectionDuration - 2;
						uiSDuration		=	uiSEnd - uiSStart + 1;
						uiREnd			=	uiRStart + uiSDuration;
						iNewRegionID = Create_Fade_Region(	iCmd,
															sClipName, 
															uiRStart, 
															uiSStart, 
															uiSEnd,
															uiSelection_Duration);
																
						uiRStart		=	uiREnd + 1;
						uiSStart		+=	uiSDuration + 1;
						uiSEnd			=	uiSStart + uiSelectionDuration - 2;
						
						// Create a new region
						mpDSP->CreateRegion(	(*it)->pSoundObject->GetSoundListItemName(),
												miChannelNumber, 
												uiRStart , 
												uiSStart, 
												uiSoundEnd);
						
						// Update GUI
						mpDSP->Refresh_Region_GUI(iRegionID, miChannelNumber);
						// No other regions can be selected and no regions will be deleted
						return;
					}
				}
			}
		}
	}
	
	// Delete Regions on Channels and GUI and erase then from list to delete
	while (lRegionsToDelete.size()) {
		tint32 iRegionID = *(lRegionsToDelete.begin());
		// Delet region
		DeleteRegion(iRegionID);
		// Delete region on GUI
		mpDSP->DeleteRegionOnGUI(miChannelNumber, iRegionID);
		// Delete region from list of regions
		lRegionsToDelete.erase(lRegionsToDelete.begin());
	}
	
}

tint32 CChannel::Create_Fade_Region(	tint32 iCmd,
										const std::string& sSoundClipName, 
										tuint64 uiTrack_Pos, 
										tuint64 uiSample_Start, 
										tuint64 uiSamplePosEnd,
										tuint64 uiSelection_Duration)
{
	//!!! max is this used at all""

	tint32 iTest =0;
	switch(iCmd){
		case giTrim:
			iTest++;
			return -1;
		case giFade_In:
			iTest++;
			break;
		case giFade_Out:
			iTest++;
			break;
	}
	
	
	
	// Render fade file
	tfloat64 fDuration		=	uiSamplePosEnd - uiSample_Start + 1;
	tfloat32 fStep_Size		=	1.0f / fDuration;

/*	
	for(tint64 i = miFade_Pos; i< uiSelection_Duration; i++)
	{
		
	
	}
*/
	//------------------------------------------------------
	// create new fade file on disk based on function param's
	// sSoundPathName = "Name of fade file"
	
	// insert the created fade region on the track, 
	// for now I use the region without any rendering
	tint32 iRegionID = mpDSP->CreateRegion(sSoundClipName, miChannelNumber, uiTrack_Pos, uiSample_Start, uiSamplePosEnd);
	return iRegionID;
}
