

#include "SToolsInternalOS.h"

#ifdef _DEBUG
//#define __FILETEST
#else
#endif

#ifdef __FILETEST
#define DEBUGFILE_OUT(a, b) _write(a, (void*)b, strlen(b))
#define DEBUGFILE_OUT_FORMAT1(a, b, c) {char psz[64];\
	sprintf((char*)psz, b, c);\
	DEBUGFILE_OUT(a, psz);}
#define DEBUGFILE_OUT_FORMAT2(a, b, c, d) {char psz[64];\
	sprintf((char*)psz, b, c, d);\
	DEBUGFILE_OUT(a, psz);}
#define DEBUGFILE_OUT_FORMAT3(a, b, c, d, e) {char psz[64];\
	sprintf((char*)psz, b, c, d, e);\
	DEBUGFILE_OUT(a, psz);}
#define DEBUGFILE_OUT_FORMAT4(a, b, c, d, e, f) {char psz[64];\
	sprintf((char*)psz, b, c, d, e, f);\
	DEBUGFILE_OUT(a, psz);}
#define DEBUGFILE_OUT_FORMAT5(a, b, c, d, e, f, g) {char psz[64];\
	sprintf((char*)psz, b, c, d, e, f, g);\
	DEBUGFILE_OUT(a, psz);}
#else
#define DEBUGFILE_OUT(a, b)
#define DEBUGFILE_OUT_FORMAT1(a, b, c)
#define DEBUGFILE_OUT_FORMAT2(a, b, c, d)
#define DEBUGFILE_OUT_FORMAT3(a, b, c, d, e)
#define DEBUGFILE_OUT_FORMAT4(a, b, c, d, e, f)
#define DEBUGFILE_OUT_FORMAT5(a, b, c, d, e, f, g)
#endif



const long glAlignment = 16;
const long glAlignmentSamples = glAlignment / sizeof(tfloat32);

#ifdef __FILETEST
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h> 
int iFile = -1;
int iFile2 = -1;
#endif


IVoiceManager* IVoiceManager::Create()
{
	return dynamic_cast<IVoiceManager*>(new CVoiceManager());
}

void CVoiceManager::Destroy()
{
	delete dynamic_cast<CVoiceManager*>(this);
}


CVoiceManager::CVoiceManager()
{
	mfFadeOutTimeMS = 2.0f;

	mpDSP = IDSPTools::Create();

	mpAlignedMemAllocator = IMemAllocator::Create(glAlignment);

	mlMaxBufferSize = 0;
	mlMaxSubBufferSize = 0;

	mbHQMode = false;
	mbHQModeActive = false;

	mpfTmpOutL = NULL;
	mpfTmpOutR = NULL;
	mpfTmpOutL2 = NULL;
	mpfTmpOutR2 = NULL;

	mfSampleRate = 44100;
	mfRealSampleRate = mfSampleRate;

	mlMIDIEventIndexRead = 0;
	mlMIDIEventIndexWrite = 0;
//	mlMIDIEventIndexDelayedRead = 0;
//	mlMIDIEventIndexDelayedWrite = 0;

//	mpDownSamplerL = IDownSampler::Create();
//	mpDownSamplerR = IDownSampler::Create();

	mpInitialMonoSection = NULL;
	mpFinalMonoSection = NULL;

	mlMonoStackSize = 0;

#ifdef __FILETEST
	if (iFile2 == -1) {
		iFile2 = _open("c:\\out.pcm", _O_BINARY | _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);
	}
#endif	// __FILETEST

	miNrOfVoicesPlaying = 0;

	mlPolyphony = 0;
	mlReserve = 0;

	muiStreamTime = 0;
}

CVoiceManager::~CVoiceManager()
{
#ifdef __FILETEST
	if (iFile2 != -1) {
		_close(iFile2);
		iFile2 = -1;
	}
#endif	// __FILETEST

//	mpDownSamplerL->Destroy();
//	mpDownSamplerR->Destroy();

	if (mpfTmpOutL) {
		mpAlignedMemAllocator->Free(mpfTmpOutL);
	}
	if (mpfTmpOutR) {
		mpAlignedMemAllocator->Free(mpfTmpOutR);
	}
	if (mpfTmpOutL2) {
		mpAlignedMemAllocator->Free(mpfTmpOutL2);
	}
	if (mpfTmpOutR2) {
		mpAlignedMemAllocator->Free(mpfTmpOutR2);
	}

	mpAlignedMemAllocator->Destroy();

	mpDSP->Destroy();
}

void CVoiceManager::SetVoiceMode(EVoiceMode eVoiceMode)
{
	Panic();

	meVoiceMode = eVoiceMode;

}

int CVoiceManager::GetVoiceMode() {

	return meVoiceMode;
}

void CVoiceManager::SetPolyphony(tint lPolyphony)
{
	Panic();

	mlPolyphony = lPolyphony;
	mlMonoStackSize = 0;
}

void CVoiceManager::SetFadeOutTime(tfloat fFadeOutTimeMS)
{
	Panic();

//	mlFadeTimeSamples = 128;

	mfFadeOutTimeMS = fFadeOutTimeMS;

	mlFadeTimeSamples = (long)(mfFadeOutTimeMS * (mfSampleRate / 1000));
}

void CVoiceManager::SetVoiceReserve(tint lReserve)
{
	Panic();

	mlReserve = lReserve;
}

void CVoiceManager::AddVoice(IVoice* pVoice)
{
	mVoices.insert(mVoices.end(), pVoice);
}

void CVoiceManager::AddInitialMonoSection(IMonoSection* pMonoSection)
{
	mpInitialMonoSection = pMonoSection;
}

