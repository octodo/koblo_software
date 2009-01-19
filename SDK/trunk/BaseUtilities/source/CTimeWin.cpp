/*!	\file CTimeWin.cpp
	\author Michael Olsen
	\date 06/May/2005
	\date 06/May/2005
*/


#include "buInternalOS.h"


CTimeWin::CTimeWin()
{
}

CTimeWin::~CTimeWin()
{
}

ITime* ITime::Create()
{
	return dynamic_cast<ITime*>(new CTimeWin());
}

void CTimeWin::Destroy()
{
	delete dynamic_cast<CTimeWin*>(this);
}

tuint32 ITime::GetTimeMS()
{
	return ::timeGetTime();
}


void ITime::SleepMS(tuint32 uiSleepMS)
{
	::Sleep(uiSleepMS);
}

