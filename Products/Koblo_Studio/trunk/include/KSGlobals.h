// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
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



//----------------------------------
// Track sizes

const int giTrack_Size_Small	= 40;
const int giTrack_Size_Big		= 184;
const int giTrack_Size_Default	= giTrack_Size_Small;
//----------------------------------
// Pane Sizes
const int giScrollBarSize		=	14;
const int giTime_Pos_SizeY		=	48;
const int giMax_Grid_Size_Y		=	128*184;

 
const ge::SSize gTrack_Editor(1000,680);
const ge::SSize gTrack_Top(gTrack_Editor.iCX, 60);
//const ge::SSize gTrack_Side(128, gTrack_Editor.iCY - gTrack_Top.iCY );
const ge::SSize gTrack_Side(128, gTrack_Editor.iCY - gTrack_Top.iCY - giScrollBarSize );
const ge::SSize gTrack_Setup(128, gTrack_Side.iCY - giTime_Pos_SizeY - giScrollBarSize );
const ge::SSize gTrack_Info_Big(gTrack_Setup.iCX, giTrack_Size_Big );
const ge::SSize gTrack_Info_Small(gTrack_Setup.iCX, giTrack_Size_Small );
const ge::SSize gTrack_File_Browser(144, gTrack_Side.iCY );
// Lasse, fix 2008-05-13
//const ge::SSize gTrack_Files(130, gTrack_Side.iCY - 40 );
const ge::SSize gTrack_Files(130, gTrack_Side.iCY - 24 );
// .. Lasse
const ge::SSize gTrack_Player(	gTrack_Editor.iCX - gTrack_Side.iCX - gTrack_File_Browser.iCX - giScrollBarSize ,
								gTrack_Editor.iCY - gTrack_Top.iCY - giScrollBarSize  );
const ge::SSize gTrack_Scroll_Editor(gTrack_Editor.iCX*4 , gTrack_Editor.iCY*4);
const ge::SSize gTrack_Time_Pos (gTrack_Scroll_Editor.iCX , giTime_Pos_SizeY );
const ge::SSize gTrack_System_Info(500 , 380 );
const ge::SSize gTrack_System_Setup (gTrack_System_Info.iCX-2 , gTrack_System_Info.iCY -71 );

const ge::SSize gMix_Channel(64, 368);
const ge::SSize gMix_Bus(64, 368);
const ge::SSize gMix_Master(64, 368);


const ge::SSize gMix_Scroll_Default(10 * gMix_Channel.iCX,380);
const ge::SSize gMix(986,382);
const ge::SSize gMix_Min(
	1 * gMix_Channel.iCX + 6 + 4 * gMix_Bus.iCX + 6 + gMix_Master.iCX + giScrollBarSize,
	gMix.iCY
);
const ge::SSize gMix_Max(0, gMix.iCY);

const ge::SSize gTrack_Aux_Insert (64 , 130 );

const ge::SSize gAUX(598, 298);
const ge::SSize gAUX_Min(gAUX.iCX, 0);
const ge::SSize gAUX_Max(gAUX.iCX, 0);


const int		giNumber_Of_Tracks			=	128;
const int		giDefault_Number_Of_Tracks	=	8;
const int		giNumber_Of_Busses			=	4;
const int		giNumber_Of_Masters			=	1;
const int		giNumber_Of_Inserts			=	4;
const int		giNumber_Of_AUXes			=	2;
const int		giMin_Pix_Pr_Gridline		=	4;
const int		giMax_Scren_Size			=	7680;
const int		giNumber_Of_Grid_Lines		=	8192;  
//!!! Note if CTrack_Player2 size.x  gets greater than (giMin_Pix_Pr_Gridline *giNumber_Of_Grid_Lines)   the lines will not draw coretly.
const int		giNumber_Of_Regions			=	8192;
const tfloat	gfAnti_denormal					=	1e-10f;

struct SGUIData
{
	tint32 iID;
	tint32 iVal;
};

struct SResize_Data
{
	tint32 iX;
	tint32 iY;
};

struct SSelection_Info
{
	tuint32 uiCmd;
	tuint64 uiSelection_Start_Sample;
	tuint64 uiSelection_End_Sample;
};


