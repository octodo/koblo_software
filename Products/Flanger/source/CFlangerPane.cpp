
#include "FlangerOS.h"


CFlangerPane::CFlangerPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
}

CFlangerPane::~CFlangerPane()
{
}

void CFlangerPane::Init()
{
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(397, 99));
	// BackDrop
	CreateBitmap(ge::IControl::giNoID, IDB_FlangerBackground, ge::SPos(0, 0));
	

	// Delay time Dial
	CreateKnob(giCtrl_Time, IDB_DialMedium, ge::SPos(51, 38), 65);
	// Feedback Dial
	CreateKnob(giCtrl_Depth, IDB_DialMedium, ge::SPos(51*2, 38), 65);
	// Lopass Dial
	CreateKnob(giCtrl_Rate, IDB_DialMedium, ge::SPos(51*3, 38), 65);
	// Highpass Dial
	CreateKnob(giCtrl_Feedback, IDB_DialMedium, ge::SPos(51*4, 38), 65);
	// Mix Dial
	CreateKnob(giCtrl_Mix, IDB_DialBig, ge::SPos(296, 32), 65);
	
}

void CFlangerPane::ConnectControls()
{
	
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Time,				giParam_Time);
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Depth,				giParam_Depth);
	RegisterGlobalControlWithLogTranslation(0,giCtrl_Rate ,					giParam_Rate, 1000);
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Feedback,			giParam_Feedback);
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Mix,				giParam_Mix);
	
	mpParmMan->ControlUpdateAll();
	mpParmMan->ParameterUpdateAll();
}

void CFlangerPane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	switch(pControl->GetID()) {
	}

	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew); 
}


