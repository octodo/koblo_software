/*!	\file IDrawPrimitives.h
	\author Michael Olsen
	\date 18/Mar/2005
	\date 18/Mar/2005
*/

/*! \class IDrawPrimitives
 * \brief Drawing primitives to draw lines, fill rects, etc.
*/
class IDrawPrimitives : public virtual IDestructable
{
public:
	//! Create IDrawPrimitives
	static IDrawPrimitives* Create();

	//! Initialize
	/*!
		\param pWnd [in]: Window we're to draw in
	*/
	virtual void Init(IWindow* pWnd) = 0;

	//! Draw a point
	/*!
		\param RectUpdate [in]: Update rectangle
		\param Pos [in]: Position where to draw
		\param Colour [in]: Colour to draw
	*/
	virtual void DrawPoint(const SRect& RectUpdate, const SPos& Pos, const SRGB& Colour) = 0;

	//! Draw line
	/*!
		\param RectUpdate [in]: Update rectangle
		\param PosFrom [in]: Position to draw from (included)
		\param PosTo [in]: Position to draw to (included);
		\param Colour [in]: Colour to draw
	*/
	virtual void DrawLine(const SRect& RectUpdate, const SPos& PosFrom, const SPos& PosTo, const SRGB& Colour) = 0;

	//! Fill rect
	/*!
		\param RectUpdate [in]: Update rectangle
		\param RectFill [in]: Rectangle to fill
		\param Colour [in]: Colour to fill with
	*/
	virtual void DrawRect(const SRect& RectUpdate, const SRect& RectFill, const SRGB& Colour) = 0;

	//! Inverts the RGB on a rect
	/*!
		\param RectUpdate [in]: Update rectangle
		\param RectInvert [in]: Rectangle to invert
	*/
	virtual void InvertRect(const SRect& RectUpdate, const SRect& RectInvert) = 0;
};