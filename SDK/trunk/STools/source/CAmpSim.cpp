

#include "SToolsInternalOS.h"


#define MINVALUE 1e-20f; 

#define jflone 0x3f800000
#define jflmsk 0x007fffff


IAmpSim* IAmpSim::Create()
{
	return dynamic_cast<IAmpSim*>(new CAmpSim());
}

void CAmpSim::Destroy()
{
	delete dynamic_cast<CAmpSim*>(this);
}

CAmpSim::CAmpSim()
{
	mfDrive	=	1.2f;
	CalckDrive();
	Reset();
	SetSampleRate(44100.0f);
	
}

CAmpSim::~CAmpSim()
{
}

void CAmpSim::SetSampleRate(float fSampleRate)
{
	double fWs = 2 * M_PI * 7 / fSampleRate * 0.5;
	double fC = cos(fWs) / sin(fWs);
	mfLc_a = (float)(fC / (1 + fC));
	mfLc_b1 = (float)((1 - fC) / (1 + fC));

	//100e-6 = 0.0001
	mfFollowerFactor = (float)(1 - exp(-1 / (0.00015 * fSampleRate)));
}

void CAmpSim::Reset()
{
	muiRandomLast = 0x426486;

	mfLc1_z1_L = mfLc2_z1_L = mfLc3_z1_L = 0.0f;

	mfA1_L = MINVALUE;
	mfA2_L = MINVALUE;
	mfA3_L = MINVALUE;
	mfA4_L = MINVALUE;
	mfA5_L = MINVALUE;
	mfA6_L = MINVALUE;
	mfA7_L = MINVALUE;
	mfA8_L = MINVALUE;
	mfA9_L = MINVALUE;
	mfA10_L = MINVALUE;


	mfLc1_z1_R = mfLc2_z1_R = mfLc3_z1_R = 0.0f;

	mfA1_R = MINVALUE;
	mfA2_R = MINVALUE;
	mfA3_R = MINVALUE;
	mfA4_R = MINVALUE;
	mfA5_R = MINVALUE;
	mfA6_R = MINVALUE;
	mfA7_R = MINVALUE;
	mfA8_R = MINVALUE;
	mfA9_R = MINVALUE;
	mfA10_R = MINVALUE;

}

void CAmpSim::Drive(float fDrive)
{
	mfDrive	=	fDrive;
	CalckDrive();
}

void CAmpSim::CalckDrive()
{
	mfPreAmpGain	=	mfDrive;
	mfOutputGain	=	1.8f / (0.8f+mfDrive) ;
}