void CVoiceManager::AddFinalMonoSection(IMonoSection* pMonoSection)
{
	mpFinalMonoSection = pMonoSection;
}

void CVoiceManager::SetHQMode(bool bMode)
{
	mbHQMode = bMode;
}

void CVoiceManager::SetSampleRate(float fSampleRate)
{
	Panic();

	mfRealSampleRate = mfSampleRate = fSampleRate;

	mbHQModeActive = false;
	if (GetHQMode()) {
		if (fSampleRate <= ST_MAX_HQFREQ) {
			mfSampleRate *= 2;
			mbHQModeActive = true;
		}
	}

	std::list<IVoice*>::iterator it = mVoices.begin();
	for (; it != mVoices.end(); it++) {
		(*it)->SetSampleFreq(mfSampleRate);
	}
	if (mpInitialMonoSection) {
		mpInitialMonoSection->SetSampleFreq(mfSampleRate);
	}
	if (mpFinalMonoSection) {
		mpFinalMonoSection->SetSampleFreq(mfSampleRate);
	}

	SetFadeOutTime(mfFadeOutTimeMS);
}

void CVoiceManager::SetMaxBufferSize(tint lMaxBufferSize)
{
	Panic();

	mlMaxBufferSize = lMaxBufferSize;

	long lMul = 1;
	if (mbHQModeActive) {
		lMul = 2;
	}

	long lWorkSize = mlMaxBufferSize * lMul;
	if (mlMaxSubBufferSize < lWorkSize) {
		lWorkSize = mlMaxSubBufferSize;
	}

	if (mpfTmpOutL) {
		mpAlignedMemAllocator->Free(mpfTmpOutL);
	}
	mpfTmpOutL = mpAlignedMemAllocator->AllocFloat32(lWorkSize);
	if (mpfTmpOutR) {
		mpAlignedMemAllocator->Free(mpfTmpOutR);
	}
	mpfTmpOutR = mpAlignedMemAllocator->AllocFloat32(lWorkSize);
	if (mpfTmpOutL2) {
		mpAlignedMemAllocator->Free(mpfTmpOutL2);
	}
	mpfTmpOutL2 = mpAlignedMemAllocator->AllocFloat32(lWorkSize);
	if (mpfTmpOutR2) {
		mpAlignedMemAllocator->Free(mpfTmpOutR2);
	}
	mpfTmpOutR2 = mpAlignedMemAllocator->AllocFloat32(lWorkSize);

	std::list<IVoice*>::iterator it = mVoices.begin();
	for (; it != mVoices.end(); it++) {
		(*it)->SetMaxBufferSize(lWorkSize);
	}
	if (mpInitialMonoSection) {
		mpInitialMonoSection->SetMaxBufferSize(lWorkSize);
	}
	if (mpFinalMonoSection) {
		mpFinalMonoSection->SetMaxBufferSize(lWorkSize);
	}
}

void CVoiceManager::SetMaxSubBufferSize(tint lMaxSubBufferSize)
{
	Panic();

	mlMaxSubBufferSize = lMaxSubBufferSize;

	// Force recalc of everything by calling setmaxbuffersize with current parameter
	if (mlMaxBufferSize != 0) {
		SetMaxBufferSize(mlMaxBufferSize);
	}
}

void CVoiceManager::Start()
{
}

void CVoiceManager::Stop()
{
	Panic();
}

void CVoiceManager::AddMIDIEvents(SMIDIEvent* pEvents, tint lNrOfEvents)
{
#ifdef __FILETEST
	if (iFile == -1) {
		iFile = _open("c:\\out.txt", _O_TEXT | _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);
	}
#endif	// __FILETEST

	long l;
	for (l = 0; l < lNrOfEvents; l++) {
//#ifdef _DEBUG
//		if (((pEvents[l].pchData[0] & 0xf0) == 0x80) ||
//			((pEvents[l].pchData[0] & 0xf0) == 0x90)) {
//			DEBUGFILE_OUT_FORMAT5(iFile, "Note: %d, Velocity: %d, Channel: %d, Status: %d, Time: %d\n", pEvents[l].pchData[1] & 0x7f, pEvents[l].pchData[2] & 0x7f, pEvents[l].pchData[0] & 0x0f, pEvents[l].pchData[0] & 0xf0, pEvents[l].ulOffset);
//		}
//#endif	// _DEBUG
		if ((pEvents[l].pchData[0] & 0xf0) == 0x90) {
			if ((pEvents[l].pchData[2] & 0x7f) == 0) {
				// Special case, note on with vel 0
				pEvents[l].pchData[0] &= (~0x90);
				pEvents[l].pchData[0] |= 0x80;
				pEvents[l].pchData[2] &= (~0x7f);
				pEvents[l].pchData[2] |= 040;
			}
		}

		mpMIDIEvents[mlMIDIEventIndexWrite] = pEvents[l];
#ifdef _DEBUG
		if (((pEvents[l].pchData[0] & 0xf0) == 0x80) ||
			((pEvents[l].pchData[0] & 0xf0) == 0x90)) {
			DEBUGFILE_OUT_FORMAT5(iFile, "Note: %d, Velocity: %d, Channel: %d, Status: %d, Time: %d\n", pEvents[l].pchData[1] & 0x7f, pEvents[l].pchData[2] & 0x7f, pEvents[l].pchData[0] & 0x0f, pEvents[l].pchData[0] & 0xf0, pEvents[l].ulOffset);
		}
#endif	// _DEBUG
#ifdef _DEBUG
		// Check disabled since we now sort before processing
/*		long lPrevIndex = mlMIDIEventIndexWrite - 1;
		if (lPrevIndex < 0) {
			lPrevIndex += ST_MAX_MIDI_EVENTS;
		}
		if (mlMIDIEventIndexWrite != mlMIDIEventIndexRead) {
			ASSERT(mpMIDIEvents[lPrevIndex].ulOffset <= mpMIDIEvents[mlMIDIEventIndexWrite].ulOffset);
			if (mpMIDIEvents[lPrevIndex].ulOffset == mpMIDIEvents[mlMIDIEventIndexWrite].ulOffset) {
				if ((mpMIDIEvents[lPrevIndex].pchData[0] & 0x0f) == (mpMIDIEvents[mlMIDIEventIndexWrite].pchData[0] & 0x0f)) {
					if ((mpMIDIEvents[lPrevIndex].pchData[1] & 0x7f) == (mpMIDIEvents[mlMIDIEventIndexWrite].pchData[1] & 0x7f)) {
						if (((mpMIDIEvents[lPrevIndex].pchData[0] & 0xf0) == 0x80) && ((mpMIDIEvents[mlMIDIEventIndexWrite].pchData[1] & 0xf0) == 0x90)) {
							DEBUG_BREAK;
						}
					}
				}
			}
		}*/
#endif	// _DEBUG
		mlMIDIEventIndexWrite++;
		if (mlMIDIEventIndexWrite >= ST_MAX_MIDI_EVENTS) {
			mlMIDIEventIndexWrite = 0;
		}
	}
}

