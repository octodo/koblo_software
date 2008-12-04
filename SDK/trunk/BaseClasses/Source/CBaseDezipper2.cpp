

#ifdef WIN32

// Base Utilities
#include "bu.h"

// Base Classes
#include "IBaseDezipperCallback.h"
#include "CBaseDezipper2.h"

#endif	// WIN32


CBaseDezipper2::CBaseDezipper2(tfloat fMin, tfloat fMax, tfloat fCurrent, tfloat64 fMSChangeTime)
{
	mfCurrentValue = fCurrent;
	mfTargetValue = fCurrent;
	mbIsDezipNeeded = false;

	mfChangePerMS = (fMax - fMin) / fMSChangeTime;
}

CBaseDezipper2::~CBaseDezipper2()
{
}

void CBaseDezipper2::Destroy()
{
	delete dynamic_cast<CBaseDezipper2*>(this);
}

void CBaseDezipper2::SetTarget(tfloat fTarget)
{
	mfTargetValue = fTarget;
	mbIsDezipNeeded = (mfCurrentValue != mfTargetValue);
}

tfloat CBaseDezipper2::DezipSamples(tint32 iSamples, tint32 iSampleRate)
{
	tfloat fMS = iSamples * 1000.0f / iSampleRate;

	if (mbIsDezipNeeded) {
		// Current and target not identical, so dezip
		if (mfCurrentValue < mfTargetValue) {
			// Current less than target, so we're moving up
			mfCurrentValue += (tfloat)(mfChangePerMS * fMS);
			if (mfCurrentValue >= mfTargetValue) {
				mfCurrentValue = mfTargetValue;
				mbIsDezipNeeded = false;
			}
		}
		else {
			// Current larger than target, so we're moving down
			mfCurrentValue -= (tfloat)(mfChangePerMS * fMS);
			if (mfCurrentValue <= mfTargetValue) {
				mfCurrentValue = mfTargetValue;
				mbIsDezipNeeded = false;
			}
		}
	}

	return mfCurrentValue;
}

void CBaseDezipper2::DezipSamples(tfloat* pfDezip, tint32 iSamples, tint32 iSampleRate)
{
	tfloat fMSForOneSample = 1 * 1000.0f / iSampleRate;

	if (mbIsDezipNeeded) {
		// Current and target not identical, so dezip
		if (mfCurrentValue < mfTargetValue) {
			// Current less than target, so we're moving up
			tint32 iSample;
			for (iSample = 0; iSample < iSamples; iSample++) {
				mfCurrentValue += (tfloat)(mfChangePerMS * fMSForOneSample);
				if (mfCurrentValue >= mfTargetValue) {
					mfCurrentValue = mfTargetValue;
					mbIsDezipNeeded = false;
				}

				pfDezip[iSample] = mfCurrentValue;
			}
		}
		else {
			// Current larger than target, so we're moving down
			tint32 iSample;
			for (iSample = 0; iSample < iSamples; iSample++) {
				mfCurrentValue -= (tfloat)(mfChangePerMS * fMSForOneSample);
				if (mfCurrentValue <= mfTargetValue) {
					mfCurrentValue = mfTargetValue;
					mbIsDezipNeeded = false;
				}

				pfDezip[iSample] = mfCurrentValue;
			}
		}
	}
	else {
		// No dezipping needed
		tint32 iSample;
		for (iSample = 0; iSample < iSamples; iSample++) {
			pfDezip[iSample] = mfCurrentValue;
		}
	}
}

void CBaseDezipper2::Reset()
{
	mfCurrentValue = mfTargetValue;
	mbIsDezipNeeded = false;
}





