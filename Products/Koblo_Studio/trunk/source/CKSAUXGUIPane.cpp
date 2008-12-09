
#include "KSOS.h"


CKSAUXGUIPane::CKSAUXGUIPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
	CBaseDSPEngine* pD = GetPlugIn()->GetDSPEngine();
	mpDSP = dynamic_cast<CKSDSP*>(pD);

	// Init meters arrays
	mpvoid_sMeters_All = (void*)new CKSDSP::SMeters_All();
	mbFirstMeterSet = true;
	mbIsVisible = false;
}

CKSAUXGUIPane::~CKSAUXGUIPane()
{
	// Delete meters arrays
	delete ((CKSDSP::SMeters_All*)mpvoid_sMeters_All);
}

void CKSAUXGUIPane::Init()
{
	mpPane = ge::IPane::Create();
	//mpPane->SetBackgroundColour(ge::SRGB(0, 0, 0));
	mpPane->SetSize(gAUX);
	
	//-------------------------------
	// Backdrop for AUX
	CreateBitmap(ge::IControl::giNoID, IDB_Back_AUX, ge::SPos(0, 0));
	//-------------------------------
	
	//--------------------------------
	// ECHO
	//--------------------------------
	
	Create2StateButton(giCtrl_Delay_Power, IDB_Button_Power, ge::SPos(25, 36), true);
	
	tint32 iX		=	154+126;
	tint32 iY		=	75;
	
	// Delay time 
	CreateKnob(giCtrl_Delay_Time, IDB_DialMedium, ge::SPos(iX, iY), 65);		iX += 64;
	CreateDisplay( giCtrl_Delay_Time_Display, ge::SPos(184, 65),  ge::SSize(56, 12), CreateFont(Generic128, IDB_AxelTight, ge::SRGB(0, 180, 0)));
	
	// Predlay
	CreateKnob(giCtrl_Delay_Feedback, IDB_DialMedium, ge::SPos(iX, iY), 65);	iX += 64;
	CreateDisplay( giCtrl_Delay_Feedback_Display, ge::SPos(184, 77),  ge::SSize(56, 12), CreateFont(Generic128, IDB_AxelTight, ge::SRGB(0, 180, 0)));
	
	// Lo Filt
	CreateKnob(giCtrl_Delay_LP_Dampning, IDB_DialMedium, ge::SPos(iX, iY), 65);	iX += 64;
	CreateDisplay( giCtrl_Delay_LP_Dampning_Display, ge::SPos(184, 89),  ge::SSize(56, 12), CreateFont(Generic128, IDB_AxelTight, ge::SRGB(0, 180, 0)));
	
	// High Filt
	CreateKnob(giCtrl_Delay_HP_Dampning, IDB_DialMedium, ge::SPos(iX, iY), 65);	iX += 64;
	CreateDisplay( giCtrl_Delay_HP_Dampning_Display, ge::SPos(184, 101),  ge::SSize(56, 12), CreateFont(Generic128, IDB_AxelTight, ge::SRGB(0, 180, 0)));
	
	
	// Output meter, Left
	mpEcho_MeterOutL = CreatePeakMeter(ge::IControl::giNoID, ge::SPos(534, 151-88), 47, ge::IPeakMeter::Up);
	FindPane(mpEcho_MeterOutL)->CreateBitmap(ge::IControl::giNoID, IDB_Meter_Off_Left_AUX, ge::SPos(0, 0));
	// Output meter, Right
	mpEcho_MeterOutR = CreatePeakMeter(ge::IControl::giNoID, ge::SPos(541, 151-88), 47, ge::IPeakMeter::Up);
	FindPane(mpEcho_MeterOutR)->CreateBitmap(ge::IControl::giNoID, IDB_Meter_Off_Right_AUX, ge::SPos(0, 0));
	//--------------------------------
	// REVERB
	//--------------------------------
	Create2StateButton(giCtrl_Reverb_Power, IDB_Button_Power, ge::SPos(25, 124), true);
	iX		=	152;
	iY		=	165;
	// Reverb Size
	CreateKnob(giCtrl_Reverb_Size, IDB_DialMedium, ge::SPos(iX, iY), 65);		iX += 64;
	// Predlay
	CreateKnob(giCtrl_Reverb_PDelay, IDB_DialMedium, ge::SPos(iX, iY), 65);		iX += 64;
	// Damping
	CreateKnob(giCtrl_Reverb_Damp, IDB_DialMedium, ge::SPos(iX, iY), 65);		iX += 64;
	// Reverb
	CreateKnob(giCtrl_Reverb_Panorama, IDB_DialMedium, ge::SPos(iX, iY), 65);	iX += 64;
	// Reverb
	CreateKnob(giCtrl_Reverb_Color1, IDB_DialMedium, ge::SPos(iX, iY), 65);		iX += 64;
	// Reverb
	CreateKnob(giCtrl_Reverb_Color2, IDB_DialMedium, ge::SPos(iX, iY), 65);		iX += 64;
	
	// Output meter, Left
	mpReverb_MeterOutL = CreatePeakMeter(ge::IControl::giNoID, ge::SPos(534, 153), 47, ge::IPeakMeter::Up);
	FindPane(mpReverb_MeterOutL)->CreateBitmap(ge::IControl::giNoID, IDB_Meter_Off_Left_AUX, ge::SPos(0, 0));
	// Output meter, Right
	mpReverb_MeterOutR = CreatePeakMeter(ge::IControl::giNoID, ge::SPos(541, 153), 47, ge::IPeakMeter::Up);
	FindPane(mpReverb_MeterOutR)->CreateBitmap(ge::IControl::giNoID, IDB_Meter_Off_Right_AUX, ge::SPos(0, 0));
}

