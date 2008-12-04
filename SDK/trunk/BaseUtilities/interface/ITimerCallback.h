/*!	\file ITimerCallback.h
	\author Michael Olsen
	\date 13/Nov/2006
*/

/*! \class ITimerCallback
 * \brief Callback for ITimer
 *
 * To receive an ITimer callback, inherit from this class
*/
class ITimerCallback
{
public:
	/*!
		\param iID [in]: ID of timer, passed to ITimer when created.
	*/
	virtual void OnTimer(tint32 iID) = 0;
};


class ITimerCallback_CanStop : public virtual ITimerCallback
{
public:
	/*!
		\param iID [in]: ID of timer, passed to ITimer when created.
	*/
	virtual void OnTimer(tint32 iID) = 0;

	/*!
		\param iID [in]: ID of timer, passed to ITimer when created.
		\return tbool: Return True to keep timer running, return false to stop it (doesn't destroy though)
	*/
	virtual tbool OnTimer_CanStop(tint32 iID) = 0;
};




