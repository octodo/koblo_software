

#include "BitCrunchOS.h"

#define EFFECT_ECHO_BUFFER_SIZE 131072*2

/*
	CBitCrunchDSP
*/

CBitCrunchDSP::CBitCrunchDSP()
	: muiChannels(0)
{
//	mpfBuffer1 = new tfloat[EFFECT_ECHO_BUFFER_SIZE];
//	mpfBuffer2 = new tfloat[EFFECT_ECHO_BUFFER_SIZE];
	
//	miDelayTime		=	22500;
//	mfFeedback		=	0.64f;
	
//	mfDamping		=	0.13605442177f; //!!! Change this
//	mfHPDamping		=	0.18140589569f; //!!! Change this
	
	mbBypass		=	false;
	mfIterator		=  0;
	
	mfLeft			=	0;
	mfRight			=	0;
	
//	Start();
}

CBitCrunchDSP::~CBitCrunchDSP()
{
//	delete[] mpfBuffer1;
//	mpfBuffer1 = NULL;

//	delete[] mpfBuffer2;
//	mpfBuffer2 = NULL;
	
}

void CBitCrunchDSP::Destroy()
{
	delete dynamic_cast<CBitCrunchDSP*>(this);
}

void CBitCrunchDSP::Start()
{
//	memset(mpfBuffer1, 0, EFFECT_ECHO_BUFFER_SIZE * sizeof(tfloat));
//	memset(mpfBuffer2, 0, EFFECT_ECHO_BUFFER_SIZE * sizeof(tfloat));

//	miPos = 0;

//	mpfLPHistory[0] = mpfLPHistory[1] = 0;
//	mpfHPHistory[0] = mpfHPHistory[1] = 0;
}

void CBitCrunchDSP::Stop()
{
}

void CBitCrunchDSP::SetMaxBufferSize(tint32 uiMaxBufferSize)
{
	CBaseDSPEngine::SetMaxBufferSize(uiMaxBufferSize);
}

void CBitCrunchDSP::SetSampleRate(tuint32 uiSampleRate)
{
	mfSampleRate	=	uiSampleRate;
	CBaseDSPEngine::SetSampleRate(uiSampleRate);
}

void CBitCrunchDSP::SetChannels(tuint32 uiChannels)
{
	muiChannels = uiChannels;
}

void CBitCrunchDSP::ProcessMono(float** ppfOut, const float** ppfIn, long lC)
{
	const float* pfIn	= ppfIn[0];
	float* pfOut		= ppfOut[0];

	if (mbBypass) {
		// Output meter
		tfloat32 fMeter = mpfMeterSignal[0];

		for (long l = 0; l < lC; l++) {
			pfOut[l] = pfIn[l];

			tfloat32 fOut = fabsf(pfOut[l]);

			if (fOut > fMeter) {
				fMeter = fOut;
			}
		}

		mpfMeterSignal[0] = fMeter;

		return;
	}


	// TODO: Replace with something meaningful
	{
		for (long l = 0; l < lC; l++) {
			pfOut[l] = pfIn[l];
		}
	}


	// Output meter
	tfloat32 fMeter = mpfMeterSignal[0];

	for (long l = 0; l < lC; l++) {
		tfloat32 fOut = fabsf(pfOut[l]);

		if (fOut > fMeter) {
			fMeter = fOut;
		}
	}

	mpfMeterSignal[0] = fMeter;
}

void CBitCrunchDSP::ProcessStereo(float** ppfOut, const float** ppfIn, long lC)
{
	const float* pfIn0	= ppfIn[0];
	const float* pfIn1	= ppfIn[1];
	float* pfOut0		= ppfOut[0];
	float* pfOut1		= ppfOut[1];

	if (mbBypass) {

		for (long l = 0; l < lC; l++) {
			pfOut0[l] = pfIn0[l];
			pfOut1[l] = pfIn1[l];
		}

		return;
	}

	for (long l = 0; l < lC; l++) {
	
		mfIterator += mfSR;
		
		if(mfIterator >= 1.0f){
			mfIterator -= 1.0f;
			
			tint32 iCrushL  = pfIn0[l] * mfBit;
			tint32 iCrushR  = pfIn1[l] * mfBit;

			mfLeft	=  mfMixWet * mfResBit * iCrushL ;
			mfRight	=  mfMixWet * mfResBit * iCrushR ;
		
			
			
		}
		pfOut0[l] = mfLeft;
		pfOut1[l] = mfRight;
		
		pfOut0[l] += pfIn0[l] * mfMixDry;
		pfOut1[l] += pfIn1[l] * mfMixDry;
	}

}

void CBitCrunchDSP::Process(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	if (muiChannels == 1) {
		ProcessMono(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
	else {
		ProcessStereo(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
}

void CBitCrunchDSP::Set_Bypass(tbool bBypass)
{
	mbBypass = bBypass;
}

void CBitCrunchDSP::Set_Bit(tint32 iBit)
{
	mfBit		=	iBit;
	mfResBit	=	1.0f / mfBit;
}

void CBitCrunchDSP:: Set_SR(tint32 iSR)
{
	mfSR =  (tfloat32)iSR / mfSampleRate;
}



void CBitCrunchDSP::Set_Mix(tint32 iMix)
{
	
	mfMixDry = iMix/128.0f;
	mfMixWet = 1.0f - mfMixDry;
	
}

