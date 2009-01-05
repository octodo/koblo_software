#include "KSOS.h"

CRegion_Pane::CRegion_Pane(CBasePane* pParent, CBaseGUI* pGUI)
	: CBasePane(pParent, pGUI)
	
{
	mpPane			= NULL;
	mpControl		= NULL;

	mppfPeak[0] = mppfPeak[1] = NULL;

	// (lasse) It's impossible to see errors if variables aren't initialized!!! Hrmpf!
	mpTrack_Player2 = NULL;
}

CRegion_Pane::~CRegion_Pane()
{
	if (mppfPeak[0]) {
		delete[] mppfPeak[0];
		delete[] mppfPeak[1];
	}
	if (mppfPeak[2]) {
		delete[] mppfPeak[2];
		delete[] mppfPeak[3];
	}

	mpDrawPrimitives->Destroy();
}

void CRegion_Pane::SetInfo(	tuint32 uiRegionID,
							tuint64	uiSample_Pos,
							tuint64	uiSample_Start, 
							tuint64 uiSample_Duration, 
							tint32 iColor, 
							tint32 iSize_Y,
							CTrack_Player2* pTrack_Player2,
							CTrack* pTrack,
							const std::string& sName)
{
	mpKSPlugIn				=	dynamic_cast<CKSPlugIn*>(GetPlugIn());

	muiRegionID				=	uiRegionID;
	muiTrack_Pos			=	uiSample_Pos;
	mfSample_Start			=	uiSample_Start;
	muiSample_Duration		=	uiSample_Duration;
	miColor					=	iColor;
	miPixel_Size_Y			=	iSize_Y ? giTrack_Size_Big: giTrack_Size_Small;
	mpTrack_Player2			=	pTrack_Player2;
	mpTrack					=	pTrack;
	mpLine_Start			=	NULL;
	mpLine_End				=	NULL;
	mbMouseCaptured			=	false;
//	mbSelection				=	false;
	mbExtendSelection		=	false;
	miEdit_State			=	giEdit_None;
	strcpy((char*)mPsz, sName.c_str());
	miTrimStart				= 0;
	mpText					=	NULL;
	
	muiSample_Fade_In		=	0;
	muiSample_Fade_Out		=	0;
	
	Set_Size_X();
	Set_Pos_X();
}