void CAmpSim::Process(tfloat32* pfLeft, tfloat32* pfRight, tint32 iSamples)
{
	float fLevel;
	float fG;
	float fAccu;
	float fXn1_L;
	float fXn2_L;
	float fXn1_R;
	float fXn2_R;
	float fNoise;
	float fLoNoise;

	int iSample;
	for (iSample = 0; iSample < iSamples; iSample++) {
		// Random
		muiRandomLast = 1664525L * muiRandomLast + 1013904223L;
		unsigned int uiTmp = jflone | (jflmsk & muiRandomLast);
		float fRand = (*(float*)&uiTmp) - 1.0f;
		if (uiTmp == 0) {
			uiTmp = 0x213243;
		}
	    // Noise
		fNoise = (fRand - 0.5f) * 0.0005f;
		fLoNoise = fNoise*0.01f;
		// LEFT --------------------------------------
		// Denormalization
		pfLeft[iSample] += MINVALUE;
		pfLeft[iSample] += fLoNoise;
		// Remove DC and apply Preamp
		fAccu = pfLeft[iSample] * mfPreAmpGain - mfLc1_z1_L * mfLc_b1;
		fXn1_L = (fAccu - mfLc1_z1_L) * mfLc_a;
	    mfLc1_z1_L = fAccu;
		// Ensretning med usymetri
		fLevel =  fXn1_L > 0 ? fabs(fXn1_L * 0.5f) : fabs(fXn1_L);
		fLevel += MINVALUE;
		// Calck gain and add noise
		fG = 4 / (4 + fLevel) + fNoise;
		// Anti alias follower / filter
		mfA1_L =     fG <=	mfA1_L ?	fG  : mfA1_L + (fG - mfA1_L)	 * mfFollowerFactor;
		mfA2_L = mfA1_L <=	mfA2_L ? mfA1_L : mfA2_L + (mfA1_L - mfA2_L) * mfFollowerFactor;
		mfA3_L = mfA2_L <=	mfA3_L ? mfA2_L : mfA3_L + (mfA2_L - mfA3_L) * mfFollowerFactor;
		mfA4_L = mfA3_L <=  mfA4_L ? mfA3_L : mfA4_L + (mfA3_L - mfA4_L) * mfFollowerFactor;
		mfA5_L = mfA4_L <=  mfA5_L ? mfA4_L : mfA5_L + (mfA4_L - mfA5_L) * mfFollowerFactor;
		mfA6_L = mfA5_L <=	mfA6_L ? mfA5_L : mfA6_L + (mfA5_L - mfA6_L) * mfFollowerFactor;
		mfA7_L = mfA6_L <=  mfA7_L ? mfA6_L : mfA7_L + (mfA6_L - mfA7_L) * mfFollowerFactor;
		mfA8_L = mfA7_L <=  mfA8_L ? mfA7_L	: mfA8_L + (mfA7_L - mfA8_L) * mfFollowerFactor;
		mfA9_L = mfA8_L <=  mfA9_L ? mfA8_L	: mfA9_L + (mfA8_L - mfA9_L) * mfFollowerFactor;
		// Last section Act as filter
		mfA10_L = mfA10_L + (mfA9_L - mfA10_L) * mfFollowerFactor;
		// Wave shaping
		fXn2_L = fXn1_L * mfA10_L;
		// Remove DC and gain
		fAccu = fXn2_L * mfOutputGain - mfLc2_z1_L * mfLc_b1;// + fNoise;
		fXn2_L = (fAccu - mfLc2_z1_L) * mfLc_a;
		// Feedback
		mfLc2_z1_L = fAccu;
		// Copy o out
		pfLeft[iSample] = fXn2_L;

		// Right --------------------------------------
		// Denormalization
		pfRight[iSample] += MINVALUE;
		pfRight[iSample] += fLoNoise;
		// Remove DC and apply Preamp
		fAccu = pfRight[iSample] * mfPreAmpGain - mfLc1_z1_R * mfLc_b1;
		fXn1_R = (fAccu - mfLc1_z1_R) * mfLc_a;
	    mfLc1_z1_R = fAccu;
		// Ensretning med usymetri
		fLevel =  fXn1_R > 0 ? fabs(fXn1_R * 0.5f) : fabs(fXn1_R);
		fLevel += MINVALUE;
		// Calck gain and add noise
		fG = 4 / (4 + fLevel) + fNoise;
		// Anti alias follower / filter
		mfA1_R =     fG <=	mfA1_R ?	fG  : mfA1_R + (fG - mfA1_R)	 * mfFollowerFactor;
		mfA2_R = mfA1_R <=	mfA2_R ? mfA1_R : mfA2_R + (mfA1_R - mfA2_R) * mfFollowerFactor;
		mfA3_R = mfA2_R <=	mfA3_R ? mfA2_R : mfA3_R + (mfA2_R - mfA3_R) * mfFollowerFactor;
		mfA4_R = mfA3_R <=  mfA4_R ? mfA3_R : mfA4_R + (mfA3_R - mfA4_R) * mfFollowerFactor;
		mfA5_R = mfA4_R <=  mfA5_R ? mfA4_R : mfA5_R + (mfA4_R - mfA5_R) * mfFollowerFactor;
		mfA6_R = mfA5_R <=	mfA6_R ? mfA5_R : mfA6_R + (mfA5_R - mfA6_R) * mfFollowerFactor;
		mfA7_R = mfA6_R <=  mfA7_R ? mfA6_R : mfA7_R + (mfA6_R - mfA7_R) * mfFollowerFactor;
		mfA8_R = mfA7_R <=  mfA8_R ? mfA7_R	: mfA8_R + (mfA7_R - mfA8_R) * mfFollowerFactor;
		mfA9_R = mfA8_R <= mfA9_R  ? mfA8_R	: mfA9_R + (mfA8_R - mfA9_R) * mfFollowerFactor;
		// Last section Act as filter
		mfA10_R = mfA10_R + (mfA9_R - mfA10_R) * mfFollowerFactor;
		// Wave shaping
		fXn2_R = fXn1_R * mfA10_R;
		// Remove DC and gain
		fAccu = fXn2_R * mfOutputGain - mfLc2_z1_R * mfLc_b1;// + fNoise;
		fXn2_R = (fAccu - mfLc2_z1_R) * mfLc_a;
		// Feedback
		mfLc2_z1_R = fAccu;
		// Copy to out
		pfRight[iSample] = fXn2_R;

	}
	
}

