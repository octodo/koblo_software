#include "KSOS.h"

CTrack_Info_Small::CTrack_Info_Small(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

}

CTrack_Info_Small::~CTrack_Info_Small()
{

}

void CTrack_Info_Small::SetInfo(tint32 iID )
{
	miID = iID;
	miCtrl_Offset	= giFirst_Track_Ctrl + ( miID * giNr_Of_Track_Ctls);
	miSection		= iID + giSection_First_Track;
}

void CTrack_Info_Small::Init()
{
	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(gTrack_Info_Small);

	// Dummy
	mpSmall_Back_Drop				= CreateBitmap(ge::IControl::giNoID, IDB_Back_Track_Info_Small, ge::SPos(0, 0));
	mpSmall_Selected_Back_Drop		= CreateBitmap(ge::IControl::giNoID, IDB_Back_Track_Info_Small_Selected, ge::SPos(0, 0));
	// Hide selection backdrop
	mpSmall_Selected_Back_Drop->SetVisible(false);
	
	//--------------------------------------------
	// Text label
	ge::IText* pText = CreateDisplay(	giTrack_Info_Small_Channel_Text+ miCtrl_Offset, 
												ge::SPos(5, 24),  
												ge::SSize(118, 10), 
												CreateFont(Generic128, IDB_Minix_Tight, ge::SRGB(204, 204, 204)),
												true);

	pText->SetHorzAlignment(ge::IText::HorzAlignLeft);
	pText->SetClicksOutsideAsSuccess();
	
	// Expand button
	ge::IMultiStateButton*  pButton = Create2StateButton(giTrack_Info_Small_Expand_Button	+ miCtrl_Offset, IDB_Track_Button_Expand, ge::SPos(2, 4), true);
	pButton->SetEventMouseUp();
	// Arm button
	Create2StateButton(giTrack_Info_Small_Arm_Button	+ miCtrl_Offset, IDB_Button_Arm_Horizontal, ge::SPos(68, 4), true);
	// Mute button
	Create2StateButton(giTrack_Info_Small_Mute_Button	+ miCtrl_Offset, IDB_Button_Mute_Horizontal, ge::SPos(68+19, 4), true);
	//Solo Button
	Create2StateButton(giTrack_Info_Small_Solo_Button	+ miCtrl_Offset, IDB_Button_Solo_Horizontal, ge::SPos(68+19+18, 4), true); 
	
	// Output meter, Left
	mpMeterOutL = CreatePeakMeter(ge::IControl::giNoID, ge::SPos(30, 6), 33, ge::IPeakMeter::Right);
	FindPane(mpMeterOutL)->CreateBitmap(ge::IControl::giNoID, IDB_Meter_Off_Top, ge::SPos(0, 0));
	// Output meter, Right
	mpMeterOutR = CreatePeakMeter(ge::IControl::giNoID, ge::SPos(30, 12), 33, ge::IPeakMeter::Right);
	FindPane(mpMeterOutR)->CreateBitmap(ge::IControl::giNoID, IDB_Meter_Off_Bottom, ge::SPos(0, 0));

	// Mouse-trap for activation of track - must be last to have effect
	mpMouseTrap = ge::IMouseTrap::Create();
	mpMouseTrap->EnableOneTrap(ge::MouseDrag, true, true);
	mpMouseTrap->EnableOneTrap(ge::MouseDrop, true, true);
	mpPane->AddControl(mpMouseTrap, ge::SPos(0, 0));
	mpMouseTrap->CloneRect(GetPane());
	mpMouseTrap->AddListener(this);

}


void CTrack_Info_Small::ConnectControls()
{
	
	RegisterGlobalControl(miSection, giTrack_Info_Small_Expand_Button + miCtrl_Offset, giParam_Track_Info_SizeY);
	// Solo Mute
	RegisterGlobalControl(miSection, giTrack_Info_Small_Solo_Button + miCtrl_Offset, giParam_ChSolo);
	RegisterGlobalControl(miSection, giTrack_Info_Small_Mute_Button + miCtrl_Offset, giParam_ChMute);
	RegisterGlobalControl(miSection, giTrack_Info_Small_Arm_Button + miCtrl_Offset, giParam_ChArm);

}

void CTrack_Info_Small::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{


	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
}

void CTrack_Info_Small::EventGeneric(ge::IControl* pControl, void* pEventData)
{
	tint32 iID = pControl->GetID() - miCtrl_Offset;
	
	switch(iID) {
		case giTrack_Info_Small_Channel_Text:
			std::string sText = std::string((const tchar*)pEventData);
			dynamic_cast<CKSPlugIn*>(mpGUI->GetPlugIn())->SetChannelName(miID, sText);
			return;
	}
	
	ge::SEventGeneric* pEvent = (ge::SEventGeneric*)pEventData;
	if (pEvent->eType = ge::giEventMouse) {
	
		switch(pEvent->iMsg){
		
			case ge::MouseDrop: {
				SendMsg(Msg_Track_MouseDrop);
				break;
			}
			case ge::MouseDrag: {
				SendMsg(Msg_Track_MouseDrag);
				break;
			}
		}
	}	
}

void CTrack_Info_Small::SendMsg(tint32 iMsg)
{
	SMsg Msg			=	iMsg;
	Msg.pDataIn			=	(void*)&miID;
	SendMsgFromTop(&Msg);
}

void CTrack_Info_Small::HandleMsg(SMsg* pMsg)
{

}

void CTrack_Info_Small::SelectTrack( tbool bSelected)
{

	mpSmall_Selected_Back_Drop->SetVisible(bSelected);

}


void CTrack_Info_Small::UpdateTrackMeters_Maybe(void* pvoid_sMeters_All)
{
	if (mpPane->IsVisible()) {
		if (pvoid_sMeters_All == NULL) {
			mpMeterOutL->SetFloatValueWithDecay(1.0, 0.0);
			mpMeterOutR->SetFloatValueWithDecay(1.0, 0.0);
		}
		else {
			CKSDSP::SMeters_All* psMeters_All = (CKSDSP::SMeters_All*)pvoid_sMeters_All;
			tfloat32* pMetersForThisChannel = psMeters_All->ppfMeters_Tracks[miID];
			mpMeterOutL->SetFloatValueWithDecay(pMetersForThisChannel[0], psMeters_All->fDecayFactor);
			mpMeterOutR->SetFloatValueWithDecay(pMetersForThisChannel[1], psMeters_All->fDecayFactor);
		}
	}
} // UpdateTrackMeters_Maybe


void CTrack_Info_Small::UpdateChannelName(const std::string& sName)
{	
	ge::IControl* pControl = mpPane->GetControl(giTrack_Info_Small_Channel_Text + miCtrl_Offset);
	ge::IText* pText = dynamic_cast<ge::IText*>(pControl);
	pText->SetText(sName.c_str());
}