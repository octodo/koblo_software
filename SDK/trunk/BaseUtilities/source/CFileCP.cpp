/*!	\file CFileCP.cpp
	\author Michael Olsen
	\date 22/Apr/2005
	\date 22/Apr/2005
*/


#include "buInternalOS.h"


CFileCP::CFileCP()
{
}

CFileCP::~CFileCP()
{
}

tuint64 CFileCP::ReadCP(tint32* p, tuint64 iSize)
{
	// Read
	tuint64 iRet = Read((tchar*)p, iSize * sizeof(tint32));
	iRet /= sizeof(tint32);

	// Crossplatform swap
	for (tuint64 iIndex = 0; iIndex < iRet; iIndex++) {
		p[iIndex] = CPSwap(p[iIndex]);
	}

	return iRet;
}

tuint64 CFileCP::WriteCP(tint32* p, tuint64 iSize)
{
	// Crossplatform swap
	tuint64 iIndex;
	for (iIndex = 0; iIndex < iSize; iIndex++) {
		p[iIndex] = CPSwap(p[iIndex]);
	}

	// Write
	tuint64 iRet = Write((tchar*)p, iSize * sizeof(tint32));
	iRet /= sizeof(tint32);

	// Crossplatform swap back
	for (iIndex = 0; iIndex < iSize; iIndex++) {
		p[iIndex] = CPSwap(p[iIndex]);
	}

	return iRet;
}

tuint64 CFileCP::ReadCP(tfloat32* p, tuint64 iSize)
{
	// Read
	tuint64 iRet = Read((tchar*)p, iSize * sizeof(tfloat32));
	iRet /= sizeof(tfloat32);

	// Crossplatform swap
	for (tuint64 iIndex = 0; iIndex < iRet; iIndex++) {
		p[iIndex] = CPSwap(p[iIndex]);
	}

	return iRet;
}

tuint64 CFileCP::WriteCP(tfloat32* p, tuint64 iSize)
{
	// Crossplatform swap
	tuint64 iIndex;
	for (iIndex = 0; iIndex < iSize; iIndex++) {
		p[iIndex] = CPSwap(p[iIndex]);
	}

	// Write
	tuint64 iRet = Write((tchar*)p, iSize * sizeof(tfloat32));
	iRet /= sizeof(tfloat32);

	// Crossplatform swap back
	for (iIndex = 0; iIndex < iSize; iIndex++) {
		p[iIndex] = CPSwap(p[iIndex]);
	}

	return iRet;
}






