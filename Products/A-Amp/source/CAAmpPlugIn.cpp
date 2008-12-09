
#include "AAmpOS.h"


// TODO: Update company name and product name (may contain spaces and other special chars)
#define COMPANY_NAME "Koblo"
#define PRODUCT_NAME "AAmp"


const tint giPolyphony = 4;
const tint giReserve = 4;
const tint giMaxSubBufferSize = 32;

CAAmpPlugIn::CAAmpPlugIn(CAAmpModule* pModule, tuint32 uiProcessIndex)
: CBasePlugIn(dynamic_cast<CBaseModule*>(pModule), giAudioMaxBufferSize, dynamic_cast<CBaseDSPEngine*>(new CAAmpDSP()), uiProcessIndex, COMPANY_NAME, PRODUCT_NAME),
	mbUpdateGUISettings(true),
	mbBypass(false)
{
	dynamic_cast<CAAmpDSP*>(mpDSPEngine)->SetChannels(2);

	mpDezipper->SetCallback(dynamic_cast<IBaseDezipperCallback*>(this));
}

CAAmpPlugIn::~CAAmpPlugIn()
{
}

void CAAmpPlugIn::Destroy()
{
	delete dynamic_cast<CAAmpPlugIn*>(this);
}

kspi::IGUI* CAAmpPlugIn::CreateGUI(tint32 /*iIndex*/)
{
	CBaseGUI* pBaseGUI = dynamic_cast<CBaseGUI*>(new CAAmpGUI(this, mpParmMan));

	mGUIs.push_back(pBaseGUI);

	kspi::IGUI* pKSPIGUI = dynamic_cast<kspi::IGUI*>(pBaseGUI);

	return pKSPIGUI;
}

void CAAmpPlugIn::GetChannelConfigurations(tuint32* puiNrOfConfigurations, tuint32* puiChannelsIn, tuint32* puiChannelsOut, tuint32* puiSideChainChannels)
{
	CAutoLock Lock(mMutex);

	*puiNrOfConfigurations = 1;

	*puiChannelsIn = 2;
	*puiChannelsOut = 2;
	*puiSideChainChannels = 0;
}

void CAAmpPlugIn::Initialize()
{
	CBasePlugIn::Initialize();
}

void CAAmpPlugIn::AddParameters()
{
	mpParmMan->SetGlobalSectionCount(1);

	// Bypass
	AddGlobalParm(0, giParmIDAAmpBypass, 0, 1, 0);
	AddGlobalParm(0, giParam_Gain, 1000, 5000, 2000);
	AddGlobalParm(0, giParam_Tube, 0, 1, 1);
	AddGlobalParm(0, giParam_Harm, 0, 1, 1);
	AddGlobalParm(0, giParam_Limiter, 0, 1, 1);
	AddGlobalParm(0, giParam_Swap, 0, 5, 0);

	

	// TODO: Create parameters by adding parm-IDs to parameter-manager below here.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_plug_in.html
	// NOTE: Remember to first create the parm-IDs in ParmIDs.h.
	// NOTE: You *must* add the parm-IDs in value order! Also don't skip any!


	// Add parameters to dezipper
//	mpDezipper->AddParameter(giParmIDAAmpVolume, 0, 3162, 1000);
}

void CAAmpPlugIn::MakeAutomationMap()
{
	// Bypass
	InsertParameterEnum(giParmIDAAmpBypass, "Bypass", "OFF@ON");

	// TODO: Add more parameters for automation below here.
	// Use the CBasePlugIn::InsertParameterXxx...() methods.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_plug_in.html

}

void CAAmpPlugIn::UpdateEngineData(tint32 iID, tint32 iValue)
{
	CAutoLock Lock(mMutex);

	if (UpdateEngineSettings()) {
		switch(iID) {
				
			case giParam_Gain:{
				tfloat	fValue	=	iValue * 0.001;
				dynamic_cast<CAAmpDSP*>(mpDSPEngine)->Set_Gain(fValue);
				break;
			}
			case giParam_Limiter:
				dynamic_cast<CAAmpDSP*>(mpDSPEngine)->Set_Limiter(iValue );
				break;
			case giParam_Harm:
				dynamic_cast<CAAmpDSP*>(mpDSPEngine)->Set_Harm(iValue );
				break;
			case giParam_Tube:
				dynamic_cast<CAAmpDSP*>(mpDSPEngine)->Set_Tube(iValue );
				break;
			case giParam_Swap:
				dynamic_cast<CAAmpDSP*>(mpDSPEngine)->Set_Swap(iValue );
				break;
			
		}
		
	}

	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CAAmpGUI*>(pGUI)->UpdateEngineData(0, 0, iID, iValue);
		
	}
}

void CAAmpPlugIn::SetValue(tint32 iParmID, tint32 iValue)
{
	if (UpdateEngineSettings()) {
//		switch(iParmID) {
//		}
	}
}

void CAAmpPlugIn::UpdateAllGUI()
{
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CAAmpGUI*>(pGUI)->UpdateAll();
	}
}

tint32 CAAmpPlugIn::GetProductID()
{
	return 27;//115;
	
	
}

void CAAmpPlugIn::GetProductName(tchar* pszName)
{
	strcpy(pszName, "AAmp");
}


