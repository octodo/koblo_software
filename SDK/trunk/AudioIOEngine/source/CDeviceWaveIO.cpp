/*!	\file CDeviceWaveIO.cpp
	\author Lasse Steen Bohnstedt
	\date 2008-05-02
*/

#include "aioeInternal.h"


void CALLBACK waveOutProc_static(
  HWAVEOUT hwo,      
  UINT uMsg,         
  DWORD_PTR dwInstance,  
  DWORD dwParam1,    
  DWORD dwParam2     
)
{
	CDeviceWaveIO* pthis = (CDeviceWaveIO*)dwInstance;
	pthis->WaveOutProc_Callback(hwo, uMsg, dwParam1);
} // waveOutProc_static


void CALLBACK waveInProc_static(
  HWAVEIN hwi,       
  UINT uMsg,         
  DWORD dwInstance,  
  DWORD dwParam1,    
  DWORD dwParam2     
)
{
	CDeviceWaveIO* pthis = (CDeviceWaveIO*)dwInstance;
	pthis->WaveInProc_Callback(hwi, uMsg, dwParam1);
} // waveInProc_static


CDeviceWaveIO::CDeviceWaveIO()
	: mpCallback (NULL),
	mfSampleRate(0),
	mbBlocked_volatile(true), mbInCallback_volatile(false), mbStarted(false)
{
	//gpDevice = this;

	mppfInputs = NULL;
	mppfOutputs = NULL;
	//for (int i = 0; i < WAVE_IO_MAX_INPUT_CHANNELS; i++) {
	//	mppfInputTmp[i] = NULL;
	//}

	// Default: Alias for default windows wave-out device
	muiDevIndex = WAVE_MAPPER;

	miHWBufferSize = 0;

	for (tint32 iStructIx = 0; iStructIx < MAX_RUNNING_WAVE_STRUCTS; iStructIx++) {
		mawavehdrOutput[iStructIx].dwFlags = 0;
		mapcOutputPCM[iStructIx] = NULL;
		mawavehdrInput[iStructIx].dwFlags = 0;
		mapcInputPCM[iStructIx] = NULL;
	}

	mbDriverPseudoLoaded = FALSE;
	miRunningStructs = 0;
	
	mhandle_out = 0;
	mhandle_in = 0;

	mbInput = mbOutput = false;
	miChannelsIn = miChannelsOut = 2;
} // constructor


CDeviceWaveIO::~CDeviceWaveIO()
{
	AbortCloseAndRelease();
} // destructor


IDevice* IDevice::CreateWaveIO()
{
	return dynamic_cast<IDevice*>(new CDeviceWaveIO());
} // CreateWaveIO


void CDeviceWaveIO::Destroy()
{
	AbortCloseAndRelease();
	delete dynamic_cast<CDeviceWaveIO*>(this);
} // Destroy


tbool CDeviceWaveIO::LoadDriver(tint iIndex, tbool bEnableInput, tbool bEnableOutput)
{
	AbortCloseAndRelease();
	mbDriverPseudoLoaded = FALSE;

	mbInput = false;
	if (iIndex == -1) {
		tuint32 uiDev = WAVE_MAPPER;
		// Can only be either output or input
		mbInput = (bEnableInput && !bEnableOutput);
		if (mbInput) {
			WAVEINCAPS caps;
			MMRESULT mmres = waveInGetDevCaps(WAVE_MAPPER, &caps, sizeof(caps));
			if (mmres != MMSYSERR_NOERROR)
				return false;
			miChannelsIn = caps.wChannels;
		}
		else {
			// We must query for default wave-out device
			if (!TestWaveOutCaps(&uiDev, 44100))
				return FALSE;
			iIndex = (tint32)uiDev;
		}
	}
	else {
		if (iIndex < 0)
			return false;
		if (iIndex >= (tint32)waveOutGetNumDevs()) {
			// Load input device
			tbool bSuccess = LoadDriverInput(iIndex - waveOutGetNumDevs());
			if (!bSuccess)
				return false;
			WAVEINCAPS caps;
			MMRESULT mmres = waveInGetDevCaps(WAVE_MAPPER, &caps, sizeof(caps));
			if (mmres != MMSYSERR_NOERROR)
				return false;
			miChannelsIn = caps.wChannels;
			mbInput = (bEnableInput && (miChannelsIn > 0));
		}
	}
	muiDevIndex = (tuint32)iIndex;
	mbDriverPseudoLoaded = TRUE;
	mbOutput = bEnableOutput;
	return (mbInput || mbOutput);
} // LoadDriver


