/*!	\file CTimeOSX.cpp
	\author Michael Olsen
	\date 06/May/2005
	\date 06/May/2005
*/


#include "buInternalOS.h"


CTimeOSX::CTimeOSX()
{
}

CTimeOSX::~CTimeOSX()
{
}

ITime* ITime::Create()
{
	return dynamic_cast<ITime*>(new CTimeOSX());
}

void CTimeOSX::Destroy()
{
	delete dynamic_cast<CTimeOSX*>(this);
}

tuint32 ITime::GetTimeMS()
{
	UnsignedWide Time;
	::Microseconds(&Time);
	
	tuint64 uiRet64 = *((tuint64*)&Time);
	uiRet64 /= 1000;

	return (tuint32)uiRet64;
}

void ITime::SleepMS(tuint32 uiSleepMS)
{
	::MPSemaphoreID Semaphore;
	::MPCreateSemaphore(1, 0, &Semaphore);
	::MPWaitOnSemaphore(Semaphore, uiSleepMS * kDurationMillisecond);
}

