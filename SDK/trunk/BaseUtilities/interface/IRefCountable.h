/*!	\file IRefCountable.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

/*! \class IRefCountable
 * \brief Interface to support reference count
 *
 * Inherit your interface from this class to support reference countable objects
*/
class IRefCountable : public virtual IDestructable
{
public:

	//! Increase reference count
	/*!
		\return tint: New reference count
	*/
	virtual tint IncRefCount() = 0;

	//! Decrease reference count and if new count is zero calls OnRefCountZero
	/*!
		\return tint: New reference count
	*/
	virtual tint DecRefCount() = 0;

	//! Returns current reference count
	/*!
		\return tint: Current reference count
	*/
	virtual tint GetRefCount() const = 0;

	//! This is called when the reference count reaches 0, and object should be destroyed
	virtual void OnRefCountZero() = 0;
};