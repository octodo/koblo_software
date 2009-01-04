#include "KSOS.h"

CTrack_Time_Pos::CTrack_Time_Pos(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI), 
	mppBar_Beat_Line(giNumber_Of_Grid_Lines + 1),
	mppBar_Beat_Text(giNumber_Of_Grid_Lines + 1),
	mppMin_Sec_Line(giNumber_Of_Grid_Lines + 1),
	mppMin_Sec_Text(giNumber_Of_Grid_Lines + 1)
{
	mpPane = NULL;

	// Max Verifyed 
	miEdit = -1;

}

CTrack_Time_Pos::~CTrack_Time_Pos()
{
}

void CTrack_Time_Pos::SetInfo()
{
	mpKSPlugIn = dynamic_cast<CKSPlugIn*>(mpPlugIn);
}

void CTrack_Time_Pos::Init()
{
	


	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(gTrack_Time_Pos);
	mpPane->SetBackgroundColour(ge::SRGB(207,207,207));

	// Custom control
	mpControl = ge::ICustomControl::Create();
	mpControl->Init(ge::IControl::giNoID, dynamic_cast<ge::ICustomControlCallback*>(this));
	mpControl->SetSize(gTrack_Time_Pos);
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpControl), ge::SPos(0, 0));

	mpMin_Sec_Fill_Rect		= CreateRect(ge::IControl::giNoID, ge::SRect(ge::SPos(0, 16), ge::SSize(gTrack_Time_Pos.iCX, 15)), ge::SRGB(207, 225, 207));
	mpBar_Beatc_Fill_Rect		= CreateRect(ge::IControl::giNoID, ge::SRect(ge::SPos(0, 32), ge::SSize(gTrack_Time_Pos.iCX, 15)), ge::SRGB(225, 207, 207));
	
	mpLine1 = CreateLine(ge::IControl::giNoID, ge::SPos(0, 15), ge::SPos(gTrack_Scroll_Editor.iCX, 15), ge::SRGB(0, 0, 0));	
	mpLine2 = CreateLine(ge::IControl::giNoID, ge::SPos(0, 31), ge::SPos(gTrack_Scroll_Editor.iCX, 31), ge::SRGB(0, 0, 0));	
	mpLine3 = CreateLine(ge::IControl::giNoID, ge::SPos(0, 47), ge::SPos(gTrack_Scroll_Editor.iCX, 47), ge::SRGB(0, 0, 0));	

	msLineColor[giColor_1] = ge::SRGB(117,98,75);
	msLineColor[giColor_2] = ge::SRGB(163,136,104);
	msLineColor[giColor_4] = ge::SRGB(163,136,104);
	msLineColor[giColor_8] = ge::SRGB(163,136,104);
	msLineColor[giColor_16] = ge::SRGB(163,136,104);
	msLineColor[giColor_32] = ge::SRGB(163,136,104);
	
	miLast_Min_Sec_Line_Used	=	0;
	miLast_Min_Sec_Text_Used	=	0;
	miLast_Bar_Beat_Text_Used	=	0;
	miLast_Bar_Beat_Line_Used	=	0;

	SetSignature(giSignature4_4);
	miLast_Bar_Beat_Text_Allocated		=	-1;
	miLast_Bar_Beat_Line_Allocated		=	-1;
	miLast_Min_Sec_Line_Allocated		=	-1;
	miLast_Min_Sec_Text_Allocated		=	-1;
	miLast_Pos_X						=	-12;
	

	mpLoop =CreateBitmap(ge::IControl::giNoID, IDB_Back_Loop_Selection, ge::SPos(100, 0));
	mpLoop->SetSize(ge::SSize(200,15));

	//IDB_Back_Loop_Selection
	//--------------------------------------------
	// Track_Player2
	// This is a container holding the audio files, it scrolls when KS plays
	mpTrack_Playhead = new CTrack_Playhead(this, GetGUI());
	mpTrack_Playhead->SetInfo(this);
	mpTrack_Playhead->Init();
	mpPane->AddControl(mpTrack_Playhead->GetPane(), ge::SPos(0, 0));
	

}


void CTrack_Time_Pos::ConnectControls()
{
	RegisterGlobalControl(giSectionGUI, giCtrl_Lopp_Start, giParamID_Loop_Start);
	RegisterGlobalControl(giSectionGUI, giCtrl_Loop_End, giParamID_Loop_End);
}