tbool CDeviceWaveIO::LoadDriverInput(tint iIndex)
{
	/*
	AbortCloseAndRelease();
	mbDriverPseudoLoaded = FALSE;
	*/

	if ((iIndex < 0) || (iIndex >= (tint32)waveInGetNumDevs())) {
		return FALSE;
	}

	mbInput = true;
	mbOutput = false;
	muiDevIndex = (tuint32)iIndex;
	mbDriverPseudoLoaded = TRUE;
	return TRUE;
} // LoadDriverInput


tbool CDeviceWaveIO::Initialize(tint32 iSampleRate, tint iHWBufferSamples)
{
	tbool bIsRateAndBuffOK = false;

	AbortCloseAndRelease();

	if (mbDriverPseudoLoaded) {
		mppfOutputs = new float*[WAVE_IO_MAX_OUTPUT_CHANNELS];
		mppfInputs = new float*[WAVE_IO_MAX_INPUT_CHANNELS];

		// Calculate required number of running structs
		tint32 iRunningStructs = iHWBufferSamples / PRF_WAVE_PCM_SAMPLES;
		tint32 iPCMLen = PRF_WAVE_PCM_SAMPLES * sizeof(tfloat32);
		if (iRunningStructs < MIN_RUNNING_WAVE_STRUCTS) {
			iRunningStructs = MIN_RUNNING_WAVE_STRUCTS;
			iPCMLen = (iHWBufferSamples / iRunningStructs) * sizeof(tfloat32);
		}
		else if (iRunningStructs > MAX_RUNNING_WAVE_STRUCTS) {
			iRunningStructs = MAX_RUNNING_WAVE_STRUCTS;
			iPCMLen = (iHWBufferSamples / iRunningStructs) * sizeof(tfloat32);
			tint32 iMaxWavePCMBytes = MAX_WAVE_PCM_SAMPLES * sizeof(tfloat32);
			if (iPCMLen > iMaxWavePCMBytes)
				iPCMLen = iMaxWavePCMBytes;
		}

		// Ensure that PCM size has a number of samples that can be divided by MIN_WAVE_PCM_SAMPLES
		tint32 iMinWavePCMBytes = MIN_WAVE_PCM_SAMPLES * sizeof(tfloat32);
		iPCMLen += (iMinWavePCMBytes - 1);
		iPCMLen /= iMinWavePCMBytes;
		iPCMLen *= iMinWavePCMBytes;

		// Set member variables
		miRunningStructs = iRunningStructs;
		miPCMLen = iPCMLen;

		// Calculate actual number of samples in buffer (may vary from input)
		miHWBufferSize = miRunningStructs * (miPCMLen / sizeof(tfloat32));
		for (tint32 iStructIx = 0; iStructIx < miRunningStructs; iStructIx++) {
			mapcOutputPCM[iStructIx] = new tchar[miPCMLen];
			mapcInputPCM[iStructIx] = new tchar[miPCMLen];
		}
		miSamplesInPCM = miPCMLen / WAVE_IO_MAX_OUTPUT_CHANNELS / (16 / 8);
		mfSampleRate = iSampleRate;

		for (tint i = 0; i < WAVE_IO_MAX_OUTPUT_CHANNELS; i++) {
			mppfOutputs[i] = new tfloat[miSamplesInPCM];
		}
		for (tint i = 0; i < WAVE_IO_MAX_INPUT_CHANNELS; i++) {
			tfloat* pf = mppfInputs[i] = new tfloat[miSamplesInPCM];
			// Init in buffer with zeroes
			for (tint i2 = 0; i2 < miSamplesInPCM; i2++) {
				pf[i2] = 0.0f;
			}
		}

		// We don't do any testing of sample-rate capabilities here - leave that to Start() method
		bIsRateAndBuffOK = true;
	}

	if (GetInputChannels() <= 0) mbInput = false;
	if (GetOutputChannels() <= 0) mbOutput = false;

	return (bIsRateAndBuffOK && (mbInput || mbOutput));
} // Initialize


