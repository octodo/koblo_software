/*!	\file CDeviceASIO.cpp
	\author Michael Olsen
	\date 09/Feb/2005
	\date 09/Feb/2005
*/

#include "aioeInternal.h"

//! Global ASIO driver. Cannot change name (or scope) because of ASIO SDK
IASIO* theAsioDriver = 0;

//! Global ASIO driver interface. Neccasary since the ASIO callback doesn't have device info
CDeviceASIO* gpDevice = NULL;


CDeviceASIO::CDeviceASIO()
	: mpCallback (NULL),
	mfSampleRate(0),
	miHWBufferSize(0)
{
	gpDevice = this;

	mppfInputs = NULL;
	mppfOutputs = NULL;
/*	for (int i = 0; i < ASIO_MAX_INPUT_CHANNELS; i++) {
		mppfInputs[i] = NULL;
	}*/

	//! Get the performance counter frequency
	::QueryPerformanceFrequency(&mPerfFreq);

	mInfo.sysRef = NULL;

	mbEnableInput = mbEnableOutput = false;
} // constructor


CDeviceASIO::~CDeviceASIO()
{
	if (theAsioDriver) {
		ASIOError err = theAsioDriver->stop();

		theAsioDriver->disposeBuffers();

#if WIN32
		mDrivers.removeCurrentDriver();
#else
		delete theAsioDriver;
		// Not 100% sure this line is needed (or even valid)
		mDrivers.removeCurrentDriver();
#endif
	}

	if (mppfInputs) {
		for (int i = 0; i < ASIO_MAX_INPUT_CHANNELS; i++) {
			if (mppfInputs[i]) {
				delete[] mppfInputs[i];
			}
		}

		delete[] mppfInputs;
	}
	if (mppfOutputs) {
		for (int i = 0; i < ASIO_MAX_OUTPUT_CHANNELS; i++) {
			delete[] mppfOutputs[i];
		}

		delete[] mppfOutputs;
	}
} // destructor


IDevice* IDevice::CreateASIO()
{
	return dynamic_cast<IDevice*>(new CDeviceASIO());
} // CreateASIO


void CDeviceASIO::Destroy()
{
	delete dynamic_cast<CDeviceASIO*>(this);
} // Destroy


tbool CDeviceASIO::LoadDriver(const tchar* pszName, tbool bEnableInput, tbool bEnableOutput)
{
	//mDrivers.loadDriver((char*)pszName);
	if (!mDrivers.loadDriver((char*)pszName))
		return FALSE;

	if (ASIOInit() != 0) {
		return FALSE;
	}

	mbEnableInput = bEnableInput;
	mbEnableOutput = bEnableOutput;

	return TRUE;
} // LoadDriver


tbool CDeviceASIO::Initialize(tint32 iSampleRate, tint iHWBufferSize)
{
	if (theAsioDriver == NULL)
		return false;

	InitASIOStaticData(iSampleRate, iHWBufferSize);

	// set up the asioCallback structure and create the ASIO data buffer
	mASIOCallbacks.bufferSwitch = &BufferSwitchWrap;
	mASIOCallbacks.sampleRateDidChange = &SampleRateChangedWrap;
	mASIOCallbacks.asioMessage = &ASIOMessagesWrap;
	mASIOCallbacks.bufferSwitchTimeInfo = &BufferSwitchTimeInfoWrap;

	CreateASIOBuffers();

	// Lasse - ASIO Initialize never fails on sample-rate and buffersize, since it adjusts rate if needed 
	// and ignores buffer-size setting.
	tbool bIsRateAndBuffOK = true;

	if (GetInputChannels() <= 0) mbEnableInput = false;
	if (GetOutputChannels() <= 0) mbEnableOutput = false;

	return (bIsRateAndBuffOK && (mbEnableInput || mbEnableOutput));
} // Initialize


tbool CDeviceASIO::Start()
{
	tbool bSuccess = false;

	if (theAsioDriver) {
		ASIOError err = theAsioDriver->start();
		if (err == ASE_OK)
			bSuccess = true;
	}

	return bSuccess;
} // Start


