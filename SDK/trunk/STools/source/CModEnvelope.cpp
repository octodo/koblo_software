

#include "SToolsInternalOS.h"

// -160 dB
#define MIN_VOL 1e-8f

// -120 dB
#define MIN_SUSTAIN 1e-6f

// -100 dB
#define MIN_RESETVOL 1e-5f



IModEnvelope* IModEnvelope::Create()
{
	return dynamic_cast<IModEnvelope*>(new CModEnvelope());
}

void CModEnvelope::Destroy()
{
	delete dynamic_cast<CModEnvelope*>(this);
}

CModEnvelope::CModEnvelope()
{
	mfSampleRate = 44100;

	mfAttackVal = 10;
	mfHoldVal = 10;
	mfDecayVal = 10;
	mfSustainVal = 1;
	mfReleaseVal = 10;

	CalcAttack();
	CalcHold();
	CalcDecay();
	CalcSustain();
	CalcRelease();
}

CModEnvelope::~CModEnvelope()
{
}

void CModEnvelope::SetSampleRate(float fSampleRate)
{
	mfSampleRate = fSampleRate;

	CalcAttack();
	CalcHold();
	CalcDecay();
	CalcSustain();
	CalcRelease();

	// Calc reset
	// ms.
	float fResetVal = 20.0f*32;
	float fSamples = mfSampleRate / 1000 * fResetVal;
	mfReset = (float)exp(log(0.0000000001) / fSamples);
}

void CModEnvelope::Reset()
{
	meState = eStateAttack;
	mfCurMul = 0;
}

void CModEnvelope::ResetSoft()
{
	meState = eStateReset;
}

void CModEnvelope::SetAttack(float fAttackMS)
{
	mfAttackVal = fAttackMS/32;
	CalcAttack();
}

void CModEnvelope::SetHold(float fHoldMS)
{
	mfHoldVal = fHoldMS/32;
	CalcHold();
}

void CModEnvelope::SetDecay(float fDecayMS)
{
	mfDecayVal = fDecayMS/32;
	CalcDecay();
}

void CModEnvelope::SetSustain(float fSustain)
{
	mfSustainVal = fSustain;
	CalcSustain();
}

void CModEnvelope::SetRelease(float fReleaseMS)
{
	mfReleaseVal = fReleaseMS/32;
	CalcRelease();
}

void CModEnvelope::EnterRelease()
{
	meState = eStateRelease;
}

tfloat CModEnvelope::Process(tint lC)
{
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
		case eStateRelease:
				
			mfCurMul *= mfRelease;

			break;
		case eStateReset:
			{
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

	float fDist	= (mfCurMul - fLast) * gfOneDivThityTwo;
	mfCurMul	=  fLast + (fDist * lC);
	return mfCurMul;
}

tfloat CModEnvelope::ProcessMul(tint lC)
{/*
	sample* pfEnd = pfOut + lC;
	while (pfOut != pfEnd) {
		switch(meState) {
			case eStateAttack:
				while (pfOut != pfEnd) {
					if (mbLinearAttackMode) {
						mfCurMul += mfAttack;
					}
					else {
						if (mfCurMul < 0.0000000001f) {
							mfCurMul = 0.0000000001f;
						}
						mfCurMul *= mfAttack;
					}
					if (mfCurMul >= 1) {
						mfCurMul = 1;
						pfOut++;
						mlHoldIndex = 0;
						meState = eStateHold;
						break;
					}
					else {
						*pfOut++ *= mfCurMul;
					}
				}
				break;
			case eStateHold:
				while (pfOut != pfEnd) {
					if (mlHoldIndex++ == mlHold) {
						meState = eStateDecay;
						break;
					}
					else {
						pfOut++;
					}
				}
				break;
			case eStateDecay:
				while (pfOut != pfEnd) {
					mfCurMul *= mfDecay;
					if (mfCurMul <= mfSustain) {
						mfCurMul = mfSustain;
						*pfOut++ *= mfCurMul;
						meState = eStateSustain;
						break;
					}
					else {
						*pfOut++ *= mfCurMul;
					}
				}
				break;
			case eStateSustain:
				while (pfOut != pfEnd) {
					*pfOut++ *= mfSustain;
				}
				break;
			case eStateRelease:
				{
					bool bDone = false;
					while (pfOut != pfEnd) {
						mfCurMul *= mfRelease;
						*pfOut++ *= mfCurMul;
						if (mfCurMul < MIN_VOL) {
							bDone = true;
						}
					}
					if (bDone) {
						return false;
					}
				}
				break;
			case eStateReset:
				{
					bool bDone = false;
					while ((pfOut != pfEnd) && (bDone == false)) {
						mfCurMul *= mfReset;
						*pfOut++ *= mfCurMul;
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
	}
	*/

	return 0;
}

void CModEnvelope::CalcAttack()
{
	mfAttack = 1 / (mfSampleRate / 1000 * mfAttackVal);
	/*
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
				mfAttack = 1 / exp(log(0.0000000001) / fSamples);
			}

			mbLinearAttackMode = false;
		}
	}
	*/
}

void CModEnvelope::CalcHold()
{
	if (mfHoldVal == 0) {
		mlHold = 0;
	}
	else {
		mlHold = (long)(mfSampleRate / 1000 * mfHoldVal);
	}
}

void CModEnvelope::CalcDecay()
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

void CModEnvelope::CalcSustain()
{
	mfSustain = mfSustainVal;
	if (mfSustain < MIN_SUSTAIN) {
		mfSustain = MIN_SUSTAIN;
	}

	// Decay has to be re-calced
	CalcDecay();
}

void CModEnvelope::CalcRelease()
{
	if (mfReleaseVal == 0) {
		mfRelease = 0;
	}
	else {
		float fSamples = mfSampleRate / 1000 * mfReleaseVal;
		mfRelease = (float)exp(log(0.0000000001) / fSamples);
	}
}

