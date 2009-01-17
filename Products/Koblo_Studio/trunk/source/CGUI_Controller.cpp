
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
	tbool bReallyVisible = gpApplication->GetModule()->GetHost()->IsWindowVisible(giMix_Window) == 0 ? false : true;
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

void CGUI_Controller::Select_All_Regions()
{
	
}

void CGUI_Controller::Close_Window()
{
	
}

void CGUI_Controller::Toggle_Window()
{
	
}

void CGUI_Controller::Show_Hide_Plugins()
{
	
}