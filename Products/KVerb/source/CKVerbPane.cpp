
#include "KVerbOS.h"


CKVerbPane::CKVerbPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
}

CKVerbPane::~CKVerbPane()
{
}

void CKVerbPane::Init()
{
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(397, 99));
	// BackDrop
	CreateBitmap(ge::IControl::giNoID, IDB_DigitalReverbBackground, ge::SPos(0, 0));
	
	// Reverb size freq Dial
	CreateKnob(giCtrl_Reverb_Size, IDB_DialMedium, ge::SPos(51, 38), 65);
	// Predelay
	CreateKnob(giCtrl_Reverb_PDelay, IDB_DialMedium, ge::SPos(51*2, 38), 65);
	// Damp
	CreateKnob(giCtrl_Reverb_Damp, IDB_DialMedium, ge::SPos(51*3, 38), 65);
	// Lopass
	CreateKnob(giCtrl_Reverb_Color2, IDB_DialMedium, ge::SPos(51*4, 38), 65);
	// Mix Dial
	CreateKnob(giCtrl_Reverb_Mix, IDB_DialBig, ge::SPos(296, 32), 65);
	
}

void CKVerbPane::ConnectControls()
{
	
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Reverb_Size,		giParam_Reverb_Size);
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Reverb_PDelay,		giParam_Reverb_PDelay);
	RegisterGlobalControlWithLinearTranslation(0,giCtrl_Reverb_Damp ,		giParam_Reverb_Damp);
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Reverb_Color2,		giParam_Reverb_Color2);
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Reverb_Mix,		giParamID_Mix);
	
	mpParmMan->ControlUpdateAll();
	mpParmMan->ParameterUpdateAll();
}

void CKVerbPane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	switch(pControl->GetID()) {
	}

	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew); 
}


