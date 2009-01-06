#include "KSOS.h"

CTrack_Setup::CTrack_Setup(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	mpPane = NULL;

	// Lasse, added 2008-04-16
	miSelectedTrack = -1;
	miMouseOverTrack = -1;
	// .. Lasse
}

CTrack_Setup::~CTrack_Setup()
{
}

void CTrack_Setup::SetInfo()
{

}

void CTrack_Setup::Init()
{
	mpScrollPane = ge::IScrollPane::Create();
	mpScrollPane->Init(ge::IControl::giNoID, giTimerID_TrackSide_ScrollPane);
	mpPane = dynamic_cast<ge::IPane*>(mpScrollPane);
	mpPane->SetSize(ge::SSize(gTrack_Setup.iCX, gTrack_Player.iCY - giTime_Pos_SizeY));
	mpPane->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteY);
	mpPane->SetBackgroundColour(ge::SRGB(204,204,204));
	

	//--------------------------------------------
	for(tint32 i=0; i<giNumber_Of_Tracks; i++) {

		//Default Track size
		tint32	iTrack_PosY			= i * gTrack_Info_Small.iCY;
		// Create Tracks
		mppTrack_Info[i] = new CTrack_Info(this, GetGUI());
		tint32 iTimerID = i + giTimerID_InfoFirstChannel_ScrollPane;
		mppTrack_Info[i]->SetInfo(i,  gTrack_Info_Small.iCY, false, iTimerID);
		mppTrack_Info[i]->Init();
		mpPane->AddControl(mppTrack_Info[i]->GetPane(), ge::SPos(0, iTrack_PosY));		

	}
	
	mpBack_Track_Shaddow_Bmp = CreateBitmap(ge::IControl::giNoID, IDB_Back_Track_Shaddow, ge::SPos(0, 0));
	mpBack_Track_Shaddow_Bmp->SetSize(ge::SSize(gTrack_Setup.iCX,8));

}


void CTrack_Setup::ConnectControls()
{
	//--------------------------------------------
	for(tint32 i=0; i<giNumber_Of_Tracks; i++) {

		mppTrack_Info[i]->ConnectControls();

	}
}

void CTrack_Setup::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
}

void CTrack_Setup::EventGeneric(ge::IControl* pControl, void* pEventData)
{
}


ge::IScrollPane* CTrack_Setup::GetScrollPane()
{
	return mpScrollPane;
	
}


void CTrack_Setup::HandleMsg(SMsg* pMsg)
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
		
		case Msg_Select_Track: {
			STrackData sData = *(STrackData*)pMsg->pDataIn;
			SelectTrack(sData.iTrack);
			break;
		}
	}
	
}


void CTrack_Setup::Set_Track_Info_Visible(tint32 iTrackID, tbool bVisible)
{	
	mppTrack_Info[iTrackID]->SetVisible(bVisible);
}


void CTrack_Setup::UpdateTrackMeters_Maybe(tint32 iTrack, void* mpvoid_sMeters_All)
{
	mppTrack_Info[iTrack]->UpdateTrackMeters_Maybe(mpvoid_sMeters_All);
} // UpdateTrackMeters_Maybe


void CTrack_Setup::Stack_Tracks(STrack_Stack sData)
{
	// Hide all tracks
	for(tint32 i=0; i<giNumber_Of_Tracks; i++){
		mppTrack_Info[i]->SetVisible(false);
	}
	
	tint32 iTrackPosY	= 0;
	tint32 iNr_Tracks	=	sData.iNr_Of_Tracks;
	//--------------------------------------------
	for(tint32 iPos=0; iPos< iNr_Tracks; iPos++) {
	
		tint iTrack	= sData.iTrack[iPos];
		
		tint32 iSize = gpApplication->GetGlobalParm(giParam_Track_Info_SizeY, iTrack+giSection_First_Track);
		mppTrack_Info[iTrack]->Set_Track_Size( iSize);
		
		mppTrack_Info[iTrack]->SetPos( iTrackPosY);
		mppTrack_Info[iTrack]->SetVisible(true);
		iTrackPosY += mppTrack_Info[iTrack]->GetSizeY() ;	
		
	}
	mpBack_Track_Shaddow_Bmp->SetPos(ge::SPos(0,iTrackPosY));
	
	ge::SScrollPos scrollpos;
	GetScrollPane()->GetScrollPos(scrollpos);
	if (iTrackPosY >= scrollpos.VisibleRect.iCY)
		scrollpos.AreaSize.iCY = iTrackPosY;
	else
		scrollpos.AreaSize.iCY = scrollpos.VisibleRect.iCY;

	
}

void CTrack_Setup::SelectTrack(tint32 iID)
{
	// if any track is selected 
	if(miSelectedTrack != -1){
		// remove selection
		mppTrack_Info[miSelectedTrack]->Track_Selected(false);
		
	}
	// if iID is a valid track
	if(iID != -1){
		mppTrack_Info[iID]->Track_Selected(true);
	}
	miSelectedTrack = iID;
	
}


void CTrack_Setup::Set_Track_Size(tint32 iTrack, tint32 iSize)
{
	mppTrack_Info[iTrack]->Set_Track_Size( iSize);

	
}

void CTrack_Setup::UpdateChannelName(tint32 iTrack, const std::string& sName)
{
	mppTrack_Info[iTrack]->UpdateChannelName( sName);
}





