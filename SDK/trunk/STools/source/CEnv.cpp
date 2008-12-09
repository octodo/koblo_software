

#include "SToolsInternalOS.h"

// -160 dB
#define MIN_VOL 1e-8f

// -120 dB
#define MIN_SUSTAIN 1e-6f

// -100 dB
#define MIN_RESETVOL 1e-5f



IEnv* IEnv::Create()
{
	return dynamic_cast<IEnv*>(new CEnv());
}

void CEnv::Destroy()
{
	delete dynamic_cast<CEnv*>(this);
}

CEnv::CEnv()
{
	mfSampleRate = 44100;

	mfAttackVal = 0;
	mfHoldVal = 0;
	mfDecayVal = 0;
	mfSustainVal = 1;
	mfReleaseVal = 0;

	CalcAttack();
	CalcHold();
	CalcDecay();
	CalcSustain();
	CalcRelease();
}

CEnv::~CEnv()
{
}

void CEnv::SetSampleRate(float fSampleRate)
{
	mfSampleRate = fSampleRate;

	CalcAttack();
	CalcHold();
	CalcDecay();
	CalcSustain();
	CalcRelease();

	// Calc reset
	// ms.
	float fResetVal = 20.0f;
	float fSamples = mfSampleRate / 1000 * fResetVal;
	mfReset = (float)exp(log(0.0000000001) / fSamples);
}

void CEnv::Reset()
{
	meState		= eStateAttack;
	mfCurMul	= 0;
//	mlHold = 0;
}

void CEnv::ResetSoft()
{
	meState = eStateReset;
}

void CEnv::SetAttack(float fAttackMS)
{
	mfAttackVal = fAttackMS/32;
	CalcAttack();
}

void CEnv::SetHold(float fHoldMS)
{
	mfHoldVal = fHoldMS/32;
	CalcHold();
}

void CEnv::SetDecay(float fDecayMS)
{
	mfDecayVal = fDecayMS/32;
	CalcDecay();
}

void CEnv::SetSustain(float fSustain)
{
	mfSustainVal = fSustain;
	CalcSustain();
}

void CEnv::SetRelease(float fReleaseMS)
{
	mfReleaseVal = fReleaseMS/32;
	CalcRelease();
}

void CEnv::EnterRelease()
{
	meState = eStateRelease;
}

tbool CEnv::Process(tfloat32* pfOut, tint lC)
{
	bool	bMore	= true;
	float	fLast	= mfCurMul;

	switch(meState) {
		case eStateAttack:
			mfCurMul += mfAttack;
			if (mfCurMul >= 1) {
				mfCurMul = 1;
				mlHoldIndex = 0;
				meState = eStateHold;
			}
			break;
		case eStateHold:
			if (mlHoldIndex++ == mlHold) {
				meState = eStateDecay;
			}
			break;
		case eStateDecay:
			mfCurMul *= mfDecay;
			if (mfCurMul <= mfSustain) {
				mfCurMul = mfSustain;
				meState = eStateSustain;
				break;
			}
			break;
		case eStateSustain:
			break;
		case eStateRelease:	{
				bool bDone = false;
				mfCurMul *= mfRelease;
				if (mfCurMul < MIN_VOL) {
					bDone = true;
				}
				if (bDone) {
					bMore = false;
				}
			}
			break;
		case eStateReset:{
				bool bDone = false;
				if (bDone == false) {
					mfCurMul *= mfReset;
					if (mfCurMul < MIN_RESETVOL) {
						bDone = true;
					}
				}
				if (bDone) {
					meState = eStateAttack;
				}
			}
			break;
	}
	float fDist	= (mfCurMul - fLast) * gfOneDivThityTwo ;

	int i;

	for(i=0; i<lC; i++) {

		pfOut[i] =  fLast + (fDist * (i+1));
	}
	// Store acutaly uotput value
	mfCurMul = pfOut[i-1];
	return	bMore;
}

