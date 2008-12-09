
#include "VST2KSPIOS.h"

#define COMPANY_NAME "Koblo"
#define PRODUCT_NAME "VST2KSPI"

VstIntPtr VSTCALLBACK audioMasterCallbackPlugIn(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
	switch(opcode) {
		case audioMasterAutomate:
			return 1;

		case audioMasterVersion:
			return 2400;

		case audioMasterCurrentId:
			return 0;

		case audioMasterIdle:
			return 0;
	}

	return 0;
}

CVST2KSPIPlugIn::CVST2KSPIPlugIn(CVST2KSPIModule* pModule, const std::string& sEffectPathName)
	: CBasePlugIn(dynamic_cast<CBaseModule*>(pModule), giAudioMaxBufferSize, dynamic_cast<CBaseDSPEngine*>(new CEcho()), 0, COMPANY_NAME, PRODUCT_NAME),
	mbUpdateGUISettings(true),
	mbBypass(false), mbShowDemoExpiredDialog(false),
	mbRegistered(false),
	miNoiseMode(0), miNoiseCounter(0),
	msEffectPathName(sEffectPathName),
	mpVSTEffect(NULL)
{
	dynamic_cast<CEcho*>(mpDSPEngine)->SetChannels(2);

	mpDezipper->SetCallback(dynamic_cast<IBaseDezipperCallback*>(this));

	mbCanProcess = true;

	muiTimeWhenStarted = ITime::GetTimeMS();

	miRandom = 0x123456;

	{
		CFBundleRef BundleRef;

		// Make a CFURLRef from the CFString representation of the bundle's path.
		tchar pszPathName[1024];
		strcpy(pszPathName, msEffectPathName.c_str());
		ToUnix(pszPathName);
		CFURLRef bundleURL = CFURLCreateWithFileSystemPath( 
			kCFAllocatorDefault, 
			__CFStringMakeConstantString(pszPathName),
			kCFURLPOSIXPathStyle,
			true );
		if ( !bundleURL ) {
		}

		// Make a bundle instance using the URLRef.
		BundleRef = CFBundleCreate( kCFAllocatorDefault, bundleURL );

		// We don't need the URL anymore
		CFRelease( bundleURL );	

		if ( !BundleRef ) {
		}

		if ( !CFBundleLoadExecutable( BundleRef ) )
		{
			CFRelease( BundleRef );
			BundleRef = NULL;
		}

		VSTMainFunc mainFunc = (VSTMainFunc)CFBundleGetFunctionPointerForName( 
					BundleRef, CFSTR("VSTPluginMain") );
		if (mainFunc == NULL) {
		}

		try {
			mpVSTEffect = mainFunc(audioMasterCallbackPlugIn);
			mpVSTEffect->dispatcher(mpVSTEffect, effOpen, 0, 0, 0, 0.0);
			mpVSTEffect->dispatcher(mpVSTEffect, effSetSampleRate, 0, 0, 0, 44100.0f);
			mpVSTEffect->dispatcher(mpVSTEffect, effSetBlockSize, 0, 32, 0, 0.0);
		}
		catch(...) {
			// Plug-in crashed
			mpVSTEffect = NULL;
		}
	}
}

CVST2KSPIPlugIn::~CVST2KSPIPlugIn()
{
	if (mpVSTEffect) {
		// Unload it
		mpVSTEffect->dispatcher(mpVSTEffect, effClose, 0, 0, 0, 0.0);
	}
}

void CVST2KSPIPlugIn::Destroy()
{
	delete dynamic_cast<CVST2KSPIPlugIn*>(this);
}

kspi::IGUI* CVST2KSPIPlugIn::CreateGUI(tint32 /*iIndex*/)
{
	CBaseGUI* pGUI = dynamic_cast<CBaseGUI*>(new CVST2KSPIGUI(this, mpParmMan));

	mGUIs.push_back(pGUI);

	return dynamic_cast<kspi::IGUI*>(pGUI);
}

tint32 CVST2KSPIPlugIn::GetProductID()
{
	return mpVSTEffect->uniqueID;
}

void CVST2KSPIPlugIn::GetProductName(tchar* pszName)
{
	strcpy(pszName, "n/a");
	try {
		mpVSTEffect->dispatcher(mpVSTEffect, effGetEffectName, 0, 0, pszName, 0.0);
	}
	catch (...) {
	}
}