struct STrackData
{
	tint32 iParam_ID;
	tint32 iVal;
	tint32 iTrack;
};

struct SBussData
{
	tint32 iParam_ID;
	tint32 iVal;
	tint32 iBuss;
};

struct SGlobalData
{
	tint32 iExtra;
	tint32 iID;
	tint32 iVal;
	
};

struct SProgressData
{
	tbool bOn;
	tfloat32 fProgress;
	tchar* pszText;
	
};

struct SMouse_Track_Data
{
	tint32 iTrackID;
	tint32 iType;
	ge::SPos pos;
};

struct SAdd_Track
{
	tint32 iID;
	tint32 iGUI_Pos;
};

struct STrack_Stack
{
	tint32 iNr_Of_Tracks;
	tint32 iTrack[giNumber_Of_Tracks];
};

struct STrackSelectionInfo
{
	//! Region
	tuint32 iRegionID;
	// Type of Selection 
	tuint32 uiSelection_Type;
	//! Sample possition on track
	tuint64 uiSelection_Pos;
	//! Sample Duration
	tuint64 uiSelection_Duration;
	

};
//! SRegion_Drawing_Info holds info about a region used for drawing
/*! All informations is in samples */
struct SRegion_Drawing_Info
{
	//! Used for wave form drawing
	tuint64 uiSound_Start;
	//! Duration of the sound
	tuint64 uiSound_Duration;
	//! Used to place the region on the track
	tuint64 uiTrack_Start;
	//! Fade in time in samples
	tuint64 uiFade_In;
	//! Fade out time in samples
	tuint64 uiFade_Out;
	//! Fade out time in samples
	tfloat32 fRegion_Volume;
};

//! SRegion_Drawing_Info holds info about a region used for drawing
/*! All informations is in samples */
struct SLoop_Drawing_Info
{
	//! Loop On/Off
	tbool	bLoop;
	//! Loop start sample
	tuint64 uiLoop_Start;
	//! Loop start sample
	tuint64 uiLoop_End;
	//! Loop start sample
	//	tuint64 uiLoop_Midt;
	
};

//! windows id's
enum EWindowIDs
{
	giMain_Window,
	giMix_Window,
	giRack_Window,
	giSplash_Window,
	giExport_Audio_Window,
	giImport_Audio_Window,
	giExport_For_Web_Window,
	giAudio_Setup_Window,
	giProject_ID_Window,
	giSign_In_Window,
	giPlug_In_Window
	
};



//! Pane messasges
enum EExportMsg
{

	//! Informs that the size has changed. Input: None. Output: None
	MsgExportMixDown,
	MsgExportSelectedTrack,
	MsgExportAllTrack,
	MsgExportSelection,
	MsgExportAllClips
	
};

//! Pane messasges
enum EPaneMsg
{

	//! Informs that the size has changed. Input: None. Output: None
	MsgGlobalData,
	MsgGUIData,
	Msg_Resize,
	Msg_Track_Data,
	Msg_Buss_Data,
	Msg_Show_Grid,
//	Msg_Set_Track_Size,
	Msg_Delete_All_Regions,
//	Msg_Deselect_All_Regions,
	Msg_Select_Track,
	Msg_Update_Track_Selection,
	Msg_Add_Region,
	Msg_Stack_Tracks,
//	Msg_Update_Zoom,
	Msg_Track_Mouse_Data,
	Msg_Show_Info,
	Msg_Show_Setup_Screen,
	Msg_Verify_Rights,
	// Lasse, added 2008-04-16
	Msg_Track_MouseClick_Inside,
	Msg_Track_MouseClick_Outside,
	Msg_Track_MouseMove_Inside,
	Msg_Track_MouseMove_Outside,
	// .. Lasse
//	Msg_Track_Selected,
	Msg_Delete_Selected_Region,
	Msg_Set_Track_Visible,
	Msg_Update_File_List,
	Msg_Set_All_Meters,
	Msg_Reset_Drag_Region_Size,
	Msg_Deselect_All,
	Msg_Reset_Update_Play_Head_Size_Y,
	Msg_Track_MouseDrop, 
	Msg_Track_MouseDrag,
	Msg_Draw_Loop,
	Msg_Go_To_Start,
	Msg_Go_To_Playhead,
	Msg_Go_To_Mouse,
	Msg_Progress,
	Msg_Init_ExportForWeb,
	Msg_Show_NormalizeAndTail,
	Msg_Set_Grid,
	MsgUpdateAudioGUI,
	MsgSign_Out,
	Msg_Do_Delete,
};