void CKSAUXGUIPane::ConnectControls()
{
	//--------------------------------
	// Echo
	//--------------------------------
	RegisterGlobalControl(giSection_AUX1,						giCtrl_Delay_Power,		giAUX1_Param_Delay_Power);
	//Delay time
	RegisterGlobalControlWithLinearTranslation(giSection_AUX1, giCtrl_Delay_Time,		giAUX1_Param_Delay_Time);
	GetControl(giCtrl_Delay_Time_Display)->SetDefaultValueConverted(mpParmMan->Get(	giAUX1_Param_Delay_Time, de::IParameterManager::TypeGlobal, giSection_AUX1)->GetDefaultValue());
	RegisterGlobalControlWithDisplayTranslater(giSection_AUX1, giCtrl_Delay_Time_Display, giAUX1_Param_Delay_Time, 0, 5, 0, "mSec");
	// Feedback
	RegisterGlobalControlWithLinearTranslation(giSection_AUX1, giCtrl_Delay_Feedback,	giAUX1_Param_Feedback);
	GetControl(giCtrl_Delay_Feedback_Display)->SetDefaultValueConverted(mpParmMan->Get(	giAUX1_Param_Feedback, de::IParameterManager::TypeGlobal, giSection_AUX1)->GetDefaultValue());
	RegisterGlobalControlWithDisplayTranslater(giSection_AUX1, giCtrl_Delay_Feedback_Display, giAUX1_Param_Feedback, 0, 5, 0, "%");
	
	
	
	
	RegisterGlobalControlWithLogTranslation(giSection_AUX1,giCtrl_Delay_LP_Dampning ,	giAUX1_Param_LP_Damping,1000);
	GetControl(giCtrl_Delay_LP_Dampning_Display)->SetDefaultValueConverted(mpParmMan->Get(	giAUX1_Param_LP_Damping, de::IParameterManager::TypeGlobal, giSection_AUX1)->GetDefaultValue());
	RegisterGlobalControlWithDisplayTranslater(giSection_AUX1, giCtrl_Delay_LP_Dampning_Display, giAUX1_Param_LP_Damping, 0, 5, 0, "Hz");
	
	
	RegisterGlobalControlWithLogTranslation(giSection_AUX1, giCtrl_Delay_HP_Dampning,	giAUX1_Param_HP_Damping,1000);
	GetControl(giCtrl_Delay_HP_Dampning_Display)->SetDefaultValueConverted(mpParmMan->Get(	giAUX1_Param_HP_Damping, de::IParameterManager::TypeGlobal, giSection_AUX1)->GetDefaultValue());
	RegisterGlobalControlWithDisplayTranslater(giSection_AUX1, giCtrl_Delay_HP_Dampning_Display, giAUX1_Param_HP_Damping, 0, 5, 0, "Hz");
	
	
	
	
	
	/*giAUX1_First_Param = 0,   RegisterGlobalControlWithLinearTranslation
	giAUX1_Param_Delay_Time,	RegisterGlobalControlWithLinearTranslation
	giAUX1_Param_Feedback,		RegisterGlobalControlWithLinearTranslation
	giAUX1_Param_LF_Damping,	RegisterGlobalControlWithLogTranslation(giSection_AUX2,
						iControlID,
						iParameterID,
						1000);
						
	giAUX1_Param_HF_Damping RegisterGlobalControlWithLogTranslation()
	giAUX1_Param_Sync,RegisterGlobalControlWithLinearTranslation*/
	//--------------------------------
	// REVERB
	//--------------------------------
	RegisterGlobalControl(giSection_AUX2, giCtrl_Reverb_Power, giParam_Reverb_On);
	RegisterGlobalControlWithLinearTranslation(giSection_AUX2, giCtrl_Reverb_Size, giParam_Reverb_Size);
	RegisterGlobalControlWithLinearTranslation(giSection_AUX2, giCtrl_Reverb_PDelay, giParam_Reverb_PDelay);
	RegisterGlobalControlWithLinearTranslation(giSection_AUX2, giCtrl_Reverb_Damp, giParam_Reverb_Damp);
	RegisterGlobalControlWithLinearTranslation(giSection_AUX2, giCtrl_Reverb_Panorama, giParam_Reverb_Panorama);
	RegisterGlobalControlWithLinearTranslation(giSection_AUX2, giCtrl_Reverb_Color1, giParam_Reverb_Color1);
	RegisterGlobalControlWithLinearTranslation(giSection_AUX2, giCtrl_Reverb_Color2, giParam_Reverb_Color2);



}

