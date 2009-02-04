/*!	\file CInvalidater.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/


#include "geInternalOS.h"


CInvalidater::CInvalidater() : mbRectInvalidated(false)
{
	miListLen_AtLastGet = 0;
	mbIsAdding = false;
	mRectInvalidated.iX = 0;
	mRectInvalidated.iY	= 0;
} // constructor


CInvalidater::~CInvalidater()
{
	mRectList.clear();
} // destructor


IInvalidater* IInvalidater::Create()
{
	return dynamic_cast<IInvalidater*>(new CInvalidater());
} // Create


void CInvalidater::Destroy()
{
	delete dynamic_cast<CInvalidater*>(this);
} // Destroy


void CInvalidater::Reset()
{
	CAutoLock Lock(mMutex);

//	printf("Invalidater Reset\n");

	if (mRectList.size() == miListLen_AtLastGet) {
		// No extra invalidates have happened since last get
		// Old behavior

		mbRectInvalidated = false;

		mRectList.clear();
		miListLen_AtLastGet = 0;
	}
	else {
		// One or more rects have been invalidated since last get
		// New behavior

		std::list<SRect>::iterator it = mRectList.begin();
		for (tint32 iDelete = 0; iDelete < miListLen_AtLastGet; iDelete++) {
			it++;
		}
		// Delete used rects
		mRectList.erase(mRectList.begin(), it);
		miListLen_AtLastGet = 0;
		mbRectInvalidated = (mRectList.size() > 0);

		if (mbRectInvalidated) {
			// Set total rect to the remainder of the list
			// First rect ..
			it = mRectList.begin();
			mRectInvalidated = *it;
			it++;
			// .. and the rest
			for ( ; it != mRectList.end(); it++) {
				SRect& Rect = *it;
				_IncludeRect(Rect);
			}
		}
	}
} // Reset


tbool CInvalidater::IsRectInvalidated() const
{
	// We don't need to grab lock here - the bool is volatile
	//CAutoLock Lock(mMutex);

	return (mbRectInvalidated) && (!mbIsAdding);
} // IsRectInvalidated


void CInvalidater::GetInvalidatedRect(SRect& rRect) //const
{
	CAutoLock Lock(mMutex);

	miListLen_AtLastGet = mRectList.size();
	rRect = mRectInvalidated;

//	printf("Invalidater GetRect: %d, %d, %d, %d\n", rRect.iY, rRect.iY, rRect.iCX, rRect.iCY);
} // GetInvalidatedRect


void CInvalidater::InvalidateRect(const SRect &Rect)
{
//	printf("Invalidater InvalidateRect: %d, %d, %d, %d\n", Rect.iY, Rect.iY, Rect.iCX, Rect.iCY);

	static int a = 0;
	if (a++ == 50000) {
		__asm int 3;
	}

	mbIsAdding = true;

	CAutoLock Lock(mMutex);

	mRectList.push_back(Rect);

	if (mbRectInvalidated == false) {
		mbRectInvalidated = true;
		mRectInvalidated = Rect;
	}
	else {
		_IncludeRect(Rect);
	}

	mbIsAdding = false;
} // InvalidateRect


void CInvalidater::_IncludeRect(const SRect& Rect)
{
	SRect RectNew;
	if (Rect.iX >= mRectInvalidated.iX) {
		RectNew.iX = mRectInvalidated.iX;
		RectNew.iCX = max(mRectInvalidated.iCX, Rect.iX + Rect.iCX - mRectInvalidated.iX);
	}
	else {
		RectNew.iX = Rect.iX;
		RectNew.iCX = max(Rect.iCX, mRectInvalidated.iX + mRectInvalidated.iCX - Rect.iX);
	}
	if (Rect.iY >= mRectInvalidated.iY) {
		RectNew.iY = mRectInvalidated.iY;
		RectNew.iCY = max(mRectInvalidated.iCY, Rect.iY + Rect.iCY - mRectInvalidated.iY);
	}
	else {
		RectNew.iY = Rect.iY;
		RectNew.iCY = max(Rect.iCY, mRectInvalidated.iY + mRectInvalidated.iCY - Rect.iY);
	}
	mRectInvalidated = RectNew;

//	printf("Invalidater new rect: %d, %d, %d, %d\n", RectNew.iY, RectNew.iY, RectNew.iCX, RectNew.iCY);

	/*
	if(-147482919 > mRectInvalidated.iX){
		tuint bob = 5; //!!! TO DO
		printf( "pos.x: %d  pos.y: %d  siz.ex: %d  size.y: %d \n " , mRectInvalidated.iX, mRectInvalidated.iY, mRectInvalidated.iCX, mRectInvalidated.iCY);
		bob++;
	}
	 */
	
	
} // _IncludeRect

