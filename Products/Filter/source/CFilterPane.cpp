
#include "FilterOS.h"


CFilterPane::CFilterPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
}

CFilterPane::~CFilterPane()
{
}

void CFilterPane::Init()
{
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(453, 100));
	// BackDrop
	CreateBitmap(ge::IControl::giNoID, IDB_Filter, ge::SPos(0, 0));
	
	//
//	Create2StateButton(giCtrl_Delay_Sync, IDB_Button_Sync, ge::SPos(35, 60), false);
	
	tint32 iX	=	38;
	tint32 iY	=	28;
	// Delay time Dial
	CreateKnob(giCtrl_Cutoff, IDB_DialBig, ge::SPos(iX, iY), 65);		
	CreateDisplay( giCtrl_Cutoff_Display, ge::SPos(iX-8, iY+50),  ge::SSize(56, 12), CreateFont(Generic128, IDF_ARIAL10, ge::SRGB(0, 255, 0))); iX += 64;
	
	// Feedback Dial
	CreateKnob(giCtrl_Resonance, IDB_DialBig, ge::SPos(iX, iY), 65);
	CreateDisplay( giCtrl_Resonance_Display, ge::SPos(iX-8, iY+50),  ge::SSize(56, 12), CreateFont(Generic128, IDF_ARIAL10, ge::SRGB(0, 255, 0)));	iX += 64;
	// Lopass Dial
	CreateKnob(giCtrl_Mix, IDB_DialBig, ge::SPos(iX, iY), 65);	
	CreateDisplay( giCtrl_Mix_Display, ge::SPos(iX-8, iY+50),  ge::SSize(56, 12), CreateFont(Generic128, IDF_ARIAL10, ge::SRGB(0, 255, 0)));	iX += 72;
	// Highpass Dial
	CreateKnob(giCtrl_LoPass, IDB_DialBig, ge::SPos(iX, iY), 65);
	CreateDisplay( giCtrl_LoPass_Display, ge::SPos(iX-8, iY+50),  ge::SSize(56, 12), CreateFont(Generic128, IDF_ARIAL10, ge::SRGB(0, 255, 0)));	iX += 64;
	// BandPass Dial
	CreateKnob(giCtrl_BandPass, IDB_DialBig, ge::SPos(iX, iY), 65);		
	CreateDisplay( giCtrl_BandPass_Display, ge::SPos(iX-8, iY+50),  ge::SSize(56, 12), CreateFont(Generic128, IDF_ARIAL10, ge::SRGB(0, 255, 0)));	iX += 64;
	// Mix Dial
	CreateKnob(giCtrl_HighPass, IDB_DialBig, ge::SPos(iX, iY), 65);		
	CreateDisplay( giCtrl_HighPass_Display, ge::SPos(iX-8, iY+50),  ge::SSize(56, 12), CreateFont(Generic128, IDF_ARIAL10, ge::SRGB(0, 255, 0)));	
	
	
	

}

void CFilterPane::ConnectControls()
{
		
	//--------------------------------
	// Echo
	//--------------------------------
	RegisterGlobalControlWithTrueLogTranslation(0, giCtrl_Cutoff,			giParam_Cutoff);
	RegisterGlobalControlWithDisplayTranslater(0,	giCtrl_Cutoff_Display,	giParam_Cutoff, 0, 3, 0," Hz");
	
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Resonance,			giParam_Resonance);
	RegisterGlobalControlWithDisplayTranslater(0, giCtrl_Resonance_Display,	giParam_Resonance, 1, 3, 1," %");
	
	RegisterGlobalControlWithLinearTranslation(0,giCtrl_Mix ,				giParam_Mix);
	RegisterGlobalControlWithDisplayTranslater(0, giCtrl_Mix_Display,		giParam_Mix, 0, 3, 0," %");
	
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_LoPass,			giParam_LoPass);
	RegisterGlobalControlWithDisplayTranslater(0,	giCtrl_LoPass_Display,	giParam_LoPass, 0, 3, 0, " %");
	

	
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_BandPass,			giParam_BandPass);
	RegisterGlobalControlWithDisplayTranslater(0,giCtrl_BandPass_Display,	giParam_BandPass, 0, 3, 0, " %");
	
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_HighPass,			giParam_HighPass);
	RegisterGlobalControlWithDisplayTranslater(0,	giCtrl_HighPass_Display,giParam_HighPass, 0, 3, 0, " %");
	

	mpParmMan->ControlUpdateAll();
	mpParmMan->ParameterUpdateAll();
}

void CFilterPane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	switch(pControl->GetID()) {
	}

	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew); 
}