bool CEnv::Process32(tfloat32* pfOut)
{
	bool	bMore	= true;
	float	fLast	= mfCurMul;

	switch(meState) {
		case eStateAttack:
			mfCurMul += mfAttack;
			if (mfCurMul >= 1) {
				mfCurMul = 1;
				mlHoldIndex = 0;
				meState = eStateHold;
			}
			break;
		case eStateHold:
			if (mlHoldIndex++ == mlHold) {
				meState = eStateDecay;
			}
			break;
		case eStateDecay:
			mfCurMul *= mfDecay;
			if (mfCurMul <= mfSustain) {
				mfCurMul = mfSustain;
				meState = eStateSustain;
				break;
			}
			break;
		case eStateSustain:
			break;
		case eStateRelease:	{
				bool bDone = false;
				mfCurMul *= mfRelease;
				if (mfCurMul < MIN_VOL) {
					bDone = true;
				}
				if (bDone) {
					bMore = false;
				}
			}
			break;
		case eStateReset:{
				bool bDone = false;
				if (bDone == false) {
					mfCurMul *= mfReset;
					if (mfCurMul < MIN_RESETVOL) {
						bDone = true;
					}
				}
				if (bDone) {
					meState = eStateAttack;
				}
			}
			break;
	}
	float fDist	= (mfCurMul - fLast) * gfOneDivThityTwo ;

	pfOut[0]  =  fLast + (fDist * 1);
	pfOut[1]  =  fLast + (fDist * 2);
	pfOut[2]  =  fLast + (fDist * 3);
	pfOut[3]  =  fLast + (fDist * 4);
	pfOut[4]  =  fLast + (fDist * 5);
	pfOut[5]  =  fLast + (fDist * 6);
	pfOut[6]  =  fLast + (fDist * 7);
	pfOut[7]  =  fLast + (fDist * 8);
	pfOut[8]  =  fLast + (fDist * 9);
	pfOut[9]  =  fLast + (fDist * 10);
	pfOut[10] =  fLast + (fDist * 11);
	pfOut[11] =  fLast + (fDist * 12);
	pfOut[12] =  fLast + (fDist * 13);
	pfOut[13] =  fLast + (fDist * 14);
	pfOut[14] =  fLast + (fDist * 15);
	pfOut[15] =  fLast + (fDist * 16);
	pfOut[16] =  fLast + (fDist * 17);
	pfOut[17] =  fLast + (fDist * 18);
	pfOut[18] =  fLast + (fDist * 19);
	pfOut[19] =  fLast + (fDist * 20);
	pfOut[20] =  fLast + (fDist * 21);
	pfOut[21] =  fLast + (fDist * 22);
	pfOut[22] =  fLast + (fDist * 23);
	pfOut[23] =  fLast + (fDist * 24);
	pfOut[24] =  fLast + (fDist * 25);
	pfOut[25] =  fLast + (fDist * 26);
	pfOut[26] =  fLast + (fDist * 27);
	pfOut[27] =  fLast + (fDist * 28);
	pfOut[28] =  fLast + (fDist * 29);
	pfOut[29] =  fLast + (fDist * 30);
	pfOut[30] =  fLast + (fDist * 31);
	pfOut[31] =  fLast + (fDist * 32);

	return	bMore;
}

void CEnv::CalcAttack()
{
	if (mfAttackVal == 0) {
		mfAttack = 1;
		mbLinearAttackMode = true;
	}
	else {
		// High value, i.e. non-linear mode disabled
		if (mfAttackVal < 20000000.0f) {
			mfAttack = 1 / (mfSampleRate / 1000 * mfAttackVal);
			mbLinearAttackMode = true;
		}
		else {
			if (mfAttackVal == 0) {
				mfAttack = 0;
			}
			else {
				float fSamples = mfSampleRate / 1000 * mfAttackVal;
				mfAttack = (float)(1 / exp(log(0.0000000001) / fSamples));
			}

			mbLinearAttackMode = false;
		}
	}
}

void CEnv::CalcHold()
{
	if (mfHoldVal == 0) {
		mlHold = 0;
	}
	else {
		mlHold = (long)(mfSampleRate / 1000 * mfHoldVal);
	}
}

void CEnv::CalcDecay()
{
	if (mfDecayVal == 0) {
		mfDecay = 0;
	}
	else {
		float fSamples = mfSampleRate / 1000 * mfDecayVal;
		float fRange = 1 - mfSustain;
		if (fRange == 0) {
			mfDecay = 0;
		}
		else {
			if (fRange == 1) {
				mfDecay = 0;
			}
			else {
				mfDecay = exp(log(1 - fRange) / fSamples);
			}
		}
	}
}

void CEnv::CalcSustain()
{
	mfSustain = mfSustainVal;
	if (mfSustain < MIN_SUSTAIN) {
		mfSustain = MIN_SUSTAIN;
	}

	// Decay has to be re-calced
	CalcDecay();
}

void CEnv::CalcRelease()
{
	if (mfReleaseVal == 0) {
		mfRelease = 0;
	}
	else {
		float fSamples = mfSampleRate / 1000 * mfReleaseVal;
		mfRelease = (float)exp(log(0.0000000001) / fSamples);
	}
}

