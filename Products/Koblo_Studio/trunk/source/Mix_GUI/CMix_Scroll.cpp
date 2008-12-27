#include "KSOS.h"

CMix_Scroll::CMix_Scroll(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;
	
}

CMix_Scroll::~CMix_Scroll()
{
}

void CMix_Scroll::SetInfo()
{

}

void CMix_Scroll::Init()
{

	mpScrollPane = ge::IScrollPane::Create();
	mpScrollPane->Init(ge::IControl::giNoID, giCtr_Mix_ScrollPane_Timer);
	mpPane = dynamic_cast<ge::IPane*>(mpScrollPane);
	
	mpPane->SetSize(ge::SSize(2*gMix.iCX, gMix.iCY-giScrollBarSize));
//	mpPane->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteX);
	
//	mpPane->StickTo(ge::IControl::PosTopLeft, ge::SPos(0, 0));
	mpPane->SetBackgroundColour(ge::SRGB(0, 0, 0));
	
//	miNr_Of_Tracks	= 0;
	miSelectedTrack	= -1;
	
	PreparePopups();
	
	for( tint32 i=0; i<giNumber_Of_Tracks; i++) {
	//--------------------------------------------
		// Channels
		mpMix_Channel[i] = new CMix_Channel(this, GetGUI());
		tint32 iTimerID = i + giTimerID_MixFirstChannel_ScrollPane;
		mpMix_Channel[i]->SetInfo(i, iTimerID);
		mpMix_Channel[i]->Init();
		mpPane->AddControl(mpMix_Channel[i]->GetPane(), ge::SPos(0, 0));
		mpMix_Channel[i]->GetPane()->SetVisible(false);
	}
	
	/*
	//--------------------------------------------
	// Busses
	for(tint32 i=0; i<giNumber_Of_Busses; i++) {
		mpMix_Buss[i] = new CMix_Buss(this, GetGUI());
		mpMix_Buss[i]->SetInfo(i);
		mpMix_Buss[i]->Init();
		tint32 iPos = (gMix.iCX*2) - ( giNumber_Of_Busses * 64) -6;
	//	iPos += i*64;
		iPos = 0;
		mpPane->AddControl(mpMix_Buss[i]->GetPane(), ge::SPos(iPos - 64, 0));
	}
	
	//--------------------------------------------
	// Master
	mpMix_Master = new CMix_Master(this, GetGUI());
	mpMix_Master->SetInfo(mpMix_Master_Out_Menu);
	mpMix_Master->Init();
	mpPane->AddControl(mpMix_Master->GetPane(), ge::SPos(gMix.iCX*2 -64, 0));
	*/



}


void CMix_Scroll::ConnectControls()
{
	// Channels
	for(tint32 i=0; i<giNumber_Of_Tracks; i++) {
		mpMix_Channel[i]->ConnectControls();
	}
	/*
	// Busses
	for(tint32 i=0; i<giNumber_Of_Busses; i++) {
		mpMix_Buss[i]->ConnectControls();
	}
	mpMix_Master->ConnectControls();
	*/
}

void CMix_Scroll::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
}

void CMix_Scroll::EventGeneric(ge::IControl* pControl, void* pEventData)
{
}


void CMix_Scroll::HandleMsg(SMsg* pMsg)
{

	switch(pMsg->iOpcode) 
	{
		case Msg_Stack_Tracks:{
		
			STrack_Stack sData = *(STrack_Stack*)pMsg->pDataIn;
			Stack_Tracks(sData);
			ge::SSize sPaneSize;
			mpPane->GetSize(sPaneSize);
			ge::SPos PosThis;
			mpPane->GetPos(PosThis);
			mpGUI->mpWindow->Redraw(ge::SRect(PosThis, sPaneSize));
			break;
		}

		case Msg_Resize: {
			//ArrangeTracks();
			break;
		}
		case Msg_Select_Track: {
			STrackData sData = *(STrackData*)pMsg->pDataIn;
			SelectTrack(sData.iTrack);
			break;
		}
	}

}

