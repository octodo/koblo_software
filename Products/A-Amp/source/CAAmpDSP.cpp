

#include "AAmpOS.h"

/*
	CAAmpDSP
*/

CAAmpDSP::CAAmpDSP()
	: muiChannels(0)
{
	mpAmpSim			=	st::IAmpSim::Create();
	mpLimiter			=	st::ILimiter::Create();
	mpDriveLimiter			=	st::ILimiter::Create();
	mpSoftDistortion	=	st::IDrive::Create();
	mpClipper			=	st::IClipper::Create();
	mpAmp				=	st::IAmp::Create();
	
	mpAmpSim->Reset();
	mpLimiter->Reset();
	mfClip	=	1.0f;
	
//	Start();
}

CAAmpDSP::~CAAmpDSP()
{
	
	mpAmpSim->Destroy();
	mpLimiter->Destroy();
	mpSoftDistortion->Destroy();
}

void CAAmpDSP::Destroy()
{
	delete dynamic_cast<CAAmpDSP*>(this);
}

void CAAmpDSP::Start()
{
	
}

void CAAmpDSP::Stop()
{
	mpAmpSim->Reset();
	mpLimiter->Reset();
}

void CAAmpDSP::SetMaxBufferSize(tint32 uiMaxBufferSize)
{
	CBaseDSPEngine::SetMaxBufferSize(uiMaxBufferSize);
}

void CAAmpDSP::SetSampleRate(tuint32 uiSampleRate)
{
	muiSampleRate	=	uiSampleRate;
	mpAmpSim->SetSampleRate(uiSampleRate);
	mpLimiter->SetSampleRate(uiSampleRate);
	CBaseDSPEngine::SetSampleRate(uiSampleRate);
}

void CAAmpDSP::SetChannels(tuint32 uiChannels)
{
	muiChannels = uiChannels;
}

void CAAmpDSP::ProcessMono(float** ppfOut, const float** ppfIn, long lC)
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

