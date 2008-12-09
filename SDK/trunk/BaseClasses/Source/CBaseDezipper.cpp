

#ifdef WIN32

// Base Utilities
#include "bu.h"

// Base Classes
#include "IBaseDezipperCallback.h"
#include "CBaseDezipper.h"

#endif	// WIN32


CBaseDezipper::CBaseDezipper()
	: mpCallback(NULL)
{
}

CBaseDezipper::~CBaseDezipper()
{
}

void CBaseDezipper::Destroy()
{
	delete dynamic_cast<CBaseDezipper*>(this);
}

void CBaseDezipper::SetCallback(IBaseDezipperCallback* pCallback)
{
	mpCallback = pCallback;
}

void CBaseDezipper::AddParameter(tint32 iID, tfloat fMin, tfloat fMax, tfloat fCurrent, tfloat64 fMSChangeTime)
{
	SParmInfo Info;
	Info.iParmID = iID;
	Info.fCurrentValue = fCurrent;
	Info.fTargetValue = fCurrent;

	Info.fChangePerMS = (fMax - fMin) / fMSChangeTime;

	mParmList.push_back(Info);
}

void CBaseDezipper::SetTarget(tint32 iID, tfloat fTarget)
{
	// Search for the parameter
	std::list<SParmInfo>::iterator it = mParmList.begin();

	for (; it != mParmList.end(); it++) {
		SParmInfo& Info = (*it);

		if (Info.iParmID == iID) {
			Info.fTargetValue = fTarget;

			// Call it once to be sure it has been called
//			ASSERT(mpCallback);
			if (mpCallback) {
				mpCallback->SetValue(Info.iParmID, (tint32)Info.fCurrentValue);
			}
			return;
		}
	}
}

void CBaseDezipper::Dezip(tfloat fMS)
{
	std::list<SParmInfo>::iterator it = mParmList.begin();

	for (; it != mParmList.end(); it++) {
		SParmInfo& Info = (*it);

		if (Info.fCurrentValue != Info.fTargetValue) {
			// Current and target not identical, so dezip
			if (Info.fCurrentValue < Info.fTargetValue) {
				// Current less than target, so we're moving up
				Info.fCurrentValue += (tfloat)(Info.fChangePerMS * fMS);
				if (Info.fCurrentValue >= Info.fTargetValue) {
					Info.fCurrentValue = Info.fTargetValue;
				}
			}
			else {
				// Current larger than target, so we're moving down
				Info.fCurrentValue -= (tfloat)(Info.fChangePerMS * fMS);
				if (Info.fCurrentValue <= Info.fTargetValue) {
					Info.fCurrentValue = Info.fTargetValue;
				}
			}

			mpCallback->SetValue(Info.iParmID, (tint32)Info.fCurrentValue);
		}
	}
}

tfloat CBaseDezipper::DezipSamples(tint32 iID, tint32 iSamples, tint32 iSampleRate)
{
	tfloat fMS = iSamples * 1000.0f / iSampleRate;

	// Note: We need a faster way to look up ID
	std::list<SParmInfo>::iterator it = mParmList.begin();
	for (; it != mParmList.end(); it++) {
		SParmInfo& Info = (*it);

		if (Info.iParmID == iID) {
			if (Info.fCurrentValue != Info.fTargetValue) {
				// Current and target not identical, so dezip
				if (Info.fCurrentValue < Info.fTargetValue) {
					// Current less than target, so we're moving up
					Info.fCurrentValue += (tfloat)(Info.fChangePerMS * fMS);
					if (Info.fCurrentValue >= Info.fTargetValue) {
						Info.fCurrentValue = Info.fTargetValue;
					}
				}
				else {
					// Current larger than target, so we're moving down
					Info.fCurrentValue -= (tfloat)(Info.fChangePerMS * fMS);
					if (Info.fCurrentValue <= Info.fTargetValue) {
						Info.fCurrentValue = Info.fTargetValue;
					}
				}
			}

			return Info.fCurrentValue;
		}
	}

	// We shouldn't be here
	ASSERT(0);
	return 0;
}

void CBaseDezipper::DezipSamples(tint32 iID, tfloat* pfDezip, tint32 iSamples, tint32 iSampleRate)
{
	tfloat fMSForOneSample = 1 * 1000.0f / iSampleRate;

	// Note: We need a faster way to look up ID
	std::list<SParmInfo>::iterator it = mParmList.begin();
	for (; it != mParmList.end(); it++) {
		SParmInfo& Info = (*it);

		if (Info.iParmID == iID) {
			if (Info.fCurrentValue != Info.fTargetValue) {
				// Current and target not identical, so dezip
				if (Info.fCurrentValue < Info.fTargetValue) {
					// Current less than target, so we're moving up
					tint32 iSample;
					for (iSample = 0; iSample < iSamples; iSample++) {
						Info.fCurrentValue += (tfloat)(Info.fChangePerMS * fMSForOneSample);
						if (Info.fCurrentValue >= Info.fTargetValue) {
							Info.fCurrentValue = Info.fTargetValue;
						}

						pfDezip[iSample] = Info.fCurrentValue;
					}
				}
				else {
					// Current larger than target, so we're moving down
					tint32 iSample;
					for (iSample = 0; iSample < iSamples; iSample++) {
						Info.fCurrentValue -= (tfloat)(Info.fChangePerMS * fMSForOneSample);
						if (Info.fCurrentValue <= Info.fTargetValue) {
							Info.fCurrentValue = Info.fTargetValue;
						}

						pfDezip[iSample] = Info.fCurrentValue;
					}
				}
			}
			else {
				// No dezipping needed
				tint32 iSample;
				for (iSample = 0; iSample < iSamples; iSample++) {
					pfDezip[iSample] = Info.fCurrentValue;
				}
			}

			return;
		}
	}

	// We shouldn't be here
	ASSERT(0);
}

void CBaseDezipper::Reset()
{
	std::list<SParmInfo>::iterator it = mParmList.begin();

	for (; it != mParmList.end(); it++) {
		SParmInfo& Info = (*it);

		if (Info.fCurrentValue != Info.fTargetValue) {
			mpCallback->SetValue(Info.iParmID, (tint32)Info.fTargetValue);
		}

		Info.fCurrentValue = Info.fTargetValue;
	}
}





