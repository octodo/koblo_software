
#include "RhythmComposerOS.h"


CRhythmComposerPane::CRhythmComposerPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
}

CRhythmComposerPane::~CRhythmComposerPane()
{
}

void CRhythmComposerPane::Init()
{
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(576, 345));
	// BackDrop
	CreateBitmap(ge::IControl::giNoID, IDB_RythmComposer, ge::SPos(0, 0));
	
	tint32 iX = 51;
	//Dial
	// Shuffle
	CreateKnob(giCtrl_Shuffle, IDB_Dial_Small, ge::SPos(iX, 131), 65);
	// Accent
	CreateKnob(giCtrl_Accent, IDB_Dial_Small, ge::SPos(iX, 131+38), 65);
	//-------------------
	// Bass Drum
	//-------------------
	iX	+=	44;
	// Tune
	CreateKnob(giCtrl_BD_Tune, IDB_Dial_Small, ge::SPos(iX, 131), 65);
	// Accent
	CreateKnob(giCtrl_BD_Attack, IDB_Dial_Small, ge::SPos(iX, 131+38), 65);
	iX	+=	28;
	// Tune
	CreateKnob(giCtrl_BD_Level, IDB_Dial_Small, ge::SPos(iX, 131), 65);
	// Accent
	CreateKnob(giCtrl_BD_Decay, IDB_Dial_Small, ge::SPos(iX, 131+38), 65);
	
	//-------------------
	// Snare Drum
	//-------------------
	iX	+=	28;
	// Tune
	CreateKnob(giCtrl_SD_Tune, IDB_Dial_Small, ge::SPos(iX, 131), 65);
	// Accent
	CreateKnob(giCtrl_SD_Tone, IDB_Dial_Small, ge::SPos(iX, 131+38), 65);
	iX	+=	28;
	// Tune
	CreateKnob(giCtrl_SD_Level, IDB_Dial_Small, ge::SPos(iX, 131), 65);
	// Accent
	CreateKnob(giCtrl_SD_Decay, IDB_Dial_Small, ge::SPos(iX, 131+38), 65);
	
	//-------------------
	// Lo Tom
	//-------------------
	iX	+=	28;
	// Tune
	CreateKnob(giCtrl_LT_Tune, IDB_Dial_Small, ge::SPos(iX, 131), 65);
	// Accent
	CreateKnob(giCtrl_LT_Dec, IDB_Dial_Small, ge::SPos(iX, 131+38), 65);
	iX	+=	28;
	// Tune
	CreateKnob(giCtrl_LT_Level, IDB_Dial_Small, ge::SPos(iX, 131), 65);
	
	//-------------------
	// Mid Tom
	//-------------------
	iX	+=	28;
	// Tune
	CreateKnob(giCtrl_MT_Tune, IDB_Dial_Small, ge::SPos(iX, 131), 65);
	// Accent
	CreateKnob(giCtrl_MT_Dec, IDB_Dial_Small, ge::SPos(iX, 131+38), 65);
	iX	+=	28;
	// Tune
	CreateKnob(giCtrl_MT_Level, IDB_Dial_Small, ge::SPos(iX, 131), 65);
	
	//-------------------
	// High Tom
	//-------------------
	iX	+=	28;
	// Tune
	CreateKnob(giCtrl_HT_Tune, IDB_Dial_Small, ge::SPos(iX, 131), 65);
	// Accent
	CreateKnob(giCtrl_HT_Dec, IDB_Dial_Small, ge::SPos(iX, 131+38), 65);
	iX	+=	28;
	// Tune
	CreateKnob(giCtrl_HT_Level, IDB_Dial_Small, ge::SPos(iX, 131), 65);
	
	//-------------------
	// High Tom
	//-------------------
	iX	+=	28;
	// Tune
	CreateKnob(giCtrl_Rim_Level, IDB_Dial_Small, ge::SPos(iX, 131), 65);
	iX	+=	28;
	// Tune
	CreateKnob(giCtrl_Clap_Level, IDB_Dial_Small, ge::SPos(iX, 131), 65);
	//-------------------
	// Hat
	//-------------------
	iX	+=	28;
	// Tune
	CreateKnob(giCtrl_CH_Level, IDB_Dial_Small, ge::SPos(iX, 131), 65);
	// Accent
	CreateKnob(giCtrl_CH_Dec, IDB_Dial_Small, ge::SPos(iX, 131+38), 65);
	iX	+=	28;
	// Tune
	CreateKnob(giCtrl_OH_Level, IDB_Dial_Small, ge::SPos(iX, 131), 65);
	// Accent
	CreateKnob(giCtrl_OH_Dec, IDB_Dial_Small, ge::SPos(iX, 131+38), 65);
	//-------------------
	// Crash Ride
	//-------------------
	iX	+=	28;
	// Tune
	CreateKnob(giCtrl_Chrash_Level, IDB_Dial_Small, ge::SPos(iX, 131), 65);
	// Accent
	CreateKnob(giCtrl_Chrash_Tune, IDB_Dial_Small, ge::SPos(iX, 131+38), 65);
	iX	+=	28;
	// Tune
	CreateKnob(giCtrl_Ride_Level, IDB_Dial_Small, ge::SPos(iX, 131), 65);
	// Ride Decay
	CreateKnob(giCtrl_Ride_Dec, IDB_Dial_Small, ge::SPos(iX, 131+38), 65);
	//-------------------
	// Playback
	//-------------------
	// Start
	Create2StateButton(giCtrl_Start,	IDB_Button_Seq,	ge::SPos(51, 220), true);

	// Tempo
	CreateKnob(giCtrl_Tempo, IDB_Dial_Big, ge::SPos(162, 221), 65);
	
	// Sync
	Create2StateButton(giCtrl_Start,	IDB_Button_Seq,	ge::SPos(207, 220), true);
	
	// Song
	Create2StateButton(giCtrl_Start,	IDB_Button_Song,	ge::SPos(263, 217), true);
	
	// Volume
	CreateKnob(giCtrl_Volume, IDB_Dial_Big, ge::SPos(502, 221), 65);
	
	// Steps
	Create2StateButton(giCtrl_Steps,	IDB_Button_Steps,	ge::SPos(94, 242), true);
	
	// Copy
	CreateButton(giCtrl_Copy,	IDB_Button_Copy,	ge::SPos(40, 283), false);
	// Paste
	CreateButton(giCtrl_Paste,	IDB_Button_Paste,	ge::SPos(40, 283+13), false);
	// Clear
	CreateButton(giCtrl_Clear,	IDB_Button_Clear,	ge::SPos(40, 283+26), false);
	
	
	
	
	// Sequence programmong buttons
	tbool bEven = true;
	tint32 iPosX = 94;
	for(tint32 i=0; i< 16; i++) {
		Create2StateButton(giCtrl_SeqStep01+i,	IDB_Button_Seq,	ge::SPos(iPosX, 288), true);
		iPosX +=	bEven ? 26 : 30;
		bEven = !bEven;
	}
	
	//
	
/*	//
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
*/	

}

void CRhythmComposerPane::ConnectControls()
{
/*		
	//--------------------------------
	// Echo
	//--------------------------------
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Delay_Time,		giParam_Delay_Time);
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Delay_Feedback,	giParam_Feedback);
	RegisterGlobalControlWithLogTranslation(0,giCtrl_Delay_LP_Dampning ,	giParam_LP_Damping,1000);
	RegisterGlobalControlWithLogTranslation(0, giCtrl_Delay_HP_Dampning,	giParam_HP_Damping,1000);
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Delay_Mix,			giParam_Mix);
	*/

	mpParmMan->ControlUpdateAll();
	mpParmMan->ParameterUpdateAll();
}

void CRhythmComposerPane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	switch(pControl->GetID()) {
	}

	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew); 
}


