/*!	\file CDeviceCoreAudio.cpp
	\author Michael Olsen
	\date 10/Feb/2006
	\date 10/Feb/2006
*/

#include "aioeInternal.h"

#include <AvailabilityMacros.h>
#include <CoreAudio/CoreAudioTypes.h>
#include <CoreAudio/CoreAudioTypes.h>

float** gppfInput = NULL;

tint32 giInputsAvailable = 0;

CMutex gMutex;

OSStatus HALIOProc(AudioDeviceID inDevice, const AudioTimeStamp* inNow, const AudioBufferList* inInputData, const AudioTimeStamp* inInputTime, AudioBufferList* outOutputData, const AudioTimeStamp* inOutputTime, void* pThis);


CDeviceCoreAudio::CDeviceCoreAudio()
	: mpCallback (NULL),
	mfSampleRate(0)
{
	mppfInputs = NULL;
	mppfOutputs = NULL;
	for (int i = 0; i < COREAUDIO_MAX_INPUT_CHANNELS; i++) {
		mppfInputTmp[i] = NULL;
	}

	if (gppfInput == NULL) {
		gppfInput = new float*[COREAUDIO_MAX_INPUT_CHANNELS];
		tint32 iInput;
		for (iInput = 0; iInput < COREAUDIO_MAX_INPUT_CHANNELS; iInput++) {
			gppfInput[iInput] = new float[16384];
		}
	}

	mbEnableInput = mbEnableOutput = false;
} // constructor


CDeviceCoreAudio::~CDeviceCoreAudio()
{
	AudioDeviceStop(mAudioDeviceID, HALIOProc);
	AudioDeviceRemoveIOProc(mAudioDeviceID, HALIOProc);

	if (mppfInputs) {
		delete[] mppfInputs;
	}
	if (mppfOutputs) {
		delete[] mppfOutputs;
	}

	for (int i = 0; i < COREAUDIO_MAX_INPUT_CHANNELS; i++) {
		if (mppfInputTmp[i]) {
			delete[] mppfInputTmp[i];
		}
	}
} // destructor


IDevice* IDevice::CreateCoreAudio()
{
	return dynamic_cast<IDevice*>(new CDeviceCoreAudio());
} // CreateCoreAudio


void CDeviceCoreAudio::Destroy()
{
	delete dynamic_cast<CDeviceCoreAudio*>(this);
} // Destroy


void CDeviceCoreAudio::SetAudioDeviceID(AudioDeviceID ID)
{
	mAudioDeviceID = ID;
}


void CDeviceCoreAudio::SetBehavior(tbool bEnableInput, tbool bEnableOutput)
{
	mbEnableInput = bEnableInput;
	mbEnableOutput = bEnableOutput;
} // SetBehavior


tbool CDeviceCoreAudio::Initialize(tint32 iSampleRate, tint iHWBufferSize)
{
	struct AudioTimeStamp timeStamp;
	timeStamp.mFlags = 0;

	Float64 fSampleRate = iSampleRate;

	OSErr err1 = AudioDeviceSetProperty(mAudioDeviceID, &timeStamp, 0, FALSE, kAudioDevicePropertyBufferFrameSize, sizeof(UInt32), &iHWBufferSize);
	OSErr err2 = AudioDeviceSetProperty(mAudioDeviceID, &timeStamp, 0, FALSE, kAudioDevicePropertyNominalSampleRate, sizeof(Float64), &fSampleRate);

	miHWBufferSize = iHWBufferSize;

	mfSampleRate = iSampleRate;


	tbool bIsRateAndBuffOK = ((err1 == 0) && (err2 == 0));

	if (GetInputChannels() <= 0) mbEnableInput = false;
	if (GetOutputChannels() <= 0) mbEnableOutput = false;

	return (bIsRateAndBuffOK && (mbEnableInput || mbEnableOutput));
} // Initialize


tbool CDeviceCoreAudio::Start()
{
	OSErr err1 = AudioDeviceAddIOProc(mAudioDeviceID, HALIOProc, (void*)this);
	OSErr err2 = AudioDeviceStart(mAudioDeviceID, HALIOProc);

	return ((err1 == 0) && (err2 == 0));
} // Start


void CDeviceCoreAudio::SetCallback(IDeviceCallback* pCallback)
{
	mpCallback = pCallback;
}


