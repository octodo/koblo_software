#include "KSOS.h"

CTrack::CTrack(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI), mppRegion_GUI(giNumber_Of_Regions)
{
	mpPane = NULL;

//!!!	mpMouseTrap		=	NULL;
	mbClicked		=	false;
	mbMouseOver		=	false;
	mbMouseCaptured	=	false;
	miLastSelectionEndPos	=	0;
	mbExtendSelection	=	false;

//	miLastRegion_GUI	=	0;

}

CTrack::~CTrack()
{
/*!!!
	if (mpMouseTrap) {
		delete mpMouseTrap;
		mpMouseTrap = NULL;
	}
*/
}

void CTrack::SetInfo(tint32 iTrackID, tint32 iTrack_SizeY,CTrack_Player2* pTrack_Player2)
{
	miTrackID			=	iTrackID;
	mTrackSize			=	ge::SSize(gTrack_Scroll_Editor.iCX ,iTrack_SizeY);
	mpTrack_Player2		=	pTrack_Player2;
	miDrag_Region_Size	=	-1;	
	mpKSPlugIn = dynamic_cast<CKSPlugIn*>(GetPlugIn());
}

void CTrack::Init()
{

	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(mTrackSize);
	mpPane->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteX );


	mpControl = ge::ICustomControl::Create();
	mpControl->Init(ge::IControl::giNoID, dynamic_cast<ge::ICustomControlCallback*>(this));
	mpControl->SetSize(mTrackSize);
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpControl), ge::SPos(0, 0));

//	mpInvert = CreateInvert(ge::IControl::giNoID, ge::SRect(ge::SPos(0, 0), ge::SSize(mTrackSize)));
//	mpInvert->SetVisible(false);

	mpTrack_Buttom_Line =CreateLine(ge::IControl::giNoID, ge::SPos(0, mTrackSize.iCY), ge::SPos(mTrackSize.iCX, mTrackSize.iCY), ge::SRGB(127, 127, 127));
	mpTrack_Buttom_Line->SetVisible(true);
	
	
	pmBmp_Select_Big =CreateBitmap(ge::IControl::giNoID, IDB_Back_Track_Selection_Big, ge::SPos(0, 0));
	pmBmp_Select_Big->SetSize(ge::SSize(200,giTrack_Size_Big-1));
	pmBmp_Select_Big->SetVisible(false);
	
	pmBmp_Select_Small =CreateBitmap(ge::IControl::giNoID, IDB_Back_Track_Selection_Small, ge::SPos(0, 0));
	pmBmp_Select_Small->SetSize(ge::SSize(200,giTrack_Size_Small-1));
	pmBmp_Select_Small->SetVisible(false);
	
	
	mbMouseCaptured = false;
	
	
}


void CTrack::ConnectControls()
{
}

void CTrack::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
}

void CTrack::EventGeneric(ge::IControl* pControl, void* pEventData)
{

}

void CTrack::OnDraw(const ge::SRect &rUpdate)
{
}

