#include "KSOS.h"

CTrack_Player2::CTrack_Player2(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI), mppLine(giNumber_Of_Grid_Lines + 1)
{
	mpPane = NULL;

	miNbOfVisibleTracks = 0;
	miAccumTrackHeight = 0;


}

CTrack_Player2::~CTrack_Player2()
{
}

void CTrack_Player2::SetInfo(CTrack_Player* CTrack_Player)
{

	mpCTrack_Player = CTrack_Player;
//	mfTempo			=	60;
	SetSignature(giSignature4_4);
	miLast_Grid_Line_Allocated		=	0;
	

	
	gpDSPEngine->SetRegionCallback(dynamic_cast<IRegionCallback*>(this));
	
}

void CTrack_Player2::Init()
{


	mpScrollPane = ge::IScrollPane::Create();
	mpScrollPane->Init(ge::IControl::giNoID, giTimerID_Player2_ScrollPane);
	mpPane = dynamic_cast<ge::IPane*>(mpScrollPane);
	mpPane->SetSize(gTrack_Scroll_Editor);
	mpPane->SetBackgroundColour(ge::SRGB(230,230,230));
	

	msLineColor[giColor_1] = ge::SRGB(83,124,214);
	msLineColor[giColor_2] = ge::SRGB(125,153,217);
	msLineColor[giColor_4] = ge::SRGB(125,153,217);
	msLineColor[giColor_8] = ge::SRGB(188,214,254);
	msLineColor[giColor_16] = ge::SRGB(188,214,254);
	msLineColor[giColor_32] = ge::SRGB(188,214,254);
	
	Allocate_Grid_Lines();
	
	Update_Zoom();
	Draw_Grid();
	
	tint32	iTrack_PosY		= 0;
	tint32	iTrack_SizeY	= gTrack_Info_Small.iCY - 1;

	tint i;
	//--------------------------------------------
	for( i=0; i<giNumber_Of_Tracks; i++) {

		// Create Tracks
		mppTrack[i] = new CTrack(this, GetGUI());
		mppTrack[i]->SetInfo(i, iTrack_SizeY, this );
		mppTrack[i]->Init();
		mpPane->AddControl(mppTrack[i]->GetPane(), ge::SPos(0, iTrack_PosY));
		mppTrack[i]->GetPane()->SetVisible(false);
		
		iTrack_PosY			+= iTrack_SizeY + 1;
	}

	
	
	//muiPane_Duration_In_Samples = gTrack_Scroll_Editor.iCX * fSamplesPrPixel;
	
	mpEmpty_Rect				= CreateRect(ge::IControl::giNoID, 
												ge::SRect(ge::SPos(0, iTrack_PosY), 
												ge::SSize(gTrack_Scroll_Editor.iCX, 
												gTrack_Scroll_Editor.iCY-iTrack_PosY)), 
												ge::SRGB(204, 204, 204));

	mpBack_Track_Shaddow_Bmp	= CreateBitmap(ge::IControl::giNoID, IDB_Back_Track_Shaddow, ge::SPos(0, iTrack_PosY));
	mpBack_Track_Shaddow_Bmp->SetSize(ge::SSize(gTrack_Scroll_Editor.iCX,8));

	// Mouse-trap for activation of track 
	mpMouseTrap = ge::IMouseTrap::Create();
	// Only link into chain, but don't remove message (the third param = false)
	mpMouseTrap->EnableOneTrap(ge::MouseMove, true, false);
	mpPane->AddControl(mpMouseTrap, ge::SPos(0, 0));
	mpMouseTrap->PlaceOnTopOf(GetPane());
	mpMouseTrap->AddListener(this);
	
	// Play Head
	mpPlay_Line = CreateLine(ge::IControl::giNoID, ge::SPos(0, 0), ge::SPos(0, 0), ge::SRGB(0, 0, 0));
	miLast_Play_Line_Pos_X = -1;
	
}


void CTrack_Player2::ConnectControls()
{
	
}

void CTrack_Player2::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
}

