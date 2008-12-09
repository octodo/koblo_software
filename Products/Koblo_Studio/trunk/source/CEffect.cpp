/*!	\file CEffect.cpp
	\author Michael Olsen
	\date 19/Apr/2005
	\date 19/Apr/2005
*/

#include "OMPSOS.h"


CAutoDelete<IUniqueID> CEffect::gpControlIDs(IUniqueID::Create(giControlIDFX));


CEffect::CEffect()
	: miMaxBufferSize(0), muiSampleRate(0), mpPane(NULL)
{
	mpParmMan = de::IParameterManager::Create();
	mpParmMan->SetCallback(dynamic_cast<de::IParameterManagerCallback*>(this));
}

CEffect::~CEffect()
{
	mpParmMan->Destroy();

	std::list<tuint32>::iterator it = mControlIDs.begin();
	for (; it != mControlIDs.end(); it++) {
		tuint32 uiID = *it;
		gpControlIDs->ReleaseID(uiID);
	}
}

kspi::IGUI* CEffect::CreateGUI(tint32 iIndex)
{
	return NULL;
}

void CEffect::PreInitialize()
{
}

void CEffect::Initialize()
{
}

void CEffect::Initialize(ge::IPane* pPane)
{
}

void CEffect::DeInitialize()
{
}

void CEffect::Start()
{
}

void CEffect::Stop()
{
}

tint32 CEffect::IsSignalGenerator()
{
	return 0;
}

tint32 CEffect::CanProcessInPlace()
{
	return 1;
}

tint32 CEffect::WantsMIDI()
{
	return 0;
}

tint32 CEffect::IsSynth()
{
	return 0;
}

tint32 CEffect::GetLatency()
{
	return 0;
}

//tuint32 CEffect::GetInputChannelCount()
//{
//	return 2;
//}

tuint32 CEffect::GetOutputChannelCount()
{
	return 1;
}

void CEffect::SetMaxBufferSize(tuint32 iBufferSize)
{
	miMaxBufferSize = iBufferSize;
}

tint32 CEffect::SupportSampleRate(tuint32 iSampleRate)
{
	return 1;
}

void CEffect::SetSampleRate(tuint32 iSampleRate)
{
	muiSampleRate = iSampleRate;
}

void CEffect::SetBPM(tfloat fBPM)
{
	mfBPM = fBPM;
}

void CEffect::InputMIDI(const kspi::SMIDIEvent* pEvents, tuint32 iNrOfEvents)
{
}

void CEffect::ProcessNonInPlace(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
//	tint iBuffersToCopy = GetInputChannelCount();
//	if (GetOutputChannelCount() < iBuffersToCopy) {
//		iBuffersToCopy = GetOutputChannelCount();
//	}
	tint iBuffersToCopy = 1;

	tint iBuffer;
	// Copy input to output
	for (iBuffer = 0; iBuffer < iBuffersToCopy; iBuffer++) {
		memcpy(ppfSamplesOut[iBuffer], ppfSamplesIn[iBuffer], iNrOfSamples * sizeof(float));
	}
	// Set remaining outputs to 0
	for (iBuffer = iBuffersToCopy; iBuffer < GetOutputChannelCount(); iBuffer++) {
		memset(ppfSamplesOut[iBuffer], 0, iNrOfSamples * sizeof(float));
	}

	ProcessInPlace(ppfSamplesOut, iNrOfSamples);
}

tuint32 CEffect::GetNrOfParameters()
{
	return guiEffectParameters;
}

void CEffect::GetParameterInfo(tuint32 iIndex, kspi::SParameterInfo* pInfo)
{
	pInfo->pszName[0] = 0;
	pInfo->pszUnitName[0] = 0;

	pInfo->Type = kspi::SParameterInfo::TypeLong;
	pInfo->iMinValue = 0;
	pInfo->iMaxValue = 127;
	pInfo->iDefaultValue = 64;
	pInfo->iDecimals = 0;

	pInfo->pszMinText[0] = 0;
	pInfo->pszMaxText[0] = 0;

	pInfo->CurveType = kspi::SParameterInfo::CurveTypeLinear;
	pInfo->pCurveData = NULL;

	pInfo->pszEnumText[0] = 0;
}

