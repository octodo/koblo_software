/*!	\file IRadioButtons.h
	\author Michael Olsen
	\date 21/Jan/2005
	\date 21/Jan/2005
*/

/*! \class IRadioButtons.h
 * \brief IRadioButtons is a collection of controls (multistate buttons) with 1 shared parameter. Only 1 of the controls is at any given time non-zero.
 *
 *	Controls should be added in the order of their value, i.e. the control with value 0 first, then value 1 etc.
*/
class IRadioButtons : public virtual IPane
{
public:
	//! Create IRadioButtons
	static IRadioButtons* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
	*/
	virtual void Init(tint32 iID) = 0;
};