tbool CTrack::OnMouse(ge::EMouseMsg MouseMsg, const ge::SPos& Pos)
{
	ge::SPos PosThis;
	ge::SSize SizeThis;
	mpControl->GetPos(PosThis);
	mpControl->GetSize(SizeThis);
	ge::SRect RectThis(PosThis, SizeThis);
	
	if(mbMouseCaptured){ 
		// --- Lasse asks: Isn't this dead code? ---

		if(MouseMsg == ge::LeftButtonUp ){
			mpPane->GetParentWindow()->ReleaseMouseFocus();
			mbMouseCaptured = false;
		}
		else if(MouseMsg == ge::MouseMove){
			

			tint64 iSample = (tint64)((Pos.iX - gTrack_Side.iCX) *mpKSPlugIn->GetSamplesPrPixel());
			if(iSample < 0) iSample = 0;
			mpTrack_Top->SetCursorInSamples(iSample);
			
			SetTrackSelection( Pos, -1, giSelect_On_Track);
			mbExtendSelection = true;

		}
		// .. Lasse - end of dead code
	}
	else if(MouseMsg == ge::MouseMove){

		tfloat64 fSamplesPrPixel	=	mpKSPlugIn->GetSamplesPrPixel();
			
		ge::SPos PosThis;
		mpControl->GetPos(PosThis);
		tint64 uiSamplesScrolled = PosThis.iX;

		tint64 iSample = (tint64)((Pos.iX - uiSamplesScrolled) *fSamplesPrPixel);
			
			
		if(iSample < 0) iSample = 0;
		mpTrack_Top->SetCursorInSamples(iSample);
	
	}
	if (!RectThis.Inside(Pos)) return false;
	
	switch(MouseMsg){
		//----------------------------------------
		// Mouse Down
		//----------------------------------------
		case ge::LeftButtonDown: {
			
			/*
			 if (!ge::IWindow::ShiftPressed()) {
			 
			 mpTrack_Player2->Get_Mouse_Down_Pos_X();
			 */
			
			mpPane->GetParentWindow()->GetMouseFocus(mpPane);
			mbMouseCaptured = true;
			
			
			if (!ge::IWindow::ShiftPressed()) {
				mpTrack_Player2->Deselect_Regions();
				mpTrack_Player2->Set_Mouse_Down_Pos_X(Pos.iX);
			}
			else if(Find_Tool() == giTool_Select)  {
				
				ge::SPos sPosNew	=	Pos;
				sPosNew.iX			=	mpTrack_Player2->Get_Mouse_Up_Pos_X();
				SetTrackSelection( sPosNew, -1, giSelect_On_Track);
				
			}

			miSelectionExtendsInYDirection = 0;

			break;
		}
		case ge::MouseDrop:
			Handle_Mouse_Drop(Pos);
			break;
		case ge::MouseDrag:
			Handle_Drag_Region(Pos);
			break;
			
		case ge::LeftButtonUp:{
			if (!ge::IWindow::ShiftPressed())
				mpTrack_Player2->Set_Mouse_Up_Pos_X(Pos.iX);
			else if(mbExtendSelection) {
				mpTrack_Player2->Set_Mouse_Up_Pos_X(Pos.iX);
			}
			mbExtendSelection = false;
			break;
		}

		
	}

	return false;
}

tbool CTrack::OnKeyDown(ge::EKey Key)
{
	return false;
}

void CTrack::OnTimer()
{
}

void CTrack::OnValueChange()
{
}

void CTrack::SetSize(const ge::SSize& Size)
{
}

void CTrack::SetPos(const ge::SPos& Pos)
{
}


void CTrack::SetTrackSelection( const ge::SPos& Pos, tint32 iRegionID, tint32 iSelection_Type  )
{
	// Set selection in samples
	tfloat64 fSamples_Pr_Pixel	=	mpKSPlugIn->GetSamplesPrPixel();
	tuint64 uiStart_Sample;
	tuint64 uiDuration;
	tuint64 uiSampleEnd;
	
	ge::SPos panePos;
	mpPane->GetPos(panePos);
	
	tint32 iMouse_Down_Pos	=	mpTrack_Player2->Get_Mouse_Down_Pos_X();
	tint32 iStart_Pos		=	iMouse_Down_Pos - panePos.iX;

	tint32 iRegionSizeX = Pos.iX - iMouse_Down_Pos;
	//-----------------------
	// LEFT <<< RIGHT
	if( iRegionSizeX < 0){
	
		uiSampleEnd		= Float2Int((tfloat32)(iStart_Pos) * fSamples_Pr_Pixel);
		
		tint32 iMousePos = Pos.iX - panePos.iX;
		if(iMousePos < 0 ) iMousePos = 0;
		uiStart_Sample	= (tfloat32)( iMousePos ) * fSamples_Pr_Pixel;
	
	}
	//-----------------------
	// LEFT >>> RIGHT
	else{
		uiStart_Sample		=	Float2Int(fSamples_Pr_Pixel * (tfloat64)iStart_Pos);
		uiDuration			=	Float2Int(fSamples_Pr_Pixel * (tfloat64)iRegionSizeX);
		uiSampleEnd			=	uiStart_Sample + uiDuration - 1;

	}
	uiStart_Sample		=	mpKSPlugIn->SnapToGridStart(uiStart_Sample);
	uiSampleEnd			=	mpKSPlugIn->SnapToGridEnd(uiSampleEnd);
	uiDuration			=	uiSampleEnd - uiStart_Sample + 1 ;
	
	// Store selection data in DSP engine
	gpDSPEngine->SetTrackSelection(miTrackID, uiStart_Sample, uiDuration, iSelection_Type , iRegionID);
	
	// Force drawing of selection
	Set_Selection_Size_And_Pos();


	// Extending selection on this track
	mpKSPlugIn->SelectTrack(miTrackID);

	// Make sure all other track with selections reflect this one
	STrackSelectionInfo sBlueSel = gpDSPEngine->GetTrackSelection(miTrackID);
	for (tint32 i = 0; i < giNumber_Of_Tracks; i++) {
		if (i != miTrackID) {
			STrackSelectionInfo sSel = gpDSPEngine->GetTrackSelection(i);
			if (sSel.uiSelection_Type != giSelect_Off) {
				gpDSPEngine->SetTrackSelection(i, sBlueSel.uiSelection_Pos, sBlueSel.uiSelection_Duration, giSelect_On_Track);
				// Post message
				SMsg msg(Msg_Update_Track_Selection, (void*)i);
				SendMsgFromTop(&msg);
			}
		}
	}
} // SetTrackSelection