void CTrack_Player2::EventGeneric(ge::IControl* pControl, void* pEventData)
{
	ge::SEventGeneric* pEvent = (ge::SEventGeneric*)pEventData;
	if (pEvent->eType = ge::giEventMouse) {
		// Event happened inside control
		if (pEvent->bInsideControlRect) {
			if (pEvent->iMsg == ge::MouseMove) {
				tint32 x = pEvent->pos.iX;
				tfloat64 fPixel_Pr_Sample	=	gpApplication->GetPixelPrSample();
				mpTrack_Top->SetCursorInSamples((x - gTrack_Side.iCX) * fPixel_Pr_Sample);

			}
		}
	}
	
}


void CTrack_Player2::HandleMsg(SMsg* pMsg)
{

	switch(pMsg->iOpcode) 
	{
		case Msg_Stack_Tracks:{
			STrack_Stack sData = *(STrack_Stack*)pMsg->pDataIn;
			Stack_Tracks(sData);
			SanitizeScrollPaneAreaSize_Y();
			// Update_Graphic();
			break;
		}
		case Msg_Resize: {
			SanitizeScrollPaneAreaSize_Y();
			Draw_Grid();
			Update_Graphic();
			break;
		}
		case Msg_Set_Grid: {
			Update_Zoom();
			Draw_Grid();
			Update_Graphic();
			break;
		}
		case MsgGUIData: {
			SGUIData sData	= *(SGUIData*)pMsg->pDataIn;
			switch( sData.iID){
				case giParamID_Zoom:
				{
					Update_Zoom();
				//	Snap_To_Grid();
					Draw_Grid();
					Update_Graphic();
					break;
				}
				case giParamID_Grid:
				{
					Draw_Grid();
					break;
				}
				default: break;
			}
			break;
		}
		case MsgGlobalData: {
			SGlobalData sData	= *(SGlobalData*)pMsg->pDataIn;
			switch( sData.iID){
				case giParamID_KS_Tempo:
				{
					
				//	mfTempo			= (tfloat32)sData.iVal/ 10.0f;
					Update_Zoom();
					Draw_Grid();
					Update_Graphic();
					break;
				}
				case giParamID_KS_Time_Signature:
				{
					SetSignature( sData.iVal);
					Update_Zoom();
					Draw_Grid();
					Update_Graphic();
					break;
				}
				
				default: break;
			}
			break;
		}
		case Msg_Delete_All_Regions:{
			STrackData sData = *(STrackData*)pMsg->pDataIn;
			mppTrack[sData.iTrack]->Delete_All_Regions();
			break;
		}
		case Msg_Track_MouseDrop:{
			tint32 iTrack = *(tint32*)pMsg->pDataIn;
			mppTrack[iTrack]->Handle_Mouse_Drop(ge::SPos(128,0));
			break;
		}
		case Msg_Track_MouseDrag:{
			tint32 iTrack = *(tint32*)pMsg->pDataIn;
			mppTrack[iTrack]->Handle_Drag_Region(ge::SPos(128,0));
			break;
		}
		 
		case Msg_Reset_Update_Play_Head_Size_Y:{
		
			// Put playhead in front
			mpPane->RemoveControl(mpPlay_Line);
			mpPane->AddControl(mpPlay_Line, ge::SPos(0, 0));


		//	tint32 iSize = *(tint32*)pMsg->pDataIn;
		//	Set_Play_Head_Size(iSize + 54);
			
			
			break;
		} 
	}
}

// Set the track size
void CTrack_Player2::Set_Track_Size(tint32 iTrack, tint32  iValue)
{ 
	mppTrack[iTrack]->Set_Track_SizeY(iValue);
}

void CTrack_Player2::Draw_Grid()
{
	gpApplication->GetGlobalParm(giParamID_Grid, giSectionGUI) ? Show_Grid():Hide_Grid();
}

void CTrack_Player2::Hide_Grid()
{
	for(tint32 i=0;  i< miLast_Grid_Line_Allocated; i++) 
		mppLine[i]-> SetVisible(false);
	
	miLast_Grid_Line_Allocated = 0;
}

