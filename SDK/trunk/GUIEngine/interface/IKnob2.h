/*!	\file IKnob2.h
	\author Michael Olsen
	\date 12/Oct/2005
	\date 12/Oct/2005
*/

/*! \class IKnob2
 * \brief And alternative knob, which uses 1 bitmap frame only, and draws it in an arc
*/
class IKnob2 : public virtual IKnob
{
public:
	//! Create IKnob2
	static IKnob2* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param iAngleStart [in]: Start angle (angle at min. position)
		\param iAngleEnd [in]: End angle (angle at max. position)
		\param pBitmap [in]: Bitmap resource to use
	*/
	virtual void Init(tint32 iID, IBitmapResource* pBitmap, tint32 iAngleStart, tint32 iAngleEnd) = 0;
};