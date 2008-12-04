#include "KSOS.h"

CTrack_Info::CTrack_Info(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

	// Lasse, added 2008-04-16
	mpMouseTrap = NULL;

}

CTrack_Info::~CTrack_Info()
{
	
}

void CTrack_Info::SetInfo(tint32 iID, tint32 iSize, tbool bIs_Visible, tint32 iTimerID)
{
	miID	= iID;
	miTimerID = iTimerID;
	miSize = iSize;
	mbIs_Visible = bIs_Visible;

}

void CTrack_Info::Init()
{
	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(gTrack_Info_Small);
//	mpPane->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteY );
	
	mpPane->SetVisible(false);
	
	// Mouse-trap for activation of track 
	mpMouseTrap = ge::IMouseTrap::Create();
	// Only link into chain, but don't remove message (the third param = false)
	mpMouseTrap->EnableOneTrap(ge::LeftButtonDown, true, false);
	mpPane->AddControl(mpMouseTrap, ge::SPos(0, 0));
	mpMouseTrap->PlaceOnTopOf(GetPane());
	mpMouseTrap->AddListener(this);



	//--------------------------------------------
	// Small Track info setup
	mpTrack_Info_Small = new CTrack_Info_Small(this, GetGUI());
	mpTrack_Info_Small->SetInfo(miID);
	mpTrack_Info_Small->Init();
	mpPane->AddControl(mpTrack_Info_Small->GetPane(), ge::SPos(0, 0));
	
	//--------------------------------------------
	// Big Track info setup
	mpTrack_Info_Big = new CTrack_Info_Big(this, GetGUI());
	mpTrack_Info_Big->SetInfo( miID, miTimerID);
	mpTrack_Info_Big->Init();
	mpPane->AddControl(mpTrack_Info_Big->GetPane(), ge::SPos(0, 0));
	// Hide big track info
	mpTrack_Info_Big->GetPane()->SetVisible(false);
	mbIs_Visible	=	false;
	
		
	
	
}


void CTrack_Info::ConnectControls()
{
	mpTrack_Info_Small->ConnectControls();
	mpTrack_Info_Big->ConnectControls();
}

void CTrack_Info::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
}

void CTrack_Info::EventGeneric(ge::IControl* pControl, void* pEventData)
{
	ge::SEventGeneric* pEvent = (ge::SEventGeneric*)pEventData;
	if (pEvent->eType = ge::giEventMouse) {
		// Event happened inside control
		if (pEvent->bInsideControlRect) {
			
			if (pEvent->iMsg == ge::LeftButtonDown) {
				CKSPlugIn* pPlugIn = dynamic_cast<CKSPlugIn*>(GetPlugIn());
				tint32 iBlueTrack = pPlugIn->GetSelectedTrack();
				if ((!ge::IWindow::ShiftPressed()) || (iBlueTrack == -1)) {
					// No shift key - or no previous selection
					// When no shift key is held we clear all selections on track ...
					for (tint32 i = 0; i < giNumber_Of_Tracks; i++) {
						STrackSelectionInfo sPrevSel = gpDSPEngine->GetTrackSelection(i);
						if (sPrevSel.uiSelection_Type != giSelect_Off) {
							gpDSPEngine->SetTrackSelection(i, 0, 0, giSelect_Off);
							// Post message
							SMsg msg(Msg_Update_Track_Selection, (void*)i);
							SendMsgFromTop(&msg);
						}
					}
					// ... then we see what to select
					if (iBlueTrack == miID) {
						// Second click: deselect
						pPlugIn->SelectTrack(-1);
					}
					else {
						// First click or clicked elsewhere: Select new track
						// Select all of new track
						pPlugIn->SelectTrack(miID);
						tint64 iFirstSample_Dummy = 0, iFinalSample = -1;
						gpDSPEngine->CalcTrackDuration(miID, &iFirstSample_Dummy, &iFinalSample);
						gpDSPEngine->SetTrackSelection(miID, 0, iFinalSample + 1, giSelect_On_Track);
						SMsg msg(Msg_Update_Track_Selection, (void*)miID);
						SendMsgFromTop(&msg);
					}
				}
				else {
					// Shift key is held and a previous selection exists
					STrackSelectionInfo sPrevSel = gpDSPEngine->GetTrackSelection(miID);
					if (sPrevSel.uiSelection_Type != giSelect_Off) {
						// Clicked once again - remove selection on track
						gpDSPEngine->SetTrackSelection(miID, 0, 0, giSelect_Off);
						// Post message
						SMsg msg(Msg_Update_Track_Selection, (void*)miID);
						SendMsgFromTop(&msg);
						// See if another track will take over the "blueishnes"
						if (iBlueTrack == miID) {
							pPlugIn->SelectTrack(-1);
							for (tint32 i = 0; i < giNumber_Of_Tracks; i++) {
								sPrevSel = gpDSPEngine->GetTrackSelection(i);
								if (sPrevSel.uiSelection_Type != giSelect_Off) {
									pPlugIn->SelectTrack(i);
									break;
								}
							}
						}
					}
					else {
						// A new track should be added to selection
						STrackSelectionInfo sBlueSel = gpDSPEngine->GetTrackSelection(iBlueTrack);
						gpDSPEngine->SetTrackSelection(miID, sBlueSel.uiSelection_Pos, sBlueSel.uiSelection_Duration, giSelect_On_Track);
						// Post message
						SMsg msg(Msg_Update_Track_Selection, (void*)miID);
						SendMsgFromTop(&msg);
					}
				}
			}
		}
	}
}


