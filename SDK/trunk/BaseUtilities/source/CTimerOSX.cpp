/*!	\file CTimerOSX.cpp
	\author Michael Olsen
	\date 13/Nov/2006
*/


#include "buInternalOS.h"


CTimerOSX::CTimerOSX()
	: mpCallback(NULL), mpCallback_CanStop(NULL), miID(-1), miSleepTimeMS(0)
{
	mbThreadStopped = true;
	mbStopThread = true;
}

CTimerOSX::~CTimerOSX()
{
	Stop();
}

ITimer* ITimer::Create()
{
	return dynamic_cast<ITimer*>(new CTimerOSX());
}

void CTimerOSX::Destroy()
{
	Stop();

	delete dynamic_cast<CTimerOSX*>(this);
}

void CTimerOSX::Stop()
{
	if (mpCallback) {
		// Timer was initialized, i.e. started
		mbStopThread = true;

		::MPSemaphoreID Semaphore;
		::MPCreateSemaphore(1, 0, &Semaphore);
		while (mbThreadStopped == false) {
			::MPYield();
			::MPWaitOnSemaphore(Semaphore, 1 * kDurationMillisecond);
		}
	}
}

void CTimerOSX::Init(tint32 iID, ITimerCallback* pCallback, tint32 iTimeMS)
{
	Stop();

	miID = iID;
	mpCallback = pCallback;
	mpCallback_CanStop = dynamic_cast<ITimerCallback_CanStop*>(pCallback);
	miSleepTimeMS = iTimeMS;

	mbStopThread = false;
	mbThreadStopped = false;

	OpaqueMPTaskID* iTaskID;
	/*OSStatus Err =*/ ::MPCreateTask(&Thread, dynamic_cast<CTimerOSX*>(this), 8192, NULL, NULL, NULL, 0, &iTaskID);
}

void CTimerOSX::ThreadWrap()
{
	//ASSERT(mpCallback);

	::MPSemaphoreID Semaphore;
	::MPCreateSemaphore(1, 0, &Semaphore);

	if (mpCallback_CanStop != NULL) {
		while (1) {
			if (!mpCallback_CanStop->OnTimer_CanStop(miID)) {
				mbStopThread = true;
			}
			else {
				::MPWaitOnSemaphore(Semaphore, miSleepTimeMS * kDurationMillisecond);
			}

			if (mbStopThread) {
				break;
			}
		}
	}
	else if (mpCallback != NULL) {
		while (1) {
			mpCallback->OnTimer(miID);

			::MPWaitOnSemaphore(Semaphore, miSleepTimeMS * kDurationMillisecond);

			if (mbStopThread) {
				break;
			}
		}
	}

	mbThreadStopped = true;
	mpCallback = NULL;
	mpCallback_CanStop = NULL;
}

OSStatus CTimerOSX::Thread(void* p)
{
	CTimerOSX* pTimer = (CTimerOSX*)p;

	pTimer->ThreadWrap();
}


