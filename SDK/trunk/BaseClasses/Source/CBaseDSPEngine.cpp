

#ifdef WIN32

// Base Utilities
#include "bu.h"

// Graphical Engine
#include "ge.h"

// KSPI
#include "kspi.h"
#include "kspiC.h"

// Base Classes
#include "BaseControlIDs.h"
#include "FontSizes.h"
#include "IBaseDezipperCallback.h"
#include "CBaseMIDICCEvent.h"
#include "CBaseDezipper.h"
#include "CBaseDezipper2.h"
#include "CBaseDSPEngine.h"
#include "CBaseModule.h"
#include "CBasePlugIn.h"
#include "CBasePane.h"
#include "CBasePaneStd.h"
#include "CBaseGUI.h"
#include "CBaseTopPane.h"
#endif	// WIN32


CBaseDSPEngine::CBaseDSPEngine()
	: muiMaxBufferSize(0), muiSampleRate(0), mbBypass(true)
{

	tint iMeter;
	for (iMeter = 0; iMeter < BASE_MAX_NR_OF_SIGNAL_METERS; iMeter++) {
		mpfMeterSignal[iMeter] = 0;
	}
	for (iMeter = 0; iMeter < BASE_MAX_NR_OF_GR_METERS; iMeter++) {
		mpfMeterGR[iMeter] = 1.0f;
	}

}

CBaseDSPEngine::~CBaseDSPEngine()
{
}

void CBaseDSPEngine::SetMaxBufferSize(tint32 uiMaxBufferSize)
{
	muiMaxBufferSize = uiMaxBufferSize;
}

tbool CBaseDSPEngine::SupportSampleRate(tuint32 uiSampleRate)
{
	return true;
}

void CBaseDSPEngine::SetSampleRate(tuint32 uiSampleRate)
{
	muiSampleRate = uiSampleRate;
}

tfloat CBaseDSPEngine::GetMeterSignal(tint iIndex)
{
	tfloat fRet = mpfMeterSignal[iIndex];

	return fRet;
}

tfloat CBaseDSPEngine::GetMeterGR(tint iIndex)
{
	tfloat fRet = mpfMeterGR[iIndex];
	return fRet;

}

tfloat CBaseDSPEngine::GetMeterSignalAndClear(tint iIndex)
{
	tfloat fRet = mpfMeterSignal[iIndex];

	mpfMeterSignal[iIndex] = 0;

	return fRet;
}

tfloat CBaseDSPEngine::GetMeterGRAndClear(tint iIndex)
{

	tfloat fRet = mpfMeterGR[iIndex];
	mpfMeterGR[iIndex] = 1.0f;
	return fRet;
}

void CBaseDSPEngine::SetBypass(tbool bBypass)
{
	mbBypass = bBypass;
}