void CAAmpDSP::ProcessStereo(float** ppfOut, const float** ppfIn, long lC)
{
	const float* pfIn0 = ppfIn[0];
	const float* pfIn1 = ppfIn[1];
	float* pfOut0 = ppfOut[0];
	float* pfOut1 = ppfOut[1];
	
/*
	if (mbBypass) {
		for (long l = 0; l < lC; l++) {
			pfOut0[l] = pfIn0[l];
			pfOut1[l] = pfIn1[l];

			tfloat32 fOutL = fabsf(pfOut0[l]);
			tfloat32 fOutR = fabsf(pfOut1[l]);
		}
		return;
	}
*/
	for (long i = 0; i < lC; i++) {
		pfOut0[i] = pfIn0[i] * mfVol;
		pfOut1[i] = pfIn1[i] * mfVol;
	}
	
	mpAmp->Process32(mfVol,pfOut0,pfOut1);
	
	switch(muiSwap){
		case 0: {
			
			if(mbTube)		mpAmpSim->Process(pfOut0,pfOut1,lC);
			if(mbHarm){
				mpAmp->Process32(mfDrive,pfOut0,pfOut1);
				//mpDriveLimiter->Process(pfOut0,pfOut1,lC);
				mpClipper->Process(mfClip, pfOut0,pfOut1,lC);
				mpSoftDistortion->ProcessHarch(pfOut0,pfOut1,mfHarch,lC);
			}
			if(mbLimiter)	mpLimiter->Process(pfOut0,pfOut1,lC);break;
			break;
		}
		case 1: {
			if(mbTube)		mpAmpSim->Process(pfOut0,pfOut1,lC);
			if(mbLimiter)	mpLimiter->Process(pfOut0,pfOut1,lC);
			if(mbHarm){
				mpAmp->Process32(mfDrive,pfOut0,pfOut1);
				//mpDriveLimiter->Process(pfOut0,pfOut1,lC);
				mpClipper->Process(mfClip, pfOut0,pfOut1,lC);
				mpSoftDistortion->ProcessHarch(pfOut0,pfOut1,mfHarch,lC);
			}	
			break;
		}
		case 2: {
			
			if(mbHarm){
				mpAmp->Process32(mfDrive,pfOut0,pfOut1);
				//mpDriveLimiter->Process(pfOut0,pfOut1,lC);
				mpClipper->Process(mfClip, pfOut0,pfOut1,lC);
				mpSoftDistortion->ProcessHarch(pfOut0,pfOut1,mfHarch,lC);
			}	
			if(mbTube)		mpAmpSim->Process(pfOut0,pfOut1,lC);
			if(mbLimiter)	mpLimiter->Process(pfOut0,pfOut1,lC);
			break;
		}
		case 3: {
			
			if(mbHarm){
				mpAmp->Process32(mfDrive,pfOut0,pfOut1);
				//mpDriveLimiter->Process(pfOut0,pfOut1,lC);
				mpClipper->Process(mfClip, pfOut0,pfOut1,lC);
				mpSoftDistortion->ProcessHarch(pfOut0,pfOut1,mfHarch,lC);
			}	
			if(mbLimiter)	mpLimiter->Process(pfOut0,pfOut1,lC);
			if(mbTube)		mpAmpSim->Process(pfOut0,pfOut1,lC);
			break;
		}
		case 4: {
			if(mbLimiter)	mpLimiter->Process(pfOut0,pfOut1,lC);
			if(mbTube)		mpAmpSim->Process(pfOut0,pfOut1,lC);
			if(mbHarm){
				mpAmp->Process32(mfDrive,pfOut0,pfOut1);
				//mpDriveLimiter->Process(pfOut0,pfOut1,lC);
				mpClipper->Process(mfClip, pfOut0,pfOut1,lC);
				mpSoftDistortion->ProcessHarch(pfOut0,pfOut1,mfHarch,lC);
			}	
			break;
			
		}
		case 5: {
			
			if(mbLimiter)	mpLimiter->Process(pfOut0,pfOut1,lC);
			if(mbHarm){
				mpAmp->Process32(mfDrive,pfOut0,pfOut1);
				//mpDriveLimiter->Process(pfOut0,pfOut1,lC);
				mpClipper->Process(mfClip, pfOut0,pfOut1,lC);
				mpSoftDistortion->ProcessHarch(pfOut0,pfOut1,mfHarch,lC);
			}	
			if(mbTube)		mpAmpSim->Process(pfOut0,pfOut1,lC);
			
			break;
			
		}
	}
	// Clip and Find peak;
	for (long i = 0; i < lC; i++) {
		
	/*	if(pfOut0[i] > 1.0) pfOut0[i]		= 1.0f;
		else if(pfOut0[i] < -1.0) pfOut0[i]	= -1.0f;
		
		if(pfOut1[i] > 1.0) pfOut1[i]		= 1.0f;
		else if(pfOut1[i] < -1.0) pfOut1[i]	= -1.0f;
	*/
		
		tfloat fPeak = fabs(pfOut0[i]) >fabs( pfOut1[i]) ? fabs(pfOut0[i]): fabs(pfOut1[i]);
		
		
	}
	
	
	// Output meter
	tfloat32 fMeter = mpfMeterSignal[0];

	
	for (long l = 0; l < lC; l++) {
		
		tfloat32 fOutL = fabsf(pfOut0[l]);
		tfloat32 fOutR = fabsf(pfOut1[l]);
		
		tfloat fPeak = fOutL > fOutR ? fOutL: fOutR;
		
		if (fPeak > fMeter) {
			fMeter = fPeak;
		}
		else
			fMeter *= 0.99995f;
		
	}
	mpfMeterSignal[0] = fMeter;
	 

}

void CAAmpDSP::Process(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	if (muiChannels == 1) {
		ProcessMono(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
	else {
		ProcessStereo(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
}
/*
void CAAmpDSP::Set_Bypass(tbool bBypass)
{
	mbBypass = bBypass;
}
*/


void CAAmpDSP::Set_Gain(tfloat32 fValue)
{
	
	mfVol		=	fValue - 0.999999f;		// Range 0.0 - 4.0;
	mfDrive		=	1.0f / fValue;			// Range 1.0 - 0.2
	mfDrive		-=	0.2f;
	mfDrive		*=	1.25;					// Range 1,0 - 0.0
	mfHarch		=	3.0f - mfDrive;			// Range 1.0 - 2.0
	mfDrive		*=	2.0f;					// Range 2.0 - 0.0;
	mfDrive		=	3.0 - mfDrive ;			// Range 1.0 - 2.0;
	mfDrive		=	1.0f / mfDrive;			// Range 1.0 - 0.5;
	
	mfClip		=	2.0f;
  	mpAmpSim->Drive( 5.6 );
	
}


