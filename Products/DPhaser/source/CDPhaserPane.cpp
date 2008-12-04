
#include "DPhaserOS.h"


CDPhaserPane::CDPhaserPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
}

CDPhaserPane::~CDPhaserPane()
{
}

void CDPhaserPane::Init()
{
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(397, 99));
	// BackDrop
	CreateBitmap(ge::IControl::giNoID, IDB_DigitalPhaserBackground, ge::SPos(0, 0));
	
	// Phaser freq Dial
	CreateKnob(giCtrl_Phaser_Freq, IDB_DialMedium, ge::SPos(51, 38), 65);
	// Range Dial
	CreateKnob(giCtrl_Phaser_Range, IDB_DialMedium, ge::SPos(51*2, 38), 65);
	// Rate Dial
	CreateKnob(giCtrl_Phaser_Rate, IDB_DialMedium, ge::SPos(51*3, 38), 65);
	// Feedback Dial
	CreateKnob(giCtrl_Phaser_Feedback, IDB_DialMedium, ge::SPos(51*4, 38), 65);
	// Mix Dial
	CreateKnob(giCtrl_Phaser_Mix, IDB_DialBig, ge::SPos(296, 32), 65);
	
}

void CDPhaserPane::ConnectControls()
{
	
	RegisterGlobalControlWithLogTranslation(0, giCtrl_Phaser_Freq,			giParamID_Freq, 1000);
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Phaser_Range,		giParamID_Range);
	RegisterGlobalControlWithLogTranslation(0,giCtrl_Phaser_Rate ,			giParamID_Rate, 1000);
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Phaser_Feedback,	giParamID_Feedback);
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Phaser_Mix,		giParamID_Mix);
	
	mpParmMan->ControlUpdateAll();
	mpParmMan->ParameterUpdateAll();
}

void CDPhaserPane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	switch(pControl->GetID()) {
	}

	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew); 
}


