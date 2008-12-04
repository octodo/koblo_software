/*!	\file IKnob.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

/*! \class IKnob
 * \brief An interface for a knob, also known as a 'dial'
*/
class IKnob : public virtual IControl, public virtual IDestructable
{
public:
	//! Create IKnob
	static IKnob* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param pBitmap [in]: Bitmap resource to use. If NULL the control is invisible (but can still be used). It can then be placed on top of a text control to get a "knob-text".
	*/
	virtual void Init(tint32 iID, IBitmapResource* pBitmap) = 0;

	//! Limits the number of frames used by "cutting off" start and/or end of bitmap.<br>Note: Only one range of frames can be active at any time. See also IControl::SetGrid for another type of limiting (can be combined).<br>To normalize to full range (default) call SetIncludeFrames(0).
	/*!
		\param iFirstIncludeFrame [in]: Index of the first (lowest number) frame in bitmap that should be used. 0 = full range.
		\param iLastIncludeFrame [in]: Index of the last (highest number) frame in bitmap that should be used. -1 = full range.
	*/
	virtual void SetIncludeFrames(tint32 iFirstIncludeFrame, tint32 iLastIncludeFrame = -1) = 0;
};