

#include "DigitalEchoOS.h"

#define EFFECT_ECHO_BUFFER_SIZE 131072*2

/*
	CDigitalEchoDSP
*/

CDigitalEchoDSP::CDigitalEchoDSP()
	: muiChannels(0)
{
	mpfBuffer1 = new tfloat[EFFECT_ECHO_BUFFER_SIZE];
	mpfBuffer2 = new tfloat[EFFECT_ECHO_BUFFER_SIZE];
	
	miDelayTime		=	22500;
	mfFeedback		=	0.64f;
	
	mfDamping		=	0.13605442177f; //!!! Change this
	mfHPDamping		=	0.18140589569f; //!!! Change this
	
	mbBypass		=	false;
	
//	Start();
}

CDigitalEchoDSP::~CDigitalEchoDSP()
{
	delete[] mpfBuffer1;
	mpfBuffer1 = NULL;

	delete[] mpfBuffer2;
	mpfBuffer2 = NULL;
	
}

void CDigitalEchoDSP::Destroy()
{
	delete dynamic_cast<CDigitalEchoDSP*>(this);
}

void CDigitalEchoDSP::Start()
{
	memset(mpfBuffer1, 0, EFFECT_ECHO_BUFFER_SIZE * sizeof(tfloat));
	memset(mpfBuffer2, 0, EFFECT_ECHO_BUFFER_SIZE * sizeof(tfloat));

	miPos = 0;

	mpfLPHistory[0] = mpfLPHistory[1] = 0;
	mpfHPHistory[0] = mpfHPHistory[1] = 0;
}

void CDigitalEchoDSP::Stop()
{
}

void CDigitalEchoDSP::SetMaxBufferSize(tint32 uiMaxBufferSize)
{
	CBaseDSPEngine::SetMaxBufferSize(uiMaxBufferSize);
}

void CDigitalEchoDSP::SetSampleRate(tuint32 uiSampleRate)
{
	muiSampleRate	=	uiSampleRate;
	CBaseDSPEngine::SetSampleRate(uiSampleRate);
}

void CDigitalEchoDSP::SetChannels(tuint32 uiChannels)
{
	muiChannels = uiChannels;
}

void CDigitalEchoDSP::ProcessMono(float** ppfOut, const float** ppfIn, long lC)
{
	const float* pfIn = ppfIn[0];
	float* pfOut = ppfOut[0];

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

void CDigitalEchoDSP::ProcessStereo(float** ppfOut, const float** ppfIn, long lC)
{
	const float* pfIn0 = ppfIn[0];
	const float* pfIn1 = ppfIn[1];
	float* pfOut0 = ppfOut[0];
	float* pfOut1 = ppfOut[1];

	if (mbBypass) {

		for (long l = 0; l < lC; l++) {
			pfOut0[l] = pfIn0[l];
			pfOut1[l] = pfIn1[l];

			tfloat32 fOutL = fabsf(pfOut0[l]);
			tfloat32 fOutR = fabsf(pfOut1[l]);
		}
		return;
	}




	tfloat* pfBuffer1 = mpfBuffer1;
	tfloat* pfBuffer2 = mpfBuffer2;
	tint32 iPos = miPos;
	
	
	for (long l = 0; l < lC; l++) {
		// Move writing position forward
		iPos++;
		// Wrap (if neccasary)
		iPos &= (EFFECT_ECHO_BUFFER_SIZE - 1);

		// Find reading tap
		tint32 iTap = iPos - miDelayTime;
		iTap &= (EFFECT_ECHO_BUFFER_SIZE - 1);

		// Read from buffer
		tfloat f1 = pfBuffer1[iTap];
		tfloat f2 = pfBuffer2[iTap];

		// Apply feedback
		f1 *= mfFeedback;
		f2 *= mfFeedback;

		// Apply LP
		f1 = mpfLPHistory[0] = gfAnti_denormal + mpfLPHistory[0] + (f1 - mpfLPHistory[0]) * mfDamping;
		f2 = mpfLPHistory[1] = gfAnti_denormal + mpfLPHistory[1] + (f2 - mpfLPHistory[1]) * mfDamping;

		// Apply HP
		mpfHPHistory[0] = gfAnti_denormal + mpfHPHistory[0] + (f1 - mpfHPHistory[0]) * mfHPDamping;
		f1 = f1 - mpfHPHistory[0];
		mpfHPHistory[1] = gfAnti_denormal + mpfHPHistory[1] + (f2 - mpfHPHistory[1]) * mfHPDamping;
		f2 = f2 - mpfHPHistory[1];

		// Write to circular buffer
		pfBuffer1[iPos] = f1 + pfIn0[l];
		pfBuffer2[iPos] = f2 + pfIn1[l];

		// Write output
		pfOut0[l] = f1 * mfMixWet;
		pfOut1[l] = f2 * mfMixWet;
		pfOut0[l] += pfIn0[l] * mfMixDry;
		pfOut1[l] += pfIn1[l] * mfMixDry;
	}
	
	miPos = iPos;
}

void CDigitalEchoDSP::Process(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	if (muiChannels == 1) {
		ProcessMono(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
	else {
		ProcessStereo(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
}
/*
void CDigitalEchoDSP::Set_Bypass(tbool bBypass)
{
	mbBypass = bBypass;
}
*/
void CDigitalEchoDSP::Set_Delay_Time(tint32 iMSec)
{
	miMSec = iMSec;
	
	Set_Delay_in_Samples();
	
}

void CDigitalEchoDSP:: Set_Feedback(tint32 iFeedback)
{
	mfFeedback = iFeedback * 0.008f;
}

void CDigitalEchoDSP::Set_LF_Damping(tint32 iLF_Damping)
{
	if (iLF_Damping == 16001) {
		mfDamping = 1;
	}
	else {
		mfDamping = (float)iLF_Damping / (44100 / 2);
	}
}

void CDigitalEchoDSP::Set_HF_Damping(tint32 iHP_Damping)
{
	if (iHP_Damping == 31) {
		mfHPDamping = 0;
	}
	else {
		mfHPDamping = (float)iHP_Damping / (44100 / 2);
	}
}

void CDigitalEchoDSP::Set_Sync_To_Tempo(tbool bSync)
{
	mbSyncToHost = bSync;
	
//	Set_Delay_in_Samples();
}

void CDigitalEchoDSP::Set_Delay_in_Samples()
{
	tint32 iSampleRate = muiSampleRate;
	miDelayTime = miMSec * iSampleRate / 1000;
}

void CDigitalEchoDSP::Set_Mix(tint32 iMix)
{
	
	mfMixWet = iMix/128.0f;
		
	mfMixDry = 1.0f - mfMixWet;
	

}

