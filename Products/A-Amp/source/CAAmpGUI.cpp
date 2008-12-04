


#include "AAmpOS.h"



const ge::SSize gSizeVU(5, 143);

CAAmpGUI::CAAmpGUI(CAAmpPlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan)
{
	muiTube_Slot	= giCtrl_Slot1;
	muiHarm_Slot	= giCtrl_Slot2;
	muiLimiter_Slot = giCtrl_Slot3;
	
	
}

CAAmpGUI::~CAAmpGUI()
{
}

void CAAmpGUI::Destroy()
{
	GetPlugIn()->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CAAmpGUI*>(this);
}

void CAAmpGUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	// TODO: Change the X and Y size of your GUI to fit the background image wou want
	*piSizeX = 435;
	*piSizeY = 225;
}

void CAAmpGUI::Initialize()
{
	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CAAmpPane(NULL, dynamic_cast<CBaseGUI*>(this));
	pPane->Init();
	SetPane(pPane);
	// This is the top pane, so set it in window
	mpWindow->SetPane(pPane->GetPane());

	dynamic_cast<CAAmpPane*>(pPane)->ConnectControls();
//	ConnectControls();
}

void CAAmpGUI::ConnectControls()
{

	mpParmMan->ControlUpdateAll();

	mpParmMan->ParameterUpdateAll();
}

void CAAmpGUI::UpdateEngineData(tint iType, tint iExtra, tint32 iID, tint32 iValue)
{
	switch(iID) {
		
		case giParam_Tube:{
			dynamic_cast<ge::IBitmap*>(mpWindow->GetControl(muiTube_Slot))->SetFrame(iValue ? 1:0);
			break;
		}
		case giParam_Harm:{
			dynamic_cast<ge::IBitmap*>(mpWindow->GetControl(muiHarm_Slot))->SetFrame(iValue ? 2:0);
			break;
		}
		case giParam_Limiter:{
			dynamic_cast<ge::IBitmap*>(mpWindow->GetControl(muiLimiter_Slot))->SetFrame(iValue ? 3:0);
			break;
		}
		
		case giParam_Swap:{
			switch(iValue){
				case 0:
					muiTube_Slot	= giCtrl_Slot1;
					muiHarm_Slot	= giCtrl_Slot2;
					muiLimiter_Slot = giCtrl_Slot3;
					break;
				case 1:
					muiTube_Slot	= giCtrl_Slot1;
					muiHarm_Slot	= giCtrl_Slot3;
					muiLimiter_Slot = giCtrl_Slot2;
					break;
				case 2:
					muiTube_Slot	= giCtrl_Slot2;
					muiHarm_Slot	= giCtrl_Slot1;
					muiLimiter_Slot = giCtrl_Slot3;
					break;
				case 3:
					muiTube_Slot	= giCtrl_Slot2;
					muiHarm_Slot	= giCtrl_Slot3;
					muiLimiter_Slot = giCtrl_Slot1;
					break;
				case 4:
					muiTube_Slot	= giCtrl_Slot3;
					muiHarm_Slot	= giCtrl_Slot1;
					muiLimiter_Slot = giCtrl_Slot2;
					break;
				case 5:
					muiTube_Slot	= giCtrl_Slot3;
					muiHarm_Slot	= giCtrl_Slot2;
					muiLimiter_Slot = giCtrl_Slot1;
					break;
			}
			
			tbool bOn	= GetPlugIn()->GetGlobalParm(giParam_Tube, 0);
			dynamic_cast<ge::IBitmap*>(mpWindow->GetControl(muiTube_Slot))->SetFrame(bOn ? 1:0);
			
			bOn	= GetPlugIn()->GetGlobalParm(giParam_Harm, 0);
			dynamic_cast<ge::IBitmap*>(mpWindow->GetControl(muiHarm_Slot))->SetFrame(bOn ? 2:0);
			
			bOn	= GetPlugIn()->GetGlobalParm(giParam_Limiter, 0);
			dynamic_cast<ge::IBitmap*>(mpWindow->GetControl(muiLimiter_Slot))->SetFrame(bOn ? 3:0);
			
			break;
		}
	}
			
	
}

/*
	IEventReceiver
*/

void CAAmpGUI::EventValueChange(ge::IControl *pSender, tint32 iValueNew)
{
	// Notify the parm manager that a control has changed
//	mpParmMan->ControlUpdate(pSender->GetID(), iValueNew);
}

void CAAmpGUI::EventGeneric(ge::IControl *pSender, void *pEventData)
{
}

void CAAmpGUI::GetName(tchar* psz)
{
	strcpy((char*)psz, ((CBasePlugIn*)GetPlugIn())->GetProductName().c_str());
}

void CAAmpGUI::OnTimer()
{
	
	// Out
	float fVUOut = GetPlugIn()->GetMeterSignal(0);
	
	// Lower by 3 dB to match meter, and raise by 10dB (calibration)
//	fVUOut *= 2.2387211385683396119549508524657f;
	
	// Limit
	if (fVUOut >= 0.99999) {
		fVUOut = 0.99999f;
	}
	
	// Calc the frame
	double dFrame = fVUOut * 37;
	int iFrame = (int)dFrame;
	
	dynamic_cast<ge::IBitmap*>(mpWindow->GetControl(giCtrl_Meter))->SetFrame(iFrame);
	 
	CBaseGUI::OnTimer();
}

