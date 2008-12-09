/*!	\file IRect.h
	\author Michael Olsen
	\date 14/Jun/2005
	\date 14/Jun/2005
*/

/*! \class IRect
 * \brief Control which draws a rect (relative to it's position)
*/
class IRect : public virtual IControl, public virtual IDestructable
{
public:
	//! Create IRect
	static IRect* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param Rect [in]: Rectangle to draw
		\param Colour [in]: Colour to draw line with
		\param pParentWindow [in]: Window to draw in
	*/
	virtual void Init(tint32 iID, const SRect& Rect, const SRGB& Colour, IWindow* pParentWindow) = 0;

	//! Changes the rect
	/*!
		\param Rect [in]: Rectangle to draw
	*/
	virtual void SetRect(const SRect& Rect) = 0;
};