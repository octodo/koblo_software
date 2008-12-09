
#include "DigitalEchoOS.h"


// TODO: Update company name and product name (may contain spaces and other special chars)
#define COMPANY_NAME "Koblo"
#define PRODUCT_NAME "DigitalEcho"


const tint giPolyphony = 4;
const tint giReserve = 4;
const tint giMaxSubBufferSize = 32;

CDigitalEchoPlugIn::CDigitalEchoPlugIn(CDigitalEchoModule* pModule, tuint32 uiProcessIndex)
: CBasePlugIn(dynamic_cast<CBaseModule*>(pModule), giAudioMaxBufferSize, dynamic_cast<CBaseDSPEngine*>(new CDigitalEchoDSP()), uiProcessIndex, COMPANY_NAME, PRODUCT_NAME),
	mbUpdateGUISettings(true),
	mbBypass(false)
{
	dynamic_cast<CDigitalEchoDSP*>(mpDSPEngine)->SetChannels(2);

	mpDezipper->SetCallback(dynamic_cast<IBaseDezipperCallback*>(this));
}

CDigitalEchoPlugIn::~CDigitalEchoPlugIn()
{
}

void CDigitalEchoPlugIn::Destroy()
{
	delete dynamic_cast<CDigitalEchoPlugIn*>(this);
}

kspi::IGUI* CDigitalEchoPlugIn::CreateGUI(tint32 /*iIndex*/)
{
	CBaseGUI* pBaseGUI = dynamic_cast<CBaseGUI*>(new CDigitalEchoGUI(this, mpParmMan));

	mGUIs.push_back(pBaseGUI);

	kspi::IGUI* pKSPIGUI = dynamic_cast<kspi::IGUI*>(pBaseGUI);

	return pKSPIGUI;
}

void CDigitalEchoPlugIn::GetChannelConfigurations(tuint32* puiNrOfConfigurations, tuint32* puiChannelsIn, tuint32* puiChannelsOut, tuint32* puiSideChainChannels)
{
	CAutoLock Lock(mMutex);

	*puiNrOfConfigurations = 1;

	*puiChannelsIn = 2;
	*puiChannelsOut = 2;
	*puiSideChainChannels = 0;
}

void CDigitalEchoPlugIn::Initialize()
{
	CBasePlugIn::Initialize();
}

void CDigitalEchoPlugIn::AddParameters()
{
	mpParmMan->SetGlobalSectionCount(1);

	// Bypass
	AddGlobalParm(0, giParmIDDigitalEchoBypass, 0, 1, 0);
	AddGlobalParm(0, giParam_Delay_Time, 1, 1000, 500);
	AddGlobalParm(0, giParam_Feedback, 0, 100, 80);
	AddGlobalParm(0, giParam_HP_Damping, 31, 16000, 31);
	AddGlobalParm(0, giParam_LP_Damping, 32, 12001, 12000);
	AddGlobalParm(0, giParam_Sync, 0, 1, 0);
	AddGlobalParm(0, giParam_Mix, 0, 128, 48);
	

	// TODO: Create parameters by adding parm-IDs to parameter-manager below here.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_plug_in.html
	// NOTE: Remember to first create the parm-IDs in ParmIDs.h.
	// NOTE: You *must* add the parm-IDs in value order! Also don't skip any!


	// Add parameters to dezipper
//	mpDezipper->AddParameter(giParmIDDigitalEchoVolume, 0, 3162, 1000);
}

void CDigitalEchoPlugIn::MakeAutomationMap()
{
	// Bypass
	InsertParameterEnum(giParmIDDigitalEchoBypass, "Bypass", "OFF@ON");

	// TODO: Add more parameters for automation below here.
	// Use the CBasePlugIn::InsertParameterXxx...() methods.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_plug_in.html

}

void CDigitalEchoPlugIn::UpdateEngineData(tint32 iID, tint32 iValue)
{
	CAutoLock Lock(mMutex);

	if (UpdateEngineSettings()) {
			switch(iID) {
				case giParmIDDigitalEchoBypass:
					mpDSPEngine->SetBypass((iValue == 0) ? false : true);
					break;
				case giParam_Delay_Time:
					dynamic_cast<CDigitalEchoDSP*>(mpDSPEngine)->Set_Delay_Time(iValue);
					break;
				case giParam_Feedback:
					dynamic_cast<CDigitalEchoDSP*>(mpDSPEngine)->Set_Feedback(iValue );
					break;
				case giParam_HP_Damping:
					dynamic_cast<CDigitalEchoDSP*>(mpDSPEngine)->Set_HF_Damping(iValue );
					break;
				case giParam_LP_Damping:
					dynamic_cast<CDigitalEchoDSP*>(mpDSPEngine)->Set_LF_Damping(iValue );
					break;
				case giParam_Sync:
					dynamic_cast<CDigitalEchoDSP*>(mpDSPEngine)->Set_Sync_To_Tempo((iValue == 0) ? false : true);
					break;
				case giParam_Mix:
					dynamic_cast<CDigitalEchoDSP*>(mpDSPEngine)->Set_Mix(iValue );
					break;


			}
		}

	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CDigitalEchoGUI*>(pGUI)->UpdateEngineData(0, 0, iID, iValue);
	}
}

void CDigitalEchoPlugIn::SetValue(tint32 iParmID, tint32 iValue)
{
	if (UpdateEngineSettings()) {
//		switch(iParmID) {
//		}
	}
}

void CDigitalEchoPlugIn::UpdateAllGUI()
{
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CDigitalEchoGUI*>(pGUI)->UpdateAll();
	}
}

tint32 CDigitalEchoPlugIn::GetProductID()
{
	return 22;
}

void CDigitalEchoPlugIn::GetProductName(tchar* pszName)
{
	strcpy(pszName, "DEcho");
}


