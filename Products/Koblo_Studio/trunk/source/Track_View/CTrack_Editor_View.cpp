
#include "KSOS.h"

CTrack_Editor_View::CTrack_Editor_View(CKSPlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CKSBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan),
	CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan)
{
		mpTimerMeters = NULL;
		mpPlay_Head_Timer = NULL;
}

CTrack_Editor_View::~CTrack_Editor_View()
{
}

void CTrack_Editor_View::Destroy()
{

	gpApplication->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CTrack_Editor_View*>(this);
}

void CTrack_Editor_View::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	*piSizeX = gTrack_Editor.iCX;
	*piSizeY = gTrack_Editor.iCY;
}


void CTrack_Editor_View::Initialize()
{

	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CTrack_Editor_Pane(NULL, dynamic_cast<CBaseGUI*>(this));
	pPane->Init();
	SetPane(pPane);
	// This is the top pane, so set it in window
	mpWindow->SetPane(pPane->GetPane());

	ConnectControls();
	
	//mpPlay_Head_Timer = ITimer::Create();
	//mpPlay_Head_Timer->Init(giTimerID_PlayHead, this, 159);

	//ITimer::Create()->Init(giTimerID_PlayTime_Display, this, 50);

	mpTimerMeters = ITimer::Create();
	mpTimerMeters->Init(giTimerID_Meters_Track, this, 50);
}

void CTrack_Editor_View::ConnectControls()
{
	dynamic_cast <CTrack_Editor_Pane*>( GetPane())->ConnectControls();
}

void CTrack_Editor_View::UpdateEngineData(tint iType, tint iExtra, tint32 iID, tint32 iValue)
{
	dynamic_cast <CTrack_Editor_Pane*>( GetPane())->UpdateGlobalData( iID, iExtra,  iID,  iValue);
}

void CTrack_Editor_View::UpdateTrackData(tint32 iID, tint32 iValue, tint32 iChannel)
{

	dynamic_cast <CTrack_Editor_Pane*>( GetPane())->UpdateTrackData( iID,  iValue,  iChannel);
}

void CTrack_Editor_View::UpdateGUIData(tint32 iID, tint32 iValue)
{
	dynamic_cast <CTrack_Editor_Pane*>( GetPane())->UpdateGUIData( iID, iValue);
}

void CTrack_Editor_View::UpdateCommandData(tint32 iID, tint32 iValue)
{
	dynamic_cast <CTrack_Editor_Pane*>( GetPane())->UpdateCommandData( iID, iValue);
}

void CTrack_Editor_View::UpdateBussData(tint32 iID, tint32 iValue, tint32 iBuss)
{

	dynamic_cast <CTrack_Editor_Pane*>( GetPane())->UpdateBussData( iID,  iValue,  iBuss);
}

void CTrack_Editor_View::UpdateMasterData(tint32 iID, tint32 iValue)
{

//	mpAuxMonitor_GUI->UpdateData( iID,  iValue);
//	dynamic_cast <CAuxMonitor_GUIPane*>( GetPane())->UpdateData( iID,  iValue);
}

/*
void CTrack_Editor_View::UpdateAuxMonitorData(tint32 iID, tint32 iValue)
{

//	mpAuxMonitor_GUI->UpdateData( iID,  iValue);
//	dynamic_cast <CAuxMonitor_GUIPane*>( GetPane())->UpdateData( iID,  iValue);
//	dynamic_cast <CTrack_Editor_Pane*>( GetPane())->UpdateAuxMonitorData( iID,  iValue);
}
*/


/*
	IEventReceiver
*/

void CTrack_Editor_View::EventValueChange(ge::IControl* pSender, tint32 iValueNew)
{
	// Notify the parm manager that a control has changed
//	mpParmMan->ControlUpdate(pSender->GetID(), iValueNew);
}

void CTrack_Editor_View::EventGeneric(ge::IControl* pControl, void *pEventData)
{
	/*
	tint32 iID = pControl->GetID() - giControlIDCh1Offset;
	if (iID >= 0) {
		tint32 iChannel = iID / giNrOfChCtls;
		iID = iID - ( iChannel * giNrOfChCtls);
		switch(iID) {
			case giChC_Txt:
				std::string sText = std::string((const tchar*)pEventData);
				dynamic_cast<CKSPlugIn*>(gpApplication)->SetChannelName(iChannel, sText);
				break;
		}
	}
*/
}

void CTrack_Editor_View::GetName(tchar* psz)
{
	strcpy((char*)psz, gpApplication->GetProductName().c_str());
}

void CTrack_Editor_View::OnTimer()
{
	CBaseGUI::OnTimer();
}


void CTrack_Editor_View::OnTimer(tint32 iTimerID)
{
	CTrack_Editor_Pane* pPane = dynamic_cast<CTrack_Editor_Pane*>(GetPane());
	if (pPane) {
		pPane->OnTimer(iTimerID);
	}
}

void CTrack_Editor_View::UpdateChannelName(tint32 iChannel, const std::string& sName)
{
	dynamic_cast<CTrack_Editor_Pane*>(GetPane())->UpdateChannelName(iChannel, sName);
}

/*
void CTrack_Editor_View::AddTrack()
{
	dynamic_cast<CTrack_Editor_Pane*>(GetPane())->AddTrack();

}

void CTrack_Editor_View::DeleteTrack()
{
	dynamic_cast<CTrack_Editor_Pane*>(GetPane())->DeleteTrack();

}



void CTrack_Editor_View::SelectTrack(tint32 iID)
{
	dynamic_cast<CTrack_Editor_Pane*>(GetPane())->SelectTrack(iID);
}

*/

tbool CTrack_Editor_View::WindowClosed(void* pWnd)
{
	if (mhWndParent != pWnd) {
		return false;
	}

#ifdef _WIN32
	::PostQuitMessage(0);
#endif

	return true;
}