void CTrack_Time_Pos::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
	
	if (pControl->GetID() == giCtrl_Lopp_Start) {
		pControl->GetValue();
	}	
}

void CTrack_Time_Pos::EventGeneric(ge::IControl* pControl, void* pEventData)
{
}

void CTrack_Time_Pos::HandleMsg(SMsg* pMsg)
{
	switch(pMsg->iOpcode) 
	{
		case Msg_Draw_Loop:{
			Draw_Loop();
			break;
		}
		case Msg_Resize: {
			Update_Graphic();
			
			break;
		}
		case MsgGUIData: {
			SGUIData sData	= *(SGUIData*)pMsg->pDataIn;
			switch( sData.iID){
				case giParamID_Zoom:
				{
					Update_Zoom();
					break;
				}
			}
			break;
		}
		case MsgGlobalData: {
			SGlobalData sData	= *(SGlobalData*)pMsg->pDataIn;
			switch( sData.iID){
				case giParamID_KS_Tempo: {
					//mfTempo			= (tfloat32)sData.iVal/ 10.0f;
					Update_Zoom();

					break;
				}
				case giParamID_KS_Time_Signature:
				{
					SetSignature( sData.iVal);
					Update_Zoom();
					
					break;
				}
				default: break;
			}
			break;
		}
	}
}

void CTrack_Time_Pos::Draw_Bars_Beat_Grid()
{


	
	tfloat64 fPixelPrSample =  mpKSPlugIn->GetPixelPrSample();

	// Suppress calls to CPane::UpdatePosition
	GetPane()->SuppressUpdatePositions(true);

	ge::SSize sPaneSize;
	mpPane->GetSize(sPaneSize);
	

	tint32 i			=	0;
	tint32 iLine		=	0;
	tfloat32 fPosX		=	0;
	tint32 iBar			=	0;
	tint32 iBeat		=	0;
	tint32 i16			=	0;
	char psz[8];
	tint iTextID		=	0;
	ge::SPos posStart(0, 32);
	ge::SPos posEnd(0, 46);
	tint iDraw_Bar		= 0;
	
	ge::SRGB textColor(64, 64, 64);
	
	while ( posStart.iX < sPaneSize.iCX   && i <= giNumber_Of_Grid_Lines - 1) {


		switch(mpiPattern[iLine]){
			case giColor_1:{
				iBar++;
				iBeat	=	1;
				i16		=	1;
				
				if (mbDraw_1){
					Draw_Bar_Beat_Line(i, posStart, posEnd, msLineColor[giColor_1]);
					if(mbDraw_2){
						mbDraw_16 ? sprintf(psz, "%d:%d:%d", iBar,iBeat,i16):sprintf(psz, "%d:%d", iBar,iBeat);
					}
					else{
						sprintf(psz, "%d", iBar);
					}
					Draw_Bar_Beat_Text( iTextID, psz, ge::SPos(posStart.iX + 2, posStart.iY), textColor);
					iTextID++;
					i++;
				}
				else{
					if(iDraw_Bar == 0){
						Draw_Bar_Beat_Line(i, posStart, posEnd, msLineColor[giColor_1]);
						sprintf(psz, "%d", iBar);
						Draw_Bar_Beat_Text( iTextID, psz, ge::SPos(posStart.iX + 2, posStart.iY), textColor);
						iTextID++;
						i++;
					}
					if(iDraw_Bar == 2 && mbDraw_1_2){
						Draw_Bar_Beat_Line(i, posStart, posEnd, msLineColor[giColor_1]);
						sprintf(psz, "%d", iBar);
						Draw_Bar_Beat_Text( iTextID, psz, ge::SPos(posStart.iX + 2, posStart.iY), textColor);
						iTextID++;
						i++;
					}
					iDraw_Bar++;
					if(iDraw_Bar == 4) iDraw_Bar=0;
				}
				
				break;
			}
			case giColor_2:
			{ 
				iBeat++;
				i16	=	1;
				if (mbDraw_2){
					Draw_Bar_Beat_Line(i, posStart, posEnd, msLineColor[giColor_2]);
					mbDraw_16 ? sprintf(psz, "%d:%d:%d", iBar,iBeat,i16):sprintf(psz, "%d:%d", iBar,iBeat);
					Draw_Bar_Beat_Text( iTextID, psz, ge::SPos(posStart.iX + 2, posStart.iY),textColor);
					iTextID++;
					i++;
				}
				break;
			}
			case giColor_4:
			{ 
				iBeat++;
				i16	=	1;
				if (mbDraw_4){
					Draw_Bar_Beat_Line(i, posStart, posEnd, msLineColor[giColor_4]);
					mbDraw_16 ? sprintf(psz, "%d:%d:%d", iBar,iBeat,i16):sprintf(psz, "%d:%d", iBar,iBeat);
					Draw_Bar_Beat_Text( iTextID, psz, ge::SPos(posStart.iX + 2, posStart.iY),textColor);
					iTextID++;
					i++;
				}
				break;
			}
			case giColor_8:
			{ 
				i16++;
				if (mbDraw_16){
					Draw_Bar_Beat_Line(i, posStart, posEnd, msLineColor[giColor_8]);
					sprintf(psz, "%d:%d:%d", iBar,iBeat,i16);
					Draw_Bar_Beat_Text( iTextID, psz, ge::SPos(posStart.iX + 2, posStart.iY),textColor);
					iTextID++;
					i++;
				}
				
				break;
			}
			case giColor_16:
			{	i16++;
				if (mbDraw_16){
					Draw_Bar_Beat_Line(i, posStart, posEnd, msLineColor[giColor_16]);
					sprintf(psz, "%d:%d:%d", iBar,iBeat,i16);
					Draw_Bar_Beat_Text( iTextID, psz, ge::SPos(posStart.iX + 2, posStart.iY),textColor);
					iTextID++;
					i++;
				}
			}
			case giColor_32:
			{ 
				if (mbDraw_32){
					Draw_Bar_Beat_Line(i, posStart, posEnd, msLineColor[giColor_32]);
					i++;
				}
				break;
			}
		}
		
		// Count up line patterne and wrap arount
		if(++iLine >= mfSub_Beats) iLine = 0;
		// Move ahead
		fPosX			+=	mfSamples_Pr_32 * fPixelPrSample;
		posStart.iX = posEnd.iX = (tint32)fPosX;
	}
	
	
	for(tint32  j=i;  j < miLast_Bar_Beat_Line_Used; j++) {
		mppBar_Beat_Line[j]-> SetVisible(false);
	}
	miLast_Bar_Beat_Line_Used	= i-1;

	for(tint32 j=iTextID;  j < miLast_Bar_Beat_Text_Used; j++) {
		mppBar_Beat_Text[j]-> SetVisible(false);
	}
	miLast_Bar_Beat_Text_Used	= iTextID-1;
	
	// Allow UpdatePosition calls again

	GetPane()->SuppressUpdatePositions(false);
	

}