void CVoiceManager::Panic()
{
	int iVoice;
	for (iVoice = 0; iVoice < MAX_POLYPHONY; iVoice++) {
		if (mpVoiceInfo[iVoice].eStatus != SVoiceInfo::StatusNotPlaying) {
			mpVoiceInfo[iVoice].eStatus = SVoiceInfo::StatusNotPlaying;

			GetVoice(iVoice)->EventVoiceDone();
		}
	}

	miNrOfVoicesPlaying = 0;
}

void CVoiceManager::DoProcess(float* pfOutL, float* pfOutR, long lC, long lChannels)
{
	if (mbHQModeActive) {
		lC *= 2;
	}

	bool bVector = false;
	if ((lC % glAlignmentSamples) == 0) {
		if (lC == mlMaxSubBufferSize) {
			bVector = true;
		}
	}

	if (mpInitialMonoSection) {
		if (lChannels == 1) {
			if (bVector) {
				mpInitialMonoSection->ProcessVector(NULL);
			}
			else {
				mpInitialMonoSection->Process(NULL, lC);
			}
		}
		else {
			if (bVector) {
				mpInitialMonoSection->ProcessVectorStereo(NULL, NULL);
			}
			else {
				mpInitialMonoSection->ProcessStereo(NULL, NULL, lC);
			}
		}
	}

//	memset(mpfTmpOutL2, 0, lC * sizeof(tfloat32));
	mpDSP->Clear(mpfTmpOutL2, lC);
	if (lChannels == 2) {
//		memset(mpfTmpOutR2, 0, lC * sizeof(tfloat32));
		mpDSP->Clear(mpfTmpOutR2, lC);
	}

	std::list<IVoice*>::iterator it = mVoices.begin();
	long l;
	for (l = 0; l < mlPolyphony + mlReserve; l++, it++) {
		IVoice* pVoice = *it;

		if ((mpVoiceInfo[l].eStatus == SVoiceInfo::StatusNoteOn) ||
			(mpVoiceInfo[l].eStatus == SVoiceInfo::StatusNoteOff)) {
			if (lChannels == 1) {
				bool bEnded;
				if (bVector) {
					bEnded = (pVoice->ProcessVector(mpfTmpOutL) == false);
				}
				else {
					bEnded = (pVoice->Process(mpfTmpOutL, lC) == false);
				}
				if (bEnded) {
					// Voice ended
					mpVoiceInfo[l].eStatus = SVoiceInfo::StatusNotPlaying;
					miNrOfVoicesPlaying--;
					pVoice->EventVoiceDone();
				}
				else {
					if (mpVoiceInfo[l].lFadeInPosition != -1) {
						FadeIn(l, mpfTmpOutL, lC);
					}
					if (mpVoiceInfo[l].lFadePosition != -1) {
						FadeOut(l, mpfTmpOutL, lC);
					}
				}
			}
			else {
				bool bEnded;
				if (bVector) {
					bEnded = (pVoice->ProcessVectorStereo(mpfTmpOutL, mpfTmpOutR) == false);
				}
				else {
					bEnded = (pVoice->ProcessStereo(mpfTmpOutL, mpfTmpOutR, lC) == false);
				}
				if (bEnded) {
					// Voice ended
					mpVoiceInfo[l].eStatus = SVoiceInfo::StatusNotPlaying;
					miNrOfVoicesPlaying--;
					pVoice->EventVoiceDone();
				}
				else {
					if (mpVoiceInfo[l].lFadeInPosition != -1) {
						FadeInStereo(l, mpfTmpOutL, mpfTmpOutR, lC);
					}
					if (mpVoiceInfo[l].lFadePosition != -1) {
						FadeOutStereo(l, mpfTmpOutL, mpfTmpOutR, lC);
					}
				}
			}

			// Accumulate
			if (lChannels == 1) {
//				for (long ll = 0; ll < lC; ll++) {
//					mpfTmpOutL2[ll] += mpfTmpOutL[ll];
//				}
				for (long ll = 0; ll < lC; ll++) {
					ASSERT((mpfTmpOutL[ll] < 4 && mpfTmpOutL[ll] >= 0) ||
						(mpfTmpOutL[ll] > -4 && mpfTmpOutL[ll] <= 0));
				}
				mpDSP->Add(mpfTmpOutL2, mpfTmpOutL, lC);
			}
			else {
//				for (long ll = 0; ll < lC; ll++) {
//					mpfTmpOutL2[ll] += mpfTmpOutL[ll];
//					mpfTmpOutR2[ll] += mpfTmpOutR[ll];
//				}
				mpDSP->Add(mpfTmpOutL2, mpfTmpOutL, lC);
				mpDSP->Add(mpfTmpOutR2, mpfTmpOutR, lC);
			}
		}
	}

	if (mpFinalMonoSection) {
		if (lChannels == 1) {
			if (bVector) {
				mpFinalMonoSection->ProcessVector(mpfTmpOutL2);
			}
			else {
				mpFinalMonoSection->Process(mpfTmpOutL2, lC);
			}
		}
		else {
			if (bVector) {
				mpFinalMonoSection->ProcessVectorStereo(mpfTmpOutL2, mpfTmpOutR2);
			}
			else {
				mpFinalMonoSection->ProcessStereo(mpfTmpOutL2, mpfTmpOutR2, lC);
			}
		}
	}

//	if (mbHQModeActive) {
//		mpDownSamplerL->Process(pfOutL, mpfTmpOutL2, lC / 2);
//		if (lChannels == 2) {
//			mpDownSamplerR->Process(pfOutR, mpfTmpOutR2, lC / 2);
//		}
//	}
//	else {
		if (lChannels == 1) {
			for (l = 0; l < lC; l++) {
				pfOutL[l] = mpfTmpOutL2[l];
			}
		}
		else {
			for (l = 0; l < lC; l++) {
				pfOutL[l] = mpfTmpOutL2[l];
				pfOutR[l] = mpfTmpOutR2[l];
			}
		}
//	}

#ifdef __FILETEST
//	_write(iFile2, (void*)pfOutL, lC * sizeof(float));
//	for (l = 0; l < lC; l++) {
//		tfloat32 s = pfOutL[l];
//		signed short ss = (signed short)(s * 32768);
//		_write(iFile2, (void*)&ss, sizeof(signed short));
//	}
#endif	__ FILETEST

	muiStreamTime += lC;
}