//tuint32 CVST2KSPIPlugIn::GetInputChannelCount()
//{
//	return 2;
//}

//tuint32 CVST2KSPIPlugIn::GetOutputChannelCount()
//{
//	CAutoLock Lock(mMutex);

//	return 2;
//}

void CVST2KSPIPlugIn::GetChannelConfigurations(tuint32* puiNrOfConfigurations, tuint32* puiChannelsIn, tuint32* puiChannelsOut, tuint32* puiSideChainChannels)
{
	*puiNrOfConfigurations = 1;

	puiChannelsIn[0] = 2;
	puiChannelsOut[0] = 2;
	puiSideChainChannels[0] = 0;
}

void CVST2KSPIPlugIn::Initialize()
{
	std::string sPrefFolder = GetPreferenceFolder();

	CBasePlugIn::Initialize();
}

void CVST2KSPIPlugIn::SetMaxBufferSize(tuint32 iBufferSize)
{
	mpVSTEffect->dispatcher(mpVSTEffect, effSetBlockSize, 0, iBufferSize, 0, 0.0);
}

tint32 CVST2KSPIPlugIn::SupportSampleRate(tuint32 iSampleRate)
{
	return 1;
}

void CVST2KSPIPlugIn::SetSampleRate(tuint32 iSampleRate)
{
	mpVSTEffect->dispatcher(mpVSTEffect, effSetSampleRate, 0, 0, 0, (tfloat32)iSampleRate);
}

void CVST2KSPIPlugIn::AddParameters()
{
	mpParmMan->SetGlobalSectionCount(1);
}

const tint32 giStrength100 = 100;

#define AUTOMATION_PARM_LINEAR(parmid, name, min, max, def, decimals) \
	mAutomationMapGlobal.insert(std::pair<tint32, tint32>(parmid, mAutomationInfo.size())); \
	Info.iExtra = 0; \
	Info.iIndex = parmid; \
	Info.Type = de::IParameterManager::TypeGlobal; \
	Info.Info = InfoClean; \
	strcpy((char*)(Info.Info.pszName), name); \
	Info.Info.Type = kspi::SParameterInfo::TypeLong; \
	Info.Info.iMinValue = min; \
	Info.Info.iMaxValue = max; \
	Info.Info.iDefaultValue = def; \
	Info.Info.iDecimals = decimals; \
	Info.Info.CurveType = kspi::SParameterInfo::CurveTypeLinear;

#define AUTOMATION_PARM_LOG100(parmid, name, min, max, def, decimals) \
	mAutomationMapGlobal.insert(std::pair<tint32, tint32>(parmid, mAutomationInfo.size())); \
	Info.iExtra = 0; \
	Info.iIndex = parmid; \
	Info.Type = de::IParameterManager::TypeGlobal; \
	Info.Info = InfoClean; \
	strcpy((char*)(Info.Info.pszName), name); \
	Info.Info.Type = kspi::SParameterInfo::TypeLong; \
	Info.Info.iMinValue = min; \
	Info.Info.iMaxValue = max; \
	Info.Info.iDefaultValue = def; \
	Info.Info.iDecimals = decimals; \
	Info.Info.CurveType = kspi::SParameterInfo::CurveTypeLog; \
	Info.Info.pCurveData = (void*)(&giStrength100);

#define AUTOMATION_INSERT mAutomationInfo.insert(mAutomationInfo.end(), Info)


void CVST2KSPIPlugIn::MakeAutomationMap()
{
/*	// For each automateable parameter make a SAutomationInfo and add it to the AutomationInfo list
	SAutomationInfo Info;
	// Keep an empty info, which we can copy from
	kspi::SParameterInfo InfoClean;

	// Bypass
	mAutomationMapGlobal.insert(std::pair<tint32, tint32>(giParmIDBypass, mAutomationInfo.size()));
	Info.iExtra = 0;
	Info.iIndex = giParmIDBypass;
	Info.Type = de::IParameterManager::TypeGlobal;

	Info.Info = InfoClean;
	strcpy((char*)(Info.Info.pszName), "Bypass");
	Info.Info.Type = kspi::SParameterInfo::TypeEnum;
	Info.Info.iMinValue = 0;
	Info.Info.iMaxValue = 1;
	Info.Info.iDefaultValue = 0;
	Info.Info.CurveType = kspi::SParameterInfo::CurveTypeLinear;
	strcpy((char*)(Info.Info.pszEnumText), "Off@On");

	mAutomationInfo.insert(mAutomationInfo.end(), Info);*/
}