tint32 CEffect::GetParameter(tuint32 iIndex)
{
	return mpiParmValue[iIndex];
}

void CEffect::SetParameter(tuint32 iIndex, tint32 iValue)
{
	mpiParmValue[iIndex] = iValue;
}

tuint32 CEffect::GetChunkSize()
{
	return 0;
}

void CEffect::GetChunk(void* /*p*/)
{
}

void CEffect::SetChunk(const void* /*p*/, tuint32 /*iChunkSize*/)
{
}

void CEffect::Timer()
{
}

/*void CEffect::SetPaneAndGUI(ge::IPane* pPane, CBaseGUI* pGUI)
{
	mpPane = pPane;

	mpGUI = pGUI;
}*/

void CEffect::SetPane(CBasePane* pPane)
{
	mpPane = pPane;
}

void CEffect::EventValueChange(ge::IControl* pSender, tint32 iValueNew)
{
	// Notify the parm manager that a control has changed
	mpParmMan->ControlUpdate(mpPane->GetPaneID(), pSender->GetID(), iValueNew);
}

tuint32 CEffect::GetUniqueID()
{
	tuint32 uiRet = gpControlIDs->GetID();

	mControlIDs.push_back(uiRet);

	return uiRet;
}

void CEffect::RegisterGlobalControl(tint iSection, tint32 iParameterID, ge::IControl* pControl) {
	mpParmMan->RegisterControl(mpPane->GetPaneID(), pControl->GetID(), iParameterID, de::IParameterManager::TypeGlobal, iSection);

	pControl->SetDefaultValueConverted(mpParmMan->Get(iParameterID, de::IParameterManager::TypeGlobal, iSection)->GetDefaultValue());
}

de::IDisplayConverter* CEffect::RegisterGlobalControlWithDisplayTranslater(tint iSection, tint32 iParameterID, ge::IControl* pControl, tint iDecimals, tint iMaxNrOfStringChars, tint iMaxNrOfStringDecimals, std::string sPostFix) {
	mpParmMan->RegisterControl(mpPane->GetPaneID(), pControl->GetID(), iParameterID, de::IParameterManager::TypeGlobal, iSection);

	de::IDisplayConverter* pConverter = de::IDisplayConverter::Create(iDecimals, iMaxNrOfStringChars, iMaxNrOfStringDecimals);
	if (sPostFix.size() != 0) {
		pConverter->SetPostFix((const tchar*)(sPostFix.c_str()));
	}

	pControl->SetDisplayConverter(pConverter);
	pControl->SetDefaultValueConverted(mpParmMan->Get(iParameterID, de::IParameterManager::TypeGlobal, iSection)->GetDefaultValue());
	return pConverter;
}

void CEffect::RegisterGlobalControlWithLinearTranslation(tint iSection, tint32 iParameterID, ge::IControl* pControl) {
	const de::IParameter* pParm = mpParmMan->Get(iParameterID, de::IParameterManager::TypeGlobal, iSection);
	tint32 iMin = pParm->GetMinValue();
	tint32 iMax = pParm->GetMaxValue();
	mpParmMan->RegisterControl(mpPane->GetPaneID(), pControl->GetID(), iParameterID, de::IParameterManager::TypeGlobal, iSection);

	pControl->SetConverter(de::IConverter::CreateLinear(iMin, iMax));
	pControl->SetDefaultValueConverted(pParm->GetDefaultValue());
}

void CEffect::RegisterGlobalControlWithLogTranslation(tint iSection, tint32 iParameterID, ge::IControl* pControl, tint iStrength) {
	const de::IParameter* pParm = mpParmMan->Get(iParameterID, de::IParameterManager::TypeGlobal, iSection);
	tint32 iMin = pParm->GetMinValue();
	tint32 iMax = pParm->GetMaxValue();
	mpParmMan->RegisterControl(mpPane->GetPaneID(), pControl->GetID(), iParameterID, de::IParameterManager::TypeGlobal, iSection);

	pControl->SetConverter(de::IConverter::CreateLog(iMin, iMax, iStrength));
	pControl->SetDefaultValueConverted(pParm->GetDefaultValue());
}