void CVoiceManager::DoNoteOn(unsigned long ulNote, unsigned long ulVelocity, unsigned long ulChannel, const SMIDIEvent& Event, bool bLegato)
{

	long lPlaying = FindPlayingNote(ulNote, ulChannel);
	if (lPlaying != -1) {
		if (mpVoiceInfo[lPlaying].lFadePosition == -1) {
			if (mpVoiceInfo[lPlaying].eStatus != SVoiceInfo::StatusNotPlaying) {
				mpVoiceInfo[lPlaying].eStatus = SVoiceInfo::StatusNotPlaying;
				miNrOfVoicesPlaying--;
				GetVoice(lPlaying)->EventVoiceDone();
			}
		}
	}
	long lVoice;

	if (meVoiceMode == VoiceModeLegato) {

		lVoice								= 0; 
		// Catch playing voice in release state
		if(mpVoiceInfo[0].eStatus == SVoiceInfo::StatusNoteOff) {
			bLegato = false;
		}
		else {
			// Set the legato flag
			bLegato = mpVoiceInfo[0].eStatus != SVoiceInfo::StatusNotPlaying;
		}
	}
	else {
		lVoice = FindFirstFreeVoice();
	}

	if (mpVoiceInfo[lVoice].eStatus == SVoiceInfo::StatusNotPlaying) {
		miNrOfVoicesPlaying++;
	}
	mpVoiceInfo[lVoice].eStatus			= SVoiceInfo::StatusNoteOn;
	mpVoiceInfo[lVoice].ulNoteNum		= ulNote;
	mpVoiceInfo[lVoice].ulChannel		= ulChannel;
	mpVoiceInfo[lVoice].lFadePosition	= -1;
	mpVoiceInfo[lVoice].lFadeInPosition = -1;
	mpVoiceInfo[lVoice].uiTime			= muiStreamTime + Event.ulOffset;

	IVoice* pVoice = GetVoice(lVoice);
	pVoice->EventNoteOn(ulNote, ulVelocity, ulChannel, bLegato);
	
}

void CVoiceManager::DoNoteOff(unsigned long ulNote, unsigned long ulVelocity, unsigned long ulChannel)
{
	long lVoice = FindPlayingNote(ulNote, ulChannel);
	// We cannot do the assert below, since it might be triggered if the voice belonging to the note has been stopped because of voice stealing
//	ASSERT(lVoice != -1);
	if (lVoice == -1) {
		return;
	}

	mpVoiceInfo[lVoice].eStatus = SVoiceInfo::StatusNoteOff;

	IVoice* pVoice = GetVoice(lVoice);
	pVoice->EventNoteOff(ulVelocity);
}

void CVoiceManager::DoMIDI(const SMIDIEvent& Event, bool bLegato)
{
	unsigned long ulStatus = Event.pchData[0] & 0xf0;
	unsigned long ulChannel = Event.pchData[0] & 0x0f;
	unsigned long ulNote = Event.pchData[1] & 0x7f;
	unsigned long ulVelocity = Event.pchData[2] & 0x7f;
	switch(ulStatus) {
		// Note off
		case 0x80:
			DoNoteOff(ulNote, ulVelocity, ulChannel);
			break;
		// Note on
		case 0x90:
			DoNoteOn(ulNote, ulVelocity, ulChannel, Event, bLegato);
			break;
	}
}

