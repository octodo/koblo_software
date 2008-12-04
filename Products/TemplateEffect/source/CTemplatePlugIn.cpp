
#include "TemplateOS.h"


// TODO: Update company name and product name (may contain spaces and other special chars)
#define COMPANY_NAME "MyCompany"
#define PRODUCT_NAME "Template"


const tint giPolyphony = 4;
const tint giReserve = 4;
const tint giMaxSubBufferSize = 32;

CTemplatePlugIn::CTemplatePlugIn(CTemplateModule* pModule, tuint32 uiProcessIndex)
: CBasePlugIn(dynamic_cast<CBaseModule*>(pModule), giAudioMaxBufferSize, dynamic_cast<CBaseDSPEngine*>(new CTemplateDSP()), uiProcessIndex, COMPANY_NAME, PRODUCT_NAME),
	mbUpdateGUISettings(true),
	mbBypass(false)
{
	dynamic_cast<CTemplateDSP*>(mpDSPEngine)->SetChannels(2);

	mpDezipper->SetCallback(dynamic_cast<IBaseDezipperCallback*>(this));
}

CTemplatePlugIn::~CTemplatePlugIn()
{
}

void CTemplatePlugIn::Destroy()
{
	delete dynamic_cast<CTemplatePlugIn*>(this);
}

kspi::IGUI* CTemplatePlugIn::CreateGUI(tint32 /*iIndex*/)
{
	CBaseGUI* pBaseGUI = dynamic_cast<CBaseGUI*>(new CTemplateGUI(this, mpParmMan));

	mGUIs.push_back(pBaseGUI);

	kspi::IGUI* pKSPIGUI = dynamic_cast<kspi::IGUI*>(pBaseGUI);

	return pKSPIGUI;
}

void CTemplatePlugIn::GetChannelConfigurations(tuint32* puiNrOfConfigurations, tuint32* puiChannelsIn, tuint32* puiChannelsOut, tuint32* puiSideChainChannels)
{
	CAutoLock Lock(mMutex);

	*puiNrOfConfigurations = 1;

	*puiChannelsIn = 2;
	*puiChannelsOut = 2;
	*puiSideChainChannels = 0;
}

void CTemplatePlugIn::Initialize()
{
	CBasePlugIn::Initialize();
}

void CTemplatePlugIn::AddParameters()
{
	mpParmMan->SetGlobalSectionCount(1);

	// Bypass
	AddGlobalParm(0, giParmIDTemplateBypass, 0, 1, 0);

	// TODO: Create parameters by adding parm-IDs to parameter-manager below here.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_plug_in.html
	// NOTE: Remember to first create the parm-IDs in ParmIDs.h.
	// NOTE: You *must* add the parm-IDs in value order! Also don't skip any!


	// Add parameters to dezipper
//	mpDezipper->AddParameter(giParmIDTemplateVolume, 0, 3162, 1000);
}

void CTemplatePlugIn::MakeAutomationMap()
{
	// Bypass
	InsertParameterEnum(giParmIDTemplateBypass, "Bypass", "OFF@ON");

	// TODO: Add more parameters for automation below here.
	// Use the CBasePlugIn::InsertParameterXxx...() methods.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_plug_in.html

}

void CTemplatePlugIn::UpdateEngineData(tint32 iID, tint32 iValue)
{
	CAutoLock Lock(mMutex);

	if (UpdateEngineSettings()) {
		switch(iID) {
			case giParmIDTemplateBypass:
				mpDSPEngine->SetBypass((iValue == 0) ? false : true);
				break;


			// TODO: Add a case for every parm-ID.
			// Remember to create a corresponding CTemplateDSP::SetXxx...() method to call for each case.

		}
	}

	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CTemplateGUI*>(pGUI)->UpdateEngineData(0, 0, iID, iValue);
	}
}

void CTemplatePlugIn::SetValue(tint32 iParmID, tint32 iValue)
{
	if (UpdateEngineSettings()) {
//		switch(iParmID) {
//		}
	}
}

void CTemplatePlugIn::UpdateAllGUI()
{
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CTemplateGUI*>(pGUI)->UpdateAll();
	}
}

tint32 CTemplatePlugIn::GetProductID()
{
	return 10;
}

void CTemplatePlugIn::GetProductName(tchar* pszName)
{
	strcpy(pszName, "MyProduct");
}