void CEffect::RegisterGlobalControlWithTrueLogTranslation(tint iSection, tint32 iParameterID, ge::IControl* pControl) {
	const de::IParameter* pParm = mpParmMan->Get(iParameterID, de::IParameterManager::TypeGlobal, iSection);
	tint32 iMin = pParm->GetMinValue();
	tint32 iMax = pParm->GetMaxValue();
	mpParmMan->RegisterControl(mpPane->GetPaneID(), pControl->GetID(), iParameterID, de::IParameterManager::TypeGlobal, iSection);

	pControl->SetConverter(de::IConverter::CreateTrueLog(iMin, iMax));
	pControl->SetDefaultValueConverted(pParm->GetDefaultValue());
}

void CEffect::RegisterGlobalControlWithEnumTranslation(tint iSection, tint32 iParameterID, ge::IControl* pControl) {
	const de::IParameter* pParm = mpParmMan->Get(iParameterID, de::IParameterManager::TypeGlobal, iSection);
	ASSERT(pParm->GetMinValue() == 0);
	tint32 iMax = pParm->GetMaxValue();
	mpParmMan->RegisterControl(mpPane->GetPaneID(), pControl->GetID(), iParameterID, de::IParameterManager::TypeGlobal, iSection);

	pControl->SetConverter(de::IConverter::CreateEnum(iMax));
	pControl->SetDefaultValueConverted(pParm->GetDefaultValue());
}

void CEffect::RegisterGlobalControlWithX3Translation(tint iSection, tint32 iControlID, tint32 iParameterID, ge::IControl* pControl, tuint uiParmDecimals) {
	const de::IParameter* pParm = mpParmMan->Get(iParameterID, de::IParameterManager::TypeGlobal, iSection);
	tint32 iMin = pParm->GetMinValue();
	tint32 iMax = pParm->GetMaxValue();
	mpParmMan->RegisterControl(mpPane->GetPaneID(), pControl->GetID(), iParameterID, de::IParameterManager::TypeGlobal, iSection);

	pControl->SetConverter(de::IConverter::CreateX3(iMin, iMax, uiParmDecimals));
	pControl->SetDefaultValueConverted(pParm->GetDefaultValue());
}



CParmEffect::CParmEffect(CEffect* pEffect, tint32 iID, tint32 iMinValue, tint32 iMaxValue, tint32 iValue)
	: mpEffect(pEffect), miID(iID), miMinValue(iMinValue), miMaxValue(iMaxValue), miDefaultValue(iValue), miValue(iValue)
{
	// Force parameter within min/max range by calling SetValue(...)
	SetValue(miValue, false);
}

CParmEffect::~CParmEffect()
{
}

void CParmEffect::Destroy()
{
	delete dynamic_cast<CParmEffect*>(this);
}

tint32 CParmEffect::GetID() const
{
	return miID;
}

tint32 CParmEffect::GetDefaultValue() const
{
	return miDefaultValue;
}

tint32 CParmEffect::GetValue() const
{
	return miValue;
}

tint32 CParmEffect::GetMinValue() const
{
	return miMinValue;
}

tint32 CParmEffect::GetMaxValue() const
{
	return miMaxValue;
}

void CParmEffect::SetValue(tint32 iValueNew, tbool bCallOnChange)
{
	if (iValueNew < miMinValue) {
		iValueNew = miMinValue;
	}
	if (iValueNew > miMaxValue) {
		iValueNew = miMaxValue;
	}

	miValue = iValueNew;

	if (bCallOnChange) {
		OnChange();
	}
}

void CParmEffect::OnChange()
{
	mpEffect->UpdateData(miID, miValue);
}