tint32 CDeviceCoreAudio::GetInputChannels()
{
	if (!mbEnableInput)
		return 0;

	OSStatus err;
	UInt32 propSize;
	tint32 result = 0;

	err = AudioDeviceGetPropertyInfo(mAudioDeviceID, 0, TRUE, kAudioDevicePropertyStreamConfiguration, &propSize, NULL);
	if (err) return -1;

	AudioBufferList* buflist = (AudioBufferList*)malloc(propSize);
	err = AudioDeviceGetProperty(mAudioDeviceID, 0, TRUE, kAudioDevicePropertyStreamConfiguration, &propSize, buflist);
	if (!err) {
		for (UInt32 i = 0; i < buflist->mNumberBuffers; ++i) {
			result += buflist->mBuffers[i].mNumberChannels;
		}
	}
	free(buflist);
	return result;
} // GetInputChannels


tint32 CDeviceCoreAudio::GetOutputChannels()
{
	if (!mbEnableOutput)
		return 0;

	OSStatus err;
	UInt32 propSize;
	tint32 result = 0;
	
	err = AudioDeviceGetPropertyInfo(mAudioDeviceID, 0, FALSE, kAudioDevicePropertyStreamConfiguration, &propSize, NULL);
	if (err) return -1;
	
	AudioBufferList* buflist = (AudioBufferList*)malloc(propSize);
	err = AudioDeviceGetProperty(mAudioDeviceID, 0, FALSE, kAudioDevicePropertyStreamConfiguration, &propSize, buflist);
	if (!err) {
		for (UInt32 i = 0; i < buflist->mNumberBuffers; ++i) {
			result += buflist->mBuffers[i].mNumberChannels;
		}
	}
	free(buflist);
	return result;
}

tuint32 CDeviceCoreAudio::GetPreferredBufferSize()
{
	UInt32 outSize = sizeof(UInt32);
	UInt32 retVal = 0;

	AudioDeviceGetProperty(mAudioDeviceID, 0, FALSE, kAudioDevicePropertyBufferFrameSize, &outSize, &retVal);

	return retVal;
}

tuint32 CDeviceCoreAudio::GetMinBufferSize()
{
	UInt32 outSize = sizeof(AudioValueRange);
	struct AudioValueRange AVR;

	AudioDeviceGetProperty(mAudioDeviceID, 0, FALSE, kAudioDevicePropertyBufferFrameSizeRange, &outSize, &AVR);

	return AVR.mMinimum;
}

tuint32 CDeviceCoreAudio::GetMaxBufferSize()
{
	UInt32 outSize = sizeof(AudioValueRange);
	struct AudioValueRange AVR;

	AudioDeviceGetProperty(mAudioDeviceID, 0, FALSE, kAudioDevicePropertyBufferFrameSizeRange, &outSize, &AVR);

	return AVR.mMaximum;
}

tuint32 CDeviceCoreAudio::GetBufferSize()
{
	return miHWBufferSize;
}

tuint32 CDeviceCoreAudio::GetPreferredSampleRate()
{
	return 44100;
}

tuint32 CDeviceCoreAudio::GetSampleRate()
{
	return Float2Int(mfSampleRate);
}

tuint32 CDeviceCoreAudio::EnumeratePossibleSampleRates(tuint32 uiLast)
{
	switch (uiLast) {
		case 0:		return 44100;
		case 44100:	return 48000;
		case 48000:	return 88200;
		case 88200:	return 96000;
		default:	return 0;
	}
}

void CDeviceCoreAudio::ShowSettings()
{
}

tuint32 CDeviceCoreAudio::GetTimeStamp()
{
	return 0;
}

