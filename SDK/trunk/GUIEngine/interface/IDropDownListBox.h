/*!	\file IDropDownListBox.h
	\author Michael Olsen
	\date 14/Mar/2005
	\date 14/Mar/2005
*/

/*! \class IDropDownListBox.h
 * \brief A standard drop down list box with non-editable text
 *
 *	2 controls should be added, first a button (IButton) for the top part, then a popup menu (IPopupMenu). When adding controls the position is ignored (set automatically by dropdown list box)
*/
class IDropDownListBox : public virtual IPane
{
public:
	//! Create IDropDownListBox
	static IDropDownListBox* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
	*/
	virtual void Init(tint32 iID) = 0;

	//! Tells the control to not update text on button
	virtual void DontSetText() = 0;
	
	
	// Lasse, added 2007-12-04 - flexible menus
	
	//! Get pointer to the pop-up menu part of the drop-down (needed for flexible addition of items)
	/*!
	 \return IPopupMenu*: Pointer to the pop-up, or NULL on error
	 */
	virtual IPopupMenu* GetPopup() = 0;
	 
	// .. Lasse
};
