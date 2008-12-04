/*!	\file CDeviceManagerASIO.cpp
	\author Michael Olsen
	\date 09/Feb/2005
	\date 09/Feb/2005
*/

#include "aioeInternal.h"

CDeviceManagerASIO::CDeviceManagerASIO()
{
}

CDeviceManagerASIO::~CDeviceManagerASIO()
{
}

IDeviceManager* IDeviceManager::CreateASIO()
{
	return dynamic_cast<IDeviceManager*>(new CDeviceManagerASIO());
}

void CDeviceManagerASIO::Destroy()
{
	delete dynamic_cast<CDeviceManagerASIO*>(this);
}

tint CDeviceManagerASIO::GetNrOfDevices()
{
	tint32 iDevices = mDriverList.asioGetNumDev();

	tint32 iDevice;
	tint32 iTmp = iDevices;
	for (iDevice = 0; iDevice < iTmp; iDevice++) {
		tchar psz[256];
		mDriverList.asioGetDriverName(iDevice, psz, 255);

		if (strcmp(psz, "ASIO DirectX Full Duplex Driver") == 0) {
			iDevices--;
		}
		if (strcmp(psz, "ASIO Multimedia Driver") == 0) {
			iDevices--;
		}
	}

	return iDevices;
} // GetNrOfDevices


tbool CDeviceManagerASIO::GetDeviceName(tint iIndex, tchar* pszName)
{
/*	tint32 iAfter = sprintf(pszName, "ASIO: ");
	tchar* pszAfter = pszName + iAfter;
	mDriverList.asioGetDriverName(iIndex, (char*)pszAfter, 129 - iAfter);
	if (*pszAfter == '\0')
		*pszName = '\0';*/

/*	mDriverList.asioGetDriverName(iIndex, pszName, 129);*/

	tint32 iOffset = 0;
	tint32 iDevice;
	for (iDevice = 0; iDevice <= iIndex + iOffset; iDevice++) {
		tchar psz[256];
		if (mDriverList.asioGetDriverName(iDevice, psz, 255) == 0) {
			if (strcmp(psz, "ASIO DirectX Full Duplex Driver") == 0) {
				iOffset++;
			}
			if (strcmp(psz, "ASIO Multimedia Driver") == 0) {
				iOffset++;
			}
		}
	}

	if (mDriverList.asioGetDriverName(iIndex + iOffset, pszName, 129) != 0) {
		*pszName = '\0';
		return false;
	}

	return true;
} // GetDeviceName


tbool CDeviceManagerASIO::GetDeviceName_InputsOnly(tint iIndex, tchar* pszName)
{
	if (!DeviceMayHaveInput(iIndex)) {
		*pszName = '\0';
		return false;
	}

	return GetDeviceName(iIndex, pszName);
} // GetDeviceName_InputsOnly


tbool CDeviceManagerASIO::GetDeviceName_OutputsOnly(tint iIndex, tchar* pszName)
{
	if (!DeviceMayHaveOutput(iIndex)) {
		*pszName = '\0';
		return false;
	}

	return GetDeviceName(iIndex, pszName);
} // GetDeviceName_OutputsOnly


IDevice* CDeviceManagerASIO::CreateDevice(tint iIndex, tbool bEnableInput /*= false*/, tbool bEnableOutput /*= true*/)
{
	tchar pszName[129];
	GetDeviceName(iIndex, pszName);

	//dynamic_cast<CDeviceASIO*>(pRet)->LoadDriver(pszName);
	try {
		IDevice* pRet = IDevice::CreateASIO();
		CDeviceASIO* pX = dynamic_cast<CDeviceASIO*>(pRet);
		if (pX == NULL)
			return NULL;
		if (!pX->LoadDriver(pszName, bEnableInput, bEnableOutput)) {
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
		return NULL;
	}

	return NULL;
} // CreateDevice


tbool CDeviceManagerASIO::DeviceMayHaveOutput(tint iIndex)
{
	tbool bNameContainsInput, bNameContainsOutput;
	TestDeviceName_InputOutput(iIndex, &bNameContainsInput, &bNameContainsOutput);
	return (bNameContainsOutput || !bNameContainsInput);
} // DeviceMayHaveOutput


tbool CDeviceManagerASIO::DeviceMayHaveInput(tint iIndex)
{
	tbool bNameContainsInput, bNameContainsOutput;
	TestDeviceName_InputOutput(iIndex, &bNameContainsInput, &bNameContainsOutput);
	return (bNameContainsInput || !bNameContainsOutput);
} // DeviceMayHaveInput


void CDeviceManagerASIO::TestDeviceName_InputOutput(tint iIndex, tbool* pbNameContainsInput, tbool* pbNameContainsOutput)
{
	tchar pszName[512];
	GetDeviceName(iIndex, pszName);
	strlwr(pszName);
	tchar* pszInput = strstr(pszName, "input");
	tchar* pszOutput = strstr(pszName, "output");
	*pbNameContainsInput = (pszInput != NULL);
	*pbNameContainsOutput = (pszOutput != NULL);
} // TestDeviceName_InputOutput