void CRegion_Pane::Init()
{
	// Create the main pane
	mpPane = ge::IPane::Create();
	ge::SSize Size(miSize_X, miPixel_Size_Y-1);
	mpPane->SetSize(Size);
	mpPane->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteY );

	// Small background
	mpRegion_Back_Small =CreateBitmapVU(ge::IControl::giNoID, IDB_Back_Region_Small, ge::SPos(0, 0), 11);
	mpRegion_Back_Small->SetSize(ge::SSize(miSize_X,giTrack_Size_Small-1));
	mpRegion_Back_Small->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteX);
	mpRegion_Back_Small->SetFrame(miColor);
	
	// Big background
	mpRegion_Back_Big =CreateBitmapVU(ge::IControl::giNoID, IDB_Back_Region_Big, ge::SPos(0, 0), 11);
	mpRegion_Back_Big->SetSize(ge::SSize(miSize_X,giTrack_Size_Big-1));
	mpRegion_Back_Big->SetAutomaticResize(true, ge::IControl::ResizeAbsoluteX);
	mpRegion_Back_Big->SetFrame(miColor);

	mpLine_Start		= CreateLine(ge::IControl::giNoID, ge::SPos(0,0), ge::SPos(0,giTrack_Size_Small-1) , ge::SRGB(128, 128, 128));
	mpLine_End			= CreateLine(ge::IControl::giNoID, ge::SPos(miSize_X-1,0), ge::SPos(miSize_X-1,giTrack_Size_Small-1) , ge::SRGB(128, 128, 128));

	//--------------------------------------------
	// Text label
	mpText = CreateDisplay(	ge::IControl::giNoID, 
							ge::SPos(2, 0),  
							ge::SSize(118, 10), 
							CreateFont(Generic128, IDB_Font_Minix_Tight, ge::SRGB(32, 32, 32)), false);
							
	mpText->SetHorzAlignment(ge::IText::HorzAlignLeft);
	mpText->SetText(mPsz);
	
	//--------------------------------------------
	// Custom control
	mpControl = ge::ICustomControl::Create();
	mpControl->Init(ge::IControl::giNoID, dynamic_cast<ge::ICustomControlCallback*>(this));
	mpControl->SetSize(Size);
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpControl), ge::SPos(0, 0));

	mpDrawPrimitives = ge::IDrawPrimitives::Create();
	mpDrawPrimitives->Init(GetWindow());
	//--------------------------------------------
	// Volume
	mpRegion_Volume_Handle = new CTrack_Fade_Handle(this, GetGUI());
	mpRegion_Volume_Handle->SetInfo(this, giRegion_Volume_Handle);
	mpRegion_Volume_Handle->Init();
	mpPane->AddControl(mpRegion_Volume_Handle->GetPane(), ge::SPos(miSize_X-7, 30));
	// Volume Line
	mpLine_Region_Volume	= CreateLine(ge::IControl::giNoID, ge::SPos(miSize_X,0), ge::SPos(miSize_X,0) , ge::SRGB(0, 0, 0));
	
	//--------------------------------------------
	// Fade in 
	mpFade_In_Handle = new CTrack_Fade_Handle(this, GetGUI());
	mpFade_In_Handle->SetInfo(this, giFade_In_Handle);
	mpFade_In_Handle->Init();
	mpPane->AddControl(mpFade_In_Handle->GetPane(), ge::SPos(0, 0));
	// Line
	mpLine_Fade_In		= CreateLine(ge::IControl::giNoID, ge::SPos(0,0), ge::SPos(0,0) , ge::SRGB(0, 0, 0));
	
	//--------------------------------------------
	// Fade out 
	mpFade_Out_Handle = new CTrack_Fade_Handle(this, GetGUI());
	mpFade_Out_Handle->SetInfo(this, giFade_Out_Handle);
	mpFade_Out_Handle->Init();
	mpPane->AddControl(mpFade_Out_Handle->GetPane(), ge::SPos(miSize_X-7, 0));
	// Line
	mpLine_Fade_Out		= CreateLine(ge::IControl::giNoID, ge::SPos(miSize_X,0), ge::SPos(miSize_X,0) , ge::SRGB(0, 0, 0));

	

	Update_Graphic();
}


void CRegion_Pane::ConnectControls()
{
}

void CRegion_Pane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
}

void CRegion_Pane::EventGeneric(ge::IControl* pControl, void* pEventData)
{
	
	
}

void CRegion_Pane::Set_Size_X()
{

	miSize_X = Float2Int(muiSample_Duration * mpKSPlugIn->GetPixelPrSample());
	
	if(mpText)
		mpText->SetVisible(miSize_X > 118);
	
		
}

void CRegion_Pane::Set_Track_SizeY(tint32 iSize_Y)
{
	miPixel_Size_Y	= iSize_Y ? giTrack_Size_Big: giTrack_Size_Small;
	Update_Graphic();
}

void CRegion_Pane::Set_Pos_X()
{
	miPos_X	=	(tfloat64)(muiTrack_Pos * mpKSPlugIn->GetPixelPrSample());
}

