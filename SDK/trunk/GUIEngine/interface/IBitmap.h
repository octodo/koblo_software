/*!	\file IBitmap.h
	\author Michael Olsen
	\date 30/Dec/2004
	\date 30/Dec/2004
*/

/*! \class IBitmap
 * \brief Static bitmap
*/
class IBitmap : public virtual IControl, public virtual IDestructable
{
public:
	//! Create IBitmap
	static IBitmap* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param pBitmap [in]: Bitmap resource to use
		\param iFrames [in]: Frames in bitmap. Default is 1.
	*/
	virtual void Init(tint32 iID, IBitmapResource* pBitmap, tint iFrames = 1) = 0;

	//! Sets the current frame to be shown
	/*!
		\param iFrame [in]: 0-based frame to be shown. Default is frame 0.
	*/
	virtual void SetFrame(tint iFrame) = 0;
};