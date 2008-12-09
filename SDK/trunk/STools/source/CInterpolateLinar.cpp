

#include "SToolsInternalOS.h"


IInterpolate* IInterpolate::CreateLinar()
{
	return dynamic_cast<IInterpolate*>(new CInterpolateLinar());
}

void CInterpolateLinar::Destroy()
{
	delete dynamic_cast<CInterpolateLinar*>(this);
}

CInterpolateLinar::CInterpolateLinar()
{

}

CInterpolateLinar::~CInterpolateLinar()
{
//	delete[] mpdBuffer;
}

void CInterpolateLinar::SetLOT(const float* pfLOT, const int iSize)
{
	mpfLOT = (float*)pfLOT;
	miSize = iSize;

}

float CInterpolateLinar::Get(const float fIndex)
{

	float fTmp	= fIndex * miSize;

	int		uiPos;
	float	fDelta;

	INDEX_DELTA(fTmp, uiPos, fDelta);

	fTmp	=	mpfLOT[uiPos]	*	(1.0f-fDelta);
	fTmp	+=	mpfLOT[uiPos+1]*	fDelta;

	return fTmp;
}


