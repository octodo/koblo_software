// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
//
// This file is part of the Koblo SDK.
//
// the Koblo SDK is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo SDK is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.



/* Parameter ID's
	KS Note.
	Paremeters are organized after the screen's where they belong
	
	!To Do....
	Get a more dynamic aproach to parameters so it's possible to add modules and parameters in the middel of a 
	parameter block without corupting old projects

*/

enum ESectionsParms
{
	
	giSectionGlobal	=	0,
	giSectionGUI,
//	giSectionAuxMonitor,
//	giSectionTransport,
	giSection_First_Track,
	giSection_Last_Track	= giSection_First_Track + giNumber_Of_Tracks - 1 ,
	giSection_First_Buss,
	giSection_Last_Buss		= giSection_First_Buss   + giNumber_Of_Busses - 1,
	giSection_Master,
	giSection_AUX1,
	giSection_AUX2,
	giNrOfSections
};

enum EGlobal_ParmID
{	
	giParmID_KS_Bypass = 0,
	//giParamID_KS_Nr_Tracks,
	giParamID_KS_Tempo,
	giParamID_KS_Time_Signature,
	giParamID_Info_Radio_Button,
	giParamID_Info_Verify_Rights,
	giParamID_Loop_On,
	giParamID_Loop_Start,
	giParamID_Loop_End,
	giParamID_KS_Snap_To,
	giParamID_Project_ID,
	giParamID_Branch_ID,
	giParamID_Revision_Nr,
	


	giNr_Of_Global_Parms

};

enum EGUI_ParmID
{
	giParmID_GUI_First = 0,
	giParamID_Tool_Selected,
	giParamID_Track_Selected,
	giParamID_Zoom,
	giParamID_Grid,
	giParamID_Show_AUX_Window,
	giParamID_Show_Mix_Window,
	giParamID_Show_Track_Window,
	giParamID_Show_Export_Window,
	giParamID_Export_Type,
	giParamID_Export_Wave,
	giParamID_Export_Compression,
	giParamID_Export_Tail,
	giParamID_Show_Export_For_Web_Window,
	giParamID_CC_License_Type,
	giParamID_Show_Import_Window,
	giParamID_Show_Waveform,
	giParamID_Show_Fade,
	giParamID_Audio_Setup_Window,
	giParamID_Normalize_On_Export,
	giParamID_Show_Projec_ID_Window,
	giParamID_Show_Sign_In_Window,
//	giParamID_Remember_Username_and_Password,

	

	giNr_Of_GUI_Parms

};

enum ETrack_ParmID
{
 
	giFirst_Track_Param		= 0,
	giParam_ChIn,
	giParam_ChInMode,
	giParam_ChInGain,
	giParam_ChMode,
	giParam_ChOut,
	giParam_ChAUX1OnOff,	// Not Used
	giParam_ChAUX2OnOff,	// Not Used
	giParam_ChAUX3OnOff,	// Not Used
	giParam_ChAUX4OnOff,	// Not Used
	giParam_ChAUX5OnOff,	// Not Used
	giParam_ChAUX6OnOff,	// Not Used
	giParam_ChAUX1,			// Echo
	giParam_ChAUX2,			// Reverb
	giParam_ChAUX3,			// Not Used
	giParam_ChAUX4,			// Not Used
	giParam_ChAUX5,			// Not Used
	giParam_ChAUX6,			// Not Used
	giParam_ChInsert1,
	giParam_ChInsert2,
	giParam_ChInsert3,
	giParam_ChInsert4,
	giParam_ChInsert5,
	giParam_ChInsert6,
	giParam_ChInsert7,
	giParam_ChInsert8,
	giParam_ChInsert1File,
	giParam_ChInsert2File,
	giParam_ChInsert3File,
	giParam_ChInsert4File,
	giParam_ChInsert5File,
	giParam_ChInsert6File,
	giParam_ChInsert7File,
	giParam_ChInsert8File,
	giParam_Track_Info_SizeY,
	giParam_ChVol,
	giParam_ChSolo,
	giParam_ChMute,
	giParam_ChSoftMute,
	giParam_ChArm,
	giParam_Track_Info_Show_Inserts,
	giParam_ChPannerMode,
	giParam_ChPannerLeftRight,
	giParam_ChPannerFrontBack,
	giParam_Ch_GUI_Position,
	giParam_Ch_In_Use,
	giParam_Ch_Insert1GUIOpen,
	giParam_Ch_Insert2GUIOpen,
	giParam_Ch_Insert3GUIOpen,
	giParam_Ch_Insert4GUIOpen,
	giParam_Ch_Insert5GUIOpen,
	giParam_Ch_Insert6GUIOpen,
	giParam_Ch_Insert7GUIOpen,
	giParam_Ch_Insert8GUIOpen,
	giParam_Ch_Insert1Bypass,
	giParam_Ch_Insert2Bypass,
	giParam_Ch_Insert3Bypass,
	giParam_Ch_Insert4Bypass,
	giParam_Ch_Insert5Bypass,
	giParam_Ch_Insert6Bypass,
	giParam_Ch_Insert7Bypass,
	giParam_Ch_Insert8Bypass,