void CVST2KSPIPlugIn::GUISettingsChanged(tint32 iID, tint32 iValue)
{
	CAutoLock Lock(mMutex);

	if (mbUpdateGUISettings) {
//		if (mpGUI) {
//			mpGUI->SettingsChanged(iID, iValue);

//			mpGUI->UpdateEngineData(giGlobal, giSectionGUI, iID, iValue);
//		}
	}
}

void CVST2KSPIPlugIn::UpdateEngineData(tint32 iID, tint32 iValue)
{
	CAutoLock Lock(mMutex);

	if (UpdateEngineSettings()) {
	}

//	if (mpGUI) {
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CVST2KSPIGUI*>(pGUI)->UpdateEngineData(0, 0, iID, iValue);
	}
}

void CVST2KSPIPlugIn::SetValue(tint32 iParmID, tint32 iValue)
{
	if (UpdateEngineSettings()) {
		switch(iParmID) {
		}
	}
}

void CVST2KSPIPlugIn::UpdateAllGUI()
{
//	if (mpGUI) {
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CVST2KSPIGUI*>(pGUI)->UpdateAll();
	}
}

void CVST2KSPIPlugIn::ProcessNonInPlace(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	if (mbCanProcess) {
#ifdef _DEMO_
			tuint32 uiTimeNow = ITime::GetTimeMS();

			const tuint32 uiTimeForDemoMinutes = 15;
			const tuint32 uiTimeForDemoSeconds = uiTimeForDemoMinutes * 60;
			const tuint32 uiTimeForDemoMS = uiTimeForDemoSeconds * 1000;
			if (muiTimeWhenStarted + uiTimeForDemoMS <= uiTimeNow) {
				// Demo time expired
				mbCanProcess = false;
				mbShowDemoExpiredDialog = true;
			}
#endif	// _DEMO_

		CBasePlugIn::ProcessNonInPlace(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);

		if (mbRegistered == false) {
			miNoiseCounterMax0 = (tint)GetSampleRate() * 10;
			miNoiseCounterMax1 = (tint)GetSampleRate() * 2;

			tfloat32* pfOut0 = ppfSamplesOut[0];
			tfloat32* pfOut1 = ppfSamplesOut[1];

			tint iSample;
			for (iSample = 0; iSample < (tint)iNrOfSamples; iSample++) {
				if (miNoiseMode == 1) {
					tuint32 uiNew;
					uiNew = ((miRandom >> 17) & 1)
						^ ((miRandom >> 4) & 1)
						^ ((miRandom >> 1) & 1)
						^ (miRandom & 1);
					miRandom = miRandom << 1;
					miRandom = miRandom & 0x7fffff;
					miRandom = miRandom | uiNew;

					if (miRandom == 0) {
						miRandom = 1;
					}

					tint32 iTmp = miRandom;
					iTmp = iTmp << 8;
					tfloat32 fRandom = (tfloat32)(iTmp / (tfloat64)0x7fffffff);
					fRandom = (fRandom - 0.5f) * 2;

					pfOut0[iSample] = (tfloat32)(pfOut0[iSample] * 0.7 + fRandom * 0.007);
					pfOut1[iSample] = (tfloat32)(pfOut1[iSample] * 0.7 + fRandom * 0.007);
				}

				miNoiseCounter++;
				tint iNoiseMax = ((miNoiseMode == 0) ? miNoiseCounterMax0 : miNoiseCounterMax1);
				if (miNoiseCounter >= iNoiseMax) {
					miNoiseCounter = 0;

					miNoiseMode = (miNoiseMode ? 0 : 1);
				}
			}
		}
	}
	else {
		tint iChannels = 2;
		for (tint iChannel = 0; iChannel < iChannels; iChannel++) {
			tfloat* pfOut = ppfSamplesOut[iChannel];
			memset(pfOut, 0, iNrOfSamples * sizeof(tfloat));
		}
	}
}





