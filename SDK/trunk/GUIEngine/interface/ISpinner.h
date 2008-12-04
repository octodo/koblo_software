/*!	\file ISpinner.h
	\author Michael Olsen (+Lasse Steen Bohnstedt)
	\date 13/Sep/2006
*/

/*! \class ISpinner.h
 * \brief ISpinner consists of two buttons 1 up and 1 down
 *
 *	After creating the ISpinner you must first add an up button and then a down button to its implicit IPane.
 *  Note: Excactly two buttons should be added.
*/
class ISpinner : public virtual IPane
{
public:
	//! Create ISpinner
	static ISpinner* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
	*/
	virtual void Init(tint32 iID) = 0;
};
