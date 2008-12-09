/*!	\file IInContext.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

// Forward declarations
class IContext;

/*! \class IInContext
 * \brief Interface to describe a class which has information about its context that can be retrieved
*/
class IInContext : public virtual IInterface
{
public:
	//! Returns context
	/*!
		\return IContext: Pointer to context object belongs to
	*/
	virtual IContext* GetContext() = 0;

	//! \overload
	virtual const IContext* GetContext() const = 0;
};