/*!	\file DialogCallback.h
	\author Michael Olsen
	\date 05/Apr/2005
	\date 05/Apr/2005
*/

// Forwards
class IDialog;

/*! \class IDialogCallback.h
 * \brief Callback for IDialog.
 *
 *	When dialog closes it calls callback, and destroys it
*/
class IDialogCallback : public virtual IDestructable
{
public:
	//! Called when dialog has just closed
	/*!
		\param iDialog [in]: Dialog which callback originates from
		\param iControlID [in]: ID of control that caused dialog to close, or, if Hide was used to close, iID parameter of Hide.
	*/
	virtual void OnClose(IDialog* pDialog, tint32 iControlID) = 0;
};
