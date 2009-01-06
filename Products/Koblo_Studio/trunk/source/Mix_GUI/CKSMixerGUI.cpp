
#include "KSOS.h"

//===================================================================================================
//===================================================================================================
//===================================================================================================
//
//
//
//
//
//===================================================================================================
//===================================================================================================
//===================================================================================================



CKSMixerGUI::CKSMixerGUI( de::IParameterManager* pParmMan)
	: CKSBaseGUI( pParmMan),
	CBaseGUI(dynamic_cast<CBasePlugIn*>(gpApplication), pParmMan)
{
}

CKSMixerGUI::~CKSMixerGUI()
{
}

void CKSMixerGUI::Destroy()
{
	gpApplication->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CKSMixerGUI*>(this);
}

void CKSMixerGUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	*piSizeX = gMix.iCX;
	*piSizeY = gMix.iCY;
}


void CKSMixerGUI::Initialize()
{
	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CKSMixerGUIPane(NULL, dynamic_cast<CBaseGUI*>(this));
	pPane->Init();
	SetPane(pPane);
	// This is the top pane, so set it in window
	mpWindow->SetPane(pPane->GetPane());

	ConnectControls();

//#ifdef WIN32
	mpContext->SetMinWindowSize(mpWindow, gMix_Min);
	mpContext->SetMaxWindowSize(mpWindow, gMix_Max);
//#endif // WIN32

	//ITimer* pTimerMeters = ITimer::Create();
	//pTimerMeters->Init(giTimerID_Meters_Mixer, this, 50);
}

void CKSMixerGUI::ConnectControls()
{
	dynamic_cast <CKSMixerGUIPane*>( GetPane())->ConnectControls();
}

void CKSMixerGUI::UpdateEngineData(tint iType, tint iExtra, tint32 iID, tint32 iValue)
{
	dynamic_cast <CKSMixerGUIPane*>( GetPane())->UpdateEngineData( iID, iExtra,  iID,  iValue);
}

void CKSMixerGUI::UpdateTrackData(tint32 iID, tint32 iValue, tint32 iChannel)
{

	dynamic_cast <CKSMixerGUIPane*>( GetPane())->UpdateTrackData( iID,  iValue,  iChannel);
}

void CKSMixerGUI::UpdateGUIData(tint32 iID, tint32 iValue)
{
	dynamic_cast <CKSMixerGUIPane*>( GetPane())->UpdateGUIData( iID, iValue);
}

void CKSMixerGUI::UpdateCommandData(tint32 iID, tint32 iValue)
{
	dynamic_cast <CKSMixerGUIPane*>( GetPane())->UpdateCommandData( iID, iValue);
}

void CKSMixerGUI::UpdateBussData(tint32 iID, tint32 iValue, tint32 iBuss)
{

	dynamic_cast <CKSMixerGUIPane*>( GetPane())->UpdateBussData( iID,  iValue,  iBuss);
}

void CKSMixerGUI::UpdateMasterData(tint32 iID, tint32 iValue)
{

//	mpAuxMonitor_GUI->UpdateData( iID,  iValue);
//	dynamic_cast <CAuxMonitor_GUIPane*>( GetPane())->UpdateData( iID,  iValue);
}

/*
void CKSMixerGUI::UpdateAuxMonitorData(tint32 iID, tint32 iValue)
{

//	mpAuxMonitor_GUI->UpdateData( iID,  iValue);
//	dynamic_cast <CAuxMonitor_GUIPane*>( GetPane())->UpdateData( iID,  iValue);
//	dynamic_cast <CKSMixerGUIPane*>( GetPane())->UpdateAuxMonitorData( iID,  iValue);
}
*/


/*
	IEventReceiver
*/

void CKSMixerGUI::EventValueChange(ge::IControl* pSender, tint32 iValueNew)
{
	// Notify the parm manager that a control has changed
//	mpParmMan->ControlUpdate(pSender->GetID(), iValueNew);
}

void CKSMixerGUI::EventGeneric(ge::IControl* pControl, void *pEventData)
{
/*
	tint32 iID = pControl->GetID() - giControlIDCh1Offset;
	if (iID >= 0) {
		tint32 iChannel = iID / giNrOfChCtls;
		iID = iID - ( iChannel * giNrOfChCtls);
		switch(iID) {
			case giChC_Txt:
				std::string sText = std::string((const tchar*)pEventData);
				gpApplication->SetChannelName(iChannel, sText);
				break;
		}
	}
*/
}

void CKSMixerGUI::GetName(tchar* psz)
{
	strcpy((char*)psz, gpApplication->GetProductName().c_str());
}

void CKSMixerGUI::OnTimer()
{
	CBaseGUI::OnTimer();
}


void CKSMixerGUI::OnTimer(tint32 iTimerID)
{
	CKSMixerGUIPane* pPane = dynamic_cast<CKSMixerGUIPane*>(GetPane());
	if (pPane) {
		pPane->OnTimer(iTimerID);
	}
}

void CKSMixerGUI::UpdateChannelName(tint32 iChannel, const std::string& sName)
{
	dynamic_cast<CKSMixerGUIPane*>(GetPane())->UpdateChannelName(iChannel, sName);
}