/*
void CMix_Scroll::SetTrackVisible(tint32 iTrackID, tbool bVisible)
{
//	mpMix_Channel->SetTrackVisible(iTrackID,bVisible);
	
}
*/
void CMix_Scroll::PreparePopups()
{

}

void CMix_Scroll::Stack_Tracks(STrack_Stack sData)
{

	// Hide all tracks
	for(tint32 i=0; i<giNumber_Of_Tracks; i++){
		mpMix_Channel[i]->GetPane()->SetVisible(false);
	}
	
	tint32 iNr_Tracks		= sData.iNr_Of_Tracks;
	tint32 iAccumWidth		= iNr_Tracks * gMix_Channel.iCX;
	tint32 iBeforeWidth		= 0;

	// Prepare to update pane areas
	ge::SScrollPos scrollpos;
	mpScrollPane->GetScrollPos(scrollpos);

	// Make sure we don't make inner-area size smaller than visible rect
	if (iAccumWidth < scrollpos.VisibleRect.iCX) {
		iBeforeWidth = scrollpos.VisibleRect.iCX - iAccumWidth;
	}
			
	// Channels
	tint32 iXPos = iBeforeWidth;
	for (tint iPos = 0; iPos < iNr_Tracks; iPos++) {
	
		tint iTrack	= sData.iTrack[iPos];
	
		ge::IPane* pCh = mpMix_Channel[iTrack]->GetPane();
		pCh->SetVisible(true);
		pCh->SetRelPos(ge::SPos(iXPos, 0));
		ge::SSize sizeCh;
		pCh->GetSize(sizeCh);
		iXPos += sizeCh.iCX;
	}

	// Update scroll-pane inner area
	scrollpos.AreaSize.iCX = iBeforeWidth + iAccumWidth;
	//!!! Todo! We must device a way of keeping the selected (and/or just added) track in visibility
	// For now we just scroll to the right-most pos
	tint32 iXOfs = iAccumWidth - scrollpos.VisibleRect.iCX;
	if (iXOfs < 0)
		iXOfs = 0;
	scrollpos.VisibleRect.iX = iXOfs;
	mpScrollPane->SetScrollPos(scrollpos);

	
}



void CMix_Scroll::UpdateGUI()
{
/*
	for(tint32 i=0; i<giNumber_Of_Tracks; i++) {
	
		tbool bTrackVisible = i < miNr_Of_Tracks;
		mpMix_Channel[i]->GetPane()->SetVisible(bTrackVisible);

	}	
*/
}

void CMix_Scroll::AddTrack()
{
/*
	// Test funcionality
	{
		miNr_Of_Tracks++;
		
		// Check for max nr tracks
		if(miNr_Of_Tracks >= giNumber_Of_Tracks) 
			miNr_Of_Tracks = giNumber_Of_Tracks-1;
		else
			// Only update GUI if a new track is added
			UpdateGUI();
	}
*/	
}

void CMix_Scroll::DeleteTrack()
{
/*
	// if a track is selected 
	if(miSelectedTrack != -1){
		mpMix_Channel[miSelectedTrack]->SelectTrack(false);
	}
		
	// Count down the number of allocated track	
	if(--miNr_Of_Tracks < 0) 
		miNr_Of_Tracks = 0;
	
	// Update gui
	UpdateGUI();
*/
}

void CMix_Scroll:: SelectTrack(tint32 iID)
{
	if(miSelectedTrack != -1)
		mpMix_Channel[miSelectedTrack]->SelectTrack(false);
		
	if(iID != -1)
		mpMix_Channel[iID]->SelectTrack(true);
	
	miSelectedTrack = iID;

}


void CMix_Scroll::Update_Tracks_Meters(void* pvoid_sMeters_All)
{
	for (tint32 iTrack = 0; iTrack < giNumber_Of_Tracks; iTrack++) {
		mpMix_Channel[iTrack]->UpdateMeters_Maybe(pvoid_sMeters_All);
	}
} // Update_Tracks_Meters


void CMix_Scroll::UpdateChannelName(tint32 iChannel, const std::string& sName)
{
	mpMix_Channel[iChannel]->UpdateChannelName( sName);
}