void CRegion_Pane::Update_Size()
{
	
	ge::SSize size(miSize_X ,miPixel_Size_Y );
	// update the size of the control
	mpControl->SetSize(size);
	
	// Start and End Lines draw only if region is wide enough
	if(miSize_X <= 2){
		mpLine_Start->SetVisible(false);
		mpLine_End->SetVisible(false);
		
	}
	else{
		mpLine_Start->SetVisible(true);
		mpLine_Start->SetLinePos(ge::SPos(0, 0 ),ge::SPos(0, miPixel_Size_Y-1 ));
		
		mpLine_End->SetVisible(true);
		mpLine_End->SetLinePos(ge::SPos(miSize_X, 0 ),ge::SPos(miSize_X, miPixel_Size_Y-1 ));
	}
	
	
	//---------------------
	// select big/ small backdrop
	switch (miPixel_Size_Y) {
		case 40:
			mpRegion_Back_Small->SetVisible( true);
			mpRegion_Back_Big->SetVisible(false);
			break;
		case 184:
			mpRegion_Back_Small->SetVisible( false);
			mpRegion_Back_Big->SetVisible(true);
			break;
		default:
			break;
	}
	// If the region is wide enough draw fade and volume
	if(mbDrawFade && miSize_X >= 32){
		mpFade_In_Handle->GetPane()->SetVisible(true);
		mpLine_Fade_In->SetVisible(true);
		mpFade_Out_Handle->GetPane()->SetVisible(true);
		mpLine_Fade_Out->SetVisible(true);
		
		mpRegion_Volume_Handle->GetPane()->SetVisible(true);
		mpLine_Region_Volume->SetVisible(true);
	}
	else{
		mpFade_In_Handle->GetPane()->SetVisible(false);
		mpLine_Fade_In->SetVisible(false);
		mpFade_Out_Handle->GetPane()->SetVisible(false);
		mpLine_Fade_Out->SetVisible(false);
		
		mpRegion_Volume_Handle->GetPane()->SetVisible(false);
		mpLine_Region_Volume->SetVisible(false);
	}
	

}

tint32 CRegion_Pane::Find_Tool()
{
	return	GetPlugIn()->GetGlobalParm(giParamID_Tool_Selected, giSectionGUI);	
}

void CRegion_Pane::Handel_Select_All_Tool()
{
	//Select region
	STrackSelectionInfo sInfo = gpDSPEngine->SelectRegion(muiRegionID);
	mpTrack->Set_Selection_Size_And_Pos();
}

void CRegion_Pane::Handle_Cut_Tool(tuint32 uiPos)
{
	tuint64 uiSamplePos = (tfloat64)uiPos * mpKSPlugIn->GetSamplesPrPixel();
	uiSamplePos			=	mpKSPlugIn->SnapToGrid(uiSamplePos);
	
	gpDSPEngine->CutRegion( mpTrack->Get_TrackID(), muiRegionID, uiSamplePos+1);
}

void CRegion_Pane::Handle_Trim_Tool(tint32 uiPos)
{

	tfloat64 SamplesPrPixel =	mpKSPlugIn->GetSamplesPrPixel();
	tint64 iSamplePos	= (tfloat64)uiPos * SamplesPrPixel;

	gpDSPEngine->TrimRegion( mpTrack->Get_TrackID(), muiRegionID, miEdit_State != giEdit_Trim_End, iSamplePos);
		
}

void CRegion_Pane::SetSelection( tint64 uiSample_Selection_Start, tint64 uiSample_Selection_Duration){
	
	muiSample_Selection_Start		=	uiSample_Selection_Start;
	muiSample_Selection_Duration	=	uiSample_Selection_Duration;
	

	Update_Graphic();
}


void CRegion_Pane::Update_Graphic()
{	
	
	Set_Size_X();
	Set_Pos_X();
	Update_Size();
	
	mpPane->SetSize(ge::SSize(miSize_X, miPixel_Size_Y-1));
	mpPane->SetPos(ge::SPos(miPos_X, 0));
	
	tfloat32 fSizeY			= miPixel_Size_Y - 6;
	muiRegion_Volume_Pixel	= Float2Int( fSizeY - (mfRegion_Volume * fSizeY));
	
	
	Draw_Fade_In();
	Draw_Fade_Out();
	Draw_Region_Volume();
}

