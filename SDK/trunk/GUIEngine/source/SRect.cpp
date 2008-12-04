/*!	\file SRect.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/



#include "geInternalOS.h"


#if WIN32

SRect::SRect(const RECT &rct)
	: SPos(rct.left, rct.top), SSize(rct.right - rct.left + 1, rct.bottom - rct.top + 1)
{
}

SRect::operator RECT() const
{
	RECT rct;
	rct.left = iX;
	rct.right = iX + iCX - 1;
	rct.top = iY;
	rct.bottom = iY + iCY - 1;

	return rct;
}

#endif	// WIN32