int CVoiceManager::GetNrOfVoicesInNoteOn()
{
	int iVoices = 0;

	for (int iVoice = 0; iVoice < mlPolyphony + mlReserve; iVoice++) {
		if (mpVoiceInfo[iVoice].eStatus == SVoiceInfo::StatusNoteOn) {
			iVoices++;
		}
	}
	
	return iVoices;
}

int CVoiceManager::GetNrOfVoicesPlaying()
{
	// This should work, but it doesn't
	// return miNrOfVoicesPlaying;

	int iVoices = 0;
	for (int iVoice = 0; iVoice < mlPolyphony + mlReserve; iVoice++) {
		if (mpVoiceInfo[iVoice].eStatus == SVoiceInfo::StatusNoteOn ||
			mpVoiceInfo[iVoice].eStatus == SVoiceInfo::StatusNoteOff) {
			iVoices++;
		}
	}

	return iVoices;
}

int CVoiceManager::GetNrOfActiveVoices()
{
	int iVoices = 0;
	for (int iVoice = 0; iVoice < mlPolyphony + mlReserve; iVoice++) {
		if (mpVoiceInfo[iVoice].eStatus == SVoiceInfo::StatusNoteOn ||
			mpVoiceInfo[iVoice].eStatus == SVoiceInfo::StatusNoteOff) {
			if (mpVoiceInfo[iVoice].lFadePosition == -1) {
				iVoices++;
			}
		}
	}

	return iVoices;
}

void CVoiceManager::ProcessFrame(float* pfOutL, float* pfOutR, long lC, long lChannels)
{
	DEBUGFILE_OUT_FORMAT1(iFile, "ProcessFrame Enter. lC == %d\n", lC);
	long lSamplesProcessed = 0;
	while (lSamplesProcessed != lC) {
		// Find the time position of next event
		long lNextEventTime = 100000;
		if (mlMIDIEventIndexRead != mlMIDIEventIndexWrite) {
			lNextEventTime = mpMIDIEvents[mlMIDIEventIndexRead].ulOffset;
		}

		long lToProcess = __min(lNextEventTime - lSamplesProcessed, lC - lSamplesProcessed);
		ASSERT(lToProcess >= 0);
		DEBUGFILE_OUT_FORMAT2(iFile, "ProcessFrame. lToProcess == %d. lSamplesProcessed == %d\n", lToProcess, lSamplesProcessed);
		// If sub-buffers are active, make sure we don't exceed the max tolerated size
		if (mlMaxSubBufferSize != 0) {
			long lTmp = mlMaxSubBufferSize;
			if (mbHQModeActive) {
				lTmp /= 2;
			}
			if (lToProcess > lTmp) {
				lToProcess = lTmp;
				DEBUGFILE_OUT_FORMAT1(iFile, "ProcessFrame. lToProcess changed to %d\n", lToProcess);
			}
		}
		if (lToProcess != 0) {
			if (lChannels == 1) {
				DoProcess(pfOutL + lSamplesProcessed, NULL, lToProcess, lChannels);
			}
			else {
				DoProcess(pfOutL + lSamplesProcessed, pfOutR + lSamplesProcessed, lToProcess, lChannels);
			}
			lSamplesProcessed += lToProcess;
		}

		// The last check is so we don't process event that should be processed on frame 0 of next buffer
		while ((lNextEventTime == lSamplesProcessed) &&
			(lNextEventTime != lC)) {
			// Variable to keep track of whether this is a legato voice
			bool bLegato = false;
			// Variable to check if note-on is triggered from mono stack
			bool bNoteOnFromMonoStack = false;
			// Check to see if we need to steal
			SMIDIEvent& Event = mpMIDIEvents[mlMIDIEventIndexRead];
			unsigned long ulStatus = Event.pchData[0] & 0xf0;
			// Note off
			if (ulStatus == 0x80) {
				
			
			}
			// Note Oon
			if (ulStatus == 0x90) {

				if (meVoiceMode == VoiceModeLegato) {

					if(mlMonoStackSize >= 0) {
						bLegato = true;
					}
					mlMonoStackSize++;
				}
				if (bLegato == false) {
					while (GetNrOfActiveVoices() >= mlPolyphony) {
						// Voice count reached, release one
						if (GetNrOfVoicesPlaying() >= mlPolyphony + mlReserve) {
							// We've reached the absolute limit, so simply cut one
							int iVoice = StopOneVoice(Event);
							ASSERT(iVoice != -1);
						}
						else {
							// We've still got reserve, so release normally
							int iVoice = FreeOneVoice(Event);
							if (iVoice == -1) {
								// It can happen that there's no voice to release.
								iVoice = StopOneVoice(Event);
								ASSERT(iVoice != -1);
							}
						}
						long lVoice = FindFirstFreeVoice();
						ASSERT(lVoice != -1);
					}
				}
			}
			if (bNoteOnFromMonoStack == false) {
				DoMIDI(mpMIDIEvents[mlMIDIEventIndexRead], bLegato);
			}
			if (++mlMIDIEventIndexRead >= ST_MAX_MIDI_EVENTS) {
				mlMIDIEventIndexRead = 0;
			}

			// Find the time position of next event
			lNextEventTime = 100000;
			if (mlMIDIEventIndexRead != mlMIDIEventIndexWrite) {
				lNextEventTime = mpMIDIEvents[mlMIDIEventIndexRead].ulOffset;
			}
			ASSERT(lNextEventTime >= 0);
		}
	}

	// Move remaining events in time according to processed samples
	long lIndex = mlMIDIEventIndexRead;
	while (lIndex != mlMIDIEventIndexWrite) {
		mpMIDIEvents[lIndex].ulOffset -= lC;
		if (++lIndex >= ST_MAX_MIDI_EVENTS) {
			lIndex = 0;
		}
	}

	DEBUGFILE_OUT(iFile, "ProcessFrame Leave\n");
}

