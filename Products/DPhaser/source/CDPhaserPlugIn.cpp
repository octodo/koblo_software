
#include "DPhaserOS.h"


// TODO: Update company name and product name (may contain spaces and other special chars)
#define COMPANY_NAME "Koblo"
#define PRODUCT_NAME "DPhaser"


const tint giPolyphony = 4;
const tint giReserve = 4;
const tint giMaxSubBufferSize = 32;

CDPhaserPlugIn::CDPhaserPlugIn(CDPhaserModule* pModule, tuint32 uiProcessIndex)
: CBasePlugIn(dynamic_cast<CBaseModule*>(pModule), giAudioMaxBufferSize, dynamic_cast<CBaseDSPEngine*>(new CDPhaserDSP()), uiProcessIndex, COMPANY_NAME, PRODUCT_NAME),
	mbUpdateGUISettings(true),
	mbBypass(false)
{
	dynamic_cast<CDPhaserDSP*>(mpDSPEngine)->SetChannels(2);

	mpDezipper->SetCallback(dynamic_cast<IBaseDezipperCallback*>(this));
}

CDPhaserPlugIn::~CDPhaserPlugIn()
{
}

void CDPhaserPlugIn::Destroy()
{
	delete dynamic_cast<CDPhaserPlugIn*>(this);
}

kspi::IGUI* CDPhaserPlugIn::CreateGUI(tint32 /*iIndex*/)
{
	CBaseGUI* pBaseGUI = dynamic_cast<CBaseGUI*>(new CDPhaserGUI(this, mpParmMan));

	mGUIs.push_back(pBaseGUI);

	kspi::IGUI* pKSPIGUI = dynamic_cast<kspi::IGUI*>(pBaseGUI);

	return pKSPIGUI;
}

void CDPhaserPlugIn::GetChannelConfigurations(tuint32* puiNrOfConfigurations, tuint32* puiChannelsIn, tuint32* puiChannelsOut, tuint32* puiSideChainChannels)
{
	CAutoLock Lock(mMutex);

	*puiNrOfConfigurations = 1;

	*puiChannelsIn = 2;
	*puiChannelsOut = 2;
	*puiSideChainChannels = 0;
}

void CDPhaserPlugIn::Initialize()
{
	CBasePlugIn::Initialize();
}

void CDPhaserPlugIn::AddParameters()
{
	mpParmMan->SetGlobalSectionCount(1);

	// Bypass
	AddGlobalParm(0, giParmIDDPhaserBypass, 0, 1, 0);
	AddGlobalParm(0, giParamID_Freq, 50, 1000, 500);
	AddGlobalParm(0, giParamID_Range, 0, 100, 40);
	AddGlobalParm(0, giParamID_Rate, 1, 1000, 1);
	AddGlobalParm(0, giParamID_Feedback, 0, 100, 80);
	AddGlobalParm(0, giParamID_Mix, 0, 128, 128);

	// Add parameters to dezipper
//	mpDezipper->AddParameter(giParmIDDPhaserVolume, 0, 3162, 1000);
}

void CDPhaserPlugIn::MakeAutomationMap()
{
	// Bypass
	InsertParameterEnum(giParmIDDPhaserBypass, "Bypass", "OFF@ON");

	// TODO: Add more parameters for automation below here.
	// Use the CBasePlugIn::InsertParameterXxx...() methods.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_plug_in.html

}

void CDPhaserPlugIn::UpdateEngineData(tint32 iID, tint32 iValue)
{
	CAutoLock Lock(mMutex);

	if (UpdateEngineSettings()) {
		switch(iID) {
			case giParmIDDPhaserBypass:
				mpDSPEngine->SetBypass((iValue == 0) ? false : true);
				break;
			case giParamID_Freq:
				dynamic_cast<CDPhaserDSP*>(mpDSPEngine)->SetFreq(iValue );
				break;
			case giParamID_Range:
				dynamic_cast<CDPhaserDSP*>(mpDSPEngine)->SetRange(iValue );
				break;
			case giParamID_Rate:
				dynamic_cast<CDPhaserDSP*>(mpDSPEngine)->SetRate(iValue );
				break;
			case giParamID_Feedback:
				dynamic_cast<CDPhaserDSP*>(mpDSPEngine)->SetFeedback(iValue );
				break;
			case giParamID_Mix:
				dynamic_cast<CDPhaserDSP*>(mpDSPEngine)->SetMix(iValue );
				break;
		}
	}

	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CDPhaserGUI*>(pGUI)->UpdateEngineData(0, 0, iID, iValue);
	}
}

void CDPhaserPlugIn::SetValue(tint32 iParmID, tint32 iValue)
{
	if (UpdateEngineSettings()) {
//		switch(iParmID) {
//		}
	}
}

void CDPhaserPlugIn::UpdateAllGUI()
{
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CDPhaserGUI*>(pGUI)->UpdateAll();
	}
}

tint32 CDPhaserPlugIn::GetProductID()
{
	return 23;//111;
}

void CDPhaserPlugIn::GetProductName(tchar* pszName)
{
	strcpy(pszName, "DPhaser");
}


