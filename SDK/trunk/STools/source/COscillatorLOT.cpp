

#include "SToolsInternalOS.h"

// Note: Currently wrong phase, since phase is accumulated before first calc

//#include "BLITTable.h"

IOscillatorLOT* IOscillatorLOT::Create()
{
	return dynamic_cast<IOscillatorLOT*>(new COscillatorLOT());
}

void COscillatorLOT::Destroy()
{
	delete dynamic_cast<COscillatorLOT*>(this);
}

COscillatorLOT::COscillatorLOT()
{
//	mlBufferSize = BLIT_TABLE_SIZE / BLIT_OVERSAMPLING;
//	mpdBuffer = new double[mlBufferSize];
}

COscillatorLOT::~COscillatorLOT()
{
//	delete[] mpdBuffer;
}

void COscillatorLOT::SetSampleRate(float fSampleRate)
{
	mfSampleRate		= fSampleRate;
	mfOneDivSampleRate	= 1 / mfSampleRate;
}

void COscillatorLOT::SetLOT(float* pfLOT,int iLOTSize)
{
	mpfLOT		=	pfLOT;
	miLOTSize	=	iLOTSize;
}

void COscillatorLOT::Reset()
{

	mfPhase			= 0;

}

void COscillatorLOT::ProcessLOT(tfloat32* pfOut, const tfloat32* pfPitch, tint lC)
{
	float			fDelta, fOut;
	unsigned int	uiIndex;

	for (long l = 0; l < lC; l++) {
		double dFreq = pfPitch[l] * mfOneDivSampleRate;

		mfPhase += (float)dFreq;

		if (mfPhase >= 1) {
			// Wrap phase
			mfPhase -= 1.0f;

		}
		// Index
		INDEX_DELTA(mfPhase*miLOTSize, uiIndex, fDelta);

		// Look up: Second order interpolation
		fOut		=	mpfLOT[uiIndex]	*	(1.0f-fDelta);
		fOut		+=	mpfLOT[uiIndex+1]	*	fDelta;	
	
		pfOut[l] = fOut;
	}
	
}

void COscillatorLOT::ProcessLOT(tfloat32* pfOut, tfloat fPitch, tint lC)
{
	float			fDelta, fOut;
	unsigned int	uiIndex;
	float fFreq = fPitch * mfOneDivSampleRate;

	for (long l = 0; l < lC; l++) {
		

		mfPhase += fFreq;

		if (mfPhase >= 1) {
			// Wrap phase
			mfPhase -= 1.0f;

		}
		// Index
		INDEX_DELTA(mfPhase*miLOTSize, uiIndex, fDelta);

		// Look up: Second order interpolation
		fOut		=	mpfLOT[uiIndex]	*	(1.0f-fDelta);
		fOut		+=	mpfLOT[uiIndex+1]	*	fDelta;	
	
		pfOut[l] = fOut;
	}
	
}

void COscillatorLOT::ProcessLOT32(tfloat32* pfOut, float fPitch)
{
	float			fDelta;
	unsigned int	uiIndex;

	float fFreq = fPitch * mfOneDivSampleRate;

	for (long l = 0; l < 32; l++) {

		mfPhase += fFreq;
		if (mfPhase >= 1) mfPhase -= 1.0f;
		INDEX_DELTA(mfPhase*miLOTSize, uiIndex, fDelta);
		pfOut[l]		=	mpfLOT[uiIndex]*(1.0f-fDelta) + mpfLOT[uiIndex+1]*fDelta;

	}
	
}

void COscillatorLOT::ProcessLOTFM(tfloat32* pfOut, const tfloat32* pfPitch, const tfloat32* pfFM, const tfloat32* pfFMAmt, tint lC)
{
	float			fDelta, fOut;
	unsigned int	uiIndex;

	float fFMVol	=	(172.0f - pfFMAmt[0]) * (1.0f/172.0f);
	
	for (long l = 0; l < lC; l++) {
		double dFreq = pfPitch[l] * mfOneDivSampleRate;

		//  FM Amount
		float fFM		=	1.0f + (pfFMAmt[l] * pfFM[l]);
		

		mfPhase += (float)(dFreq * fFM);

		if (mfPhase >= 1) {
			// Wrap phase

			while(mfPhase >= 1)
				mfPhase -= 1.0f;

		}
		else if (mfPhase < 0) {
			// Wrap phase
			while(mfPhase < 0)
				mfPhase += 1.0f;
		}
		// Index
		INDEX_DELTA(mfPhase*miLOTSize, uiIndex, fDelta);

		// Look up: Second order interpolation
		fOut		=	mpfLOT[uiIndex]	*	(1.0f-fDelta);
		fOut		+=	mpfLOT[uiIndex+1]	*	fDelta;	
	
		pfOut[l] = fOut*fFMVol;
	}
	
}

void COscillatorLOT::ProcessLOTFM(tfloat32* pfOut, const tfloat fPitch, const tfloat32* pfFM, const tfloat fFMAmt, tint lC)
{
	float			fDelta, fOut;
	unsigned int	uiIndex;
	double fFreq = fPitch * mfOneDivSampleRate;

	float fFMVol	=	(172.0f - fFMAmt) * (1.0f/172.0f);
	
	for (long l = 0; l < lC; l++) {
		

		//  FM Amount
		float fFM		=	1.0f + (fFMAmt * pfFM[l]);
		// Move phase
		mfPhase += (float)(fFreq * fFM);
		if (mfPhase >= 1) {
			// Wrap phase
			while(mfPhase >= 1)
				mfPhase -= 1.0f;
		}
		else if (mfPhase < 0) {
			// Wrap phase
			while(mfPhase < 0)
				mfPhase += 1.0f;
		}
		// Index
		INDEX_DELTA(mfPhase*miLOTSize, uiIndex, fDelta);

		// Look up: Second order interpolation
		fOut		=	mpfLOT[uiIndex]	*	(1.0f-fDelta);
		fOut		+=	mpfLOT[uiIndex+1]	*	fDelta;	
	
		pfOut[l] = fOut*fFMVol;
	}
	
}