void CKSAUXGUIPane::OnTimer(tint32 iTimerID)
{
	// Lasse, out-commented 2008-06-25 - not on this timer anymore
	/*
	if (iTimerID == giTimerID_Meters_AUXs) {
		if (mbIsVisible) {
			// Calc elapsed time since previous meter set
			tuint32 uiTimeMS_Now = ITime::GetTimeMS();
			tuint32 uiTimeMS_Elapsed = uiTimeMS_Now - miuTimeMS_Prev;
			miuTimeMS_Prev = uiTimeMS_Now;
			if (mbFirstMeterSet) {
				// Set all meters to 1.0, to make them fall-off
				mbFirstMeterSet = false;
				Update_AUXes_Meters(NULL);
			}
			else {
				// Get meter values and decay factor
				dynamic_cast<CKSPlugIn*>(mpPlugIn)->GetAllMeters_MaybeClear(mpvoid_sMeters_All, uiTimeMS_Elapsed, false);
				CKSDSP::SMeters_All* psMeters_All = (CKSDSP::SMeters_All*)mpvoid_sMeters_All;
				// Send pointer to struct with all meter values to all tracks and let them decide if they need it
				Update_AUXes_Meters(mpvoid_sMeters_All);
			}
		}
	}
	*/
}


void CKSAUXGUIPane::SendMsgFromTop(SMsg* pMsg)
{
	switch(pMsg->iOpcode) 
	{
		case Msg_Set_All_Meters:
			Update_All_Meters(pMsg->pDataIn);
			break;
	}
	
}


void CKSAUXGUIPane::Update_All_Meters(void* pvoid_sMeters_All)
{
	if (mbIsVisible) {
		if (mbFirstMeterSet) {
			// Set all meters to 1.0, to make them fall-off
			pvoid_sMeters_All = NULL;
			mbFirstMeterSet = false;
		}
		// Send pointer to struct with all meter values to all AUXs and let them decide if they need it
		Update_AUXes_Meters(pvoid_sMeters_All);
	}
}


void CKSAUXGUIPane::Update_AUXes_Meters(void* pvoid_sMeters_All)
{
	if (true) { //mpPane->IsVisible()) {
		if (pvoid_sMeters_All == NULL) {
			mpEcho_MeterOutL->SetFloatValueWithDecay(1.0, 0.0);
			mpEcho_MeterOutR->SetFloatValueWithDecay(1.0, 0.0);
			mpReverb_MeterOutL->SetFloatValueWithDecay(1.0, 0.0);
			mpReverb_MeterOutR->SetFloatValueWithDecay(1.0, 0.0);
		}
		else {
			CKSDSP::SMeters_All* psMeters_All = (CKSDSP::SMeters_All*)pvoid_sMeters_All;
			tfloat32* pMetersForEcho = psMeters_All->ppfMeters_AUXes[0];
			mpEcho_MeterOutL->SetFloatValueWithDecay(pMetersForEcho[0], psMeters_All->fDecayFactor);
			mpEcho_MeterOutR->SetFloatValueWithDecay(pMetersForEcho[1], psMeters_All->fDecayFactor);
			tfloat32* pMetersForReverb = psMeters_All->ppfMeters_AUXes[1];
			mpReverb_MeterOutL->SetFloatValueWithDecay(pMetersForReverb[0], psMeters_All->fDecayFactor);
			mpReverb_MeterOutR->SetFloatValueWithDecay(pMetersForReverb[1], psMeters_All->fDecayFactor);
		}
	}
}


void CKSAUXGUIPane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	switch(pControl->GetID()) {
	}
	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew); 
}

void CKSAUXGUIPane::Resize(ge::SSize SizeNew)
{
}


void CKSAUXGUIPane::UpdateGUIData(tint32 iID, tint32 iValue)
{

	if (iID == giParamID_Show_AUX_Window) {
		if (iValue == 0) {
			mbIsVisible = false;
		}
		else {
			mbFirstMeterSet = true;
			mbIsVisible = true;
		}
		return;
	}

}


void CKSAUXGUIPane::UpdateAUX1Data(tint32 iID, tint32 iValue)
{
	
	switch(iID){
	}	
}

void CKSAUXGUIPane::UpdateAUX2Data(tint32 iID, tint32 iValue)
{
	
	switch(iID){
	}	
}

