
#include "VChorusOS.h"


// TODO: Update company name and product name (may contain spaces and other special chars)
#define COMPANY_NAME "Koblo"
#define PRODUCT_NAME "VChorus"


const tint giPolyphony = 4;
const tint giReserve = 4;
const tint giMaxSubBufferSize = 32;

CVChorusPlugIn::CVChorusPlugIn(CVChorusModule* pModule, tuint32 uiProcessIndex)
: CBasePlugIn(dynamic_cast<CBaseModule*>(pModule), giAudioMaxBufferSize, dynamic_cast<CBaseDSPEngine*>(new CVChorusDSP()), uiProcessIndex, COMPANY_NAME, PRODUCT_NAME),
	mbUpdateGUISettings(true),
	mbBypass(false)
{
	dynamic_cast<CVChorusDSP*>(mpDSPEngine)->SetChannels(2);

	mpDezipper->SetCallback(dynamic_cast<IBaseDezipperCallback*>(this));
}

CVChorusPlugIn::~CVChorusPlugIn()
{
}

void CVChorusPlugIn::Destroy()
{
	delete dynamic_cast<CVChorusPlugIn*>(this);
}

kspi::IGUI* CVChorusPlugIn::CreateGUI(tint32 /*iIndex*/)
{
	CBaseGUI* pBaseGUI = dynamic_cast<CBaseGUI*>(new CVChorusGUI(this, mpParmMan));

	mGUIs.push_back(pBaseGUI);

	kspi::IGUI* pKSPIGUI = dynamic_cast<kspi::IGUI*>(pBaseGUI);

	return pKSPIGUI;
}

void CVChorusPlugIn::GetChannelConfigurations(tuint32* puiNrOfConfigurations, tuint32* puiChannelsIn, tuint32* puiChannelsOut, tuint32* puiSideChainChannels)
{
	CAutoLock Lock(mMutex);

	*puiNrOfConfigurations = 1;

	*puiChannelsIn = 2;
	*puiChannelsOut = 2;
	*puiSideChainChannels = 0;
}

void CVChorusPlugIn::Initialize()
{
	CBasePlugIn::Initialize();
}

void CVChorusPlugIn::AddParameters()
{
	mpParmMan->SetGlobalSectionCount(1);

	// Bypass
	AddGlobalParm(0, giParmIDVChorusBypass, 0, 1, 0);

	AddGlobalParm(0, giParam_DelayTime, 50,300, 150);
	AddGlobalParm(0, giParam_Depth, 0, 100, 80);
	AddGlobalParm(0, giParam_Rate, 1, 1000, 10);
	AddGlobalParm(0, giParam_With, 0, 100, 0);
	AddGlobalParm(0, giParam_Mix, 0, 128, 128);

	// TODO: Create parameters by adding parm-IDs to parameter-manager below here.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_plug_in.html
	// NOTE: Remember to first create the parm-IDs in ParmIDs.h.
	// NOTE: You *must* add the parm-IDs in value order! Also don't skip any!


	// Add parameters to dezipper
//	mpDezipper->AddParameter(giParmIDVChorusVolume, 0, 3162, 1000);
}

void CVChorusPlugIn::MakeAutomationMap()
{
	// Bypass
	InsertParameterEnum(giParmIDVChorusBypass, "Bypass", "OFF@ON");

	// TODO: Add more parameters for automation below here.
	// Use the CBasePlugIn::InsertParameterXxx...() methods.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_plug_in.html

}

void CVChorusPlugIn::UpdateEngineData(tint32 iID, tint32 iValue)
{
	CAutoLock Lock(mMutex);

	if (UpdateEngineSettings()) {
		switch(iID) {
			case giParmIDVChorusBypass:
				mpDSPEngine->SetBypass((iValue == 0) ? false : true);
				break;

			
			case giParam_DelayTime:
				dynamic_cast<CVChorusDSP*>(mpDSPEngine)->SetDelayTime(iValue);
				break;
			case giParam_Depth:
				dynamic_cast<CVChorusDSP*>(mpDSPEngine)->SetDepth(iValue );
				break;
			case giParam_Rate:
				dynamic_cast<CVChorusDSP*>(mpDSPEngine)->SetRate(iValue );
				break;
			case giParam_With:
				dynamic_cast<CVChorusDSP*>(mpDSPEngine)->SetWith(iValue );
				break;
			case giParam_Mix:
				dynamic_cast<CVChorusDSP*>(mpDSPEngine)->SetMix(iValue );
				break;
			
		}
	}

	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CVChorusGUI*>(pGUI)->UpdateEngineData(0, 0, iID, iValue);
	}
}

void CVChorusPlugIn::SetValue(tint32 iParmID, tint32 iValue)
{
	if (UpdateEngineSettings()) {
//		switch(iParmID) {
//		}
	}
}

void CVChorusPlugIn::UpdateAllGUI()
{
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CVChorusGUI*>(pGUI)->UpdateAll();
	}
}

tint32 CVChorusPlugIn::GetProductID()
{
	return 26;//114;
}

void CVChorusPlugIn::GetProductName(tchar* pszName)
{
	strcpy(pszName, "VChorus");
}


