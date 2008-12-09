/*!	\file CFFTOSX.cpp
	\author Michael Olsen
	\date 20/Dec/2005
	\date 20/Dec/2005
*/


#include "dspeInternalOS.h"


CFFTOSX::CFFTOSX()
	: mFFT(NULL)
{
	mA.realp = mA.imagp = NULL;
}

CFFTOSX::~CFFTOSX()
{
	if (mA.realp) {
		delete[] mA.realp;
		delete[] mA.imagp;
	}

	if (mFFT) {
		destroy_fftsetup(mFFT);
	}
}

IFFT* IFFT::Create()
{
	return dynamic_cast<IFFT*>(new CFFTOSX());
}

void CFFTOSX::Destroy()
{
	delete dynamic_cast<CFFTOSX*>(this);
}

void CFFTOSX::Init(tuint uiOrder)
{
	muiOrder = uiOrder;
	muiFFTSize = Float2Int(pow(2, muiOrder));

	mA.realp = new float[muiFFTSize / 2];
	mA.imagp = new float[muiFFTSize / 2];

	mFFT = vDSP_create_fftsetup(muiOrder, FFT_RADIX2);
}

void CFFTOSX::GetMagnitude(sample* pfSamples) throw (IException*)
{
	// Only 32-bit sample is supported
	if (sizeof(sample) != 4) {
		throw IException::Create(IException::TypeCode,
			IException::ReasonGeneric,
			EXCEPTION_INFO,
			(tchar*)"Unknown or unsupported sample size");
	}

	// Convert to required format
	ctoz((COMPLEX*)pfSamples, 2, &mA, 1, muiFFTSize / 2);

	// FFT
	fft_zrip(mFFT, &mA, 1, muiOrder, FFT_FORWARD);

	// Scale it
	float fScale = 1.0f / (2 * muiFFTSize);
	vsmul(mA.realp, 1, &fScale, mA.realp, 1, muiFFTSize / 2);
	vsmul(mA.imagp, 1, &fScale, mA.imagp, 1, muiFFTSize / 2);
	
	// Convert to magnitude
	tint32 iIndex;
	for (iIndex = 1; iIndex < muiFFTSize / 2; iIndex++) {
		pfSamples[iIndex] = (float)sqrt((mA.realp[iIndex] * mA.realp[iIndex]) + (mA.imagp[iIndex] * mA.imagp[iIndex]));
	}
	// Special cases
	pfSamples[0] = (float)sqrt(mA.realp[0] * mA.realp[0]);
	pfSamples[muiFFTSize / 2] = (float)sqrt(mA.realp[1] * mA.realp[1]);
}