void CTrack_Player2::Show_Grid()
{

	tfloat64 fPixelPrSample =  gpApplication->GetPixelPrSample();
	
	// Suppress calls to CPane::UpdatePosition
	ge::IPane* pPane = GetPane();
	ge::IScrollPane* pSP = NULL;
	if (pPane) {
		pSP = dynamic_cast<ge::IScrollPane*>(pPane);
	}
	if (pSP) {
		pSP->SuppressUpdatePositions(true);
	}
	
	
	ge::SSize sPaneSize;
	mpPane->GetSize(sPaneSize);

	tint32 iPixelPos	=	0;
	tint32 i			=	0;
	tint32 iSizeY		=	miAccumTrackHeight;

	ge::SPos posStart(iPixelPos, 0);
	ge::SPos posEnd(iPixelPos, iSizeY);

	tint32 iLine		=	0;
	tfloat32 fPosX		=	0.0f;
	tbool bFlipFlop		=	true;

	while ( posStart.iX < sPaneSize.iCX  && i < giNumber_Of_Grid_Lines-1) {

		switch(mpiPattern[iLine]){
			
			case giColor_1:{
				if (mbDraw_1 || bFlipFlop){
					Get_Grid_Line(i);
					mppLine[i]->SetColour(msLineColor[giColor_1]);
					mppLine[i]->SetLinePos(posStart, posEnd);
					i++;
				}
				bFlipFlop = !bFlipFlop;
				break;
			}
			case giColor_2:
			{ 
				if (mbDraw_2 ){
					Get_Grid_Line(i);
					mppLine[i]->SetColour(msLineColor[giColor_2]);
					mppLine[i]->SetLinePos(posStart, posEnd);
					i++;
				}
				
				break;
			}
			case giColor_4:
			{ 
				if (mbDraw_4){
					Get_Grid_Line(i);
					mppLine[i]->SetColour(msLineColor[giColor_4]);
					mppLine[i]->SetLinePos(posStart, posEnd);
					i++;
				}
				break;
			}
			case giColor_8:
			{ 
				if (mbDraw_8){
					Get_Grid_Line(i);
					mppLine[i]->SetColour(msLineColor[giColor_8]);
					mppLine[i]->SetLinePos(posStart, posEnd);
					i++;
				}
				break;
			}
			case giColor_16:
			{ 
				if (mbDraw_16){
					Get_Grid_Line(i);
					mppLine[i]->SetColour(msLineColor[giColor_16]);
					mppLine[i]->SetLinePos(posStart, posEnd);
					i++;
				}
			}
			case giColor_32:
			{ 
				if (mbDraw_32){
					Get_Grid_Line(i);
					mppLine[i]->SetColour(msLineColor[giColor_32]);
					mppLine[i]->SetLinePos(posStart, posEnd);
					i++;
				}
				break;
			}
		}
		// Count up line patterne and wrap arount
		if(++iLine >= mfSub_Beats) iLine = 0;
		// Move ahead
		fPosX		+=	mfSamples_Pr_32 * fPixelPrSample;
		posStart.iX =	(tint32)(fPosX);	 
		posEnd.iX	=	(tint32)(fPosX);
		
		
	}

	for(tint32 j=i;  j< miLast_Grid_Line_Allocated; j++) {

		mppLine[j]-> SetVisible(false);
	}

	miLast_Grid_Line_Allocated = i;

	// Allow UpdatePosition calls again
	if (pSP) {
		pSP->SuppressUpdatePositions(false);
	}
}

void CTrack_Player2::Allocate_Grid_Lines()
{
	// Suppress 8192 calls to CPane::UpdatePosition
	ge::IPane* pPane = GetPane();
	ge::IScrollPane* pSP = NULL;
	if (pPane) {
		pSP = dynamic_cast<ge::IScrollPane*>(pPane);
	}
	if (pSP) {
		pSP->SuppressUpdatePositions(true);
	}

	for(tint32 iLine =0; iLine < giNumber_Of_Grid_Lines; iLine++){
		mppLine[iLine] = CreateLine(ge::IControl::giNoID, ge::SPos(0,0), ge::SPos(0,0) , ge::SRGB(0, 0, 0));
		mppLine[iLine]->SetVisible(false);
	}

	// Allow UpdatePosition again
	if (pSP) {
		pSP->SuppressUpdatePositions(false);
	}
}

void CTrack_Player2::Get_Grid_Line( tint32 iLine)
{
	mppLine[iLine]->SetVisible(true);
}

