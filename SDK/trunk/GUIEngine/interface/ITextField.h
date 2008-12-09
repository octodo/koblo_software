/*!	\file ITextField.h
	\author Michael Olsen
	\date 01/Jan/2005
	\date 01/Jan/2005
*/

/*! \class ITextField
 * \brief Text control, to show font
*/
class ITextField : public virtual IControl, public virtual IDestructable
{
public:
	//! Create ITextField
	static ITextField* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param pFont [in]: Font resource to use
	*/
	virtual void Init(tint32 iID, IFontResource* pFont) = 0;

	//! Changes the current font (can be used to change text colour by setting a new font with a different colour). Must not be called before Init.
	/*!
		\param pFont [in]: New font. Both this and the old font will get its reference count decreased automatically.
	*/
	virtual void ChangeFont(IFontResource* pFont) = 0;

	//! Sets the current text
	/*!
		\param pszText [in]: Text to set
	*/
	virtual void SetText(const tchar* pszText) = 0;

	//! Retrieves the current text
	/*!
		\param pszText [out]: Is filled with zero-terminated text. Must be preallocated with min. 65 characters
	*/
	virtual void GetText(tchar* pszText) = 0;

	//! Enum to describe horizontal alignment
	enum EHorizontalAlignment {
		//! Left alignment (default)
		HorzAlignLeft = 0,
		//! Center alignment
		HorzAlignCenter,
		//! Right alignment
		HorzAlignRight
	};

	//! Enum to describe vertical alignment
	enum EVerticalAlignment {
		//! Top alignment (default)
		VertAlignTop = 0,
		//! Center alignment
		VertAlignCenter,
		//! Bottom alignment
		VertAlignBottom
	};

	//! Sets the horizontal alignment. If not set it will default to left
	/*!
		\param Alignment [in]: The new alignment to use
	*/
	virtual void SetHorzAlignment(EHorizontalAlignment Alignment) = 0;

	//! Sets the vertical alignment. If not set it will default to top
	/*!
		\param Alignment [in]: The new alignment to use
	*/
	virtual void SetVertAlignment(EVerticalAlignment Alignment) = 0;

	//! Enables text size limiting mode. If the text doesn't fit inside the space given it will shorten it, by first removing vocals, and then cutting out the center of the text
	virtual void EnableTextLimiting(tbool bEnable = true) = 0;

	//! Makes a text editable
	virtual void MakeEditable() = 0;

	//! Makes clicks outside control end text edit the same way as pressing return. Otherwise clicks outside control will end text edit the same way as pressing cancel.
	virtual void SetClicksOutsideAsSuccess() = 0;

	//! Set size of text field
	virtual void SetTextFieldSize(ge::SSize Size) = 0;

	//! Set the maximum number off lines
	virtual void SetMaxNumberLines(tint iMaxLines ) = 0;
};