void CRegion_Pane::OnDraw(const ge::SRect &rUpdate)
{
	if (mppfPeak[0] == NULL) {
		tint32 iChannel;
		for (iChannel = 0; iChannel < 2; iChannel++) {
			tint32 iPeakFile;
			for (iPeakFile = 0; iPeakFile < 2; iPeakFile++) {
				gpDSPEngine->GetRegionPeakFile(muiRegionID, &(mppPeakFile[iPeakFile + iChannel * 2]), iChannel, iPeakFile);

				if (mppPeakFile[iPeakFile + iChannel * 2] == NULL) {
					mpiPeakFileSize[iPeakFile + iChannel * 2] = 0;
					mppfPeak[iPeakFile + iChannel * 2] = NULL;
				}
				else {
					mpiPeakFileSize[iPeakFile + iChannel * 2] = mppPeakFile[iPeakFile + iChannel * 2]->GetSizeWhenOpened();
					mppfPeak[iPeakFile + iChannel * 2] = new tfloat32[(tint32)(mpiPeakFileSize[iPeakFile + iChannel * 2] / sizeof(tfloat32))];
					mppPeakFile[iPeakFile + iChannel * 2]->Seek(0);
					tint64 iWantsToRead = mpiPeakFileSize[iPeakFile + iChannel * 2];
					tint64 iActuallyRead = mppPeakFile[iPeakFile + iChannel * 2]->Read((tchar*)mppfPeak[iPeakFile + iChannel * 2], iWantsToRead);
					maiActuallyRead[iPeakFile + iChannel * 2] = iActuallyRead;
				}
			}
		}
	}
	tbool bDraw_Waveform	= mpKSPlugIn->GetGlobalParm(giParamID_Show_Waveform, giSectionGUI);	
	if(bDraw_Waveform){

		tint32 iChannels = 1;
		if (mppPeakFile[2] != NULL) {
			iChannels = 2;
		}

		ge::SPos PosThis;
		mpControl->GetPos(PosThis);

		ge::SSize SizeThis;
		mpControl->GetSize(SizeThis);
		
		SizeThis.iCY = miPixel_Size_Y-2;


		tuint64 uiPixelOffset	=	Float2Int(mfSample_Start * mpKSPlugIn->GetPixelPrSample());

		tfloat64 fSamplesPerPixel	= mpKSPlugIn->GetSamplesPrPixel();
		
		tint32 iDrawStartX = 0;
		//if (iDrawStartX < 0) {
		if (PosThis.iX < 0) {
			iDrawStartX = -PosThis.iX;
		}
		tint32 iDrawEndX = SizeThis.iCX;
		if (iDrawEndX > rUpdate.iX + rUpdate.iCX - PosThis.iX) {
			iDrawEndX = rUpdate.iX + rUpdate.iCX - PosThis.iX;
		}

		const tint32 iSpaceMid =  5;

		if (iChannels == 1) {
			tfloat32* pfPeak = mppfPeak[0];
			tint32 iPeakSize = 1024;
			if (fSamplesPerPixel <= 512) {
				pfPeak = mppfPeak[1];
				iPeakSize = 64;
			}

			// (lasse) Crash-fix
			if (pfPeak == NULL) {
				// Don't attempt draw if peak file is missing
				iDrawEndX = iDrawStartX;
			}

			tint32 iPixel;
			for (iPixel = iDrawStartX; iPixel < iDrawEndX; iPixel++) {
				tfloat32 fPeakIndex = (tfloat32)((uiPixelOffset+iPixel) * fSamplesPerPixel);
				fPeakIndex /= iPeakSize;

				// Linear interpolate
				tint32 iPeakLow = (tint32)fPeakIndex;
				tint32 iPeakHigh = iPeakLow + 1;
				tfloat32 fAlpha = fPeakIndex - iPeakLow;
				tfloat32 fPeak = pfPeak[iPeakLow] * (1 - fAlpha) + pfPeak[iPeakHigh] * fAlpha;

				tint32 iPeak = (tint32)(fPeak * SizeThis.iCY / 2);

				mpDrawPrimitives->DrawLine(rUpdate,
					PosThis + ge::SPos(iPixel, SizeThis.iCY / 2 + iPeak),
					PosThis + ge::SPos(iPixel, SizeThis.iCY / 2 - iPeak),
					ge::SRGB(84, 84, 84));
			}
		}
		else {
			tfloat32* pfPeakL;
			tfloat32* pfPeakR;
			tint32 iPeakSize;
			if (fSamplesPerPixel <= 512) {
				pfPeakL = mppfPeak[1];
				pfPeakR = mppfPeak[3];
				iPeakSize = 64;

				// (Lasse) crash-fix: Don't read longer than there's peakfile
				if (iDrawEndX > maiActuallyRead[1])
					iDrawEndX = maiActuallyRead[1];
				if (iDrawEndX > maiActuallyRead[3])
					iDrawEndX = maiActuallyRead[3];
			}
			else {
				pfPeakL = mppfPeak[0];
				pfPeakR = mppfPeak[2];
				iPeakSize = 1024;

				// (Lasse) crash-fix: Don't read longer than there's peakfile
				if (iDrawEndX > maiActuallyRead[0])
					iDrawEndX = maiActuallyRead[0];
				if (iDrawEndX > maiActuallyRead[2])
					iDrawEndX = maiActuallyRead[2];
			}


			tint32 iPixel;
			for (iPixel = iDrawStartX; iPixel < iDrawEndX; iPixel++) {
				tfloat32 fPeakIndex = (tfloat32)((uiPixelOffset+iPixel) * fSamplesPerPixel);
				fPeakIndex /= iPeakSize;

				// Linear interpolate
				tint32 iPeakLow = (tint32)fPeakIndex;
				tint32 iPeakHigh = iPeakLow + 1;
				tfloat32 fAlpha = fPeakIndex - iPeakLow;
				tfloat32 fPeak = pfPeakL[iPeakLow] * (1 - fAlpha) + pfPeakL[iPeakHigh] * fAlpha;
				tint32 iPeakL = (tint32)(fPeak * (SizeThis.iCY - iSpaceMid) / 4);

				// Linear interpolate
				fPeak = pfPeakR[iPeakLow] * (1 - fAlpha) + pfPeakR[iPeakHigh] * fAlpha;
				tint32 iPeakR = (tint32)(fPeak * (SizeThis.iCY - iSpaceMid) / 4);

				mpDrawPrimitives->DrawLine(rUpdate,
					PosThis + ge::SPos(iPixel, (SizeThis.iCY - iSpaceMid) / 4 + iPeakL),
					PosThis + ge::SPos(iPixel, (SizeThis.iCY - iSpaceMid) / 4 - iPeakL),
					ge::SRGB(84, 84, 84));

				mpDrawPrimitives->DrawLine(rUpdate,
					PosThis + ge::SPos(iPixel, SizeThis.iCY - (SizeThis.iCY - iSpaceMid) / 4 + iPeakR),
					PosThis + ge::SPos(iPixel, SizeThis.iCY - (SizeThis.iCY - iSpaceMid) / 4 - iPeakR),
					ge::SRGB(84, 84, 84));
			}
		}
	}
}