void CTrack_Player2::Update_Zoom()
{

	
	mfSamples_Pr_32						=	gpApplication->GetSamples_Pr_32();
	tfloat64 iMin_Samples_Pr_Gridline	=	giMin_Pix_Pr_Gridline * gpApplication->GetSamplesPrPixel();
	
	tint32 iGrid = gpApplication->GetGlobalParm(giParamID_KS_Snap_To, giSectionGlobal);
	
	
	/*
	 SMsg Msg			=	Msg_Set_Grid;
	 SendMsgFromTop(&Msg);
	 }
	 */

	
	
	// Dont draw gridlines if they are to close to each other
	mbDraw_32	= mfSamples_Pr_32		>= iMin_Samples_Pr_Gridline? true: false;
	mbDraw_16	= mfSamples_Pr_32 * 2	>= iMin_Samples_Pr_Gridline? true: false;
	mbDraw_8	= mfSamples_Pr_32 * 4	>= iMin_Samples_Pr_Gridline? true: false;
	mbDraw_4	= mfSamples_Pr_32 * 8	>= iMin_Samples_Pr_Gridline? true: false;
	mbDraw_2	= mfSamples_Pr_32 * 16	>= iMin_Samples_Pr_Gridline? true: false;
	mbDraw_1	= mfSamples_Pr_32 * 32	>= iMin_Samples_Pr_Gridline? true: false;
	
	switch(iGrid){
		case 1:
			break;
		case 2:
			mbDraw_32	= false;
			break;
		case 4:
			mbDraw_32	= false;
			mbDraw_16	= false;
			break;
		case 8:
			mbDraw_32	= false;
			mbDraw_16	= false;
			mbDraw_8	= false;
			break;
		case 16:
			mbDraw_32	= false;
			mbDraw_16	= false;
			mbDraw_8	= false;
			mbDraw_4	= false;
			break;
		case 32:
			mbDraw_32	= false;
			mbDraw_16	= false;
			mbDraw_8	= false;
			mbDraw_4	= false;
			mbDraw_2	= false;
			break;
	}
		
	
	if(mbDraw_32)		mfGrid_In_Samples =	 mfSamples_Pr_32;
	else if(mbDraw_16)	mfGrid_In_Samples =  mfSamples_Pr_32 * 2.0f;
	else if(mbDraw_8)	mfGrid_In_Samples =  mfSamples_Pr_32 * 4.0f;
	else if(mbDraw_4)	mfGrid_In_Samples =  mfSamples_Pr_32 * 8.0f;
	else if(mbDraw_2)	mfGrid_In_Samples =  mfSamples_Pr_32 * 16.0f;
	else if(mbDraw_1)	mfGrid_In_Samples =  mfSamples_Pr_32 * 32.0f; 
	
	gpApplication->SetGridLinesInSamples(mfGrid_In_Samples);
}

void CTrack_Player2::Stack_Tracks(STrack_Stack sData)
{
	
	// Hide all tracks
	for(tint32 i=0; i<giNumber_Of_Tracks; i++){
		mppTrack[i]->GetPane()->SetVisible(false);
	}
	
	ge::SSize sPaneSize;
	mpPane->GetSize(sPaneSize);

	tint32 iTrackPosY		= 0;
	tint32 iNr_Tracks		= sData.iNr_Of_Tracks;
	
	for (tint32 iPos=0; iPos<iNr_Tracks; iPos++) {
	
		tint iTrack	= sData.iTrack[iPos];
		mppTrack[iTrack]->SetPos( iTrackPosY);
		mppTrack[iTrack]->GetPane()->SetVisible(true);
		tbool bBig = (gpApplication->GetGlobalParm(giParam_Track_Info_SizeY, iTrack+giSection_First_Track) != 0);
		tint32 iCY = (bBig) ? giTrack_Size_Big : giTrack_Size_Small;
		iTrackPosY += iCY;

	}
	
	
	miAccumTrackHeight = iTrackPosY;
	GetScrollPane()->SetMinSizeOfSubPane(ge::SSize(0, miAccumTrackHeight));
	
	Update_Empty_Array( sPaneSize);
	
	SMsg Msg			=	Msg_Reset_Update_Play_Head_Size_Y;
	Msg.pDataIn			=	(void*)&miAccumTrackHeight;
	SendMsgFromTop(&Msg);
	// Update grid line size
	Show_Grid();

}