void CTrack_Time_Pos::Draw_Min_Sec_Grid()
{

	tfloat64 fPixelPrSample =  mpKSPlugIn->GetPixelPrSample();
	
	// Suppress calls to CPane::UpdatePosition
	GetPane()->SuppressUpdatePositions(true);
	
	char psz[8];
	ge::SSize sPaneSize;
	mpPane->GetSize(sPaneSize);	

	tfloat32 fPos				= 0.0f;
	ge::SPos posStart(0, 16);
	ge::SPos posEnd(0, 30);
	tint32 iLine				= 0;
	tint32 iSec					= 0;
	
	tint32 iDisplayMin;
	tint32 iDisplaySec;

	while ( posStart.iX < sPaneSize.iCX) {
	
		// Draw Sec
		iDisplayMin = iSec/60;
		iDisplaySec = iSec%60;
		sprintf(psz, "%d:%d",iDisplayMin, iDisplaySec);
		Draw_Min_Sec_Text( iLine, psz, ge::SPos(posStart.iX + 2, posStart.iY),ge::SRGB(64,64,64));
		Draw_Min_Sec_Line( iLine, posStart, posEnd, ge::SRGB(127,150,150));

		// Move ahead
		fPos		+=	mfSamples_Per_Line * fPixelPrSample;
		iSec		+=	miSec_Pr_line;
		posStart.iX =	posEnd.iX = (tint32)fPos;
		iLine++;
	
	}
	iLine--;
	for(tint32  j=iLine;  j <= miLast_Min_Sec_Line_Used; j++) {
		mppMin_Sec_Line[j]-> SetVisible(false);
		mppMin_Sec_Text[j]-> SetVisible(false);
	}

	
	miLast_Min_Sec_Line_Used	=	iLine;
	// Allow UpdatePosition calls again
	GetPane()->SuppressUpdatePositions(false);

}