tint32 CDeviceWaveIO::GetInputChannels()
{
	return mbInput ? miChannelsIn : 0;
}

tint32 CDeviceWaveIO::GetOutputChannels()
{
	return mbOutput ? 2 : 0;
}

void CDeviceWaveIO::SetCallback(IDeviceCallback* pCallback)
{
	mpCallback = pCallback;
}


void CDeviceWaveIO::InitFormat(WAVEFORMATEX& format, tint32 iHz)
{
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 2;
	format.wBitsPerSample = 16;
	format.nSamplesPerSec = iHz;
	format.nBlockAlign = format.nChannels * (format.wBitsPerSample / 8);
	format.nAvgBytesPerSec = (format.nSamplesPerSec * format.nBlockAlign);
	format.cbSize = 0; // No extra bytes in end of structure for PCM/Wave
} // InitFormat


tbool CDeviceWaveIO::Start()
{
	if ((mpCallback) && (mppfOutputs) && (mppfInputs) && (!mbStarted)) {
		mbBlocked_volatile = false;

		WAVEFORMATEX format;
		InitFormat(format, (tint32)mfSampleRate);
		// Open driver - will invoke waveOutProc_static with uMsg == WOM_OPEN
		MMRESULT mmres = MMSYSERR_ERROR; // Default => unspecified error
		if (mbOutput) {
			mmres = waveOutOpen(
				&mhandle_out,
				muiDevIndex,
				&format,
				(DWORD)&waveOutProc_static,
				(DWORD)this,
				CALLBACK_FUNCTION
			);
		}
		if (mbInput) {
			mmres = waveInOpen(
				&mhandle_in,
				muiDevIndex,
				&format,
				(DWORD)&waveInProc_static,
				(DWORD)this,
				CALLBACK_FUNCTION
			);
		}
		/*
MMSYSERR_ALLOCATED Specified resource is already allocated. 
MMSYSERR_BADDEVICEID Specified device identifier is out of range. 
MMSYSERR_NODRIVER No device driver is present. 
MMSYSERR_NOMEM Unable to allocate or lock memory. 
WAVERR_BADFORMAT 
		*/
		if (mmres == MMSYSERR_NOERROR) {
			mmres = StartBuffers();
		}
		mbStarted = (mmres == MMSYSERR_NOERROR);
		if (!mbStarted)
			mbBlocked_volatile = true;
	}

	return mbStarted;
} // Start


