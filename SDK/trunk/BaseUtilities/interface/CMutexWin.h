/*!	\file CMutex.h
	\author Michael Olsen
	\date 10/Jan/2005
	\date 10/Jan/2005
*/

/*! \class CMutex
 * \brief A standard mutex.
 *
 *	The use of this class is different in that it doesn't have an interface. This means it is created and destroyed "directly", i.e. through constructor and destructor. A mutex is normally locked and unlocked through an autolock
*/
class CMutex
{
public:
	//! Constructor
	CMutex() : miAccessCount(0)
	{
		::InitializeCriticalSection(&mCritSec);
	}

	//! Destructor
	~CMutex()
	{
		ASSERT(miAccessCount == 0);

		::DeleteCriticalSection(&mCritSec);
	}

	//! Lock the mutex
	virtual void Lock()
	{
		::EnterCriticalSection(&mCritSec);

		miAccessCount++;

		ASSERT(miAccessCount > 0);
	}

	//! Release the mutex
	virtual void UnLock()
	{
		ASSERT(miAccessCount > 0);

		miAccessCount--;

		::LeaveCriticalSection(&mCritSec);
	}

	//! Releases the mutex, no matter what count it has. This is used by the CLockReleaser class
	/*!
		\param tint: Accesscount before release
	*/
	virtual tint FullRelease()
	{
		// Access count must be larger than 0, because otherwise calling this was unsafe
		ASSERT(miAccessCount > 0);

		tint iRet = miAccessCount;

		tint iAccessCount = miAccessCount;
		while (iAccessCount) {
			UnLock();

			iAccessCount--;
		}

		return iRet;
	}

	//! Reaquires the mutex, after a FullRelease(). This is used by the CLockReleaser class
	/*!
		\param iCount [in]: Times to acquire the mutex
	*/
	virtual void ReAquire(tint iCount)
	{
		ASSERT(iCount > 0);

		while (iCount) {
			Lock();

			iCount--;
		}
	}

protected:
	//! Counts how many times the mutex is accesed currently.
	tint miAccessCount;
	//! Windows object to handle a mutex variant.
	CRITICAL_SECTION mCritSec;
};


