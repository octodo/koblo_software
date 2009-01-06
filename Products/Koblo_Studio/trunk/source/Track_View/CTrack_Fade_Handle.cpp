#include "KSOS.h"

CTrack_Fade_Handle::CTrack_Fade_Handle(CBasePane* pParent, CBaseGUI* pGUI)
	: CBasePane(pParent, pGUI)
	
{
	mpPane			= NULL;
	mpControl		= NULL;

}

CTrack_Fade_Handle::~CTrack_Fade_Handle()
{
	
	
}

void CTrack_Fade_Handle::SetInfo(CRegion_Pane* pRegion_GUI, tuint uiHandle_Type)
{
	muiHandle_Type			=	uiHandle_Type;
	mpRegion_Pane			=	pRegion_GUI;
	mbMouseCaptured			=	false;
}

void CTrack_Fade_Handle::Init()
{


	// Create the main pane
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(6,6));
	mpPane->SetBackgroundColour(ge::SRGB(0,0,64));
	mpFill_Rect		= CreateRect(ge::IControl::giNoID, ge::SRect(ge::SPos(1, 1), ge::SSize(4, 4)), ge::SRGB(200, 200, 200));

	// Custom control
	mpControl = ge::ICustomControl::Create();
	mpControl->Init(ge::IControl::giNoID, dynamic_cast<ge::ICustomControlCallback*>(this));
	mpControl->SetSize(ge::SSize(8,8));
	mpPane->AddControl(dynamic_cast<ge::IControl*>(mpControl), ge::SPos(0, 0));


}


void CTrack_Fade_Handle::ConnectControls()
{
}

void CTrack_Fade_Handle::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew);
}

void CTrack_Fade_Handle::EventGeneric(ge::IControl* pControl, void* pEventData)
{
	
}


void CTrack_Fade_Handle::OnDraw(const ge::SRect &rUpdate)
{
}

tbool CTrack_Fade_Handle::OnMouse(ge::EMouseMsg MouseMsg, const ge::SPos& Pos)
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
				
				ge::SPos sPosThis;
				GetPane()->GetPos(sPosThis);
				
				muiMouseDowPos	=	Pos.iX - sPosThis.iX;
				
				return true;
			}
			
			return false;
		}
		//----------------------------------------
		// Mouse Up
		//----------------------------------------	
		case ge::LeftButtonUp: {
		
			mbMouseCaptured	= false;
			mpPane->GetParentWindow()->ReleaseMouseFocus();
			return true;

		}
		//----------------------------------------
		// Mouse Move
		//----------------------------------------
		case ge::MouseMove: {
			ge::SPos sPosThis;
			mpRegion_Pane->GetPane()->GetPos(sPosThis);
			ge::SPos sPosNew(Pos.iX - sPosThis.iX - muiMouseDowPos, 0);
			if(sPosNew.iX < 0) sPosNew.iX =	0;
			
			ge::SPos sPosNewY(0 , Pos.iY - sPosThis.iY);
			tint iPosY = sPosNewY.iY;
			iPosY++;
			
			switch(muiHandle_Type){
				case giFade_Out_Handle:
					mpRegion_Pane->Update_Fade_Out(sPosNew.iX);
					break;
				case giFade_In_Handle:
					mpRegion_Pane->Update_Fade_In(sPosNew.iX);
					break;
				case giRegion_Volume_Handle:
					mpRegion_Pane->Update_Region_Volume(iPosY);
					break;
			}
					
			return true;	
		}
	}
	return false;
}

tbool CTrack_Fade_Handle::OnKeyDown(ge::EKey Key)
{
	return false;
}

void CTrack_Fade_Handle::OnTimer()
{
}

void CTrack_Fade_Handle::OnValueChange()
{
}

void CTrack_Fade_Handle::SetSize(const ge::SSize& Size)
{
}

void CTrack_Fade_Handle::SetPos(const ge::SPos& Pos)
{
	mpPane->SetPos(Pos);

}