MMRESULT CDeviceWaveIO::StartBuffers()
{
	MMRESULT mmres = MMSYSERR_ERROR; // Default => unspecified error

	HWAVEOUT hwo = mhandle_out;
	HWAVEIN hwi = mhandle_in;

	// Open device - load first data
	if ((!mbBlocked_volatile)&&(mpCallback)) {
		tint32 iStructIx = 0;
		do
		// For every block
		{
			// Load new data
			mpCallback->ProcessAudio(mppfOutputs, (const tfloat32**)mppfInputs, miSamplesInPCM);

			if (mbOutput) {
				WAVEHDR* pwavehdr = &(mawavehdrOutput[iStructIx]);
				// Init block
				pwavehdr->lpData = (LPSTR)mapcOutputPCM[iStructIx];
				pwavehdr->dwBufferLength = (DWORD)miPCMLen;
				pwavehdr->dwFlags = 0L;
				pwavehdr->dwLoops = 0L;
				// Translate into interlaced PCM
				XlatPPFloat2PCM(mppfOutputs, pwavehdr, miSamplesInPCM);
				// Start struct
				mmres = waveOutPrepareHeader(hwo, pwavehdr, sizeof(WAVEHDR));
				switch (mmres) {
					case MMSYSERR_NOERROR:
						break;
					case MMSYSERR_INVALHANDLE:
						return mmres;
					case MMSYSERR_NODRIVER:
						return mmres;
					case MMSYSERR_NOMEM:
						return mmres;
					default:
						return mmres;
				}
			}

			if (mbInput) {
				WAVEHDR* pwavehdr = &(mawavehdrInput[iStructIx]);
				// Init block
				pwavehdr->lpData = (LPSTR)mapcInputPCM[iStructIx];
				pwavehdr->dwBufferLength = (DWORD)miPCMLen;
				pwavehdr->dwFlags = 0L;
				pwavehdr->dwLoops = 0L;
				// Start struct
				mmres = waveInPrepareHeader(hwi, pwavehdr, sizeof(WAVEHDR));
				switch (mmres) {
					case MMSYSERR_NOERROR:
						break;
					case MMSYSERR_INVALHANDLE:
						return mmres;
					case MMSYSERR_NODRIVER:
						return mmres;
					case MMSYSERR_NOMEM:
						return mmres;
					default:
						return mmres;
				}
			}

			iStructIx++;
		}
		while ((iStructIx < miRunningStructs) && (mmres == MMSYSERR_NOERROR));

		// Then we queue to wave out device
		if (mbOutput) waveOutPause(hwo);
		//if (mbInput) waveInPause(hwo);
		for (iStructIx = 0; (mmres == MMSYSERR_NOERROR) && (iStructIx < miRunningStructs); iStructIx++) {
			if (mbOutput) {
				mmres = waveOutWrite(hwo, &mawavehdrOutput[iStructIx], sizeof(WAVEHDR));
			}
			if (mbInput) {
				mmres = waveInAddBuffer(hwi, &mawavehdrInput[iStructIx], sizeof(WAVEHDR));
			}
		}
		if (mmres == MMSYSERR_NOERROR) {
			if (mbOutput) mmres = waveOutRestart(hwo);
			if (mbInput) mmres = waveInStart(hwi);
		}
	}

	return mmres;
} // StartBuffers


MMRESULT CDeviceWaveIO::StopBuffers()
{
	HWAVEOUT hwo = mhandle_out;
	HWAVEIN hwi = mhandle_in;

	MMRESULT mmres = MMSYSERR_NOERROR;

	// Close device - release buffers
	for (tint32 iStructIx = 0; iStructIx < miRunningStructs; iStructIx++) {
		if (mbOutput) {
			MMRESULT mmres1 = waveOutUnprepareHeader(hwo, &mawavehdrOutput[iStructIx], sizeof(WAVEHDR));
			if (mmres == MMSYSERR_NOERROR)
				mmres = mmres1;
		}
		if (mbInput) {
			MMRESULT mmres2 = waveInUnprepareHeader(hwi, &mawavehdrInput[iStructIx], sizeof(WAVEHDR));
			if (mmres == MMSYSERR_NOERROR)
				mmres = mmres2;
		}
	}

	return mmres;
} // StopBuffers


