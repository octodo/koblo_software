/*!	\file CTimerWin.cpp
	\author Michael Olsen
	\date 13/Nov/2006
*/


#include "buInternalOS.h"

#include <process.h>


CTimerWin::CTimerWin()
	: mpCallback(NULL), mpCallback_CanStop(NULL), miID(-1), miSleepTimeMS(0)
{
	mbThreadStopped = true;
	mbStopThread = true;
}

CTimerWin::~CTimerWin()
{
}

ITimer* ITimer::Create()
{
	return dynamic_cast<ITimer*>(new CTimerWin());
}

void CTimerWin::Destroy()
{
	Stop();

	delete dynamic_cast<CTimerWin*>(this);
}

void CTimerWin::Stop()
{
	if (mpCallback) {
		mbStopThread = true;

		while (!mbThreadStopped) {
			::Sleep(1);
		}
		mpCallback = NULL;
		mpCallback_CanStop = NULL;
	}
}

void CTimerWin::Init(tint32 iID, ITimerCallback* pCallback, tint32 iTimeMS)
{
	Stop();

	miID = iID;
	mpCallback = pCallback;
	ITimerCallback_CanStop* pTmp = dynamic_cast<ITimerCallback_CanStop*>(pCallback);
	mpCallback_CanStop = pTmp;
	miSleepTimeMS = iTimeMS;

	mbStopThread = false;
	mbThreadStopped = false;

	_beginthread(Thread, 8192, dynamic_cast<CTimerWin*>(this));
}

void CTimerWin::ThreadWrap()
{
	//ASSERT(mpCallback);

	if (mpCallback_CanStop != NULL) {
		while (1) {
			if (!mpCallback_CanStop->OnTimer_CanStop(miID))
			{
				mbStopThread = true;
			}
			else {
				::Sleep(miSleepTimeMS);
			}

			if (mbStopThread) {
				break;
			}
		}
	}
	else if (mpCallback != NULL) {
		while (1) {
			mpCallback->OnTimer(miID);

			::Sleep(miSleepTimeMS);

			if (mbStopThread) {
				break;
			}
		}
	}

	mbThreadStopped = true;
}

void __cdecl CTimerWin::Thread(void* p)
{
	CTimerWin* pTimer = (CTimerWin*)p;

	pTimer->ThreadWrap();
}


