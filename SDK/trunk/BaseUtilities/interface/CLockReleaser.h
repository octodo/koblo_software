/*!	\file CLockReleaser.h
	\author Michael Olsen
	\date 27/Jun/2005
	\date 27/Jun/2005
*/

/*! \class CLockReleaser
 * \brief A lock releaser is used to temporarily release a mutex to in order to give other threads an chance to run. It is imperative that the mutex is first acquired, which is why Lock is called in the constructor.
 *
 *	The use of this class is different in that it doesn't have an interface. This means it is created and destroyed "directly", i.e. through constructor and destructor.
*/
class CLockReleaser
{
public:
	//! Constructor
	/*!
		\parm rMutex: Mutex to access
	*/
	CLockReleaser(CMutex& rMutex)
	{
		rMutex.Lock();

		mpMutex = &rMutex;

		miAccessCount = mpMutex->FullRelease();
	}

	//! \overload
	CLockReleaser(CMutex* pMutex)
	{
		pMutex->Lock();

		mpMutex = pMutex;

		miAccessCount = mpMutex->FullRelease();
	}

	//! Destructor
	virtual ~CLockReleaser()
	{
		mpMutex->ReAquire(miAccessCount);

		mpMutex->UnLock();
	}

protected:
	//! Pointer to the mutex we're accessing
	CMutex* mpMutex;

	//! Access count of mutex we released
	tint miAccessCount;

private:
	//! Default constructor is inaccessible
	CLockReleaser();

	//! Copy constructor is inaccessible
	CLockReleaser(const CLockReleaser&);

	//! Assignment operator is inaccessible
	CLockReleaser operator=(const CLockReleaser);
};


