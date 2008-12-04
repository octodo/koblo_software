/*!	\file CMutexMac.h
	\author Michael Olsen
	\date 09/Mar/2005
	\date 09/Mar/2005
*/

//#include <Multiprocessing.h>

class CMutex
{
public:
	//! Constructor
	CMutex() : miAccessCount(0)
	{
		mbMPPresent = MPLibraryIsLoaded();

		if (mbMPPresent) {
			OSStatus oserr = MPCreateCriticalRegion(&mMPCriticalRegion);
			if (oserr != noErr) {
				mbMPPresent = false;
			}
		}
	}

	//! Destructor
	~CMutex()
	{
		if (mbMPPresent) {
			MPDeleteCriticalRegion(mMPCriticalRegion);
		}
	}

	//! Lock the mutex
	virtual void Lock()
	{
		if (mbMPPresent) {
			MPEnterCriticalRegion(mMPCriticalRegion, kDurationForever);
		}

		miAccessCount++;
	}

	//! Release the mutex
	virtual void UnLock()
	{
		miAccessCount--;

		if (mbMPPresent) {
			MPExitCriticalRegion(mMPCriticalRegion);
		}
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

	//! If true, the MP library is loaded and available
	tbool mbMPPresent;

	//! MP library mutex object
    MPCriticalRegionID mMPCriticalRegion;
};

