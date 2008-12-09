
/*!	\file CMultiStateDropDownListBox.h
	\author Lasse Steen Bohnstedt
	\date 28/Nov/2006
*/


//#ifndef IMultiStateDropDownListBox_H
//#define IMultiStateDropDownListBox_H


/*! \class CMultiStateDropDownListBox
 * \brief IMultiStateDropDownListBox consists of a multi-state button, a mouse-trap that is placed on top of it, and a popup-menu
 *
 *	After creating the IMultiStateDropDownListBox you must add the controls to the implicit IPane in the above-mentioned order.
*/
class IMultiStateDropDownListBox : public virtual IPane //, public virtual IEventReceiver
{
public:
	//! Create IMultiStateDropDownListBox
	static IMultiStateDropDownListBox* Create();
 
	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param msgTrigger [in]: The mouse message that should trigger a "show pop-up" operation (it must be an "inside SRect" message).<br><b>Note!</b> Must be the same as the one that is setup for the mouse-trap. If other triggers are enabled they will be relayed on to the IMultiStateDropDownListBox's event-receivers.
		\param bPopupAtClickPos [in]: True = the pop-up will show at the position of the click, False = it won't overlap the multi-state button.

	*/
	virtual void Init(tint32 iID, EMouseMsg msgTrigger = RightButtonUp, tbool bPopupAtClickPos = true) = 0;

	//! Hide the pop-up.<br>Call this when other pop-ups appear
	virtual void HidePopup() = 0;

protected:
	//! Show the pop-up
	virtual void ShowPopup(SPos& posAbsClicked) = 0;
};


//#endif // IMultiStateDropDownListBox_H