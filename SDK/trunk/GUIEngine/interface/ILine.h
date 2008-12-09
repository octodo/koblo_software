/*!	\file ILine.h
	\author Michael Olsen
	\date 21/Mar/2005
	\date 21/Mar/2005
*/

/*! \class ILine
 * \brief Control which draws a line between 2 points (relative to it's position)
*/
class ILine : public virtual IControl, public virtual IDestructable
{
public:
	//! Create ILine
	static ILine* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param PosStart [in]: Start position of line
		\param PosEnd [in]: End position of line
		\param Colour [in]: Colour to draw line with
		\param pParentWindow [in]: Window to draw in
	*/
	virtual void Init(tint32 iID, const SPos& PosStart, const SPos& PosEnd, const SRGB& Colour, IWindow* pParentWindow) = 0;

	//! Changes the line positions
	/*!
		\param PosStart [in]: Start position of line
		\param PosEnd [in]: End position of line
	*/
	virtual void SetLinePos(const SPos& PosStart, const SPos& PosEnd) = 0;
	
	virtual void SetColour(const SRGB& Colour) = 0;
};