tint32 CDeviceASIO::GetInputChannels()
{
	if (theAsioDriver == NULL)
		return -1;

	if (!mbEnableInput)
		return 0;

	ASIOError err = theAsioDriver->getChannels((long*)&miChannelsIn, (long*)&miChannelsOut);
	if (err != ASE_OK)
		return -1;

	return miChannelsIn;
} // GetInputChannels


tint32 CDeviceASIO::GetOutputChannels()
{
	if (theAsioDriver == NULL)
		return -1;

	if (!mbEnableOutput)
		return 0;

	ASIOError err = theAsioDriver->getChannels((long*)&miChannelsIn, (long*)&miChannelsOut);
	if (err != ASE_OK)
		return -1;

	return miChannelsOut;
} // GetOutputChannels


void CDeviceASIO::SetCallback(IDeviceCallback* pCallback)
{
	mpCallback = pCallback;
} // SetCallback


ASIOError CDeviceASIO::ASIOInit()
{
#if MAC || SGI || SUN || BEOS || LINUX
	if(theAsioDriver) {
		delete theAsioDriver;
		theAsioDriver = 0;
	}		

	mInfo.driverVersion = 0;
	strcpy(mInfo.name, "No ASIO Driver");
	theAsioDriver = getDriver();

	if(!theAsioDriver) {
		strcpy(mInfo.errorMessage, "Not enough memory for the ASIO driver!"); 
		return ASE_NotPresent;
	}

	if(!theAsioDriver->init(mInfo.sysRef)) {
		theAsioDriver->getErrorMessage(mInfo.errorMessage);
		delete theAsioDriver;
		theAsioDriver = 0;
		return ASE_NotPresent;
	}

	strcpy(mInfo.errorMessage, "No ASIO Driver Error");
	theAsioDriver->getDriverName(mInfo.name);
	mInfo.driverVersion = theAsioDriver->getDriverVersion();
	return ASE_OK;

#else
	mInfo.driverVersion = 0;
	strcpy(mInfo.name, "No ASIO Driver");

	//if ((theAsioDriver) && (mInfo.sysRef)) {
	if (theAsioDriver) {
		tbool bInitError = false;
		try {
			if(!theAsioDriver->init(mInfo.sysRef)) {
				theAsioDriver->getErrorMessage(mInfo.errorMessage);
				theAsioDriver = 0;
				return ASE_NotPresent;
			}
		}
		catch (...) {
			bInitError = true;
		}
		if (bInitError) {
			sprintf(mInfo.errorMessage, "Exception caught");
			try {
				theAsioDriver->getErrorMessage(mInfo.errorMessage);
			}
			catch (...)
			{}

			theAsioDriver = 0;
			return ASE_NotPresent;
		}

		strcpy(mInfo.errorMessage, "No ASIO Driver Error");
		theAsioDriver->getDriverName(mInfo.name);
		mInfo.driverVersion = theAsioDriver->getDriverVersion();
		return ASE_OK;
	}

	return ASE_NotPresent;

#endif	// !MAC
} // ASIOInit


void CDeviceASIO::InitASIOStaticData(tint32 iSampleRate, tint iBufferSize)
{
	theAsioDriver->getChannels((long*)&miChannelsIn, (long*)&miChannelsOut);

	theAsioDriver->getBufferSize((long*)&miBufferSizeMin, (long*)&miBufferSizeMax, (long*)&miBufferSizePreferred, (long*)&miBufferSizeGranularity);

	// Just use the preferred size
	miHWBufferSize = miBufferSizePreferred;

	// Get the currently selected sample rate (for for info, we don't need it)
	double dSampleRateCur;
	theAsioDriver->getSampleRate(&dSampleRateCur);

	// Set the sample rate
	theAsioDriver->setSampleRate((double)iSampleRate);

	// Get the currently selected sample rate (maybe changing sample rate failed)
	theAsioDriver->getSampleRate(&mfSampleRate);

	// Check wether the driver requires the ASIOOutputReady() optimization
	// (can be used by the driver to reduce output latency by one block)
	if(theAsioDriver->outputReady() == ASE_OK) {
		mbPostOutput = true;
	}
	else {
		mbPostOutput = false;
	}
} // InitASIOStaticData


