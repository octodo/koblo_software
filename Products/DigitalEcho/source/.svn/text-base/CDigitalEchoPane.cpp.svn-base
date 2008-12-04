
#include "DigitalEchoOS.h"


CDigitalEchoPane::CDigitalEchoPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
}

CDigitalEchoPane::~CDigitalEchoPane()
{
}

void CDigitalEchoPane::Init()
{
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(397, 99));
	// BackDrop
	CreateBitmap(ge::IControl::giNoID, IDB_DigitalEchoBackground, ge::SPos(0, 0));
	
	//
	Create2StateButton(giCtrl_Delay_Sync, IDB_Button_Sync, ge::SPos(35, 60), false);
	
	// Delay time Dial
	CreateKnob(giCtrl_Delay_Time, IDB_DialMedium, ge::SPos(51, 38), 65);
	// Feedback Dial
	CreateKnob(giCtrl_Delay_Feedback, IDB_DialMedium, ge::SPos(51*2, 38), 65);
	// Lopass Dial
	CreateKnob(giCtrl_Delay_LP_Dampning, IDB_DialMedium, ge::SPos(51*3, 38), 65);
	// Highpass Dial
	CreateKnob(giCtrl_Delay_HP_Dampning, IDB_DialMedium, ge::SPos(51*4, 38), 65);
	// Mix Dial
	CreateKnob(giCtrl_Delay_Mix, IDB_DialBig, ge::SPos(296, 32), 65);
	

}

void CDigitalEchoPane::ConnectControls()
{
		
	//--------------------------------
	// Echo
	//--------------------------------
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Delay_Time,		giParam_Delay_Time);
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Delay_Feedback,	giParam_Feedback);
	RegisterGlobalControlWithLogTranslation(0,giCtrl_Delay_LP_Dampning ,	giParam_LP_Damping,1000);
	RegisterGlobalControlWithLogTranslation(0, giCtrl_Delay_HP_Dampning,	giParam_HP_Damping,1000);
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Delay_Mix,			giParam_Mix);
	

	mpParmMan->ControlUpdateAll();
	mpParmMan->ParameterUpdateAll();
}

void CDigitalEchoPane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	switch(pControl->GetID()) {
	}

	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew); 
}


