/*!	\file ITime.h
	\author Michael Olsen
	\date 06/May/2005
	\date 06/May/2005
*/

/*! \class ITime
 * \brief Interface for reporting of time
*/
class ITime : public virtual IDestructable
{
public:
	//! Creates ITime
	static ITime* Create();

	//! Returns the time in milli seconds. The time is not neccasarily excact, in that it may be have jitter causing it to be several ms. wrong.
	static tuint32 GetTimeMS();

	//! Pauses the current thread for an approximate amount of time (may be larger since other threads may take over control)
	/*!
		\param iSleepMS [in]: Delay in milli-seconds. Should be > 0 since sleeping 0 ms isn't documented for OS X
	*/
	static void SleepMS(tuint32 uiSleepMS);
};




