/*!	\file IMultiStateButton.h
	\author Michael Olsen
	\date 19/Jan/2005
	\date 19/Jan/2005
*/

/*! \class IMultiStateButton
 * \brief A multistate button is a button which can have multiple states (or "positions"). Normally it would have 2 states, i.e. "on"/"off". The states are cycles when pressed.
 *
 * If you need radiobutton functionality, use IRadioButton instead
*/
class IMultiStateButton : public virtual IControl, public virtual IDestructable
{
public:
	//! Create IMultiStateButton
	static IMultiStateButton* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param pBitmap [in]: Bitmap resource to use
		\param iNrOfStates [in]: Number of states in button
	*/
	virtual void Init(tint32 iID, IBitmapResource* pBitmap, tint iNrOfStates = 2) = 0;

	//! Add a text to a specific button state
	/*!
		\param iStateValue [in]: Value of state to use text with
		\param pszText [in]: Text to write
		\param pFont [in]: Font to use. Will be destroyed with the control
	*/
	virtual void AddText(tint iStateValue, const tchar* pszText, IFontResource* pFont) = 0;

	//! Call to make events on mouse up (instead of mouse down)
	virtual void SetEventMouseUp() = 0;

	//! Invert picture drawing order
	virtual void Invert() = 0;

	//! When enabled, the number of frames in the bitmap is 2 * NrOfStates, with the odd-numbered frames (starting from 1) being the "regular" frames, and the even-numbered being "mouse-down" frames, shown while the mouse is down. For instance, if a button has 2 states it has 4 frames. When the current state is 0, frame 0 is shown. When the button is clicked frame 1 is shown. When the mouse is released frame 2 is shown. When the mouse is clicked again frame 3 is shown, and when it's released we're back to frame 0. Note that this does not work if the button has text (AddText). Also it does not work with "inverted" button (Invert)
	virtual void EnableMouseStateFrames() = 0;

	//! When enabled, the button face changes with the stated interval. Can be used in conjunction with EnableMouseStateFrames()
	/*!
		\param bCycle [in]: true = start cycling, false = stop cycling
		\param iCycleIntervalMs [in]: how long each value is held
	*/
	virtual void Cycle(tbool bCycle = true, tint32 iCycleIntervalMs = 250) = 0;

	//! Find out if button is cycling
	virtual tbool IsCycling() = 0;
};