void CTrack_Time_Pos::Update_Zoom()
{

	tfloat64 fSamplesPrPixel	=	mpKSPlugIn->GetSamplesPrPixel();
	mfSamples_Pr_32				=	mpKSPlugIn->GetSamples_Pr_32();
	
	tfloat32 fMin_Sample_Dist	=	 giMin_Pix_Pr_Gridline * 8;
	fMin_Sample_Dist			*=	fSamplesPrPixel;
	
	mbDraw_32	= mfSamples_Pr_32		> fMin_Sample_Dist ? true : false;
	mbDraw_16	= mfSamples_Pr_32 * 2	> fMin_Sample_Dist ? true : false;
	mbDraw_8	= mfSamples_Pr_32 * 4	> fMin_Sample_Dist ? true : false;
	mbDraw_4	= mfSamples_Pr_32 * 8	> fMin_Sample_Dist ? true : false;
	mbDraw_2	= mfSamples_Pr_32 * 16	> fMin_Sample_Dist ? true : false;
	mbDraw_1	= mfSamples_Pr_32 * 32	> fMin_Sample_Dist ? true : false;
	mbDraw_1_2	= mfSamples_Pr_32 * 64	> fMin_Sample_Dist ? true : false;
	mbDraw_1_4	= mfSamples_Pr_32 * 128	> fMin_Sample_Dist ? true : false;
	
	
	
	//---------------------------------------------------------------
	// Minuts and seconds on top of pane
	tfloat fSamples_Pr_Second		=	GetPlugIn()->GetSampleRate();
	fMin_Sample_Dist				=	50.0f * fSamplesPrPixel;
	mfSamples_Per_Line				=	fSamples_Pr_Second;
	miSec_Pr_line					=	1;
	
	while( mfSamples_Per_Line < fMin_Sample_Dist){
		mfSamples_Per_Line	+= fSamples_Pr_Second;
		miSec_Pr_line++;
	}
	Update_Graphic();
}




void CTrack_Time_Pos::Update_Graphic()
{

	tfloat64 fPixelPrSample			=	mpKSPlugIn->GetPixelPrSample();
	tfloat64  fSession_End_Sample	=	gpDSPEngine->Get_Session_End_Sample();
	tuint32	uiEndPixel				=	fSession_End_Sample * fPixelPrSample;
	uiEndPixel						+=  128;
	
	if (uiEndPixel < 4000)
		uiEndPixel = 4000;
	
	
	ge::SSize sPaneSize(uiEndPixel,gTrack_Time_Pos.iCY);
	mpPane->SetSize(ge::SSize(uiEndPixel,48));
	
	ge::SRect rRect(ge::SPos(0, 16), ge::SSize(uiEndPixel, 15));
	
	mpBar_Beatc_Fill_Rect->SetRect(rRect);
	rRect.iY = 32;
	mpMin_Sec_Fill_Rect->SetRect(rRect);
	
	mpLine1->SetLinePos(ge::SPos(0,15), ge::SPos(uiEndPixel,15));
	mpLine2->SetLinePos(ge::SPos(0,31), ge::SPos(uiEndPixel,31));
	mpLine3->SetLinePos(ge::SPos(0,47), ge::SPos(uiEndPixel,47));
	
	Draw_Loop();
	Draw_Bars_Beat_Grid();
	Draw_Min_Sec_Grid();

}

void CTrack_Time_Pos::Draw_Min_Sec_Line( tint32 iLine, ge::SPos posStart, ge::SPos posEnd, ge::SRGB col)
{

	// Allocate gridline
	if(miLast_Min_Sec_Line_Allocated <= iLine) {
		mppMin_Sec_Line[iLine] = CreateLine(iLine, posStart, posEnd , col);
		miLast_Min_Sec_Line_Allocated++;
	}
	// Make gridline visible
	else{
		
		mppMin_Sec_Line[iLine]->SetColour(col);
		mppMin_Sec_Line[iLine]->SetLinePos(posStart, posEnd);
	}
	mppMin_Sec_Line[iLine]->SetVisible(true);
}