bool operator<(const SMIDIEvent& a, const SMIDIEvent& b)
{
	if (a.ulOffset < b.ulOffset) {
		return true;
	}
	if (a.ulOffset > b.ulOffset) {
		return false;
	}

	// Ok, same offset... note-on is lowest
	if ((a.pchData[0] & 0xf0) == 0x90) {
		return true;
	}
	if ((b.pchData[0] & 0xf0) == 0x90) {
		return false;
	}

	// No note-on, anything but note-off is lowest
	if ((a.pchData[0] & 0xf0) == 0x80) {
		if ((b.pchData[0] & 0xf0) != 0x80) {
			return false;
		}
	}
	else {
		if ((b.pchData[0] & 0xf0) == 0x80) {
			return true;
		}
	}

	// Ok, at this point we dont care, but we have to order them somehow (because both a < b and b < a might be true)
	if (a.pchData[0] < b.pchData[0]) {
		return true;
	}
	if (b.pchData[0] < a.pchData[0]) {
		return false;
	}
	if (a.pchData[1] < b.pchData[1]) {
		return true;
	}
	if (b.pchData[1] < a.pchData[1]) {
		return false;
	}
	if (a.pchData[2] < b.pchData[2]) {
		return true;
	}
	if (b.pchData[2] < a.pchData[2]) {
		return false;
	}

	// They seem to be identical, doesn't matter
	return true;
}

void HeapSort(unsigned long n, SMIDIEvent ra[])
{
	if (n < 2) {
		return;
	}

	SMIDIEvent rra;
	unsigned long i;
	unsigned long j;

	unsigned long l = (n >> 1) + 1;
	unsigned long ir = n;

	for (;;) {
		if (l > 1) {
			l--;
			rra = ra[l - 1];
		}
		else {
			rra = ra[ir - 1];
			ra[ir - 1] = ra[0];
			if (--ir == 1) {
				ra[0] = rra;
				break;
			}
		}
		i = l;
		j = l+l;
		while (j <= ir) {
			if (j < ir && ra[j - 1] < ra[j+1 - 1]) {
				j++;
			}
			if (rra < ra[j - 1]) {
				ra[i - 1] = ra[j - 1];
				i = j;
				j <<= 1;
			}
			else {
				break;
			}
		}
		ra[i - 1] = rra;
	}
}

void CVoiceManager::SortMIDI()
{
	long lNrOfMIDIEvents = mlMIDIEventIndexWrite - mlMIDIEventIndexRead;
	// Adjust for wrapping
	if (lNrOfMIDIEvents < 0) {
		lNrOfMIDIEvents += ST_MAX_MIDI_EVENTS;
	}

	// Copy to tmp storage
	long lCurrentRead = mlMIDIEventIndexRead;
	long l;
	for (l = 0; l < lNrOfMIDIEvents; l++) {
		mpMIDIEventsTmp[l] = mpMIDIEvents[lCurrentRead];
		if (++lCurrentRead == ST_MAX_MIDI_EVENTS) {
			lCurrentRead = 0;
		}
	}

	// Sort
	HeapSort(lNrOfMIDIEvents, mpMIDIEventsTmp);

#ifdef _DEBUG
	// Check whether sort was ok
	SMIDIEvent Prev;
	Prev = mpMIDIEventsTmp[0];
	for (l = 1; l < lNrOfMIDIEvents; l++) {
		if ((Prev < mpMIDIEventsTmp[l]) == false) {
			DEBUG_BREAK;
		}
		Prev = mpMIDIEventsTmp[l];
	}
#endif	// _DEBUG

	// Copy back
	lCurrentRead = mlMIDIEventIndexRead;
	for (l = 0; l < lNrOfMIDIEvents; l++) {
		mpMIDIEvents[lCurrentRead] = mpMIDIEventsTmp[l];
		if (++lCurrentRead == ST_MAX_MIDI_EVENTS) {
			lCurrentRead = 0;
		}
	}
}

void CVoiceManager::Process(tfloat* pfOutL, tfloat* pfOutR, tint lC)
{
	SortMIDI();
	ProcessFrame(pfOutL, pfOutR, lC, 2);
}

void CVoiceManager::ProcessMono(tfloat* pfOut, tint lC)
{
	SortMIDI();
	ProcessFrame(pfOut, NULL, lC, 1);
}

long CVoiceManager::FindFirstFreeVoice()
{
/*	if (meVoiceMode == VoiceModeMono) {
		long lIndex;
		for (lIndex = 0; lIndex < mlPolyphony; lIndex++) {
			if (mpVoiceInfo[lIndex].eStatus == SVoiceInfo::StatusNotPlaying) {
				return lIndex;
			}
		}
	}
	else {*/
		long lIndex;
		for (lIndex = 0; lIndex < mlPolyphony + mlReserve; lIndex++) {
			if (mpVoiceInfo[lIndex].eStatus == SVoiceInfo::StatusNotPlaying) {
				return lIndex;
			}
		}
/*	}*/

	// Found none
	return -1;
}

long CVoiceManager::FindFirstPlayingVoice()
{
	long lIndex;
	for (lIndex = 0; lIndex < mlPolyphony + mlReserve; lIndex++) {
		if (mpVoiceInfo[lIndex].eStatus == SVoiceInfo::StatusNoteOn) {
			return lIndex;
		}
	}

	// Found none
	return -1;
}

