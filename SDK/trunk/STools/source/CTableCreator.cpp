

#include "SToolsInternalOS.h"

ITableCreator* ITableCreator::Create()
{
	return dynamic_cast<ITableCreator*>(new CTableCreator());
}

void CTableCreator::Destroy()
{
	delete dynamic_cast<CTableCreator*>(this);
}

CTableCreator::CTableCreator()
{

}

CTableCreator::~CTableCreator()
{
//	delete[] mpdBuffer;
}

void CTableCreator::InitPitchModTable(float* pfPitchModTable, const int iSize)
{

	int		iUp, iDown;
	int		iHalfTableSize			=	iSize/2;

	float	fMaxModRange			=	16.0f;
	float	fFac					=	pow(fMaxModRange,(1.0f/iSize));

	
	pfPitchModTable[iHalfTableSize]	=	1.0f;

	// Create table
	int uiIndex;
	for (uiIndex = 0; uiIndex < iHalfTableSize; uiIndex++) {
		
		iUp						=	(iHalfTableSize + 1) + uiIndex;
		iDown					=	(iHalfTableSize - 1) - uiIndex;

		pfPitchModTable[iUp]	=	pfPitchModTable[iUp - 1] * fFac;
		pfPitchModTable[iDown]	=	1.0f / pfPitchModTable[iUp];
	}
	pfPitchModTable[iSize]		=	pfPitchModTable[iSize - 1] * fFac;
	pfPitchModTable[iSize+1]	=	pfPitchModTable[iSize] * fFac;

}
