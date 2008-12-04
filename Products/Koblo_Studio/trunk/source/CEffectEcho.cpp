/*!	\file CEffectEcho.cpp
	\author Michael Olsen
	\date 19/Apr/2005
	\date 19/Apr/2005
*/

#include "OMPSOS.h"

// (mo) Not sure this is the right value
const tfloat32 gfAnti_denormal = 1e-10f;

//! Used by CEffectEcho::GetParameterInfo
//static tint32 giStrength1000 = 1000;

//! Parameter indices for echo effect
enum EParmEcho
{
	giParamDelayOn = 0,
	giParamDelayTime,
	giParamFeedback,
	giParamDamping,
	giParamHighFreqDamping,
	giParamSyncToHost,

	NrOfParams
};

CEffectEcho::CEffectEcho()
	: mpfBuffer1(NULL), mpfBuffer2(NULL)
{
}

CEffectEcho::~CEffectEcho()
{
	if (mpfBuffer1) {
		delete[] mpfBuffer1;
	}
	if (mpfBuffer2) {
		delete[] mpfBuffer2;
	}
}

void CEffectEcho::Destroy()
{
	delete dynamic_cast<CEffectEcho*>(this);
}

void CEffectEcho::Initialize()
{
	mpfBuffer1 = new tfloat[EFFECT_ECHO_BUFFER_SIZE];
	mpfBuffer2 = new tfloat[EFFECT_ECHO_BUFFER_SIZE];

	mpParmMan->SetGlobalSectionCount(1);

	AddEffectParm(0, giParamDelayOn, 0, 1, 0);
	AddEffectParm(0, giParamDelayTime, 1, 1000, 500);
	AddEffectParm(0, giParamFeedback, 0, 100, 80);
	AddEffectParm(0, giParamDamping, 32, 16001, 12000);
	AddEffectParm(0, giParamHighFreqDamping, 31, 16000, 31);
	AddEffectParm(0, giParamSyncToHost, 0, 1, 0);

	ge::IControl* pControl;
	// Note: All controls must have an ID, even if it's not used. So NoID cannot be used in effects.
	pControl = mpPane->CreateBitmap(GetUniqueID(), IDB_Mask_Echo, ge::SPos(1, 0));
	GetWindow()->ControlAdded(pControl->GetID(), pControl);
	
//	de::IDisplayConverter* pConverter  = new CX3DisplayConverter();
//	de::IDisplayConverter* pConverter;

	// On Off
	pControl = mpPane->Create2StateButton(GetUniqueID(), IDB_Button_OffOn, ge::SPos(48, 5), mpPane, true);
	GetWindow()->ControlAdded(pControl->GetID(), pControl);
	RegisterGlobalControl(0, giParamDelayOn, pControl);
	// Delay Time
	pControl = mpPane->CreateKnob(GetUniqueID(), IDB_Dial_1, ge::SPos(2, 46), 65);
	GetWindow()->ControlAdded(pControl->GetID(), pControl);
	RegisterGlobalControlWithLinearTranslation(0, giParamDelayTime, pControl);
	tint32 iID = GetUniqueID();
	pControl = mpPane->CreateDisplay(iID , ge::SPos(27, 50), ge::SSize(40, 14), mpPane->CreateFont(mpPane->Generic128, IDB_AxelTight, ge::SRGB(0, 255, 255)));
	GetWindow()->ControlAdded(pControl->GetID(), pControl);
	RegisterGlobalControlWithDisplayTranslater(0,giParamDelayTime, pControl,  1, 5,  1,  " ms"); 
	
	//giParamFeedback
	tint iY = 52;
	pControl = mpPane->CreateKnob(GetUniqueID(), IDB_Dial_1, ge::SPos(2, 46+ iY), 65);
	GetWindow()->ControlAdded(pControl->GetID(), pControl);
	RegisterGlobalControlWithLinearTranslation(0, giParamFeedback, pControl);
	iID = GetUniqueID();
	pControl = mpPane->CreateDisplay(iID , ge::SPos(27, 50+ iY), ge::SSize(40, 14), mpPane->CreateFont(mpPane->Generic128, IDB_AxelTight, ge::SRGB(0, 255, 255)));
	GetWindow()->ControlAdded(pControl->GetID(), pControl);
	RegisterGlobalControlWithDisplayTranslater(0,giParamFeedback, pControl,  1, 5,  1,  " %"); 
	
	//giParamHighFreqDamping
	iY += 51;
	pControl = mpPane->CreateKnob(GetUniqueID(), IDB_Dial_1, ge::SPos(2, 46+ iY), 65);
	GetWindow()->ControlAdded(pControl->GetID(), pControl);
	RegisterGlobalControlWithLinearTranslation(0, giParamHighFreqDamping, pControl);
	iID = GetUniqueID();
	pControl = mpPane->CreateDisplay(iID , ge::SPos(27, 50+ iY), ge::SSize(40, 14), mpPane->CreateFont(mpPane->Generic128, IDB_AxelTight, ge::SRGB(0, 255, 255)));
	GetWindow()->ControlAdded(pControl->GetID(), pControl);
	RegisterGlobalControlWithDisplayTranslater(0,giParamHighFreqDamping, pControl,  1, 5,  1,  " %"); 
	
	//giParamDamping
	iY += 52;
	pControl = mpPane->CreateKnob(GetUniqueID(), IDB_Dial_1, ge::SPos(2, 46+ iY), 65);
	GetWindow()->ControlAdded(pControl->GetID(), pControl);
	RegisterGlobalControlWithLinearTranslation(0, giParamDamping, pControl);
	iID = GetUniqueID();
	pControl = mpPane->CreateDisplay(iID , ge::SPos(27, 50+ iY), ge::SSize(40, 14), mpPane->CreateFont(mpPane->Generic128, IDB_AxelTight, ge::SRGB(0, 255, 255)));
	GetWindow()->ControlAdded(pControl->GetID(), pControl);
	RegisterGlobalControlWithDisplayTranslater(0,giParamDamping, pControl,  1, 5,  1,  " %"); 

	mbSyncToHost = false;

	mpParmMan->ControlUpdateAll();
	mpParmMan->ParameterUpdateAll();
}

