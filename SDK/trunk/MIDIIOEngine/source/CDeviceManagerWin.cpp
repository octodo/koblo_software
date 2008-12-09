/*!	\file CDeviceManagerWin.cpp
	\author Michael Olsen
	\date 09/Feb/2005
	\date 09/Feb/2005
*/

#include "mioeInternal.h"

CDeviceManagerWin::CDeviceManagerWin()
{
}

CDeviceManagerWin::~CDeviceManagerWin()
{
}

IDeviceManager* IDeviceManager::Create()
{
	return dynamic_cast<IDeviceManager*>(new CDeviceManagerWin());
}

void CDeviceManagerWin::Destroy()
{
	delete dynamic_cast<CDeviceManagerWin*>(this);
}

tint CDeviceManagerWin::GetNrOfInputDevices()
{
	return ::midiInGetNumDevs();
}

void CDeviceManagerWin::GetInputDeviceName(tint iIndex, tchar* pszName)
{
	MIDIINCAPS caps;
	::midiInGetDevCaps(iIndex, &caps, sizeof(MIDIINCAPS));
	strcpy((char*)pszName, caps.szPname);
}

IIDevice* CDeviceManagerWin::CreateInputDevice(tint iIndex)
{
	IIDevice* pRet = IIDevice::Create(&iIndex);

	return pRet;
}

tint CDeviceManagerWin::GetNrOfOutputDevices()
{
	return ::midiOutGetNumDevs();
}

void CDeviceManagerWin::GetOutputDeviceName(tint iIndex, tchar* pszName)
{
	MIDIOUTCAPS caps;
	::midiOutGetDevCaps(iIndex, &caps, sizeof(MIDIOUTCAPS));
	strcpy((char*)pszName, caps.szPname);
}

IODevice* CDeviceManagerWin::CreateOutputDevice(tint iIndex)
{
	IODevice* pRet = IODevice::Create(&iIndex);

	return pRet;
}


