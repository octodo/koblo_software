
#include "KSOS.h"


CGUI_Controller::CGUI_Controller()
{
	
}

CGUI_Controller::~CGUI_Controller()
{
	
}

void CGUI_Controller::Open_Close_Mix_Window()
{
	//!!! sugestion ; change code so if the window is open and not in front bring it to front
	
	tbool bTest = (gpApplication->GetGlobalParm(giParamID_Show_Mix_Window, giSectionGUI) != 0);
	tbool bReallyVisible = gpApplication->GetModule()->GetHost()->IsWindowVisible(giMix_Window) != 0 ;
	if (bTest != bReallyVisible) {
		gpApplication->GetParmMan()->Set(true, !bTest, giParamID_Show_Mix_Window, de::IParameterManager::TypeGlobal, giSectionGUI);
	}
	gpApplication->GetParmMan()->Set(true, !bReallyVisible, giParamID_Show_Mix_Window, de::IParameterManager::TypeGlobal, giSectionGUI);
	
}

void CGUI_Controller::Open_Close_Rack_Window()
{
	tbool bTest = (gpApplication->GetGlobalParm(giParamID_Show_AUX_Window, giSectionGUI) != 0);
	tbool bReallyVisible = (gpApplication->GetModule()->GetHost()->IsWindowVisible(giRack_Window) != 0);
	if (bTest != bReallyVisible) {
		gpApplication->GetParmMan()->Set(true, !bTest, giParamID_Show_AUX_Window, de::IParameterManager::TypeGlobal, giSectionGUI);
	}
	gpApplication->GetParmMan()->Set(true, !bReallyVisible, giParamID_Show_AUX_Window, de::IParameterManager::TypeGlobal, giSectionGUI);
	
}

void CGUI_Controller::Open_Close_Import_Audio_Window()
{
	if (gpApplication->No_Project_Is_Loaded()) {
		gpApplication->ShowMessageBox("You must create or load a project before importing audio", "Sorry");
		return;
	}
	
	
	tbool bTest = (gpApplication->GetGlobalParm(giParamID_Show_Import_Window, giSectionGUI) != 0);
	if(!bTest){
		gpApplication->SetGlobalParm(giParamID_Show_Import_Window,true, giSectionGUI);
	}
	else
		gpApplication->GetModule()->GetHost()->ActivateWindow(giImport_Audio_Window);
	
}

void CGUI_Controller::Select_All_Regions()
{
	
}

void CGUI_Controller::Close_Window()
{
	
}

void CGUI_Controller::Toggle_Window()
{
	
}

void CGUI_Controller::Show_All_Plugins(tbool bShow)
{
	for (tint32 i = 0; i < giNumber_Of_Tracks; i++) {
		
		if(bShow){
			gpApplication->GetPlugInManager()->OpenGUI(i, 0);
			gpApplication->GetPlugInManager()->OpenGUI(i, 1);
			gpApplication->GetPlugInManager()->OpenGUI(i, 2);
			gpApplication->GetPlugInManager()->OpenGUI(i, 3);
		}
		else{
			gpDSPEngine->Close_Plug_In_GUI(i,0);
			gpDSPEngine->Close_Plug_In_GUI(i,1);
			gpDSPEngine->Close_Plug_In_GUI(i,2);
			gpDSPEngine->Close_Plug_In_GUI(i,3);
		}
	}
}

void CGUI_Controller::Update_Windows() 
{
	// mix window
	tbool bVisibel	=	gpApplication->GetGlobalParm(giParamID_Show_Mix_Window,  giSectionGUI);
	if(bVisibel )
		gpApplication->GetModule()->GetHost()->ActivateWindow(giMix_Window);
	else
		gpApplication->GetModule()->GetHost()->HideWindow(giMix_Window);
	

	// rack window
	bVisibel		=	gpApplication->GetGlobalParm(giParamID_Show_AUX_Window,  giSectionGUI);
	if(bVisibel )
		gpApplication->GetModule()->GetHost()->ActivateWindow(giRack_Window);
	else
		gpApplication->GetModule()->GetHost()->HideWindow(giRack_Window);

}