tbool CRegion_Pane::OnMouse(ge::EMouseMsg MouseMsg, const ge::SPos& Pos)
{
	if(!mbMouseCaptured && MouseMsg != ge::LeftButtonDown) 
		return false;
	else if (!mpControl->IsVisible())  return false;

	switch(MouseMsg){
		//----------------------------------------
		// Mouse Down
		//----------------------------------------
		case ge::LeftButtonDown: {
			
			return Handle_LeftButtonDown( Pos);
			break;
		}
		//----------------------------------------
		// Mouse Up
		//----------------------------------------	
		case ge::LeftButtonUp: {
			
			return Handle_LeftButtonUp(Pos);
			break;
		}
		//----------------------------------------
		// Mouse Move
		//----------------------------------------
		case ge::MouseMove: {
			return Handle_MouseMove(Pos);
			break;
		}
	}
	return false;
}
tbool CRegion_Pane::Handle_LeftButtonDown( const ge::SPos& Pos)
{
	ge::SPos PosThis;
	ge::SSize SizeThis;
	mpControl->GetPos(PosThis);
	mpControl->GetSize(SizeThis);
	ge::SRect RectThis(PosThis, SizeThis);
	
	if (RectThis.Inside(Pos)) {
		mbMouseCaptured = true;
		mpPane->GetParentWindow()->GetMouseFocus(mpPane);
		// If the shift key is not pressed
		if (!ge::IWindow::ShiftPressed()) {
			// Deselect all other regions selected
			mpTrack_Player2->Deselect_Regions();
			// Store mouse down position	
			mpTrack_Player2->Set_Mouse_Down_Pos_X(Pos.iX - PosThis.iX);
			//mMouse_Down_Pos = Pos - PosThis;
		}
		else if(Find_Tool() == giTool_Select)  {

			ge::SPos sPosNew	=	Pos;
			sPosNew.iX			=	mpTrack_Player2->Get_Mouse_Up_Pos_X();
			mpTrack->SetTrackSelection(sPosNew, muiRegionID, giSelect_On_Track);
	
		}
		// Update mouse cursor
		mpControl->GetParentWindow()->SetMouseCursor(ge::IWindow::CursorHand);
		// Handle tools
		switch(Find_Tool()){
				// Highlight entire region if hand tool is selected
			case giTool_Hand:{
				Handel_Select_All_Tool();
				break;
			}
			case giTool_Trim:{
				if(	mpTrack_Player2->Get_Mouse_Down_Pos_X() < SizeThis.iCX/2)
					miEdit_State	=	giEdit_Trim_Start;
				else
					miEdit_State	=	giEdit_Trim_End;
				
				Handle_Trim_Tool(mpTrack_Player2->Get_Mouse_Down_Pos_X());
				break;
			}
			case giTool_Cut:{
				Handle_Cut_Tool(mpTrack_Player2->Get_Mouse_Down_Pos_X());
				break;
			}
			case giTool_Select:{
				if (!ge::IWindow::ShiftPressed())
					mpTrack_Player2->Set_Mouse_Down_Pos_X(Pos.iX);
				break;
				
			}
		}
		// Do this to prevent the mouse down message to 
		// go true to the underlying controls
		return true; 
	}
	return false;
	
	
}

