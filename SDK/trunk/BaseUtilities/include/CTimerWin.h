/*!	\file CTimerWin.h
	\author Michael Olsen
	\date 13/Nov/2006
*/


/*! \class CTimerWin
 * \brief Win32 implementation of ITimer
*/
class CTimerWin : public virtual ITimer
{
public:
	//! Constructor
	CTimerWin();
	//! Destructor
	virtual ~CTimerWin();

	//! IDestructable override
	virtual void Destroy();

	//! ITimer override
	virtual void Init(tint32 iID, ITimerCallback* pCallback, tint32 iTimeMS);

	//! ITimer override
	virtual void Stop();

	//! Called when the extra thread starts
	virtual void ThreadWrap();

protected:
	//! Callback to call on timer events
	ITimerCallback* mpCallback;

	ITimerCallback_CanStop* mpCallback_CanStop;

	//! ID to pass to callback
	tint32 miID;

	//! Ms. between each timer
	tint32 miSleepTimeMS;

	static void __cdecl Thread(void* p);

	//! True if thread has stopped
	volatile tbool mbThreadStopped;

	//! True if thread should stop
	volatile tbool mbStopThread;
};