tuint32 CDeviceASIO::GetPreferredBufferSize()
{
	// Exit gracefully on error (don't crash here at least!)
	if (theAsioDriver == NULL)
		return 0;

	// Get preferred size
	long lMin_Dummy, lMax_Dummy, lPreferred, lGranularity_Dummy;
	theAsioDriver->getBufferSize(&lMin_Dummy, &lMax_Dummy, &lPreferred, &lGranularity_Dummy);
	return lPreferred;
} // GetPreferredBufferSize


tuint32 CDeviceASIO::GetBufferSize()
{
	// This value isn't valid until after Initialize - but that's ok
	return miHWBufferSize;
} // GetBufferSize


tuint32 CDeviceASIO::GetMinBufferSize()
{
	return GetPreferredBufferSize();
} // GetMinBufferSize


tuint32 CDeviceASIO::GetMaxBufferSize()
{
	return GetPreferredBufferSize();
} // GetMaxBufferSize


tuint32 CDeviceASIO::GetPreferredSampleRate()
{
	return 44100;
} // GetPreferredSampleRate


tuint32 CDeviceASIO::GetSampleRate()
{
	return Float2Int(mfSampleRate);
} // GetSampleRate


tuint32 CDeviceASIO::EnumeratePossibleSampleRates(tuint32 uiLast)
{
	switch (uiLast) {
		case 0:		return 44100;
		case 44100:	return 48000;
		case 48000:	return 88200;
		case 88200:	return 96000;
		default:	return 0;
	}
} // EnumeratePossibleSampleRates


void CDeviceASIO::ShowSettings()
{
	theAsioDriver->controlPanel();
} // ShowSettings


tuint32 CDeviceASIO::GetTimeStamp()
{
	LARGE_INTEGER TimeNow;
	::QueryPerformanceCounter(&TimeNow);

	__int64 iTimeDelta = TimeNow.QuadPart - mTimeLast.QuadPart;
	iTimeDelta *= (__int64)mfSampleRate;
	iTimeDelta /= mPerfFreq.QuadPart;

	double dTimeDelta = (double)(TimeNow.QuadPart - mTimeLast.QuadPart);
	dTimeDelta /= (double)mPerfFreq.QuadPart;
	dTimeDelta *= mfSampleRate;

	return (tuint32)iTimeDelta;
} // GetTimeStamp


#ifdef WIN32
#pragma pack(push, 1)
#endif	// WIN32
struct SSample24
{
	unsigned char a;
	unsigned char b;
	char c;
} PACKED;
#ifdef WIN32
#pragma pack(pop)
#endif	// WIN32


