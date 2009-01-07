
#include "KSOS.h"

st::IStreamManager* gpStreamManager = NULL;

/*
	CDSP
*/

CDSP::CDSP()
	: muiChannels(0), muiFinalSongPosition(0),
	mRegionSearchEnd(0)
{
	gpStreamManager = mpStreamManager = st::IStreamManager::Create();
	mpStreamManager->CreateStreams(giNrOfStreams);

/*	tint32 iStream;
	for (iStream = 0; iStream < giNrOfStreams; iStream++) {
		mppStreams[iStream] = NULL;
	}*/

	mppAUXes = new CTrack_DSP*[giNumber_Of_AUXes];
	mppAUXes[0] = new CRack_Echo_DSP(this, 0, true);
	mppAUXes[0]->Initialize();
	mppAUXes[1] = new CRack_Reverb_DSP(this, 1, true);
	mppAUXes[1]->Initialize();

	mppTracks = new CTrack_DSP*[giNumber_Of_Tracks];
	tint32 iChannel;
	for (iChannel = 0; iChannel < giNumber_Of_Tracks; iChannel++) {
		mppTracks[iChannel] = new CTrack_DSP(this, iChannel, false, mppAUXes);
		mppTracks[iChannel]->Initialize();
		mppTracks[iChannel]->SetSongPosition(0);

		mpTrackSelectionInfo[iChannel].uiSelection_Type		=	giSelect_Off;
		mpTrackSelectionInfo[iChannel].iRegionID	=	-1;
		mbMute_Track_Flag[iChannel]					= false;
	}

	mppBusses = new CTrack_DSP*[giNumber_Of_Busses];
	tint32 iBus;
	for (iBus = 0; iBus < giNumber_Of_Busses; iBus++) {
		mppBusses[iBus] = new CTrack_DSP(this, iBus, true, mppAUXes);
		mppBusses[iBus]->Initialize();
	}

	mpMaster = new CTrack_DSP(this, 1024, true, mppAUXes);
	mpMaster->Initialize();

	tint32 i;
/*	for (i = 0; i < giNumber_Of_Tracks; i++) {
		mbMute_Track_Flag[i] = false;
		
	}
*/
	for (i = 0; i <  giNumber_Of_Busses; i++) {
		mbMute_Buss_Flag[i] = false;
	}
	for (i = 0; i < giNumber_Of_AUXes; i++) {
		mbMute_AUX_Flag[i] = false;
	}

//	mpBufferMix = new CBuffer();

	// Lasse, 2008-0515
	mpAudio_Duplex = NULL;
	mpAudioCallback_Duplex = NULL;
	mpAudio_Input = NULL;
	mpAudioCallback_Input = NULL;
	mpAudio_Output = NULL;
	mpAudioCallback_Output = NULL;
	for (tint i = 0; i < NUMBER_OF_AUDIO_MANAGERS; i++) {
		mapAudioManager[i] = NULL;
	}

#ifdef _Mac
	mapAudioManager[0] = aioe::IDeviceManager::CreateCoreAudio();
#endif	// _Mac
#ifdef WIN32
	mapAudioManager[0] = aioe::IDeviceManager::CreateASIO();
	mapAudioManager[1] = aioe::IDeviceManager::CreateWaveIO();
#endif	// WIN32

	LoadPrefs();
	// .. Lasse
	
//	miSelected_Region	= -1;
//	miSelection			= giSelect_Off;

	// Max 8 meters for all tracks/busses/AUXes (7.1 surround)
	mpsMeters_All = new SMeters_All();
	mpsMeters_All->Zero();
	
	miSelectedTrack	=	-1;
	muiSession_End_Sample = 4000;

	miStutterCounter = 0;
} // constructor


CDSP::~CDSP()
{
//	delete mpBufferMix;

	tint32 iAUX;
	for (iAUX = 0; iAUX < giNumber_Of_AUXes; iAUX++) {
		mppAUXes[iAUX]->DeInitialize();
		delete mppAUXes[iAUX];
	}
	delete[] mppAUXes;

	tint32 iBus;
	for (iBus = 0; iBus < giNumber_Of_Busses; iBus++) {
		mppBusses[iBus]->DeInitialize();
		delete mppBusses[iBus];
	}
	delete[] mppBusses;

	mpMaster->DeInitialize();
	delete mpMaster;

	tint32 iChannel;
	for (iChannel = 0; iChannel < giNumber_Of_Tracks; iChannel++) {
		mppTracks[iChannel]->DeInitialize();
		delete mppTracks[iChannel];
	}
	delete[] mppTracks;

/*	tint32 iStream;
	for (iStream = 0; iStream < giNrOfStreams; iStream++) {
		if (mppStreams[iStream] != NULL) {
			mpStreamManager->ReleaseStream(mppStreams[iStream]);
		}
	}*/

	mpStreamManager->Destroy();

	CloseAudioDevices();
	KillAudioManagers();

	// Delete arrays for meters
	delete mpsMeters_All;
	
	muiLoopStart		=	0;
	muiLoopEnd			=	-1;
	muiStored_LoopEnd	=	-1;
	
} // destructor


void CDSP::LoadPrefs()
{
	CKSPreferences::LoadAll(&mPrefs_1, &mPrefs_2);
} // LoadPrefs


void CDSP::SavePrefs()
{
	CKSPreferences::SaveAll(&mPrefs_1, &mPrefs_2);
} // SavePrefs


void CDSP::KillAudioManagers()
{
	for (tint i = 0; i < NUMBER_OF_AUDIO_MANAGERS; i++) {
		if (mapAudioManager[i]) {
			mapAudioManager[i]->Destroy();
			mapAudioManager[i] = NULL;
		}
	}
}

void CDSP::Destroy()
{
	delete dynamic_cast<CDSP*>(this);
}

/*void CDSP::Start()
{
}*/

/*void CDSP::Stop()
{
}*/

void CDSP::SetMaxBufferSize(tint32 uiMaxBufferSize)
{
	CBaseDSPEngine::SetMaxBufferSize(uiMaxBufferSize);
}

void CDSP::SetSampleRate(tuint32 uiSampleRate)
{
	CBaseDSPEngine::SetSampleRate(uiSampleRate);

/*	CWave_File* pWaveFile = new CWave_File();
	pWaveFile->LoadSoundStream(512, ":Macintosh HD:Track1.wav");
	mpStream = mpStreamManager->GetStream();
	IFile* pFile;
	tint32 iOffset;
	tint32 iLength;
	pWaveFile->GetStreamInfo(pFile, iOffset, iLength);
	mpStream->Reset(pFile, iOffset, iLength, false, NULL);*/
}

void CDSP::SetChannels(tuint32 uiChannels)
{
	muiChannels = uiChannels;
}

void CDSP::SetInputsForChannel(tuint32 uiChannel, tint32 iInCount, tint32 iInFirst)
{
	if ((tuint32)(iInFirst+iInCount) > muiChannels)
		return;
	
	CTrack_DSP* pTrack = mppTracks[uiChannel];
	pTrack->SetInputChannelCount(iInCount);
	pTrack->SetFirstInput(iInFirst);
}

void CDSP::SetModeForChannel(tuint32 uiChannel, tint32 iMode)
{
	// Lasse, ooops!!...
	/*
	if (uiChannel >= muiChannels)
		return;
	*/
	if (uiChannel >= muiChannels)
		return;
	// .. Lasse
	CTrack_DSP* pTrack = mppTracks[uiChannel];
	pTrack->SetTrackMode(iMode);
}

void CDSP::SetPanningLeftRightForChannel(tuint32 uiChannel, tfloat32 fLeftRight)
{
	// Lasse, ooops!!...
	/*
	if (uiChannel >= muiChannels)
		return;
	*/
	if (uiChannel >= giNumber_Of_Tracks+giNumber_Of_Busses)
		return;
	// .. Lasse

	CTrack_DSP* pTrack = mppTracks[uiChannel];
	pTrack->SetPanningLeftRight(fLeftRight);
}

void CDSP::SetPanningFrontBackForChannel(tuint32 uiChannel, tfloat32 fFrontBack)
{
	// Lasse, ooops!!...
	/*
	if (uiChannel >= muiChannels)
		return;
	*/
	if (uiChannel >= giNumber_Of_Tracks+giNumber_Of_Busses)
		return;
	// .. Lasse

	CTrack_DSP* pTrack = mppTracks[uiChannel];
	pTrack->SetPanningFrontBack(fFrontBack);
}


void CDSP::LimitDataFromStreams(tbool bEnableLimiting, tint64 iFirstSampleIx, tint64 iFinalSampleIx)
{
	// Did we specify the entire range?
	if (
		(iFirstSampleIx <= 0)
		&&
		((iFinalSampleIx < 0) || (iFinalSampleIx >= Get_Session_End_Sample()))
	) {
		// "No, no! No-no-no-no! No-no-no-no! No-no, there's no limit!"
		bEnableLimiting = false;
	}

	// Enforce limits for all stems
	for (tint32 iStem = 0; iStem < giNumber_Of_Tracks; iStem++) {
		// See if this stem should actually be limited
		tbool bLimit_ThisStem = bEnableLimiting;
		if (bLimit_ThisStem) {
			tint64 iFirstSample_ThisStem_Ignored;
			tint64 iFinalSample_ThisStem = Get_Session_End_Sample();
			CalcTrackDuration(iStem, &iFirstSample_ThisStem_Ignored, &iFinalSample_ThisStem);
			if ((iFirstSampleIx == 0) && (iFinalSample_ThisStem <= iFinalSampleIx)) {
				// "No, no! No-no-no-no! No-no-no-no! No-no, there's no limit!"
				bLimit_ThisStem = false;
			}
		}

		// Enforce limits for this stem
		CTrack_DSP* pStem = mppTracks[iStem];
		pStem->LimitDataFromStream(bLimit_ThisStem, iFirstSampleIx, iFinalSampleIx);
	}
} // LimitDataFromStreams


void CDSP::ProcessMono(float** /*ppfOut*/, const float** /*ppfIn*/, long /*lC*/)
{
	// This is never called
}

