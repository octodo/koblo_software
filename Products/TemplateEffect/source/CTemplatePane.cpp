
#include "TemplateOS.h"


CTemplatePane::CTemplatePane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
}

CTemplatePane::~CTemplatePane()
{
}

void CTemplatePane::Init()
{
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(600, 400));
}

void CTemplatePane::ConnectControls()
{
	// TODO: Connect control IDs (for GUI use) to parm-IDs (for parameter-manager use) below here.
	// Use the CBaseGUI::RegisterGlobalControlXxx...() methods.
	// HELP: file://C:\Development\Source~1\SDK\BaseClasses\documentation\Docs\html\class_c_base_g_u_i.html
	// NOTE: Remember to first add the parm-IDs to the parameter-manager (in CDigitalEchoPlugIn::AddParameters() ).
	// NOTE: Several control-IDs may be connected to one parm-ID.
	// NOTE: Only one parm-ID may be connected to each control-ID.
	
	
	mpParmMan->ControlUpdateAll();
	mpParmMan->ParameterUpdateAll();
}

void CTemplatePane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	switch(pControl->GetID()) {
	}

	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew); 
}