long CVoiceManager::FindFirstPlayingVoiceNoRelease()
{
	long lIndex;
	for (lIndex = 0; lIndex < mlPolyphony + mlReserve; lIndex++) {
		if (mpVoiceInfo[lIndex].eStatus == SVoiceInfo::StatusNoteOn) {
			if (mpVoiceInfo[lIndex].lFadePosition == -1) {
				return lIndex;
			}
		}
	}

	// Found none
	return -1;
}

// Should optimize this by linking voice info and IVoice
IVoice* CVoiceManager::GetVoice(long lIndex)
{
	std::list<IVoice*>::iterator it = mVoices.begin();
	while (lIndex != 0) {
		it++;
		lIndex--;
	}
	return (*it);
}

long CVoiceManager::FindPlayingNote(unsigned long ulNote, unsigned long ulChannel)
{
	// Only return notes with noteon status
	long lIndex;
	for (lIndex = 0; lIndex < mlPolyphony + mlReserve; lIndex++) {
		if (mpVoiceInfo[lIndex].eStatus == SVoiceInfo::StatusNoteOn) {
			if ((mpVoiceInfo[lIndex].ulNoteNum == ulNote) &&
				(mpVoiceInfo[lIndex].ulChannel == ulChannel)) {
				return lIndex;
			}
		}
	}

	return -1;
}

long CVoiceManager::FreeOneVoice(const SMIDIEvent& Event)
{
	unsigned long ulChannel = Event.pchData[0] & 0x0f;
	unsigned long ulNote = Event.pchData[1] & 0x7f;

	// First check to see if there's a playing voice on same note
	long lVoiceToFree = FindPlayingNote(ulNote, ulChannel);
	if (mpVoiceInfo[lVoiceToFree].lFadePosition != -1) {
		lVoiceToFree = -1;
	}

	if (lVoiceToFree == -1) {
		// Look for oldest voice in release phase (exluding voices fading out)
		long lVoice = -1;
		unsigned int uiTimeLowest = (unsigned int)-1;
		for (long lIndex = 0; lIndex < mlPolyphony + mlReserve; lIndex++) {
			if (mpVoiceInfo[lIndex].eStatus == SVoiceInfo::StatusNoteOff) {
				if (mpVoiceInfo[lIndex].lFadePosition == -1) {
					if (mpVoiceInfo[lIndex].uiTime < uiTimeLowest) {
						uiTimeLowest = mpVoiceInfo[lIndex].uiTime;
					}
				}
			}
		}
		if (uiTimeLowest != (unsigned int)-1) {
			for (long lIndex = 0; lIndex < mlPolyphony + mlReserve; lIndex++) {
				if (mpVoiceInfo[lIndex].eStatus == SVoiceInfo::StatusNoteOff) {
					if (mpVoiceInfo[lIndex].lFadePosition == -1) {
						if (mpVoiceInfo[lIndex].uiTime == uiTimeLowest) {
							lVoice = lIndex;
						}
					}
				}
			}
		}
		lVoiceToFree = lVoice;
	}

	if (lVoiceToFree == -1) {
		// Ok, no voices in release phase, so take oldest note (exluding voices fading out)
		long lVoice = -1;
		unsigned int uiTimeLowest = (unsigned int)-1;
		for (long lIndex = 0; lIndex < mlPolyphony + mlReserve; lIndex++) {
			if (mpVoiceInfo[lIndex].eStatus == SVoiceInfo::StatusNoteOn) {
				if (mpVoiceInfo[lIndex].lFadePosition == -1) {
					if (mpVoiceInfo[lIndex].uiTime < uiTimeLowest) {
						uiTimeLowest = mpVoiceInfo[lIndex].uiTime;
					}
				}
			}
		}
		if (uiTimeLowest != (unsigned int)-1) {
			for (long lIndex = 0; lIndex < mlPolyphony + mlReserve; lIndex++) {
				if (mpVoiceInfo[lIndex].eStatus == SVoiceInfo::StatusNoteOn) {
					if (mpVoiceInfo[lIndex].lFadePosition == -1) {
						if (mpVoiceInfo[lIndex].uiTime == uiTimeLowest) {
							lVoice = lIndex;
						}
					}
				}
			}
		}
		lVoiceToFree = lVoice;
	}

	if (lVoiceToFree != -1) {
		ReleaseVoice(lVoiceToFree);
	}

	return lVoiceToFree;
}

