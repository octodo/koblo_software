
#include "FilterOS.h"


// TODO: Update company name and product name (may contain spaces and other special chars)
#define COMPANY_NAME "Koblo"
#define PRODUCT_NAME "VFilter"


const tint giPolyphony = 4;
const tint giReserve = 4;
const tint giMaxSubBufferSize = 32;

CFilterPlugIn::CFilterPlugIn(CFilterModule* pModule, tuint32 uiProcessIndex)
: CBasePlugIn(dynamic_cast<CBaseModule*>(pModule), giAudioMaxBufferSize, dynamic_cast<CBaseDSPEngine*>(new CFilterDSP()), uiProcessIndex, COMPANY_NAME, PRODUCT_NAME),
	mbUpdateGUISettings(true),
	mbBypass(false)
{
	dynamic_cast<CFilterDSP*>(mpDSPEngine)->SetChannels(2);

	mpDezipper->SetCallback(dynamic_cast<IBaseDezipperCallback*>(this));
}

CFilterPlugIn::~CFilterPlugIn()
{
}

void CFilterPlugIn::Destroy()
{
	delete dynamic_cast<CFilterPlugIn*>(this);
}

kspi::IGUI* CFilterPlugIn::CreateGUI(tint32 /*iIndex*/)
{
	CBaseGUI* pBaseGUI = dynamic_cast<CBaseGUI*>(new CFilterGUI(this, mpParmMan));

	mGUIs.push_back(pBaseGUI);

	kspi::IGUI* pKSPIGUI = dynamic_cast<kspi::IGUI*>(pBaseGUI);

	return pKSPIGUI;
}

void CFilterPlugIn::GetChannelConfigurations(tuint32* puiNrOfConfigurations, tuint32* puiChannelsIn, tuint32* puiChannelsOut, tuint32* puiSideChainChannels)
{
	CAutoLock Lock(mMutex);

	*puiNrOfConfigurations = 1;

	*puiChannelsIn = 2;
	*puiChannelsOut = 2;
	*puiSideChainChannels = 0;
}

void CFilterPlugIn::Initialize()
{
	CBasePlugIn::Initialize();
}

void CFilterPlugIn::AddParameters()
{
	mpParmMan->SetGlobalSectionCount(1);

	// Bypass
	AddGlobalParm(0, giParmIDFilterBypass, 0, 1, 0);
	AddGlobalParm(0, giParam_Cutoff, 10, 16000, 6000);
	AddGlobalParm(0, giParam_Resonance, 5, 1000, 50);
	AddGlobalParm(0, giParam_Mix, 0, 100, 100);
	AddGlobalParm(0, giParam_LoPass,0,100,100);
	AddGlobalParm(0, giParam_BandPass, 0, 100,0);
	AddGlobalParm(0, giParam_HighPass, 0, 100, 0);
	

	// TODO: Create parameters by adding parm-IDs to parameter-manager below here.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_plug_in.html
	// NOTE: Remember to first create the parm-IDs in ParmIDs.h.
	// NOTE: You *must* add the parm-IDs in value order! Also don't skip any!


	// Add parameters to dezipper
	mpDezipper->AddParameter(giParam_Mix, 0, 100, 100);
	mpDezipper->AddParameter(giParam_LoPass, 0, 100, 100);
	mpDezipper->AddParameter(giParam_BandPass, 0, 100, 0);
	mpDezipper->AddParameter(giParam_HighPass, 0, 100, 0);
}

void CFilterPlugIn::MakeAutomationMap()
{
	// Bypass
	InsertParameterEnum(giParmIDFilterBypass, "Bypass", "OFF@ON");

	// TODO: Add more parameters for automation below here.
	// Use the CBasePlugIn::InsertParameterXxx...() methods.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_plug_in.html

}

void CFilterPlugIn::UpdateEngineData(tint32 iID, tint32 iValue)
{
	CAutoLock Lock(mMutex);

	if (UpdateEngineSettings()) {
			switch(iID) {
				case giParmIDFilterBypass:
					mpDSPEngine->SetBypass((iValue == 0) ? false : true);
					break;
				case giParam_Cutoff:
					dynamic_cast<CFilterDSP*>(mpDSPEngine)->Set_Cutoff(iValue);
					break;
				case giParam_Resonance:
					dynamic_cast<CFilterDSP*>(mpDSPEngine)->Set_Resonance(iValue );
					break;
				case giParam_Mix:
					mpDezipper->SetTarget(giParam_Mix, iValue);
				//	dynamic_cast<CFilterDSP*>(mpDSPEngine)->Set_Mix(iValue );
					break;
				case giParam_LoPass:
					mpDezipper->SetTarget(giParam_LoPass, iValue);
				//	dynamic_cast<CFilterDSP*>(mpDSPEngine)->Set_LoPass(iValue );
					break;
				case giParam_BandPass:
					mpDezipper->SetTarget(giParam_BandPass, iValue);
				//	dynamic_cast<CFilterDSP*>(mpDSPEngine)->Set_BandPass(iValue);
					break;
				case giParam_HighPass:
					mpDezipper->SetTarget(giParam_HighPass, iValue);
				//	dynamic_cast<CFilterDSP*>(mpDSPEngine)->Set_HighPass(iValue );
					break;


			}
		}

	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CFilterGUI*>(pGUI)->UpdateEngineData(0, 0, iID, iValue);
	}
}

void CFilterPlugIn::SetValue(tint32 iParmID, tint32 iValue)
{
	if (UpdateEngineSettings()) {
		switch(iParmID) {
			
			case giParam_Mix:
				//mpDezipper->SetTarget(giParam_LoPass, iValue);
				dynamic_cast<CFilterDSP*>(mpDSPEngine)->Set_Mix(iValue );
				break;
			case giParam_LoPass:
				//mpDezipper->SetTarget(giParam_LoPass, iValue);
				dynamic_cast<CFilterDSP*>(mpDSPEngine)->Set_LoPass(iValue );
				break;
			case giParam_BandPass:
				//mpDezipper->SetTarget(giParam_BandPass, iValue);
					dynamic_cast<CFilterDSP*>(mpDSPEngine)->Set_BandPass(iValue);
				break;
			case giParam_HighPass:
				//mpDezipper->SetTarget(giParam_HighPass, iValue);
					dynamic_cast<CFilterDSP*>(mpDSPEngine)->Set_HighPass(iValue );
				break;
				
				
		}
	}
}

void CFilterPlugIn::UpdateAllGUI()
{
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CFilterGUI*>(pGUI)->UpdateAll();
	}
}

tint32 CFilterPlugIn::GetProductID()
{
	return 29;
}

void CFilterPlugIn::GetProductName(tchar* pszName)
{
	strcpy(pszName, "VFilter");
}


