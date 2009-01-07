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



// Base Utilities
#include "bu.h"

// XML File parser
#include "tinyxml.h"
#include "CKSXML_Read_Project.h"
#include "CKSXML_Write_Project.h"
#include "CKSXML_Create_Project.h"
#include "CKSXML_Create_Sample.h"

#include "CKSUsername_And_Password_Model.h"
#include "CKSInternet_Features.h"

#include "aioe.h"
#include "mioe.h"

// Graphical Engine
#include "ge.h"

// STools
#include "stools.h"

// AudioCompress
#include "ac.h"

// Inet engine
#include "ine.h"
#include "IINetUtil.h"

// KSPI
#include "kspi.h"
#include "kspiC.h"

// Base Classes
#include "BaseControlIDs.h"
#include "FontSizes.h"
#include "IBaseDezipperCallback.h"
#include "CBaseMIDICCEvent.h"
#include "CBaseDezipper.h"
#include "CBaseDezipper2.h"
#include "CBaseDSPEngine.h"
#include "CBaseModule.h"
#include "CBasePlugIn.h"
#include "CBasePane.h"
#include "CBasePaneStd.h"
#include "CBaseGUI.h"
#include "CBaseTopPane.h"
#include "IRegionCallback.h"




// Parameter
#include "KSGlobals.h"

// Parameters
#include "ParmIDs.h"

// Controls
#include "ControlIDs.h"

// GUI
//#include "GUIIDs.h"

// Collaboration
//#include "CollaborationIDs.h"

#include "main.h"

// Module
#include "CKSModule.h"

// Parameter
#include "CParmGlobal.h"
#include "Grid_Patterns.h"

#include "WaveFile.h"

// Sounds
class CApplication;
#include "CSample_DSP.h"
#include "CRegion_Model.h"
#include "CRegion_DSP.h"

// PlugIn
class CPlugInManager;
#include "CApplication.h"

// Plug-Ins
#include "CPlugInManager.h"

// Built-in effects
#include "CKobloVerb.h"

// GUI
#include  "CEffectControl.h"
#include  "CX3DisplayConverter.h"
#include  "CKSBaseGUI.h"
//-----------------------
// Track editor
#include  "CTrack_Editor_Pane.h"
#include  "CTrack_Editor_View.h"
#include  "CKSExportGUI.h"
#include  "CKSExportGUIPane.h"
#include  "CKSImportGUI.h"
#include  "CKSImportGUIPane.h"
#include  "CKS_Import_File_Browser.h"
#include  "CKS_Import_Files.h"
#include  "CKSExportForWebGUI.h"
#include  "CKSExportForWebGUIPane.h"
#include  "CKSSetupGUIPane.h"
#include  "CKSSetupGUI.h"

// Username and Password
#include  "CKSUsername_And_Password_Pane.h"
#include  "CKSUsername_And_Password_View.h"


#include  "CKSProject_ID_GUI_Pane.h"
#include  "CKSProject_ID_GUI.h"


#include  "CTrack_Top.h"
#include  "CTrack_Playhead.h"
#include  "CTrack_Side.h"
#include  "CTrack_Setup.h"
#include  "CTrack_Time_Pos.h"
#include  "CTrack_Player.h"
#include  "CTrack_Player2.h"
#include  "CTrack_File_Browser.h"
#include  "CTrack_Files.h"
#include  "CTrack.h"
#include  "CRegion_Pane.h"
#include  "CTrack_Info.h"
#include  "CTrack_Info_Small.h"
#include  "CTrack_Info_Big.h"
#include  "CTrack_AUX_Insert.h"
#include  "CTrack_Insert.h"
#include  "CTrack_Fade_Handle.h"
#include  "CMix_Scroll.h"
#include  "CMix_Channel.h"
#include  "CMix_Buss.h"
#include  "CMix_Buss_AUX_Insert.h"
#include  "CMix_Master.h"
#include  "CMix_Master_AUX_Insert.h"
#include  "CMix_AUX_Insert.h"
#include  "CPlugin_Toolbar.h"


#include "CKSSplashScreen.h"
#include "CKSMixerGUIPane.h"
#include "CKSMixerGUI.h"
#include "CKSAUXGUIPane.h"
#include "CKSAUXGUI.h"
#include  "CSoloMuteState.h"
#include  "CSoloMuteContainer.h"

#include "CKSPlugInGUIPane.h"
#include "CKSPlugInGUI.h"
#include "CKSPlugInEffectPane.h"

#include "CProgressTask.h"
#include "CExportClipTask.h"
#include "CExportDSPTask.h"
#include "CImportAudioTask.h"
#include "CAfterImportTask.h"
#include "CProgressTaskList.h"



// Resources
#include "Resources.h"

// Settings/preferences
#include "CKSPreferences.h"

// DSP
#include "CBuffer.h"
#include "CTrack_DSP.h"
#include "CRack_Echo_DSP.h"
#include "CRack_Reverb_DSP.h"
#include "CDSP.h"


