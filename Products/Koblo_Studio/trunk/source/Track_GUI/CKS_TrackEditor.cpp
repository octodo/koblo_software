
#include "KSOS.h"

CKS_TrackEditor::CKS_TrackEditor(CKSPlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CKSBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan),
	CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan)
{
		mpTimerMeters = NULL;
		mpPlay_Head_Timer = NULL;
}

CKS_TrackEditor::~CKS_TrackEditor()
{
}

void CKS_TrackEditor::Destroy()
{

	GetPlugIn()->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CKS_TrackEditor*>(this);
}

void CKS_TrackEditor::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	*piSizeX = gTrack_Editor.iCX;
	*piSizeY = gTrack_Editor.iCY;
}


void CKS_TrackEditor::Initialize()
{

	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CKS_TrackEditorPane(NULL, dynamic_cast<CBaseGUI*>(this));
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

void CKS_TrackEditor::ConnectControls()
{
	dynamic_cast <CKS_TrackEditorPane*>( GetPane())->ConnectControls();
}

void CKS_TrackEditor::UpdateEngineData(tint iType, tint iExtra, tint32 iID, tint32 iValue)
{
	dynamic_cast <CKS_TrackEditorPane*>( GetPane())->UpdateGlobalData( iID, iExtra,  iID,  iValue);
}

void CKS_TrackEditor::UpdateTrackData(tint32 iID, tint32 iValue, tint32 iChannel)
{

	dynamic_cast <CKS_TrackEditorPane*>( GetPane())->UpdateTrackData( iID,  iValue,  iChannel);
}

void CKS_TrackEditor::UpdateGUIData(tint32 iID, tint32 iValue)
{
	dynamic_cast <CKS_TrackEditorPane*>( GetPane())->UpdateGUIData( iID, iValue);
}

void CKS_TrackEditor::UpdateCommandData(tint32 iID, tint32 iValue)
{
	dynamic_cast <CKS_TrackEditorPane*>( GetPane())->UpdateCommandData( iID, iValue);
}

void CKS_TrackEditor::UpdateBussData(tint32 iID, tint32 iValue, tint32 iBuss)
{

	dynamic_cast <CKS_TrackEditorPane*>( GetPane())->UpdateBussData( iID,  iValue,  iBuss);
}

void CKS_TrackEditor::UpdateMasterData(tint32 iID, tint32 iValue)
{

//	mpAuxMonitor_GUI->UpdateData( iID,  iValue);
//	dynamic_cast <CAuxMonitor_GUIPane*>( GetPane())->UpdateData( iID,  iValue);
}

/*
void CKS_TrackEditor::UpdateAuxMonitorData(tint32 iID, tint32 iValue)
{

//	mpAuxMonitor_GUI->UpdateData( iID,  iValue);
//	dynamic_cast <CAuxMonitor_GUIPane*>( GetPane())->UpdateData( iID,  iValue);
//	dynamic_cast <CKS_TrackEditorPane*>( GetPane())->UpdateAuxMonitorData( iID,  iValue);
}
*/


/*
	IEventReceiver
*/

void CKS_TrackEditor::EventValueChange(ge::IControl* pSender, tint32 iValueNew)
{
	// Notify the parm manager that a control has changed
//	mpParmMan->ControlUpdate(pSender->GetID(), iValueNew);
}

void CKS_TrackEditor::EventGeneric(ge::IControl* pControl, void *pEventData)
{
	/*
	tint32 iID = pControl->GetID() - giControlIDCh1Offset;
	if (iID >= 0) {
		tint32 iChannel = iID / giNrOfChCtls;
		iID = iID - ( iChannel * giNrOfChCtls);
		switch(iID) {
			case giChC_Txt:
				std::string sText = std::string((const tchar*)pEventData);
				dynamic_cast<CKSPlugIn*>(GetPlugIn())->SetChannelName(iChannel, sText);
				break;
		}
	}
*/
}

void CKS_TrackEditor::GetName(tchar* psz)
{
	strcpy((char*)psz, GetPlugIn()->GetProductName().c_str());
}

void CKS_TrackEditor::OnTimer()
{
	CBaseGUI::OnTimer();
}


void CKS_TrackEditor::OnTimer(tint32 iTimerID)
{
	CKS_TrackEditorPane* pPane = dynamic_cast<CKS_TrackEditorPane*>(GetPane());
	if (pPane) {
		pPane->OnTimer(iTimerID);
	}
}

void CKS_TrackEditor::UpdateChannelName(tint32 iChannel, const std::string& sName)
{
	dynamic_cast<CKS_TrackEditorPane*>(GetPane())->UpdateChannelName(iChannel, sName);
}

/*
void CKS_TrackEditor::AddTrack()
{
	dynamic_cast<CKS_TrackEditorPane*>(GetPane())->AddTrack();

}

void CKS_TrackEditor::DeleteTrack()
{
	dynamic_cast<CKS_TrackEditorPane*>(GetPane())->DeleteTrack();

}



void CKS_TrackEditor::SelectTrack(tint32 iID)
{
	dynamic_cast<CKS_TrackEditorPane*>(GetPane())->SelectTrack(iID);
}

*/

tbool CKS_TrackEditor::WindowClosed(void* pWnd)
{
	if (mhWndParent != pWnd) {
		return false;
	}

#ifdef _WIN32
	::PostQuitMessage(0);
#endif

	return true;
}