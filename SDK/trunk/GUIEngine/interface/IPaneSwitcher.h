/*!	\file IPaneSwitcher.h
	\author Michael Olsen
	\date 06/Sep/2007
	\date 06/Sep/2007
*/

/*! \class IPaneSwitcher.h
 * \brief IPaneSwitcher is a collection of controls (typically panes). Only 1 control is visible, depending on the controls value. Control value must be ranging from 0 to nrofcontrols-1, with no gaps.
 *
 *	Controls should be added in the order of their value, i.e. the control with value 0 first, then value 1 etc.
*/
class IPaneSwitcher : public virtual IPane
{
public:
	//! Create IPaneSwitcher
	static IPaneSwitcher* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
	*/
	virtual void Init(tint32 iID) = 0;
};
