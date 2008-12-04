
#include "VChorusOS.h"


CVChorusPane::CVChorusPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
}

CVChorusPane::~CVChorusPane()
{
}

void CVChorusPane::Init()
{
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(397, 99));
	
	
	
	
	// BackDrop
	CreateBitmap(ge::IControl::giNoID, IDB_VintageChorusBackground, ge::SPos(0, 0));
	

	
	// Delay time Dial
	CreateKnob(giCtrl_Delay_Time, IDB_DialMedium, ge::SPos(51, 38), 65);
	// Feedback Dial
	CreateKnob(giCtrl_Depth, IDB_DialMedium, ge::SPos(51*2, 38), 65);
	// Lopass Dial
	CreateKnob(giCtrl_Rate, IDB_DialMedium, ge::SPos(51*3, 38), 65);
	// Highpass Dial
	CreateKnob(giCtrl_With, IDB_DialMedium, ge::SPos(51*4, 38), 65);
	// Mix Dial
	CreateKnob(giCtrl_Mix, IDB_DialBig, ge::SPos(296, 32), 65);
	
	
	
	
}

void CVChorusPane::ConnectControls()
{
	// TODO: Connect control IDs (for GUI use) to parm-IDs (for parameter-manager use) below here.
	// Use the CBaseGUI::RegisterGlobalControlXxx...() methods.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_g_u_i.html
	// NOTE: Remember to first add the parm-IDs to the parameter-manager (in CDigitalEchoPlugIn::AddParameters() ).
	// NOTE: Several control-IDs may be connected to one parm-ID.
	// NOTE: Only one parm-ID may be connected to each control-ID.
	
	//--------------------------------
	// Echo
	//--------------------------------
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Delay_Time,		giParam_DelayTime);
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Depth,				giParam_Depth);
	RegisterGlobalControlWithLogTranslation(0,giCtrl_Rate ,					giParam_Rate,1000);
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_With,				giParam_With);
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Mix,				giParam_Mix);
	
	
	mpParmMan->ControlUpdateAll();
	mpParmMan->ParameterUpdateAll();
}

void CVChorusPane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	switch(pControl->GetID()) {
	}

	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew); 
}


