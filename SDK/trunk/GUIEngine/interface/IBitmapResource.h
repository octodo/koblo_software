/*!	\file IBitmapResource.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
	\todo Move create to CBitmap (also for other resources)
*/

/*! \class IBitmapResource
 * \brief Interface for bitmap resource.
 *
 * This resource is used by most controls.
*/
class IBitmapResource : public virtual IResource
{
public:
	//! Create bitmap resource.
	static IBitmapResource* Create(tint iNrOfFrames, tint iNrOfVariants);

	//! Return bitmaps frame size. I.e. for a, say, 5 frame bitmap width returned is 5 times shown width.
	/*!
		\param rSize [out]: Size of bitmap
	*/
	virtual void GetSize(SSize& rSize) const = 0;

	//! Return bitmaps width. I.e. for a, say, 5 frame bitmap width returned is 5 times shown width.
	/*!
		\return tint: Width of bitmap
	*/
	virtual tint GetWidth() const = 0;

	//! Return bitmaps height. I.e. for a, say, bitmap with 5 frame sets, returned height is 5 times shown height
	/*!
		\return tint: Height of bitmap
	*/
	virtual tint GetHeight() const = 0;

	//! Returns the "real" size, i.e. the size as the user sees it
	/*!
		\param rSize [out]: Size of bitmap as the user sees it
	*/
	virtual void GetRealSize(SSize& rSize) const = 0;

	//! Return bytes per row
	/*!
		\return tint: Bytes per row
	*/
	virtual tint GetBytesPerRow() const = 0;

	//! Returns bit per pixel (24 for rgb, 32 for rgba). Do not use to calculate bytes per row, since the data might be padded, use GetBytesPerRow instead
	/*!
		\return tint: Bits per pixel
	*/
	virtual tint GetBitsPerPixel() const = 0;

	//! Draw bitmap
	/*!
		\param pWnd [in]: Window to draw in
		\param rctUpdate [in]: Update rectangle
		\param rctDraw [in]: Target rectangle
		\param iFrame [in]: Frame to draw (0-based)
		\param bGhost [in]: If true, the last variant is drawn
	*/
	virtual void Draw(IWindow* pWnd, const SRect& rctUpdate, const SRect& rctDraw, tint iFrame, tbool bGhost = false) = 0;
};






