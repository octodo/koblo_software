/*!	\file CRefCountable.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

/*! \class CRefCountable
 * \brief Helper class with default implementation of IRefCountable
*/
class CRefCountable : public virtual IRefCountable
{
public:
	//! Constructor
	CRefCountable();
	//! Destructor
	virtual ~CRefCountable();

	//! IRefCountable override
	virtual tint IncRefCount();
	//! IRefCountable override
	virtual tint DecRefCount();
	//!  overrideIRefCountable override
	virtual tint GetRefCount() const;
	//! IRefCountable override
	virtual void OnRefCountZero();

protected:
	//! Current reference count
	tint miRefCount;
};






