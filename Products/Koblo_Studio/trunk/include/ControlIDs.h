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

//! Control id's with this value and above are reserved
const tint32 giControlIDFX = 10000;




/* Controll ID's
	KS Note.
	Control ID's are organized after the screen's where they belong

*/

/*
enum EToolCtrl
{
	giCtrl_ToolSelection = 1000000000,
};
*/




enum ETrack_Ctrls
{
	// Controlls connected to parameters
	giFirst_Track_Ctrl = 0,
	giTrack_Info_Small_Expand_Button,
	giTrack_Info_Small_Arm_Button,
	giTrack_Info_Small_Mute_Button,
	giTrack_Info_Small_Solo_Button,
	giTrack_Info_Small_Channel_Text,
	giTrack_Info_Big_Arm_Button,
	giTrack_Info_Big_Mute_Button,
	giTrack_Info_Big_Solo_Button,
	giTrack_Info_Big_Colaps_Button,
	giTrack_Info_Big_Input_Popup,
	giTrack_Info_Big_Output_Popup,
	giTrack_Info_Big_Show_Insert_Button,
	giTrack_Info_Big_Volume_Slider,
	giTrack_Info_Big_Pan,
	giTrack_Info_Big_Volume_Slider_Display,
	giTrack_Info_Big_Channel_Text,
	giCtrl_Track_Big_AUX1_Send,
	giCtrl_Track_Big_AUX2_Send,
	giCtr_Mix_Channel_Text,
	giChannel_Arm_Button,
	giChannel_Mute_Button,
	giChannel_Solo_Button,
	giChannel_Show_Insert_Button,
	giChannel_Volume_Slider,
	giChannel_Volume_Slider_Display,
	giChannel_Input_Popup,
	giChannel_Output_Popup,
	giChannel_Pan,
	//giCtr_Mix_Aux_ScrollPane_Timer,
	giCtrl_AUX1Send,
	giCtrl_AUX2Send,
	giCtrl_InsertPop1,
	giCtrl_InsertPop2,
	giCtrl_InsertPop3,
	giCtrl_InsertPop4,
	giCtrl_InsertTrackPop1,
	giCtrl_InsertTrackPop2,
	giCtrl_InsertTrackPop3,
	giCtrl_InsertTrackPop4,
	giCtrlOpenPlugEdit1,
	giCtrlOpenPlugEdit2,
	giCtrlOpenPlugEdit3,
	giCtrlOpenPlugEdit4,

	giNr_Of_Track_Ctls,

};

enum EBuss_Ctrls
{
	// Controlls connected to parameters
	giFirst_Buss_Ctrl = giNr_Of_Track_Ctls *giNumber_Of_Tracks,
	//giCtr_Mix_Buss_Aux_ScrollPane_Timer,
	giCtr_Mix_Buss_Text,
	giCtr_Mix_Buss_Arm_Button,
	giCtr_Mix_Buss_Mute_Button,
	giCtr_Mix_Buss_Solo_Button,
	giCtr_Mix_Buss_Show_Insert_Button,
	giCtr_Mix_Buss_Volume_Slider,
	giCtr_Mix_Buss_Volume_Slider_Display,
	giCtr_Mix_Buss_Output_Popup,
	giCtr_Mix_Buss_Pan,
	//gCtr_iCtr_Buss_Mix_Aux_ScrollPane_Timer,
		
	giNr_Of_Buss_Ctls,

};

enum E_Master_Ctrls
{
	// Mixer
	giCtr_Mix_Master_ID = giNr_Of_Buss_Ctls * giNumber_Of_Busses,
	giCtr_Mix_Master_Aux_ScrollPane_Timer,
	giCtr_Mix_Master_Text,
//	giCtr_Mix_Master_Mute_Button,
//	giCtr_Mix_Master_Solo_Button,
	giCtr_Mix_Master_Show_Insert_Button,
	giCtr_Mix_Master_Volume_Slider,
	giCtr_Mix_Master_Volume_Slider_Display,
	giCtrl_Mix_Master_Out_Popup,
	giCtrl_Lopp_Start,
	giCtrl_Loop_End,
	giCtrlm_InsertPop1,
	giCtrlm_InsertPop2,
	giCtrlm_InsertPop3,
	giCtrlm_InsertPop4,
	giCtrlm_InsertTrackPop1,
	giCtrlm_InsertTrackPop2,
	giCtrlm_InsertTrackPop3,
	giCtrlm_InsertTrackPop4,
	giCtrlmOpenPlugEdit1,
	giCtrlmOpenPlugEdit2,
	giCtrlmOpenPlugEdit3,
	giCtrlmOpenPlugEdit4,
	
//	giCtr_Mix_Master_Out_Display,

	
	giNr_Of_Master_Ctls

};

enum E_System_Timers
{
	giTimerID_Meters_Track = giNr_Of_Master_Ctls,
	//giTimerID_Meters_Mixer,
	//giTimerID_Meters_AUXs,

	giTimerID_Player1_ScrollPane,
	giTimerID_Player2_ScrollPane,
	giTimerID_TrackSide_ScrollPane,
	giTimerID_TrackFiles_ScrollPane,
	giTimerID_SystemSetup_ScrollPane,

	giTimerID_InfoFirstChannel_ScrollPane,
	giTimerID_InfoLastChannel_ScrollPane = giTimerID_InfoFirstChannel_ScrollPane + giNumber_Of_Tracks - 1,