void CTrack::Set_Selection_Size_And_Pos()
{
	if(!mpPane->IsVisible()) return;
	STrackSelectionInfo sInfo = gpDSPEngine->GetTrackSelection(miTrackID);
	
	if ((sInfo.uiSelection_Type == giSelect_Off) || (sInfo.uiSelection_Duration == 0)) {
		Hide_Selection();
		// Maybe update "blue track"
		tint32 iBlueTrack = mpKSPlugIn->Get_Selected_Track();
		if (iBlueTrack == miTrackID) {
			// We must change blue track
			iBlueTrack = -1;
			for (tint32 i = 0; (iBlueTrack < 0) && (i < giNumber_Of_Tracks); i++) {
				STrackSelectionInfo sPrevSel = gpDSPEngine->GetTrackSelection(i);
				if (sPrevSel.uiSelection_Type != giSelect_Off) {
					// This one's a cancidate! Make it blue
					iBlueTrack = i;
				}
			}
			// Set new (or none) blue track
			mpKSPlugIn->SelectTrack(iBlueTrack);
		}
	}
	else{

		tfloat64 fPixel_Pr_Sample	=	mpKSPlugIn->GetPixelPrSample();
		
		tuint64 uiStart_Sample			=	sInfo.uiSelection_Pos;
		tuint64 uiSelection_Duration	=	sInfo.uiSelection_Duration;
		tuint64 uiEnd_Sample			=	uiStart_Sample + uiSelection_Duration - 1 ;
		
		mpTrack_Top->SetStartSamples(uiStart_Sample);
		mpTrack_Top->SetEndSamples(uiEnd_Sample);

		tfloat64 fTest ;
		tuint32 uiStart_Pixel			=	fTest = (tfloat64)uiStart_Sample	* fPixel_Pr_Sample;
		tuint32 uiEnd_Pixel				=	fTest = (tfloat64)uiEnd_Sample		* fPixel_Pr_Sample;
		
		if(  miLastSelectionEndPos > uiEnd_Pixel){	
			Redraw_Pane_Rect();
		}
		miLastSelectionEndPos = uiEnd_Pixel;
		
		tuint32 uiSelectionSize	=	uiEnd_Pixel - uiStart_Pixel;
		
		if(miTrack_Size_Y){
			pmBmp_Select_Big->SetPos(ge::SPos(		uiStart_Pixel,		0));
			pmBmp_Select_Big->SetSize(ge::SSize(	uiSelectionSize ,	giTrack_Size_Big-1));

		}
		else{
			pmBmp_Select_Small->SetPos(ge::SPos(	uiStart_Pixel,		0));
			pmBmp_Select_Small->SetSize(ge::SSize(	uiSelectionSize,	giTrack_Size_Small-1));
		}
	
		Show_Selection();

		// If no previous "blue track" then let it be this one
		tint32 iPrevBlueTrack = mpKSPlugIn->Get_Selected_Track();
		if (iPrevBlueTrack < 0) {
			mpKSPlugIn->SelectTrack(miTrackID);
		}
	}
}

void CTrack::Show_Selection()
{
	if(miTrack_Size_Y){
		pmBmp_Select_Small->SetVisible(false);
		pmBmp_Select_Big->SetVisible(true);
	}
	else{
		pmBmp_Select_Small->SetVisible(true);
		pmBmp_Select_Big->SetVisible(false);
	}
	//Redraw_Pane_Rect();
}

