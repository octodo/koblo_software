
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