void CDeviceCoreAudio::Process_Old(tfloat32* pfOut, const tfloat32* pfIn, tint32 iBufferSize, tint32 iBuffersIn)
{
	// Allocate buffers if not already allocated
	if (mppfInputs == NULL) {
		mppfInputs = new float*[COREAUDIO_MAX_INPUT_CHANNELS];
		mppfOutputs = new float*[COREAUDIO_MAX_INPUT_CHANNELS];
		for (int ii = 0; ii < COREAUDIO_MAX_INPUT_CHANNELS; ii++) {
			mppfInputs[ii] = new float[miHWBufferSize];
			mppfOutputs[ii] = new float[miHWBufferSize];
		}
	}

	tint32 iSample;
	if (pfIn != 0) {
		tint32 iBuffer;
		for (iBuffer = 0; iBuffer < iBuffersIn; iBuffer++) {
			for (iSample = 0; iSample < iBufferSize; iSample++) {
				mppfInputs[iBuffer][iSample] = pfIn[iSample * iBuffersIn + iBuffer];
			}
		}
	}

	if (pfIn != 0) {
		if (pfOut == 0) {
			tint32 iBuffer;
			for (iBuffer = 0; iBuffer < iBuffersIn; iBuffer++) {
				float* pfInput = gppfInput[iBuffer];
				for (iSample = 0; iSample < iBufferSize; iSample++) {
					pfInput[iSample] = mppfInputs[iBuffer][iSample];
				}
			}
			giInputsAvailable = iBuffersIn;
			return;
		}
	}
	else {
		if (pfOut != 0) {
			if (giInputsAvailable) {
				iBuffersIn = giInputsAvailable;
				tint32 iBuffer;
				for (iBuffer = 0; iBuffer < iBuffersIn; iBuffer++) {
					float* pfInput = gppfInput[iBuffer];
					for (iSample = 0; iSample < iBufferSize; iSample++) {
						mppfInputs[iBuffer][iSample] = pfInput[iSample];
					}
				}
				giInputsAvailable = 0;
			}
		}
	}

	mpCallback->ProcessAudio((tfloat32**)mppfOutputs, (const tfloat32**)mppfInputs, iBufferSize);

	if (pfOut != 0) {
		for (iSample = 0; iSample < iBufferSize; iSample++) {
			pfOut[iSample * 2] = mppfOutputs[0][iSample];
			pfOut[iSample * 2 + 1] = mppfOutputs[1][iSample];
		}
	}
} // Process_Old


void CDeviceCoreAudio::Process(tfloat32* pfOut, const tfloat32* pfIn, tint32 iBufferSize, tint32 iBuffersIn)
{
	// Allocate buffers if not already allocated
	if (mppfInputs == NULL) {
		mppfInputs = new float*[COREAUDIO_MAX_INPUT_CHANNELS];
		mppfOutputs = new float*[COREAUDIO_MAX_INPUT_CHANNELS];
		for (int ii = 0; ii < COREAUDIO_MAX_INPUT_CHANNELS; ii++) {
			mppfInputs[ii] = new float[miHWBufferSize];
			mppfOutputs[ii] = new float[miHWBufferSize];
		}
	}
	
	tint32 iSample;
	if ((pfIn != 0) && (mbEnableInput)) {
		tint32 iBuffer;
		for (iBuffer = 0; iBuffer < iBuffersIn; iBuffer++) {
			for (iSample = 0; iSample < iBufferSize; iSample++) {
				mppfInputs[iBuffer][iSample] = pfIn[iSample * iBuffersIn + iBuffer];
			}
		}
	}
	
	mpCallback->ProcessAudio((tfloat32**)mppfOutputs, (const tfloat32**)mppfInputs, iBufferSize);
	
	if (pfOut != 0) {
		// (lasse) I wonder: is this ok for > 2 channels?
		if (mbEnableOutput) {
			for (iSample = 0; iSample < iBufferSize; iSample++) {
				pfOut[iSample * 2] = mppfOutputs[0][iSample];
				pfOut[iSample * 2 + 1] = mppfOutputs[1][iSample];
			}
		}
		else {
			// Silence both channels
			memset(pfOut, '\0', iBufferSize * 2 * sizeof(tfloat32));
		}
	}
} // Process

void CDeviceCoreAudio::ProcessNonInterleaved(tfloat32* pfOut0, tfloat32* pfOut1, const tfloat32* pfIn0, const tfloat32* pfIn1, tint32 iBufferSize)
{
	// (mo) This could be optimized, i.e. don't allocate buffers, but work directly on supplied data

	// Allocate buffers if not already allocated
	if (mppfInputs == NULL) {
		mppfInputs = new float*[COREAUDIO_MAX_INPUT_CHANNELS];
		mppfOutputs = new float*[COREAUDIO_MAX_INPUT_CHANNELS];
		for (int ii = 0; ii < COREAUDIO_MAX_INPUT_CHANNELS; ii++) {
			mppfInputs[ii] = new float[miHWBufferSize];
			mppfOutputs[ii] = new float[miHWBufferSize];
		}
	}

	tint32 iSample;
	if (pfIn0 != 0) {
		for (iSample = 0; iSample < iBufferSize; iSample++) {
			mppfInputs[0][iSample] = pfIn0[iSample];
			mppfInputs[1][iSample] = pfIn1[iSample];
		}
	}

	mpCallback->ProcessAudio((tfloat32**)mppfOutputs, (const tfloat32**)mppfInputs, iBufferSize);

	for (iSample = 0; iSample < iBufferSize; iSample++) {
		pfOut0[iSample] = mppfOutputs[0][iSample];
		pfOut1[iSample] = mppfOutputs[1][iSample];
	}
}