void CTrack_Time_Pos::Draw_Bar_Beat_Line( tint32 iLine, ge::SPos posStart, ge::SPos posEnd, ge::SRGB col)
{
	// Allocate gridline
	if(miLast_Bar_Beat_Line_Allocated < iLine) {
		mppBar_Beat_Line[iLine] = CreateLine(iLine, posStart, posEnd , col);
		miLast_Bar_Beat_Line_Allocated++;
	}
	// Make gridline visible
	else{
		mppBar_Beat_Line[iLine]->SetColour(col);
		mppBar_Beat_Line[iLine]->SetLinePos(posStart, posEnd);
	}
	mppBar_Beat_Line[iLine]->SetVisible(true);
}

void CTrack_Time_Pos::Draw_Bar_Beat_Text( tint32 iText_ID, char* psz, ge::SPos pos, ge::SRGB color)
{
	
	if( iText_ID > giNumber_Of_Grid_Lines) iText_ID = 0; //!!! Catch error
	//!!! Allocate dynamicaly here
	if(miLast_Bar_Beat_Text_Allocated < iText_ID) {
		mppBar_Beat_Text[iText_ID] = CreateText(	ge::IControl::giNoID, 
												pos, 
												ge::SSize(76, 15), 
												CreateFont(Generic128, IDB_Font_Axel_Tight,color), 
												psz, 
												ge::IText::HorzAlignLeft, 
												ge::IText::VertAlignCenter);
		miLast_Bar_Beat_Text_Allocated++;
	}
	else{
		mppBar_Beat_Text[iText_ID]->SetPos(pos);
		mppBar_Beat_Text[iText_ID]->SetText(psz);
	}
	mppBar_Beat_Text[iText_ID]->SetVisible(mbDraw_1_4);
}

void CTrack_Time_Pos::Draw_Min_Sec_Text( tint32 iText_ID, char* psz, ge::SPos pos, ge::SRGB color)
{
	if( iText_ID > giNumber_Of_Grid_Lines) iText_ID = 0; //!!! Catch error

	if(miLast_Min_Sec_Text_Allocated < iText_ID) {
		mppMin_Sec_Text[iText_ID] = CreateText(	ge::IControl::giNoID, 
												pos, 
												ge::SSize(76, 15), 
												CreateFont(Generic128, IDB_Font_Axel_Tight,color), 
												psz, 
												ge::IText::HorzAlignLeft, 
												ge::IText::VertAlignCenter);
		miLast_Min_Sec_Text_Allocated++;
		//mppMin_Sec_Text[iText_ID]->SetVisible(true);
	}
	else{
		mppMin_Sec_Text[iText_ID]->SetPos(pos);
		mppMin_Sec_Text[iText_ID]->SetText(psz);
	}
	mppMin_Sec_Text[iText_ID]->SetVisible(true);
	
}

void CTrack_Time_Pos::SetSignature(tint32 iSignature)
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

void CTrack_Time_Pos::Set_Play_Head(tint64 iSample_Pos)
{

	tfloat64 fPixelPrSample =  mpKSPlugIn->GetPixelPrSample();
	
	// Find position
	tint32 iPixel_Pos	=	(tint32)(fPixelPrSample * iSample_Pos);
	tint32 iPlay_Head_Pos	=	iPixel_Pos - 7;
	
	if(!mpTrack_Playhead->Is_Moving()){
	
		if(miLast_Pos_X	!=iPixel_Pos){
		
			// Move Playhead
			mpTrack_Playhead->SetPos(ge::SPos(iPlay_Head_Pos,1));
			// Refresh
			ge::SSize Size;
			ge::SPos Pos;
			GetPane()->GetPos(Pos);
			tint32 XDistMoved = iPlay_Head_Pos - miLast_Pos_X ;
			
			// Moving from left to reight
			if( XDistMoved >= 0){
				Pos.iX += miLast_Pos_X;
				Size = ge::SSize(XDistMoved + 16, 47);
			}
			else{
				XDistMoved	=	abs(XDistMoved) ;
				Pos.iX	+=	miLast_Pos_X - XDistMoved;
				Size	=	ge::SSize( XDistMoved + 16, 47);
			}
			mpPane->Redraw(ge::SRect(Pos, Size));
		}
	}
	// Store last pos x
	miLast_Pos_X	=	iPixel_Pos;
}



void CTrack_Time_Pos::OnDraw(const ge::SRect &rUpdate)
{
}

