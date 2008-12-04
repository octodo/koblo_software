/*!	\file ICustomControlCallback.h
	\author Michael Olsen
	\date 18/Mar/2005
	\date 18/Mar/2005
*/

/*! \class ICustomControlCallback
 * \brief Callback used by ICustomControl to defer actions (draw, mouse, etc.)
*/
class ICustomControlCallback
{
public:
	//! Called when control should draw itself
	/*!
		\param rUpdate [in]: Update rectangle. Only draw inside this rectangle (there's no automatic clipping)
	*/
	virtual void OnDraw(const SRect &rUpdate) = 0;

	//! Called when there's a mouse action
	/*!
		\param MouseMsg [in]: Mouse action message
		\param Pos [in]: Mouse position when action originated
		\return tbool: true if mouse message was handled, false otherwise
	*/
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos) = 0;

	//! Called when there's a keyboard action
	/*!
		\param iKey [in]: Key that was pressed
		\return tbool: true if keyboard message was handed, false otherwise
	*/
	virtual tbool OnKeyDown(EKey Key) = 0;

	//! Called when there's a timer event
	virtual void OnTimer() = 0;

	//! Called when the controls value has changed
	virtual void OnValueChange() = 0;

	//! Called when the size of the control is set
	/*!
		\param Size [in]: New size
	*/
	virtual void SetSize(const SSize& Size) = 0;

	//! Called when the pos of the control is set
	/*!
		\param Pos [in]: New pos
	*/
	virtual void SetPos(const SPos& Pos) = 0;
};
