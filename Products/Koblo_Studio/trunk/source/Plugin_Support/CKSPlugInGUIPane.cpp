
#include "KSOS.h"


CKSPlugInGUIPane::CKSPlugInGUIPane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
}

CKSPlugInGUIPane::~CKSPlugInGUIPane()
{
}

void CKSPlugInGUIPane::SetInfo(tint32 iSectionID, tint32 iInsertIndex)
{
	miSectionID = iSectionID;
	miInsertIndex = iInsertIndex;

	mpToolbar->SetInfo(miSectionID, miInsertIndex);
}

void CKSPlugInGUIPane::Init()
{
	mpPane = ge::IPane::Create();
	mpPane->SetSize(ge::SSize(396, 124));
	mpPane->SetBackgroundColour(ge::SRGB(0, 0, 0));
	
	
	// Max add new tool bar pane to mpPane pane here
	
	//--------------------------------------------
	// Track_File_Browser
	// This is a container that scrools when KS plays
	mpToolbar = new CPlugin_Toolbar(this, GetGUI());
//	mpToolbar->SetInfo(miSectionID, miInsertIndex);
	mpToolbar->Init();
	mpPane->AddControl(mpToolbar->GetPane(), ge::SPos(0, 0));
	
	
	// Top label
//	CreateBitmap(ge::IControl::giNoID, IDB_Back_Plug_File_Browser_Left, ge::SPos(0, 24));
//	ge::IBitmap* pBmp =CreateBitmap(ge::IControl::giNoID, IDB_Back_Plug_File_Browser_Center, ge::SPos(16, 24));
//	pBmp->SetSize(ge::SSize( 365 , 100  ));
	
//	CreateBitmap(ge::IControl::giNoID, IDB_Back_Plug_File_Browser_Right, ge::SPos(365+16, 24));

	mpEffectPane = new CKSPlugInEffectPane(this, GetGUI());
	mpEffectPane->Init();
	mpPane->AddControl(mpEffectPane->GetPane(), ge::SPos(0, 24));
}

void CKSPlugInGUIPane::ConnectControls()
{
	mpToolbar->ConnectControls();
}

void CKSPlugInGUIPane::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	switch(pControl->GetID()) {
	}

	GetParmMan()->ControlUpdate(miPaneID, pControl->GetID(), iValueNew); 
}

void CKSPlugInGUIPane::SetGUI(kspi::IGUI* pGUI)
{
	mpEffectPane->SetGUI(pGUI);
}

void CKSPlugInGUIPane::SetWidth(tuint32 uiNew)
{
	mpPane->SetSize(ge::SSize(uiNew, 124));
	GetWindow()->SetSize(ge::SSize(uiNew, 124));

	mpToolbar->SetWidth(uiNew);

	ge::SRect rct(ge::SPos(0, 0), ge::SSize(uiNew, 124));
	mpPane->Redraw(rct);
}

void CKSPlugInGUIPane::OnCloseGUI()
{
	mpEffectPane->OnCloseGUI();

	gpApplication->GetPlugInManager()->WindowClosed(miSectionID, miInsertIndex);
}

void CKSPlugInGUIPane::UpdateChannelData(tint32 iID, tint32 iValue, tint32 iChannel)
{

}

void CKSPlugInGUIPane::UpdateChannelName(tint32 iChannel, const std::string& sName)
{
	mpToolbar->UpdateChannelName(iChannel, sName);
}




