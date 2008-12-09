/*!	\file IMouseTrap.h
	\author Lasse Steen Bohnstedt
	\date 27/Oct/2006
*/

/*! \class IMouseTrap.h
 * \brief This is an invisible control whose sole purpose is to listen for - and filter - mouse events.
 *
 *  After creating this control you must set its position and size, either manually (SetRelPos(..) and SetSize(...)) or
 *  by calling the PlaceOnTopOf(...) method.
 *  All mouse events that don't get "caught" (i.e. removed) by the IMouseTrap are passed on to the controls below it.
 *  The messages will be sent as SEventGeneric with event.eType = giEventMouse (catch in EventGeneric handler method).
*/
class IMouseTrap : public virtual ge::IControl
{
public:
	//! Create IMouseTrap
	static IMouseTrap* Create();


	//! OBSOLETE! Please use CloneRect(...) instead; this method just invokes CloneRect(pControl).<br>Moves the IMouseTrap control to the same position and resizes to the same size as another control.<br>To ensure that IMouseTrap actually gets the mouse events you still need to create it *after* the other control.
	/*!
		\param pControl [in]: The control to appear on top of.
	*/
	virtual void PlaceOnTopOf(ge::IControl* pControl) = 0;


	//! Makes MouseTrap listen for a specific type of mouse event - the event message will then be relayed back through NotifyGeneric(...) to listeners.<br>Call this method for every type of event you want to listen for.
	/*!
		\param msg [in]: The type of mouse message (i.e. event type) to listen for
		\param bEnable [in]: True to listen, False to ignore
		\param bRemove [in]: True to remove the mouse event from further processing, False to pass it on to other listeners.
	*/
	virtual void EnableOneTrap(const ge::EMouseMsg msg, tbool bEnable = true, tbool bRemove = true) = 0;


	//! Makes MouseTrap listen for (and relay) every type of mouse events. (Use EnableOne(..) to enable individual types).
	/*!
		\param bEnable [in]: True to listen, False to ignore
		\param bRemove [in]: True to remove the mouse event from further processing, False to pass it on to other listeners.
	*/
	virtual void EnableAllTraps(tbool bEnable = true, tbool bRemove = true) = 0;

	//! Makes MouseTrap listen for a specific type of mouse event *outside* of its SRect - the event message will then be relayed back through NotifyGeneric(...) to listeners.<br>Call this method for every type of event you want to listen for.
	/*!
		\param msg [in]: The type of mouse message (i.e. event type) to listen for
		\param bEnable [in]: True to listen, False to ignore
		\param bRemove [in]: True to remove the mouse event from further processing, False to pass it on to other listeners.
	*/
	virtual void EnableOneTrapOutside(const ge::EMouseMsg msg, tbool bEnable = true, tbool bRemove = false) = 0;

	//! Makes MouseTrap listen for (and relay) every type of mouse events *outside* of its SRect. (Use EnableOneOutside(..) to enable individual types).
	/*!
		\param bEnable [in]: True to listen, False to ignore
		\param bRemove [in]: True to remove the mouse event from further processing, False to pass it on to other listeners.
	*/
	virtual void EnableAllTrapsOutside(tbool bEnable = true, tbool bRemove = false) = 0;

	//! Makes MouseTrap ignore any mouse events that occur within the SRect. Any number of blind spots may be added
	/*!
		\param rRect [in]: The SRect (position and size) of the area that should be ignored
	*/
	virtual void AddBlindSpot(const SRect& rRect) = 0;

	//! Makes MouseTrap stop to ignore any mouse events that occur within the SRect.
	/*!
		\param rRect [in]: The SRect (position and size) of the area that should no longer be ignored. It must fit exactly with a previously added blind spot.
		\return tbool: True if the blind spot was removed, false if it wasn't there.
	*/
	virtual tbool RemoveBlindSpot(const SRect& rRect) = 0;

	//! Remove all blind spots (see AddBlindSpot(...) for explanation)
	virtual void ClearBlindSpots() = 0;

	//! Checks if the position is within a blind spot (see AddBlindSpot(...) for explanation)
	/*!
		\param rPos [in]: The position to check
		\return tbool: True if position is in a blind spot, otherwise False.
	*/
	virtual tbool IsWithinBlindSpot(const SPos& rPos) = 0;
};
