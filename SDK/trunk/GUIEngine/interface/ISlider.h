/*!	\file ISlider.h
	\author Max Grønlund
	\date 17/Jul/2005
	\date 13/Dec/2005
*/

/*! \class ISlider
 * \brief An interface for a Slider, also known as a 'fader'
*/
class ISlider : public virtual IControl, public virtual IDestructable
{
public:
	//! Create ISlider
	static ISlider* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param pBitmap [in]: Bitmap resource to use for cap
		\param pBitmap [in]: Bitmap resource to use for line, or NULL if none
		\Offset [in]: offset for line drawing. Ignored if no line is drawn
	*/
	virtual void Init(tint32 iID, IBitmapResource* pCapBitmap, IBitmapResource* pLineBitmap = NULL, SPos Offset = ge::SPos(0, 0)) = 0;

	//! Sets the responsiveness ("speed") of the slider
	/*!
		\param fResponse [in]: Responsiveness of the slider. Default is 0.5. The higher the value, the faster the slider moves.
	*/
	virtual void SetResponsiveness(tfloat fResponse) = 0;
};