tbool CRegion_Pane::Handle_LeftButtonUp( const ge::SPos& Pos)
{
	if(mbMouseCaptured){
		mpPane->GetParentWindow()->ReleaseMouseFocus();
		mbMouseCaptured	= false;
		mpTrack->Release_Mouse();
	}

	// (lasse) Crash-fix: Upon return from this clause the *this class doesn't exist anymore, so it MUST be last!
	/*
	if( miEdit_State == giEdit_Dragging) {
		
		ge::SPos sPosNew = Pos;
		sPosNew.iX -= mpTrack_Player2->Get_Mouse_Down_Pos_X();
		if(sPosNew.iX < 128) sPosNew.iX = 128;
		mpControl->GetParentWindow()->OnMouseDrop(sPosNew);
	}
	miEdit_State = giEdit_None;
	*/
	// .. (lasse)
	if (!ge::IWindow::ShiftPressed())
		mpTrack_Player2->Set_Mouse_Up_Pos_X(Pos.iX);
	
	else if(mbExtendSelection) {

		mpTrack_Player2->Set_Mouse_Up_Pos_X(Pos.iX);
	}
	mbExtendSelection = false;

	// (lasse) Crash-fix 2008-12-02 - code moved here, MUST be last!
	if( miEdit_State != giEdit_Dragging) {
		miEdit_State = giEdit_None;
	}
	else {
		// (lasse) actually... edit state doesn't matter much; soon we'll be deleted
		miEdit_State = giEdit_None;

		ge::SPos sPosNew = Pos;
		sPosNew.iX -= mpTrack_Player2->Get_Mouse_Down_Pos_X();
		if(sPosNew.iX < 128) sPosNew.iX = 128;
		// (lasse) Crash-fix: Upon return from OnMouseDrop(..) the *this class doesn't exist anymore, so it MUST be last!
		mpControl->GetParentWindow()->OnMouseDrop(sPosNew);
	}
	// .. (lasse)

	return true;
}