tbool CTrack_Time_Pos::OnMouse(ge::EMouseMsg MouseMsg, const ge::SPos& Pos)
{
	if(!mpControl->IsVisible())  return false;
	else if(!mbMouseCaptured){
	
		if(MouseMsg == ge::MouseMove && MouseMsg != ge::LeftButtonDown){
			

			tfloat64 fSamplesPrPixel	=	mpKSPlugIn->GetSamplesPrPixel();
			
			ge::SPos PosThis;
			mpControl->GetPos(PosThis);
			tint64 uiSamplesScrolled = PosThis.iX;

			tint64 iSample = (Pos.iX - uiSamplesScrolled) * fSamplesPrPixel;
			if(iSample < 0) iSample = 0;
			mpTrack_Top->SetCursorInSamples(iSample);
			return false;

		}
			
	}

	ge::SPos PosThis;
	ge::SSize SizeThis;
	mpControl->GetPos(PosThis);
	mpControl->GetSize(SizeThis);
	ge::SRect RectThis(PosThis, SizeThis);
	
	tint32 iMinSamples		= GetPlugIn()->GetSampleRate() / 10 ;


	switch(MouseMsg){
		//----------------------------------------
		// Mouse Down
		//----------------------------------------
		case ge::LeftButtonDown: {
			if (RectThis.Inside(Pos)) {
				mbMouseCaptured = true;
				mpPane->GetParentWindow()->GetMouseFocus(mpPane);
				// Store mouse down position
				ge::SPos sPosThis;
				mpPane->GetRelPos(sPosThis);	
				mfMouse_Down_Pos_X = Pos.iX - PosThis.iX;
				
				mfMouse_Down_Pos_Y = Pos.iY - PosThis.iY;
			
				if(mfMouse_Down_Pos_Y < 16){
					
					mfSLoop_Drawing_Info		=	gpDSPEngine->Get_Loop_Drawing_Info();
					Set_Loop_Selection();
				}
				else{
					tfloat32 fSampelsPrPixel	=	mpKSPlugIn->GetSamplesPrPixel();
					mpKSPlugIn->PlaybackGoToPos(mfMouse_Down_Pos_X * fSampelsPrPixel);
					//Redraw_Pane_Rect();
					miEdit = giEdit_Loop_Off;
				}
				
		
			}
			break;
		}
		//----------------------------------------
		// Mouse Up
		//----------------------------------------	
		case ge::LeftButtonUp: {
		
			//mpPane->GetParentWindow()->ReleaseMouseFocus();
			
			if(mbMouseCaptured){
				
				mpPane->GetParentWindow()->ReleaseMouseFocus();
			}
			mbMouseCaptured	= false;
			break;
		}
		//----------------------------------------
		// Mouse Move
		//----------------------------------------
		case ge::MouseMove: {

			ge::SPos sPosThis;
			mpPane->GetRelPos(sPosThis);
			tfloat32 fPosX	=	Pos.iX - PosThis.iX;


			tfloat32 fSamplsPrPixel	=	mpKSPlugIn->GetSamplesPrPixel();
			tint64 iSampleCursor;
			tint64 iSampleStart;
			tint64 iSampleEnd;
			tfloat fSizeX;

			switch(miEdit){
				case giNew_Loop:{
					if(fPosX > mfMouse_Down_Pos_X){
						fSizeX				=	fPosX - mfMouse_Down_Pos_X + 1;
						iSampleStart		=	mfMouse_Down_Pos_X * fSamplsPrPixel;
						iSampleEnd			=	iSampleStart + (fSizeX * fSamplsPrPixel);
						iSampleCursor		=	iSampleEnd;
					}
					else{
						fSizeX				=	mfMouse_Down_Pos_X - fPosX + 1;
						iSampleEnd			=	mfMouse_Down_Pos_X * fSamplsPrPixel;
						iSampleStart		=	iSampleEnd - (fSizeX * fSamplsPrPixel);
						iSampleCursor		=	iSampleStart;
					}

					break;
				}
				case giEdit_Loop_Start:{
				
					tint64 iSamplePos		=	fPosX * fSamplsPrPixel;
					iSampleEnd				=	mfSLoop_Drawing_Info.uiLoop_End;
					if(iSamplePos < iSampleEnd - iMinSamples)
						iSampleStart		=	 iSamplePos;
					else
						iSampleStart		=	 iSampleEnd - iMinSamples;
						
					if(iSampleStart < 0)
						iSampleStart = 0;
						
					iSampleCursor = iSampleStart;
					break;
				}
				case giEdit_Loop_Midt:{

					tfloat32 fMove		=	fPosX - mfMouse_Down_Pos_X;
					fMove				*=	fSamplsPrPixel;
					iSampleStart		=	mfSLoop_Drawing_Info.uiLoop_Start	+ fMove;
					iSampleEnd			=	mfSLoop_Drawing_Info.uiLoop_End		+ fMove;
					
					if(iSampleStart < 0)
						iSampleStart = 0;
					if(iSampleEnd < iMinSamples)
						iSampleEnd = iMinSamples;
						
					iSampleCursor = fPosX * fSamplsPrPixel;

					break;
				}
				case giEdit_Loop_End:{

					iSampleEnd			=	fPosX * fSamplsPrPixel;
					iSampleStart		=	mfSLoop_Drawing_Info.uiLoop_Start;
	
					if(iSampleEnd < iSampleStart + iMinSamples)
						iSampleEnd		=	iSampleStart + iMinSamples;
						
					
					iSampleCursor = iSampleEnd;
					break;
				}
				case giEdit_Loop_Off:
					break;
			}
			
			if (miEdit >= 0 && miEdit != giEdit_Loop_Off) {
				
				mpTrack_Top->SetCursorInSamples(iSampleCursor);
				mpTrack_Top->SetStartSamples(iSampleStart);
				mpTrack_Top->SetEndSamples(iSampleEnd) ;
				
				Update_Loop(iSampleStart, iSampleEnd);
			}
			// .. Lasse
			break;
		}
	}
	return false;
}