void CTrack::Hide_Selection()
{
	pmBmp_Select_Small->SetVisible(false);
	pmBmp_Select_Big->SetVisible(false);

}

void CTrack::Handle_Drag_Region(ge::SPos Pos)
{
	tfloat64 fPixel_Pr_Sample	=	mpKSPlugIn->GetPixelPrSample();
	
		
	if(miDrag_Region_Size == -1){

		ge::IWindow::SDragAndDropInfo Info;
		GetWindow()->GetDragAndDropInfo(Info);
		
		if (Info.pOrigin->GetID() == giCtrl_File_List) {
			// Drag and drop operation from file list. Add region
			// File name is in Info.psz
			miDrag_Region_Size = gpDSPEngine->GetRegionSize( Info.psz, 0, -1);
			tint32 iRegionSizeX = (tfloat32)miDrag_Region_Size * fPixel_Pr_Sample;
			pmBmp_Select_Small->SetSize(ge::SSize( iRegionSizeX,giTrack_Size_Small-1));
			pmBmp_Select_Big->SetSize(ge::SSize( iRegionSizeX,giTrack_Size_Big-1));
			Show_Selection();
		}
		else if(Find_Tool() == giTool_Hand) {
			// Drag and drop operation from region (region was moved)
			tuint32 uiID = strtoul(Info.psz, NULL, 0);
			miDrag_Region_Size = gpDSPEngine->GetRegionSize( uiID);
			tint32 iRegionSizeX = (tfloat32)miDrag_Region_Size * fPixel_Pr_Sample;
			pmBmp_Select_Small->SetSize(ge::SSize( iRegionSizeX,giTrack_Size_Small-1));
			pmBmp_Select_Big->SetSize(ge::SSize( iRegionSizeX,giTrack_Size_Big-1));
			Show_Selection();
		}

	}
	mpTrack_Player2->SetMouse_Over_Track(miTrackID);
	SMsg Msg(Msg_Reset_Drag_Region_Size);
	SendMsgFromTop(&Msg);					// Can be optimized
		

	// Get Track pos
	ge::SPos panePos;
	mpPane->GetPos(panePos);
	// Find pos on track
	Pos.iX = Pos.iX - panePos.iX;
	Pos.iY	=	0;

	tuint64 uiSamplePos = (tfloat32)Pos.iX * mpKSPlugIn->GetSamplesPrPixel();
	
	
	tfloat64 fStart = (tfloat64)mpKSPlugIn->SnapToGrid(uiSamplePos);
	Pos.iX			=	fStart * fPixel_Pr_Sample;
	
	miTrack_Size_Y ? pmBmp_Select_Big->SetPos(Pos): pmBmp_Select_Small->SetPos(Pos);
	Redraw_Pane_Rect();
	
	/*
	Pos.iX		=	(tfloat32)mpKSPlugIn->SnapToGrid(uiSamplePos) * fPixel_Pr_Sample;

	miTrack_Size_Y ? pmBmp_Select_Big->SetPos(Pos): pmBmp_Select_Small->SetPos(Pos);
	Redraw_Pane_Rect();
	
	// Updat topbar display
	tfloat64 fStart = Pos.iX / fPixel_Pr_Sample;
	*/
	
	//fStart	=	mpKSPlugIn->SnapToGrid(fStart);
	mpTrack_Top->SetStartSamples(fStart);
	mpTrack_Top->SetEndSamples(miDrag_Region_Size + fStart);
	mpTrack_Top->SetCursorInSamples(fStart);
	
}


