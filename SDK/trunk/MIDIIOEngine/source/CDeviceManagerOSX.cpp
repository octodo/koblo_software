/*!	\file CDeviceManagerOSX.cpp
	\author Michael Olsen
	\date 13/Feb/2006
	\date 13/Feb/2006
*/

#include "mioeInternal.h"

CDeviceManagerOSX::CDeviceManagerOSX()
{
}

CDeviceManagerOSX::~CDeviceManagerOSX()
{
}

IDeviceManager* IDeviceManager::Create()
{
	return dynamic_cast<IDeviceManager*>(new CDeviceManagerOSX());
}

void CDeviceManagerOSX::Destroy()
{
	delete dynamic_cast<CDeviceManagerOSX*>(this);
}

tint CDeviceManagerOSX::GetNrOfInputDevices()
{
//	return ::MIDIGetNumberOfDevices();
	return ::MIDIGetNumberOfSources();
}

void CDeviceManagerOSX::GetInputDeviceName(tint iIndex, tchar* pszName)
{
//	CFStringRef pname, pmanuf, pmodel;
	CFStringRef pname;

//	MIDIDeviceRef dev = MIDIGetDevice(iIndex);
	MIDIEndpointRef dev = ::MIDIGetSource(iIndex);

	MIDIObjectGetStringProperty(dev, kMIDIPropertyName, &pname);
//	MIDIObjectGetStringProperty(dev, kMIDIPropertyManufacturer, &pmanuf);
//	MIDIObjectGetStringProperty(dev, kMIDIPropertyModel, &pmodel);

	CFStringGetCString(pname, (char*)pszName, 129, 0);
//	CFStringGetCString(pmanuf, manuf, sizeof(manuf), 0);
//	CFStringGetCString(pmodel, model, sizeof(model), 0);
	CFRelease(pname);
//	CFRelease(pmanuf);
//	CFRelease(pmodel);
}

IIDevice* CDeviceManagerOSX::CreateInputDevice(tint iIndex)
{
	IIDevice* pRet = IIDevice::Create(&iIndex);

	return pRet;
}

tint CDeviceManagerOSX::GetNrOfOutputDevices()
{
	return 0;
}

void CDeviceManagerOSX::GetOutputDeviceName(tint /*iIndex*/, tchar* /*pszName*/)
{
}

IODevice* CDeviceManagerOSX::CreateOutputDevice(tint /*iIndex*/)
{
	return 0;
}


