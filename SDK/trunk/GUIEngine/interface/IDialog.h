/*!	\file IDialog.h
	\author Michael Olsen
	\date 05/Apr/2005
	\date 05/Apr/2005
*/

/*! \class IDialog.h
 * \brief IDialog is a modeless dialogue which is shown inside the window.
 *
 *	Since dialogue are often used to get an answer to some question, as to decide on some action, a modeless dialogue needs some way to "continue" the work after it's closed. To accomblish this add a callback, which will be called after the dialogue has been hidden
*/
class IDialog : public virtual IPane
{
public:
	//! Create IDialog
	static IDialog* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
	*/
	virtual void Init(tint32 iID) = 0;

	//! Sets the callback
	/*!
		\param pCallback [in]: Callback to set. Will be destroyed when the dialog is destroyed.
	*/
	virtual void SetCallback(IDialogCallback* pCallback) = 0;

	//! Gets the callback (or NULL if none)
	/*!
		\return IDialogCallback*: Current callback (or NULL if none)
	*/
	virtual IDialogCallback* GetCallback() = 0;

	//! Overload
	virtual const IDialogCallback* GetCallback() const = 0;

	//! Used to determine what kind of button is added to a dialog
	enum EButtonType {
		//! Normal button type
		ButtonTypeNormal = 0,
		//! Button which can close dialog
		ButtonTypeClose
	};

	//! IPane override
	/*!
		\param ButtonType [in]: Type of button (if control is button) that is added. Should be ButtonTypeClose if it's a button that can close the dialog.
	*/
	virtual void AddControl(IControl* pToAdd, const SPos& Pos, EButtonType ButtonType = ButtonTypeNormal) = 0;

	//! Shows the dialog. Do not call SetVisible(true) instead.
	virtual void Show() = 0;

	//! Hides ("closes") the dialog. Do not call SetVisible(false) instead.
	/*!
		\param iID [in]: ID passed on to callback
	*/
	virtual void Hide(tint32 iID) = 0;
};
