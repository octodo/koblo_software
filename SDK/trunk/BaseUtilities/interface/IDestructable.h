/*!	\file IDestructable.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

// Class that can be destroy'ed should derive from this
/*! \class IDestructable
 * \brief Interface to support destruction through Destroy
 *
 * Inherit your interface from this class to support destruction of objects
*/
class IDestructable : public virtual IInterface
{
public:

	//! Destroys object. It is imperative to use this instead of delete
	virtual void Destroy() = 0;
};