
#include "KVerbOS.h"


// TODO: Update company name and product name (may contain spaces and other special chars)
#define COMPANY_NAME "Koblo"
#define PRODUCT_NAME "KVerb"


const tint giPolyphony = 4;
const tint giReserve = 4;
const tint giMaxSubBufferSize = 32;

CKVerbPlugIn::CKVerbPlugIn(CKVerbModule* pModule, tuint32 uiProcessIndex)
: CBasePlugIn(dynamic_cast<CBaseModule*>(pModule), giAudioMaxBufferSize, dynamic_cast<CBaseDSPEngine*>(new CKVerbDSP()), uiProcessIndex, COMPANY_NAME, PRODUCT_NAME),
	mbUpdateGUISettings(true),
	mbBypass(false)
{
	dynamic_cast<CKVerbDSP*>(mpDSPEngine)->SetChannels(2);

	mpDezipper->SetCallback(dynamic_cast<IBaseDezipperCallback*>(this));
}

CKVerbPlugIn::~CKVerbPlugIn()
{
}

void CKVerbPlugIn::Destroy()
{
	delete dynamic_cast<CKVerbPlugIn*>(this);
}

kspi::IGUI* CKVerbPlugIn::CreateGUI(tint32 /*iIndex*/)
{
	CBaseGUI* pBaseGUI = dynamic_cast<CBaseGUI*>(new CKVerbGUI(this, mpParmMan));

	mGUIs.push_back(pBaseGUI);

	kspi::IGUI* pKSPIGUI = dynamic_cast<kspi::IGUI*>(pBaseGUI);

	return pKSPIGUI;
}

void CKVerbPlugIn::GetChannelConfigurations(tuint32* puiNrOfConfigurations, tuint32* puiChannelsIn, tuint32* puiChannelsOut, tuint32* puiSideChainChannels)
{
	CAutoLock Lock(mMutex);

	*puiNrOfConfigurations = 1;

	*puiChannelsIn = 2;
	*puiChannelsOut = 2;
	*puiSideChainChannels = 0;
}

void CKVerbPlugIn::Initialize()
{
	CBasePlugIn::Initialize();
}

void CKVerbPlugIn::AddParameters()
{
	mpParmMan->SetGlobalSectionCount(1);

	// Bypass
	AddGlobalParm(0, giParmIDKVerbBypass, 0, 1, 0);
	AddGlobalParm(0, giParam_Reverb_Size, 0, 128, 48);
	AddGlobalParm(0, giParam_Reverb_PDelay, 0, 128, 0);
	AddGlobalParm(0, giParam_Reverb_Damp, 0, 128, 8);
	AddGlobalParm(0, giParam_Reverb_Color2, 0, 128, 80);
	AddGlobalParm(0, giParamID_Mix, 0, 128, 32);

	// Add parameters to dezipper
//	mpDezipper->AddParameter(giParmIDKVerbVolume, 0, 3162, 1000);
}

void CKVerbPlugIn::MakeAutomationMap()
{
	// Bypass
	InsertParameterEnum(giParmIDKVerbBypass, "Bypass", "OFF@ON");

	// TODO: Add more parameters for automation below here.
	// Use the CBasePlugIn::InsertParameterXxx...() methods.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_plug_in.html

}

void CKVerbPlugIn::UpdateEngineData(tint32 iID, tint32 iValue)
{
	CAutoLock Lock(mMutex);

	if (UpdateEngineSettings()) {
		switch(iID) {
			case giParmIDKVerbBypass:
				mpDSPEngine->SetBypass((iValue == 0) ? false : true);
				break;
			case giParam_Reverb_Size:
				dynamic_cast<CKVerbDSP*>(mpDSPEngine)->SetRoomSize(iValue / 128.0f );
				break;
			case giParam_Reverb_PDelay:
				dynamic_cast<CKVerbDSP*>(mpDSPEngine)->SetPredelay(iValue );
				break;
			case giParam_Reverb_Damp:
				dynamic_cast<CKVerbDSP*>(mpDSPEngine)->SetDamp(iValue / 128.0f );
				break;
			case giParam_Reverb_Color2:
				dynamic_cast<CKVerbDSP*>(mpDSPEngine)->SetLP(iValue / 600.0f );
				break;
			case giParamID_Mix:
				dynamic_cast<CKVerbDSP*>(mpDSPEngine)->SetMix(iValue/ 128.0f );
				break;
		}
	}

	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CKVerbGUI*>(pGUI)->UpdateEngineData(0, 0, iID, iValue);
	}
}

void CKVerbPlugIn::SetValue(tint32 iParmID, tint32 iValue)
{
	if (UpdateEngineSettings()) {
//		switch(iParmID) {
//		}
	}
}

void CKVerbPlugIn::UpdateAllGUI()
{
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CKVerbGUI*>(pGUI)->UpdateAll();
	}
}

tint32 CKVerbPlugIn::GetProductID()
{
	return 25;//113; 
}

void CKVerbPlugIn::GetProductName(tchar* pszName)
{
	strcpy(pszName, "KVerb");
}


