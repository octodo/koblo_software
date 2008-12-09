

#include "SToolsInternalOS.h"

// Note: Currently wrong phase, since phase is accumulated before first calc

//#include "BLITTable.h"

ILFOLOT* ILFOLOT::Create()
{
	return dynamic_cast<ILFOLOT*>(new CLFOLOT());
}

void CLFOLOT::Destroy()
{
	delete dynamic_cast<CLFOLOT*>(this);
}

CLFOLOT::CLFOLOT()
{
//	mlBufferSize = BLIT_TABLE_SIZE / BLIT_OVERSAMPLING;
//	mpdBuffer = new double[mlBufferSize];
}

CLFOLOT::~CLFOLOT()
{
//	delete[] mpdBuffer;
}

void CLFOLOT::SetSampleRate(float fSampleRate)
{
	mfSampleRate				= fSampleRate;
	mfOneDivSampleRate			= 32000 / mfSampleRate;
	mfThirtyTwoDivSampleRate	= 32 / mfSampleRate;
	mfItrFactor					= 0x40000000 * mfThirtyTwoDivSampleRate ;
}

void CLFOLOT::SetAttack(float fMs)
{

	mfAttack	=	mfOneDivSampleRate / fMs;
}

void CLFOLOT::SetLOT(float* pfLOT,int iLOTSize)
{
	mpfLOT		=	pfLOT;
	miLOTSize	=	iLOTSize;

	ASSERT(miLOTSize == 2048);
}

void CLFOLOT::Reset()
{
	miPhase			=	0;
	mfEnvAmp		=	0;	
}

tfloat CLFOLOT::Process(const tfloat32 pfRate, tint lC)
{
	float	 fOut, fFloor;
	// Move phase
	miPhase		+=	(tint)(pfRate * mfItrFactor);
	// Wrap phase
	miPhase		&= 0x3FFFFFFF;
	// Envelope Attack
	mfEnvAmp	+= mfAttack;
	if(mfEnvAmp > 1){
		mfEnvAmp  = 1;	
	}
	fFloor = 0.5f * (1.0f - mfEnvAmp);
	// LOT ! No interpolation
	fOut		=	mpfLOT[miPhase >> 19];
	// Make Unipolar
	fOut	+= 1.0f;
	// Apply Amp
	fOut	*= 0.5f * mfEnvAmp;
	fOut	+=	fFloor;
	// Output
	return fOut;
	
}
