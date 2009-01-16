#include "KSOS.h"


void CRack_Echo_DSP::Process(tint32 iSamples)
{
	mpBuffer->SetChannelsNoConvert(2);
	
	if(miPower){

		tfloat32* pfL = mpBuffer->GetData(0);
		tfloat32* pfR = mpBuffer->GetData(1);

		// Start of echo
		tfloat* pfBuffer1 = mpfBuffer1;
		tfloat* pfBuffer2 = mpfBuffer2;
		tint32 iPos = miPos;

		for (tuint uiSample = 0; (tint)uiSample < iSamples; uiSample++) {
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
			pfBuffer1[iPos] = f1 + pfL[uiSample];
			pfBuffer2[iPos] = f2 + pfR[uiSample];

			// Write output
			pfL[uiSample] = f1 * mfAUX1_Return;
			pfR[uiSample] = f2 * mfAUX1_Return;
			
			miPos = iPos;
			// End of echo
			muiSongPos += iSamples;
		}
	}
	else{
		for (tuint uiSample = 0; (tint)uiSample < iSamples; uiSample++) {
			tfloat32* pfL = mpBuffer->GetData(0);
			tfloat32* pfR = mpBuffer->GetData(1);
			// Write output
			pfL[uiSample] = 0.0f;
			pfR[uiSample] = 0.0f;
		}
		// End of echo
		muiSongPos += iSamples;
	
	}

	
}

void CRack_Echo_DSP::Initialize()
{
 	mpfBuffer1 = new tfloat[EFFECT_ECHO_BUFFER_SIZE];
	mpfBuffer2 = new tfloat[EFFECT_ECHO_BUFFER_SIZE];
	
	miDelayTime		=	22500;
	mfFeedback		=	0.64f;
	
	mfDamping		=	0.13605442177f; //!!! Change this
	mfHPDamping		=	0.18140589569f; //!!! Change this
	
	Start();
}

void CRack_Echo_DSP::DeInitialize()
{
	delete[] mpfBuffer1;
	mpfBuffer1 = NULL;

	delete[] mpfBuffer2;
	mpfBuffer2 = NULL;
}

void CRack_Echo_DSP::ResetAllEffectsTails()
{
	Stop();
	Start();
}

void CRack_Echo_DSP::Start()
{
	memset(mpfBuffer1, 0, EFFECT_ECHO_BUFFER_SIZE * sizeof(tfloat));
	memset(mpfBuffer2, 0, EFFECT_ECHO_BUFFER_SIZE * sizeof(tfloat));

	miPos = 0;

	mpfLPHistory[0] = mpfLPHistory[1] = 0;
	mpfHPHistory[0] = mpfHPHistory[1] = 0;
}

void CRack_Echo_DSP::Stop()
{
}

void CRack_Echo_DSP::Set_Delay_Time(tint32 iMSec)
{
	miMSec = iMSec;
	
	Set_Delay_in_Samples();
	
//	miInterFaceDelayTime = iValue * GetSampleRate() / 1000;
//	miDelayTime = mbSyncToHost ? miHostDelayTime: miInterFaceDelayTime;
}

void CRack_Echo_DSP::Set_Delay_Power(tbool bPower)
{
	miPower = bPower;
	if(!bPower){
	// Reset buffers
		Start();
	}
}

void CRack_Echo_DSP:: Set_Feedback(tint32 iFeedback)
{
	mfFeedback = iFeedback * 0.008f;
}

void CRack_Echo_DSP::Set_LF_Damping(tint32 iLF_Damping)
{
	if (iLF_Damping == 16001) {
		mfDamping = 1;
	}
	else {
		mfDamping = (float)iLF_Damping / (44100 / 2);
	}
}

void CRack_Echo_DSP::Set_HP_Damping(tint32 iHP_Damping)
{
	if (iHP_Damping == 31) {
		mfHPDamping = 0;
	}
	else {
		mfHPDamping = (float)iHP_Damping / (44100 / 2);
	}
}

void CRack_Echo_DSP::Set_Sync_To_Tempo(tbool bSync)
{
	mbSyncToHost = bSync;
	
}

void CRack_Echo_DSP::Set_Delay_in_Samples()
{
	tint32 iSampleRate = gpApplication->GetSampleRate();
	miDelayTime = miMSec * iSampleRate / 1000;
}



void CRack_Echo_DSP::Set_AUX1_Return(tfloat32  fVal)
{

	mfAUX1_Return = fVal;
}