void CDeviceWaveIO::AbortCloseAndRelease()
{
	if (mbStarted) {
		// Prevent more data from getting buffered
		mbBlocked_volatile = TRUE;

		// Wait until callback is finished - max wait time one second
		tbool bWait = true;
		tbool bTimeOut = false;
		DWORD dwTime0 = ::GetTickCount();
		while (bWait) {
			::Sleep(5);
			if (!mbInCallback_volatile)
				bWait = false;
			else {
				DWORD dwTime1 = ::GetTickCount();
				DWORD dwElapsed = (dwTime1 - dwTime0);
				if (dwElapsed > 1000) {
					// We've waited a whole second - abort now
					bWait = false;
					bTimeOut = true;
				}
			}
		}

		MMRESULT mmres = 0;

		if (!bTimeOut) {
			// Kill queued output (invokes waveOutProc_static with uMsg == WOM_DONE
			// - but new data won't get queued, because we've blocked)
			if (mbOutput) mmres = waveOutReset(mhandle_out);
			if (mbInput) mmres = waveInStop(mhandle_in);
			// Release the pair of buffers before we close device
			mmres = StopBuffers();
			// Close driver (invokes waveOutProc_static with uMsg == WOM_CLOSE)
			if (mbOutput) mmres = waveOutClose(mhandle_out);
			if (mbInput) mmres = waveInClose(mhandle_in);
		}
		// Remember we're closed
		mbStarted = FALSE;
	}

	for (tint32 iStructIx = 0; iStructIx < miRunningStructs; iStructIx++) {
		if (mapcOutputPCM[iStructIx]) {
			delete[] mapcOutputPCM[iStructIx];
			mapcOutputPCM[iStructIx] = NULL;
		}
		if (mapcInputPCM[iStructIx]) {
			delete[] mapcInputPCM[iStructIx];
			mapcInputPCM[iStructIx] = NULL;
		}
	}


	if (mppfInputs) {
		for (tint i = 0; i < WAVE_IO_MAX_INPUT_CHANNELS; i++) {
			delete[] mppfInputs[i];
		}
		delete[] mppfInputs;
		mppfInputs = NULL;
	}
	if (mppfOutputs) {
		for (tint i = 0; i < WAVE_IO_MAX_OUTPUT_CHANNELS; i++) {
			delete[] mppfOutputs[i];
		}
		delete[] mppfOutputs;
		mppfOutputs = NULL;
	}

	miRunningStructs = 0;
} // CDeviceWaveIO::AbortCloseAndRelease


void CDeviceWaveIO::WaveOutProc_Callback(HWAVEOUT hwo, UINT uMsg, DWORD dwParam1)
{
	if (mbBlocked_volatile) {
		return;
	}

	mbInCallback_volatile = true;

	switch (uMsg) {
		case WOM_OPEN:
			{
				//StartBuffers();
			}
			break;

		case WOM_DONE:
			{
				// Previous processing done - load new data
				WAVEHDR* pwavehdr = (WAVEHDR*)dwParam1;
				mpCallback->ProcessAudio(mppfOutputs, (const tfloat32**)mppfInputs, miSamplesInPCM);
				// Translate into interlaced PCM
				XlatPPFloat2PCM(mppfOutputs, pwavehdr, miSamplesInPCM);
				// Queue to wave out device
				waveOutWrite(hwo, pwavehdr, sizeof(WAVEHDR));
			}
			break;

		case WOM_CLOSE:
			{
				// StopBuffers();
			}
			break;
	}

	mbInCallback_volatile = false;
} // CDeviceWaveIO::WaveOutProc_Callback


void CDeviceWaveIO::WaveInProc_Callback(HWAVEIN hwi, UINT uMsg, DWORD dwParam1)
{
	if (mbBlocked_volatile) {
		return;
	}

	mbInCallback_volatile = true;

	switch (uMsg) {
		case WIM_OPEN:
			{
				//StartBuffers();
			}
			break;

		case WIM_DATA:
			{
				// Translate from interlaced PCM
				WAVEHDR* pwavehdr = (WAVEHDR*)dwParam1;
				tint32 iSamples = 0;
				XlatPCM2PPFloat(pwavehdr, mppfInputs, &iSamples);
				// Let calling app process the input via callback
				mpCallback->ProcessAudio(mppfOutputs, (const tfloat32**)mppfInputs, miSamplesInPCM);
				// Queue to wave in device
				waveInAddBuffer(hwi, pwavehdr, sizeof(WAVEHDR));
			}
			break;

		case WIM_CLOSE:
			{
				// StopBuffers();
			}
			break;
	}

	mbInCallback_volatile = false;
} // CDeviceWaveIO::WaveInProc_Callback


