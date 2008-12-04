/*!	\file IInvert.h
	\author Michael Olsen
	\date 05/May/2008
	\date 05/May/2008
*/

/*! \class IInvert
 * \brief Control which inverts a rect (relative to it's position)
*/
class IInvert : public virtual IControl, public virtual IDestructable
{
public:
	//! Create IInvert
	static IInvert* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param Rect [in]: Rectangle to invert
		\param pParentWindow [in]: Window to do the invertion in
	*/
	virtual void Init(tint32 iID, const SRect& Rect, IWindow* pParentWindow) = 0;

	//! Changes the rect to invert
	/*!
		\param Rect [in]: Rectangle to invert
	*/
	virtual void SetRect(const SRect& Rect) = 0;
};