tbool CRegion_Pane::Handle_MouseMove( const ge::SPos& Pos)
{
	ge::SPos PosThis;
	ge::SSize SizeThis;
	mpControl->GetPos(PosThis);
	mpControl->GetSize(SizeThis);
	ge::SRect RectThis(PosThis, SizeThis);
	
	switch(Find_Tool()){
			
		case giTool_Select:{
			if(mbMouseCaptured){
				mbExtendSelection = true;
				mpTrack->SetTrackSelection(Pos, muiRegionID, giSelect_On_Track);
			}
			break;
		}
		case giTool_Hand:{
			if (mbMouseCaptured) {
				if(miEdit_State != giEdit_Dragging){
					miEdit_State	=	giEdit_Dragging;
					ge::IWindow::SDragAndDropInfo Info;
					Info.pOrigin = dynamic_cast<ge::IControl*>(mpControl);
					sprintf(Info.psz, "%d", GetID());
					mpControl->GetParentWindow()->SetDragAndDropInfo(Info);
				}
				///mMouse_Down_Pos
				
				ge::SPos sPosNew = Pos;
				sPosNew.iX -= mpTrack_Player2->Get_Mouse_Down_Pos_X();
				mpControl->GetParentWindow()->OnMouseDrag(sPosNew);
				mpControl->GetParentWindow()->SetMouseCursor(ge::IWindow::CursorHand);
				//return true;
			}
			
			break;
		}
		case giTool_Trim:{
			if (mbMouseCaptured) {
				if(miEdit_State == giEdit_Trim_Start){
					ge::SPos sPosNew = Pos;
					tint32 iRegionPos = Pos.iX - PosThis.iX;
					sPosNew.iX -= mpTrack_Player2->Get_Mouse_Down_Pos_X();
					Handle_Trim_Tool(iRegionPos);
					
				}
				else{
					ge::SPos sPosNew = Pos;
					tint32 iRegionPos = Pos.iX - PosThis.iX;
					sPosNew.iX -= mpTrack_Player2->Get_Mouse_Down_Pos_X();
					Handle_Trim_Tool(iRegionPos);
					
				}
				//return true;
			}
			
			break;
		}
	}
	return true;
}

tbool CRegion_Pane::OnKeyDown(ge::EKey Key)
{
	return false;
}

void CRegion_Pane::OnTimer()
{
}

void CRegion_Pane::OnValueChange()
{
}

void CRegion_Pane::SetSize(const ge::SSize& Size)
{
}

void CRegion_Pane::SetPos(const ge::SPos& Pos)
{
}

void CRegion_Pane::Refresh_Region_GUI()
{

	SRegion_Drawing_Info info	=	gpDSPEngine->Get_Region_Drawing_Info(muiRegionID);
	muiTrack_Pos				=	info.uiTrack_Start;
	mfSample_Start				=	info.uiSound_Start;
	muiSample_Duration			=	info.uiSound_Duration;
	muiSample_Fade_In			=	info.uiFade_In;
	muiSample_Fade_Out			=	info.uiFade_Out;
	mfRegion_Volume				=	info.fRegion_Volume;
	muiSample_End				=	muiSample_Duration - mfSample_Start + 1;	
	mbDrawFade					=	GetPlugIn()->GetGlobalParm(giParamID_Show_Fade, giSectionGUI);

	Set_Size_X();
	Set_Pos_X();
	Update_Graphic();

}