tbool CDeviceWaveIO::TestWaveOutCaps(tuint32* puiDevID, tint32 iHz)
{
	WAVEFORMATEX format;
	InitFormat(format, iHz);
	
	MMRESULT mmres;
	if (*puiDevID != WAVE_MAPPER) {
		// Real device ID - Just query
		mmres = waveOutOpen(
			NULL,
			*puiDevID,
			&format,
			0,
			0,
			WAVE_FORMAT_QUERY
		);
	}
	else {
		// For wave mapper we need to actually open and then close
		HWAVEOUT handle;
		mmres = waveOutOpen(
			&handle,
			*puiDevID,
			&format,
			0,
			0,
			CALLBACK_NULL
		);
		if (mmres == MMSYSERR_NOERROR) {
			// Success! 
			// Save device ID
			waveOutGetID(handle, puiDevID);
			// Now close it again
			waveOutClose(handle);
		}
	}
	return (mmres == MMSYSERR_NOERROR);
} // CDeviceWaveIO::TestWaveOutCaps


void CDeviceWaveIO::XlatPPFloat2PCM(tfloat32** ppf, WAVEHDR* pwavehdr, tint32 iSamples)
{
	tchar* pcPCM = pwavehdr->lpData;
	tfloat32* pfL = ppf[0];
	tfloat32* pfR = ppf[1];
	tint32 iInputBytes = iSamples * sizeof(tfloat32);
	tint64 iAccumOverflows_Dummy = 0;
	st::Xlat::DualAF32Mono_PCM16Stereo(
		(tchar*)pfL, (tchar*)pfR, iInputBytes,
		&pcPCM, NULL,
		&iAccumOverflows_Dummy);
} // CDeviceWaveIO::XlatPPFloat2PCM


void CDeviceWaveIO::XlatPCM2PPFloat(WAVEHDR* pwavehdr, tfloat32** ppf, tint32* piSamples)
{
	tchar* pcPCM = pwavehdr->lpData;
	tfloat32* pfL = ppf[0];
	tfloat32* pfR = ppf[1];
	tint32 iInputBytes = pwavehdr->dwBytesRecorded;
	*piSamples = (iInputBytes / (sizeof(tint16) * 2));
	st::Xlat::PCM16Stereo_DualAF32Mono(
		pcPCM, iInputBytes,
		(tchar**)&pfL, NULL, (tchar**)&pfR, NULL);
} // CDeviceWaveIO::XlatPCM2PPFloat


tuint32 CDeviceWaveIO::GetTimeStamp()
{
	return 0;
}

tuint32 CDeviceWaveIO::GetPreferredBufferSize()
{
	// Return as preferred number of samples in buffer
	// * 2 because there's really two buffers
	// / sizeof(tfloat32) because we use one tfloat32 per sample
	return GetPrfBufferSize_InBytes() / (sizeof(tfloat32));
}

tuint32 CDeviceWaveIO::GetBufferSize()
{
	// Return as actual number of samples in buffer
	// * 2 because there's really two buffers
	// / sizeof(tfloat32) because we use one tfloat32 per sample
	return miHWBufferSize / (sizeof(tfloat32));
}

tuint32 CDeviceWaveIO::GetMinBufferSize()
{
	// Return as min number of samples in buffer
	// * 2 because there's really two buffers
	// / sizeof(tfloat32) because we use one tfloat32 per sample
	return GetMinBufferSize_InBytes() / (sizeof(tfloat32));
}

tuint32 CDeviceWaveIO::GetMaxBufferSize()
{
	// Return as max number of samples in buffer
	// / sizeof(tfloat32) because we use one tfloat32 per sample
	return GetMaxBufferSize_InBytes() / (sizeof(tfloat32));
}

tuint32 CDeviceWaveIO::GetPreferredSampleRate()
{
	return 44100;
}

tuint32 CDeviceWaveIO::GetSampleRate()
{
	return Float2Int(mfSampleRate);
}

tuint32 CDeviceWaveIO::EnumeratePossibleSampleRates(tuint32 uiLast)
{
	switch (uiLast) {
		case 0:		return 44100;
		//case 44100:	return 48000;
		//case 48000:	return 88200;
		//case 88200:	return 96000;
		default:	return 0;
	}
}

void CDeviceWaveIO::ShowSettings()
{
	// Do nothing
}
