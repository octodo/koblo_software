/*!	\file CUniqueID.cpp
	\author Michael Olsen
	\date 17/Jan/2005
	\date 17/Jan/2005
*/


#include "buInternalOS.h"


CUniqueID::CUniqueID(tuint32 uiMinValue)
	: muiMinValue(uiMinValue)
{
}

CUniqueID::~CUniqueID()
{
}

IUniqueID* IUniqueID::Create(tuint32 uiMinValue)
{
	return dynamic_cast<IUniqueID*>(new CUniqueID(uiMinValue));
}

void CUniqueID::Destroy()
{
	delete dynamic_cast<CUniqueID*>(this);
}

tuint32 CUniqueID::GetID()
{
	// Find an ID to use
//	tuint32 uiID = muiMinValue;

	// The above code starts at the min value, but when we have a lot of IDs this is too slow. Instead we start at at random value, but makes sure it is at least the min value
	tuint32 uiID = ITime::GetTimeMS();
	while (uiID < muiMinValue) {
		uiID += 100000;
	}

	tbool bFoundOne = false;
	while (bFoundOne == false) {
		if (mIDMap.find(uiID) == mIDMap.end()) {
			// We didn't find an entry, i.e. ID is not in use
			bFoundOne = true;
		}
		else {
			// Entry found, i.e. ID is in use
			uiID++;

			// If ID == 0, that means we wrapped around, not finding any, i.e. we've exceeded our limitation
			if (uiID == 0) {
				throw IException::Create(IException::TypeStorageLimitationExceeded, IException::ReasonGeneric, EXCEPTION_INFO, (tchar*)"No more unique IDs");
			}
		}
	}

	// Insert entry in map
	mIDMap.insert(std::pair <int, int>(uiID, 0));

	return uiID;
}

void CUniqueID::ReleaseID(tuint32 iID)
{
	// Find entry
	std::map<tuint32, tuint32>::iterator it = mIDMap.find(iID);

	// If entry exist, delete from map
	if (it != mIDMap.end()) {
		mIDMap.erase(it);
	}
}



