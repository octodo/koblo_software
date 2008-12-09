#include "KSOS.h"

CTrack_Player::CTrack_Player(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;
	
}

CTrack_Player::~CTrack_Player()
{
}

void CTrack_Player::SetInfo()
{

}

void CTrack_Player::Init()
{
	mpScrollPane = ge::IScrollPane::Create();
	mpScrollPane->Init(ge::IControl::giNoID, giTimerID_Player1_ScrollPane);
	mpPane = dynamic_cast<ge::IPane*>(mpScrollPane);
//	mpPane->SetSize(ge::SSize(gTrack_Scroll_Editor.iCX, gTrack_Scroll_Editor.iCY ));
	mpPane->SetBackgroundColour(ge::SRGB(0,255,0));//ge::SRGB(242,0,242));
	
	//--------------------------------------------
	// Track_Player2
	// This is a container holding the audio files, it scrolls when KS plays
	mpTrack_Player2 = new CTrack_Player2(this, GetGUI());
	mpTrack_Player2->SetInfo(this);
	mpTrack_Player2->Init();
	mpPane->AddControl(mpTrack_Player2->GetPane(), ge::SPos(6, giTime_Pos_SizeY));
	mpTrack_Player2->Update_Graphic();

//	mpDragLine = CreateLine(ge::IControl::giNoID, ge::SPos(0, 0), ge::SPos(1, 10000), ge::SRGB(10, 10, 10));
//	mpDragLine->SetVisible(false);

	ge::IMouseTrap* pMouseTrap = ge::IMouseTrap::Create();
	pMouseTrap->EnableOneTrap(ge::MouseDrag, true, false);
	pMouseTrap->EnableOneTrapOutside(ge::MouseDrag, true, false);
	pMouseTrap->EnableOneTrap(ge::MouseDrop, true, false);
	pMouseTrap->EnableOneTrapOutside(ge::MouseDrop, true, false);
	mpPane->AddControl(pMouseTrap, ge::SPos(0, 0));
	pMouseTrap->SetSize(ge::SSize(10000, 10000));
	pMouseTrap->AddListener(this);
	
	
	//--------------------------------------------
	// Track Position and playback
	// Scrolls during plaback
	mpTrack_Time_Pos = new CTrack_Time_Pos(this, GetGUI());
	mpTrack_Time_Pos->SetInfo();
	mpTrack_Time_Pos->Init();
//	mpPane->AddControl(mpTrack_Time_Pos->GetPane(), ge::SPos(gTrack_Side.iCX, gTrack_Top.iCY));
	mpPane->AddControl(mpTrack_Time_Pos->GetPane(), ge::SPos(0, 0));
	

}


void CTrack_Player::ConnectControls()
{
}

void CTrack_Player::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	/*
	switch(pControl->GetID()) {
		case giChC_Track_ShowMix:{
			GetPlugIn()->GetModule()->GetHost()->ActivateWindow(0);
			break;
			}
		case giChC_Track_ShowRack:
			GetPlugIn()->GetModule()->GetHost()->ActivateWindow(5);
			break;
		case giChC_Track_ShowShare:
			GetPlugIn()->GetModule()->GetHost()->ActivateWindow(2);
			break;
		case giChC_Track_ShowTransporter:
			GetPlugIn()->GetModule()->GetHost()->ActivateWindow(4);
			break;
	}
	*/
}

void CTrack_Player::EventGeneric(ge::IControl* pControl, void* pEventData)
{
/*
	ge::SEventGeneric* pEvent = (ge::SEventGeneric*)pEventData;
	if (pEvent->eType = ge::giEventMouse) {
		if (pEvent->iMsg == ge::MouseDrag) {
			if (pEvent->bInsideControlRect == false) {
				mpDragLine->SetVisible(false);
			}
			else {
				// Get Mouse Pos
				ge::SPos pos;
				mpPane->GetMousePos(pos);
				// Get Track pos
				ge::SPos panePos;
				mpPane->GetPos(panePos);
				// Find pos on track
				pos.iX = pos.iX - panePos.iX;
				// Adjust for current scrolling position
				ge::SScrollPos ScrollPos;
				dynamic_cast<ge::IScrollPane*>(mpPane)->GetScrollPos(ScrollPos);
				pos.iX = pos.iX + ScrollPos.VisibleRect.iX;

				mpDragLine->SetPos(ge::SPos(pos.iX, 0));
				mpDragLine->SetVisible(true);
			}
		}
		if (pEvent->iMsg == ge::MouseDrop) {
			mpDragLine->SetVisible(false);
		}
		Redraw_Pane_Rect();
	}
*/
}


void CTrack_Player::HandleMsg(SMsg* pMsg)
{
/*	switch(pMsg->iOpcode) 
	{
	}
	*/
	// Pass directly on to trackplayer 2 (inner scroll-pane)
//	SendMsg(pMsg);
}

void CTrack_Player::Resize(const ge::SSize& Size)
{
	CBasePane::Resize(Size);
}


ge::IScrollPane* CTrack_Player::GetScrollPane()
{
	return mpScrollPane;

}

ge::IScrollPane* CTrack_Player::GetScrollPane2()
{
	return mpTrack_Player2->GetScrollPane();

}


void CTrack_Player::Set_Track_Size(tint32 iChannel,tint32  iValue)
{
	mpTrack_Player2->Set_Track_Size(iChannel,  iValue);
	
}



void CTrack_Player::Set_Play_Head(tint64 iSample_Pos)
{
	mpTrack_Time_Pos->Set_Play_Head(iSample_Pos);
	mpTrack_Player2->Set_LinePos(iSample_Pos);

}

void CTrack_Player::Set_Scrol_Pane_Size_X(tint32 iSizeX)
{
	ge::SScrollPos sp;
	GetScrollPane()->GetScrollPos(sp);

	if( sp.AreaSize.iCX != iSizeX){
		sp.AreaSize.iCX = iSizeX; 
		GetScrollPane()->SetScrollPos(sp);
		
		mpTrack_Time_Pos->GetPane()->SetSize(ge::SSize(iSizeX, 48));
	}
}

void CTrack_Player::SetTrackTopPtr(CTrack_Top* pTrack_Top)
{ 
	mpTrack_Time_Pos->SetTrackTopPtr(pTrack_Top);
	mpTrack_Player2->SetTrackTopPtr(pTrack_Top);
}