ASIOTime* CDeviceASIO::BufferSwitchTimeInfo(ASIOTime* pTimeInfo, long iIndex, ASIOBool bProcessNow)
{
/*	if (mbPostOutput) {
		theAsioDriver->outputReady();
	}
	return 0L;*/
//	LARGE_INTEGER timeBegin;
//	::QueryPerformanceCounter(&timeBegin);

	// Update current time
	::QueryPerformanceCounter(&mTimeLast);

	// the actual processing callback.
	// Beware that this is normally in a seperate thread, hence be sure that you take care
	// about thread synchronization. This is omitted here for simplicity.
	static tint processedSamples = 0;

	int iInputIndex = 0;
	int iOutputIndex = 0;

	// Allocate buffers if not already allocated
	if (mppfInputs == NULL) {
		mppfInputs = new float*[ASIO_MAX_INPUT_CHANNELS];
		int ii;
		for (ii = 0; ii < ASIO_MAX_INPUT_CHANNELS; ii++) {
			mppfInputs[ii] = new float[miHWBufferSize];
		}
		mppfOutputs = new float*[ASIO_MAX_OUTPUT_CHANNELS];
		for (ii = 0; ii < ASIO_MAX_OUTPUT_CHANNELS; ii++) {
			mppfOutputs[ii] = new float[miHWBufferSize];
		}
	}

	ASIOChannelInfo Info;
	Info.channel = 0;
	Info.isInput = false;
//	ASIOGetChannelInfo(&Info);
	theAsioDriver->getChannelInfo(&Info);

	// Perform the processing
	int i;
	for (i = 0; i < miChannelsIn + miChannelsOut; i++) {
		if (mBuffers[i].isInput == false) {
//			mppfOutputs[iOutputIndex++] = (float*)(mBuffers[i].buffers[iIndex]);
		}
		else {
			float* pfDest = mppfInputs[iInputIndex];
			long* plSrc = (long*)(mBuffers[i].buffers[iIndex]);
			for (int ii = 0; ii < miHWBufferSize; ii++) {
				pfDest[ii] = (float)(plSrc[ii] / 2147483648.0);
			}

			iInputIndex++;
		}
	}

	if (mpCallback) {
		mpCallback->ProcessAudio(mppfOutputs, (const tfloat**)mppfInputs, miHWBufferSize);
	}

	// Convert from float to 32LSB
	iOutputIndex = 0;
	for (i = 0; i < miChannelsIn + miChannelsOut; i++) {
		if (mBuffers[i].isInput == false) {
			ASIOConvertSamples Converter;
			switch(Info.type) {
				case ASIOSTInt24LSB:
//					Converter.float32toInt24inPlace(mppfOutputs[iOutputIndex], miHWBufferSize);
//					memcpy(mBuffers[i].buffers[iIndex], mppfOutputs[iOutputIndex], miHWBufferSize * 3);
					{
						SSample24* pDest = (SSample24*)(mBuffers[i].buffers[iIndex]);
						if (mbEnableOutput) {
							float* pSrc = mppfOutputs[iOutputIndex];
							tint32 iIndex;
							for (iIndex = 0; iIndex < miHWBufferSize; iIndex++) {
								tfloat32 f = pSrc[iIndex];
								tint32 i = (tint32)(f * 8388607);
								SSample24 s;
								s.a = i & 0xff;
								s.b = (i >> 8) & 0xff;
								s.c = (i >> 16) & 0xff;
								pDest[iIndex] = s;
							}
						}
						else {
							// Silence
							memset(pDest, '\0', miHWBufferSize * 3);
						}
					}
					break;

				case ASIOSTInt32LSB:
					{
						void* pDest = mBuffers[i].buffers[iIndex];
						if (mbEnableOutput) {
//							Converter.float32toInt32inPlace(mppfOutputs[ii], miHWBufferSize);
							memcpy(pDest, mppfOutputs[iOutputIndex], miHWBufferSize * 4);
							Converter.float32toInt32inPlace((float*)(mBuffers[i].buffers[iIndex]), miHWBufferSize);
						}
						else {
							// Silence
							memset(pDest, '\0', miHWBufferSize * 4);
						}
					}
					break;

				default:
					// Unknown format
					ASSERT(0);
			}

			iOutputIndex++;
		}
	}

	// If the driver supports the ASIOOutputReady() optimization, do it here, all data are in place
	if (mbPostOutput) {
		theAsioDriver->outputReady();
	}

/*	LARGE_INTEGER timeEnd;
	::QueryPerformanceCounter(&timeEnd);

	__int64 iTimeDelta = timeEnd.QuadPart - timeBegin.QuadPart;
	double dTimeSeconds = (double)(timeEnd.QuadPart - timeBegin.QuadPart) / (double)gpIO->mPerfFreq.QuadPart;
	double dTime100Percent = (double)buffSize / gpIO->miSampleRate;
	gpIO->mfCPUTimeLast = (float)(dTimeSeconds / dTime100Percent);*/

	return 0L;
} // BufferSwitchTimeInfo


ASIOTime* CDeviceASIO::BufferSwitchTimeInfoWrap(ASIOTime* pTimeInfo, long iIndex, ASIOBool bProcessNow)
{
	return gpDevice->BufferSwitchTimeInfo(pTimeInfo, iIndex, bProcessNow);
} // BufferSwitchTimeInfoWrap


void CDeviceASIO::BufferSwitchWrap(long iIndex, ASIOBool bProcessNow)
{
	// As this is a "back door" into the bufferSwitchTimeInfo a timeInfo needs to be created
	// though it will only set the timeInfo.samplePosition and timeInfo.systemTime fields and the according flags
	ASIOTime TimeInfo;
	memset(&TimeInfo, 0, sizeof(TimeInfo));

	// Get the time stamp of the buffer
	theAsioDriver->getSamplePosition(&TimeInfo.timeInfo.samplePosition,
		&TimeInfo.timeInfo.systemTime);
	TimeInfo.timeInfo.flags = kSystemTimeValid | kSamplePositionValid;

	BufferSwitchTimeInfoWrap(&TimeInfo, iIndex, bProcessNow);
} // BufferSwitchWrap


