/*!	\file ITimer.h
	\author Michael Olsen
	\date 13/Nov/2006
*/

/*! \class ITimer
 * \brief Timer interface
 *
 * This is not a high-precision timer, i.e. there's no guarantee about the excact timing. The time runs until the object is destroyed. After the object has been destroyed, the timer is guaranteed to have stopped.
*/
class ITimer : public virtual IDestructable
{
public:
	//! Creates ITimer
	static ITimer* Create();

	//! Initialize and starts timer.
	/*!
		\param iTimerID [in]: ID of timer, passed to callback.
		\param pCallback [in]: Callback to call on timer events.
		\param iTimeMS [in]: Time in between each timer calls.
	*/
	virtual void Init(tint32 iTimerID, ITimerCallback* pCallback, tint32 iTimeMS) = 0;

	//! Stops the running timer. When method returns, the timer is guaranteed to have stopped.
	virtual void Stop() = 0;
};