void CTrack_Info::HandleMsg(SMsg* pMsg)
{
			

}

void CTrack_Info::SelectTrack( tbool bSelected)
{

	mpTrack_Info_Small->SelectTrack(bSelected);

}

void CTrack_Info::Set_Track_Size( tint32 iSize)
{

	if(iSize == 0) {
	// Small
		mpPane->SetSize(gTrack_Info_Small);
		mpMouseTrap->SetSize(gTrack_Info_Small);
		miSize = gTrack_Info_Small.iCY;
		mpTrack_Info_Big->GetPane()->SetVisible(false);
		mpTrack_Info_Small->GetPane()->SetVisible(true);
	}
	else {
	// Big
		mpPane->SetSize(gTrack_Info_Big);
		mpMouseTrap->SetSize(gTrack_Info_Big);
		miSize = gTrack_Info_Big.iCY;
		mpTrack_Info_Big->GetPane()->SetVisible(true);
		mpTrack_Info_Small->GetPane()->SetVisible(false);
	}
	
	
	
//	GetPlugIn()->Stack_Tracks();
}


void CTrack_Info::UpdateTrackMeters_Maybe(void* mpvoid_sMeters_All)
{
	mpTrack_Info_Small->UpdateTrackMeters_Maybe(mpvoid_sMeters_All);
	mpTrack_Info_Big->UpdateTrackMeters_Maybe(mpvoid_sMeters_All);
} // UpdateTrackMeters_Maybe


void CTrack_Info::SetVisible(tbool bVisible)
{
	mbIs_Visible = bVisible;
	mpPane->SetVisible(bVisible);
}

void CTrack_Info::SetPos(tint32 iPosY)
{
	mpPane->SetPos(ge::SPos(0, iPosY));
}

tint32 CTrack_Info::GetSizeY() 
{
	return miSize;
}

void CTrack_Info::Track_Selected(tbool bSelected)
{
	mpTrack_Info_Small->SelectTrack( bSelected);
	mpTrack_Info_Big->SelectTrack( bSelected);
}

void CTrack_Info::UpdateChannelName( const std::string& sName)
{
	mpTrack_Info_Small->UpdateChannelName( sName);
	mpTrack_Info_Big->UpdateChannelName( sName);
}