tbool CTrack_Time_Pos::OnKeyDown(ge::EKey Key)
{
	return false;
}

void CTrack_Time_Pos::OnTimer()
{
}

void CTrack_Time_Pos::OnValueChange()
{
}

void CTrack_Time_Pos::SetSize(const ge::SSize& Size)
{
}

void CTrack_Time_Pos::SetPos(const ge::SPos& Pos)
{
	
}

void CTrack_Time_Pos::Draw_Loop()
{

	tint32 iStart;
	tint32 iSize;
	Get_Loop( iStart, iSize);

	mpLoop->SetSize(ge::SSize(iSize,15));
	mpLoop->SetPos(ge::SPos(iStart,0));
		
	Redraw_Pane_Rect();
	
	
		
}

void CTrack_Time_Pos::Get_Loop(tint32& iStart, tint32& iSize)
{

	SLoop_Drawing_Info info		=	gpDSPEngine->Get_Loop_Drawing_Info();
	tfloat32 fStart				=	info.uiLoop_Start;
	tfloat32 fEnd				=	info.uiLoop_End;
		
	tfloat32 fPixelPrSampel		=	mpKSPlugIn->GetPixelPrSample();
		
	tfloat32 fSize				=	fEnd - fStart + 1.0;
	fSize						*=	fPixelPrSampel;
	iSize						=	fSize + 0.5;
	iStart						=	fStart * fPixelPrSampel;
	
	Redraw_Pane_Rect();
}

void CTrack_Time_Pos::Set_Loop_Selection()
{
	tint32 iStart;
	tint32 iSize;
	Get_Loop( iStart,iSize);
	tint32	iEnd		= iStart + iSize - 1;
	tint32	iSection	= (iSize/3);
	
	if( mfMouse_Down_Pos_X  < iStart || mfMouse_Down_Pos_X  > iEnd){
		miEdit = giNew_Loop;
	}
	else if( mfMouse_Down_Pos_X  < iStart + iSection){
		miEdit = giEdit_Loop_Start;
	}
	else if( mfMouse_Down_Pos_X  < iStart + iSection*2){
		miEdit = giEdit_Loop_Midt;
	}
	else{
		miEdit = giEdit_Loop_End;
	}
}

void CTrack_Time_Pos::Update_Loop(tint32 iSampleStart, tint32 iSampleEnd)
{
	
	iSampleStart	=	mpKSPlugIn->SnapToGrid(iSampleStart);
	iSampleEnd		=	mpKSPlugIn->SnapToGrid(iSampleEnd);

				
	GetPlugIn()->SetGlobalParm(giParamID_Loop_Start,iSampleStart, giSectionGlobal);
	GetPlugIn()->SetGlobalParm(giParamID_Loop_End,iSampleEnd, giSectionGlobal);

}
