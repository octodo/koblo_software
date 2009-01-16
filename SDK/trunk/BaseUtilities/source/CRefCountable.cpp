/*!	\file CRefCountable.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

#include "buInternalOS.h"

CRefCountable::CRefCountable() : miRefCount(1)
{
}

CRefCountable::~CRefCountable()
{
	ASSERT(miRefCount == 0);
}

tint CRefCountable::IncRefCount()
{
	miRefCount++;
	return miRefCount;
}

tint CRefCountable::DecRefCount()
{
	ASSERT(miRefCount > 0);
	miRefCount--;
	tint iRet = miRefCount;
	if (miRefCount == 0) {
		OnRefCountZero();
	}
	return iRet;
}

tint CRefCountable::GetRefCount() const
{
	return miRefCount;
}

void CRefCountable::OnRefCountZero()
{
	(dynamic_cast<IDestructable*>(this))->Destroy();
}