void CTrack_Player2::Update_Graphic()
{
	ge::SSize sPaneSize;
	mpPane->GetSize(sPaneSize);

	Update_Empty_Array( sPaneSize);
	SanitizeScrollPaneAreaSize_X();


}

void CTrack_Player2::Update_Empty_Array(ge::SSize sPaneSize)
{
	ge::SPos pos	= ge::SPos(0,miAccumTrackHeight);
	ge::SSize size	= ge::SSize(sPaneSize.iCX,sPaneSize.iCY - miAccumTrackHeight);	
	ge::SRect rect	= ge::SRect(pos,size);
	mpEmpty_Rect->SetRect(rect);
	mpBack_Track_Shaddow_Bmp->SetPos(pos);

}

void CTrack_Player2::SetSignature(tint32 iSignature)
{
	miSignature = iSignature;
	
	switch(iSignature){
		case giSignature2_4: {
		mfBeats			=	(tint32)giSignature2_4_Beats;
		mfSub_Beats		=	(tint32)giSignature2_4_Sub_Beats;
		mpiPattern		=	(tint32*)gpiSignature2_4;
		break;
		}
		case giSignature3_4: {
		mfBeats			=	(tint32)giSignature3_4_Beats;
		mfSub_Beats		=	(tint32)giSignature3_4_Sub_Beats;
		mpiPattern		=	(tint32*)gpiSignature3_4;
		break;
		}
		case giSignature4_4: {
		mfBeats			=	(tint32)giSignature4_4_Beats;
		mfSub_Beats		=	(tint32)giSignature4_4_Sub_Beats;
		mpiPattern		=	(tint32*)gpiSignature4_4;
		break;
		}
		case giSignature6_8: {
		mfBeats			=	(tint32)giSignature6_8_Beats;
		mfSub_Beats		=	(tint32)giSignature6_8_Sub_Beats;
		mpiPattern		=	(tint32*)gpiSignature6_8;
		break;
		}
		case giSignature9_8: {
		mfBeats			=	(tint32)giSignature9_8_Beats;
		mfSub_Beats		=	(tint32)giSignature9_8_Sub_Beats;
		mpiPattern		=	(tint32*)gpiSignature9_8;
		break;
		}
		case giSignature12_8: {
		mfBeats			=	(tint32)giSignature12_8_Beats;
		mfSub_Beats		=	(tint32)giSignature12_8_Sub_Beats;
		mpiPattern		=	(tint32*)gpiSignature12_8;
		break;
		}
	}
}


void CTrack_Player2::SanitizeScrollPaneAreaSize_Y()
{
	ge::SScrollPos sp;
	GetScrollPane()->GetScrollPos(sp);
	tint32 iNewHeight;
	if (miAccumTrackHeight >= sp.VisibleRect.iCY)
		iNewHeight = miAccumTrackHeight;
	else
		iNewHeight = sp.VisibleRect.iCY;

	if (iNewHeight != sp.AreaSize.iCY) {
		sp.AreaSize.iCY = iNewHeight;
		GetScrollPane()->SetScrollPos(sp);
	}
}


void CTrack_Player2::SanitizeScrollPaneAreaSize_X()
{

	tfloat64 fPixelPrSample			=	gpApplication->GetPixelPrSample();
	tfloat64  fSession_End_Sample	=	gpDSPEngine->Get_Session_End_Sample();
	tuint32	uiEndPixel				=	fSession_End_Sample * fPixelPrSample;
	
	ge::SScrollPos sp;
	GetScrollPane()->GetScrollPos(sp);
	
//	tint32 iX = sp.AreaSize.iCX;
	

	
	tint32 iPixel_Size_New;//	= muiPane_Duration_In_Samples *  fPixelPrSample;
	iPixel_Size_New = uiEndPixel + 128;
	
	
	if (iPixel_Size_New < 4000)
		iPixel_Size_New = 4000;

	if( sp.AreaSize.iCX != iPixel_Size_New){

		sp.AreaSize.iCX = iPixel_Size_New; 

		
		
		mpCTrack_Player->Set_Scrol_Pane_Size_X(iPixel_Size_New);
		mpEmpty_Rect->SetSize(ge::SSize(iPixel_Size_New,sp.AreaSize.iCX-8));
		mpBack_Track_Shaddow_Bmp->SetSize(ge::SSize(iPixel_Size_New,8));
		
		for(tint32 i = 0; i < giNumber_Of_Tracks; i++)
			mppTrack[i]->SetSizeX( iPixel_Size_New);
	}
}

