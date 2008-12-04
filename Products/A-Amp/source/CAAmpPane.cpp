
#include "AAmpOS.h"


CAAmpPane::CAAmpPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
}

CAAmpPane::~CAAmpPane()
{
}

void CAAmpPane::Init()
{
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(435, 225));
	// BackDrop
	CreateBitmap(ge::IControl::giNoID, IDB_AAmp, ge::SPos(0, 0));
	
	tint32 iX	=	31;
	tint32 iY	=	118;
	// Gain time Dial
	CreateKnob(giCtrl_Gain, IDB_DialBig, ge::SPos(56, 132), 65);iX		+=	62;
	
	// Limiter off / on
	Create2StateButton(giCtrl_Tube,	IDB_Button_On,	ge::SPos(136, iY), true);
	// Harm off / on
	Create2StateButton(giCtrl_Harm,	IDB_Button_On,	ge::SPos(136, iY + 24 ), true);
	// tube off / on
	Create2StateButton(giCtrl_Limiter,	IDB_Button_On,	ge::SPos(136, iY + 48 ), true);
	
	mpDSP_Slot1 =CreateBitmapVU(giCtrl_Slot1, IDB_Flow, ge::SPos(220, 123), 4);
	mpDSP_Slot1->SetFrame(1);
	
	mpDSP_Slot2 =CreateBitmapVU(giCtrl_Slot2, IDB_Flow, ge::SPos(220, 123+22), 4);
	mpDSP_Slot2->SetFrame(2);
	
	mpDSP_Slot3 =CreateBitmapVU(giCtrl_Slot3, IDB_Flow, ge::SPos(220, 123+44), 4);
	mpDSP_Slot3->SetFrame(3);
	// Swap
	CreateButton(giCtrl_Swap_Button,	IDB_Button_Swap,	ge::SPos(136+160-18, iY +15 ), false);
	
	
	// Output meter, Left
	//mpMeter = CreatePeakMeter(ge::IControl::giNoID, ge::SPos(333, 136), 37, ge::IPeakMeter::Right);
	//FindPane(mpMeter)->CreateBitmap(ge::IControl::giNoID, IDB_Meter, ge::SPos(0, 0));
	
	
	mpMeter =CreateBitmapVU(giCtrl_Meter, IDB_Meter, ge::SPos(333, 136), 37);
	mpMeter->SetFrame(0);




}

void CAAmpPane::ConnectControls()
{
		
	RegisterGlobalControlWithTrueLogTranslation(0, giCtrl_Gain,		giParam_Gain);
	RegisterGlobalControl(0, giCtrl_Tube,							giParam_Tube);
	RegisterGlobalControl(0, giCtrl_Harm,							giParam_Harm);
	RegisterGlobalControl(0, giCtrl_Limiter,						giParam_Limiter);
	

	mpParmMan->ControlUpdateAll();
	mpParmMan->ParameterUpdateAll();
}

void CAAmpPane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	
	switch(pControl->GetID()) {
			
		case giCtrl_Swap_Button:{

			tuint uiSwap = GetPlugIn()->GetGlobalParm(giParam_Swap, 0);
			uiSwap++;
			if(uiSwap > 5) uiSwap = 0;
			GetPlugIn()->SetGlobalParm(giParam_Swap, uiSwap, 0);


			break;	
		}
		default: {
			GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew); 
			break;
		}
	
	}
	
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew); 
}