OSStatus HALIOProc (AudioDeviceID inDevice, const AudioTimeStamp* inNow, const AudioBufferList* inInputData, const AudioTimeStamp* inInputTime, AudioBufferList* outOutputData, const AudioTimeStamp* inOutputTime, void* pThis)
{
	CAutoLock Lock(gMutex);

	CDeviceCoreAudio* pDevice = (CDeviceCoreAudio*)pThis;

	if (outOutputData->mNumberBuffers == 0) {
		pDevice->Process(NULL,
						(const tfloat32*)inInputData->mBuffers[0].mData,
						pDevice->GetBufferSize(),
						inInputData->mBuffers[0].mNumberChannels);
	}
	else if (outOutputData->mNumberBuffers == 1) {
		if (inInputData->mNumberBuffers > 0) {
			pDevice->Process((tfloat32*)outOutputData->mBuffers[0].mData,
							(const tfloat32*)inInputData->mBuffers[0].mData,
							pDevice->GetBufferSize(),
							inInputData->mBuffers[0].mNumberChannels);
		}
		else {
			pDevice->Process((tfloat32*)outOutputData->mBuffers[0].mData,
							 NULL,
							 pDevice->GetBufferSize(),
							0);
		}
	}
	else {
		if (outOutputData->mNumberBuffers == 2) {
			pDevice->ProcessNonInterleaved((tfloat32*)outOutputData->mBuffers[0].mData,
				(tfloat32*)outOutputData->mBuffers[1].mData,
				(const tfloat32*)inInputData->mBuffers[0].mData,
				(const tfloat32*)inInputData->mBuffers[1].mData,
				pDevice->GetBufferSize());
		}
	}

	return noErr;
}

/*	// Update current time
	::QueryPerformanceCounter(&mTimeLast);

	// the actual processing callback.
	// Beware that this is normally in a seperate thread, hence be sure that you take care
	// about thread synchronization. This is omitted here for simplicity.
	static processedSamples = 0;

	int iInputIndex = 0;
	int iOutputIndex = 0;

	// Allocate buffers if not already allocated
	if (mppfInputs == NULL) {
		mppfInputs = new float*[CoreAudio_MAX_INPUT_CHANNELS];
		for (int ii = 0; ii < CoreAudio_MAX_INPUT_CHANNELS; ii++) {
			mppfInputTmp[ii] = new float[miHWBufferSize];
		}
		mppfOutputs = new float*[CoreAudio_MAX_INPUT_CHANNELS];
	}

	// Perform the processing
	int i;
	for (i = 0; i < miChannelsIn + miChannelsOut; i++) {
		if (mBuffers[i].isInput == false) {
			mppfOutputs[iOutputIndex++] = (float*)(mBuffers[i].buffers[iIndex]);
		}
		else {
			float* pfDest = mppfInputTmp[iInputIndex];
			long* plSrc = (long*)(mBuffers[i].buffers[iIndex]);
			for (int ii = 0; ii < miHWBufferSize; ii++) {
				pfDest[ii] = (float)(plSrc[ii] / 2147483648.0);
			}

			mppfInputs[iInputIndex] = mppfInputTmp[iInputIndex];
			iInputIndex++;
		}
	}

	mpCallback->ProcessAudio(mppfOutputs, (const tfloat**)mppfInputs, miHWBufferSize);

	// Convert from float to 32LSB
	CoreAudioConvertSamples Converter;
	int ii;
	for (ii = 0; ii < miChannelsOut; ii++) {
		Converter.float32toInt32inPlace(mppfOutputs[ii], miHWBufferSize);
	}

	// If the driver supports the CoreAudioOutputReady() optimization, do it here, all data are in place
	if (mbPostOutput) {
		theCoreAudioDriver->outputReady();
	}

	return 0L;*/