void CTrack::Handle_Mouse_Drop(ge::SPos Pos)
{
	mpTrack_Player2->Deselect_Regions();
	
	ge::IWindow::SDragAndDropInfo Info;
	GetWindow()->GetDragAndDropInfo(Info);
	// Get Track pos
	ge::SPos panePos;
	mpPane->GetPos(panePos);
	// Find pixel pos on track
	tint32 iPixel_Pos = Pos.iX - panePos.iX;
	

	tfloat64 fSamples_Pr_Pixel	=	mpKSPlugIn->GetSamplesPrPixel();
	tuint64 uiSamplePos			=	mpKSPlugIn->SnapToGrid(iPixel_Pos * fSamples_Pr_Pixel);

	if (Info.pOrigin->GetID() == giCtrl_File_List) {
		// Drag and drop operation from file list. Add region
		// List item *display name* is in Info.psz
		gpDSPEngine->CreateRegion(Info.psz, miTrackID, uiSamplePos, 0);
	}
	else if(Find_Tool() == giTool_Hand) {
		// Drag and drop operation from region (region was moved)
		tuint32 uiID = strtoul(Info.psz, NULL, 0);
		gpDSPEngine->MoveRegion(uiID, miTrackID, uiSamplePos);
	}
	Set_Selection_Size_And_Pos();
	Redraw_Pane_Rect();


}
void CTrack::HandleMsg(SMsg* pMsg)
{
	switch(pMsg->iOpcode) 
	{
		case Msg_Add_Region:
		{
			break;
		}
		case Msg_Track_MouseClick_Inside:
		{
			tint32 iNew = (tint32)(pMsg->pDataIn);
			tbool bThis = (iNew == miTrackID);
			mbClicked = bThis;
			break;
		}
		case Msg_Track_MouseMove_Inside:
		{
			tint32 iNew = (tint32)(pMsg->pDataIn);
			tbool bThis = (iNew == miTrackID);
			mbMouseOver = bThis;
			break;
		}
		case Msg_Reset_Drag_Region_Size:{
		
			if(mpTrack_Player2->GetMouse_Over_Track() != miTrackID){
				miDrag_Region_Size = -1;
				Hide_Selection();
			}
			break;
		}
		case Msg_Deselect_All:
			Hide_Selection();
			break;
		case MsgGUIData:
			{
				SGUIData sData	= *(SGUIData*)pMsg->pDataIn;
				switch( sData.iID) {
					case giParamID_Show_Fade:
					case giParamID_Show_Waveform:
						Update_Region_Graphic();
						break;
					case giParamID_Zoom:{
						Set_Selection_Size_And_Pos();
						Update_Region_Graphic();
					}
					break;
				}
			}
			break;
		case Msg_Resize:{
			Update_Region_Graphic();
			break;
		}
	}
}

void CTrack::Update_Region_Graphic()
{
	std::list<CRegion_Pane*>::const_iterator it = mppRegions.begin();
	
	for (; it != mppRegions.end(); it++) {
		CRegion_Pane*  pRegion = *it;
		pRegion->Refresh_Region_GUI();

	}

}

void CTrack::Refresh_Region_GUI(tuint32 uiRegionID)
{
//	SRegion_Drawing_Info info = gpDSPEngine->Get_Region_Drawing_Info(uiRegionID);	

	std::list<CRegion_Pane*>::const_iterator it = mppRegions.begin();
	
	for (; it != mppRegions.end(); it++) {
		CRegion_Pane*  pRegion = *it;
		if(pRegion->GetID() == uiRegionID){
			pRegion->Refresh_Region_GUI();
			//pRegion->Draw_Region_GUI();
			break;
		}
	}

}


void CTrack::Set_Track_SizeY(tint32 iSize) 
{
	mbMouseCaptured = false;
	miTrack_Size_Y	= iSize;
	mTrackSize.iCY = iSize ? giTrack_Size_Big-1: giTrack_Size_Small-1;
	mpPane->SetSize( mTrackSize );
	mpControl->SetSize(mTrackSize);
//!!!	mpMouseTrap->SetSize(mTrackSize);

	std::list<CRegion_Pane*>::const_iterator it = mppRegions.begin();
	for (; it != mppRegions.end(); it++) {
		CRegion_Pane* mpRegions = *it;
		mpRegions->Set_Track_SizeY(iSize);
	}
	//Move line
	mpTrack_Buttom_Line->SetLinePos(ge::SPos(0, mTrackSize.iCY ),ge::SPos(mTrackSize.iCX, mTrackSize.iCY ));
	
	
	Set_Selection_Size_And_Pos();
	
	/*	
	mpInvert->SetSize(mTrackSize);
	ge::SPos Pos;
	mpInvert->GetRelPos(Pos);
	ge::SRect Rct(Pos, mTrackSize);
	mpInvert->SetRect(Rct);
	*/

}
void CTrack::SetSizeX(tint32 iSizeX)
{
	tint32 iY;
	iY = miTrack_Size_Y ? giTrack_Size_Big:giTrack_Size_Small;
		
	mpPane->SetSize( ge::SSize(iSizeX, iY) );
	mpControl->SetSize(ge::SSize(iSizeX, iY));
	mpTrack_Buttom_Line->SetLinePos(ge::SPos(0, mTrackSize.iCY), ge::SPos(iSizeX, mTrackSize.iCY));
}

