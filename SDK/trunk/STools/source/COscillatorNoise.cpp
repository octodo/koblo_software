

#include "SToolsInternalOS.h"


IOscillatorNoise* IOscillatorNoise::Create(float* pfFreqToCutoff,int iSize)
{
	return dynamic_cast<IOscillatorNoise*>(new COscillatorNoise(pfFreqToCutoff,iSize));
}

void COscillatorNoise::Destroy()
{
	delete dynamic_cast<COscillatorNoise*>(this);
}

COscillatorNoise::COscillatorNoise(float* pfFreqToCutoff,int iSize)
{

	mpfFreqToCutoff		=	pfFreqToCutoff;
	iFreqToCutoffSize	=	iSize;

}

COscillatorNoise::~COscillatorNoise()
{
//	delete[] mpdBuffer;
}

void COscillatorNoise::SetSampleRate(float fSampleRate)
{
	mfSampleRate		= fSampleRate;
	mfOneDivSampleRate	= 1 / mfSampleRate;

	for( int i=1; i<iFreqToCutoffSize; i++) {

		float fFreq	=	(float)i;

		if(fFreq < 16274.0f) {

			float fDelta		=	gfPI *(fFreq / fSampleRate);
			mpfFreqToCutoff[i]	=	2.0f * sin(fDelta);
		}
		else {

			mpfFreqToCutoff[i]	=	1.0f;
		}
	}

}

void COscillatorNoise::SetNoise(float* pfNoise,int iNoiseSize)
{
	mpfNoise		=	pfNoise;
	miNoiseSize	=	iNoiseSize;
}

void COscillatorNoise::Reset()
{

	mfB			=	0;
	mfBp		=	0;
	mfL			=	0;
	mfH			=	0;
	miNoise		=	68176187;

}

void COscillatorNoise::ProcessNoise(tfloat32* pfOut, const tfloat32* pfPitch, tint lC)
{
	//-------------------------------------------
	// Set filter cutoff
	//-------------------------------------------
	float	fCutoff, fDelta, fFreq, fAmp;				
	int	iIndex;

	// Move frequency up one octaves
	fFreq	=	2.0f * pfPitch[0];

	fAmp	=	200.0f / fFreq;

	if(fAmp > 1.0f)
		fAmp = 1.0f;

	INDEX_DELTA(fFreq,iIndex, fDelta);

	if(iIndex >=iFreqToCutoffSize-1) {

		fCutoff =  1.0f;
	}
	else {

		fCutoff			=	mpfFreqToCutoff[iIndex]		* (1.0f-fDelta);
		fCutoff			+=	mpfFreqToCutoff[iIndex+1]	* fDelta;
	}
	//-------------------------------------------
	// Process Noise
	//-------------------------------------------
	for (int i = 0; i < lC; i++) {	
		// int noice
		miNoise			=	miNoise * 12345 + 6789;
		// float noise
		float fNoise	=	gfScaleNoise * miNoise;	
	//--------------------------------------------
	// Filter
	//--------------------------------------------
		// Chamberlin Filter
		mfB				=	mfBp;
		mfL				=	mfB * fCutoff + mfL;
		mfH				=	fNoise - mfL - mfB * 0.1f;	
		mfBp			=	mfH * fCutoff + mfBp;
		// Avoid denomarlization problems
		mfBp			+=	gfAnti_denormal;
		// Output from filter
		pfOut[i]		=	mfL * fAmp;
	}
}

void COscillatorNoise::ProcessNoise(tfloat32* pfOut, const tfloat fPitch, tint lC)
{
	//-------------------------------------------
	// Set filter cutoff
	//-------------------------------------------
	float	fCutoff, fDelta, fFreq, fAmp;				
	int	iIndex;

	// Move frequency up one octaves
	fFreq	=	2.0f * fPitch;

	fAmp	=	200.0f / fFreq;

	if(fAmp > 1.0f)
		fAmp = 1.0f;

	INDEX_DELTA(fFreq,iIndex, fDelta);

	if(iIndex >=iFreqToCutoffSize-1) {

		fCutoff =  1.0f;
	}
	else {

		fCutoff			=	mpfFreqToCutoff[iIndex]		* (1.0f-fDelta);
		fCutoff			+=	mpfFreqToCutoff[iIndex+1]	* fDelta;
	}
	//-------------------------------------------
	// Process Noise
	//-------------------------------------------
	for (int i = 0; i < lC; i++) {	
		// int noice
		miNoise			=	miNoise * 12345 + 6789;
		// float noise
		float fNoise	=	gfScaleNoise * miNoise;	
	//--------------------------------------------
	// Filter
	//--------------------------------------------
		// Chamberlin Filter
		mfB				=	mfBp;
		mfL				=	mfB * fCutoff + mfL;
		mfH				=	fNoise - mfL - mfB * 0.1f;	
		mfBp			=	mfH * fCutoff + mfBp;
		// Avoid denomarlization problems
		mfBp			+=	gfAnti_denormal;
		// Output from filter
		pfOut[i]		=	mfL * fAmp;
	}
}
void COscillatorNoise::ProcessNoiseFM(tfloat32* pfOut, const tfloat32* pfPitch, const tfloat32* pfFM, const tfloat32* pfFMAmt, tint lC)
{
	
}