void CDSP::ProcessStereo(float** ppfOut, const float** ppfIn, long lC)
{
	mppfInputCur = (float**)ppfIn;

//	const float* pfIn0 = ppfIn[0];
//	const float* pfIn1 = ppfIn[1];
	float* pfOut0 = ppfOut[0];
	float* pfOut1 = ppfOut[1];

	// Clear busses, auxes and mix
//	mpBufferMix->SetChannelsNoConvert(2);
//	mpBufferMix->Clear();
	tint32 iBus;
	for (iBus = 0; iBus < giNumber_Of_Busses; iBus++) {
		CBuffer* pBuffer = mppBusses[iBus]->GetBuffer();
		// Prepare the bus by setting its channel count and clear the data
		pBuffer->SetChannelsNoConvert(mppBusses[iBus]->GetChannelMode());
		pBuffer->Clear();
	}
	tint32 iAUX;
	for (iAUX = 0; iAUX < giNumber_Of_AUXes; iAUX++) {
		CBuffer* pBuffer = mppAUXes[iAUX]->GetBuffer();
		// Prepare the AUX by setting its channel count and clear the data
		pBuffer->SetChannelsNoConvert(2);
		pBuffer->Clear();
	}
	{
		CBuffer* pBuffer = mpMaster->GetBuffer();
		pBuffer->SetChannelsNoConvert(2);
		pBuffer->Clear();
	}

	CKSApplication::EPlaybackState eState = gpApplication->GetPlaybackState();
	if (eState == CKSApplication::geStateExportingTrack) {
		// We only need to "play" one single track!

		tint32 iTrack = gpApplication->GetTrackToExport();
		if (mppTracks[iTrack]->HasRegions()) {
			// "Play" track
			mppTracks[iTrack]->Process(lC);

			CBuffer* pBuffer = mppTracks[iTrack]->GetBuffer();
			tfloat32* pfData0 = pBuffer->GetData(0);
			tfloat32* pfData1 = pBuffer->GetData(1);

			// Copy to output
			memcpy(pfOut0, pfData0, lC * sizeof(tfloat32));
			memcpy(pfOut1, pfData1, lC * sizeof(tfloat32));
		}
		else {
			// Channels process was not called, so update the song position
			mppTracks[iTrack]->IncSongPos(lC);

			// Zero output
			memset(pfOut0, '\0', lC * sizeof(tfloat32));
			memset(pfOut1, '\0', lC * sizeof(tfloat32));
		}

		// That's it - no more processing when exporting a track
		return;
	}
	else if (
		(eState == CKSApplication::geStatePlaying)
		||
		(eState == CKSApplication::geStateRecording)
		||
		(eState == CKSApplication::geStateExportingOutMix)
	) {
		tint32 iTrack;
		for (iTrack = 0; iTrack < giNumber_Of_Tracks; iTrack++) {
			if (mppTracks[iTrack]->HasRegions() || mppTracks[iTrack]->IsArmed()) {
				mppTracks[iTrack]->Process(lC);

				CBuffer* pBuffer = mppTracks[iTrack]->GetBuffer();

				tuint32 uiDestination = mppTracks[iTrack]->GetOutputDestination();
				if (uiDestination == ParmIOOffsetMix) {
					// Mix
//					*mpBufferMix += *pBuffer;
					CBuffer* pBufferMix = mpMaster->GetBuffer();
					*pBufferMix += *pBuffer;
				}
				else {
					// Bus
					tuint32 uiBus = uiDestination - ParmIOOffsetBus;

					if (uiBus < giNumber_Of_Busses) {
						// Get the busses buffer
						CBuffer* pBufferBus = mppBusses[uiBus]->GetBuffer();

						// Accumulate
						*pBufferBus += *pBuffer;
					}
				}

				// Only volumes if playing (not exporting)
				if (!gpApplication->IsInProgressTaskState()) {
					// Get volumes for track (max 7.1 surround)
					tfloat32 afAbsMeters[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
					CTrack_DSP* pTrack = mppTracks[iTrack];
					tint32 iNbOfChannels = pTrack->GetNumberOfChannelsForPanner();
					for (tint32 iChannelIx = 0; iChannelIx < iNbOfChannels; iChannelIx++) {
						tfloat32* pfBuff = pBuffer->GetData(iChannelIx);
						tfloat32 fAbsMeter = 0.0f;
						for (tint32 iSampleIx = 0; iSampleIx < lC; iSampleIx++) {
							tfloat32 fAbsNew = fabs(*pfBuff++);
							if (fAbsNew > fAbsMeter)
								fAbsMeter = fAbsNew;
						}
						afAbsMeters[iChannelIx] = fAbsMeter;
					}
					SetMeters_Track(iTrack, afAbsMeters);
				}
			}
			else {
				// Channels process was not called, so update the song position
				mppTracks[iTrack]->IncSongPos(lC);
			}
		}
		muiSongPos += lC;
		if(muiSongPos >= muiLoopEnd)
		{
		
			gpApplication->PlaybackGoToPos(muiLoopStart);
			// Set scroll pos to playhead
			mpRegionCallback->JumpToPlayhead();
		
		}
		else
			gpApplication->IncSongPos(lC);

		for (iBus = 0; iBus < giNumber_Of_Busses; iBus++) {
			mppBusses[iBus]->Process(lC);

			// (mo) Note: Currently sends to mix always
			CBuffer* pBuffer = mppBusses[iBus]->GetBuffer();

//			*mpBufferMix += *pBuffer;
			CBuffer* pBufferMix = mpMaster->GetBuffer();
			*pBufferMix += *pBuffer;

			// Only volumes if playing (not exporting)
			if (gpApplication->IsInProgressTaskState()) {
				// Get volumes for bus (max 7.1 surround)
				tfloat32 afAbsMeters[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
				CTrack_DSP* pBus = mppBusses[iBus];
				tint32 iNbOfChannels = pBus->GetNumberOfChannelsForPanner();
				for (tint32 iChannelIx = 0; iChannelIx < iNbOfChannels; iChannelIx++) {
					tfloat32* pfBuff = pBuffer->GetData(iChannelIx);
					tfloat32 fAbsMeter = 0.0f;
					for (tint32 iSampleIx = 0; iSampleIx < lC; iSampleIx++) {
						tfloat32 fAbsNew = fabs(*pfBuff++);
						if (fAbsNew > fAbsMeter)
							fAbsMeter = fAbsNew;
					}
					afAbsMeters[iChannelIx] = fAbsMeter;
				}
				SetMeters_Bus(iBus, afAbsMeters);
			}
		}

		for (iAUX = 0; iAUX < giNumber_Of_AUXes; iAUX++) {
			mppAUXes[iAUX]->Process(lC);

			CBuffer* pBuffer = mppAUXes[iAUX]->GetBuffer();

//			*mpBufferMix += *pBuffer;
			CBuffer* pBufferMix = mpMaster->GetBuffer();
			*pBufferMix += *pBuffer;

			// Only volumes if playing (not exporting)
			if (gpApplication->IsInProgressTaskState()) {
				// Get volumes for AUX (max 7.1 surround)
				tfloat32 afAbsMeters[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		//		CTrack_DSP* pAUX = mppAUXes[iAUX];
				// Lasse, this is very very temporary!
				//tint32 iNbOfChannels = pAUX->GetNumberOfChannelsForPanner();
				tint32 iNbOfChannels = 2;
				// .. Lasse
				for (tint32 iChannelIx = 0; iChannelIx < iNbOfChannels; iChannelIx++) {
					tfloat32* pfBuff = pBuffer->GetData(iChannelIx);
					tfloat32 fAbsMeter = 0.0f;
					for (tint32 iSampleIx = 0; iSampleIx < lC; iSampleIx++) {
						tfloat32 fAbsNew = fabs(*pfBuff++);
						if (fAbsNew > fAbsMeter)
							fAbsMeter = fAbsNew;
					}
					afAbsMeters[iChannelIx] = fAbsMeter;
				}
				SetMeters_AUX(iAUX, afAbsMeters);
			}
		}
	}

	mpMaster->Process(lC);
	CBuffer* pBufferMix = mpMaster->GetBuffer();
//	tfloat32* pfData0 = mpBufferMix->GetData(0);
//	tfloat32* pfData1 = mpBufferMix->GetData(1);
	tfloat32* pfData0 = pBufferMix->GetData(0);
	tfloat32* pfData1 = pBufferMix->GetData(1);
	// Only volumes if not exporting
	if (gpApplication->IsInProgressTaskState()) {
		// Copy to output
		memcpy(pfOut0, pfData0, lC * sizeof(tfloat32));
		memcpy(pfOut1, pfData1, lC * sizeof(tfloat32));
	}
	else {
		// Find peaks for master meter
		tfloat32 afAbsMeters[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		tfloat32 fAbsNew;
		for (long l = 0; l < lC; l++) {
			pfOut0[l] = pfData0[l];
			pfOut1[l] = pfData1[l];

			fAbsNew = fabs(pfOut0[l]);
			if (fAbsNew > afAbsMeters[0])
				afAbsMeters[0] = fAbsNew;
			fAbsNew = fabs(pfOut1[l]);
			if (fAbsNew > afAbsMeters[1])
				afAbsMeters[1] = fAbsNew;
		}
		SetMeters_Master(afAbsMeters);
	}
	
} // ProcessStereo


void CDSP::Process(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	if (muiChannels == 1) {
		ProcessMono(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
	else {
		ProcessStereo(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
} // Process

void CDSP::GetInput(CBuffer* mpBuffer, tint32 iChannels, tint32 iInputChannel, tint32 iNrOfSamples)
{
	if (iChannels == 1) {
		tfloat* pf = mppfInputCur[iInputChannel];

		memcpy(mpBuffer->GetData(0), pf, iNrOfSamples * sizeof(tfloat));
	}
	else {
		tfloat* pf1 = mppfInputCur[iInputChannel];
		tfloat* pf2 = mppfInputCur[iInputChannel + 1];

		memcpy(mpBuffer->GetData(0), pf1, iNrOfSamples * sizeof(tfloat));
		memcpy(mpBuffer->GetData(1), pf2, iNrOfSamples * sizeof(tfloat));
	}
} // GetInput


void CDSP::Start()
{
	tint32 iTrack;
	for (iTrack = 0; iTrack < giNumber_Of_Tracks; iTrack++) {
		mppTracks[iTrack]->Start();
	}

	tint32 iAUX;
	for (iAUX = 0; iAUX < giNumber_Of_AUXes; iAUX++) {
		mppAUXes[iAUX]->Start();
	}

	mpMaster->Start();
} // Start


void CDSP::Stop()
{
	tint32 iTrack;
	for (iTrack = 0; iTrack < giNumber_Of_Tracks; iTrack++) {
		mppTracks[iTrack]->Stop();
	}
	
	tint32 iAUX;
	for (iAUX = 0; iAUX < giNumber_Of_AUXes; iAUX++) {
		mppAUXes[iAUX]->Stop();
	}
	
	mpMaster->Stop();
} // Stop


//void CDSP::SetTrackInputFile(tint32 iTrack, const std::string& sTargetPathName)
//{
//	CWave_File* pWaveFile = new CWave_File();
//	pWaveFile->LoadSoundStream(512, sTargetPathName.c_str());
///*	mppStreams[iTrack] = mpStreamManager->GetStream();*/
//	IFile* pFile;
//	tint32 iOffset;
//	tint32 iLength;
//	pWaveFile->GetStreamInfo(pFile, iOffset, iLength);
///*	mppStreams[iTrack]->Reset(pFile, iOffset, iLength, false, NULL);*/
///*	mppStreams[iTrack]->SetPosition(mpPlugIn->GetSongPos());*/
//
//	if (iLength > muiFinalSongPosition) {
//		muiFinalSongPosition = iLength;
//	}
//	
//}


void CDSP::SetSongPosition(tuint64 uiPosNew)
{
 	muiSongPos	=	uiPosNew;
/*	tint32 iStream;
	for (iStream = 0; iStream < giNrOfStreams; iStream++) {
		if (mppStreams[iStream]) {
			mppStreams[iStream]->SetPosition(uiPosNew);
		}
	}*/

	tint32 iChannel;
	for (iChannel = 0; iChannel < giNumber_Of_Tracks; iChannel++) {
		mppTracks[iChannel]->SetSongPosition(uiPosNew);
	}
} // SetSongPosition


void CDSP::SetSongPosition_AndResetEffectsTails(tuint64 uiPosNew)
{
 	muiSongPos	=	uiPosNew;
/*	tint32 iStream;
	for (iStream = 0; iStream < giNrOfStreams; iStream++) {
		if (mppStreams[iStream]) {
			mppStreams[iStream]->SetPosition(uiPosNew);
		}
	}*/

	tbool bDummy = gpStreamManager->GetStutter();

	tint32 iChannel;
	for (iChannel = 0; iChannel < giNumber_Of_Tracks; iChannel++) {
		mppTracks[iChannel]->SetSongPosition(uiPosNew);
		mppTracks[iChannel]->ResetAllEffectsTails();
	}

	mppAUXes[0]->ResetAllEffectsTails();
	mppAUXes[1]->ResetAllEffectsTails();

	bDummy = gpStreamManager->GetStutter();
	miStutterCounter = 0;
} // SetSongPosition_AndResetEffectsTails


tint32 CDSP::GetStutterCounter()
{
	if (gpStreamManager->GetStutter()) {
		miStutterCounter++;
	}
	return miStutterCounter;
} // GetStutterCounter


tuint32 CDSP::GetFinalSoundPos()
{
	return muiSession_End_Sample;
}


/*
void CDSP::UpdateAUXData(tint32 iID, tint32 iValue, tint32 iAUX)
{

	switch(iID) {
		case giParam_ChVol:
//			if(!mppTracks[iAUX]->mbMuteFlag)
				mppAUXes[iAUX]->SetVolume((tfloat32)(iValue / 10000.0));
				miStored_AUX_Volume[iAUX] = iValue;
			break;
		case giParam_ChSoftMute:
		case giParam_ChMute: {
			if(iValue) {
				mbMute_AUX_Flag[iAUX] = true;
				mppAUXes[iAUX]->SetVolume((tfloat32)(0.000001f / 10000.0));
			}
			else {
				mbMute_AUX_Flag[iAUX] = false;
				mppAUXes[iAUX]->SetVolume((tfloat32)(miStored_AUX_Volume[iAUX] / 10000.0));
			}
			break;
		}

		case giParam_ChIn:
//			mppAUXes[iAUX]->SetInputChannelCount(iValue);
			break;

		case giParam_ChMode:
			// Lasse, rem 2008-01-16
			//mppAUXes[iAUX]->SetInputChannelCount(iValue);
			// .. Lasse
			mppAUXes[iAUX]->SetTrackMode(iValue);
			break;

		case giParam_ChOut:
			mppAUXes[iAUX]->SetOutputDestination(iValue, FindDestinationNumberOfChannels(iValue));
			break;
			
		case giParam_ChPannerLeftRight:
			{
				tfloat32 fLeftRight = (tfloat32)iValue / 10000.0f;
				CTrack_DSP* pAUX = mppAUXes[iAUX];
				pAUX->SetPanningLeftRight(fLeftRight);
			}
			break;
	}

}
*/
void CDSP::UpdateBussData(tint32 iID, tint32 iValue, tint32 iBus)
{
	switch(iID) {
		case giParam_ChVol:
//			if(!mppTracks[iBus]->mbMuteFlag)
				mppBusses[iBus]->SetVolume((tfloat32)(iValue / 10000.0));
				miStored_Buss_Volume[iBus] = iValue;
			break;
		case giParam_ChSoftMute:
		case giParam_ChMute: {
			if(iValue) {
				mbMute_Buss_Flag[iBus] = true;
				mppBusses[iBus]->SetVolume((tfloat32)(0.000001f / 10000.0));
			}
			else {
				mbMute_Buss_Flag[iBus] = false;
				mppBusses[iBus]->SetVolume((tfloat32)(miStored_Buss_Volume[iBus] / 10000.0));
			}
			break;
		}

		case giParam_ChInMode:
			mppBusses[iBus]->SetInputChannelCount(iValue);
			break;

		case giParam_ChMode:
			// Lasse, rem 2008-01-16
			//mppBusses[iBus]->SetInputChannelCount(iValue);
			// .. Lasse
			mppBusses[iBus]->SetTrackMode(iValue);
			break;

		case giParam_ChOut:
			mppBusses[iBus]->SetOutputDestination(iValue, FindDestinationNumberOfChannels(iValue));
			break;
			
		case giParam_Buss_PannerLeftRight:
			{
				tfloat32 fLeftRight = iValue;
				fLeftRight	/=	2.0f;
				fLeftRight	+=  5000.0f;
				fLeftRight	/= 10000.0f;
				CTrack_DSP* pBus = mppBusses[iBus];
				pBus->SetPanningLeftRight(fLeftRight);
			}
			break;

		case giParam_Buss_Insert1:
		case giParam_Buss_Insert2:
		case giParam_Buss_Insert3:
		case giParam_Buss_Insert4:
			{
				tint32 iInsert = iID - giParam_Buss_Insert1;
				mppBusses[iBus]->AddInsert(iInsert, iValue >> 8, iValue & 0xff, iValue);
			}
			break;

		case giParam_Bus_Insert1Bypass:
		case giParam_Bus_Insert2Bypass:
		case giParam_Bus_Insert3Bypass:
		case giParam_Bus_Insert4Bypass:
			{
				tint32 iInsert = iID - giParam_Bus_Insert1Bypass;
				mppBusses[iBus]->SetInsertBypass(iInsert, iValue ? true : false);
			}
			break;
	}
}

void CDSP::UpdateMasterData(tint32 iID, tint32 iValue)
{
	switch(iID) {
		case giParam_Master_Insert1:
		case giParam_Master_Insert2:
		case giParam_Master_Insert3:
		case giParam_Master_Insert4:
			{
				tint32 iInsert = iID - giParam_Master_Insert1;
				mpMaster->AddInsert(iInsert, iValue >> 8, iValue & 0xff, iValue);
			}
			break;

		case giParam_Master_Insert1Bypass:
		case giParam_Master_Insert2Bypass:
		case giParam_Master_Insert3Bypass:
		case giParam_Master_Insert4Bypass:
			{
				tint32 iInsert = iID - giParam_Master_Insert1Bypass;
				mpMaster->SetInsertBypass(iInsert, iValue ? true : false);
			}
			break;
		case giParam_Master_Volume:{
			
			
			mpMaster->SetVolume((tfloat32)(iValue / 10000.0));
			break;
		}
	}
}

void CDSP::UpdateChannelData(tint32 iID, tint32 iValue, tint32 iChannel)
{
/*	if (iChannel >= giNumber_Of_Tracks) {
		UpdateBusData(iID, iValue, iChannel - giNumber_Of_Tracks);
		return;
	}
*/
	switch(iID) {
		case giParam_ChVol:{
			if(!mbMute_Track_Flag[iChannel])
				mppTracks[iChannel]->SetVolume((tfloat32)(iValue / 10000.0));
			miStored_Track_Volume[iChannel] = iValue;
			break;
			}
		case giParam_ChSoftMute:
		case giParam_ChMute: {
			if(iValue) {
				mbMute_Track_Flag[iChannel] = true;
				mppTracks[iChannel]->SetVolume((tfloat32)(0.000001f / 10000.0));
			}
			else {
				mbMute_Track_Flag[iChannel] = false;
				mppTracks[iChannel]->SetVolume((tfloat32)(miStored_Track_Volume[iChannel] / 10000.0));
			}
			break;
		}

		case giParam_ChInMode:
			mppTracks[iChannel]->SetInputChannelCount(iValue);
			break;

		case giParam_ChMode:
			mppTracks[iChannel]->SetTrackMode(iValue);
			break;

		case giParam_ChOut:
			mppTracks[iChannel]->SetOutputDestination(iValue, FindDestinationNumberOfChannels(iValue));
			break;
			
		case giParam_ChPannerLeftRight:{
		
			tfloat32 fLeftRight = iValue;
			fLeftRight	/=	2.0f;
			fLeftRight	+=  5000.0f;
			SetPanningLeftRightForChannel(iChannel, fLeftRight / 10000.0f);
			break;
		}
		case giParam_ChPannerFrontBack:
			SetPanningFrontBackForChannel(iChannel, (tfloat32)iValue / 10001.0f);
			break;

		case giParam_ChAUX1:
			mppTracks[iChannel]->SetAUXVolume(0, (tfloat32)(iValue / 10000.0));
			break;
		case giParam_ChAUX2:
			mppTracks[iChannel]->SetAUXVolume(1, (tfloat32)(iValue / 10000.0));
			break;

		case giParam_ChInsert1:
		case giParam_ChInsert2:
		case giParam_ChInsert3:
		case giParam_ChInsert4:
			{
				tint32 iInsert = iID - giParam_ChInsert1;
				mppTracks[iChannel]->AddInsert(iInsert, iValue >> 8, iValue & 0xff, iValue);
			}
			break;

		case giParam_Ch_Insert1Bypass:
		case giParam_Ch_Insert2Bypass:
		case giParam_Ch_Insert3Bypass:
		case giParam_Ch_Insert4Bypass:
			{
				tint32 iInsert = iID - giParam_Ch_Insert1Bypass;
				mppTracks[iChannel]->SetInsertBypass(iInsert, iValue ? true : false);
			}
			break;

		case giParam_ChArm:
			mppTracks[iChannel]->SetArmed(iValue ? true : false);
			break;

		case giParam_ChIn:
			mppTracks[iChannel]->SetInputChannel(iValue);
			break;
	}
}

void CDSP::UpdateAUX1Data(tint32 iID, tint32 iValue)
{
	CRack_Echo_DSP* pEcho = dynamic_cast<CRack_Echo_DSP*>(mppAUXes[0]);
	
	switch(iID) {
		case giAUX1_Param_Delay_Power:
			pEcho->Set_Delay_Power(iValue != 0);
			break;
		case giAUX1_Param_Delay_Time:
			pEcho->Set_Delay_Time(iValue);
			break;
		case giAUX1_Param_Feedback:
			pEcho->Set_Feedback(iValue);
			break;
		case giAUX1_Param_LP_Damping:
			pEcho->Set_LF_Damping(iValue);
			break;
		case giAUX1_Param_HP_Damping:
			pEcho->Set_HP_Damping(iValue);
			break;
		case giAUX1_Param_Sync:
			pEcho->Set_Sync_To_Tempo(iValue != 0);
			break;
		
	}
}

void CDSP::UpdateAUX2Data(tint32 iID, tint32 iValue)
{
	CRack_Reverb_DSP* pReverb = dynamic_cast<CRack_Reverb_DSP*>(mppAUXes[1]);
	
	switch(iID) {
		case giParam_Reverb_On:
			pReverb->Set_Reverb_On(iValue);
			break;
		case giParam_Reverb_Size:
			pReverb->Set_Reverb_Size(iValue);
			break;
		case giParam_Reverb_PDelay:
			pReverb->Set_Reverb_PDelay(iValue);
			break;
		case giParam_Reverb_Damp:
			pReverb->Set_Reverb_Damp(iValue);
			break;
		case giParam_Reverb_Panorama:
			pReverb->Set_Reverb_Panorama(iValue);
			break;
		case giParam_Reverb_Color1:
			pReverb->Set_Reverb_Color1(iValue);
			break;
		case giParam_Reverb_Color2:
			pReverb->Set_Reverb_Color2(iValue);
			break;
		case giParam_Reverb_Mix:
			pReverb->Set_Reverb_Mix(iValue);
			break;
	}
}


tint32 CDSP::FindDestinationNumberOfChannels(tint32 iDestination)
{
	tint32 iBusNr = iDestination - ParmIOOffsetBus;
	if ((iBusNr >= 0) && (iBusNr < giNumber_Of_Busses))
		return mppBusses[iBusNr]->GetNumberOfInputChannels();
	return 2;
}

tint32 CDSP::GetNumberOfInputChannels(tint32 iChannelOrBus)
{
	tint32 iChannels = 0;
	tint32 iBusNr = iChannelOrBus - giNumber_Of_Tracks;
	if (iBusNr < 0)
		iChannels = mppTracks[iChannelOrBus]->GetNumberOfInputChannels();
	else {
		if (iBusNr < giNumber_Of_Busses)
			iChannels = mppBusses[iBusNr]->GetNumberOfInputChannels();
		else {
			// TODO: Find out: Why does this return 0?
//			iChannels = mpBufferMix->GetChannels();
//			if (iChannels == 0)
				iChannels = 2;
		}
	}
	return iChannels;
}

tint32 CDSP::GetNumberOfChannelsForPanner(tint32 iChannelOrBus)
{
	tint32 iChannels = 0;
	tint32 iBusNr = iChannelOrBus - giNumber_Of_Tracks;
	if (iBusNr < 0)
		iChannels = mppTracks[iChannelOrBus]->GetNumberOfChannelsForPanner();
	else {
		if (iBusNr < giNumber_Of_Busses)
			iChannels = mppBusses[iBusNr]->GetNumberOfChannelsForPanner();
		else {
			// TODO: Find out: Why does this return 0?
//			iChannels = mpBufferMix->GetChannels();
//			if (iChannels == 0)
				iChannels = 2;
		}
	}
	return iChannels;
}

tint32 CDSP::GetDestinationForChannelOrBus(tint32 iChOrBus)
{
	tint32 iBusNr = iChOrBus - giNumber_Of_Tracks;
	if (iBusNr < 0) {
		// This is a channel		
		return mppTracks[iChOrBus]->GetOutputDestination();
	}
	else if (iBusNr < giNumber_Of_Busses) {
		// This is a bus
		return mppBusses[iBusNr]->GetOutputDestination();
	}
	// This is the master mix
	return -1;
}

void CDSP::SetDestinationForChannelOrBus(tint32 iChOrBus, tint32 iDestination, tint32 iDestNumberOfChannels)
{
	tint32 iBusNr = iChOrBus - giNumber_Of_Tracks;
	if (iBusNr < 0) {
		// This is a channel		
		mppTracks[iChOrBus]->SetOutputDestination(iDestination, iDestNumberOfChannels);
	}
	else if (iBusNr < giNumber_Of_Busses) {
		// This is a bus
		mppBusses[iBusNr]->SetOutputDestination(iDestination, iDestNumberOfChannels);
	}
	// This is the master mix
	//return -1;
}

tbool CDSP::UpdateDestinationForChannelOrBus_ButOnlyIfItMatches(tint32 iChOrBus, tint32 iDestination, tint32 iDestNumberOfChannels)
{
	tint32 iBusNr = iChOrBus - giNumber_Of_Tracks;
	if (iBusNr < 0) {
		// This is a channel
		if (iDestination == mppTracks[iChOrBus]->GetOutputDestination()) {
			mppTracks[iChOrBus]->SetOutputDestination(iDestination, iDestNumberOfChannels);
			return true;
		}
	}
	else if (iBusNr < giNumber_Of_Busses) {
		// This is a bus
		if (iDestination == mppBusses[iBusNr]->GetOutputDestination()) {
			mppBusses[iBusNr]->SetOutputDestination(iDestination, iDestNumberOfChannels);
			return true;
		}
	}
	// This is the master mix - or destination didn't match for iChOrBus
	return false;
}

CDSP::RegionSearchHandle CDSP::GetFirstRegion(SRegionInfo& rInfo)
{
	tint32 uiTrack;
	for (uiTrack = 0; uiTrack < giNumber_Of_Tracks; uiTrack++) {
		CTrack_DSP* pTrack = mppTracks[uiTrack];

		std::list<CTrack_DSP::SChannelRegionInfo*>::const_iterator it = pTrack->GetRegionList().begin();
		if (it != pTrack->GetRegionList().end()) {
			// Track not empty, return first region

			rInfo.uiRegionID = (*it)->pRegion->GetID();
			rInfo.uiTrack = uiTrack;
			rInfo.uiStartPos = (*it)->uiTrackPosStart;
			rInfo.uiEndPos = rInfo.uiStartPos + (*it)->pRegion->GetDuration() - 1;

			SRegionSearchInfo* pInfo = new SRegionSearchInfo();
			pInfo->uiTrackCur = uiTrack;
			pInfo->itCur = it;

			return (RegionSearchHandle)pInfo;
		}
	}

	return mRegionSearchEnd;
}

void CDSP::GetNextRegion(CDSP::RegionSearchHandle& Handle, CDSP::SRegionInfo& rInfo)
{
	SRegionSearchInfo* pInfo = (SRegionSearchInfo*)Handle;

	tuint32 uiChannelOrg = pInfo->uiTrackCur;

	tuint32 uiTrack;
	for (uiTrack = uiChannelOrg; uiTrack < giNumber_Of_Tracks; uiTrack++) {
		CTrack_DSP* pTrack = mppTracks[uiTrack];

		std::list<CTrack_DSP::SChannelRegionInfo*>::const_iterator it;
		if (uiTrack == uiChannelOrg) {
			it = pInfo->itCur;
			it++;
		}
		else {
			it = pTrack->GetRegionList().begin();
		}

		if (it != pTrack->GetRegionList().end()) {
			// Still a region on the track

			rInfo.uiRegionID = (*it)->pRegion->GetID();
			rInfo.uiTrack = uiTrack;
			rInfo.uiStartPos = (*it)->uiTrackPosStart;
			rInfo.uiEndPos = rInfo.uiStartPos + (*it)->pRegion->GetDuration() - 1;

			pInfo->uiTrackCur = uiTrack;
			pInfo->itCur = it;

			Handle = (RegionSearchHandle)pInfo;
			return;
		}
	}

	// We seem to have run out of regions
	delete pInfo;

	Handle = mRegionSearchEnd;
}

void CDSP::FinishRegionSearch(CDSP::RegionSearchHandle Handle)
{
	if (Handle != mRegionSearchEnd) {
		SRegionSearchInfo* pInfo = (SRegionSearchInfo*)Handle;
		delete pInfo;
	}
}
	
void CDSP::Delete_Selection()
{
	for(tint iTrack=0; iTrack<giNumber_Of_Tracks; iTrack++){
	
		tint32 iSelection_Type = mpTrackSelectionInfo[iTrack].uiSelection_Type;
		tint32 iRegionID = mpTrackSelectionInfo[iTrack].iRegionID;
		switch(iSelection_Type){
			case giSelect_Region:{
				DeleteRegion(iTrack, iRegionID );
				break;
			}
			case giSelect_On_Track:{
				tuint64 uiSelection_Pos			= mpTrackSelectionInfo[iTrack].uiSelection_Pos;
				tuint64 uiSelection_Duration	= mpTrackSelectionInfo[iTrack].uiSelection_Duration;
				mppTracks[iTrack]->Edit_Selection(giTrim,uiSelection_Pos, uiSelection_Duration);

				break;
			}
		}
	}
	Deselect_All_Tracks();
}

void CDSP::Refresh_Region_GUI(tint32 iRegion, tuint32 iTrack)
{	

	mpRegionCallback->Refresh_Region_GUI(iRegion, iTrack);

}

/*tint32 CDSP::CreateRegion(const std::string& sSoundListItemName, 
							tint32 iChannel, 
							tuint64 uiTrack_Pos, 
							tuint64 uiSample_Start, 
							tuint64 uiSamplePosEnd,
							tuint64 uiFadeInLength,
							tuint64 uiFadeOutLength,
							tfloat32 fRegionVolume)
{
	// (mo) Temporary id code:
	static int iID = 0;
	tint32 iRegionID = iID++;

	// DSP
   	mppTracks[iChannel]->CreateRegion(iRegionID, 
										sSoundListItemName, 
										uiTrack_Pos, 
										uiSample_Start, 
										uiSamplePosEnd,
										uiFadeInLength,
										uiFadeOutLength,
										fRegionVolume);
	
	// GUI		
	tuint64 uiSample_Duration = uiSamplePosEnd - uiSample_Start;
	//Set_Session_Size( uiSample_Duration + uiTrack_Pos + 32);
	mpRegionCallback->InsertRegion(iRegionID, iChannel, uiTrack_Pos, uiSample_Start, uiSample_Duration, sSoundListItemName.c_str());
	mpRegionCallback->SelectRegion(iRegionID, iChannel);
	SelectRegion(iRegionID);
	
	//Set_Session_Size( (uiSamplePosEnd - uiSample_Start) + uiTrack_Pos + 32);
	Sanitize_Session_End_Sample();	

	//	if (iLength > muiFinalSongPosition) {
//		muiFinalSongPosition = iLength;
//	}
	
	return iRegionID;
}
*/
void CDSP::Set_Session_Size(tuint64 uiRegionEndSample)
{
	if( muiSession_End_Sample < uiRegionEndSample)
		muiSession_End_Sample = uiRegionEndSample;

}

void CDSP::Sanitize_Session_End_Sample()
{
	tuint64 uiEndPos = 0;
	SRegionInfo Info;
	// Make list of regions to delete
	RegionSearchHandle Search = GetFirstRegion(Info);
	while (Search != mRegionSearchEnd) {
		if (uiEndPos < (tint64)Info.uiEndPos)
			uiEndPos = (tint64)Info.uiEndPos;
		GetNextRegion(Search, Info);
	}
	FinishRegionSearch(Search);
	muiSession_End_Sample = uiEndPos;
} // Sanitize_Session_End_Sample


tbool CDSP::CalcTrackDuration(tint32 iTrack, tint64* piFirstSample, tint64* piFinalSample)
{
	*piFirstSample = Get_Session_End_Sample();
	*piFinalSample = -1;

	// Search through all regions
	SRegionInfo Info;
	// Make list of regions to delete
	RegionSearchHandle Search = GetFirstRegion(Info);
	while (Search != mRegionSearchEnd) {
		if (Info.uiTrack == iTrack) {
			if (*piFirstSample > (tint64)Info.uiStartPos)
				*piFirstSample = (tint64)Info.uiStartPos;
			if (*piFinalSample < (tint64)Info.uiEndPos)
				*piFinalSample = (tint64)Info.uiEndPos;
		}
		GetNextRegion(Search, Info);
	}
	FinishRegionSearch(Search);
	
	return true;
} // CalcTrackDuration


void CDSP::MoveRegion(tuint32 uiRegionID, tint32 iChannelNew, tuint64 uiTrackPosStartNew)
{
	SRegionInfo RegionInfo;
	GetRegionInfo( RegionInfo, uiRegionID);
	
	CRegion_DSP* pRegionSoundObject	=	mppTracks[RegionInfo.uiTrack]->GetRegion_DSP(uiRegionID);
	std::string sClipName				=	pRegionSoundObject->GetSoundListItemName();
	tuint64 uiSoundPosStart				=	pRegionSoundObject->GetSoundStartPos();
	tuint64 uiSoundDutation				=	pRegionSoundObject->GetDuration();
	tuint64 uiSamplePosEnd				=	uiSoundPosStart + uiSoundDutation-1;
	tuint64 uiFadeInLength				=	pRegionSoundObject->GetFadeInLength();
	tuint64 uiFadeOutLength				=	pRegionSoundObject->GetFadeOutLength();
	tfloat32 fRegionVolume				=	pRegionSoundObject->GetRegionVolume();
	
	DeleteRegion(iChannelNew, uiRegionID);
	
	uiRegionID = CreateRegion(sClipName, 
							  iChannelNew, 
							  uiTrackPosStartNew, 
							  uiSoundPosStart, 
							  uiSamplePosEnd,
							  uiFadeInLength, 
							  uiFadeOutLength,
							  fRegionVolume);
	Deselect_All_Tracks();
	SelectRegion(uiRegionID);
}

void  CDSP::GetRegionInfo(SRegionInfo& RegionInfo, tuint32 uiRegionID)
{
	RegionSearchHandle Search = GetFirstRegion(RegionInfo);
	while ((Search != mRegionSearchEnd) &&
		(RegionInfo.uiRegionID != uiRegionID)) {
		GetNextRegion(Search, RegionInfo);
	}

	if (Search == mRegionSearchEnd) {
		// We didn't find a matching region. Make sure we don't crash
		RegionInfo.uiRegionID = 0;
		RegionInfo.uiTrack = 0;
		RegionInfo.uiStartPos = 0;
		RegionInfo.uiEndPos = 0;
		return;
	}
	FinishRegionSearch(Search);

}

tuint64 CDSP::GetRegionSize(const std::string& sSoundPathName,tuint64 uiSamplePosStart, tint64 uiSamplePosEnd)
{
	tint64 iSize = mppTracks[0]->GetRegionSize(0, sSoundPathName, uiSamplePosStart, uiSamplePosEnd);
	return iSize;
}

tuint64 CDSP::GetRegionSize(tuint32 uiID )
{
	SRegionInfo RegionInfo;
	GetRegionInfo( RegionInfo, uiID);
	CRegion_DSP* pRegionSoundObject	=	mppTracks[RegionInfo.uiTrack]->GetRegion_DSP(uiID);
	return	pRegionSoundObject->GetDuration();
}

SRegion_Drawing_Info CDSP::Get_Region_Drawing_Info(tuint iRegionID)
{
	SRegion_Drawing_Info info;

	SRegionInfo RegionInfo;
	GetRegionInfo( RegionInfo, iRegionID);

	CRegion_DSP* pRegion	=	mppTracks[RegionInfo.uiTrack]->GetRegion_DSP(iRegionID);
	GetRegionInfo( RegionInfo, iRegionID);

	if (pRegion) {
		info.uiSound_Start		=	pRegion->GetSoundStartPos();
		info.uiSound_Duration	=	pRegion->GetDuration();
		info.uiTrack_Start		=	RegionInfo.uiStartPos;
		info.uiFade_In			=	pRegion->GetFadeInLength();
		info.uiFade_Out			=	pRegion->GetFadeOutLength();
		info.fRegion_Volume		=	pRegion->GetRegionVolume();
	}
	else {
		info.uiSound_Start		=	0;
		info.uiSound_Duration	=	0;
		info.uiTrack_Start		=	RegionInfo.uiStartPos;
		info.uiFade_In			=	0;
		info.uiFade_Out			=	0;
		info.fRegion_Volume		=	0;
	}

	return info;
}

void CDSP::GetRegionPeakFile(tuint32 uiID, IFile** ppFile, tint32 iChannel, tint32 iSize)
{
	SRegionInfo RegionInfo;
	GetRegionInfo(RegionInfo, uiID);

	CRegion_DSP* pRegion = mppTracks[RegionInfo.uiTrack]->GetRegion_DSP(uiID);
	if (pRegion)
		pRegion->GetPeakFile(ppFile, iChannel, iSize);
	else
		*ppFile = NULL;
}

void CDSP::DeleteAllRegionsForTrack(tint32 iTrack)
{
	// remove selectin from track
	Deselect_Track(iTrack);
	std::list<tuint32> listuiIDs;
	// Search through all regions
	SRegionInfo Info;
	// Make list of regions to delete
	RegionSearchHandle Search = GetFirstRegion(Info);
	while (Search != mRegionSearchEnd) {
		if (Info.uiTrack == iTrack) {
			listuiIDs.insert(listuiIDs.begin(), Info.uiRegionID);
		}
		GetNextRegion(Search, Info);
	}
	FinishRegionSearch(Search);
	
	// Delete Regions on Channels and GUI and erase then from list to delete
	while (listuiIDs.size()) {
		tuint32 uiID = *(listuiIDs.begin());
		mppTracks[iTrack]->DeleteRegion(uiID);
		mpRegionCallback->DeleteRegion(uiID, iTrack);
		listuiIDs.erase(listuiIDs.begin());
	}	
}


void CDSP::DeleteRegion(tuint32 uiTrack, tuint32 uiRegionID)
{
	SRegionInfo RegionInfo;
	GetRegionInfo( RegionInfo, uiRegionID);
	// Delete region on channel
	mppTracks[RegionInfo.uiTrack]->DeleteRegion(uiRegionID);
	// Find new end of song
	Sanitize_Session_End_Sample();
	// Delete region on GUI
	DeleteRegionOnGUI(RegionInfo.uiTrack, uiRegionID);

}

void CDSP::DeleteRegionOnGUI(tuint32 uiTrack, tuint32 uiRegionID)
{
	mpTrackSelectionInfo[uiTrack].uiSelection_Type = giSelect_Off;
	mpTrackSelectionInfo[uiTrack].iRegionID	= -1;
	mpRegionCallback->DeleteRegion(uiRegionID, uiTrack);
}

tbool CDSP::SaveTrackRegionDataToChunk(tint32 iTrack, IChunk* pChunk)
{
	CTrack_DSP* pTrack = GetTrack(iTrack);
	std::list<CTrack_DSP::SChannelRegionInfo*>::const_iterator it = pTrack->GetRegionList().begin();
	std::string sRc = "";

#ifdef _WIN32
	const tchar pszUI64Format[] = "%I64u|";
#else _WIN32
	const tchar pszUI64Format[] = "%llu|";
#endif // #else _WIN32
				
	const tint32 iSaveVersion = 2;
	if (iSaveVersion == 1) {
		sRc = "0001|"; // Version
		for (; it != pTrack->GetRegionList().end(); it++) {
			CTrack_DSP::SChannelRegionInfo* pRegionInfo = *it;
			CRegion_DSP* pObject = pRegionInfo->pRegion;

			tuint64 uiStart = pObject->GetSoundStartPos();
			tuint64 uiEnd = pObject->GetDuration();
			std::string sPath = pObject->GetSound(0)->GetPathName();
			tchar pszPathOnly_Dummy[1024];
			tchar pszNameOnly[1024];
			if (IFile::SplitPathToPathAndName(sPath.c_str(), pszPathOnly_Dummy, pszNameOnly)) {
				tchar* pszLastDot = strrchr(pszNameOnly, '.');
				if (pszLastDot) {
					// Remove extension
					*pszLastDot = '\0';
				}

				tchar pszBuff[1024];
				tchar* psz = pszBuff;
				//psz += sprintf(psz, "%u|", uiID);uiTrackPosStart
				psz += sprintf(psz, pszUI64Format, pRegionInfo->uiTrackPosStart);
				psz += sprintf(psz, pszUI64Format, uiStart);
				psz += sprintf(psz, pszUI64Format, uiEnd);
				psz += sprintf(psz, "%s|", pszNameOnly);

				sRc += pszBuff;
			}
		}
		
		// Delete ending '|' char
		sRc.erase(sRc.length() - 1);
	}
	else if (iSaveVersion == 2) {
		//sRc = "0002|4|"; // Version number and record length
		sRc = "0002|7|"; // Version number and record length
		for (; it != pTrack->GetRegionList().end(); it++) {
			CTrack_DSP::SChannelRegionInfo* pRegionInfo = *it;
			CRegion_DSP* pObject = pRegionInfo->pRegion;
			const tchar* pszListItemName = pObject->GetSoundListItemName();
			tuint64 uiStart = pObject->GetSoundStartPos();
			tuint64 uiDurat = pObject->GetDuration();
			//
			tuint64 uiFadeIn = pObject->GetFadeInLength();
			tuint64 uiFadeOut = pObject->GetFadeOutLength();
			tfloat32 fVolume = pObject->GetRegionVolume();
			tuint64 uiVol1000000 = Float2Int(fVolume * (tuint64)1000000);
			
			tchar pszBuff[1024];
			tchar* psz = pszBuff;
			//psz += sprintf(psz, "%u|", uiID);uiTrackPosStart
			psz += sprintf(psz, pszUI64Format, pRegionInfo->uiTrackPosStart);
			psz += sprintf(psz, pszUI64Format, uiStart);
			psz += sprintf(psz, pszUI64Format, uiDurat);
			psz += sprintf(psz, "%s|", pszListItemName);
			//
			psz += sprintf(psz, pszUI64Format, uiFadeIn);
			psz += sprintf(psz, pszUI64Format, uiFadeOut);
			psz += sprintf(psz, pszUI64Format, uiVol1000000);

			sRc += pszBuff;
		}
		
		// Delete ending '|' char
		sRc.erase(sRc.length() - 1);
	}

	pChunk->Write(sRc.c_str(), sRc.size());

	return true;
} // SaveTrackRegionDataToChunk


tbool CDSP::CreateRegionFromChunkData(tint32 iTrack, IChunk* pChunk)
{
//	CKSApplication* pPlugIn = (CKSApplication*)GetPlugIn();

	tchar* pszBuff = new tchar[pChunk->GetSize() + 1];
	pChunk->Read(pszBuff, pChunk->GetSize());
	pszBuff[pChunk->GetSize()] = '\0';

	std::list<std::string> asItems;
	PszEnumString2List(pszBuff, '|', asItems);
	std::list<std::string>::iterator it = asItems.begin();

#ifdef _WIN32
	const tchar pszUI64Format[] = "%I64u";
#else _WIN32
	const tchar pszUI64Format[] = "%llu";
#endif // #else _WIN32

	tint32 iItems = asItems.size();
	if (iItems > 0) {
		std::string sVersion = *it++;
		iItems--;
		tint32 iVersion = 0;
		sscanf(sVersion.c_str(), "%d", &iVersion);
		tint32 iItemsPerRecord = -1;
		if (iVersion > 1) {
			std::string sItem = *it++;
			sscanf(sItem.c_str(), "%d", &iItemsPerRecord);
		}
		switch (iVersion) {
			case 1:
				{
					iItemsPerRecord = 4;
					while (iItems >= iItemsPerRecord) {
						tuint64 uiTrackPosStart;
						// No support for trimming in version 1 saved files
						//tuint64 uiStart;
						//tuint64 uiEnd;

						std::string sItem = *it++;
						sscanf(sItem.c_str(), pszUI64Format, &uiTrackPosStart);
						sItem = *it++;
						//sscanf(sItem.c_str(), pszUI64Format, &uiStart);
						sItem = *it++;
						//sscanf(sItem.c_str(), pszUI64Format, &uiEnd);
						sItem = *it++;
						CreateRegion(sItem.c_str(), iTrack, uiTrackPosStart, 0, (tuint64)-1);//, uiStart, uiEnd);
						
						iItems -= iItemsPerRecord;
					}
				}
				break;

			case 2:
				{
					if (iItemsPerRecord >= 4) {
						// How many items the current version of KS expects
						tint32 iItemsPerRecord_backCompat = 7;
						while (iItems >= iItemsPerRecord) {
							std::string sClipName;
							tuint64 uiTrackPosStart;
							tuint64 uiRegionStartPos;
							tuint64 uiRegionDuration;
							//
							tuint64			uiFadeIn		= 0;
							tuint64			uiFadeOut		= 0;
							tfloat32		fRegionVolume	= 1.0f;

							std::string sItem = *it++;
							sscanf(sItem.c_str(), pszUI64Format, &uiTrackPosStart);
							sItem = *it++;
							sscanf(sItem.c_str(), pszUI64Format, &uiRegionStartPos);
							sItem = *it++;
							sscanf(sItem.c_str(), pszUI64Format, &uiRegionDuration);
							sClipName = *it++;
							tuint64 uiRegionStopPos = uiRegionStartPos + uiRegionDuration - 1;
							//
							if (iItemsPerRecord >= 6) {
								sItem = *it++;
								sscanf(sItem.c_str(), pszUI64Format, &uiFadeIn);
								sItem = *it++;
								sscanf(sItem.c_str(), pszUI64Format, &uiFadeOut);
							}
							//
							if (iItemsPerRecord >= 7) {
								sItem = *it++;
								tuint64 uiVol1000000;
								sscanf(sItem.c_str(), pszUI64Format, &uiVol1000000);
								fRegionVolume = (tfloat32)(uiVol1000000 / 1000000.0);
							}
							//
							CreateRegion(sClipName.c_str(), iTrack,
								uiTrackPosStart, uiRegionStartPos, uiRegionStopPos,
								uiFadeIn, uiFadeOut, fRegionVolume);
							//


							// Ignore extra items not supported yet (save-file from newer version of KS)
							while (iItemsPerRecord_backCompat < iItemsPerRecord) {
								iItemsPerRecord_backCompat++;
								it++;
							}
							
							iItems -= iItemsPerRecord;
						}
					} // >= 4
				}
				break;

			default:
				return false;
		}
	}

	return true;
} // CreateRegionFromChunkData


void CDSP::DeleteTrack(tuint32 uiTrack)
{

	//mpRegionCallback
}


tbool CDSP::IsAudioDeviceDuplex()
{
	return IsAudioDeviceDuplex(-1, -1, -1, -1);
} // IsAudioDeviceDuplex


tbool CDSP::IsAudioDeviceDuplex(tint32 iInMgrIx, tint32 iInDevIx, tint32 iOutMgrIx, tint32 iOutDevIx)
{
	tbool bDoesSupportDuplex = false;

	SKSPrefs_1 Prefs1;
	if (iOutMgrIx >= 0) {
		Prefs1.iAudioManagerIx = iOutMgrIx;
		Prefs1.iAudioDeviceIx = iOutDevIx;
	}
	else {
		Prefs1.iAudioManagerIx = mPrefs_1.iAudioManagerIx;
		Prefs1.iAudioDeviceIx = mPrefs_1.iAudioDeviceIx;
	}

	CKSPrefs_2 Prefs2;
	if (iInMgrIx >= 0) {
		Prefs2.iInputManagerIx = iInMgrIx;
		Prefs2.iInputDeviceIx = iInDevIx;
	}
	else {
		Prefs2.iInputManagerIx = mPrefs_2.iInputManagerIx;
		Prefs2.iInputDeviceIx = mPrefs_2.iInputDeviceIx;
	}

	if ((Prefs1.IsSane()) && (Prefs2.IsSane())) {
		if (Prefs1.iAudioManagerIx == Prefs2.iInputManagerIx) {
			if (Prefs1.iAudioDeviceIx == Prefs2.iInputDeviceIx) {
				// It is the same device
				aioe::IDeviceManager* pMgr = mapAudioManager[Prefs1.iAudioManagerIx];
				bDoesSupportDuplex = pMgr->DevicesMayBeDuplex();
			}
		}
	}

	return bDoesSupportDuplex;
} // IsAudioDeviceDuplex


tbool CDSP::OpenAudioDevice_FromPrefs_Output(tbool* pbAudioDevice_LastOpenCrashed, tbool* pbMidiDevice_LastOpenCrashed)
{
	LoadPrefs();
	*pbAudioDevice_LastOpenCrashed = (mPrefs_1.cbAudioDevice_LastOpenCrashed != 0);
	*pbMidiDevice_LastOpenCrashed = (mPrefs_1.cbMidiDevice_LastOpenCrashed != 0);
	if ((*pbAudioDevice_LastOpenCrashed) || (*pbMidiDevice_LastOpenCrashed)) {
		// Previously the application crashed using these settings. We dare not try again
		// Lasse, should we load default settings here? - like this:
		//SKSPrefs_1 prefsTemp; // new struct with default settings
		//mPrefs_1 = prefsTemp;
		// .. Lasse
		return false;
	}

	return OpenAudioDevice_Output(
		mPrefs_1.iAudioManagerIx, mPrefs_1.iAudioDeviceIx, mPrefs_1.pszAudioDevName,
		mPrefs_1.iAudioSampleRate, mPrefs_1.iAudioBufferSize,
		false
	);
} // OpenAudioDevice_FromPrefs_Output


tbool CDSP::OpenAudioDevice_FromPrefs_Input(tbool* pbAudioDevice_LastOpenCrashed)
{
	LoadPrefs();
	*pbAudioDevice_LastOpenCrashed = (mPrefs_2.cbInputDevice_LastOpenCrashed != 0);
	if (*pbAudioDevice_LastOpenCrashed) {
		// Previously the application crashed using these settings. We dare not try again
		// Lasse, should we load default settings here? - like this:
		//CKSPrefs_2 prefsTemp; // new struct with default settings
		//mPrefs_2 = prefsTemp;
		// .. Lasse
		return false;
	}
	
	return OpenAudioDevice_Input(
						   mPrefs_2.iInputManagerIx, mPrefs_2.iInputDeviceIx, mPrefs_2.pszInputDevName,
						   /*mPrefs_1.iAudioSampleRate,*/ mPrefs_2.iInputBufferSize,
						   false
						   );
} // OpenAudioDevice_FromPrefs_Input


tbool CDSP::OpenAudioDevice_Output(tint32 iMgrIx, tint32 iDevIx, tchar* pszDevName, tint32 iFreqHz, tint32 iBuffSize, tbool bInvokeStart)
{
	// First of all see if we should be running as duplex
	tbool bIsNewDuplex = IsAudioDeviceDuplex(-1, -1, iMgrIx, iDevIx);
	if (bIsNewDuplex) {
		return OpenAudioDevice_Duplex(iMgrIx, iDevIx, pszDevName, iFreqHz, iBuffSize, bInvokeStart);
	}
	// We should be running output only - did we duplex before?
	tbool bIsPreviousDuplex = IsAudioDeviceDuplex();

	// ---

	tbool bSuccess = false;
	tbool bError = false;

	// Try to open new
	if ((iMgrIx >= 0) && (iMgrIx < NUMBER_OF_AUDIO_MANAGERS)) {
		aioe::IDeviceManager* pMgr = mapAudioManager[iMgrIx];

		std::list<tint32> listMatchingNames;
		if ((pszDevName) && (pszDevName[0])) {
			tbool bMatch = false;

			// Find all matching names
			for (tint32 iDevIx_Read = -1; iDevIx_Read < pMgr->GetNrOfDevices(); iDevIx_Read++) {
				tchar pszDevName_Read[1024];
				pszDevName_Read[0] = '\0';
				pMgr->GetDeviceName_OutputsOnly(iDevIx_Read, pszDevName_Read);
				if (pszDevName_Read[0] != '\0') {
					if (stricmp(pszDevName_Read, pszDevName) == 0) {
						listMatchingNames.insert(listMatchingNames.end(), iDevIx_Read);
						if (iDevIx_Read == iDevIx) {
							bMatch = true;
						}
					}
				}
			}

			// If name didn't match - try to correct
			if (!bMatch) {
				if (listMatchingNames.size() == 0) {
					// No names from list matches!
					bError = true;
				}
				else if (listMatchingNames.size() > 1) {
					// More than one name from list matches!
					// We can't guess what to do
					bError = true;
				}
				else {
					// Device has moved in list - correct it
					iDevIx = listMatchingNames.front();
				}
			}
		}

		if (!bError) {
			// Same device?
			if (
				(mpAudio_Output)
				&&
				(iMgrIx == mPrefs_1.iAudioManagerIx) && (iDevIx == mPrefs_1.iAudioDeviceIx)
				&&
				(
					(iFreqHz == mPrefs_1.iAudioSampleRate)
					||
					((iFreqHz <= 0) && (mPrefs_1.iAudioSampleRate > 0))
				)
				&&
				(
					(iBuffSize == mPrefs_1.iAudioBufferSize)
					||
					((iBuffSize < 0) && (mPrefs_1.iAudioBufferSize > 0))
				)
			) {
				// Same device, same settings - do nothing
				bSuccess = true;
				// Still save settings - may be missing file
				pMgr->GetDeviceName_OutputsOnly(iDevIx, mPrefs_1.pszAudioDevName);
				SavePrefs();
			}
			else {
				// Different device or settings - close old device and open new

				// Only go ahead if we're sane
				if (mpAudioCallback_Output) {

					// Close previous audio device
					if (bIsPreviousDuplex) {
						CloseAudioDevice_Duplex();
					}
					CloseAudioDevice_Output();
					// Re-load settings for closed device (so we don't trash setup)
					LoadPrefs();

					// Save the settings we're about to try
					mPrefs_1.iAudioManagerIx = iMgrIx;
					mPrefs_1.iAudioDeviceIx = iDevIx;
					mPrefs_1.iAudioSampleRate = iFreqHz;
					mPrefs_1.iAudioBufferSize = iBuffSize;
					pMgr->GetDeviceName_OutputsOnly(iDevIx, mPrefs_1.pszAudioDevName);
					// Be pessimistic about it
					mPrefs_1.cbAudioDevice_LastOpenCrashed = (tchar)true;
					mPrefs_1.cbAudioDevice_LastOpenSuccess = (tchar)false;
					SavePrefs();

					// Too early! Possibly -1 here, moved below
					/*
					// Maybe change speed in DSP engine
					SetSampleRate(mPrefs_1.iAudioSampleRate);
					*/

					// Try to open new device
					mpAudio_Output = pMgr->CreateDevice(iDevIx, false, true);
					if (mpAudio_Output) {
						mpAudio_Output->SetCallback(mpAudioCallback_Output);
						if (iBuffSize < 0) {
							// Use default buffer size
							iBuffSize = mpAudio_Output->GetPreferredBufferSize();
							mPrefs_1.iAudioBufferSize = iBuffSize;
						}
						if (iFreqHz < 0) {
							// Use default sample rate
							iFreqHz = mpAudio_Output->GetPreferredSampleRate();
							mPrefs_1.iAudioSampleRate = iFreqHz;
						}

						// Maybe change speed in DSP engine
						SetSampleRate(mPrefs_1.iAudioSampleRate);
						
						if (mpAudio_Output->Initialize(iFreqHz, iBuffSize)) {
							if ((!bInvokeStart) || (mpAudio_Output->Start())) {
								// Remember that we succeded
								mPrefs_1.cbAudioDevice_LastOpenSuccess = (tchar)true;
								bSuccess = true;
							}
						}
						if (!bSuccess) {
							// We failed Initialize or Start - kill audio object
							mpAudio_Output->Destroy();
							mpAudio_Output = NULL;
						}
					}

					// Remember that we didn't crash
					mPrefs_1.cbAudioDevice_LastOpenCrashed = (tchar)false;
					SavePrefs();
				}
				else {
					// Error opening device
					bError = true;
				}
			}
		}
	}

	if (bError) {
		bSuccess = false;
	}

	if ((!bSuccess) && (mpAudio_Output)) {
		mpAudio_Output->Destroy();
		mpAudio_Output = NULL;
	}

	if (bIsPreviousDuplex) {
		// Before this call input device was output too - now we must open it by itself
		tbool bDummy = false;
		if (!OpenAudioDevice_FromPrefs_Input(&bDummy)) {
			bSuccess = false;
		}
	}
	
	return bSuccess;
} // OpenAudioDevice_Output


tbool CDSP::OpenAudioDevice_Input(tint32 iMgrIx, tint32 iDevIx, tchar* pszDevName, tint32 iBuffSize, tbool bInvokeStart)
{
	// First of all see if we should be running as duplex
	tbool bIsNewDuplex = IsAudioDeviceDuplex(iMgrIx, iDevIx, -1, -1);
	if (bIsNewDuplex) {
		tint32 iFreqHz = mPrefs_1.iAudioSampleRate;
		return OpenAudioDevice_Duplex(iMgrIx, iDevIx, pszDevName, iFreqHz, iBuffSize, bInvokeStart);
	}
	// We should be running input only - did we duplex before?
	tbool bIsPreviousDuplex = IsAudioDeviceDuplex();

	// ---

	tbool bSuccess = false;
	tbool bError = false;
	
	// Try to open new
	if ((iMgrIx >= 0) && (iMgrIx < NUMBER_OF_AUDIO_MANAGERS)) {
		aioe::IDeviceManager* pMgr = mapAudioManager[iMgrIx];
		
		std::list<tint32> listMatchingNames;
		if ((pszDevName) && (pszDevName[0])) {
			tbool bMatch = false;
			
			// Find all matching names
			for (tint32 iDevIx_Read = -1; iDevIx_Read < pMgr->GetNrOfDevices(); iDevIx_Read++) {
				tchar pszDevName_Read[1024];
				pszDevName_Read[0] = '\0';
				pMgr->GetDeviceName_InputsOnly(iDevIx_Read, pszDevName_Read);
				if (pszDevName_Read[0] != '\0') {
					if (stricmp(pszDevName_Read, pszDevName) == 0) {
						listMatchingNames.insert(listMatchingNames.end(), iDevIx_Read);
						if (iDevIx_Read == iDevIx) {
							bMatch = true;
						}
					}
				}
			}
			
			// If name didn't match - try to correct
			if (!bMatch) {
				if (listMatchingNames.size() == 0) {
					// No names from list matches!
					bError = true;
				}
				else if (listMatchingNames.size() > 1) {
					// More than one name from list matches!
					// We can't guess what to do
					bError = true;
				}
				else {
					// Device has moved in list - correct it
					iDevIx = listMatchingNames.front();
				}
			}
		}
		
		if (!bError) {
			// Same device?
/*			if (
				(mpAudio_Input)
				&&
				(iMgrIx == mPrefs_2.iInputManagerIx) && (iDevIx == mPrefs_2.iInputDeviceIx)
				&&
				(
				 (iFreqHz == mPrefs_2.iAudioSampleRate)
				 ||
				 ((iFreqHz <= 0) && (mPrefs_2.iAudioSampleRate > 0))
				 )
				&&
				(
				 (iBuffSize == mPrefs_2.iInputBufferSize)
				 ||
				 ((iBuffSize < 0) && (mPrefs_2.iInputBufferSize > 0))
				 )
				) {
				// Same device, same settings - do nothing
				bSuccess = true;
				// Still save settings - may be missing file
				pMgr->GetDeviceName(iDevIx, mPrefs_2.pszInputDevName);
				SavePrefs();
			}
			else {*/
				// Different device or settings - close old device and open new
				
				// Only go ahead if we're sane
				if (mpAudioCallback_Input) {
					
					// Close previous audio device
					if (bIsPreviousDuplex) {
						CloseAudioDevice_Duplex();
					}
					CloseAudioDevice_Input();
					// Re-load settings for closed device (so we don't trash setup)
					LoadPrefs();
					
					// Save the settings we're about to try
					mPrefs_2.iInputManagerIx = iMgrIx;
					mPrefs_2.iInputDeviceIx = iDevIx;
					//mPrefs_1.iAudioSampleRate = iFreqHz;
					mPrefs_2.iInputBufferSize = iBuffSize;
					pMgr->GetDeviceName_InputsOnly(iDevIx, mPrefs_2.pszInputDevName);
					// Be pessimistic about it
					mPrefs_2.cbInputDevice_LastOpenCrashed = (tchar)true;
					mPrefs_2.cbInputDevice_LastOpenSuccess = (tchar)false;
					SavePrefs();
					
					// Try to open new device
					mpAudio_Input = pMgr->CreateDevice(iDevIx, true, false);
					if (mpAudio_Input) {
						mpAudio_Input->SetCallback(mpAudioCallback_Input);
						if (iBuffSize < 0) {
							// Use default buffer size
							iBuffSize = mpAudio_Input->GetPreferredBufferSize();
							//mPrefs_2.iInputBufferSize = iBuffSize;
						}
						//if (iFreqHz < 0) {
						//	// Use default sample rate
						//	iFreqHz = mpAudio_Input->GetPreferredSampleRate();
						//	mPrefs_2.iAudioSampleRate = iFreqHz;
						//}
						
						// Maybe change speed in DSP engine
						// - use same speed as output - SetSampleRate(mPrefs_1.iAudioSampleRate);
						
						if (mpAudio_Input->Initialize(mPrefs_1.iAudioSampleRate, iBuffSize)) {
							if ((!bInvokeStart) || (mpAudio_Input->Start())) {
								// Remember that we succeded
								mPrefs_2.cbInputDevice_LastOpenSuccess = (tchar)true;
								bSuccess = true;
							}
						}
						if (!bSuccess) {
							// We failed Initialize or Start - kill audio object
							mpAudio_Input->Destroy();
							mpAudio_Input = NULL;
						}
					}
					gpApplication->UpdateAudioGUI();
					
					// Remember that we didn't crash
					mPrefs_2.cbInputDevice_LastOpenCrashed = (tchar)false;
					SavePrefs();
				}
				else {
					// Error opening device
					bError = true;
				}
/*			}*/
		}
	}

	if (bError) {
		bSuccess = false;
	}
	
	if ((!bSuccess) && (mpAudio_Input)) {
		mpAudio_Input->Destroy();
		mpAudio_Input = NULL;
	}

	if (bIsPreviousDuplex) {
		// Before this call output device was input too - now we must open it by itself
		LoadPrefs();
		tbool bDummy = false;
		if (!OpenAudioDevice_FromPrefs_Output(&bDummy, &bDummy)) {
			bSuccess = false;
		}
	}
	
	return bSuccess;
} // OpenAudioDevice_Input


tbool CDSP::OpenAudioDevice_Duplex(tint32 iMgrIx, tint32 iDevIx, tchar* pszDevName, tint32 iFreqHz, tint32 iBuffSize, tbool bInvokeStart)
{
	// Did we duplex before?
	tbool bIsPreviousDuplex = IsAudioDeviceDuplex();

	// ---

	tbool bSuccess = false;
	tbool bError = false;

	// Try to open new
	if ((iMgrIx >= 0) && (iMgrIx < NUMBER_OF_AUDIO_MANAGERS)) {
		aioe::IDeviceManager* pMgr = mapAudioManager[iMgrIx];

		std::list<tint32> listMatchingNames;
		if ((pszDevName) && (pszDevName[0])) {
			tbool bMatch = false;

			// Find all matching names
			for (tint32 iDevIx_Read = -1; iDevIx_Read < pMgr->GetNrOfDevices(); iDevIx_Read++) {
				tchar pszDevName_Read[1024];
				pszDevName_Read[0] = '\0';
				pMgr->GetDeviceName(iDevIx_Read, pszDevName_Read);
				if (pszDevName_Read[0] != '\0') {
					if (stricmp(pszDevName_Read, pszDevName) == 0) {
						listMatchingNames.insert(listMatchingNames.end(), iDevIx_Read);
						if (iDevIx_Read == iDevIx) {
							bMatch = true;
						}
					}
				}
			}

			// If name didn't match - try to correct
			if (!bMatch) {
				if (listMatchingNames.size() == 0) {
					// No names from list matches!
					bError = true;
				}
				else if (listMatchingNames.size() > 1) {
					// More than one name from list matches!
					// We can't guess what to do
					bError = true;
				}
				else {
					// Device has moved in list - correct it
					iDevIx = listMatchingNames.front();
				}
			}
		}

		if (!bError) {
			// Same device?
			if (
				(mpAudio_Duplex)
				&&
				(iMgrIx == mPrefs_1.iAudioManagerIx) && (iDevIx == mPrefs_1.iAudioDeviceIx)
				&&
				(iMgrIx == mPrefs_2.iInputManagerIx) && (iDevIx == mPrefs_2.iInputDeviceIx)
				&&
				(
					(iFreqHz == mPrefs_1.iAudioSampleRate)
					||
					((iFreqHz <= 0) && (mPrefs_1.iAudioSampleRate > 0))
				)
				&&
				(
					(iBuffSize == mPrefs_1.iAudioBufferSize)
					||
					((iBuffSize < 0) && (mPrefs_1.iAudioBufferSize > 0))
				)
			) {
				// Same device, same settings - do nothing
				bSuccess = true;
				// Still save settings - may be missing file
				pMgr->GetDeviceName(iDevIx, mPrefs_1.pszAudioDevName);
				pMgr->GetDeviceName(iDevIx, mPrefs_2.pszInputDevName);
				SavePrefs();
			}
			else {
				// Different device or settings - close old device and open new

				// Only go ahead if we're sane
				if (mpAudioCallback_Duplex) {

					// Close previous audio device
					if (bIsPreviousDuplex) {
						CloseAudioDevice_Duplex();
					}
					CloseAudioDevice_Output();
					// Re-load settings for closed device (so we don't trash setup)
					LoadPrefs();

					// Save the settings we're about to try
					mPrefs_1.iAudioManagerIx = iMgrIx;
					mPrefs_1.iAudioDeviceIx = iDevIx;
					mPrefs_1.iAudioSampleRate = iFreqHz;
					mPrefs_1.iAudioBufferSize = iBuffSize;
					mPrefs_2.iInputManagerIx = iMgrIx;
					mPrefs_2.iInputDeviceIx = iDevIx;
					pMgr->GetDeviceName(iDevIx, mPrefs_1.pszAudioDevName);
					pMgr->GetDeviceName(iDevIx, mPrefs_2.pszInputDevName);
					// Be pessimistic about it
					mPrefs_1.cbAudioDevice_LastOpenCrashed = (tchar)true;
					mPrefs_1.cbAudioDevice_LastOpenSuccess = (tchar)false;
					mPrefs_2.cbInputDevice_LastOpenCrashed = (tchar)true;
					mPrefs_2.cbInputDevice_LastOpenSuccess = (tchar)false;
					SavePrefs();

					// Too early! Possibly -1 here, moved below
					/*
					// Maybe change speed in DSP engine
					SetSampleRate(mPrefs_1.iAudioSampleRate);
					*/

					// Try to open new device
					mpAudio_Duplex = pMgr->CreateDevice(iDevIx, true, true);
					if (mpAudio_Duplex) {
						mpAudio_Duplex->SetCallback(mpAudioCallback_Duplex);
						if (iBuffSize < 0) {
							// Use default buffer size
							iBuffSize = mpAudio_Duplex->GetPreferredBufferSize();
							mPrefs_1.iAudioBufferSize = iBuffSize;
							//mPrefs_2.iAudioBufferSize = -1;
						}
						if (iFreqHz < 0) {
							// Use default sample rate
							iFreqHz = mpAudio_Duplex->GetPreferredSampleRate();
							mPrefs_1.iAudioSampleRate = iFreqHz;
						}

						// Maybe change speed in DSP engine
						SetSampleRate(mPrefs_1.iAudioSampleRate);
						
						if (mpAudio_Duplex->Initialize(iFreqHz, iBuffSize)) {
							if ((!bInvokeStart) || (mpAudio_Duplex->Start())) {
								// Remember that we succeded
								tint32 iInputs = mpAudio_Duplex->GetInputChannels();
								tint32 iOutputs = mpAudio_Duplex->GetOutputChannels();
								mPrefs_1.cbAudioDevice_LastOpenSuccess = (tchar)(iOutputs > 0);
								mPrefs_2.cbInputDevice_LastOpenSuccess = (tchar)(iInputs > 0);
								bSuccess = (iOutputs > 0);
							}
						}
						if (!bSuccess) {
							// We failed Initialize or Start - kill audio object
							mpAudio_Duplex->Destroy();
							mpAudio_Duplex = NULL;
						}
					}

					// Remember that we didn't crash
					mPrefs_1.cbAudioDevice_LastOpenCrashed = (tchar)false;
					mPrefs_2.cbInputDevice_LastOpenCrashed = (tchar)false;
					SavePrefs();
				}
				else {
					// Error opening device
					bError = true;
				}
			}
		}
	}

	if (bError) {
		bSuccess = false;
	}

	if ((!bSuccess) && (mpAudio_Duplex)) {
		mpAudio_Duplex->Destroy();
		mpAudio_Duplex = NULL;
	}

	// Verify that the device is indeed duplex
	if (bSuccess) {
		tint32 iInputs = mpAudio_Duplex->GetInputChannels();
		tint32 iOutputs = mpAudio_Duplex->GetOutputChannels();
		bSuccess = ((iInputs > 0) && (iOutputs > 0));

		// If it's got no output channels we might as well unload it
		if (iOutputs <= 0) {
			mpAudio_Duplex->Destroy();
			mpAudio_Duplex = NULL;
		}
	}

	return bSuccess;
} // OpenAudioDevice_Duplex


void CDSP::StartAudioDevices()
{
	// Attempt start of full duplex audio device
	StartAudioDevice_Duplex();

	// Attempt start of separate output and input audio devices
	StartAudioDevice_Output();
	StartAudioDevice_Input();
} // StartAudioDevices


void CDSP::StartAudioDevice_Duplex()
{
	if (mpAudio_Duplex)
		mpAudio_Duplex->Start();
} // StartAudioDevice_Duplex


void CDSP::StartAudioDevice_Output()
{
	if (mpAudio_Output)
		mpAudio_Output->Start();
} // StartAudioDevice_Output


void CDSP::StartAudioDevice_Input()
{
	if (mpAudio_Input)
		mpAudio_Input->Start();
} // StartAudioDeviceInput


void CDSP::SetTrackSelection(tint32 iTrack, tint64 uiSelection_Pos, tint64 fDuration, tint32 uiSelection_Type, tint32 iRegionID)
{
	miSelectedTrack												=	iTrack;
	mpTrackSelectionInfo[iTrack].uiSelection_Pos				=	uiSelection_Pos;
	mpTrackSelectionInfo[iTrack].uiSelection_Duration			=	fDuration;
	mpTrackSelectionInfo[iTrack].uiSelection_Type				=	uiSelection_Type;
	mpTrackSelectionInfo[iTrack].iRegionID						=	iRegionID;
}

void CDSP::Deselect_All_Tracks()
{
	for(tint32 i=0; i<giNumber_Of_Tracks; i++){
		Deselect_Track(i);
	}
}

void CDSP::Deselect_Track(tuint32 uiTrack)
{

	mpTrackSelectionInfo[uiTrack].uiSelection_Type		=	giSelect_Off;
	mpTrackSelectionInfo[uiTrack].iRegionID	=	-1;

}

STrackSelectionInfo CDSP::SelectRegion(tint32 iRegionID)
{ 
	
	tuint32 uiID =		iRegionID;
	SRegionInfo RegionInfo;
	GetRegionInfo( RegionInfo, uiID);

	CRegion_DSP* pRegionSoundObject	=	mppTracks[RegionInfo.uiTrack]->GetRegion_DSP(uiID);
	//std::string sClipName				=	pRegionSoundObject->GetSoundListItemName();
	tint32 iChannel						=	RegionInfo.uiTrack;

	// Trim selection from start
	tuint64 uiStart_Sample				=	mppTracks[iChannel]->GetRegionPosOnTrack(uiID);	
	tuint64 uiDuration					=	(pRegionSoundObject ? pRegionSoundObject->GetDuration() : 0);
	
	tint32 iNewSelectType = giSelect_Region;
	if (ge::IWindow::ShiftPressed()) {
		STrackSelectionInfo sinfo = GetTrackSelection(iChannel);
		tint32 iOldType = sinfo.uiSelection_Type;
		if ((iOldType == giSelect_On_Track) || (iOldType == giSelect_Region)) {
			iNewSelectType = giSelect_On_Track;
			tuint64 uiOldStart = sinfo.uiSelection_Pos;
			if (uiOldStart < uiStart_Sample) {
				uiDuration += (uiStart_Sample - uiOldStart);
				uiStart_Sample = uiOldStart;
			}
			tuint64 uiOldDurat = sinfo.uiSelection_Duration;
			tuint64 uiOldFinal = uiOldStart + uiOldDurat - 1;
			tuint64 uiNewFinal = uiStart_Sample + uiDuration - 1;
			if (uiOldFinal > uiNewFinal) {
				uiDuration = uiOldFinal - uiStart_Sample + 1;
			}
		}
	}
	SetTrackSelection(iChannel,  uiStart_Sample,  uiDuration, iNewSelectType, uiID);
	
	return mpTrackSelectionInfo[iChannel];
}


void CDSP::CutRegion(tuint32 uiTrack, tuint32 uiRegionID, tuint64 uiCutPos)
{ 

	//--------------------------------------
	// Trim end
	//tuint32 uiID	=	iRegionID;
	SRegionInfo RegionInfo;
	GetRegionInfo( RegionInfo, uiRegionID);
	CRegion_DSP* pRegionSoundObject	=	mppTracks[RegionInfo.uiTrack]->GetRegion_DSP(uiRegionID);
	std::string sClipName				=	pRegionSoundObject->GetSoundListItemName();
	tfloat64 uiRegionTrackPosStart		=	mppTracks[RegionInfo.uiTrack]->GetRegionPosOnTrack(uiRegionID);	
//	tfloat64 fDuration					=	pRegionSoundObject->GetDuration();
	tuint64 uiSoundPos					=	pRegionSoundObject->GetSoundStartPos();
	tuint64 uiSampleEnd					=	pRegionSoundObject->GetDuration() + uiSoundPos - 1;
	
	
//	tuint64 uiFadeInLength				=	pRegionSoundObject->GetFadeInLength();
	tuint64 uiFadeOutLength				=	pRegionSoundObject->GetFadeOutLength();
	
	
	// Trim end of curent region
	mppTracks[RegionInfo.uiTrack]->TrimRegion(uiRegionID, uiRegionTrackPosStart, uiSoundPos, uiSoundPos + uiCutPos-1);
	mpRegionCallback->Refresh_Region_GUI(uiRegionID, uiTrack);

	// Create new region
	tuint32 uiTrack_Pos					=	uiRegionTrackPosStart	+ uiCutPos;
	tuint64 uiSampleStart				=	uiSoundPos				+ uiCutPos;
	tuint64 uiSampleDuration			=	uiSampleStart			-	uiSampleEnd	+1;	
	if ( uiFadeOutLength > uiSampleDuration)
		uiFadeOutLength = uiSampleDuration;
	uiRegionID = CreateRegion(sClipName, uiTrack, uiTrack_Pos , uiSampleStart, uiSampleEnd, 0, uiFadeOutLength );

}

void CDSP::TrimRegion(tuint32 uiTrack, tuint32 uiRegionID, tbool bStart, tint64 iSamplePos)
{ 

	
	SRegionInfo RegionInfo;
	GetRegionInfo( RegionInfo, uiRegionID);
	CRegion_DSP* pRegion				=	mppTracks[RegionInfo.uiTrack]->GetRegion_DSP(uiRegionID);
	std::string sClipName				=	pRegion->GetSoundListItemName();
	tint64 iRegionTrackPosStart			=	mppTracks[RegionInfo.uiTrack]->GetRegionPosOnTrack(uiRegionID);	
	tint64 iDuration					=	pRegion->GetDuration();
	tint64 iSoundPos					=	pRegion->GetSoundStartPos();
	tint64 iSampleEnd					=	pRegion->GetDuration() + iSoundPos - 1;
	tint64 iSoundDuration				=	pRegion->GetSound(0)->GetLength();

	//--------------------------------------
	// Trim start
	if(bStart){
	
		if( iSoundPos + iSamplePos < 0){
			tint32 iCorection = iSoundPos + iSamplePos;
			iSamplePos -= iCorection ;
		}

		tint64 iClipSize	=	iSamplePos;
		
		if( iClipSize + iSoundPos > iSampleEnd -256)
			iClipSize	=	iSampleEnd -256 -iSoundPos;
		
		iRegionTrackPosStart	+=	iClipSize;
		iSoundPos				+=	iClipSize;
		
		
		mppTracks[RegionInfo.uiTrack]->TrimRegion(uiRegionID, iRegionTrackPosStart, iSoundPos, iSampleEnd);
		mpRegionCallback->Refresh_Region_GUI(uiRegionID, uiTrack);
	}
	//--------------------------------------
	// Trim end
	else{
	
		tint64 iClipSize	=	iDuration - iSamplePos;
		
		iSampleEnd	-=	iClipSize;
		
		if( iSampleEnd > iSoundDuration)
			iSampleEnd = iSoundDuration;
			
		if(iSampleEnd < iSoundPos + 256)
			iSampleEnd = iSoundPos + 256;
		
		mppTracks[RegionInfo.uiTrack]->TrimRegion(uiRegionID, iRegionTrackPosStart, iSoundPos, iSampleEnd);
		mpRegionCallback->Refresh_Region_GUI(uiRegionID, uiTrack);
	}

}

void CDSP::DuplicateRegion()
{ 
	
	for(tint32 uiTrack=0; uiTrack<giNumber_Of_Tracks; uiTrack++){
		
		
		if(mpTrackSelectionInfo[uiTrack].uiSelection_Type = giSelect_Region &&  mpTrackSelectionInfo[uiTrack].iRegionID != -1)
		{
			tuint32 uiRegionID = mpTrackSelectionInfo[uiTrack].iRegionID ;
			
			SRegionInfo RegionInfo;
			GetRegionInfo( RegionInfo, uiRegionID);
			CRegion_DSP* pRegionSoundObject	=	mppTracks[RegionInfo.uiTrack]->GetRegion_DSP(uiRegionID);
			std::string sClipName				=	pRegionSoundObject->GetSoundListItemName();
			tfloat64 uiRegionTrackPosStart		=	mppTracks[RegionInfo.uiTrack]->GetRegionPosOnTrack(uiRegionID);	
			tfloat64 fDuration					=	pRegionSoundObject->GetDuration();
			tuint64 uiSoundStartPos				=	pRegionSoundObject->GetSoundStartPos();
			tuint64 uiSamplePosEnd				=	pRegionSoundObject->GetEndPos();
			
			// Create new region
			tuint32 uiTrack_Pos					=	uiRegionTrackPosStart + fDuration -1;
			tuint32 muiRegionID = CreateRegion(sClipName, uiTrack, uiTrack_Pos , uiSoundStartPos, uiSamplePosEnd);
			SelectRegion(muiRegionID);
			mpRegionCallback->SelectRegion(muiRegionID,uiTrack);
			return;
		}
	}
}

void CDSP::NormaliseRegion()
{ 
	
	
}

void CDSP::InverseRegion()
{ 
	
	
}

const tchar* CDSP::GetClipNameOfSelectedRegion()
{
	for(tint32 uiTrack=0; uiTrack<giNumber_Of_Tracks; uiTrack++){
		if(mpTrackSelectionInfo[uiTrack].uiSelection_Type = giSelect_Region &&  mpTrackSelectionInfo[uiTrack].iRegionID != -1)
		{
			tuint32 uiRegionID = mpTrackSelectionInfo[uiTrack].iRegionID ;
			
			SRegionInfo RegionInfo;
			GetRegionInfo( RegionInfo, uiRegionID);
			CRegion_DSP* pRegionSoundObject	=	mppTracks[RegionInfo.uiTrack]->GetRegion_DSP(uiRegionID);
			return pRegionSoundObject->GetSoundListItemName();
		}
	}
	return "";
} // GetClipNameOfSelectedRegion



tuint64 CDSP::Fade_In(tuint32 uiRegionID, tuint64 uiFadeInLength)
{ 

	SRegionInfo RegionInfo;
	GetRegionInfo( RegionInfo, uiRegionID);
	CRegion_DSP* pRegion	=	mppTracks[RegionInfo.uiTrack]->GetRegion_DSP(uiRegionID);
	
	
	tuint64 uiSoundDuraion				=	pRegion->GetDuration();
	tuint64 uiFadeOutLength				=	pRegion->GetFadeOutLength();
	
	if(uiFadeInLength > uiSoundDuraion)
		uiFadeInLength = uiSoundDuraion;
		
	if( uiFadeInLength + uiFadeOutLength > uiSoundDuraion){
		uiFadeOutLength = uiSoundDuraion - uiFadeInLength;
		pRegion->SetFadeOutLength(uiFadeOutLength);
	}
		
	
	pRegion->SetFadeInLength(uiFadeInLength);
	return uiFadeInLength;
	
}

tuint64 CDSP::Fade_Out(tuint32 uiRegionID, tuint64 uiFadeOutLength)
{ 
	
	SRegionInfo RegionInfo;
	GetRegionInfo( RegionInfo, uiRegionID);
	CRegion_DSP* pRegionSoundObject	=	mppTracks[RegionInfo.uiTrack]->GetRegion_DSP(uiRegionID);
	
	
	tuint64 uiSoundDuraion				=	pRegionSoundObject->GetDuration();
	tuint64 uiFadeInLength				=	pRegionSoundObject->GetFadeInLength();
	
	if(uiFadeOutLength > uiSoundDuraion)
		uiFadeOutLength = uiSoundDuraion;
	
	if(uiFadeOutLength + uiFadeInLength > uiSoundDuraion){
		uiFadeInLength = uiSoundDuraion - uiFadeOutLength;
		pRegionSoundObject->SetFadeInLength(uiFadeInLength);
	}
	
	pRegionSoundObject->SetFadeOutLength(uiFadeOutLength);
	
	return uiFadeOutLength;
}

/*
void CDSP::Region_Volume(tuint32 uiRegionID, tfloat32 fRegion_Volume)
{ 
	
	SRegionInfo RegionInfo;
	GetRegionInfo( RegionInfo, uiRegionID);
	CRegion_DSP* pRegion	=	mppTracks[RegionInfo.uiTrack]->GetRegion_DSP(uiRegionID);
	pRegion->SetRegionVolume(fRegion_Volume);	
	
}
*/

void CDSP::LoopSelection()
{ 

	if( miSelectedTrack != -1){
	
		tuint64 uiSelection_Pos			= mpTrackSelectionInfo[miSelectedTrack].uiSelection_Pos;
		tuint64 uiSelection_Duration	= mpTrackSelectionInfo[miSelectedTrack].uiSelection_Duration;
		
		muiLoopStart					=	uiSelection_Pos;
		muiStored_LoopEnd				=	uiSelection_Duration + uiSelection_Pos + 1;
		
		mLoop_Drawing_Info.uiLoop_Start = muiLoopStart;
		mLoop_Drawing_Info.uiLoop_End = muiStored_LoopEnd;
		
		Update_Loop();
	}
	
}


void CDSP::CloseAudioDevices()
{
	CloseAudioDevice_Duplex();
	CloseAudioDevice_Input();
	CloseAudioDevice_Output();
} // CloseAudioDevices


void CDSP::CloseAudioDevice_Output()
{
	if (mpAudio_Output) {
		mpAudio_Output->Destroy();
		mpAudio_Output = NULL;

		mPrefs_1.iAudioManagerIx = -1;
		mPrefs_1.iAudioDeviceIx = -1;
		mPrefs_1.pszAudioDevName[0] = '\0';
		mPrefs_1.iAudioSampleRate = 44100;
		mPrefs_1.iAudioBufferSize = 0;
	}
} // CloseAudioDevice_Output


void CDSP::CloseAudioDevice_Input()
{
	if (mpAudio_Input) {
		mpAudio_Input->Destroy();
		mpAudio_Input = NULL;

		mPrefs_2.iInputDeviceIx = -1;
		mPrefs_2.pszInputDevName[0] = '\0';
	}
} // CloseAudioDevice_Input


void CDSP::CloseAudioDevice_Duplex()
{
	if (mpAudio_Duplex) {
		mpAudio_Duplex->Destroy();
		mpAudio_Duplex = NULL;

		mPrefs_1.iAudioManagerIx = -1;
		mPrefs_1.iAudioDeviceIx = -1;
		mPrefs_1.pszAudioDevName[0] = '\0';
		mPrefs_1.iAudioSampleRate = 44100;
		mPrefs_1.iAudioBufferSize = 0;

		mPrefs_2.iInputDeviceIx = -1;
		mPrefs_2.pszInputDevName[0] = '\0';
	}
} // CloseAudioDevice_Duplex


tint32 CDSP::GetAudioDeviceMenuItems(ge::IPopupMenu::SMenuItemList& rlist, tint32& riSelectedValue)
{
	rlist.iItemCount = 0;

	// Fill in audio devices
	tint32 iListValue = 0;
	for (tint32 iMgr = 0; iMgr < NUMBER_OF_AUDIO_MANAGERS; iMgr++) {
		if (mapAudioManager[iMgr]) {
			for (tint32 i = -1; i < mapAudioManager[iMgr]->GetNrOfDevices(); i++) {
				tchar pszDevName[1024];
				pszDevName[0] = '\0';
				mapAudioManager[iMgr]->GetDeviceName_OutputsOnly(i, pszDevName);
				if (pszDevName[0]) {
					ge::IPopupMenu::SMenuItem& item = rlist.pItems[rlist.iItemCount++];
					item = ge::IPopupMenu::SMenuItem(pszDevName, iListValue, NULL);





				}
				if ((iMgr == mPrefs_1.iAudioManagerIx) && (i == mPrefs_1.iAudioDeviceIx)) {
					riSelectedValue = iListValue;
				}
				iListValue++;
			}
		}
	}

	return rlist.iItemCount;
} // GetAudioDeviceMenuItems


tint32 CDSP::GetAudioDeviceMenuItemsInput(ge::IPopupMenu::SMenuItemList& rlist, tint32& riSelectedValue)
{
	rlist.iItemCount = 0;
	
	// Fill in audio devices
	tint32 iListValue = 0;
	for (tint32 iMgr = 0; iMgr < NUMBER_OF_AUDIO_MANAGERS; iMgr++) {
		if (mapAudioManager[iMgr]) {
			for (tint32 i = -1; i < mapAudioManager[iMgr]->GetNrOfDevices(); i++) {
				tchar pszDevName[1024];
				pszDevName[0] = '\0';
				mapAudioManager[iMgr]->GetDeviceName_InputsOnly(i, pszDevName);
				if (pszDevName[0]) {
					ge::IPopupMenu::SMenuItem& item = rlist.pItems[rlist.iItemCount++];
					item = ge::IPopupMenu::SMenuItem(pszDevName, iListValue, NULL);
				}
				if ((iMgr == mPrefs_2.iInputManagerIx) && (i == mPrefs_2.iInputDeviceIx)) {
					riSelectedValue = iListValue;
				}
				iListValue++;
			}
		}
	}
	
	return rlist.iItemCount;
} // GetAudioDeviceMenuItemsInput


tint32 CDSP::GetAudioFreqsMenuItems(ge::IPopupMenu::SMenuItemList& rlist, tint32& riSelectedValue)
{
	rlist.iItemCount = 0;

	// Fill in possible sample rates
	if (mpAudio_Duplex) {
		// Only one duplex audio device - just enumerate it's values

		tint32 iFreq = 0;
		do {
			iFreq = (tint32)mpAudio_Duplex->EnumeratePossibleSampleRates(iFreq);
			if (iFreq > 0) {
				tchar psz[1024];
				sprintf(psz, "%d", iFreq);
				ge::IPopupMenu::SMenuItem& item = rlist.pItems[rlist.iItemCount++];
				item = ge::IPopupMenu::SMenuItem(psz, iFreq / 100, NULL);
			}
		} while (iFreq > 0);
	}
	else if (mpAudio_Output) {
		// Two separate audio devices for output and input - find out which frequencies fit

		tint32 iFreqOut = 0;
		tint32 iFreqIn = -1;
		do {
			iFreqOut = (tint32)mpAudio_Output->EnumeratePossibleSampleRates(iFreqOut);
			if (iFreqOut > 0) {
				if (mpAudio_Input) {
					while ((iFreqIn < iFreqOut) && (iFreqIn != 0)) {
						// See if next input frequency fits
						if (iFreqIn < 0) iFreqIn = 0;
						iFreqIn = (tint32)mpAudio_Output->EnumeratePossibleSampleRates(iFreqIn);
					}
				}
				if ((mpAudio_Input == NULL) || (iFreqOut == iFreqIn)) {
					tchar psz[1024];
					sprintf(psz, "%d", iFreqOut);
					ge::IPopupMenu::SMenuItem& item = rlist.pItems[rlist.iItemCount++];
					item = ge::IPopupMenu::SMenuItem(psz, iFreqOut / 100, NULL);
				}
			}
		} while (iFreqOut > 0);
	}

	riSelectedValue = mPrefs_1.iAudioSampleRate / 100;
	return rlist.iItemCount;
}

tint32 CDSP::GetAudioBuffsMenuItems(ge::IPopupMenu::SMenuItemList& rlist, tint32& riSelectedValue)
{
	rlist.iItemCount = 0;

	aioe::IDevice* pAudio = NULL;
	if (mpAudio_Duplex) pAudio = mpAudio_Duplex;
	if (mpAudio_Output) pAudio = mpAudio_Output;

	// Fill in possible buffer sizes
	if (pAudio) {
		// Make sure min buff is at least 32 frames and an even power of two (32, 64, 128, 256, etc..)
		tint32 iBuffSizeMin = pAudio->GetMinBufferSize();
		tint32 iPow2 = 32;
		while (iPow2 < iBuffSizeMin)
			iPow2 *= 2;
		if (iPow2 > iBuffSizeMin)
			iBuffSizeMin = iPow2;
		
		tint32 iBuffSizeMax = pAudio->GetMaxBufferSize();

		tint32 iBuffSizeLargestJump = iBuffSizeMax;
		if (iBuffSizeMax > iBuffSizeMin * 8) {
			iBuffSizeLargestJump = iBuffSizeMax / 8;
		}
		else if (iBuffSizeMax > iBuffSizeMin * 4) {
			iBuffSizeLargestJump = iBuffSizeMax / 4;
		}

		for (tint32 iBuffSize = iBuffSizeMin; iBuffSize <= iBuffSizeMax; ) {
			tchar pszBuffSize[1024];
			sprintf(pszBuffSize, "%d", iBuffSize);
			ge::IPopupMenu::SMenuItem& item = rlist.pItems[rlist.iItemCount++];
			item = ge::IPopupMenu::SMenuItem(pszBuffSize, iBuffSize / 32, NULL);
			if (iBuffSize < iBuffSizeLargestJump) {
				// At low sizes we jump in powers of two
				iBuffSize *= 2;
			}
			else {
				// At higher sizes we only jump smaller amount
				iBuffSize += iBuffSizeLargestJump;
			}
		}
	}

	riSelectedValue = mPrefs_1.iAudioBufferSize / 32;
	return rlist.iItemCount;
}

tbool CDSP::OpenAudioDevice_FromMenu_Output(tint32 iListValue, tint32 iFreqHz, tint32 iBuffSize)
{
	tbool bSuccess = false;

	tint32 iRelIndex = iListValue;
	tbool bFound = false;
	for (tint iMgr = 0; (!bFound) && (iMgr < NUMBER_OF_AUDIO_MANAGERS); iMgr++) {
		aioe::IDeviceManager* pMgr = mapAudioManager[iMgr];
		if (iRelIndex <= pMgr->GetNrOfDevices()) {
			// The selected audio device belongs to this audio manager
			bFound = true;
			// Allow for -1 = default device
			tint32 iAudioDevIndex = iRelIndex - 1;
			// Read name (for comparison - test of functionality, could be NULL..)
			tchar pszDevName[1024];
			pszDevName[0] = '\0';
			pMgr->GetDeviceName(iAudioDevIndex, pszDevName);
			// Create device
			bSuccess = OpenAudioDevice_Output(iMgr, iAudioDevIndex, pszDevName, iFreqHz, iBuffSize, true);
		}
		else {
			// The selection doesn't belong to this audio manager
			tint32 iSkip = pMgr->GetNrOfDevices() + 1;
			iRelIndex -= iSkip;
		}
	}

	return bSuccess;
} // OpenAudioDevice_FromMenu_Output


tbool CDSP::OpenAudioDevice_FromMenu_Input(tint32 iListValue, /*tint32 iFreqHz,*/ tint32 iBuffSize)
{
	tbool bSuccess = false;
	
	tint32 iRelIndex = iListValue;
	tbool bFound = false;
	for (tint iMgr = 0; (!bFound) && (iMgr < NUMBER_OF_AUDIO_MANAGERS); iMgr++) {
		aioe::IDeviceManager* pMgr = mapAudioManager[iMgr];
		if (iRelIndex <= pMgr->GetNrOfDevices()) {
			// The selected audio device belongs to this audio manager
			bFound = true;
			// Allow for -1 = default device
			tint32 iAudioDevIndex = iRelIndex - 1;
			// Read name (for comparison - test of functionality, could be NULL..)
			tchar pszDevName[1024];
			pszDevName[0] = '\0';
			pMgr->GetDeviceName(iAudioDevIndex, pszDevName);
			// Create device
			bSuccess = OpenAudioDevice_Input(iMgr, iAudioDevIndex, pszDevName, /*iFreqHz,*/ iBuffSize, true);
		}
		else {
			// The selection doesn't belong to this audio manager
			tint32 iSkip = pMgr->GetNrOfDevices() + 1;
			iRelIndex -= iSkip;
		}
	}
	
	return bSuccess;
} // OpenAudioDevice_FromMenu_Input


void CDSP::ClearAllMeters()
{
	mpsMeters_All->Zero();
} // ClearAllMeters


void CDSP::GetAllMeters_MaybeClear(SMeters_All* psMeters_All, tbool bClear)
{
	// Get meter arrays for all possible channels (max 7.1 surround)

	// Master
	memcpy(
		(tchar*)(psMeters_All->pfMeters_Master),
		(tchar*)(mpsMeters_All->pfMeters_Master),
		8 * sizeof(tfloat32)
		);

	// Tracks
	for (tint32 iTrack = 0; iTrack < giNumber_Of_Tracks; iTrack++) {
		memcpy(
			(tchar*)(psMeters_All->ppfMeters_Tracks[iTrack]),
			(tchar*)(mpsMeters_All->ppfMeters_Tracks[iTrack]),
			8 * sizeof(tfloat32)
			);
	}

	// Busses
	for (tint32 iBus = 0; iBus < giNumber_Of_Busses; iBus++) {
		memcpy(
			(tchar*)(psMeters_All->ppfMeters_Busses[iBus]),
			(tchar*)(mpsMeters_All->ppfMeters_Busses[iBus]),
			8 * sizeof(tfloat32)
			);
	}

	// Built-in AUXes
	for (tint32 iAUX = 0; iAUX < giNumber_Of_AUXes; iAUX++) {
		memcpy(
			(tchar*)(psMeters_All->ppfMeters_AUXes[iAUX]),
			(tchar*)(mpsMeters_All->ppfMeters_AUXes[iAUX]),
			8 * sizeof(tfloat32)
			);
	}

	// Clear arrays?
	if (bClear)
		ClearAllMeters();
}

void CDSP::SetMeters_Master(tfloat32* pfAbsValues)
{
	// Set all (max 7.1 surround) channels for track
	tfloat32* pfAbsCurr = mpsMeters_All->pfMeters_Master;
	tfloat32* pfAbsNew = pfAbsValues;
	for (tint32 iChannelIx = 0; iChannelIx < 8; iChannelIx++) {
		if (*pfAbsNew > *pfAbsCurr)
			*pfAbsCurr = *pfAbsNew;
		pfAbsCurr++;
		pfAbsNew++;
	}
}


void CDSP::SetMeters_Track(tint32 iTrack, tfloat32* pfAbsValues)
{
	// Set all (max 7.1 surround) channels for track
	tfloat32* pfAbsCurr = mpsMeters_All->ppfMeters_Tracks[iTrack];
	tfloat32* pfAbsNew = pfAbsValues;
	for (tint32 iChannelIx = 0; iChannelIx < 8; iChannelIx++) {
		if (*pfAbsNew > *pfAbsCurr)
			*pfAbsCurr = *pfAbsNew;
		pfAbsCurr++;
		pfAbsNew++;
	}
}


void CDSP::SetMeters_Bus(tint32 iBus, tfloat32* pfAbsValues)
{
	// Set all (max 7.1 surround) channels for bus
	tfloat32* pfAbsCurr = mpsMeters_All->ppfMeters_Busses[iBus];
	tfloat32* pfAbsNew = pfAbsValues;
	for (tint32 iChannelIx = 0; iChannelIx < 8; iChannelIx++) {
		if (*pfAbsNew > *pfAbsCurr)
			*pfAbsCurr = *pfAbsNew;
		pfAbsCurr++;
		pfAbsNew++;
	}
}


void CDSP::SetMeters_AUX(tint32 iAUX, tfloat32* pfAbsValues)
{
	// Set all (max 7.1 surround) channels for AUX
	tfloat32* pfAbsCurr = mpsMeters_All->ppfMeters_AUXes[iAUX];
	tfloat32* pfAbsNew = pfAbsValues;
	for (tint32 iChannelIx = 0; iChannelIx < 8; iChannelIx++) {
		if (*pfAbsNew > *pfAbsCurr)
			*pfAbsCurr = *pfAbsNew;
		pfAbsCurr++;
		pfAbsNew++;
	}
}

void CDSP::Loop_On(tbool bLoop)
{
	mbLoop_On	= bLoop;
	Update_Loop();
	
	mLoop_Drawing_Info.bLoop = bLoop;
}

void CDSP::Set_Loop_Start(tuint64 uiStart)
{
	tuint64	uiTemp	=	uiStart/32;
	uiTemp			*=	32;
	muiLoopStart = uiTemp;
	Update_Loop();
	
	mLoop_Drawing_Info.uiLoop_Start = muiLoopStart;
}
	
void CDSP::Set_Loop_End(tuint64 uiEnd)
{
	tuint64	uiTemp	=	uiEnd/32;
	uiTemp			*=	32;
	muiStored_LoopEnd = uiTemp;
	Update_Loop();
	
	mLoop_Drawing_Info.uiLoop_End = muiStored_LoopEnd;
}

void CDSP::Set_Loop_To_Selection()
{


	tuint64 uiPos		=	mpTrackSelectionInfo[miSelectedTrack].uiSelection_Pos;
	tuint64 uiDuration	=	mpTrackSelectionInfo[miSelectedTrack].uiSelection_Duration;
	tuint64 uiEnd		=	uiPos + uiDuration - 1;

	tuint64	uiTemp	=	uiPos/32;
	uiTemp			*=	32;
	gpApplication->SetGlobalParm(giParamID_Loop_Start,uiTemp, giSectionGlobal);
	

	uiTemp	=	uiEnd/32;
	uiTemp			*=	32;
	gpApplication->SetGlobalParm(giParamID_Loop_End,uiTemp, giSectionGlobal);

	
}

void CDSP::Update_Loop()
{
	if(mbLoop_On){
		muiLoopEnd	= muiStored_LoopEnd;
	}
	else{
		muiLoopEnd = 0x7FFFFFFF;
	}
	if(muiLoopEnd == 0)
		muiLoopEnd	=	0x7FFFFFFF;
}

tint32 CDSP::GetNrOfInputChannels()
{
	if (mpAudio_Input) {
		return mpAudio_Input->GetInputChannels();
	}

	if (mpAudio_Duplex) {
		return mpAudio_Duplex->GetInputChannels();
	}

	return 0;
} // GetNrOfInputChannels




