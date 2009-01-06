#include "KSOS.h"

CTrack_Playhead::CTrack_Playhead(CBasePane* pParent, CBaseGUI* pGUI)
	: CBasePane(pParent, pGUI)
	
{
	mpPane			= NULL;
	mpControl		= NULL;

}

CTrack_Playhead::~CTrack_Playhead()
{
	
	
}

void CTrack_Playhead::SetInfo(CTrack_Time_Pos* pTrack_Time_Pos)
{
	mpCTrack_Time_Pos		=	pTrack_Time_Pos;
	mbMouseCaptured			=	false;
	

}

void CTrack_Playhead::Init()
{
	gpApplication = dynamic_cast<CKSPlugIn*>(mpPlugIn);

	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(15,47));

	// Custom control
	mpControl = ge::ICustomControl::Create();
	mpControl->Init(ge::IControl::giNoID, dynamic_cast<ge::ICustomControlCallback*>(this));
	mpControl->SetSize(ge::SSize(15,47));
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpControl), ge::SPos(0, 0));

	//  Background
	CreateBitmap(ge::IControl::giNoID, IDB_Back_Play_Head, ge::SPos(0, 0));
}


void CTrack_Playhead::ConnectControls()
{
}

void CTrack_Playhead::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
}

void CTrack_Playhead::EventGeneric(ge::IControl* pControl, void* pEventData)
{
	
}


void CTrack_Playhead::OnDraw(const ge::SRect &rUpdate)
{
}

tbool CTrack_Playhead::OnMouse(ge::EMouseMsg MouseMsg, const ge::SPos& Pos)
{
	if(!mbMouseCaptured && MouseMsg != ge::LeftButtonDown) 
		return false;
	else if (!mpControl->IsVisible())  return false;

	ge::SPos PosThis;
	ge::SSize SizeThis;
	mpControl->GetPos(PosThis);
	mpControl->GetSize(SizeThis);
	ge::SRect RectThis(PosThis, SizeThis);

	switch(MouseMsg){
		//----------------------------------------
		// Mouse Down
		//----------------------------------------
		case ge::LeftButtonDown: {
			if (RectThis.Inside(Pos)) {
				mbMouseCaptured = true;
				mpPane->GetParentWindow()->GetMouseFocus(mpPane);
				// Update mouse cursor
				mpControl->GetParentWindow()->SetMouseCursor(ge::IWindow::CursorHand);
				
				gpApplication->PrepareMovePlayhead();
				return true;
			}
			
			break;
		}
		//----------------------------------------
		// Mouse Up
		//----------------------------------------	
		case ge::LeftButtonUp: {
		
			mbMouseCaptured	= false;
			mpPane->GetParentWindow()->ReleaseMouseFocus();
			mpCTrack_Time_Pos->Redraw_Pane_Rect();
			gpApplication->EndMovePlayhead();
			return true;
			break;
		}
		//----------------------------------------
		// Mouse Move
		//----------------------------------------
		case ge::MouseMove: {
			ge::SPos sPosThis;
			mpCTrack_Time_Pos->GetPane()->GetPos(sPosThis);
			ge::SPos sPosNew(Pos.iX -sPosThis.iX,1);
			if(sPosNew.iX < 0) sPosNew.iX =	0;
			// Move playhead
			SetPos(ge::SPos(sPosNew.iX - 8,sPosNew.iY) );
			tint64 iSamplePos	=	(tint64)((tfloat32)(sPosNew.iX) * gpApplication->GetSamplesPrPixel());
			gpApplication->MovePlayhead(iSamplePos);
			// Redraw parrent pane
			mpCTrack_Time_Pos->Redraw_Pane_Rect();	
			return true;	
			break;
		}
	}
	return false;
}

tbool CTrack_Playhead::OnKeyDown(ge::EKey Key)
{
	return false;
}

void CTrack_Playhead::OnTimer()
{
}

void CTrack_Playhead::OnValueChange()
{
}

void CTrack_Playhead::SetSize(const ge::SSize& Size)
{
}

void CTrack_Playhead::SetPos(const ge::SPos& Pos)
{
	mpPane->SetPos(Pos);
//	mpCTrack_Time_Pos->Redraw_Pane_Rect();

}