// Track Editor



enum EPlugTypes
{
	giPlugNone = 0/*,
	giPlug4BandEq,
	giPlugEcho,
	giPlugReverb,
	giNumberOfBuildInPlugs*/
};

enum EToolType
{
	giTool_Hand = 0,
	giTool_Trim,
	giTool_Select,
	giTool_Cut
};

enum ELoopEdit
{
	giNew_Loop = 0,
	giEdit_Loop_Start,
	giEdit_Loop_Midt,
	giEdit_Loop_End, 
	giEdit_Loop_Off
};

enum ESelectionCmd
{
	giSelect_Off = 0,
	giSelect_Array,
	giSelect_Region,
	giSelect_All,
	giSelect_Start,
	giSelect_Region_Center,
	giSelect_On_Track,
	giSelect_End,
	giSelect_Trim_Start,
	giSelect_Trim_End,
	giSelect_Add_To_End,
	giSelect_Add_To_Start,
	giTrim,
	giFade_In,
	giFade_Out,
	giCut_Pos,
	
};

enum EHandleTypes
{
	giFade_Out_Handle = 0,
	giFade_In_Handle,
	giRegion_Volume_Handle	
};




/*
enum EChannelSizes
{
	giChSizeMicro = 0,
	giChSizeMini,
	giChSizeSmall,
	giChSizeMedium,
	giChSizeLarge,
	giChSizeXLarge,
	giChSizeXXLarge,
	giNumberOfChSizes,
	giChOptionVoid,
	giChOptionLoadWaveFile,
	giChOptionConsolidateTrack,
	giChOptionViewAudio,
	giChOptionViewMIDI,
	giNrOfChOption
	
};
*/

/*
enum ETrackSizes
{
	giTrackSetupCtrlSizeMicro = 0, // Micro buttons
	giTrackSetupCtrlSizeMini, // micro buttons and a display
	giTrackSetupCtrlSizeSmall, // normal buttons and a display
	giTrackSetupCtrlSizeNormal, // normal buttons and version controll
	giTrackSetupCtrlSizeLarge,
	giNrOfTrackSetupCtrlSizes
};
*/
//const int giDefaultTrackSize		= giChSizeMedium;

/*
enum EPositionPopup
{
	giHide_Pos = 0,
	giShow_Pos,
	giGrid_Off,
	giGrid_Bar_Beat,
	giGrid_Minute_Sec,
	giGrid_Sample,
	giAllChSizeMicro,
	giAllChSizeMini,
	giAllChSizeSmall,
	giAllChSizeMedium,
	giAllChSizeLarge,
	giAllChSizeXLarge,
	giAllChSizeXXLarge,
	

	
	giNumberOfGridPopupItems
	
};
*/
// CC Licences
enum EGUI_License_Types
{
	giCC_By = 0,
	giCC_By_Sa,
	giCC_By_Nd,
	giCC_By_Nc,
	giCC_By_Nc_Sa,
	giCC_By_Nc_Nd,
	
};
enum EMasterOutputs
{
	giMasterOut12 = 0,
	giMasterOut34,
	giMasterOut56,
	giMasterOut78, 
	giMasterOut1234,
	giMasterOut5678,
};

enum ERegionEdit
{
	giEdit_None = 0,
	giEdit_Trim_Start,
	giEdit_Trim_End, 
	giEdit_Dragging,
	//giEdit_Trim_Start,
};

enum ETinyXmlTypes
{
	
	giTinyXml_Type_String = 0,
	giTinyXml_Type_Int,
	giTinyXml_Type_Float,
	giTinyXml_Type_Take
	
};

