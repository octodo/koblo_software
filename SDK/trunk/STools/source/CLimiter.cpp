

#include "SToolsInternalOS.h"


#define MINVALUE 1e-20f;

#define jflone 0x3f800000
#define jflmsk 0x007fffff


ILimiter* ILimiter::Create()
{
	return dynamic_cast<ILimiter*>(new CLimiter());
}

void CLimiter::Destroy()
{
	delete dynamic_cast<CLimiter*>(this);
}

CLimiter::CLimiter()
{
	Reset();
}

CLimiter::~CLimiter()
{
}

void CLimiter::SetSampleRate(float fSampleRate)
{
	tfloat fSR	= fSampleRate;
	mfK			= 1.0e6F / fSR;
	mfK1		= 1.0F / (fSR*8.0e-6f);
	mfK2		= 1.0F / (fSR*20.0e-6f);
	Reset();
	
}

void CLimiter::Reset()
{
	mfThresh	= 0.5f;
	mfKR1		= 1.0e-5f;
	mfKR2		= 1.0e-5f;
	mfKRs		= 1.0F / 220000.0f;
	mfKin		= 0.001F;
	mfK12		= 2.4545e-5f;
	mfDrive		= (float)pow(10.0F, 0.005f);
	mfEnv		= mfEnv1 = mfEnv2 = 0.0F;

}

void CLimiter::Process(tfloat32* pfLeft, tfloat32* pfRight, tint32 iSamples)
{

	for( tint i = 0; i<iSamples; i++) {
		
		tfloat32 fInL = pfLeft[i];
		tfloat32 fInR = pfRight[i];
		tfloat32 fOutL;
		tfloat32 fOutR;

		tfloat fAmp = (float)exp(-5.0F * mfEnv) * mfDrive;

		fOutL = fInL * fAmp;
		fOutR = fInR * fAmp;

		fAmp = fabs(fOutL) > fabs(fOutR) ? fabs(fOutL) : fabs(fOutR);
		tfloat fVin = fAmp - mfThresh - mfEnv;

		if (fVin < 0.0F) fVin = 0.0f;

		mfEnv	+= mfK  * (fVin*mfKin + mfEnv1* mfKR1 + mfEnv2*mfKR2 - mfEnv*mfK12);
		mfEnv1	+= mfK1 * (mfEnv - mfEnv1)	 * mfKR1;
		mfEnv2	+= mfK2 * (mfEnv - mfEnv2)	 * mfKR2;

		// Copy to output
		pfLeft[i]	=	fOutL;
		pfRight[i]	=	fOutR;
	}
	
}