	giLast_Track_Param
};



enum EBuss_ParmID
{
	giFirst_Buss_Param		= 0,
	giParam_Buss_In,
	giParam_Buss_InMode,
	giParam_Buss_InGain,
	giParam_Buss_Mode,
	giParam_Buss_Out,
	giParam_Buss_AUX1OnOff,	// Not Used
	giParam_Buss_AUX2OnOff,	// Not Used
	giParam_Buss_AUX3OnOff,	// Not Used
	giParam_Buss_AUX4OnOff,	// Not Used
	giParam_Buss_AUX5OnOff,	// Not Used
	giParam_Buss_AUX6OnOff,	// Not Used
	giParam_Buss_AUX1,			// Echo
	giParam_Buss_AUX2,			// Reverb
	giParam_Buss_AUX3,			// Not Used
	giParam_Buss_AUX4,			// Not Used
	giParam_Buss_AUX5,			// Not Used
	giParam_Buss_AUX6,			// Not Used
	giParam_Buss_Insert1,
	giParam_Buss_Insert2,
	giParam_Buss_Insert3,
	giParam_Buss_Insert4,
	giParam_Buss_Insert5,
	giParam_Buss_Insert6,
	giParam_Buss_Insert7,
	giParam_Buss_Insert8,
	giParam_Buss_Insert1File,
	giParam_Buss_Insert2File,
	giParam_Buss_Insert3File,
	giParam_Buss_Insert4File,
	giParam_Buss_Insert5File,
	giParam_Buss_Insert6File,
	giParam_Buss_Insert7File,
	giParam_Buss_Insert8File,
	giParam_Buss_Info_SizeY,
	giParam_Buss_Vol,
	giParam_Buss_Solo,
	giParam_Buss_Mute,
	giParam_Buss_SoftMute,
	giParam_Buss_Arm,
	giParam_Buss_Info_Show_Inserts,
	giParam_Buss_PannerMode,
	giParam_Buss_PannerLeftRight,
	giParam_Buss_PannerFrontBack,
	giParam_Buss_Insert1GUIOpen,
	giParam_Buss_Insert2GUIOpen,
	giParam_Buss_Insert3GUIOpen,
	giParam_Buss_Insert4GUIOpen,
	giParam_Buss_Insert5GUIOpen,
	giParam_Buss_Insert6GUIOpen,
	giParam_Buss_Insert7GUIOpen,
	giParam_Buss_Insert8GUIOpen,
	giParam_Bus_Insert1Bypass,
	giParam_Bus_Insert2Bypass,
	giParam_Bus_Insert3Bypass,
	giParam_Bus_Insert4Bypass,
	giParam_Bus_Insert5Bypass,
	giParam_Bus_Insert6Bypass,
	giParam_Bus_Insert7Bypass,
	giParam_Bus_Insert8Bypass,

	giLast_Buss_Param
};



enum EAUX1ParmID
{	
	giAUX1_First_Param = 0,
	giAUX1_Param_Delay_Power,
	giAUX1_Param_Delay_Time,
	giAUX1_Param_Feedback,
	giAUX1_Param_HP_Damping,
	giAUX1_Param_LP_Damping,
	giAUX1_Param_Sync,
	
};

enum EAUX2ParmID
{	
	giFirst_AUX2_Param = 0,
	giParam_Reverb_On,
	giParam_Reverb_Size,
	giParam_Reverb_PDelay,
	giParam_Reverb_Damp,
	giParam_Reverb_Panorama,
	giParam_Reverb_Color1,
	giParam_Reverb_Color2,
	giParam_Reverb_Mix,
	giLast_AUX1_Param
	
};

enum EMasterParmID
{	
	giFirst_Master_Param = 0,
	giParam_Master_Show_Inserts,
	giParam_Master_Out,
	giParam_Master_Volume,
	giParam_Master_Insert1,
	giParam_Master_Insert2,
	giParam_Master_Insert3,
	giParam_Master_Insert4,
	giParam_Master_Insert5,
	giParam_Master_Insert6,
	giParam_Master_Insert7,
	giParam_Master_Insert8,
	giParam_Ch_Master1GUIOpen,
	giParam_Ch_Master2GUIOpen,
	giParam_Ch_Master3GUIOpen,
	giParam_Ch_Master4GUIOpen,
	giParam_Ch_Master5GUIOpen,
	giParam_Ch_Master6GUIOpen,
	giParam_Ch_Master7GUIOpen,
	giParam_Ch_Master8GUIOpen,
	giParam_Master_Insert1Bypass,
	giParam_Master_Insert2Bypass,
	giParam_Master_Insert3Bypass,
	giParam_Master_Insert4Bypass,
	giParam_Master_Insert5Bypass,
	giParam_Master_Insert6Bypass,
	giParam_Master_Insert7Bypass,
	giParam_Master_Insert8Bypass,

	giLast_Master_Param,
};

/*
enum EMasterParmID
{	
	giFirst_AUX2_Param = 0,
	
	giLast_AUX2_Param,
	
};

*/
enum EParmIOOffsets
{
	ParmIOOffsetMix = 0,
	ParmIOOffsetBus = 100
};


	

