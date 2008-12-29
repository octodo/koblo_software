#include "KSOS.h"

CPlugin_Toolbar::CPlugin_Toolbar(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

	miChannel = 0;
	miInsert = 0;
	mpText = NULL;
}

CPlugin_Toolbar::~CPlugin_Toolbar()
{
}

void CPlugin_Toolbar::SetInfo(tint32 iChannel, tint32 iInsert)
{
	miChannel = iChannel;
	miInsert = iInsert;

	if (mpText) {
		
		// Verifyed by Max
		if (iChannel < 1024) {
			std::string sTrackName = dynamic_cast<CKSPlugIn*>(GetPlugIn())->GetChannelName(miChannel);
			tchar psz[128];
			if (sTrackName.size() == 0) {
				sprintf(psz, "Track %d, insert: %d", miChannel + 1, miInsert + 1);
			}
			else {
				sprintf(psz, "%s, insert: %d", sTrackName.c_str(), miInsert + 1);
			}
			mpText->SetText(psz);
		}
		else {
			if (miChannel < 2048) {
				tchar psz[128];
				sprintf(psz, "Bus %d, insert: %d", miChannel - 1024 + 1, miInsert + 1);
				mpText->SetText(psz);
			}
			else {
				tchar psz[128];
				sprintf(psz, "Master, insert: %d", miInsert + 1);
				mpText->SetText(psz);
			}
		}
	}

	if (miChannel < 1024) {
		RegisterGlobalControl(giSection_First_Track + miChannel, giControlBypass, giParam_Ch_Insert1Bypass + miInsert);
	}
	else if (miChannel < 2048) {
		RegisterGlobalControl(giSection_First_Buss + miChannel - 1024, giControlBypass, giParam_Bus_Insert1Bypass + miInsert);
	}
	else {
		RegisterGlobalControl(giSection_Master, giControlBypass, giParam_Master_Insert1Bypass + miInsert);
	}
}

void CPlugin_Toolbar::Init()
{
	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(397,24));
	mpPane->SetBackgroundColour(ge::SRGB(0, 0, 0));
	
	// Top label
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Plugin_Toolbar, ge::SPos(0, 0));
	ge::IBitmap*  pBmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Toolbar_Right, ge::SPos(397, 0));
	pBmp->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteX);
	//

	mpText = CreateText(ge::IControl::giNoID, ge::SPos(78, 9), ge::SSize(200, 12),
		CreateFont(Generic128, IDB_AxelTight, ge::SRGB(0, 255, 0)),
		"");
	
	
	//-------------------------------
	// AB

/*	ge::IRadioButtons* pABRadioButton = CreateRadioButtonsGroup(ge::IControl::giNoID, ge::SPos(0, 0));*/
	
	tint32 iX	=	4;
	Create2StateButton(giControlBypass, IDB_Button_Power, ge::SPos(4, 4), true);		iX += 73;
/*	CreateButton(ge::IControl::giNoID, IDB_Button_AB, ge::SPos(iX,4), false);				iX += 35;
	CreateButton(ge::IControl::giNoID, IDB_Button_BA, ge::SPos(iX,4), false);				iX += 34;
	CreateRadioButton(ge::IControl::giNoID, IDB_Button_A, ge::SPos(iX, 4), pABRadioButton, true);	iX	+= 18;
	CreateRadioButton(ge::IControl::giNoID, IDB_Button_B, ge::SPos(iX, 4), pABRadioButton, true);	iX	+= 17;*/
}


void CPlugin_Toolbar::ConnectControls()
{
/*	if (miChannel < 1024) {
		RegisterGlobalControl(giSection_First_Track + miChannel, giControlBypass, giParam_Ch_Insert1Bypass + miInsert);
	}
	else if (miChannel < 2048) {
		RegisterGlobalControl(giSection_First_Buss + miChannel - 1024, giControlBypass, giParam_Bus_Insert1Bypass + miInsert);
	}
	else {
		RegisterGlobalControl(giSection_Master, giControlBypass, giParam_Master_Insert1Bypass + miInsert);
	}*/
}

void CPlugin_Toolbar::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
}

void CPlugin_Toolbar::EventGeneric(ge::IControl* pControl, void* pEventData)
{
}


void CPlugin_Toolbar::HandleMsg(SMsg* pMsg)
{
}

void CPlugin_Toolbar::SetWidth(tuint32 uiNew)
{
	mpPane->SetSize(ge::SSize(uiNew, 24));
}

void CPlugin_Toolbar::UpdateChannelName(tint32 iChannel, const std::string& sName)
{
	if (iChannel == miChannel) {
		tchar psz[128];
		sprintf(psz, "%s   Insert %d", sName.c_str(), miInsert + 1);
		mpText->SetText(psz);
	}
}


