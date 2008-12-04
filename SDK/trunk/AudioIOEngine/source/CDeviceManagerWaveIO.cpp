/*!	\file CDeviceManagerWaveIO.cpp
	\author Lasse Steen Bohnstedt
	\date 2008-05-02
*/

#include "aioeInternal.h"


CDeviceManagerWaveIO::CDeviceManagerWaveIO()
{
	muiNbOfDrivers_Outputs = waveOutGetNumDevs();
	tuint32 ui;
	for (ui = 0; ui<muiNbOfDrivers_Outputs; ui++) {
		UINT uiDevID = ui;
		SMyWaveOutCaps* p = new SMyWaveOutCaps();
		MMRESULT mmres = waveOutGetDevCaps((UINT_PTR)uiDevID, &(p->wocaps), sizeof(WAVEOUTCAPS));
		if (mmres == MMSYSERR_NOERROR) {
			if (p->wocaps.dwFormats & WAVE_FORMAT_4S16)
				p->b44100 = TRUE;
			p->b48000 = CDeviceWaveIO::TestWaveOutCaps(&uiDevID, 48000);
			p->b88200 = CDeviceWaveIO::TestWaveOutCaps(&uiDevID, 88200);
			if (p->wocaps.dwFormats & WAVE_FORMAT_96S16)
				p->b96000 = TRUE;
#ifdef _DEBUG
			tchar pszDevName[1024];
			strcpy(pszDevName, p->wocaps.szPname);
			WORD wChannels = p->wocaps.wChannels;
			std::cout << "CDeviceManagerWaveIO::CDeviceManagerWaveIO() - Add device " << uiDevID << " = '" << pszDevName << "', " << wChannels << " channels.\n";
#endif _DEBUG
		}
		else {
			std::cerr << "CDeviceManagerWaveIO::CDeviceManagerWaveIO() - Unable to add device " << uiDevID << ".\n";
		}
		mlistpMyWaveOutCaps.insert(mlistpMyWaveOutCaps.end(), p);
	}

	muiNbOfDrivers_Total = muiNbOfDrivers_Outputs + waveInGetNumDevs();
	for (ui = muiNbOfDrivers_Outputs; ui < muiNbOfDrivers_Total; ui++) {
		UINT uiDevID = ui - muiNbOfDrivers_Outputs;
		SWaveInCaps* p = new SWaveInCaps();
		MMRESULT mmres = waveInGetDevCaps((UINT_PTR)uiDevID, &(p->wicaps), sizeof(WAVEINCAPS));
		if (mmres == MMSYSERR_NOERROR) {
#ifdef _DEBUG
			tchar pszDevName[1024];
			strcpy(pszDevName, p->wicaps.szPname);
			WORD wChannels = p->wicaps.wChannels;
			std::cout << "CDeviceManagerWaveIO::CDeviceManagerWaveIO() - Add device " << uiDevID << " = '" << pszDevName << "', " << wChannels << " channels.\n";
#endif _DEBUG
		}
		else {
			std::cerr << "CDeviceManagerWaveIO::CDeviceManagerWaveIO() - Unable to add device " << uiDevID << ".\n";
		}
		mWaveInCaps.push_back(p);
	}
} // constructor


CDeviceManagerWaveIO::~CDeviceManagerWaveIO()
{
} // destructor


IDeviceManager* IDeviceManager::CreateWaveIO()
{
	return dynamic_cast<IDeviceManager*>(new CDeviceManagerWaveIO());
} // CreateWaveIO


void CDeviceManagerWaveIO::Destroy()
{
	delete dynamic_cast<CDeviceManagerWaveIO*>(this);
} // Destroy


tint CDeviceManagerWaveIO::GetNrOfDevices()
{
	return (tint)muiNbOfDrivers_Total;
} // GetNrOfDevices


tbool CDeviceManagerWaveIO::GetDeviceName(tint iIndex, tchar* pszName)
{
	pszName[0] = '\0';

	if ((iIndex < -1) || (iIndex >= (tint)muiNbOfDrivers_Total))
		return false;

	if ((iIndex == -1) && (muiNbOfDrivers_Outputs > 0)) {
		strcpy(pszName, "WAVE: Default mapper");
	}
	else {
		std::list<SMyWaveOutCaps*>::iterator it = mlistpMyWaveOutCaps.begin();
		tint i;
		for (i = 0; i <= iIndex && it != mlistpMyWaveOutCaps.end(); i++, it++) {
			if (i == iIndex) {
				SMyWaveOutCaps* p = *it;
				WAVEOUTCAPS* pwocaps = &(p->wocaps);
				tint32 iAfter = sprintf(pszName, "WAVE: ");
				tchar* pszAfter = pszName + iAfter;
				sprintf(pszAfter, pwocaps->szPname);
			}
		}
		std::list<SWaveInCaps*>::iterator it2 = mWaveInCaps.begin();
		for (; i <= iIndex && it2 != mWaveInCaps.end(); i++, it2++) {
			if (i == iIndex) {
				SWaveInCaps* p = *it2;
				WAVEINCAPS* pwicaps = &(p->wicaps);
				tint32 iAfter = sprintf(pszName, "WAVE: ");
				tchar* pszAfter = pszName + iAfter;
				sprintf(pszAfter, pwicaps->szPname);
			}
		}
	}

	return true;
} // GetDeviceName


tbool CDeviceManagerWaveIO::GetDeviceName_InputsOnly(tint iIndex, tchar* pszName)
{
	if (!DeviceMayHaveInput(iIndex)) {
		*pszName = '\0';
		return false;
	}

	return GetDeviceName(iIndex, pszName);
} // GetDeviceName_InputsOnly


tbool CDeviceManagerWaveIO::GetDeviceName_OutputsOnly(tint iIndex, tchar* pszName)
{
	if (!DeviceMayHaveOutput(iIndex)) {
		*pszName = '\0';
		return false;
	}

	return GetDeviceName(iIndex, pszName);
} // GetDeviceName_OutputsOnly


IDevice* CDeviceManagerWaveIO::CreateDevice(tint iIndex, tbool bEnableInput /*= false*/, tbool bEnableOutput /*= true*/)
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

	//dynamic_cast<CDeviceWaveIO*>(pRet)->LoadDriver(pszName);
	try {
		IDevice* pRet = IDevice::CreateWaveIO();
		CDeviceWaveIO* pX = dynamic_cast<CDeviceWaveIO*>(pRet);
		if (pX == NULL)
			return NULL;
		// Maybe change index to be input-only index
		if ((iIndex != -1) && (bEnableInput)) {
			iIndex -= muiNbOfDrivers_Outputs;
		}
		if (!pX->LoadDriver(iIndex, bEnableInput, bEnableOutput)) {
			pRet->Destroy();
			return NULL;
		}

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

	// Fallthrough error
	return NULL;
} // CreateDevice


tbool CDeviceManagerWaveIO::DeviceMayHaveOutput(tint iIndex)
{
	return ((iIndex >= -1) && (iIndex < (tint)muiNbOfDrivers_Outputs));
} // DeviceMayHaveOutput


tbool CDeviceManagerWaveIO::DeviceMayHaveInput(tint iIndex)
{
	return ((iIndex == -1) || (iIndex >= (tint)muiNbOfDrivers_Outputs));
} // DeviceMayHaveInput