void CEffectEcho::DeInitialize()
{
	delete[] mpfBuffer1;
	mpfBuffer1 = NULL;

	delete[] mpfBuffer2;
	mpfBuffer2 = NULL;

	// Remove controls
	mpParmMan->DeregisterAllControls();

	std::list<tuint32>::iterator it = mControlIDs.begin();
	for (; it != mControlIDs.end(); it++) {
		tuint32 uiID = *it;

		ge::IControl* pControl = mpPane->GetControl(uiID);
		mpPane->GetPane()->RemoveControl(pControl);
		pControl->Destroy();
	}
}

void CEffectEcho::Start()
{
	memset(mpfBuffer1, 0, EFFECT_ECHO_BUFFER_SIZE * sizeof(tfloat));
	memset(mpfBuffer2, 0, EFFECT_ECHO_BUFFER_SIZE * sizeof(tfloat));

	miPos = 0;

	mpfLPHistory[0] = mpfLPHistory[1] = 0;
	mpfHPHistory[0] = mpfHPHistory[1] = 0;
}

void CEffectEcho::ProcessInPlace(tfloat** ppfSamples, tuint32 iNrOfSamples)
{
	float* pfL = ppfSamples[0];
//	float* pfR = ppfSamples[1];

	tfloat* pfBuffer1 = mpfBuffer1;
//	tfloat* pfBuffer2 = mpfBuffer2;
	tint32 iPos = miPos;

	for (tuint uiSample = 0; uiSample < iNrOfSamples; uiSample++) {
		// Move writing position forward
		iPos++;
		// Wrap (if neccasary)
		iPos &= (EFFECT_ECHO_BUFFER_SIZE - 1);

		// Find reading tap
		tint32 iTap = iPos - miDelayTime;
		iTap &= (EFFECT_ECHO_BUFFER_SIZE - 1);

		// Read from buffer
		tfloat f1 = pfBuffer1[iTap];
//		tfloat f2 = pfBuffer2[iTap];

		// Apply giParamFeedback
		f1 *= mfFeedback;
//		f2 *= mfFeedback;

		// Apply LP
		f1 = mpfLPHistory[0] = gfAnti_denormal + mpfLPHistory[0] + (f1 - mpfLPHistory[0]) * mfDamping;
//		f2 = mpfLPHistory[1] = gfAnti_denormal + mpfLPHistory[1] + (f2 - mpfLPHistory[1]) * mfDamping;

		// Apply HP
		mpfHPHistory[0] = gfAnti_denormal + mpfHPHistory[0] + (f1 - mpfHPHistory[0]) * mfHFDamping;
		f1 = f1 - mpfHPHistory[0];
//		mpfHPHistory[1] = gfAnti_denormal + mpfHPHistory[1] + (f2 - mpfHPHistory[1]) * mfHFDamping;
//		f2 = f2 - mpfHPHistory[1];

		// Write to circular buffer
		pfBuffer1[iPos] = f1 + pfL[uiSample];
//		pfBuffer2[iPos] = f2 + pfR[uiSample];

		// Write output
		pfL[uiSample] = f1;
//		pfR[uiSample] = f2;
	}

	miPos = iPos;
}

tuint32 CEffectEcho::GetNrOfParameters()
{
	return NrOfParams;
}

void CEffectEcho::GetParameterInfo(tuint32 iIndex, kspi::SParameterInfo* pInfo)
{
	// Fill default info
	CEffect::GetParameterInfo(iIndex, pInfo);
}

void CEffectEcho::UpdateData(tint32 iID, tint32 iValue)
{
	switch(iID) {
		case giParamDelayTime:
			miInterFaceDelayTime = iValue * GetSampleRate() / 1000;
			SetDelayTime();
			break;
		case giParamFeedback:
			mfFeedback = iValue * 0.008f;
			break;
		case giParamDamping:
			if (iValue == 16001) {
				mfDamping = 1;
			}
			else {
				mfDamping = (float)iValue / (GetSampleRate() / 2);
			}
			break;
		case giParamHighFreqDamping:
			if (iValue == 31) {
				mfHFDamping = 0;
			}
			else {
				mfHFDamping = (float)iValue / (GetSampleRate() / 2);
			}
			break;
		case giParamSyncToHost:
			mbSyncToHost = iValue? true: false;
			SetDelayTime();
			break;
	}
}

void CEffectEcho::SetParameter(tuint32 iIndex, tint32 iValue)
{
	CEffect::SetParameter(iIndex, iValue);

	UpdateData(iIndex, iValue);
}

void CEffectEcho::SetDelayTime()
{
	 miDelayTime = mbSyncToHost ? miHostDelayTime: miInterFaceDelayTime;
}

void CEffectEcho::SetBPM(tfloat fBPM)
{
	CEffect::SetBPM(fBPM);

	miHostDelayTime = (tint32)((60.0f / mfBPM) * GetSampleRate());
	SetDelayTime();

}

void CEffectEcho::GUINotify(tint32 iPaneID, tint32 iControlID, tint32 iValue)
{
	mpPane->GetPane()->SetControlValue(iControlID, iValue, true);
}

void CEffectEcho::AutomationNotify(de::IParameterManager::EParameterType Type, tint32 iExtra, tint32 iParameterID, tint32 iValue)
{
}








