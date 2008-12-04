/*!	\file IWaitCursor.h
	\author Michael Olsen
	\date 29/Jun/2005
	\date 29/Jun/2005
*/

/*! \class IWaitCursor.h
 * \brief Class which changes mouse cursor to a waiting cursor while it exists
*/
class IWaitCursor : public virtual IDestructable
{
public:
	//! Create IWaitCursor
	static IWaitCursor* Create();
};
