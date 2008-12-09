
#include "FlangerOS.h"


// TODO: Update company name and product name (may contain spaces and other special chars)
#define COMPANY_NAME "Koblo"
#define PRODUCT_NAME "Flanger"


const tint giPolyphony = 4;
const tint giReserve = 4;
const tint giMaxSubBufferSize = 32;

CFlangerPlugIn::CFlangerPlugIn(CFlangerModule* pModule, tuint32 uiProcessIndex)
: CBasePlugIn(dynamic_cast<CBaseModule*>(pModule), giAudioMaxBufferSize, dynamic_cast<CBaseDSPEngine*>(new CFlangerDSP()), uiProcessIndex, COMPANY_NAME, PRODUCT_NAME),
	mbUpdateGUISettings(true),
	mbBypass(false)
{
	dynamic_cast<CFlangerDSP*>(mpDSPEngine)->SetChannels(2);

	mpDezipper->SetCallback(dynamic_cast<IBaseDezipperCallback*>(this));
}

CFlangerPlugIn::~CFlangerPlugIn()
{
}

void CFlangerPlugIn::Destroy()
{
	delete dynamic_cast<CFlangerPlugIn*>(this);
}

kspi::IGUI* CFlangerPlugIn::CreateGUI(tint32 /*iIndex*/)
{
	CBaseGUI* pBaseGUI = dynamic_cast<CBaseGUI*>(new CFlangerGUI(this, mpParmMan));

	mGUIs.push_back(pBaseGUI);

	kspi::IGUI* pKSPIGUI = dynamic_cast<kspi::IGUI*>(pBaseGUI);

	return pKSPIGUI;
}

void CFlangerPlugIn::GetChannelConfigurations(tuint32* puiNrOfConfigurations, tuint32* puiChannelsIn, tuint32* puiChannelsOut, tuint32* puiSideChainChannels)
{
	CAutoLock Lock(mMutex);

	*puiNrOfConfigurations = 1;

	*puiChannelsIn = 2;
	*puiChannelsOut = 2;
	*puiSideChainChannels = 0;
}

void CFlangerPlugIn::Initialize()
{
	CBasePlugIn::Initialize();
}

void CFlangerPlugIn::AddParameters()
{
	mpParmMan->SetGlobalSectionCount(1);

	// Bypass
	AddGlobalParm(0, giParmIDFlangerBypass, 0, 1, 0);
	AddGlobalParm(0, giParam_Time, 10, 1000, 100);
	AddGlobalParm(0, giParam_Depth, 0, 100, 60);
	AddGlobalParm(0, giParam_Rate, 1, 1000, 1);
	AddGlobalParm(0, giParam_Feedback, 0, 100, 80);
	AddGlobalParm(0, giParam_Mix, 0, 128, 64);
	

	// Add parameters to dezipper
//	mpDezipper->AddParameter(giParmIDFlangerVolume, 0, 3162, 1000);
}

void CFlangerPlugIn::MakeAutomationMap()
{
	// Bypass
	InsertParameterEnum(giParmIDFlangerBypass, "Bypass", "OFF@ON");

	// TODO: Add more parameters for automation below here.
	// Use the CBasePlugIn::InsertParameterXxx...() methods.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_plug_in.html

}

void CFlangerPlugIn::UpdateEngineData(tint32 iID, tint32 iValue)
{
	CAutoLock Lock(mMutex);

	if (UpdateEngineSettings()) {
		switch(iID) {
			case giParmIDFlangerBypass:
				mpDSPEngine->SetBypass((iValue == 0) ? false : true);
				break;

			case giParam_Time:
				dynamic_cast<CFlangerDSP*>(mpDSPEngine)->Set_Time(iValue);
				break;
			case giParam_Depth:
				dynamic_cast<CFlangerDSP*>(mpDSPEngine)->Set_Depth(iValue );
				break;
			case giParam_Rate:
				dynamic_cast<CFlangerDSP*>(mpDSPEngine)->Set_Rate(iValue );
				break;
			case giParam_Feedback:
				dynamic_cast<CFlangerDSP*>(mpDSPEngine)->Set_Feedback(iValue );
				break;
			case giParam_Mix:
				dynamic_cast<CFlangerDSP*>(mpDSPEngine)->Set_Mix(iValue );
				break;
				
				
				
				
		}
	}

	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CFlangerGUI*>(pGUI)->UpdateEngineData(0, 0, iID, iValue);
	}
}

void CFlangerPlugIn::SetValue(tint32 iParmID, tint32 iValue)
{
	if (UpdateEngineSettings()) {
//		switch(iParmID) {
//		}
	}
}

void CFlangerPlugIn::UpdateAllGUI()
{
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CFlangerGUI*>(pGUI)->UpdateAll();
	}
}

tint32 CFlangerPlugIn::GetProductID()
{
	return 24;//112;
}

void CFlangerPlugIn::GetProductName(tchar* pszName)
{
	strcpy(pszName, "Flanger");
}