	giTimerID_MixFirstChannel_ScrollPane,
	giTimerID_MixLastChannel_ScrollPane = giTimerID_MixFirstChannel_ScrollPane + giNumber_Of_Tracks - 1,
	giTimerID_MixFirstBus_ScrollPane,
	giTimerID_MixLastBus_ScrollPane = giTimerID_MixFirstBus_ScrollPane + giNumber_Of_Busses - 1,
	giTimerID_MixMaster_ScrollPane,

	giTimerID_ImportAudioPreview,
	giTimerID_ProgressTasks,
	giTimerID_NonModalDialog,

	giTimerID_NotValid_LastIndex
};

enum E_System_Ctrls
{
	giCtrl_ToolSelection = giTimerID_NotValid_LastIndex,
	giChC_Track_Player_ScrollBar_Vertical,
	giChC_Track_Player_ScrollBar_Horizontal,
	giChC_Zoom_Slider,
//	giChC_Add_Track,
//	giChC_Delete_Track,
	giChC_Tempo_Display,
	giControlIDSplashText,
	giControlID_Hour_Display,
	giControlID_Min_Display,
	giControlID_Sec_Display,
	giControlID_MSec_Display,
	giControlIDProgressText,
	giControlID_Sample_Start_Display,
	giChC_Import_Audio,
	giChC_Mix_ScrollBar_Horizontal,
	giCtr_Mix_ScrollPane_Timer,
	giControlID_Open_Mix_Button,
	giControlID_View_AUX_Button,
//	giControlID_View_Info_Button,
	giControlID_Remember_Password,
	giControlID_User_Name,
	giControlID_User_Password,
	giControlID_Timesignatur_Popup,
	giControlID_Snap_To_Popup,
	giControlID_Record,
	giControlID_Go_To_Start,
	giControlID_Go_To_End,
	giControlID_Fast_Back,
	giControlID_Play,
	giControlID_Stop,
	giControlID_Forward,
	giControlID_Loop,
	giCtrl_Info_Selection,
	giCtrl_Export_Selection,
	giCtrl_Export,
	giCtrl_Cancel_Export,
	giCtrl_Export_Tail,
	
	giCtrl_ExportForWeb,
	giCtrl_Cancel_ExportForWeb,
	giCtrl_Export_Compression_Popup,
	
	
	giCtrl_Close_Audio_Setup,
	giCtrl_Asio_Setup,
	
	giCtrl_Browser_Popup,
	giCtrl_Remove_Import,
	giCtrl_Add_Import,
	giCtrl_Stop_Import,
	giCtrl_Play_Import,

	giCtrl_Import,
	giCtrl_Cancel_Import,
	
	giCtrl_Verify_Rights,
	giCtrl_Grid,
	giCtrl_Show_Fade,
	giCtrl_Show_Waveform,
	giCtrl_File_List,
	giCtrl_File_List2,
	
	giCtrl_Upload_To_Koblo,
	giCtrl_Export_To_Local,
	giCtrl_Pack_Project,
	giCtrl_Remove_Unused_Tracks,
	giCtrl_Leave_Plugins,
	giCtrl_Leave_Regions,
	giCtrl_Compression_Popup,
	giCtrl_Channels_Popup,
	giCtrl_Wave_Type_Popup,
	giCtrl_Sound_Card_In_Popup,
	giCtrl_Sound_Card_Popup,
	giCtrl_Sample_Rate_Popup,
	giCtrl_Buffer_Size_Popup,
	
	giCtrl_Play_Head_Timer,
	
	giCtrl_CC_License_URL_Txt,
	giCtrl_CC_License_Author_Txt,
	giCtrl_CC_License_Status_Txt,
	giCtrl_CC_License_Radio_Button,
	
	giCtrl_Normalize_On_Export,
	
	
//	giCtrl_Export,


	giControlType,
	giControlBitDepth,
	giControlSampleRate,
	giControlChannels,
	
	giNr_Of_System_Ctls
};

enum E_AUX1_Ctrls
{

	giFirst_AUX1_Ctrl = giNr_Of_System_Ctls,
	giCtrl_Delay_Power,
	giCtrl_Delay_Time,
	giCtrl_Delay_Time_Display,
	giCtrl_Delay_Feedback,
	giCtrl_Delay_Feedback_Display,
	giCtrl_Delay_LP_Dampning,
	giCtrl_Delay_LP_Dampning_Display,
	giCtrl_Delay_HP_Dampning,
	giCtrl_Delay_HP_Dampning_Display,
	giCtrl_Delay_Sync_To_Host,
	giLast_AUX1_Ctrl,
};

enum E_AUX2_Ctrls
{

	giFirst_AUX2_Ctrl = giLast_AUX1_Ctrl,
	giCtrl_Reverb_Power,
	giCtrl_Reverb_Size,
	giCtrl_Reverb_Size_Display,
	giCtrl_Reverb_PDelay,
	giCtrl_Reverb_PDelay_Display,
	giCtrl_Reverb_Damp,
	giCtrl_Reverb_Panorama,
	giCtrl_Reverb_Color1,
	giCtrl_Reverb_Color2,
	giCtrl_Reverb_On,
	giCtrl_Reverb_Mix,
	giLast_AUX2_Ctrl
};

enum EPlugInControls
{
	giControlBypass = 0
};
