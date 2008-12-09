/*!	\file CDeviceManagerCoreAudio.cpp
	\author Michael Olsen
	\date 10/Feb/2006
	\date 10/Feb/2006
*/

#include "aioeInternal.h"


CDeviceManagerCoreAudio::CDeviceManagerCoreAudio()
{
}

CDeviceManagerCoreAudio::~CDeviceManagerCoreAudio()
{
}

IDeviceManager* IDeviceManager::CreateCoreAudio()
{
	return dynamic_cast<IDeviceManager*>(new CDeviceManagerCoreAudio());
}

void CDeviceManagerCoreAudio::Destroy()
{
	delete dynamic_cast<CDeviceManagerCoreAudio*>(this);
}

tint CDeviceManagerCoreAudio::GetNrOfDevices()
{
	Boolean isWritable;
	UInt32 dataSize;

	::AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices, &dataSize, &isWritable);
	void* pDeviceInfo = (void*) new char[dataSize];
	::AudioHardwareGetProperty(kAudioHardwarePropertyDevices, &dataSize, pDeviceInfo);
	tint iRet = dataSize / sizeof(AudioDeviceID);
	delete[] ((char*)pDeviceInfo);

	return iRet;
}

tbool CDeviceManagerCoreAudio::GetDeviceName(tint iIndex, tchar* pszName)
{
	AudioDeviceID theDevice;
	Boolean isWritable;
	UInt32 dataSize;

	OSStatus err = ::AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices, &dataSize, &isWritable);
	if (err != 0)
		return false;
	void* pDeviceInfo = (void*) new char[dataSize];
	err = ::AudioHardwareGetProperty(kAudioHardwarePropertyDevices, &dataSize, pDeviceInfo);
	if (err != 0)
		return false;
	theDevice = *(((AudioDeviceID*)pDeviceInfo) + iIndex);
	delete[] ((char*)pDeviceInfo);

	err = AudioDeviceGetPropertyInfo(theDevice, 0, FALSE, kAudioDevicePropertyDeviceName, &dataSize, &isWritable);
	if (err != 0)
		return false;
	err = AudioDeviceGetProperty(theDevice, 0, FALSE, kAudioDevicePropertyDeviceName, &dataSize, pszName);
	if (err != 0)
		return false;
	
	return true;
} // GetDeviceName


tbool CDeviceManagerCoreAudio::GetDeviceName_InputsOnly(tint iIndex, tchar* pszName)
{
	if (!DeviceMayHaveInput(iIndex)) {
		*pszName = '\0';
		return false;
	}

	return GetDeviceName(iIndex, pszName);
} // GetDeviceName_InputsOnly


tbool CDeviceManagerCoreAudio::GetDeviceName_OutputsOnly(tint iIndex, tchar* pszName)
{
	if (!DeviceMayHaveOutput(iIndex)) {
		*pszName = '\0';
		return false;
	}

	return GetDeviceName(iIndex, pszName);
} // GetDeviceName_OutputsOnly


IDevice* CDeviceManagerCoreAudio::CreateDevice(tint iIndex, tbool bEnableInput /*= false*/, tbool bEnableOutput /*= true*/)
{
	// Verify parameter sanity
	if (bEnableInput) {
		if (!DeviceMayHaveInput(iIndex))
			bEnableInput = false;
	}
	if (bEnableOutput) {
		if (!DeviceMayHaveOutput(iIndex))
			bEnableOutput = false;
	}
	if (!bEnableInput && !bEnableOutput) {
		// We can't do anything with this combination of parameters
		return NULL;
	}

	AudioDeviceID theDevice;
	Boolean isWritable;
	UInt32 dataSize;

	try {
		::AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices, &dataSize, &isWritable);
		void* pDeviceInfo = (void*) new char[dataSize];
		::AudioHardwareGetProperty(kAudioHardwarePropertyDevices, &dataSize, pDeviceInfo);
		theDevice = *(((AudioDeviceID*)pDeviceInfo) + iIndex);
		delete[] ((char*)pDeviceInfo);

		IDevice* pRet = IDevice::CreateCoreAudio();
		CDeviceCoreAudio* pX = dynamic_cast<CDeviceCoreAudio*>(pRet);
		if (pX == NULL) {
			if (pRet != NULL)
				pRet->Destroy();
			return NULL;
		}
		pX->SetAudioDeviceID(theDevice);
		pX->SetBehavior(bEnableInput, bEnableOutput);

		// Verify sanity after creation
		tint32 iInputChannels = pRet->GetInputChannels();
		tint32 iOutputChannels = pRet->GetOutputChannels();
		if (iInputChannels <= 0) bEnableInput = false;
		if (iOutputChannels <= 0) bEnableOutput = false;
		if (!bEnableInput && !bEnableOutput) {
			// Hm... device loaded, but it's not what we wanted
			pRet->Destroy();
			pRet = NULL;
		}

		return pRet;
	}
	catch (...) {
		// Don't care what particular error
	}

	// Error
	return NULL;
}


tbool CDeviceManagerCoreAudio::DeviceMayHaveOutput(tint iIndex)
{
	tbool bNameContainsInput, bNameContainsOutput;
	TestDeviceName_InputOutput(iIndex, &bNameContainsInput, &bNameContainsOutput);
	return (bNameContainsOutput || !bNameContainsInput);
} // DeviceMayHaveOutput


tbool CDeviceManagerCoreAudio::DeviceMayHaveInput(tint iIndex)
{
	tbool bNameContainsInput, bNameContainsOutput;
	TestDeviceName_InputOutput(iIndex, &bNameContainsInput, &bNameContainsOutput);
	return (bNameContainsInput || !bNameContainsOutput);
} // DeviceMayHaveInput


void CDeviceManagerCoreAudio::TestDeviceName_InputOutput(tint iIndex, tbool* pbNameContainsInput, tbool* pbNameContainsOutput)
{
	tchar pszName[512];
	GetDeviceName(iIndex, pszName);
	//strlwr(pszName); -- strlwr not in XCode, hm...
	tchar* pszInput = strcasestr(pszName, "input");
	tchar* pszMike = strcasestr(pszName, "microphone");
	tchar* pszOutput = strcasestr(pszName, "output");
	*pbNameContainsInput = ((pszInput != NULL) || (pszMike != NULL));
	*pbNameContainsOutput = (pszOutput != NULL);
} // TestDeviceName_InputOutput
