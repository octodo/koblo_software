/*!	\file CODeviceWin.cpp
	\author Michael Olsen
	\date 09/Feb/2005
	\date 09/Feb/2005
*/

#include "mioeInternal.h"

CODeviceWin::CODeviceWin(tint iIndex)
{
}

CODeviceWin::~CODeviceWin()
{
}

IODevice* IODevice::Create(void* p)
{
	tint iIndex = *((tint*)p);
	return dynamic_cast<IODevice*>(new CODeviceWin(iIndex));
}

void CODeviceWin::Destroy()
{
	delete dynamic_cast<CODeviceWin*>(this);
}