void CTrack::SetPos(tint32 iPosY)
{
	mpPane->SetPos(ge::SPos(0, iPosY));
}



void CTrack::Deselect_Regions()
{
/*
	gpDSPEngine->SetSelected_Region(-1);
	std::list<CRegion_GUI*>::const_iterator it = mppRegions.begin();
	for (; it != mppRegions.end(); it++) {
		CRegion_GUI* mpRegions = *it;
		mpRegions->Deselect();
	}
*/
}

void CTrack::InsertRegion(tuint32 uiRegionID, tuint64 uiSample_Pos, tuint64 uiSample_Start, tuint64 uiSample_Duration, const std::string& sName)
{


	mpPane->RemoveControl(pmBmp_Select_Big);
	mpPane->RemoveControl(pmBmp_Select_Small);

	
	
	tint32 iColor	=	miTrackID % 10;

	
	CRegion_Pane* region;
	region = new CRegion_Pane(this, GetGUI());
	region->SetInfo(uiRegionID, uiSample_Pos, uiSample_Start ,uiSample_Duration , iColor, miTrack_Size_Y, mpTrack_Player2,this, sName );
	region->Init();
	mpPane->AddControl(region->GetPane(), ge::SPos(0, 0));
//	region->Update_Graphic();
	
	mppRegions.push_back(region);
	
	

	mpPane->AddControl(pmBmp_Select_Big, ge::SPos(0, 0));
	mpPane->AddControl(pmBmp_Select_Small, ge::SPos(0, 0));
	pmBmp_Select_Small->SetVisible(false);
	pmBmp_Select_Big->SetVisible(false);


	region->Refresh_Region_GUI();

}

void CTrack::DeleteRegion(tuint32 uiID)
{
	std::list<CRegion_Pane*>::const_iterator it = mppRegions.begin();
	
	for (; it != mppRegions.end(); it++) {
		CRegion_Pane*  pRegion = *it;
		if(pRegion->GetID() == uiID){
			ge::IPane* pPaneRegion = pRegion->GetPane();
			if (pPaneRegion->GetParentWindow()->HasMouseFocus(pPaneRegion)) {
				// The region we're about to delete has focus
				// Remove focus from it
				pPaneRegion->GetParentWindow()->ReleaseMouseFocus();
			}
			mpPane->RemoveControl(pPaneRegion);
		
			// Experimental code
			this->RemoveChild(pRegion);
			
			mppRegions.remove(*it);

			delete pRegion;
			break;
		}
	}
	miDrag_Region_Size = -1;
	Hide_Selection();
}

void CTrack::Delete_All_Regions()
{
//	std::list<CRegion_GUI*>pRegionsToDelete;
	std::list<tuint32>pRegionsToDelete;
	
	
	// Find regions to delete
	std::list<CRegion_Pane*>::const_iterator it = mppRegions.begin();
	for (; it != mppRegions.end(); it++) {

//		CRegion_GUI*  pRegion = *it;
//		mpPane->RemoveControl(pRegion->GetPane());
		
//		pRegionsToDelete.push_back(*it);
			
		
		pRegionsToDelete.push_back((*it)->GetID());
		
		
	}
	
	
	// Deleter regions
	std::list<tuint32>::const_iterator it2 = pRegionsToDelete.begin();
	for (; it2 != pRegionsToDelete.end(); it2++) {
		
		//CRegion_GUI*  pRegion	= *it2;
	//	tuint iID				=	*it2;
		
		DeleteRegion(*it2);

	}
	
	mppRegions.clear();

}

tint32 CTrack::Find_Tool()
{
	return	GetPlugIn()->GetGlobalParm(giParamID_Tool_Selected, giSectionGUI);	
}

void CTrack::SelectRegion(tuint32 uiRegionID)
{
	//Select region
//	STrackSelectionInfo sInfo = gpDSPEngine->SelectRegion(muiRegionID);
	Set_Selection_Size_And_Pos();

}