void CDeviceASIO::SampleRateChangedWrap(ASIOSampleRate Rate)
{
}

long CDeviceASIO::ASIOMessages(long iSelector, long iValue, void* /*pMessage*/, double* /*pOpt*/)
{
	long ret = 0;

	switch(iSelector)
	{
		case kAsioSelectorSupported:
			if(iValue == kAsioResetRequest
			|| iValue == kAsioEngineVersion
			|| iValue == kAsioResyncRequest
			|| iValue == kAsioLatenciesChanged
			// the following three were added for ASIO 2.0, you don't necessarily have to support them
			|| iValue == kAsioSupportsTimeInfo
			|| iValue == kAsioSupportsTimeCode
			|| iValue == kAsioSupportsInputMonitor)
				ret = 1L;
			break;
		case kAsioResetRequest:
			// defer the task and perform the reset of the driver during the next "safe" situation
			// You cannot reset the driver right now, as this code is called from the driver.
			// Reset the driver is done by completely destruct is. I.e. ASIOStop(), ASIODisposeBuffers(), Destruction
			// Afterwards you initialize the driver again.
//			mInfo.stopped;  // In this sample the processing will just stop
			ret = 1L;
			break;
		case kAsioResyncRequest:
			// This informs the application, that the driver encountered some non fatal data loss.
			// It is used for synchronization purposes of different media.
			// Added mainly to work around the Win16Mutex problems in Windows 95/98 with the
			// Windows Multimedia system, which could loose data because the Mutex was hold too long
			// by another thread.
			// However a driver can issue it in other situations, too.
			ret = 1L;
			break;
		case kAsioLatenciesChanged:
			// This will inform the host application that the drivers were latencies changed.
			// Beware, it this does not mean that the buffer sizes have changed!
			// You might need to update internal delay data.
			ret = 1L;
			break;
		case kAsioEngineVersion:
			// return the supported ASIO version of the host application
			// If a host applications does not implement this selector, ASIO 1.0 is assumed
			// by the driver
			ret = 2L;
			break;
		case kAsioSupportsTimeInfo:
			// informs the driver wether the asioCallbacks.bufferSwitchTimeInfo() callback
			// is supported.
			// For compatibility with ASIO 1.0 drivers the host application should always support
			// the "old" bufferSwitch method, too.
			ret = 1;
			break;
		case kAsioSupportsTimeCode:
			// informs the driver wether application is interested in time code info.
			// If an application does not need to know about time code, the driver has less work
			// to do.
			ret = 0;
			break;
	}
	return ret;
} // ASIOMessages


long CDeviceASIO::ASIOMessagesWrap(long iSelector, long iValue, void* pMessage, double* pOpt)
{
	return gpDevice->ASIOMessages(iSelector, iValue, pMessage, pOpt);
}


void CDeviceASIO::CreateASIOBuffers()
{
	tint i;

	// Fill the bufferInfos from the start without a gap
	ASIOBufferInfo* pInfo = mBuffers;

	// Prepare inputs
	if (miChannelsIn > ASIO_MAX_INPUT_CHANNELS) {
		miChannelsIn = ASIO_MAX_INPUT_CHANNELS;
	}

	for (i = 0; i < miChannelsIn; i++, pInfo++) {
		pInfo->isInput = ASIOTrue;
		pInfo->channelNum = i;
		pInfo->buffers[0] = pInfo->buffers[1] = 0;
	}

	// Prepare outputs
	if (miChannelsOut > ASIO_MAX_OUTPUT_CHANNELS) {
		miChannelsOut = ASIO_MAX_OUTPUT_CHANNELS;
	}

	for(i = 0; i < miChannelsOut; i++, pInfo++)
	{
		pInfo->isInput = ASIOFalse;
		pInfo->channelNum = i;
		pInfo->buffers[0] = pInfo->buffers[1] = 0;
	}

	ASIOError err = theAsioDriver->createBuffers(mBuffers,
		miChannelsIn + miChannelsOut,
		miHWBufferSize,
		&mASIOCallbacks);
} // CreateASIOBuffers
