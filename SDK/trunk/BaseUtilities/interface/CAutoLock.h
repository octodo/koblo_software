/*!	\file CAutoLock.h
	\author Michael Olsen
	\date 10/Jan/2005
	\date 10/Jan/2005
*/

/*! \class CAutoLock
 * \brief An autolock is used to wrap in access to a mutex. Whenever you need to lock a mutex, use an autolock instead. Doing this you're ensured that the number of UnLocks matches the number of Locks. What an autolock does it to Lock the mutex in its constructor and UnLock it its destructor.
 *
 *	The use of this class is different in that it doesn't have an interface. This means it is created and destroyed "directly", i.e. through constructor and destructor.
*/
class CAutoLock
{
public:
	//! Constructor
	/*!
		\parm rMutex: Mutex to access
	*/
	CAutoLock(CMutex& rMutex)
	{
		mpMutex = &rMutex;
		mpMutex->Lock();
	}

	//! \overload
	CAutoLock(CMutex* pMutex)
	{
		mpMutex = pMutex;
		mpMutex->Lock();
	}

	//! Destructor
	virtual ~CAutoLock()
	{
		mpMutex->UnLock();
	}

protected:
	//! Pointer to the mutex we're accessing
	CMutex* mpMutex;

private:
	//! Default constructor is inaccessible
	CAutoLock();

	//! Copy constructor is inaccessible
	CAutoLock(const CAutoLock&);

	//! Assignment operator is inaccessible
	CAutoLock operator=(const CAutoLock);
};