void CTrack_Player2::Deselect_Regions()
{ 
	//!!! Always return false
	//tbool bTest =  -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	gpDSPEngine->Deselect_All_Tracks();
	for(tint32 i = 0; i < giNumber_Of_Tracks; i++){
		mppTrack[i]->Set_Selection_Size_And_Pos();
	}
}


void CTrack_Player2::RedrawSelection_For_Track(tint32 iTrack)
{
	mppTrack[iTrack]->Set_Selection_Size_And_Pos();
} // RedrawSelection_For_Track


void CTrack_Player2::InsertRegion(tuint32 uiRegionID, tuint32 uiTrack, tuint64 uiSample_Pos, tuint64 uiSample_Start, tuint64 uiSample_Duration, const std::string& sName)
{

	SanitizeScrollPaneAreaSize_X();
	
	mppTrack[uiTrack]->InsertRegion( uiRegionID,  uiSample_Pos, uiSample_Start,  uiSample_Duration, sName);
	
	
	
	
}

void CTrack_Player2::SelectRegion(tuint32 uiRegionID, tuint32 uiTrack)
{
	mppTrack[uiTrack]->SelectRegion( uiRegionID);
}
void CTrack_Player2::Refresh_Region_GUI(tuint32 uiID, tuint32 uiTrack)
{
	mppTrack[uiTrack]->Refresh_Region_GUI( uiID);
	SMsg Msg(Msg_Deselect_All);
	SendMsgFromTop(&Msg);
}


void CTrack_Player2::DeleteRegion(tuint32 uiID, tuint32 uiTrack)
{
	mppTrack[uiTrack]->DeleteRegion(uiID);
}

void CTrack_Player2::SetSelection( tuint64 uiSelection_Start_Sample, tuint64 uiSelection_End_Sample)
{
	msSelection_Info.uiSelection_Start_Sample		=	uiSelection_Start_Sample;
	msSelection_Info.uiSelection_End_Sample			=	uiSelection_End_Sample;
}

void CTrack_Player2::SetTrackTopPtr(CTrack_Top* pTrack_Top)
{
	mpTrack_Top = pTrack_Top;
	
	for(tint32 i = 0; i < giNumber_Of_Tracks; i++){
		mppTrack[i]->SetTrackTopPtr(pTrack_Top);
	}
	

}

void CTrack_Player2::JumpToPlayhead()
{
	SMsg Msg(Msg_Go_To_Playhead);
	SendMsgFromTop(&Msg);
}

void CTrack_Player2::Set_LinePos(tint64 iSample_Pos)
{
	// Lasse, dynamic_cast is slow on Windows!
	tint32 iPixel_Pos	=	(tint32)(gpApplication->GetPixelPrSample() * iSample_Pos);


	if(miLast_Play_Line_Pos_X	!= iPixel_Pos){
		miLast_Play_Line_Pos_X = iPixel_Pos;
		//Move line
		mpPlay_Line->SetLinePos(ge::SPos(iPixel_Pos,0), ge::SPos(iPixel_Pos, miAccumTrackHeight));
		
		
		// Refresh
		ge::SSize Size;
		ge::SPos Pos;
		GetPane()->GetPos(Pos);
		tint32 XDistMoved = iPixel_Pos - miLast_Play_Line_Pos_X;
		
		// Moving from left to right
		if( XDistMoved >= 0){
			Pos.iX += miLast_Play_Line_Pos_X;
			Size = ge::SSize(XDistMoved, miAccumTrackHeight);
		}
		else{
			XDistMoved	=	abs(XDistMoved) ;
			Pos.iX	+=	miLast_Play_Line_Pos_X - XDistMoved;
			Size	=	ge::SSize( XDistMoved, miAccumTrackHeight);
		
		}
		mpPane->Redraw(ge::SRect(Pos, Size));
	}
	
//	Redraw_Pane_Rect();

}
