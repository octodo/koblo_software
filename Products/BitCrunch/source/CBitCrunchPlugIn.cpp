
#include "BitCrunchOS.h"


// TODO: Update company name and product name (may contain spaces and other special chars)
#define COMPANY_NAME "Koblo"
#define PRODUCT_NAME "BitCrush"


const tint giPolyphony = 4;
const tint giReserve = 4;
const tint giMaxSubBufferSize = 32;

CBitCrunchPlugIn::CBitCrunchPlugIn(CBitCrunchModule* pModule, tuint32 uiProcessIndex)
: CBasePlugIn(dynamic_cast<CBaseModule*>(pModule), giAudioMaxBufferSize, dynamic_cast<CBaseDSPEngine*>(new CBitCrunchDSP()), uiProcessIndex, COMPANY_NAME, PRODUCT_NAME),
	mbUpdateGUISettings(true),
	mbBypass(false)
{
	dynamic_cast<CBitCrunchDSP*>(mpDSPEngine)->SetChannels(2);

	mpDezipper->SetCallback(dynamic_cast<IBaseDezipperCallback*>(this));
}

CBitCrunchPlugIn::~CBitCrunchPlugIn()
{
}

void CBitCrunchPlugIn::Destroy()
{
	delete dynamic_cast<CBitCrunchPlugIn*>(this);
}

kspi::IGUI* CBitCrunchPlugIn::CreateGUI(tint32 /*iIndex*/)
{
	CBaseGUI* pBaseGUI = dynamic_cast<CBaseGUI*>(new CBitCrunchGUI(this, mpParmMan));

	mGUIs.push_back(pBaseGUI);

	kspi::IGUI* pKSPIGUI = dynamic_cast<kspi::IGUI*>(pBaseGUI);

	return pKSPIGUI;
}

void CBitCrunchPlugIn::GetChannelConfigurations(tuint32* puiNrOfConfigurations, tuint32* puiChannelsIn, tuint32* puiChannelsOut, tuint32* puiSideChainChannels)
{
	CAutoLock Lock(mMutex);

	*puiNrOfConfigurations = 1;

	*puiChannelsIn = 2;
	*puiChannelsOut = 2;
	*puiSideChainChannels = 0;
}

void CBitCrunchPlugIn::Initialize()
{
	CBasePlugIn::Initialize();
}

void CBitCrunchPlugIn::AddParameters()
{
	mpParmMan->SetGlobalSectionCount(1);

	// Bypass
	AddGlobalParm(0, giParmIDBitCrunchBypass, 0, 1, 0);
	AddGlobalParm(0, giParmIDBit, 4, 256, 32);
	AddGlobalParm(0, giParmIDSR, 200, 44100, 11050);
	AddGlobalParm(0, giParmIDMix, 0, 128, 64);
/*	AddGlobalParm(0, giParam_Feedback, 0, 100, 80);
	AddGlobalParm(0, giParam_HP_Damping, 31, 16000, 31);
	AddGlobalParm(0, giParam_LP_Damping, 32, 12001, 12000);
	AddGlobalParm(0, giParam_Sync, 0, 1, 0);
	AddGlobalParm(0, giParam_Mix, 0, 128, 48);
	*/

	// TODO: Create parameters by adding parm-IDs to parameter-manager below here.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_plug_in.html
	// NOTE: Remember to first create the parm-IDs in ParmIDs.h.
	// NOTE: You *must* add the parm-IDs in value order! Also don't skip any!


	// Add parameters to dezipper
//	mpDezipper->AddParameter(giParmIDBitCrunchVolume, 0, 3162, 1000);
}

void CBitCrunchPlugIn::MakeAutomationMap()
{
	// Bypass
	InsertParameterEnum(giParmIDBitCrunchBypass, "Bypass", "OFF@ON");

	// TODO: Add more parameters for automation below here.
	// Use the CBasePlugIn::InsertParameterXxx...() methods.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_plug_in.html

}

void CBitCrunchPlugIn::UpdateEngineData(tint32 iID, tint32 iValue)
{
	CAutoLock Lock(mMutex);

	if (UpdateEngineSettings()) {
			switch(iID) {
				case giParmIDBitCrunchBypass:
					mpDSPEngine->SetBypass((iValue == 0) ? false : true);
					break;
				case giParmIDBit:
					dynamic_cast<CBitCrunchDSP*>(mpDSPEngine)->Set_Bit(iValue);
					break;
				case giParmIDSR:
					dynamic_cast<CBitCrunchDSP*>(mpDSPEngine)->Set_SR(iValue );
					break;
				case giParmIDMix:
					dynamic_cast<CBitCrunchDSP*>(mpDSPEngine)->Set_Mix(iValue );
					break;
				

			}
		}

	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CBitCrunchGUI*>(pGUI)->UpdateEngineData(0, 0, iID, iValue);
	}
}

void CBitCrunchPlugIn::SetValue(tint32 iParmID, tint32 iValue)
{
	if (UpdateEngineSettings()) {
//		switch(iParmID) {
//		}
	}
}

void CBitCrunchPlugIn::UpdateAllGUI()
{
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CBitCrunchGUI*>(pGUI)->UpdateAll();
	}
}

tint32 CBitCrunchPlugIn::GetProductID()
{
	return 222;
}

void CBitCrunchPlugIn::GetProductName(tchar* pszName)
{
	strcpy(pszName, "BitCrush");
}


