/*!	\file IEventReceiver.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

// Forward declarations
class IControl;

/*! \class IEventReceiver
 * \brief Callback interface to support even notification
 *
 * Any class that receives callbacks from the GUI engine, should inherit from this interface
*/
class IEventReceiver : public virtual IInterface
{
public:
	//! Called when controls value changes
	/*!
		\param pSender [in]: Control where notification originates from
		\param iValueNew [in]: New value. If control has a converter attached, the value has been converted
	*/
	virtual void EventValueChange(IControl *pSender, tint32 iValueNew) = 0;

	//! Called when a generic event occurs
	/*!
		\param pSender [in]: Control where notification originates from
		\param pEventData [in]: Pointer to event data (even specific)
	*/
	virtual void EventGeneric(IControl *pSender, void *pEventData) = 0;

	//! Called when the user starts editing control (i.e. clicks mouse down on knob etc.)
	virtual void EventEditBegin() = 0;

	//! Called when the user stops editing control (i.e. releases mouse after turning knob etc.)
	virtual void EventEditEnd() = 0;
};








