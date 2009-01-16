/*!	\file CTimerOSX.h
	\author Michael Olsen
	\date 13/Nov/2006
*/


/*! \class CTimerOSX
 * \brief OSX implementation of ITimer
*/
class CTimerOSX : public virtual ITimer
{
public:
	//! Constructor
	CTimerOSX();
	//! Destructor
	virtual ~CTimerOSX();

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

	static OSStatus Thread(void* p);

	//! True if thread has stopped
	volatile tbool mbThreadStopped;

	//! True if thread should stop
	volatile tbool mbStopThread;
};






