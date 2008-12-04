/*!	\file IBitmapResourceText.h
	\author Michael Olsen
	\date 19/Jan/2005
	\date 19/Jan/2005
*/

// Forward declarations
class IFontResource;

/*! \class IBitmapResource
 * \brief Interface for bitmap resource.with text blended into graphics
*/
class IBitmapResourceText : public virtual IBitmapResource
{
public:
	//! Create bitmap resource.
	/*!
		\param iNrOfFrames [in]: Number of frames in bitmap
		\param iNrOfVariants [in]: Number of variants in bitmap
	*/
	static IBitmapResourceText* Create(tint iNrOfFrames, tint iNrOfVariants);

	//! Blends text onto bitmap
	/*!
		\param pWindow [in]: Window we're attached to
		\param pFont [in]: Font to draw text with. Note: Font must have been created with forground colour 255,255,255 and background colour 0,0,0
		\param pppszText [in]: Array of array of strings to blend onto bitmap. This is the format: [variant][frame][character], so that for instance the string for variant 1, frame 2, is pppszText[1][2]
		\param Colour [in]: Colour to draw text with
	*/
	virtual void BlendText(IWindow* pWindow, IFontResource* pFontResource, const tchar*** pppszText, const SRGB& Colour) = 0;

	//! Blends text onto bitmap
	/*!
		\param pWindow [in]: Window we're attached to
		\param pppFont [in]: Array of fonts to draw text with, one for each frame/variant (FontToDraw = ppFont[variant][frame]. Note: Font must have been created with forground colour 255,255,255 and background colour 0,0,0
		\param pppszText [in]: Array of array of strings to blend onto bitmap. This is the format: [variant][frame][character], so that for instance the string for variant 1, frame 2, is pppszText[1][2]
		\param ppColour [in]: Colour to draw text with (ColourToDraw = ppColour[iVariant][iFrame])
	*/
	virtual void BlendText(IWindow* pWindow, IFontResource*** pppFontResource, const tchar*** pppszText, const SRGB** ppColour) = 0;
};






