/*!	\file IFontResource.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

const tint giMaxNrOfCharsInFont = 256;

/*! \class IFontResource
 * \brief Interface for font resources
*/
class IFontResource : public virtual IBitmapResource
{
public:
	//! Create IFontResource
	static IFontResource* Create();

	//! Returns the length of a given character
	/*!
		\param iIndex [in]: Index (ascii) of character to get length for
		\return tint: Length of character
	*/
	virtual tint GetLength(tint iIndex) const = 0;

	//! Returns offset into string for given character
	/*!
		\param iIndex [in]: Index (ascii) of character to get offset for
		\return tint: Offset of character
	*/
	virtual tint GetOffset(tint iIndex) const = 0;

	//! Returns text width for given string
	/*!
		\param psz [in]: String to get length for
		\return tint: Length, in pixels, for given string
	*/
	virtual tint GetTextWidth(const tchar* psz) const = 0;

	//! Returns text height
	/*!
		\return tint: Max. height of text (i.e. bitmap height)
	*/
	virtual tint GetTextHeight() const = 0;

	//! Return font top line. For old style fonts (Arial10..Arial18Bold) always returns 0
	/*!
		\return tint: Offset of top line of font (used for centering text vertically).
	*/
	virtual tint GetFontTopLine() const = 0;

	//! Return font base line. For old style fonts (Arial10..Arial18Bold) always returns height - 1
	/*!
		\return tint: Offset of base line of font (used for centering text vertically).
	*/
	virtual tint GetFontBaseLine() const = 0;

	//! Load resource from resource file. Note that the files current position must be the beginning of the resource data
	/*!
		\param pWnd [in]: Window resource will be used in (This is neccasary because of DirectX)
		\param pFile [in]: File to read resource from
		\param iSize [in]: Size of resource (in bytes).
		\param iResID [in]: ID of resource
		\param iNrOfChars [in]: Number of charactes in font. Must be equal to or lower than giMaxNrOfCharsInFont
		\param piCharSizes [in]: Pointer to array of iNrOfChars size, with size for each character. Characters not present should be set to 0.
		\param TextColour [in]: Text colour for font
	*/
	virtual void Load(IWindow* pWnd, IFile* pFile, tint iSize, tint32 iResID, tint iNrOfChars, const tint* piCharSizes, const SRGB& TextColour) = 0;

	//! Draw font
	/*!
		\param pWnd [in]: Window to draw in
		\param rctUpdate [in]: Update rectangle
		\param rctDraw [in]: Target rectangle
		\param sText [in]: Text to draw
	*/
	virtual void Draw(IWindow* pWnd, const SRect& rctUpdate, const SRect& rctDraw, const std::string& sText) = 0;
};






