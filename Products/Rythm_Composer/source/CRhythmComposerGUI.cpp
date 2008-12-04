


#include "RhythmComposerOS.h"



const ge::SSize gSizeVU(5, 143);

CRhythmComposerGUI::CRhythmComposerGUI(CDigitalEchoPlugIn* pPlugIn, de::IParameterManager* pParmMan)
	: CBaseGUI(dynamic_cast<CBasePlugIn*>(pPlugIn), pParmMan)
{
}

CRhythmComposerGUI::~CRhythmComposerGUI()
{
}

void CRhythmComposerGUI::Destroy()
{
	GetPlugIn()->GUIDestroyed(dynamic_cast<CBaseGUI*>(this));

	delete dynamic_cast<CRhythmComposerGUI*>(this);
}

void CRhythmComposerGUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	// TODO: Change the X and Y size of your GUI to fit the background image wou want
	*piSizeX = 576;
	*piSizeY = 345;
}

void CRhythmComposerGUI::Initialize()
{
	mpWindow->SetFineAdjustModifier(ge::IWindow::ModifierControl);

	CBasePane* pPane = new CRhythmComposerPane(NULL, dynamic_cast<CBaseGUI*>(this));
	pPane->Init();
	SetPane(pPane);
	// This is the top pane, so set it in window
	mpWindow->SetPane(pPane->GetPane());

	dynamic_cast<CRhythmComposerPane*>(pPane)->ConnectControls();
//	ConnectControls();
}

void CRhythmComposerGUI::ConnectControls()
{

	mpParmMan->ControlUpdateAll();

	mpParmMan->ParameterUpdateAll();
}

void CRhythmComposerGUI::UpdateEngineData(tint iType, tint iExtra, tint32 iID, tint32 iValue)
{
}

/*
	IEventReceiver
*/

void CRhythmComposerGUI::EventValueChange(ge::IControl *pSender, tint32 iValueNew)
{
	// Notify the parm manager that a control has changed
//	mpParmMan->ControlUpdate(pSender->GetID(), iValueNew);
}

void CRhythmComposerGUI::EventGeneric(ge::IControl *pSender, void *pEventData)
{
}

void CRhythmComposerGUI::GetName(tchar* psz)
{
	strcpy((char*)psz, ((CBasePlugIn*)GetPlugIn())->GetProductName().c_str());
}

void CRhythmComposerGUI::OnTimer()
{
	CBaseGUI::OnTimer();
}

