/*!	\file IListBox.h
	\author Michael Olsen
	\date 24/Apr/2008
*/

/*! \class IListBox
 * \brief Kinda like a Windows listbox, except, well, there's no box... So, it's a (vertical) list of text, which can be selected (currently only single selection is supported).
*/
class IListBox : public virtual IControl, public virtual IDestructable
{
public:
	//! Create IListBox
	static IListBox* Create();

	//! Initializer with array represented as one enum string (like automation string for CBasePlugIn::InsertParameterEnum(...))
	/*!
		\param iID [in]: Control ID
		\param pFont [in]: Font resource to use
	*/
	virtual void Init(tint32 iID, IFontResource* pFont) = 0;

	/*!
		\param pszEnumList [in]: Zero-terminated string containing valid texts to show. The individual values must be delimited by a character.
		\param cEnumDelimiter [in]: The delimiter character used to split the string pszEnumList into values (default '@' like for CBasePlugIn::InsertParameterEnum(...)).<br>If you use '\0' chars for delimiter the value "" will be illegal, and you must mark the end of string with two '\0' chars.
	*/
	virtual void SetText(const tchar* pszEnumList, tchar cEnumDelimiter = '@') = 0;

	//! Make the list-box control listen for more than the usual mouse events.<br/><b>Note!</b> If you use this functionality to send non-standard values (like -2 and below) you've got make sure that the listener keeps track of the *real* value itself! Real values/indexes are -1 (for no selection) or above (for a valid selection)
	/*!
		\param eMsg [in]: An event to listen for besides LeftButtonClick.<br/><b>Note!</b> If you try to catch e.g. LeftButtonUp messages you'll probably not get any, since the list-box uses them already and "eats" them.
		\param iValueToSet [in]: The list-box control will get set to this value when the event occurs. If -1 is stated then the same value is used as for ordinary operation. To not collide with ordinary return values one could use values like -2 and below (just a suggestion...)
	*/
	virtual void EnableExtraMouseEvent(EMouseMsg eMsg = LeftButtonDblClk, tint32 iValueToSet = -1) = 0;

protected:
};