long CVoiceManager::StopOneVoice(const SMIDIEvent& Event)
{
	unsigned long ulChannel = Event.pchData[0] & 0x0f;
	unsigned long ulNote = Event.pchData[1] & 0x7f;

	// First check to see if there's a playing voice on same note
	long lVoiceToFree = FindPlayingNote(ulNote, ulChannel);

	if (lVoiceToFree == -1) {
		// Look for oldest voice in release phase
		long lVoice = -1;
		unsigned int uiTimeLowest = (unsigned int)-1;
		for (long lIndex = 0; lIndex < mlPolyphony + mlReserve; lIndex++) {
			if (mpVoiceInfo[lIndex].eStatus == SVoiceInfo::StatusNoteOff) {
				if (mpVoiceInfo[lIndex].uiTime < uiTimeLowest) {
					uiTimeLowest = mpVoiceInfo[lIndex].uiTime;
				}
			}
		}
		if (uiTimeLowest != (unsigned int)-1) {
			for (long lIndex = 0; lIndex < mlPolyphony + mlReserve; lIndex++) {
				if (mpVoiceInfo[lIndex].eStatus == SVoiceInfo::StatusNoteOff) {
					if (mpVoiceInfo[lIndex].uiTime == uiTimeLowest) {
						lVoice = lIndex;
					}
				}
			}
		}
		lVoiceToFree = lVoice;
	}

	if (lVoiceToFree == -1) {
		// Ok, no voices in release phase, so take oldest note
		long lVoice = -1;
		unsigned int uiTimeLowest = (unsigned int)-1;
		for (long lIndex = 0; lIndex < mlPolyphony + mlReserve; lIndex++) {
			if (mpVoiceInfo[lIndex].eStatus == SVoiceInfo::StatusNoteOn) {
				if (mpVoiceInfo[lIndex].uiTime < uiTimeLowest) {
					uiTimeLowest = mpVoiceInfo[lIndex].uiTime;
				}
			}
		}
		if (uiTimeLowest != (unsigned int)-1) {
			for (long lIndex = 0; lIndex < mlPolyphony + mlReserve; lIndex++) {
				if (mpVoiceInfo[lIndex].eStatus == SVoiceInfo::StatusNoteOn) {
					if (mpVoiceInfo[lIndex].uiTime == uiTimeLowest) {
						lVoice = lIndex;
					}
				}
			}
		}
		lVoiceToFree = lVoice;
	}

	if (lVoiceToFree == -1) {
		// Should only happen if we're not playing any voice
		ASSERT(miNrOfVoicesPlaying == -1);
	}

	if (lVoiceToFree != -1) {
		if (mpVoiceInfo[lVoiceToFree].eStatus != SVoiceInfo::StatusNotPlaying) {
			mpVoiceInfo[lVoiceToFree].eStatus = SVoiceInfo::StatusNotPlaying;
			miNrOfVoicesPlaying--;
			GetVoice(lVoiceToFree)->EventVoiceDone();
		}
	}

	return lVoiceToFree;
}

void CVoiceManager::ReleaseVoice(long lVoice)
{
	mpVoiceInfo[lVoice].lFadePosition = mlFadeTimeSamples;
}

void CVoiceManager::FadeOut(long lVoice, tfloat32* pf, long lC)
{
	float fOneDivFadeMax = 1 / (float)mlFadeTimeSamples;

	long lFadePosition = mpVoiceInfo[lVoice].lFadePosition;
	long lToProcess = __min(lC, lFadePosition);
	for (long l = lToProcess; l; l--, lFadePosition--) {
		*pf++ *= (lFadePosition * fOneDivFadeMax);
	}
	// Process remainer
	lToProcess = lC - lToProcess;
	for (long l2 = lToProcess; l2; l2--) {
		*pf++ = 0;
	}

	mpVoiceInfo[lVoice].lFadePosition = lFadePosition;
	if (mpVoiceInfo[lVoice].lFadePosition == 0) {
		mpVoiceInfo[lVoice].eStatus = SVoiceInfo::StatusNotPlaying;
		miNrOfVoicesPlaying--;
		GetVoice(lVoice)->EventVoiceDone();
	}
}

void CVoiceManager::FadeOutStereo(long lVoice, tfloat32* pfL, tfloat32* pfR, long lC)
{
	float fOneDivFadeMax = 1 / (float)mlFadeTimeSamples;

	long lFadePosition = mpVoiceInfo[lVoice].lFadePosition;
	long lToProcess = __min(lC, lFadePosition);
	for (long l = lToProcess; l; l--, lFadePosition--) {
		*pfL++ *= (lFadePosition * fOneDivFadeMax);
		*pfR++ *= (lFadePosition * fOneDivFadeMax);
	}
	// Process remainer
	lToProcess = lC - lToProcess;
	for (long l2 = lToProcess; l2; l2--) {
		*pfL++ = 0;
		*pfR++ = 0;
	}

	mpVoiceInfo[lVoice].lFadePosition = lFadePosition;
	if (mpVoiceInfo[lVoice].lFadePosition == 0) {
		mpVoiceInfo[lVoice].eStatus = SVoiceInfo::StatusNotPlaying;
		miNrOfVoicesPlaying--;
		GetVoice(lVoice)->EventVoiceDone();
	}
}


void CVoiceManager::FadeIn(long lVoice, tfloat32* pf, long lC)
{
	float fOneDivFadeMax = 1 / (float)mlFadeTimeSamples;

	long lFadePosition = mpVoiceInfo[lVoice].lFadeInPosition;
	long lToProcess = __min(lC, lFadePosition);
	for (long l = lToProcess; l; l--, lFadePosition--) {
		*pf++ *= ((mlFadeTimeSamples - lFadePosition + 1) * fOneDivFadeMax);
	}

	mpVoiceInfo[lVoice].lFadeInPosition = lFadePosition;
	if (mpVoiceInfo[lVoice].lFadeInPosition == 0) {
		mpVoiceInfo[lVoice].lFadeInPosition = -1;
	}
}

void CVoiceManager::FadeInStereo(long lVoice, tfloat32* pfL, tfloat32* pfR, long lC)
{
	float fOneDivFadeMax = 1 / (float)mlFadeTimeSamples;

	long lFadePosition = mpVoiceInfo[lVoice].lFadeInPosition;
	long lToProcess = __min(lC, lFadePosition);
	for (long l = lToProcess; l; l--, lFadePosition--) {
		*pfL++ *= ((mlFadeTimeSamples - lFadePosition + 1) * fOneDivFadeMax);
		*pfR++ *= ((mlFadeTimeSamples - lFadePosition + 1) * fOneDivFadeMax);
	}

	mpVoiceInfo[lVoice].lFadeInPosition = lFadePosition;
	if (mpVoiceInfo[lVoice].lFadeInPosition == 0) {
		mpVoiceInfo[lVoice].lFadeInPosition = -1;
	}
}