void CRegion_Pane::Update_Fade_In(tuint32 uiFade_In_Pixel)
{
	tfloat64 fSamplesPrPixel		=	mpKSPlugIn->GetSamplesPrPixel();
	muiSample_Fade_In				=	(tfloat64)uiFade_In_Pixel * fSamplesPrPixel;
	
	muiSample_Fade_In = gpDSPEngine->Fade_In(muiRegionID, muiSample_Fade_In);
	
	Refresh_Region_GUI();

}

void CRegion_Pane::Draw_Fade_In()
{
	tfloat64 fPixelPrSample		=	mpKSPlugIn->GetPixelPrSample();
	muiFade_In_Pixel			=	(tfloat64)muiSample_Fade_In * fPixelPrSample;
	
	
	if(muiFade_In_Pixel > miSize_X-11)
		muiFade_In_Pixel	=	miSize_X-11;
	
	mpLine_Fade_In->SetLinePos(ge::SPos(0, miPixel_Size_Y-1 ),ge::SPos(muiFade_In_Pixel, muiRegion_Volume_Pixel ));
	mpFade_In_Handle->SetPos(ge::SPos(muiFade_In_Pixel,muiRegion_Volume_Pixel));
}

void CRegion_Pane::Update_Fade_Out(tuint32 uiFade_Out_Pixel)
{
	uiFade_Out_Pixel += 6;
	if(uiFade_Out_Pixel > miSize_X+6) 
		uiFade_Out_Pixel = miSize_X+6;
	
	tfloat64 fSamplesPrPixel		=	mpKSPlugIn->GetSamplesPrPixel();
	tuint64 uiSamples				=	(tfloat64)uiFade_Out_Pixel * fSamplesPrPixel;
	
	if(uiSamples > muiSample_Duration)
		muiSample_Fade_Out			=	0;
	else
		muiSample_Fade_Out			=	muiSample_Duration - uiSamples;	
	
	muiSample_Fade_Out = gpDSPEngine->Fade_Out(muiRegionID, muiSample_Fade_Out);
	
	Refresh_Region_GUI();
	
	
}

void CRegion_Pane::Draw_Fade_Out()
{
	
	tfloat64 fPixelPrSample		=	mpKSPlugIn->GetPixelPrSample();
	tuint64 uiSamples			=	muiSample_Duration - muiSample_Fade_Out;
	muiFade_Out_Pixel			=	(tfloat64)uiSamples * fPixelPrSample;
	
	if(muiFade_Out_Pixel < 11)
		muiFade_Out_Pixel	=	11;
	
	mpLine_Fade_Out->SetLinePos(ge::SPos(miSize_X-1, miPixel_Size_Y-1 ),ge::SPos(muiFade_Out_Pixel, muiRegion_Volume_Pixel ));
	mpFade_Out_Handle->SetPos(ge::SPos(muiFade_Out_Pixel-6,muiRegion_Volume_Pixel));
	
}

void CRegion_Pane::Update_Region_Volume(tint32 uiRegion_Volume_Pixel)
{

	// called when the user drags in the volume handel
	
	
	tfloat32 fVolume = 1.0f - ( (tfloat32)uiRegion_Volume_Pixel / (tfloat32)miPixel_Size_Y);
	
	if(fVolume < 0.0f) 
		fVolume = 0.0f;
	else if(fVolume > 1.0f ) 
		fVolume = 1.0f;
	
	
	gpDSPEngine->Region_Volume(muiRegionID, fVolume);
	
	Refresh_Region_GUI();
	

	
}

void CRegion_Pane::Draw_Region_Volume()
{		
	mpLine_Region_Volume->SetLinePos(ge::SPos(muiFade_In_Pixel, muiRegion_Volume_Pixel ),ge::SPos(muiFade_Out_Pixel, muiRegion_Volume_Pixel ));
	
	
	tuint uiLength	=	muiFade_Out_Pixel - muiFade_In_Pixel;
	tuint uiMidt	=	uiLength / 2;
	tuint uiPos		=	muiFade_In_Pixel + uiMidt;
	
	mpRegion_Volume_Handle->SetPos(ge::SPos(uiPos-3, muiRegion_Volume_Pixel));

	
}










