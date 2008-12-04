/*!	\file CFFTWin.cpp
	\author Michael Olsen
	\date 13/Dec/2005
	\date 13/Dec/2005
*/


#include "dspeInternalOS.h"


CFFTWin::CFFTWin()
	: mpFreqDomain(NULL)
{
}

CFFTWin::~CFFTWin()
{
	if (mpFreqDomain) {
		delete[] mpFreqDomain;
	}
}

IFFT* IFFT::Create()
{
	return dynamic_cast<IFFT*>(new CFFTWin());
}

void CFFTWin::Destroy()
{
	delete dynamic_cast<CFFTWin*>(this);
}

void CFFTWin::Init(tuint uiOrder)
{
	if (mpFreqDomain) {
		delete[] mpFreqDomain;
	}

	muiOrder = uiOrder;
	muiFFTSize = Float2Int(pow((tfloat64)2, (tint)muiOrder));

	mpFreqDomain = (void*)new sample[muiFFTSize + 2];
}

void CFFTWin::GetMagnitude(sample* pfSamples)
{
	if (sizeof(sample) == 4) {
		// 32-bit
		nspsRealFftNip((const float*)pfSamples, (SCplx*)mpFreqDomain, muiOrder, NSP_Forw);
		float fFFTMakeUp = 1.0f / muiFFTSize;
		for (tuint32 iIndex = 0; iIndex < muiFFTSize + 2; iIndex++) {
			((tfloat32*)mpFreqDomain)[iIndex] *= fFFTMakeUp;
		}
		nspcbMag((SCplx*)mpFreqDomain, (float*)pfSamples, muiFFTSize / 2 + 1);
	}
	else if (sizeof(sample) == 8) {
		// 64-bit
		nspdRealFftNip((const double*)pfSamples, (DCplx*)mpFreqDomain, muiOrder, NSP_Forw);
		float fFFTMakeUp = 1.0f / muiFFTSize;
		for (tuint32 iIndex = 0; iIndex < muiFFTSize + 2; iIndex++) {
			((tfloat64*)mpFreqDomain)[iIndex] *= fFFTMakeUp;
		}
		nspzbMag((DCplx*)mpFreqDomain, (double*)pfSamples, muiFFTSize / 2 + 1);
	}
	else {
		// Unknown sample size
		debug_break;
		throw IException::Create(IException::TypeCode,
			IException::ReasonGeneric,
			EXCEPTION_INFO,
			(tchar*)"Unknown sample size");
	}
}





