
#include "BitCrunchOS.h"


CBitCrunchPane::CBitCrunchPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
}

CBitCrunchPane::~CBitCrunchPane()
{
}

void CBitCrunchPane::Init()
{
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(222, 106));
	// BackDrop
	CreateBitmap(ge::IControl::giNoID, IDB_BitCrush, ge::SPos(0, 0));
	
	tint32 iX = 18;

	// Bits
	CreateDisplay( giCtrl_Bit_Display, ge::SPos(iX, 86),  ge::SSize(32, 12), CreateFont(Generic128, IDB_Scripto_Tight, ge::SRGB(0, 255, 0)));	iX += 6;
	CreateKnob(giCtrl_Bit, IDB_BC_Dial, ge::SPos(iX, 52), 65);																					iX += 64;
	// Sample Rate
	CreateDisplay( giCtrl_SR_Display, ge::SPos(iX+4, 86),  ge::SSize(32, 12), CreateFont(Generic128, IDB_Scripto_Tight, ge::SRGB(0, 255, 0)));	iX += 6;
	CreateKnob(giCtrl_SR, IDB_BC_Dial, ge::SPos(iX, 52), 65);																					iX += 64;
	// Mix
	CreateDisplay( giCtrl_Mix_Display, ge::SPos(iX, 86),  ge::SSize(32, 12), CreateFont(Generic128, IDB_Scripto_Tight, ge::SRGB(0, 255, 0)));	iX += 6;
	CreateKnob(giCtrl_Mix, IDB_BC_Dial, ge::SPos(iX, 52), 65);																					


}

void CBitCrunchPane::ConnectControls()
{
		
	
	// Bits
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Bit,	giParmIDBit);
	RegisterGlobalControlWithDisplayTranslater(0, giCtrl_Bit_Display, giParmIDBit, 0, 5, 0);
	// Sample Rate
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_SR,	giParmIDSR);
	RegisterGlobalControlWithDisplayTranslater(0, giCtrl_SR_Display, giParmIDSR, 0, 5, 0);
	// Mix
	RegisterGlobalControlWithLinearTranslation(0, giCtrl_Mix,	giParmIDMix);
	RegisterGlobalControlWithDisplayTranslater(0, giCtrl_Mix_Display, giParmIDMix, 0, 5, 0);
	
	mpParmMan->ControlUpdateAll();
	mpParmMan->ParameterUpdateAll();
}

void CBitCrunchPane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	switch(pControl->GetID()) {
	}

	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew); 
}


