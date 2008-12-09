/*!	\file ITextList.h
	\author Lasse Steen Bohnstedt
	\date 14/Sep/2006
*/

/*! \class ITextList
 * \brief TextList control, will only accept some input according to list
*/
class ITextList : public virtual IText, public virtual IDestructable
{
public:
	//! Create ITextList
	static ITextList* Create();

	//! Initializer with array represented as one enum string (like automation string for CBasePlugIn::InsertParameterEnum(...))
	/*!
		\param iID [in]: Control ID
		\param pFont [in]: Font resource to use
		\param pszEnumList [in]: Zero-terminated string containing valid texts to show. The individual values must be delimited by a character.
		\param cEnumDelimiter [in]: The delimiter character used to split the string pszEnumList into values (default '@' like for CBasePlugIn::InsertParameterEnum(...).)
	*/
	virtual void Init(tint32 iID, IFontResource* pFont, const tchar* pszEnumList, tchar cEnumDelimiter = '@') = 0;

	//! Initializer with array of tchar* as input.<br><b>Please use one of the other Init(...) calls instead!</b>
	/*!
		\param iID [in]: Control ID
		\param pFont [in]: Font resource to use
		\param ppszTextList [in]: Array of zero-terminated string containing valid texts to show. Last item (not shown) must be NULL or "".<br><b>Note! The calling point in application must delete the array afterwards.</b>
	*/
	virtual void Init(tint32 iID, IFontResource* pFont, const tchar** ppszTextList) = 0;


	//! Sets the current text
	/*!
		\param pszText [in]: Text to set
	*/
	virtual void SetText(const tchar* pszText) = 0;

	
	/*!
		\param pszEnumList [in]: Zero-terminated string containing valid texts to show. The individual values must be delimited by a character.
		\param cEnumDelimiter [in]: The delimiter character used to split the string pszEnumList into values (default '@' like for CBasePlugIn::InsertParameterEnum(...)).<br>If you use '\0' chars for delimiter the value "" will be illegal, and you must mark the end of string with two '\0' chars.
		\return tint32: The number of values created from string.

		No checking of list sanity is performed (yet)
	*/
	virtual tint32 SetTextList(const tchar* pszEnumList, tchar cEnumDelimiter = '@') = 0;

	//! Changes list of valid texts to show. Uses an array of tchar* as input.<br><b>Please use one of the other SetTextList(...) calls instead!</b>
	/*!
		\param ppszTextList [in]: Array of zero-terminated string containing valid texts to show. Last item (not shown) must be NULL or "".<br><b>Note! The calling point in application must delete the array afterwards.</b>
		\return tint32: The number of values created from string.

		No checking of list sanity is performed (yet)
	*/
	virtual tint32 SetTextList(const tchar** ppszTextList) = 0;


	//! Get the text for a given value / index
	/*!
		\param iValue [in]: The value to look up text for
		\param pszTextOut [out]: The string that matches the index (pointer must be allocated!)
		\param iSizeOfTextOut [in]: The size in chars of pszTextOut (max length of string + 1 for terminating zero).
		\return tbool: True = Success
	*/
	virtual tbool GetText(tint32 iValue, tchar* pszTextOut, tint32 iSizeOfTextOut) = 0;

protected:

	//! Get the text for a given value / index
	/*!
		\param iValue [in]: The value to look up text for. No validation is done.
		\return const tchar*: Pointer to the string that matches the index (don't modify!).
	*/
	virtual const tchar* _GetText(tint32 iValue) = 0;

};

