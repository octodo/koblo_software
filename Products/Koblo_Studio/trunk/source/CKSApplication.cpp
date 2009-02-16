
#include "KSOS.h"


#define COMPANY_NAME "Koblo"
#define PRODUCT_NAME "KS"


extern CDSP* gpDSPEngine = NULL;
extern CKSApplication* gpApplication = NULL;


const tint giPolyphony = 4;
const tint giReserve = 4;
const tint giMaxSubBufferSize = 32;

#ifdef _WIN32
	#include <shlwapi.h>
#endif // _WIN32

#ifdef WIN32
#pragma pack(push, 1)
#endif	// WIN32
struct SSample24
{
	unsigned char a;
	unsigned char b;
	char c;
} PACKED;
#ifdef WIN32
#pragma pack(pop)
#endif	// WIN32


CKSApplication::CKSApplication(CKSModule* pModule, tuint32 uiProcessIndex) 
: CBasePlugIn(dynamic_cast<CBaseModule*>(pModule),giAudioMaxBufferSize, dynamic_cast<CBaseDSPEngine*>(new CDSP()), uiProcessIndex, COMPANY_NAME, PRODUCT_NAME),
mbUpdateGUISettings(true),
mbBypass(false), 
miSongPos(0), 
mePlaybackState(geStateStopped),
mpPlugInManager(NULL),
mbAreGUIsReady(false),
mbRecord(false),
mbTimer(false),
CKSXML_Create_Project(),
CKSXML_Read_Project(),
CKSXML_Write_Project(),
CKSUsername_And_Password_Model(),
CKSInternet_Features(),
CKSXML_Create_Sample()

{
	gpDSPEngine = dynamic_cast<CDSP*>(mpDSPEngine);
	gpApplication = this;

	gpDSPEngine->SetChannels(2);
	
		
	mpDezipper->SetCallback(dynamic_cast<IBaseDezipperCallback*>(this));
	
	//!!! Max Temp quick fix
	tfloat64 fPixels_Pr_Sec		=	gpiPixels_Pr_Sec[gi_Default_Zoom_Level];
	mfPixelPrSample				=	fPixels_Pr_Sec / 44100.0f;
	mfSamplesPrPixel			=	1.0f / mfPixelPrSample;
		
	miSelected_Track	= -1;
	mbGUI_Created	= false;
	mbAll_Plugins_Created = false;
	
	msStack.iNr_Of_Tracks = 0;
	for( tint i = 0; i < giNumber_Of_Tracks; i++){
		msStack.iTrack[i]	=	i;
	}

	mpTimer_ProgressTasks = NULL;

	mapszLicenseCodes[0] = "by";
	mapszLicenseUrls[0] = "http://creativecommons.org/licenses/by/3.0/legalcode";
	mapszLicenseTexts[0] =
		"Attribution (by)\r\n"
		"\r\n"
		"You may distribute, remix, tweak, and build upon this work\r\n"
		"in any way, even commercially, as long as you credit %s for\r\n"
		"the original creation.";

	mapszLicenseCodes[1] = "by-sa";
	mapszLicenseUrls[1] = "http://creativecommons.org/licenses/by-sa/3.0/legalcode";
	mapszLicenseTexts[1] =
		"Attribution (by-sa)\r\n"
		"\r\n"
		"You may distribute, remix, tweak, and build upon this work\r\n"
		"in any way, even commercially, as long as you credit %s for\r\n"
		"the original creation.\r\n"
		"Any new work created from this must carry exactly the same license.";

	mapszLicenseCodes[2] = "by-nd";
	mapszLicenseUrls[2] = "http://creativecommons.org/licenses/by-nd/3.0/legalcode";
	mapszLicenseTexts[2] =
		"Attribution (by-nd)\r\n"
		"\r\n"
		"You may distribute this work in any way, even commercially,\r\n"
		"as long as you credit %s for the original creation.\r\n"
		"You may not change it or build other work upon it.";

	mapszLicenseCodes[3] = "by-nc";
	mapszLicenseUrls[3] = "http://creativecommons.org/licenses/by-nc/3.0/legalcode";
	mapszLicenseTexts[3] =
		"Attribution (by-nc)\r\n"
		"\r\n"
		"You may distribute, remix, tweak, and build upon this work,\r\n"
		"but only non-commercially, and you must credit %s for\r\n"
		"the original creation.\r\n"
		"Any new work created from this must be non-commercial too.";

	mapszLicenseCodes[4] = "by-nc-sa";
	mapszLicenseUrls[4] = "http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode";
	mapszLicenseTexts[4] =
		"Attribution (by-nc-sa)\r\n"
		"\r\n"
		"You may distribute, remix, tweak, and build upon this work,\r\n"
		"but only non-commercially, and you must credit %s for\r\n"
		"the original creation.\r\n"
		"Any new work created from this must carry exactly the same license.";

	mapszLicenseCodes[5] = "by-nc-nd";
	mapszLicenseUrls[5] = "http://creativecommons.org/licenses/by-nc-nd/3.0/legalcode";
	mapszLicenseTexts[5] =
		"Attribution (by-nc-nd)\r\n"
		"\r\n"
		"You may distribute this work, but only non-commercially,\r\n"
		"and you must credit %s for the original creation.\r\n"
		"You may not change it or build other work upon it.";

	miPreviousLicenseNb = -1;

	miAudioInput_IntermediateBuffer_FullChainCount = kiAudioInput_IntermediateBuffer_DefaultFullChainCount;
	mbAudioInput_IntermediateBuffer_EverFull = false;
	miAudioInput_IntermediateBuffer_FirstLinkIx = 0;
	miAudioInput_IntermediateBuffer_FirstLinkSamples = 0;
	
	mbZoomFlipFlop = false;
	mbKSProject_Imported = false;
	

} // constructor


CKSApplication::~CKSApplication()
{
	if (mpPlugInManager) {
		delete[] mpPlugInManager;
	}

	AbortProgressTasks(true);
	NonModalDialogs_ZapList();

	AudioInput_IntermediateBuffer_Kill();
} // destructor


void CKSApplication::Destroy()
{
	delete dynamic_cast<CKSApplication*>(this);
	
} // Destroy


kspi::IGUI* CKSApplication::CreateGUI(tint32 iIndex)
{
	CBaseGUI* pGUI;
	switch(iIndex) {
		case giMain_Window:
			pGUI = dynamic_cast<CBaseGUI*>(new CTrack_Editor_View( mpParmMan));
			break;		

		case giSplash_Window:
			pGUI = dynamic_cast<CBaseGUI*>(new CKSSplashScreen( mpParmMan));
			break;

		case giMix_Window:
			pGUI = dynamic_cast<CBaseGUI*>(new CKSMixerGUI( mpParmMan));
			break;
		
		case giRack_Window:
			pGUI = dynamic_cast<CBaseGUI*>(new CKSAUXGUI( mpParmMan));
			break;
		
		case giPlug_In_Window:
			pGUI = dynamic_cast<CBaseGUI*>(new CKSPlugInGUI( mpParmMan));
			break;
			
		case giExport_Audio_Window:
			pGUI = dynamic_cast<CBaseGUI*>(new CKSExportGUI( mpParmMan));
			break;
			
		case giExport_For_Web_Window:
			pGUI = dynamic_cast<CBaseGUI*>(new CKSExportForWebGUI( mpParmMan));
			break;
			
		case giImport_Audio_Window:
			pGUI = dynamic_cast<CBaseGUI*>(new CKSImportGUI( mpParmMan));
			
			break;
		case giAudio_Setup_Window:
			pGUI = dynamic_cast<CBaseGUI*>(new CKSSetupGUI( mpParmMan));
			break;
			
		case giProject_ID_Window:
			pGUI = dynamic_cast<CBaseGUI*>(new CKSProject_ID_GUI(mpParmMan));
			break;
			
		case giSign_In_Window:
			pGUI = dynamic_cast<CBaseGUI*>(new CKSUsername_And_Password_View( mpParmMan));
			break;
		
		default: {
			break;
		}
	}

	mGUIs.push_back(pGUI);

	kspi::IGUI* pKSPIGUI = dynamic_cast<kspi::IGUI*>(pGUI);

	return pKSPIGUI;
} // CreateGUI


void CKSApplication::GetChannelConfigurations(tuint32* /*puiNrOfConfigurations*/, tuint32* /*puiChannelsIn*/, tuint32* /*puiChannelsOut*/, tuint32* /*puiSideChainChannels*/)
{
}


void CKSApplication::Initialize()
{
	CBasePlugIn::Initialize();

	mpPlugInManager = new CPlugInManager();
	mpPlugInManager->Init();

	PlaybackGoToStart();
	
	mpProgressTasks = new CProgressTaskList();
	mpTimer_NonModalDialog = ITimer::Create();
	mpTimer_NonModalDialog->Init(giTimerID_NonModalDialog, this, 100);
	Start_Timer();
} // Initialize


void CKSApplication::AddParameters()
{
	mpParmMan->SetGlobalSectionCount(giNrOfSections);
	
	
		
	//------------------------------------------------
	// Global
	//------------------------------------------------
	AddGlobalParm(giSectionGlobal, giParmID_KS_Bypass, 0, 1, 0);
	//AddGlobalParm(giSectionGlobal, giParamID_KS_Nr_Tracks, 0, giNumber_Of_Tracks, 4);
	AddGlobalParm(giSectionGlobal, giParamID_KS_Tempo, 300, 3000, 1200);
	AddGlobalParm(giSectionGlobal, giParamID_KS_Time_Signature, giSignature2_4, giSignature12_8, giSignature4_4);
	AddGlobalParm(giSectionGlobal, giParamID_Info_Radio_Button, 0, 4, 0);
	AddGlobalParm(giSectionGlobal, giParamID_Info_Verify_Rights, 0, 1, 0);
	AddGlobalParm(giSectionGlobal, giParamID_Loop_On, 0, 1, 0);
	AddGlobalParm(giSectionGlobal, giParamID_Loop_Start, 0, 0x7FFFFFFF, 0);
	AddGlobalParm(giSectionGlobal, giParamID_Loop_End, 0, 0x7FFFFFFF, 44100*4);
	AddGlobalParm(giSectionGlobal, giParamID_KS_Snap_To, 0, 32, 4);
	AddGlobalParm(giSectionGlobal, giParamID_Project_ID, -1, 0x7FFFFFFF, -1);
	AddGlobalParm(giSectionGlobal, giParamID_Branch_ID, -1, 0x7FFFFFFF, -1);
	AddGlobalParm(giSectionGlobal, giParamID_Revision_Nr, -1, 0x7FFFFFFF, -1);
	

	//------------------------------------------------
	// GUI
	//------------------------------------------------
	AddGlobalParm(giSectionGUI, giParmID_GUI_First, 0, 1, 1);
	AddGlobalParm(giSectionGUI, giParamID_Tool_Selected, 0, 4, 0);
	AddGlobalParm(giSectionGUI, giParamID_Track_Selected, -1, giNumber_Of_Tracks, -1);
	AddGlobalParm(giSectionGUI, giParamID_Zoom, 0, giZoom_Levels-1, gi_Default_Zoom_Level);
	AddGlobalParm(giSectionGUI, giParamID_Grid, 0, 1, 1);
	AddGlobalParm(giSectionGUI, giParamID_Show_AUX_Window, 0, 1, 0);
	AddGlobalParm(giSectionGUI, giParamID_Show_Mix_Window, 0, 1, 1);
	AddGlobalParm(giSectionGUI, giParamID_Show_Track_Window, 0, 1, 0);
	AddGlobalParm(giSectionGUI, giParamID_Show_Export_Window, 0, 1, 0);
	AddGlobalParm(giSectionGUI, giParamID_Export_Type, 0, 4, 0);
	AddGlobalParm(giSectionGUI, giParamID_Export_Wave, 0, 4, 0);
	AddGlobalParm(giSectionGUI, giParamID_Export_Compression, 0, 16, 4);
	AddGlobalParm(giSectionGUI, giParamID_Export_Tail, 0, 60000, 200);//1000);
	AddGlobalParm(giSectionGUI, giParamID_Show_Export_For_Web_Window, 0, 1, 0);
	AddGlobalParm(giSectionGUI, giParamID_CC_License_Type, giCC_By, giCC_By_Nc_Sa, giCC_By);
	AddGlobalParm(giSectionGUI, giParamID_Show_Import_Window, 0, 1, 0);
	AddGlobalParm(giSectionGUI, giParamID_Show_Waveform, 0, 1, 1);
	AddGlobalParm(giSectionGUI, giParamID_Show_Fade, 0, 1, 1);
	AddGlobalParm(giSectionGUI, giParamID_Audio_Setup_Window, 0, 1, 0);
	AddGlobalParm(giSectionGUI, giParamID_Normalize_On_Export, 0, 1, 1);
	AddGlobalParm(giSectionGUI, giParamID_Show_Projec_ID_Window, 0, 1, 0);
	AddGlobalParm(giSectionGUI, giParamID_Show_Sign_In_Window, 0, 1, 0);
//	AddGlobalParm(giSectionGUI, giParamID_Remember_Username_and_Password, 0, 1, 0);

	

	tuint32 iGUI_Pos = 0;
	
	for(tint32 i = giSection_First_Track; i <= giSection_Last_Track; i++) {
		
		AddGlobalParm(i, giFirst_Track_Param, 0, 1, 0);
		AddGlobalParm(i, giParam_ChIn, 0, 100000, 0);
		AddGlobalParm(i, giParam_ChInMode, 1, 2, 2);	
		AddGlobalParm(i, giParam_ChInGain, 0, 31622, 10000);
		AddGlobalParm(i, giParam_ChMode, 1, 8, 2);
		AddGlobalParm(i, giParam_ChOut, ParmIOOffsetMix, 1000000 , ParmIOOffsetMix);
		AddGlobalParm(i, giParam_ChAUX1OnOff, 0, 1, 0);
		AddGlobalParm(i, giParam_ChAUX2OnOff, 0, 1, 0);
		AddGlobalParm(i, giParam_ChAUX3OnOff, 0, 1, 0);
		AddGlobalParm(i, giParam_ChAUX4OnOff, 0, 1, 0);
		AddGlobalParm(i, giParam_ChAUX5OnOff, 0, 1, 0);
		AddGlobalParm(i, giParam_ChAUX6OnOff, 0, 1, 0);
		AddGlobalParm(i, giParam_ChAUX1, 0, 31622, 0);
		AddGlobalParm(i, giParam_ChAUX2, 0, 31622, 0);
		AddGlobalParm(i, giParam_ChAUX3, 0, 31622, 0);
		AddGlobalParm(i, giParam_ChAUX4, 0, 31622, 0);
		AddGlobalParm(i, giParam_ChAUX5, 0, 31622, 0);
		AddGlobalParm(i, giParam_ChAUX6, 0, 31622, 0);
		AddGlobalParm(i, giParam_ChInsert1, (tint32)0x80000000, (tint32)(((tuint32)1 << 31) - 1), 0);
		AddGlobalParm(i, giParam_ChInsert2, (tint32)0x80000000, (tint32)(((tuint32)1 << 31) - 1), 0);
		AddGlobalParm(i, giParam_ChInsert3, (tint32)0x80000000, (tint32)(((tuint32)1 << 31) - 1), 0);
		AddGlobalParm(i, giParam_ChInsert4, (tint32)0x80000000, (tint32)(((tuint32)1 << 31) - 1), 0);
		AddGlobalParm(i, giParam_ChInsert5, (tint32)0x80000000, (tint32)(((tuint32)1 << 31) - 1), 0);
		AddGlobalParm(i, giParam_ChInsert6, (tint32)0x80000000, (tint32)(((tuint32)1 << 31) - 1), 0);
		AddGlobalParm(i, giParam_ChInsert7, (tint32)0x80000000, (tint32)(((tuint32)1 << 31) - 1), 0);
		AddGlobalParm(i, giParam_ChInsert8, (tint32)0x80000000, (tint32)(((tuint32)1 << 31) - 1), 0);
		AddGlobalParm(i, giParam_ChInsert1File, 0, 1, 1);
		AddGlobalParm(i, giParam_ChInsert2File, 0, 1, 1);
		AddGlobalParm(i, giParam_ChInsert3File, 0, 1, 1);
		AddGlobalParm(i, giParam_ChInsert4File, 0, 1, 1);
		AddGlobalParm(i, giParam_ChInsert5File, 0, 1, 1);
		AddGlobalParm(i, giParam_ChInsert6File, 0, 1, 1);
		AddGlobalParm(i, giParam_ChInsert7File, 0, 1, 1);
		AddGlobalParm(i, giParam_ChInsert8File, 0, 1, 1);
		AddGlobalParm(i, giParam_Track_Info_SizeY,	0, 1, 0);
		AddGlobalParm(i, giParam_ChVol,  0, 31622, 10000);
		AddGlobalParm(i, giParam_ChSolo,		0, 1, 0);
		AddGlobalParm(i, giParam_ChMute,		0, 1, 0);
		AddGlobalParm(i, giParam_ChSoftMute,	0, 1, 0);
		AddGlobalParm(i, giParam_ChArm,			0, 1, 0);
		AddGlobalParm(i, giParam_Track_Info_Show_Inserts,	0, 1, 0);
		AddGlobalParm(i, giParam_ChPannerMode, 0, 2, 1);
		AddGlobalParm(i, giParam_ChPannerLeftRight, -10000, 10000, 0);
		AddGlobalParm(i, giParam_ChPannerFrontBack, 0, 10001, 500);
		AddGlobalParm(i, giParam_Ch_GUI_Position, 0, giNumber_Of_Tracks, iGUI_Pos++);
		AddGlobalParm(i, giParam_Ch_In_Use, 0, 1, 0);
		AddGlobalParm(i, giParam_Ch_Insert1GUIOpen, 0, 1, 0);
		AddGlobalParm(i, giParam_Ch_Insert2GUIOpen, 0, 1, 0);
		AddGlobalParm(i, giParam_Ch_Insert3GUIOpen, 0, 1, 0);
		AddGlobalParm(i, giParam_Ch_Insert4GUIOpen, 0, 1, 0);
		AddGlobalParm(i, giParam_Ch_Insert5GUIOpen, 0, 1, 0);
		AddGlobalParm(i, giParam_Ch_Insert6GUIOpen, 0, 1, 0);
		AddGlobalParm(i, giParam_Ch_Insert7GUIOpen, 0, 1, 0);
		AddGlobalParm(i, giParam_Ch_Insert8GUIOpen, 0, 1, 0);
		AddGlobalParm(i, giParam_Ch_Insert1Bypass, 0, 1, 0);
		AddGlobalParm(i, giParam_Ch_Insert2Bypass, 0, 1, 0);
		AddGlobalParm(i, giParam_Ch_Insert3Bypass, 0, 1, 0);
		AddGlobalParm(i, giParam_Ch_Insert4Bypass, 0, 1, 0);
		AddGlobalParm(i, giParam_Ch_Insert5Bypass, 0, 1, 0);
		AddGlobalParm(i, giParam_Ch_Insert6Bypass, 0, 1, 0);
		AddGlobalParm(i, giParam_Ch_Insert7Bypass, 0, 1, 0);
		AddGlobalParm(i, giParam_Ch_Insert8Bypass, 0, 1, 0);
	}
		
		
		
		
		
		
	for(tint32 i = giSection_First_Buss; i <= giSection_Last_Buss; i++) {
	
		AddGlobalParm(i, giFirst_Buss_Param, 0, 1, 0);
		AddGlobalParm(i, giParam_Buss_In, 1, 8, 2);
		AddGlobalParm(i, giParam_Buss_InMode, 1, 8, 2);
		AddGlobalParm(i, giParam_Buss_InGain, 0, 31622, 10000);
		AddGlobalParm(i, giParam_Buss_Mode, 1, 8, 2);
		AddGlobalParm(i, giParam_Buss_Out, ParmIOOffsetMix, 1000000 , ParmIOOffsetMix);
		AddGlobalParm(i, giParam_Buss_AUX1OnOff, 0, 1, 0);
		AddGlobalParm(i, giParam_Buss_AUX2OnOff, 0, 1, 0);
		AddGlobalParm(i, giParam_Buss_AUX3OnOff, 0, 1, 0);
		AddGlobalParm(i, giParam_Buss_AUX4OnOff, 0, 1, 0);
		AddGlobalParm(i, giParam_Buss_AUX5OnOff, 0, 1, 0);
		AddGlobalParm(i, giParam_Buss_AUX6OnOff, 0, 1, 0);
		AddGlobalParm(i, giParam_Buss_AUX1, 0, 31622, 0);
		AddGlobalParm(i, giParam_Buss_AUX2, 0, 31622, 0);
		AddGlobalParm(i, giParam_Buss_AUX3, 0, 31622, 0);
		AddGlobalParm(i, giParam_Buss_AUX4, 0, 31622, 0);
		AddGlobalParm(i, giParam_Buss_AUX5, 0, 31622, 0);
		AddGlobalParm(i, giParam_Buss_AUX6, 0, 31622, 0);
		AddGlobalParm(i, giParam_Buss_Insert1, 0, 10000, 0);
		AddGlobalParm(i, giParam_Buss_Insert2, 0, 10000, 0);
		AddGlobalParm(i, giParam_Buss_Insert3, 0, 10000, 0);
		AddGlobalParm(i, giParam_Buss_Insert4, 0, 10000, 0);
		AddGlobalParm(i, giParam_Buss_Insert5, 0, 10000, 0);
		AddGlobalParm(i, giParam_Buss_Insert6, 0, 10000, 0);
		AddGlobalParm(i, giParam_Buss_Insert7, 0, 10000, 0);
		AddGlobalParm(i, giParam_Buss_Insert8, 0, 10000, 0);
		AddGlobalParm(i, giParam_Buss_Insert1File, 0, 1, 1);
		AddGlobalParm(i, giParam_Buss_Insert2File, 0, 1, 1);
		AddGlobalParm(i, giParam_Buss_Insert3File, 0, 1, 1);
		AddGlobalParm(i, giParam_Buss_Insert4File, 0, 1, 1);
		AddGlobalParm(i, giParam_Buss_Insert5File, 0, 1, 1);
		AddGlobalParm(i, giParam_Buss_Insert6File, 0, 1, 1);
		AddGlobalParm(i, giParam_Buss_Insert7File, 0, 1, 1);
		AddGlobalParm(i, giParam_Buss_Insert8File, 0, 1, 1);
		AddGlobalParm(i, giParam_Buss_Info_SizeY,	0, 1, 0);
		AddGlobalParm(i, giParam_Buss_Vol,  0, 31622, 10000);
		AddGlobalParm(i, giParam_Buss_Solo,		0, 1, 0);
		AddGlobalParm(i, giParam_Buss_Mute,		0, 1, 0);
		AddGlobalParm(i, giParam_Buss_SoftMute,	0, 1, 0);
		AddGlobalParm(i, giParam_Buss_Arm,			0, 1, 0);
		AddGlobalParm(i, giParam_Track_Info_Show_Inserts,	0, 1, 0);
		AddGlobalParm(i, giParam_Buss_PannerMode, 0, 2, 1);
		AddGlobalParm(i, giParam_Buss_PannerLeftRight, -10000, 10000, 0);
		AddGlobalParm(i, giParam_Buss_PannerFrontBack, -10000, 10000, 0);
		AddGlobalParm(i, giParam_Buss_Insert1GUIOpen, 0, 1, 0);
		AddGlobalParm(i, giParam_Buss_Insert2GUIOpen, 0, 1, 0);
		AddGlobalParm(i, giParam_Buss_Insert3GUIOpen, 0, 1, 0);
		AddGlobalParm(i, giParam_Buss_Insert4GUIOpen, 0, 1, 0);
		AddGlobalParm(i, giParam_Buss_Insert5GUIOpen, 0, 1, 0);
		AddGlobalParm(i, giParam_Buss_Insert6GUIOpen, 0, 1, 0);
		AddGlobalParm(i, giParam_Buss_Insert7GUIOpen, 0, 1, 0);
		AddGlobalParm(i, giParam_Buss_Insert8GUIOpen, 0, 1, 0);
		AddGlobalParm(i, giParam_Bus_Insert1Bypass, 0, 1, 0);
		AddGlobalParm(i, giParam_Bus_Insert2Bypass, 0, 1, 0);
		AddGlobalParm(i, giParam_Bus_Insert3Bypass, 0, 1, 0);
		AddGlobalParm(i, giParam_Bus_Insert4Bypass, 0, 1, 0);
		AddGlobalParm(i, giParam_Bus_Insert5Bypass, 0, 1, 0);
		AddGlobalParm(i, giParam_Bus_Insert6Bypass, 0, 1, 0);
		AddGlobalParm(i, giParam_Bus_Insert7Bypass, 0, 1, 0);
		AddGlobalParm(i, giParam_Bus_Insert8Bypass, 0, 1, 0);
	}
	
		
		
	AddGlobalParm(giSection_Master, giFirst_Master_Param, 0, 1, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Show_Inserts, 0, 1, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Out,	0, 2, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Volume,  0, 31622, 10000);
	AddGlobalParm(giSection_Master, giParam_Master_Insert1, 0, 10000, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Insert2, 0, 10000, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Insert3, 0, 10000, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Insert4, 0, 10000, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Insert5, 0, 10000, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Insert6, 0, 10000, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Insert7, 0, 10000, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Insert8, 0, 10000, 0);
	AddGlobalParm(giSection_Master, giParam_Ch_Master1GUIOpen, 0, 1, 0);
	AddGlobalParm(giSection_Master, giParam_Ch_Master2GUIOpen, 0, 1, 0);
	AddGlobalParm(giSection_Master, giParam_Ch_Master3GUIOpen, 0, 1, 0);
	AddGlobalParm(giSection_Master, giParam_Ch_Master4GUIOpen, 0, 1, 0);
	AddGlobalParm(giSection_Master, giParam_Ch_Master5GUIOpen, 0, 1, 0);
	AddGlobalParm(giSection_Master, giParam_Ch_Master6GUIOpen, 0, 1, 0);
	AddGlobalParm(giSection_Master, giParam_Ch_Master7GUIOpen, 0, 1, 0);
	AddGlobalParm(giSection_Master, giParam_Ch_Master8GUIOpen, 0, 1, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Insert1Bypass, 0, 1, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Insert2Bypass, 0, 1, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Insert3Bypass, 0, 1, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Insert4Bypass, 0, 1, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Insert5Bypass, 0, 1, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Insert6Bypass, 0, 1, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Insert7Bypass, 0, 1, 0);
	AddGlobalParm(giSection_Master, giParam_Master_Insert8Bypass, 0, 1, 0);
	
	AddGlobalParm(giSection_Master, giParam_Master_AUX1_Return, 0, 31622, 10000);
	AddGlobalParm(giSection_Master, giParam_Master_AUX2_Return, 0, 31622, 10000);
	
	AddGlobalParm(giSection_Master, giLast_Master_Param, 0, 1, 0);
	
	
	AddGlobalParm(giSection_AUX1, giAUX1_First_Param, 0, 1, 0);
	AddGlobalParm(giSection_AUX1, giAUX1_Param_Delay_Power,0, 1, 1);
	AddGlobalParm(giSection_AUX1, giAUX1_Param_Delay_Time, 1, 1000, 500);
	AddGlobalParm(giSection_AUX1, giAUX1_Param_Feedback, 0, 100, 80);
	AddGlobalParm(giSection_AUX1, giAUX1_Param_HP_Damping, 31, 16000, 31);
	AddGlobalParm(giSection_AUX1, giAUX1_Param_LP_Damping, 32, 12001, 12000);
	AddGlobalParm(giSection_AUX1, giAUX1_Param_Sync, 0, 1, 0);

	
	
	
	AddGlobalParm(giSection_AUX2, giFirst_AUX2_Param, 0, 1, 0);
	AddGlobalParm(giSection_AUX2, giParam_Reverb_On, 0, 1, 1);
	AddGlobalParm(giSection_AUX2, giParam_Reverb_Size, 0, 128, 64);
	AddGlobalParm(giSection_AUX2, giParam_Reverb_PDelay, 0, 128, 32);
	AddGlobalParm(giSection_AUX2, giParam_Reverb_Damp, 0, 128, 0);
	AddGlobalParm(giSection_AUX2, giParam_Reverb_Panorama, 0, 128, 0);
	AddGlobalParm(giSection_AUX2, giParam_Reverb_Color1, 0, 128, 0);
	AddGlobalParm(giSection_AUX2, giParam_Reverb_Color2, 0, 128, 0);
	AddGlobalParm(giSection_AUX2, giParam_Reverb_Mix, 0, 128, 0);

	
	// Add parameters to dezipper
//	mpDezipper->AddParameter(giParmIDKSVolume, 0, 3162, 1000);

} // AddParameters


void CKSApplication::MakeAutomationMap()
{
	// For each automateable parameter make a SAutomationInfo and add it to the AutomationInfo list
	SAutomationInfo Info;
	// Keep an empty info, which we can copy from
	kspi::SParameterInfo InfoClean;

	// Bypass
	mAutomationMapGlobal.insert(std::pair<tint32, tint32>(giParmID_KS_Bypass, mAutomationInfo.size()));
	Info.iExtra = 0;
	Info.iIndex = giParmID_KS_Bypass;
	Info.Type = de::IParameterManager::TypeGlobal;

	Info.Info = InfoClean;
	strcpy((char*)(Info.Info.pszName), "Bypass");
	Info.Info.Type = kspi::SParameterInfo::TypeEnum;
	Info.Info.iMinValue = 0;
	Info.Info.iMaxValue = 1;
	Info.Info.iDefaultValue = 0;
	Info.Info.CurveType = kspi::SParameterInfo::CurveTypeLinear;
	strcpy((char*)(Info.Info.pszEnumText), "Off@On");

	mAutomationInfo.insert(mAutomationInfo.end(), Info);
} // MakeAutomationMap


void CKSApplication::UpdateEngineData(tint32 iParamID, tint32 iValue)
{
	CAutoLock Lock(mMutex);

	if (UpdateEngineSettings()) {
		switch(iParamID) {
			case giParmID_KS_Bypass:
				mpDSPEngine->SetBypass((iValue == 0) ? false : true);
				break;
			case giParam_Ch_GUI_Position:
				break;
			case giParamID_Loop_On:{
				gpDSPEngine->Loop_On(iValue == 1);
				CBasePane::SMsg Msg(Msg_Draw_Loop);
				std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
				for (; it != mGUIs.end(); it++) {
					(*it)->GetPane()->SendMsg(&Msg);
				}
				break;
			}
			case giParamID_KS_Tempo:{
			
				mfTempo = (tfloat32)iValue/ 10.0f;
				break;
			}
			case giParamID_Loop_Start:{
				gpDSPEngine->Set_Loop_Start(iValue);
				CBasePane::SMsg Msg(Msg_Draw_Loop);
				std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
				for (; it != mGUIs.end(); it++) {
					(*it)->GetPane()->SendMsg(&Msg);
				}
				break;
			}
			case giParamID_Loop_End:{
				gpDSPEngine->Set_Loop_End(iValue);
				CBasePane::SMsg Msg(Msg_Draw_Loop);
				std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
				for (; it != mGUIs.end(); it++) {
					(*it)->GetPane()->SendMsg(&Msg);
				}
				break;
			}
		}
	}

	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CKSBaseGUI*>(pGUI)->UpdateEngineData(0, 0, iParamID, iValue);
	}
} // UpdateEngineData


void CKSApplication::Maintain_Number_Of_Tracks(tint32 iNr_Tracks)
{
	tint32 iTracks_In_Use = Get_Number_Of_Tracks();
	if(iTracks_In_Use < iNr_Tracks){
		while(iTracks_In_Use < iNr_Tracks){
			AddTrack();
			iTracks_In_Use++;
		}
		
	}
	else if(iTracks_In_Use > iNr_Tracks){
	
		while(iTracks_In_Use > iNr_Tracks){
			DeleteTrack();
			iTracks_In_Use--;
		}
	}
} // Maintain_Number_Of_Tracks


void CKSApplication::UpdateGUIData(tint32 iID, tint32 iValue)
{
	if(iID == giParamID_Zoom){

		tfloat64 fPixels_Pr_Sec		=	gpiPixels_Pr_Sec[iValue];
		tfloat64 fSampleRate		=	GetSampleRate();
		mfPixelPrSample				=	fPixels_Pr_Sec / fSampleRate;
		mfSamplesPrPixel			=	1.0f/ mfPixelPrSample;
	}
	
	// Don't update GUI repeatedly when creating / loading
	if (!mbAreGUIsReady)
		return;

	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CKSBaseGUI*>(pGUI)->UpdateGUIData(iID, iValue);
	}
	
	if( iID == giParamID_Export_Type) {
		
		
		SGUIData sData;
		sData.iID	= Msg_Show_NormalizeAndTail;
		sData.iVal	= iValue;

		
		CBasePane::SMsg Msg;
		Msg = MsgGUIData;
		Msg.pDataIn	= (void*)&sData;
		Send_Msg_To_All_Panes(&Msg);
		
	}
} // UpdateGUIData


void CKSApplication::UpdateCommandData(tint32 iID, tint32 iValue)
{
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CKSBaseGUI*>(pGUI)->UpdateCommandData(iID, iValue);
	}
//	dynamic_cast<CDSP*>(mpDSPEngine)->UpdateChannelData(iID, iValue);
} // UpdateCommandData


void CKSApplication::UpdateTrackData(tint32 iID, tint32 iValue, tint32 iChannel)
{
	if(mbGUI_Created != true) 
		return;
	
	switch(iID){
			/*
		case giParam_Ch_Insert1GUIOpen:{
			if(iValue != 0)
				GetPlugInManager()->OpenGUI(iChannel, iValue);
			else{
			//	WindowClosed(void* pWnd)
				GetPlugInManager()->CloseGUI(iChannel, iValue);	
			}
			break;
		}
			 */
	}

	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CKSBaseGUI*>(pGUI)->UpdateTrackData(iID, iValue, iChannel);
	}
	dynamic_cast<CDSP*>(mpDSPEngine)->UpdateChannelData(iID, iValue, iChannel);
} // UpdateTrackData


void CKSApplication::UpdateBussData(tint32 iID, tint32 iValue, tint32 iBuss)
{
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CKSBaseGUI*>(pGUI)->UpdateBussData(iID, iValue, iBuss);
	}

	dynamic_cast<CDSP*>(mpDSPEngine)->UpdateBussData(iID, iValue, iBuss);
} // UpdateBussData


void CKSApplication::UpdateMasterData(tint32 iID, tint32 iValue)
{
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CKSBaseGUI*>(pGUI)->UpdateMasterData(iID, iValue);
	}

	dynamic_cast<CDSP*>(mpDSPEngine)->UpdateMasterData(iID, iValue);
} // UpdateMasterData


void CKSApplication::UpdateAUX1Data(tint32 iID, tint32 iValue)
{

	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CKSBaseGUI*>(pGUI)->UpdateAUX1Data(iID, iValue);
	}
	dynamic_cast<CDSP*>(mpDSPEngine)->UpdateAUX1Data(iID, iValue);
} // UpdateAUX1Data


void CKSApplication::UpdateAUX2Data(tint32 iID, tint32 iValue)
{

	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CKSBaseGUI*>(pGUI)->UpdateAUX2Data(iID, iValue);
	}
	dynamic_cast<CDSP*>(mpDSPEngine)->UpdateAUX2Data(iID, iValue);
} // UpdateAUX2Data


void CKSApplication::SetValue(tint32 iParmID, tint32 iValue)
{
	if (UpdateEngineSettings()) {
//		switch(iParmID) {
//		}
	}
}


void CKSApplication::UpdateAllGUI()
{
//	if (mpGUI) {
//		dynamic_cast<CKSGUI*>(mpGUI)->UpdateAll();
//	}
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CKSBaseGUI*>(pGUI)->UpdateAll();
	}
} // UpdateAllGUI


struct SMIDIHeader
{
	unsigned short Format;
	unsigned short NumTracks;
	unsigned short Division;
};


inline tuint16 SwapB(tuint16 us)
{
	return (((us>>8) & 0x00FF) |  ((us<<8) & 0xFF00));
}
inline tint16 SwapB(tint16 s)
{
	return (((s>>8) & 0x00FF) |  ((s<<8) & 0xFF00));
}


tuint32 GetVarLen(tuchar* pIn, tint32& iIndex)
{
	tuint32 value;
	tuchar c;

	value = pIn[iIndex];
	iIndex++;
	if (value & 0x80) {
		value &= 0x7F;
		do {
			c = pIn[iIndex];
			iIndex++;

			value = (value << 7) + (c & 0x7f);
		} while (c & 0x80);
	}

	return value;
} // GetVarLen


void CKSApplication::OnLoad(const std::string& sPathName)
{
	CAutoDelete<IChunkFile> pChunkFile(IChunkFile::Create());
	tint32 iVersionNumber;
	pChunkFile->Open(sPathName.c_str(), IFile::FileRead, iVersionNumber, false);

	IChunk* pChunk = pChunkFile->GetChunk('MThd');
	SMIDIHeader Header;
	pChunk->Read((tchar*)&Header, 6);
	pChunk->Destroy();

	Header.Format = SwapB(Header.Format);
	Header.NumTracks = SwapB(Header.NumTracks);
	Header.Division = SwapB(Header.Division);

	if (Header.Format > 1) {
		return;
	}

	// Avoid repeated GUI updates on load
	SetGUIsReady(false);

	tint32 iTrack;
	tint64 iFileIndex = 0;
	tint32 iRunningStatus = 0;
	// For each track
	for (iTrack = 0; iTrack < Header.NumTracks; iTrack++) {
		// Get chunk
		IChunk* pChunkTrack = pChunkFile->GetNextChunk(iFileIndex, 'MTrk');
		// Get data from chunk
		tint32 iChunkSize = pChunkTrack->GetSize();
		tuchar* pData = new tuchar[iChunkSize];
		pChunkTrack->Read((tchar*)pData, iChunkSize);
		pChunkTrack->Destroy();

		CMIDITrack* pTrack = new CMIDITrack;
		tint32 iRead = 0;
		while (iRead < iChunkSize) {
//			tuint32 iTime = GetVarLen(pData, iRead);
			tuint32 iStatus = pData[iRead++];
			if (iStatus == 0x23) {
				iStatus = iRunningStatus;
			}
			iRunningStatus = iStatus;
			tuint32 iData0 = pData[iRead++];
			tuint32 iData1 = 0;
			switch(iStatus & 0xf0) {
				case 0xd0:
					break;

				case 0x0:
				case 0x10:
				case 0x20:
				case 0x30:
				case 0x40:
				case 0x50:
				case 0x60:
				case 0x70:
				case 0x80:
				case 0x90:
				case 0xb0:
				case 0xc0:
				case 0xe0:
					iData1 = pData[iRead++];

					CMIDIEvent Event;
					Event.cStatus = (tuchar)iStatus;
					Event.cData0 = (tuchar)iData0;
					Event.cData1 = (tuchar)iData1;
					pTrack->mEvents.push_back(Event);
					break;

				case 0xF0:
					// Meta event
					tuint i;
					{
						if (iStatus == 0xff) {
							switch(iData0) {
								case 0:
									// Sequence number
									i = pData[iRead++];
									i = pData[iRead++];
									if (i == 2) {
										i =	pData[iRead++];
										i = pData[iRead++];
									}
									break;

								case 1:
									// Text
									i = GetVarLen(pData, iRead);
									iRead += i;
									break;

								case 2:
									// Copyright
									i = GetVarLen(pData, iRead);
									iRead += i;
									break;

								case 3:
									// Track name
									i = GetVarLen(pData, iRead);
									iRead += i;
									break;

								case 0x2F:
									// End of Track
									iRead += 1;
									mSong.mTracks.push_back(*pTrack);
									delete pTrack;
									pTrack = new CMIDITrack;
									break;

								case 0x51:
									// Tempo
									iRead += 4;
									break;

								case 0x58:
									// Time signature
									iRead += 5;
									break;

								default:
									break;
							}
						}
						break;
					}
			}
		}

		delete[] pData;
	}

	// Allow GUI updates again
	SetGUIsReady(true);
	// .. and force update now
	Stack_Tracks();
} // OnLoad

std::string CKSApplication::Default_Project_Folder()
{
	
	tchar pszDefaultFolder[1024];
	IFile::GetSystemDirectory(IFile::SystemDirMyMusic, pszDefaultFolder);
/*	strcat(pszPathName, "Koblo Studio:");
	IFile::CreateDirectory(pszPathName);
*/	
	
	std::string s;
	
	return s;
}

void CKSApplication::GetDefaultProjectFolder(tchar* pszPathName)
{

	IFile::GetSystemDirectory(IFile::SystemDirMyMusic, pszPathName);
	strcat(pszPathName, "Koblo Studio:");
	IFile::CreateDirectory(pszPathName);

	tbool bIsFolder;
	if (!IFile::Exists(pszPathName, &bIsFolder) && (bIsFolder)) {
#ifdef WIN32
		IFile::PathFromOS2("C:\\", pszPathName);
#endif // WIN32
#ifdef _Mac
		IFile::PathFromOS2("/", pszPathName);
#endif // _Mac
	}
	
} // GetDefaultProjectFolder


void CKSApplication::GetTempFolder(tchar* pszPathName)
{
	IFile::GetSystemDirectory(IFile::SystemDirScratch, pszPathName);
	strcat(pszPathName, "Koblo Studio Temp:");
	IFile::CreateDirectory(pszPathName);
	
	tbool bIsFolder;
	if (!IFile::Exists(pszPathName, &bIsFolder) && (bIsFolder)) {
		// Fall back to root folder - that's always there (we may not have access to it, but we'll catch that later)
#ifdef WIN32
		IFile::PathFromOS2("C:\\", pszPathName);
#endif // WIN32
#ifdef _Mac
		IFile::PathFromOS2("/", pszPathName);
#endif // _Mac
	}
}


#define KSPROJ_FILE "Project.KSProject"
const tchar pszCorrectExt[] = ".KSProject";


void CKSApplication::SanitizeProjectFolderPath(std::string& rsProjectFolder, const tchar* pszCorrectExt)
{
	std::string sProjectFolder = rsProjectFolder;
	tchar pszPath[1024];
	tchar pszName[1024];
	tbool bDone = false;
	while (!bDone) {
		if (!IFile::SplitPathToPathAndName(sProjectFolder.c_str(), pszPath, pszName)) {
			// Some error
			bDone = true;
		}
		else {
			if (*pszPath == '\0') {
				// We're through the entire path
				bDone = true;
			}
			else {
				if (*pszName == '\0') {
					// No name part - split ending colon
					sProjectFolder.erase(sProjectFolder.length() - 1, 1);
				}
				else {
					tchar* pc = strrchr(pszName, '.');
					if ((pc) && (stricmp(pc, pszCorrectExt) == 0)) {
						// This folder qualifies as a project bundle - skip remainder
						rsProjectFolder = sProjectFolder;
					}
					// Look at next level too
					sProjectFolder = pszPath;
				}
			}
		}
	}
} // SanitizeProjectFolderPath


void CKSApplication::CleanProject(tint32 iCreateEmptyTracks)
{
	// Stop playback
	PlaybackStop();
	// Reset size of projece
	gpDSPEngine->Reset_Session_End_Sample();
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CKSBaseGUI*>(pGUI)->PlaybackStopped();
	}
	Project_Name("");

	// Delete all tracks
	Maintain_Number_Of_Tracks(0);
	msStack.iNr_Of_Tracks = 0;

	mSample_Data_List.clear();
	UpdateGUIFileList();
	miPreviousLicenseNb = -1;
	Set_Selected_Track(-1);
	
	Set_Branch_Name("");
	
	
	for (tint32 i = 0; i < giNumber_Of_Tracks; i++) {
		// reorder all tracks, so they are straight forward (GUI position)
		msStack.iTrack[i]		= i;
		// unmute all tracks
		SetGlobalParm(giParam_ChSolo,		0,		giSection_First_Track + i);
		SetGlobalParm(giParam_ChMute,		0,		giSection_First_Track + i);
		SetGlobalParm(giParam_ChSoftMute,	0,		giSection_First_Track + i);
	}

	// Add empty tracks
	Maintain_Number_Of_Tracks(iCreateEmptyTracks);
	msStack.iNr_Of_Tracks	= iCreateEmptyTracks;
	
	// set zoom to default 
	SetGlobalParm(giParamID_Zoom,	gi_Default_Zoom_Level,		giSectionGUI);
	// show track window
	SetGlobalParm(giParamID_Show_Track_Window,	1,		giSectionGUI);
	// hide mix window
	SetGlobalParm(giParamID_Show_Mix_Window,	0,		giSectionGUI);
	// hide aux window
	SetGlobalParm(giParamID_Show_AUX_Window,	0,		giSectionGUI);
	// hide export window
	SetGlobalParm(giParamID_Show_Export_Window,	0,		giSectionGUI);
	
	
	
} // CleanProject


std::string CKSApplication::GetExtendedError()
{
	return msExtendedError;
}


void CKSApplication::ShowMessageBox(const tchar* pszMessage, const tchar* pszHeader)
{
	ge::IWindow::ShowMessageBox(pszMessage, pszHeader);
} // ShowMessageBox


void CKSApplication::ShowMessageBox_NonModal(const tchar* pszMessage, const tchar* pszHeader)
{
	NonModalDialog_Push(pszHeader, pszMessage);
} // ShowMessageBox_NonModal


void CKSApplication::LoadSaveErrDlg(std::string sMsg)
{
	std::string sExtendedError = GetExtendedError();
	if (sExtendedError.length() > 0)
		sMsg += std::string("\n\n") + sExtendedError;
	ShowMessageBox(sMsg.c_str(), "Sorry");
} // LoadSaveErrDlg


// Lasse: Menu resource IDs
#ifdef WIN32
	#include "projects\win32\resource.h"
	#include "afxres.h"
#endif // WIN32
// .. Lasse

void CKSApplication::OnMenuEvent(const tchar* pszString)
{
#ifdef WIN32
	// It's not really a string - typecast and get value
	tint32 iResourceID = *((tint32*)pszString);
	switch (iResourceID) {
		case ID_FILE_OPEN:
			if (!MenuFileLoadProject()) {
				LoadSaveErrDlg("Error loading project");
			}
			break;
			
		case ID_FILE_NEW:
			if (!MenuFileCreateNewProject()) {
				LoadSaveErrDlg("Error creating project");
			}
			break;
			
		case ID_FILE_SAVE:
			if(mbKSProject_Imported){
				mbKSProject_Imported = false;
				if (!Save_As()) {
					LoadSaveErrDlg("Error saving project");
				}
			}
			else{
			// Save
				if (gpApplication->Project_Folder().length() == 0) 
					ShowMessageBox("No Project to Save", "Sorry");
			
				else
					Save_Project();
			}
			break;
			
		case ID_FILE_SAVE_AS:
			try {
				if (!MenuFileSaveProjectAs()) {
					LoadSaveErrDlg("Error saving project");
				}
			}
			catch (IException* pEx) {
				// Display reason
				LoadSaveErrDlg(pEx->GetFullDescription());
			}
			break;

		case ID_FILE_CLOSEPROJECT:
			{
				// Nothing here yet
			}
			break;

		case ID_FILE_REVERTTOSAVED:
			{
				// Nothing here yet
			}
			break;

		case ID_FILE_IMPORTKSPROJECT:
			{
				mbKSProject_Imported = true;
				MenuFileLoadProject();
				Project_Name("Imported");
			}
			break;
			
		case ID_FILE_EXPORTAUDIO:
			{
				tbool bTest = (GetGlobalParm(giParamID_Show_Export_Window, giSectionGUI) != 0);
				if(!bTest){
					SetGlobalParm(giParamID_Show_Export_Window,true, giSectionGUI);
				}
				else
					GetModule()->GetHost()->ActivateWindow(giExport_Audio_Window);
			}
			break;

		case ID_FILE_EXPORTFORWEB:
			{
				tbool bTest = (GetGlobalParm(giParamID_Show_Export_For_Web_Window, giSectionGUI) != 0);
				if(!bTest){
					SetGlobalParm(giParamID_Show_Export_For_Web_Window,true, giSectionGUI);
				}
				else
					GetModule()->GetHost()->ActivateWindow(giExport_For_Web_Window);
			}
			break;

		case ID_FILE_DOWNLOADPROJECT:
			On_Menu_Download_Project();
			break;
		
		case ID_FILE_UPLOADPROJECT:
			On_Menu_Upload_Project();
			break;
		
		case ID_FILE_COMMITCHANGES:
			On_Menu_Commit_Project();
			break;
		
		case ID_FILE_UPDATEPROJECT:
			On_Menu_Update_Project();
			break;

		case ID_FILE_IMPORTAUDIO:
			Open_Close_Import_Audio_Window();
			break;

/*
		case IDM_AUDIOSETUP:
			{
				tbool bTest = GetGlobalParm(giParamID_Audio_Setup_Window, giSectionGUI) ;

				SetGlobalParm(giParamID_Audio_Setup_Window,!bTest, giSectionGUI);
		
				SGUIData sData;
				sData.iID	= Msg_Show_Info;
				sData.iVal	= !bTest;
				CBasePane::SMsg Msg;
				Msg = MsgGUIData;
				Msg.pDataIn	= (void*)&sData;
				Send_Msg_To_All_Panes(&Msg);
			}
			break;
*/

		case ID_EDIT_UNDO:
			{
				// Nothing here yet
			}
			break;

		case ID_EDIT_COPYREGION:
			{
				// Nothing here yet
			}
			break;

		case ID_EDIT_CUTSELECTION:
			{
				// Nothing here yet
			}
			break;

		case ID_EDIT_PASTEREGION:
			{
				// Nothing here yet
			}
			break;

		case ID_EDIT_DUPLICATEREGION:
			{
				gpDSPEngine->Duplicate_Region();
			}
			break;

		case ID_EDIT_INVERSEREGION:
			gpDSPEngine->InverseRegion();
			break;

		case ID_EDIT_NORMALIZEREGION:
			gpDSPEngine->NormaliseRegion();
			break;

		case ID_EDIT_ADDTRACK:
			if(Project_Folder().size() == 0)
			
			if (GetProjDir().length() == 0) {
				ShowMessageBox("You must create or load a project before adding tracks", "Sorry");
			}
			else {
				AddTrack();
			}
			break;

		case ID_EDIT_DELETETRACK:
			//!!! Make a warning here
			// Are you sure: OK/Cancel
			DeleteTrack();
			break;

		case ID_EDIT_LOOPSELECTION:
			{
				gpDSPEngine->LoopSelection();
				CBasePane::SMsg Msg(Msg_Draw_Loop);
				Send_Msg_To_All_Panes(&Msg);
			}
			break;

		case ID_EDIT_LOOP:
			{
				tbool bTest = (gpApplication->GetGlobalParm(giParamID_Loop_On, giSectionGlobal) != 0);
				gpApplication->GetParmMan()->Set(true, !bTest, giParamID_Loop_On, de::IParameterManager::TypeGlobal, giSectionGlobal);
			}
			break;

		case ID_VIEW_MIXER:
			{
				// show/hide rack Window
				Open_Close_Mix_Window();
			}
			break;

		case ID_VIEW_TRACKEDITOR:
			GetModule()->GetHost()->ActivateWindow(0);
			break;

		case ID_VIEW_AUXRACK:
			{
				// show/hide rack Window
				Open_Close_Rack_Window();
			}
			break;

		case ID_VIEW_VIEWWAVEFORMS:
			{
				tbool bToggle = (GetGlobalParm(giParamID_Show_Waveform, giSectionGUI) == 0);
				SetGlobalParm(giParamID_Show_Waveform, bToggle, giSectionGUI);
			}
			break;

		case ID_VIEW_VIEWGRID:
			{
				tbool bToggle = (GetGlobalParm(giParamID_Grid, giSectionGUI) == 0);
				SetGlobalParm(giParamID_Grid, bToggle, giSectionGUI);
			}
			break;

		case ID_VIEW_VIEWFADES:
			{
				tbool bToggle = (GetGlobalParm(giParamID_Show_Fade, giSectionGUI) == 0);
				SetGlobalParm(giParamID_Show_Fade, bToggle, giSectionGUI);
			}
			break;

		case ID_VIEW_ZOOM:
			{
				Zoom();
			}
			break;

		case ID_VIEW_ZOOMIN:
			{
				tint32 iTest = gpApplication->GetGlobalParm(giParamID_Zoom, giSectionGUI) +1;
				gpApplication->GetParmMan()->Set(true, iTest, giParamID_Zoom, de::IParameterManager::TypeGlobal, giSectionGUI);
			}
			break;

		case ID_VIEW_ZOOMOUT:
			{
				tint32 iTest = gpApplication->GetGlobalParm(giParamID_Zoom, giSectionGUI) -1;
				gpApplication->GetParmMan()->Set(true, iTest, giParamID_Zoom, de::IParameterManager::TypeGlobal, giSectionGUI);
			}
			break;

		case ID_VIEW_JUMPTOMOUSE:
			{
				CBasePane::SMsg Msg(Msg_Go_To_Mouse);
				Send_Msg_To_All_Panes(&Msg);
			}
			break;

		case ID_SETTINGS_AUDIOSETUP:
			{
				MenuSetupAudio();
			}
			break;

		case ID_SETTINGS_SETUSERNAMEANDPASSWORD:
			{
				Open_Username_And_Password_Dialog();
			}
			break;

		case ID_SETTINGS_CLEARUSERNAMEANDPASSWORD:
			{
				Clear_Username_And_Password();
			}
			break;

		case ID_SETTINGS_PROJECTLICENSE:
			{
				Set_Project_License();
			}
			break;

		case ID_TOOLS_HAND:
			{
				gpApplication->SetGlobalParm(giParamID_Tool_Selected,giTool_Hand, giSectionGUI);
			}
			break;

		case ID_TOOLS_TRIM:
			{
				gpApplication->SetGlobalParm(giParamID_Tool_Selected,giTool_Trim, giSectionGUI);
			}
			break;

		case ID_TOOLS_SELECT:
			{
				gpApplication->SetGlobalParm(giParamID_Tool_Selected,giTool_Select, giSectionGUI);
			}
			break;

		case ID_TOOLS_CUT:
			{
				gpApplication->SetGlobalParm(giParamID_Tool_Selected,giTool_Cut, giSectionGUI);
			}
			break;

		case ID_APP_EXIT:
		case IDM_EXIT:
			::PostQuitMessage(0);
			break;
			
		case ID_HELP_KOBLOCOM:
			OpenBrowser((const tchar*)"http://koblo.com/");
			break;

		case ID_HELP:
		case IDM_ABOUT:
		case ID_HELP_KOBLOSTUDIOHELP:
			OpenBrowser((const tchar*)"http://koblo.com/wiki/koblostudio");
			break;

		case ID_HELP_GETTINGSTARTED:
			OpenBrowser((const tchar*)"http://koblo.com/wiki/ks_tutorials");
			break;

		case ID_HELP_SHORTCUTS:
			OpenBrowser((const tchar*)"http://koblo.com/wiki/ks_shortcuts");
			break;
	}
#endif // WIN32
	
#ifdef _Mac
	std::string s(pszString);
	 if(s.compare("Setup@About Koblo Studio") == 0) {
		
		GetModule()->GetHost()->ActivateWindow(giSplash_Window);
		
	}
	else if (s.compare("Setup@Audio Setup") == 0) {
		MenuSetupAudio();
	}
	
/*	else if (s.compare("Setup@Collaboration") == 0) {
		MenuCollaboration();
	}
*/	
	else if (s.compare("Setup@Set Username and Password") == 0) {
		Open_Username_And_Password_Dialog();
	}
	else if (s.compare("Setup@Clear Username and Password") == 0) {
		Clear_Username_And_Password();
	}
	else if (s.compare("Setup@Project License") == 0) {
		Set_Project_License();
	}
	//------------------------------------------
	// file menu
	//------------------------------------------
	else if(s.compare("File@New Project") == 0) {
		// Create
		if (!MenuFileCreateNewProject()) {
		//	LoadSaveErrDlg("Error creating project");			
		}
		
	}
	else if (s.compare("File@Open Project") == 0) {
		
		
		if (!Open_Project()) {
			LoadSaveErrDlg("Error loading project");
		}
		/*
		// Load
		if (!MenuFileLoadProject()) {
			LoadSaveErrDlg("Error loading project");
		}
		 */
	}
	else if (s.compare("File@Save Project") == 0) {
		
		if(mbKSProject_Imported){
			mbKSProject_Imported = false;
			if (!Save_As()) {
				LoadSaveErrDlg("Error saving project");
			}
		}
		else{
		// Save
			if (gpApplication->Project_Folder().length() == 0) 
				ShowMessageBox("No Project to Save", "Sorry");
		
			else
				Save_Project();
		}

	}
	else if (s.compare("File@Save Project As") == 0) {
		// Save
		try {
			
			if (!Save_As()) {
				LoadSaveErrDlg("Error saving project");
			}
		}
		catch (IException* pEx) {
			// Display reason
			LoadSaveErrDlg(pEx->GetFullDescription());
		}
	}
	else if (s.compare("File@Close Project") == 0) {
		
	}
	else if (s.compare("File@Revert to Saved") == 0) {
		
	}
	
	
	else if (s.compare("File@Export Audio") == 0) {
		
		tbool bTest = GetGlobalParm(giParamID_Show_Export_Window, giSectionGUI) ;
		if(!bTest){
			SetGlobalParm(giParamID_Show_Export_Window,true, giSectionGUI);
		}
		else
			
			GetModule()->GetHost()->ActivateWindow(giExport_Audio_Window);

	}
	
	else if (s.compare("File@Import Audio") == 0) {
		Open_Close_Import_Audio_Window();
	}
	
	else if (s.compare("File@Import KSProject") == 0) {
		mbKSProject_Imported = true;
		MenuFileLoadProject();
		Project_Name("Imported");
	}
	
	else if (s.compare("File@Download Project") == 0) {

		On_Menu_Download_Project();
	}
	
	else if (s.compare("File@Upload Project") == 0) {
		
		On_Menu_Upload_Project();
	}
	
	else if (s.compare("File@Commit Changes") == 0) {
		
		On_Menu_Commit_Project();
	}
	
	else if (s.compare("File@Update Project") == 0) {
		
		On_Menu_Update_Project();
	}
	//-------------------------------------------
	// Edit Menu
	//-------------------------------------------
	else if (s.compare("Edit@Undo") == 0) {
		
		
		//	CBasePane::SMsg Msg(Msg_Do_Delete);
		//	Send_Msg_To_All_Panes(&Msg);
		
	}
	
	else if (s.compare("Edit@Copy Region") == 0) {
		
		gpDSPEngine->Copy_Region();
		
	}
	else if (s.compare("Edit@Cut Selection") == 0) {
		
		gpDSPEngine->Copy_Region();
		CBasePane::SMsg Msg(Msg_Do_Delete);
		Send_Msg_To_All_Panes(&Msg);
		
	}
	else if (s.compare("Edit@Paste Region") == 0) {
		
		
		gpDSPEngine->Paste_Region(0,0);
		
	}
	else if (s.compare("Edit@Duplicate Region") == 0) {
		
		gpDSPEngine->Duplicate_Region();
		
	}
	else if (s.compare("Edit@Delete") == 0) {
		
		CBasePane::SMsg Msg(Msg_Do_Delete);
		Send_Msg_To_All_Panes(&Msg);
	}
	
	
	else if (s.compare("Edit@Add Track") == 0) {
		
		if (GetProjDir().length() == 0) {
			ShowMessageBox("You must create or load a project before adding tracks", "Sorry");
		}
		else {
			AddTrack();
		}
	}
	
	else if (s.compare("Edit@Delete Track") == 0) {
		
		if (Get_Number_Of_Tracks() == 0) {
			ShowMessageBox("No tracks to delete", "Sorry");
		}
		else {
			ge::IWindow::EMsgBoxReturn eRet = ge::IWindow::ShowMessageBox("Are you sure?", "Delete Track",ge::IWindow::MsgBoxOKCancel);
			if(eRet == ge::IWindow::MsgBoxRetOK){
				// Go ahead delete track
				DeleteTrack();
			}
		}
		
	}
	
	else if (s.compare("Edit@Loop Selection") == 0) {
		
		gpDSPEngine->LoopSelection();
		
		CBasePane::SMsg Msg(Msg_Draw_Loop);
		Send_Msg_To_All_Panes(&Msg);
		
	}
	else if (s.compare("Edit@Loop") == 0) {
		
		tbool bTest = (gpApplication->GetGlobalParm(giParamID_Loop_On, giSectionGlobal) != 0);
		gpApplication->GetParmMan()->Set(true, !bTest, giParamID_Loop_On, de::IParameterManager::TypeGlobal, giSectionGlobal);
		
	}
	//-------------------------------------------
	// tools Menu
	//-------------------------------------------
	else if (s.compare("Tools@Hand") == 0) {
		gpApplication->SetGlobalParm(giParamID_Tool_Selected,giTool_Hand, giSectionGUI);
	}
	else if (s.compare("Tools@Trim") == 0) {
		gpApplication->SetGlobalParm(giParamID_Tool_Selected,giTool_Trim, giSectionGUI);
	}
	else if (s.compare("Tools@Select") == 0) {
		gpApplication->SetGlobalParm(giParamID_Tool_Selected,giTool_Select, giSectionGUI);
	}
	else if (s.compare("Tools@Cut") == 0) {
		gpApplication->SetGlobalParm(giParamID_Tool_Selected,giTool_Cut, giSectionGUI);
	}

	
	//-------------------------------------------
	// view Menu
	// mix window
	else if (s.compare("View@Mixer") == 0) {
		Open_Close_Mix_Window();
	}
	// track Editor
	else if (s.compare("View@Track Editor") == 0) {
	
		GetModule()->GetHost()->ActivateWindow(giMain_Window);
	}
	// rack window
	else if (s.compare("View@AUX Rack") == 0) {
		
		Open_Close_Rack_Window();
	}
	
	// View Waveforms
	else if (s.compare("View@View Waveforms") == 0) {
		tbool bTest = GetGlobalParm(giParamID_Show_Waveform, giSectionGUI) ;
		
		SetGlobalParm(giParamID_Show_Waveform,!bTest, giSectionGUI);
		
	}
	// View Grid
	else if (s.compare("View@View Grid") == 0) {
	
		tbool bTest = GetGlobalParm(giParamID_Grid, giSectionGUI) ;
		
		SetGlobalParm(giParamID_Grid,!bTest, giSectionGUI);
		
	}
	// View Fade
	else if (s.compare("View@View Fades") == 0) {
		
		tbool bTest = GetGlobalParm(giParamID_Show_Fade, giSectionGUI) ;
		SetGlobalParm(giParamID_Show_Fade,!bTest, giSectionGUI);
		
	}
	// Zoom
	else if (s.compare("View@Zoom") == 0) {
		
		Zoom();
		/*
		
		mbZoomFlipFlop = !mbZoomFlipFlop;
		if(mbZoomFlipFlop){
			miStoredZoom = gpApplication->GetGlobalParm(giParamID_Zoom, giSectionGUI);
			gpApplication->GetParmMan()->Set(true, giZoom_Levels - 10, giParamID_Zoom, de::IParameterManager::TypeGlobal, giSectionGUI);
		}
		else{
			gpApplication->GetParmMan()->Set(true, miStoredZoom, giParamID_Zoom, de::IParameterManager::TypeGlobal, giSectionGUI);
			
		}
		 */
		
	}
	// zoom in
	else if (s.compare("View@Zoom In") == 0) {
		
		tint32 iTest = gpApplication->GetGlobalParm(giParamID_Zoom, giSectionGUI) +1;
		gpApplication->GetParmMan()->Set(true, iTest, giParamID_Zoom, de::IParameterManager::TypeGlobal, giSectionGUI);
		
	}
	
	// zoom out
	else if (s.compare("View@Zoom Out") == 0) {
		
		tint32 iTest = gpApplication->GetGlobalParm(giParamID_Zoom, giSectionGUI) -1;
		gpApplication->GetParmMan()->Set(true, iTest, giParamID_Zoom, de::IParameterManager::TypeGlobal, giSectionGUI);
		
	}
	// jump to mouse pos
	else if (s.compare("View@Jump to Mouse") == 0) {
		
		CBasePane::SMsg Msg(Msg_Go_To_Mouse);
		Send_Msg_To_All_Panes(&Msg);
		
	}
	
	//-------------------------------------------
	// Help Menu
	// Koblo.com
	else if (s.compare("Help@Koblo.com") == 0) {
		
		OpenBrowser((const tchar*)"http://koblo.com/");
		
	} 
	// Koblo Studio
	else if (s.compare("Help@Koblo Studio Help") == 0) {
		
		OpenBrowser((const tchar*)"http://koblo.com/wiki/koblostudio");
		
	} 
	// Getting Started
	else if (s.compare("Help@Getting Started") == 0) {
		
		OpenBrowser((const tchar*)"http://koblo.com/wiki/ks_tutorials");
		
	}
	// Short Cuts
	else if (s.compare("Help@Short Cuts") == 0) {
		
		OpenBrowser((const tchar*)"http://koblo.com/wiki/ks_shortcuts");
		
	}


#endif // #ifdef _Mac
} // OnMenuEvent

/*
void CKSApplication::MenuFileImportAudio()
{
	
	
	if (Get_Project_Folder().length() == 0) {
		ShowMessageBox("You must create or load a project before importing audio", "Sorry");
		return;
	}
	
	
	tbool bTest = (GetGlobalParm(giParamID_Show_Import_Window, giSectionGUI) != 0);
	if(!bTest){
		SetGlobalParm(giParamID_Show_Import_Window,true, giSectionGUI);
	}
	else
		GetModule()->GetHost()->ActivateWindow(giImport_Audio_Window);
} // MenuFileImportAudio

*/
void CKSApplication::MenuSetupAudio()
{
	tbool bTest = (GetGlobalParm(giParamID_Audio_Setup_Window, giSectionGUI) != 0);

	SetGlobalParm(giParamID_Audio_Setup_Window,!bTest, giSectionGUI);
	
	SGUIData sData;
	sData.iID	= Msg_Show_Info;
	sData.iVal	= !bTest;
	CBasePane::SMsg Msg;
	Msg = MsgGUIData;
	Msg.pDataIn	= (void*)&sData;
	Send_Msg_To_All_Panes(&Msg);		
} // MenuSetupAudio


void CKSApplication::Export(ac::EAudioCodec eCodec, tint32 iQuality, tint32 iChannels, tint32 iTailMS, tbool bNormalize)
{
	tint32 iSelection = GetGlobalParm(giParamID_Export_Type, giSectionGUI);
	
	switch(iSelection){
	
		case MsgExportMixDown:{
			MenuFileDistributeMix(eCodec, (ac::EQuality)iQuality, iChannels, iTailMS, bNormalize);
			break;
		}
		case MsgExportSelectedTrack:{
			ExportSelection(true, eCodec, (ac::EQuality)iQuality, iChannels, iTailMS, bNormalize);
			//ExportSelectedTrack(eCodec, (ac::EQuality)iQuality, iChannels, iTailMS, bNormalize);
			break;
		}
		case MsgExportAllTrack:{
			ExportAllTracks(eCodec, (ac::EQuality)iQuality, iChannels, iTailMS, bNormalize);
			break;
		}
		case MsgExportSelection:{
			ExportSelection(false, eCodec, (ac::EQuality)iQuality, iChannels, iTailMS, bNormalize);
			break;
		}
		case MsgExportAllClips:{
			ExportAllClips(eCodec, (ac::EQuality)iQuality, iChannels);
			break;
		}
	}
}

void CKSApplication::Export_For_Web(ac::EQuality eQuality, tint32 iLicenseNb, std::string sUserName, std::string sUserUrl, std::string sUserStatus)
{
	msExportForWeb.eQuality = eQuality;
	msExportForWeb.iLicenseNb = iLicenseNb;
	msExportForWeb.sUserName = sUserName.empty() ? "(unknown author)" : sUserName;
	msExportForWeb.sUserUrl = sUserUrl.empty() ? "(no url)" : sUserUrl;
	msExportForWeb.sUserStatus = sUserStatus.empty() ? "(no status)" : sUserStatus;
	
	try {
		if (!MenuFileSaveProjectAs("", true)) {
			LoadSaveErrDlg("Error exporting project");
		}
	}
	catch (IException* pEx) {
		// Display reason
		LoadSaveErrDlg(pEx->GetFullDescription());
	}
} // Export_For_Web


void CKSApplication::ExportAllClips(ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels)
{
	tbool bSuccess = false;
	msExtendedError = "";

	if (msProjectPathName.empty()) {
		msExtendedError = "You must create or load a project first";
	}
	else {
		std::list<CExportClipTask*> listpInfo;
		std::list<CSample_Data>::iterator it = Get_Sample_Data_List_Begin();
		for ( ; it != Get_Sample_Data_List_End(); it++) {
			CSample_Data* pSample_Data = &(*it);
			const tchar* pszClipName = pSample_Data->Name().c_str();
			CExportClipTask* pExportClipTask = new CExportClipTask( pszClipName, 0, (tuint64)-1);
			listpInfo.insert(listpInfo.end(), pExportClipTask);
		}
		bSuccess = ExportClipsList(&listpInfo, eCodec, eQuality, iChannels);
	}

	if (!bSuccess) {
		if (msExtendedError.empty())
			msExtendedError = "Unknown error";

		LoadSaveErrDlg("Can't export all clips.");
	}

	//return bSuccess;
} // ExportAllClips


void CKSApplication::ExportSelection(tbool bIncludeEffects, ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels, tint32 iTailMS, tbool bNormalize)
{
	tbool bSuccess = true;
	msExtendedError = "";

	if (msProjectPathName.empty()) {
		msExtendedError = "You must create or load a project first";
		bSuccess = false;
	}
	else {
		tint64 iFirstSample = 0, iDuration = 0;
		std::list<tint32> listiTracks;
		std::list<tint32> listiTrack_NonMuted;
		for(tint32 iTrack=0; (bSuccess) && (iTrack<giNumber_Of_Tracks); iTrack++) {
			CTrack_DSP* pChannel = gpDSPEngine->GetTrack(iTrack);
			if (pChannel->HasRegions()) {
				STrackSelectionInfo sInfo = gpDSPEngine->GetTrackSelection(iTrack);
				if (
					(sInfo.uiSelection_Type == giSelect_On_Track)
					||
					(sInfo.uiSelection_Type == giSelect_Region)
				) {
					//bSuccess = ExportTracksSelection_Raw(
					//	iTrack,
					//	sInfo.uiSelection_Pos,
					//	sInfo.uiSelection_Duration,
					//	eCodec, eQuality, iChannels);
					listiTracks.insert(listiTracks.end(), iTrack);
					iFirstSample = sInfo.uiSelection_Pos;
					iDuration = sInfo.uiSelection_Duration;
					if (!gpDSPEngine->IsTrackMuted(iTrack)) {
						listiTrack_NonMuted.insert(listiTrack_NonMuted.end(), iTrack);
					}
				}
			}
		}

		if (listiTracks.size() == 0) {
			msExtendedError += "Nothing is selected";
			bSuccess = false;
		}
		else if ((bIncludeEffects) && (listiTrack_NonMuted.size() == 0)) {
			msExtendedError += "No unmuted tracks are selected";
			bSuccess = false;
		}
		else {
			if (bIncludeEffects) {
				bSuccess = ExportTracksOrMix(
					geStateExportingTrack, &listiTrack_NonMuted,
					true, 
					eCodec, eQuality, iChannels, iTailMS, bNormalize);
			}
			else {
				bSuccess = ExportTracksSelection_Raw(
					listiTracks,
					iFirstSample,
					iDuration,
					eCodec, eQuality, iChannels);
			}
		}
	}

	if (!bSuccess) {
		if (msExtendedError.empty()) {
			msExtendedError = "Unknown error";
		}

		LoadSaveErrDlg("Can't export selection.");
	}

	//return bSuccess;
} // ExportSelection


tbool CKSApplication::ExportTracksSelection_Raw(std::list<tint32> listiTracks, tint64 iStartIx, tint64 iDuration, ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels)
{
	std::list<CExportClipTask*> listpTasks;
	std::list<tint32>::iterator it = listiTracks.begin();
	for ( ; it != listiTracks.end(); it++) {
		tint32 iTrack = *it;
		if (!ExportTracksSelection_Raw_AddOne(iTrack, iStartIx, iDuration, eCodec, eQuality, iChannels, &listpTasks)) {
			return false;
		}
		if (!ExportClipsList(&listpTasks, eCodec, eQuality, iChannels)) {
			return false;
		}
		listpTasks.clear();
	}

	//return ExportClipsList(&listpInfo, eCodec, eQuality, iChannels);
	return true;
} // ExportTracksSelection_Raw


tbool CKSApplication::ExportTracksSelection_Raw_AddOne(tint32 iTrack, tint64 iStartIx, tint64 iDuration, ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels, std::list<CExportClipTask*>* plistpInfo)
{
	tchar pszNameUniteDefault[512];
	sprintf(pszNameUniteDefault, "%s Clips Selection", GetChannelName(iTrack).c_str());

	CExportClipTask* pClipPrev = NULL;

	CTrack_DSP* pChannel = gpDSPEngine->GetTrack(iTrack);
	const std::list<CTrack_DSP::SChannelRegionInfo*>& regions = pChannel->GetRegionList();
	std::list<CTrack_DSP::SChannelRegionInfo*>::const_iterator it = regions.begin();

	tint64 iFirstSample = 0, iFinalSample = -1;
	gpDSPEngine->CalcTrackDuration(iTrack, &iFirstSample, &iFinalSample);
	if (iStartIx < 0) iStartIx = iFirstSample;
	if (iDuration < 0) iDuration = iFinalSample - iFirstSample + 1;

	tint64 iLatestPos = iStartIx;
	tint64 iEndPos = iStartIx + (iDuration - 1);
	for ( ; (iLatestPos <= iEndPos) && (it != regions.end()); it++) {
		CTrack_DSP::SChannelRegionInfo* pRegionInfo = *it;

		// Maybe zereos before this clip
		tint64 iZeroesBefore = pRegionInfo->uiTrack_Pos - iLatestPos;
		tint64 iRegionCutOffStart = 0;
		if (iZeroesBefore <= 0) {
			// No zeroes before - actually we don't use start of this region
			iRegionCutOffStart = -iZeroesBefore;
			iZeroesBefore = 0;
		}
		else {
			// Insert zeroes before this clip
			CExportClipTask* pClipInfoZeroes = new CExportClipTask( "", 0, iZeroesBefore);
			//pClipInfoZeroes->bConcatenateNext = true;
			pClipInfoZeroes->sDestName_Concatenate = pszNameUniteDefault;
			plistpInfo->insert(plistpInfo->end(), pClipInfoZeroes);

			// Maybe link with a clip before silence
			if (pClipPrev) {
				pClipPrev->pConcatenateNextTask = pClipInfoZeroes;
			}
			// Make sure we can link this silent bit to next
			pClipPrev = pClipInfoZeroes;

			// Advance position
			iLatestPos += iZeroesBefore;
		}

		// Export clip
		CRegion_DSP* pRegion = pRegionInfo->pRegion;
		tint64 iClipStartPos = pRegion->Get_Sample_Offset();
		tint64 iClipDuration = pRegion->Get_Duration();
		const tchar* pszSample_Name = pRegion->Get_Sample_Name();
		if (iRegionCutOffStart > iClipDuration) {
			// Cut-off entire clip - do nothing
		}
		else {
			// Maybe cut off start of region
			iClipStartPos += iRegionCutOffStart;
			iClipDuration -= iRegionCutOffStart;

			// Maybe cut off end of region
			tint64 iDurationLimit = iDuration - (iLatestPos - iStartIx);
			if (iClipDuration > iDurationLimit)
				iClipDuration = iDurationLimit;

			// Add clip info
			CExportClipTask* pClipInfo = new CExportClipTask( pszSample_Name, iClipStartPos, iClipDuration);
			//pClipInfo->bConcatenateNext = true;
			pClipInfo->sDestName_Concatenate = pszNameUniteDefault;
			plistpInfo->insert(plistpInfo->end(), pClipInfo);

			// Maybe link clip before to this one
			if (pClipPrev) {
				pClipPrev->pConcatenateNextTask = pClipInfo;
			}
			// Make sure we can link this clip to next (if any)
			pClipPrev = pClipInfo;

			// Advance position
			iLatestPos += iClipDuration;
		}
	}
	
	// Maybe pad with zeroes after last region
	if (iLatestPos <= iEndPos) {
		tint64 iZeroesAfter = iEndPos - iLatestPos + 1;
		CExportClipTask* pClipInfoZeroes = new CExportClipTask( "", 0, iZeroesAfter);
		plistpInfo->insert(plistpInfo->end(), pClipInfoZeroes);

		// Maybe link clip before to this silent bit
		if (pClipPrev) {
			pClipPrev->pConcatenateNextTask = pClipInfoZeroes;
		}
		// No more
		pClipPrev = NULL;
	}

	//return ExportClipsList(&listpInfo, eCodec, eQuality, iChannels);
	return true;
} // ExportTracksSelection_Raw_AddOne


tbool CKSApplication::ExportRegionClip(const tchar* pszClipName, ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels)
{
	std::list<CExportClipTask*> listpInfo;
	CExportClipTask* pInfo = new CExportClipTask( pszClipName, 0, (tuint64)-1);
	listpInfo.insert(listpInfo.end(), pInfo);
	return ExportClipsList(&listpInfo, eCodec, eQuality, iChannels);
} // ExportRegionClip


tbool CKSApplication::ExportClipsList(std::list<CExportClipTask*>* plistpInfo, ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels)
{
	//CAutoDelete<ac::IEncoder> pEncoder(ac::IEncoder::Create(eCodec));
	//if (!pEncoder) {
	//	msExtendedError = "Internal error creating IEncoder object";
	//	return false;
	//}

	tbool bSuccess = true;

	// Verify sanity of clips
	std::list<CExportClipTask*>::iterator it = plistpInfo->begin();
	for ( ; (bSuccess) && (it != plistpInfo->end()); it++) {
		CExportClipTask* pInfo = *it;
		bSuccess = ExportClipsList_VerifyOne(pInfo);
	}

	// Set max channels as output channels for concatenated output
	if (bSuccess) {
		std::list<CExportClipTask*>::iterator itStart = plistpInfo->begin();
		while (itStart != plistpInfo->end()) {
			tbool bConcatenateLast = ((*itStart)->pConcatenateNextTask != NULL);
			if (!bConcatenateLast) {
				itStart++;
			}
			else {
				tint32 iChannelsMax_ForConcat = (*itStart)->miChannels;
				std::list<CExportClipTask*>::iterator itNextStart = itStart;
				itNextStart++;
				for ( ; (itNextStart != plistpInfo->end()) && (bConcatenateLast); bConcatenateLast = ((*itNextStart)->pConcatenateNextTask != NULL), itNextStart++) {
					CExportClipTask* pInfo = *itNextStart;
					if (pInfo->miChannels > iChannelsMax_ForConcat) {
						iChannelsMax_ForConcat = pInfo->miChannels;
					}
				}

				// Enforce same = highest number of output channels for concatenation
				(*itStart)->miChannels = iChannelsMax_ForConcat;

				// Go to next concatenation - if any
				itStart = itNextStart;
			}
		}
	}

	// Maybe warn about quality loss
	tbool bGoAhead = false;
	if (bSuccess) {
		bSuccess = ExportClipsList_WarnQuality(plistpInfo, eCodec, eQuality, iChannels, &bGoAhead);
	}

	// Ask for destination
	tchar pszDestinationFolder[1024];
	*pszDestinationFolder = '\0';
	if (bGoAhead) {
		bSuccess = ExportClipsList_SelectDestination(plistpInfo, eCodec, pszDestinationFolder);
	}

	// Do encoding / copying
	if (*pszDestinationFolder) {
		bSuccess = ExportClipsList_AndAction(plistpInfo, eCodec, eQuality, iChannels);
	}

	/*
	// Clean up
	it = plistpInfo->begin();
	for ( ; it != plistpInfo->end(); it++) {
		CExportClipTask* pInfo = *it;
		delete pInfo;
	}
	plistpInfo->erase(plistpInfo->begin(), plistpInfo->end());
	*/

	return bSuccess;
} // ExportClipsList


tbool CKSApplication::ExportClipsList_AndAction(std::list<CExportClipTask*>* plistpInfo, ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels)
{
	tbool bError = false;
	std::list<CExportClipTask*>::iterator it = plistpInfo->begin();

	//tint32 iWarnings = 0;
	//std::string sWarnings;
	//tint32 iAndMore = 0;
	//std::string sWarningLast;
	//tint32 iSuccesses = 0;

	if (!IsInProgressTaskState())
		PlaybackStop();

	for ( ; !bError && (it != plistpInfo->end()); it++) {
		CExportClipTask* pTask = *it;
		pTask->meCodecDst = eCodec;
		pTask->meQualityDst = eQuality;
		pTask->miChannelsDst = iChannels;

		mpProgressTasks->Add(pTask);
	}

	Playback_InProgressTask();

	/*
	if (bError) {
		tint32 iMissing = plistpInfo->size() - iSuccesses;
		tchar pszMsg[128];
		if (iSuccesses) {
			sprintf(pszMsg, "\n - Successfully exported %d", iSuccesses);
			msExtendedError += pszMsg;
		}
		msExtendedError += "\n - Failed 1";
		if (iMissing > 1) {
			sprintf(pszMsg, "\n - Not attempted %d", iMissing - 1);
			msExtendedError += pszMsg;
		}
	}
	else {
		tchar pszMsg[128];
		sprintf(pszMsg, "Success!\n\n%d clip(s) exported.", iSuccesses);
		ge::IWindow::ShowMessageBox(
			pszMsg,
			"Done",
			ge::IWindow::MsgBoxOK);
	}
	*/

	return !bError;
} // ExportClipsList_AndAction


tbool CKSApplication::ExportClipsList_WarnQuality(
	std::list<CExportClipTask*>* plistpInfo, ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels,
	tbool* pbGoAhead)
{
	std::list<CExportClipTask*>::iterator it = plistpInfo->begin();

	std::string sWarningClipLast;
	ac::EQuality eQualityClipLast;
	tint32 iWarnings = 0;
	std::string sWarnings;
	tint32 iAndMore = 0;
	std::string sWarningLast;

	tbool bConcatenateLast = false;
	for ( ; it != plistpInfo->end(); bConcatenateLast = (*it)->ConcatenateNext(), it++) {
		CExportClipTask* pInfo = *it;

		if (!pInfo->bIsDecompressed) {
			// Original wave. All is good
			pInfo->bDoEncode = true;
		}
		else {
			// Original was compressed file
			if (
				((!bConcatenateLast) && (!pInfo->ConcatenateNext()))
				&&
				(pInfo->sCompressedOriginal.length() > 0)
				&&
				(eCodec == pInfo->eCodecOfOriginal)
				&&
				(eQuality == pInfo->eQualityOfCompressed)
				&&
				((iChannels < 1) || (iChannels == pInfo->miChannels))
			) {
				// Use original compressed file directly
				pInfo->bDoCopy = true;
			}
			else {
				// Encode anew (quality loss - so maybe ask user)
				if (eQuality < pInfo->eQualityOfOriginal) {
					// New quality is lower than original - extra loss will be quite impossible to hear
					// Just go ahead
					pInfo->bDoEncode = true;
				}
				else {
					// Add warning
					iWarnings++;
					sWarningClipLast = pInfo->sScreenName;
					eQualityClipLast = pInfo->eQualityOfOriginal;
					sWarningLast = std::string(" - ") + pInfo->sScreenName + "\n";
					if (iWarnings < 10)
						sWarnings += sWarningLast;
					else
						iAndMore++;
				}
			}
		}
	}

	// Report any warnings
	if (iWarnings == 0) {
		*pbGoAhead = true;
	}
	else {
		tchar pszMsg[1024];
		if (iWarnings == 1) {
			sprintf(pszMsg,
				"Warning!\n"
				"\n"
				"The clip '%s' has been previously encoded\n"
				"with '%s' quality.\n"
				"Encoding it again with '%s' quality will\n"
				"%s.\n"
				"\n"
				"Go ahead anyway?",
				sWarningClipLast.c_str(),
				IEncoder::GetQualityName(eQualityClipLast),
				IEncoder::GetQualityName(eQuality),
				(
					(eQualityClipLast == eQuality)
					? "probably decrease quality"
					: "probably leave quality unchanged (or slightly worse)"
				)
			);
		}
		else {
			if (iAndMore == 1)
				sWarnings += sWarningLast;
			else if (iAndMore > 1) {
				tchar pszAndMore[128];
				sprintf(pszAndMore, " (and %d more)\n", iAndMore);
				sWarnings += pszAndMore;
			}
			sprintf(pszMsg,
				"Warning!\n"
				"\n"
				"Some clips has been previously encoded\n"
				"with a lower or same quality compression.\n"
				"Encoding them again with '%s' quality\n"
				"will probably leave quality unchanged\n"
				"(or slightly worse)\n"
				"\n"
				"These are the clips:\n"
				"%s"
				"\n"
				"Go ahead anyway?",
				IEncoder::GetQualityName(eQuality),
				sWarnings.c_str()
			);
		}
		ge::IWindow::EMsgBoxReturn eRet = ge::IWindow::ShowMessageBox(
			pszMsg,
			"Quality Loss",
			ge::IWindow::MsgBoxOKCancel);
		switch (eRet) {
			case ge::IWindow::MsgBoxRetOK:
				// Go ahead despite quality loss
				it = plistpInfo->begin();
				for ( ; it != plistpInfo->end(); it++) {
					CExportClipTask* pInfo = *it;
					if (!pInfo->bDoCopy)
						pInfo->bDoEncode = true;
				}
				*pbGoAhead = true;
				break;

			case ge::IWindow::MsgBoxRetCancel: 
				// Not an error
				*pbGoAhead = false;
				break;
		}
	}

	// Always success - go ahead in *pbGoAhead
	return true;
} // ExportClipsList_WarnQuality


tbool CKSApplication::ExportClipsList_SelectDestination(std::list<CExportClipTask*>* plistpInfo, ac::EAudioCodec eCodec, tchar* pszDestinationFolder)
{
	pszDestinationFolder[0] = '\0';

	tchar pszDefaultFolder[1024];
	GetDefaultProjectFolder(pszDefaultFolder);
	const tchar* kpszExt = IEncoder::GetDefaultExt(eCodec);

	// Figure out how many individual destinations
	std::list<tint32> listiIxNames;
	std::list<CExportClipTask*>::iterator it = plistpInfo->begin();
	tbool bConcatenateLast = false;
	tint32 iIx = 0;
	for ( ; it != plistpInfo->end(); bConcatenateLast = (*it)->ConcatenateNext(), it++, iIx++) {
		if (!bConcatenateLast)
			listiIxNames.insert(listiIxNames.end(), iIx);
	}

	// Only one destination?
	tbool bOne = (listiIxNames.size() == 1);

	it = plistpInfo->begin();
	if (bOne) {
		// Let user select destination + name
		CExportClipTask* pInfo = *it;

		std::string sDefaultName;
		if (pInfo->sDestName_Concatenate.empty())
			sDefaultName = pInfo->sScreenName + kpszExt;
		else
			sDefaultName = pInfo->sDestName_Concatenate + kpszExt;

		CAutoDelete<ge::ISaveAsDialog> pDlg(ge::ISaveAsDialog::Create());
		pDlg->SetBundleBehaviour(1);
		pDlg->DoDialog(
			pszDestinationFolder, pszDefaultFolder,
			(std::string("*") + kpszExt).c_str(), IEncoder::GetFileTypeDesc(eCodec),
			sDefaultName.c_str());

		if (*pszDestinationFolder) {
			tchar* pszColon = strrchr(pszDestinationFolder, ':');
			if (pszColon) {
				tchar* pszNameAndExt = (pszColon + 1);
				pInfo->sDestNameAndExt = pszNameAndExt;
				*pszNameAndExt = '\0';
				pInfo->sDestFolder = pszDestinationFolder;
			}
		}
	}
	else {
		// Let user select destination folder
		CAutoDelete<ge::IPickDirDialog> pDlg(ge::IPickDirDialog::Create());
		pDlg->DoDialog(pszDestinationFolder, pszDefaultFolder);

		// Warn about overwritings
		tint32 iOverwritings = 0;
		std::string sOverwritten;
		tint32 iAndMore = 0;
		std::string sOverwrittenLast;
		if (*pszDestinationFolder) {
			// Se if any will be overwritten
			while (it != plistpInfo->end()) {
				CExportClipTask* pInfo = *it;
				pInfo->sDestFolder = pszDestinationFolder;
				if (pInfo->sDestName_Concatenate.empty())
					pInfo->sDestNameAndExt = pInfo->sScreenName + kpszExt;
				else
					pInfo->sDestNameAndExt = pInfo->sDestName_Concatenate + kpszExt;
				if (IFile::Exists((pInfo->sDestFolder + pInfo->sDestNameAndExt).c_str())) {
					iOverwritings++;
					sOverwrittenLast = std::string(" - ") + pInfo->sDestNameAndExt + "\n";
					if (iOverwritings < 10)
						sOverwritten += sOverwrittenLast;
					else {
						iAndMore++;
					}
				}
				it++;
			}

			// Report overwritings
			if (iOverwritings) {
				if (iAndMore == 1)
					sOverwritten += sOverwrittenLast;
				else if (iAndMore > 1) {
					tchar pszAndMore[128];
					sprintf(pszAndMore, " (and %d more)\n", iAndMore);
					sOverwritten += pszAndMore;
				}

				tchar pszMsg[4096];
				sprintf(pszMsg,
					"Warning!\n"
					"\n"
					"This will result in the following file(s) being overwritten:\n"
					"%s"
					"\n"
					"Go ahead anyway?",
					sOverwritten.c_str());
				ge::IWindow::EMsgBoxReturn eRet = ge::IWindow::ShowMessageBox(
					pszMsg,
					"Overwritings",
					ge::IWindow::MsgBoxOKCancel);
				switch (eRet) {
					case ge::IWindow::MsgBoxRetOK:
						// Go ahead despite overwriting
						break;
					case ge::IWindow::MsgBoxRetCancel: 
						// Not an error
						pszDestinationFolder[0] = '\0';
						break;
				}
			}
		}
	}

	// Always success - user Cancel show in empty string
	return true;
} // ExportClipsList_SelectDestination


tbool CKSApplication::ExportClipsList_VerifyOne(CExportClipTask* pInfo)
{
	/*
	if (pInfo->sClipName.empty()) {
		// This is a bit of silence - no files to verify
		pInfo->iFiles = 0;
		pInfo->iSampleRate = GetSampleRate();
		return true;
	}

	tbool bIsClipSane = false;

	std::string sWavePathNameL;
	std::string sWavePathNameR;
	pInfo->iFiles = GetFromListName_ClipWavePathNames(
		pInfo->sClipName.c_str(),
		sWavePathNameL, sWavePathNameR, &(pInfo->bIsDecompressed));

	if (pInfo->iFiles == 0) {
		msExtendedError = std::string("Unable to find waves for clip: ") + pInfo->sClipName;
	}
	else {
		tint32 iOpened = 0;
		if (pInfo->pfWaveL->Open(sWavePathNameL.c_str(), IFile::FileRead))
			iOpened++;
		if ((pInfo->iFiles > 1) && (pInfo->pfWaveR->Open(sWavePathNameR.c_str(), IFile::FileRead)))
			iOpened++;
		if (iOpened < pInfo->iFiles) {
			msExtendedError = std::string("Unable to open waves for clip: ") + pInfo->sClipName;
		}
		else {
			// Examine RIFF/Wave files
			CAutoDelete<ac::IDecoder> pDecoder_Wave(ac::IDecoder::Create(pInfo->pfWaveL));
			if (pDecoder_Wave) {
				// Examine previous quality of left side (or mono)
				tint32 iSane = 0;
				if (pDecoder_Wave->TestFile(pInfo->pfWaveL)) {
					iSane++;
				}
				else {
					tchar pszErr[1024];
					pszErr[0] = '\0';
					pDecoder_Wave->GetErrMsg(pszErr, 1024);
					msExtendedError += pszErr;
				}
				if (pInfo->iFiles > 1) {
					// Stereo
					if (pDecoder_Wave->TestFile(pInfo->pfWaveR)) {
						iSane++;
					}
					else {
						tchar pszErr[1024];
						pszErr[0] = '\0';
						pDecoder_Wave->GetErrMsg(pszErr, 1024);
						msExtendedError += pszErr;
					}
				}
				if (iSane < pInfo->iFiles) {
					msExtendedError += std::string("\nUnable to verify wave(s) for clip: ") + pInfo->sClipName;
				}
				else {
					bIsClipSane = true;
					pInfo->iSampleRate = pDecoder_Wave->miOutputSampleFreq;
					pInfo->iChannels = pInfo->iFiles;
					if (!pInfo->bIsDecompressed) {
						// That's it! We have original Waves, no need to look for compressed files
						pInfo->eQualityOfOriginal = pDecoder_Wave->meLowestInputQuality;
					}
					else {
						// The wave in bundle isn't an original - find out quality of compression
						pInfo->sCompressedOriginal = GetFromWaveName_ClipComp_Safe(pInfo->sClipName.c_str());
						CAutoDelete<IFile> pfComp(IFile::Create());
						if (
							(pfComp == NULL)
							||
							(pInfo->sCompressedOriginal.empty())
							||
							(!pfComp->Open(pInfo->sCompressedOriginal.c_str(), IFile::FileRead))
						) {
							pInfo->sCompressedOriginal = "";
						}
						else {
							CAutoDelete<ac::IDecoder> pDecoder_Comp(ac::IDecoder::Create(pfComp));
							if (pDecoder_Comp) {
								if (pDecoder_Comp->TestFile(pfComp)) {
									pInfo->eCodecOfOriginal = pDecoder_Comp->GetAudioCodec();
									ac::EQuality eQuality_Comp = pDecoder_Comp->meLowestInputQuality;
									ac::EQuality eQuality_Wave = pDecoder_Wave->meLowestInputQuality;
									if (eQuality_Wave >= ac::geQualityLossless16) {
										// No info about earlier compression in decompressed Wave
										pInfo->eQualityOfOriginal = eQuality_Comp;
										pInfo->eQualityOfCompressed = eQuality_Comp;
									}
									else {
										// Use lowest
										pInfo->eQualityOfOriginal = min(eQuality_Comp, eQuality_Wave);
										pInfo->eQualityOfCompressed = eQuality_Comp;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	 

	return bIsClipSane;
	 */
	//!! dummy code
	return true;
} // ExportClipsList_VerifyOne


tbool CKSApplication::MenuFileCreateNewProject()
{
	return New_Project();
	
	/*
	msExtendedError = "";
	
	if(Project_Has_Changed()) {
		
		// setup dialog: do you want to save before closing the project?
		tuint32 hest = 0;
		hest++;
		// save project and continue
		// cancel return true

	}
	
	CleanProject(giDefault_Number_Of_Tracks);

	tbool bRes = MenuFileSaveProjectAs((std::string("NewProject") + pszCorrectExt).c_str());
	
	if (msProjectFolder.length() == 0)
		CleanProject(0);

	return bRes;
	 */
} // MenuFileCreateNewProject


tbool CKSApplication::MenuFileSaveProjectAs(const tchar* pszDefaultName /*= ""*/, tbool bCompress /*= true*/)
{	
	/*
	msExtendedError = "";

	if ((msProjectPathName.length() == 0) && ((pszDefaultName == NULL) || (*pszDefaultName == '\0'))) {
		if (!bCompress)
			return MenuFileCreateNewProject();

		msExtendedError = "You must create or load a project first";
		return false;
	}

	CAutoDelete<ge::ISaveAsDialog> pDlg(ge::ISaveAsDialog::Create());

	// (lasse) Not here!!! (doh)
	//CAutoLock Lock(mMutex);
	
	// Remember old project wave file info
	std::string sProjDir_Old				= msProjectFolder;
	std::string sProjDir_ProjFile_Old		= msProjectPathName;
	std::string sProjDir_Audio_Old			= GetProjDir_Audio();
	std::string sProjDir_Clips_Old			= GetProjDir_Clips();
	std::string sProjDir_ClipsDecomp_Old	= GetProjDir_ClipsDecomp();
	
	std::list<CSample_Data*> Sample_Infos_Old;
	std::list<CSample_Data*>::iterator itSample_Info = mSample_Data_List.begin();
	
	for (; itSample_Info != mSample_Data_List.end(); itSample_Info++) {
		
		
		CSample_Data* pSample_Old	= new CSample_Data;
		pSample_Old->sName			= (*itSample_Info)->sName;
		
		// Turn this in to list of takes later
		CTake_Data *pTake			= (*itSample_Info)->Get_Take_Data();
		CTake_Data *pTake_Old		= pSample_Old->Get_Take_Data();
		

		
		pTake_Old->sWaveNameL			= pTake->sWaveNameL;
		pTake_Old->sWaveNameR			= pTake->sWaveNameR;
//		pTake_Old->sOriginalName		= pTake->sOriginalName;
//		pTake_Old->sOriginalExt			= pTake->sOriginalExt;
//		pTake_Old->bIsOriginalStereo	= pTake->bIsOriginalStereo;
//		pTake_Old->bIsOriginalLossy		= pTake->bIsOriginalLossy;
		pTake_Old->bIsStereoInList		= pTake->bIsStereoInList;
		pTake_Old->iOriginalChannelMask = pTake->iOriginalChannelMask;
		pTake_Old->sWavePathNameL		= pTake->sWavePathNameL;
		pTake_Old->sWavePathNameR		= pTake->sWavePathNameR;
		
		
		Sample_Infos_Old.insert(Sample_Infos_Old.end(), pSample_Old);
	}

	std::string sDefaultName = "";
	if ((pszDefaultName) && (*pszDefaultName != '\0')) {
		sDefaultName = pszDefaultName;
	}
	tchar pszDefaultFolder[1024];
	if (msProjectFolder.length()) {
		std::string sPath = msProjectFolder;
		sPath.erase(sPath.find_last_of(':'));
		tint32 iPosColon = sPath.find_last_of(':');
		if (bCompress)
			GetTempFolder(pszDefaultFolder);
		else {
			strncpy(pszDefaultFolder, sPath.c_str(), iPosColon + 1);
			pszDefaultFolder[iPosColon + 1] = '\0';
		}
		if (sDefaultName.length() == 0) {
			// Use current project name as default
			sDefaultName = sPath;
			sDefaultName.erase(0, iPosColon + 1);
		}
	}
	else
		GetDefaultProjectFolder(pszDefaultFolder);
	
	tchar pszPathName[1024];
	pDlg->SetBundleBehaviour(1);
	pDlg->DoDialog(pszPathName, pszDefaultFolder, "*.KSProject", "KS Project (*.KSProject)", sDefaultName.c_str());

	if (pszPathName[0] == 0) {
		// User escaped dialog - it's not an error; we just quit now
		return true;
	}
	
	// On mac this is a folder . on windows it's a file
	std::string sItem = pszPathName;
	
	// convert sItem to a folder path
	SanitizeProjectFolderPath(sItem, pszCorrectExt);
	
	// Verify that it has the correct extension
	tint32 iPosColon = sItem.find_last_of(':');
	if (iPosColon == sItem.length() - 1) {
		// Remove trailing colon
		sItem.erase(iPosColon, 1);
		// Search again
		iPosColon = sItem.find_last_of(':');
	}
	if (iPosColon < 0) {
		msExtendedError = "Internal error: Path interpretation failed";
		return false;
	}
	
	
	const tchar* pszNameOnly = pszPathName + (iPosColon + 1);
	//_strlwr(pszNameOnly);
	
	tbool bCorrectExt = false;
	tint32 iPosDot = sItem.find_last_of('.');
	if (iPosDot > iPosColon) {
		iPosDot -= iPosColon + 1;
		if (stricmp(pszNameOnly + iPosDot, pszCorrectExt) == 0)
			bCorrectExt = true;
	}
	if (!bCorrectExt) {
		// It's not a valid project file name - append proper extension
		sItem += pszCorrectExt;
	}
	
	// Now it's OK to take lock
	if (1) {
		CAutoLock Lock(mMutex);

		tbool bIsFolder;
		tbool bExists = IFile::Exists(sItem.c_str(), &bIsFolder);
		if (bExists) {
			if (bIsFolder) {
				msProjectFolder = sItem + ":";
				msProjectPathName = GetProjDir_Contents() + KSPROJ_FILE;
				SetProjectName(GetProjDir_Contents());
			}
			else {
				// This means we've selected an existing file
				
				msProjectFolder = sItem + ":"; //.substr(0, iPosColon + 1);
				msProjectPathName = GetProjDir_Contents() + KSPROJ_FILE;
				SetProjectName(GetProjDir_Contents());
			}
		}
		else {
			tint32 iPos = sItem.find_last_of(':');
			std::string sPath = sItem.substr(0, iPos + 1);
			
			// Lasse, remove any doublet file-extension
			std::string sToReplace = std::string(pszCorrectExt) + pszCorrectExt;
			std::string sReplaceWith = pszCorrectExt;
			
			for (tint32 iIxToReplace = 0; iIxToReplace < (tint32)sItem.length(); iIxToReplace++) {
				if (strnicmp(sItem.c_str() + iIxToReplace, sToReplace.c_str(), sToReplace.length()) == 0) {
					sItem.replace(iIxToReplace, sToReplace.length(), sReplaceWith.c_str(), sReplaceWith.length());
				}
			}
			
			msProjectFolder = sItem + ':';
			msProjectPathName = GetProjDir_Contents() + KSPROJ_FILE;
			SetProjectName(GetProjDir_Contents());
		}
		
		// Save the project
		if (!MenuFileSaveProject(true)) {
			msProjectFolder = sProjDir_Old;
			msProjectPathName = sProjDir_ProjFile_Old;
			return false;
		}
		
		// Actually changed path?
		if (stricmp(msProjectPathName.c_str(), sProjDir_ProjFile_Old.c_str()) == 0) {
			// Same path
			if (bCompress) {
				msExtendedError = "You can't export a project onto itself";
				return false;
			}
		}
		else {
			// Kill any orphaned wave and ogg files
			if (1) {
				CAutoDelete<IFileSearch> pSearchWaves(IFileSearch::Create());
				
				// Kill originals (wav, ogg or mp3) and any generated oggs
				std::string sPath = GetProjDir_Clips();
				if (pSearchWaves->Init2((sPath + "*.*").c_str())) {
					tchar pszName[1024];
					tbool bIsFolder;
					while (pSearchWaves->GetNext(pszName, bIsFolder)) {
						if (!bIsFolder)
							IFile::DeleteFile((sPath + pszName).c_str());
					}
				}
				// Kill waves in deprecated path
				sPath = GetProjDir_Audio();
				if (pSearchWaves->Init2((sPath + "*.*").c_str())) {
					tchar pszName[1024];
					tbool bIsFolder;
					while (pSearchWaves->GetNext(pszName, bIsFolder)) {
						if (!bIsFolder)
							IFile::DeleteFile((sPath + pszName).c_str());
					}
				}
				// Kill waves decompressed from oggs or mp3s
				sPath = GetProjDir_ClipsDecomp();
				if (pSearchWaves->Init2((sPath + "*.*").c_str())) {
					tchar pszName[1024];
					tbool bIsFolder;
					while (pSearchWaves->GetNext(pszName, bIsFolder)) {
						if (!bIsFolder)
							IFile::DeleteFile((sPath + pszName).c_str());
					}
				}
			}
			
			// Copy waves, oggs and mp3s
			itSample_Info = Sample_Infos_Old.begin();
			
			for (; itSample_Info != Sample_Infos_Old.end(); itSample_Info++) {
				
				
				// This takes quite a long time - so display hour-glass cursor
				CAutoDelete<ge::IWaitCursor> pWaitCursor(ge::IWaitCursor::Create());
				
				CSample_Data* pSample_Data = *itSample_Info;
				CTake_Data* pTake_Info		= pSample_Data->Get_Take_Data();
				
				// Are we exporting?
				if (bCompress) {
					
					// Exporting project for upload - create oggs (or copy existing oggs/mp3s)
					std::string sWaveOriginalL = pTake_Info->sWaveNameL.empty() ? "" : sProjDir_Clips_Old + pTake_Info->sWaveNameL + ".wav";
					std::string sWaveOriginalR = pTake_Info->sWaveNameR.empty() ? "" : sProjDir_Clips_Old + pTake_Info->sWaveNameR + ".wav";
					tbool bFoundAllWaveOriginals = false;
					if (!pTake_Info->bIsOriginalStereo) {
						// Original for import was mono

						sWaveOriginalR = "";

						if (CanWaveFilePlay(sWaveOriginalL, true, false)) {
							bFoundAllWaveOriginals = true;
						}
						else {
							// Check for backward compatible path (mono only)
							sWaveOriginalL = sProjDir_Audio_Old + pTake_Info->sWaveNameL + ".wav";
							if (CanWaveFilePlay(sWaveOriginalL, true, false)) {
								bFoundAllWaveOriginals = true;
							}
							else {
								sWaveOriginalL = "";
							}
						}
					}
					else {
						// Original for import was stereo

						tint32 iCountMissing = 0;
						if ((pTake_Info->iOriginalChannelMask & 0x01) && (!CanWaveFilePlay(sWaveOriginalL, true, false))) {
							// Left side orginal wave missing
							iCountMissing++;
						}
						if ((pTake_Info->iOriginalChannelMask & 0x02) && (!CanWaveFilePlay(sWaveOriginalR, true, false))) {
							// Right side orginal wave missing
							iCountMissing++;
						}
						bFoundAllWaveOriginals = (iCountMissing == 0);
					}
					if (bFoundAllWaveOriginals) {
						if (!ExportProjectForWeb_Compress(sWaveOriginalL, sWaveOriginalR, pSample_Data, ac::geQualityLossless24)) {
							msProjectFolder = sProjDir_Old;
							msProjectPathName = sProjDir_ProjFile_Old;
							SetProjectName(GetProjDir_Contents());
							return false;
						}
					}
					else {
						// Maybe copy an existing compressed file

						CAutoDelete<ge::IWaitCursor> pWaitCursor(ge::IWaitCursor::Create());

						std::string sExistingCompressed_Name = pTake_Info->sOriginalName + pTake_Info->sOriginalExt;
						std::string sExistingCompressed_NameL = "";
						std::string sExistingCompressed_NameR = "";
						if (
							(!pTake_Info->bIsOriginalLossy)
							||
							(!IFile::Exists((sProjDir_Clips_Old + sExistingCompressed_Name).c_str()))
						)
						{
							// No info about a compressed original - try the default (single file)
							if ((!pTake_Info->bIsOriginalStereo) || (pTake_Info->bIsStereoInList)) {
								sExistingCompressed_Name = pSample_Data->sName + ".ogg";
							}
						}
						// Copy compressed files
						tbool bAnyMissing = sExistingCompressed_Name.empty();
						tbool bAnyErrors = false;

						// Try file
						if (!bAnyMissing) {
							if (!IFile::Exists((sProjDir_Clips_Old + sExistingCompressed_Name).c_str())) {
								// No existing compressed file!
								bAnyMissing = true;
							}
							// Only do something if not already copied (left/right side of stereo)
							else if ((pTake_Info->iOriginalChannelMask & 0x01) || (pTake_Info->iOriginalChannelMask == 0)) {
								// Should we copy original because quality is same or lower as requested?
								tbool bDoCopy = false;
								ac::EQuality ePreviousQuality = ac::geQualityUnknown;
								CAutoDelete<IFile> pfCompressed(IFile::Create());
								if (pfCompressed) {
									if (pfCompressed->Open(
										(sProjDir_Clips_Old + sExistingCompressed_Name).c_str(),
										IFile::FileRead))
									{
										CAutoDelete<ac::IDecoder> pDecoder_Temp(ac::IDecoder::Create(pfCompressed));
										if (pDecoder_Temp) {
											ePreviousQuality = pDecoder_Temp->meLowestInputQuality;
											if (ePreviousQuality <= msExportForWeb.eQuality) {
												// The existing compressed file is lower
												// or same quality as what we requested.
												// Use it as it is.
												bDoCopy = true;
											}
										}
									}
								}
								if (bDoCopy) {
									// Create context for handling events
									CAutoDelete<ge::IContext> pContext(ge::IContext::Create());
									//pContext->Init(); -- not Init(), crashes
									tbool bAbort = false;
									std::string sProgress =
										std::string("Copying compressed clip '")
										+ pSample_Data->sName + "'";
									// Can't show real progress for copying file
									ShowProgress(0, 1, sProgress.c_str(), pContext, &bAbort);
									if (bAbort) {
										msExtendedError = "It seems you cancelled operation";
										msProjectFolder = sProjDir_Old;
										msProjectPathName = sProjDir_ProjFile_Old;
										SetProjectName(GetProjDir_Contents());
										ShowProgress(0, 0, NULL, NULL, NULL);
										return false;
									}

									if (!IFile::CopyFile(GetProjDir_Clips().c_str(), sProjDir_Clips_Old.c_str(), sExistingCompressed_Name.c_str()))
									{
										// Copy of stereo file failed!
										bAnyErrors = true;
									}
									ShowProgress(0, 0, NULL, NULL, NULL);
								}
								else {
									// Re-compress already compressed file because we need a lower quality

									if (!pTake_Info->bIsStereoInList) {
										// Mono file - go for it
										if (!ExportProjectForWeb_Compress(
											pTake_Info->sWavePathNameL, "",
											pSample_Data, ePreviousQuality)
										) {
											msProjectFolder = sProjDir_Old;
											msProjectPathName = sProjDir_ProjFile_Old;
											SetProjectName(GetProjDir_Contents());
											return false;
										}
									}
									else {
										// Create stereo file
										if (!ExportProjectForWeb_Compress(
											pTake_Info->sWavePathNameL, pTake_Info->sWavePathNameR,
											pSample_Data, ePreviousQuality)
										) {
											msProjectFolder = sProjDir_Old;
											msProjectPathName = sProjDir_ProjFile_Old;
											SetProjectName(GetProjDir_Contents());
											return false;
										}
									}
								}
							}
						}

						if (bAnyMissing) {
							// We have no original; neither wave nor ogg or mp3 - what to do?
							// Include non-original decompressed wave?

							msExtendedError = "Unable to export clip '" + pSample_Data->sName + "'\nNo original wave(s) and/or premade compressed file(s) found.";
							msProjectFolder = sProjDir_Old;
							msProjectPathName = sProjDir_ProjFile_Old;
							SetProjectName(GetProjDir_Contents());
							ShowProgress(0, 0, NULL, NULL, NULL);
							return false;
						}
						if (bAnyErrors) {
							msExtendedError = "Unable to export clip '" + pSample_Data->sName + "'\nCopy operation of premade compressed file(s) failed.";
							msProjectFolder = sProjDir_Old;
							msProjectPathName = sProjDir_ProjFile_Old;
							SetProjectName(GetProjDir_Contents());
							ShowProgress(0, 0, NULL, NULL, NULL);
							return false;
						}
					}
				}
				else {
					// Ordinary Save As - Create copies of waves

					// Maybe try left side
					if ((!pTake_Info->bIsOriginalStereo) || (pTake_Info->iOriginalChannelMask & 0x01)) {
						if (
							(IFile::CopyFile(GetProjDir_Clips().c_str(), sProjDir_Clips_Old.c_str(), (pTake_Info->sWaveNameL + ".wav").c_str()))
							||
							(IFile::CopyFile(GetProjDir_Clips().c_str(), sProjDir_Audio_Old.c_str(), (pTake_Info->sWaveNameL + ".wav").c_str()))
						)
						{
							// Succesfully copied left side original wave file - remember its new location
							pTake_Info->sWavePathNameL = GetFromWaveName_ClipWave(pTake_Info->sWaveNameL.c_str());
						}
						else {
							// No left side original wave
		
							// Copy left side (or stereo) ogg or mp3 - ignore errors
							std::string sCompressedName = pTake_Info->sOriginalName + pTake_Info->sOriginalExt;
							if (!IFile::Exists((sProjDir_Clips_Old + sCompressedName).c_str()))
								sCompressedName = pTake_Info->sWaveNameL + ".ogg";
							IFile::CopyFile(GetProjDir_Clips().c_str(), sProjDir_Clips_Old.c_str(), sCompressedName.c_str());

							// Copy left side decompressed (lower quality) wave - ignore errors
							IFile::CopyFile(GetProjDir_ClipsDecomp().c_str(), sProjDir_ClipsDecomp_Old.c_str(), (pTake_Info->sWaveNameL + ".wav").c_str());
							// Remember new location
							pTake_Info->sWavePathNameL = GetFromWaveName_ClipWaveDecomp(pTake_Info->sWaveNameL.c_str());
						}
					}

					// Maybe try right side
					if (pTake_Info->iOriginalChannelMask & 0x02) {
						if (
							(IFile::CopyFile(GetProjDir_Clips().c_str(), sProjDir_Clips_Old.c_str(), (pTake_Info->sWaveNameR + ".wav").c_str()))
							||
							(IFile::CopyFile(GetProjDir_Clips().c_str(), sProjDir_Audio_Old.c_str(), (pTake_Info->sWaveNameR + ".wav").c_str()))
						)
						{
							// Succesfully copied right side original wave file - remember its new location
							pTake_Info->sWavePathNameR = GetFromWaveName_ClipWave(pTake_Info->sWaveNameR.c_str());
						}
						else {
							// No right side original wave
		
							// Copy right side (not stereo!) ogg - ignore errors
							std::string sCompressedName = pTake_Info->sWaveNameR + ".ogg";
							if (IFile::Exists((sProjDir_Clips_Old + sCompressedName).c_str())) {
								// Original compressed file found

								IFile::CopyFile(GetProjDir_Clips().c_str(), sProjDir_Clips_Old.c_str(), sCompressedName.c_str());
							}
						
							// Copy right side decompressed (lower quality) wave - ignore errors
							IFile::CopyFile(GetProjDir_ClipsDecomp().c_str(), sProjDir_ClipsDecomp_Old.c_str(), (pTake_Info->sWaveNameR + ".wav").c_str());
							// Remember new location
							pTake_Info->sWavePathNameR = GetFromWaveName_ClipWaveDecomp(pTake_Info->sWaveNameR.c_str());
						}
					}
				}
				delete pTake_Info;
			}
			Sample_Infos_Old.erase(Sample_Infos_Old.begin(), Sample_Infos_Old.end());
		}		
		
		if (bCompress) {
			// Create license files
			CAutoDelete<IFile> pfLicense(IFile::Create());
			if (pfLicense) {
				if (pfLicense->Open((GetProjDir_Contents() + "license.dat").c_str(), IFile::FileCreate)) {
					tchar* pszCode = mapszLicenseCodes[msExportForWeb.iLicenseNb];
					tint32 iLen = strlen(pszCode) + 1;
					pfLicense->Write(pszCode, iLen);
					//
					const tchar* pszAuthor = msExportForWeb.sUserName.c_str();
					iLen = strlen(pszAuthor) + 1;
					pfLicense->Write(pszAuthor, iLen);
					//
					const tchar* pszUrl = msExportForWeb.sUserUrl.c_str();
					iLen = strlen(pszUrl) + 1;
					pfLicense->Write(pszUrl, iLen);
					//
					const tchar* pszStatus = msExportForWeb.sUserStatus.c_str();
					iLen = strlen(pszStatus) + 1;
					pfLicense->Write(pszStatus, iLen);
					//
					pfLicense->Close();
				}
				if (pfLicense->Open((GetProjDir_Contents() + "license.txt").c_str(), IFile::FileCreate)) {
					tchar pszLicenseText[1024];
					sprintf(
						pszLicenseText,
						mapszLicenseTexts[msExportForWeb.iLicenseNb],
						msExportForWeb.sUserName.c_str());
					std::string sLicenseText = pszLicenseText;
					sLicenseText += "\r\n\r\nSee full description of license at:\r\n";
					sLicenseText += mapszLicenseUrls[msExportForWeb.iLicenseNb];
					sLicenseText += "\r\n\r\nAuthor: ";
					sLicenseText += msExportForWeb.sUserName;
					sLicenseText += "\r\nUrl: ";
					sLicenseText += msExportForWeb.sUserUrl;
					sLicenseText += "\r\nStatus: ";
					sLicenseText += msExportForWeb.sUserStatus;
					pfLicense->Write(sLicenseText.c_str(), sLicenseText.length());
					pfLicense->Close();
				}
			}

			// Go back to original folder after successful export
			msProjectFolder = sProjDir_Old;
			msProjectPathName = sProjDir_ProjFile_Old;
		}
		else {
			// Not exporting

			// Copy any license files - no fail on error
 			IFile::CopyFile(GetProjDir_Contents().c_str(), (sProjDir_Old + "Contents:").c_str(), "license.dat");
			IFile::CopyFile(GetProjDir_Contents().c_str(), (sProjDir_Old + "Contents:").c_str(), "license.txt");
		}
	}
		
	// No error
	*/
	return true;
} // MenuFileSaveProjectAs


tbool CKSApplication::ExportProjectForWeb_Compress(
	std::string sWavePathL, std::string sWavePathR,
	CSample_Data* pSample_Data, ac::EQuality ePreviousQuality)
{
	/*
	CTake_Data* pTake_Info = pSample_Data->Get_Take_Data();
	
	// Compress one or two original waves
	CAutoDelete<IFile> pWavL(IFile::Create());
	CAutoDelete<IFile> pWavR(IFile::Create());
	CAutoDelete<IFile> pOgg(IFile::Create());
	CAutoDelete<ac::IEncoder> pEnc(ac::IEncoder::Create(geAudioCodecVorbis));
	if (pEnc == NULL) {
		msExtendedError = "Unable to allocate ogg/vorbis encoder";
		return false;
	}

	// Maybe set quality info at a lower value because the file has been previously compressed
	pEnc->SetQualityOfOriginal(ePreviousQuality);

	if (!pWavL->Open(sWavePathL.c_str(), IFile::FileRead)) {
		msExtendedError = std::string("Unable to read clip wave file '") + sWavePathL + "'";
		return false;
	}
	if ((sWavePathR.length()) && (!pWavR->Open(sWavePathR.c_str(), IFile::FileRead))) {
		msExtendedError = std::string("Unable to read clip wave file '") + sWavePathR + "'";
		return false;
	}
	std::string sOgg = GetFromWaveName_ClipDefaultOgg(pTake_Info->sOriginalName.c_str());
	if (!pOgg->Open(sOgg.c_str(), IFile::FileCreate)) {
		msExtendedError = "Unable to create clip ogg/vorbis file '" + sOgg + "'";
		return false;
	}
	if (!pEnc->Init(pOgg, msExportForWeb.eQuality))
	{
		tchar pszErr[1024];
		pEnc->GetErrMsg(pszErr, 1024, true);
		msExtendedError = pszErr;
		pOgg->Close();
		IFile::DeleteFile(sOgg.c_str());
		return false;
	}

	// Create context for handling events
	CAutoDelete<ge::IContext> pContext(ge::IContext::Create());
	//pContext->Init(); -- not Init(), crashes
	tbool bAbort = false;
	std::string sProgress =
		std::string("Compressing clip '")
		+ pSample_Data->sName + "'";
	const tint32 iWantsToProcess = sWavePathR.length() ? 0x10000 : 0x20000;
	tint64 iProcessIx = 0;
	tuint64 uiProcessMax = 1;
	tint64 iActuallyProcessed = 0;

	do {
		CAutoDelete<ge::IWaitCursor> pWaitCursor(ge::IWaitCursor::Create());
		tbool bPartSuccess = false;
		if (sWavePathR.length())
			bPartSuccess = pEnc->Process(pWavL, pWavR, (tint32)iProcessIx, iWantsToProcess, &iActuallyProcessed);
		else
			bPartSuccess = pEnc->Process(pWavL, (tint32)iProcessIx, iWantsToProcess, &iActuallyProcessed);
		if (!bPartSuccess) {
			tchar pszErr[1024];
			pEnc->GetErrMsg(pszErr, 1024, true);
			msExtendedError = pszErr;
			// Close encoder manually (before IFiles get destroyed)
			pEnc->Finalize();
			pOgg->Close();
			IFile::DeleteFile(sOgg.c_str());
			ShowProgress(0, 0, NULL, NULL, NULL);
			return false;
		}
		iProcessIx += iActuallyProcessed;
		uiProcessMax = pEnc->miInputSamples_IncludingSkipped;
		ShowProgress(iProcessIx, uiProcessMax, sProgress.c_str(), pContext, &bAbort);
		if (bAbort) {
			msExtendedError = "It seems you cancelled operation";
			ShowProgress(0, 0, NULL, NULL, NULL);
			return false;
		}
	} while ((!bAbort) && (iActuallyProcessed > 0));
	// Close encoder manually (before IFiles get destroyed)
	pEnc->Finalize();
	ShowProgress(0, 0, NULL, NULL, NULL);
	 */
	// Success
	return true;
} // ExportProjectForWeb_Compress




tbool CKSApplication::MenuFileLoadProject()
{
	/*
	
	msExtendedError = "";

	PlaybackStop();
	Stop_Timer();
	
	CAutoDelete<ge::IOpenDialog> pDlg(ge::IOpenDialog::Create());


	// Default Name
	tchar pszDefaultFolder[1024];
	std::string sDefaultName = "";
	if (msProjectFolder.length()) {
		std::string sPath = msProjectFolder;
		sPath.erase(sPath.find_last_of(':'));
		tint32 iPosColon = sPath.find_last_of(':');
		strncpy(pszDefaultFolder, sPath.c_str(), iPosColon + 1);
		pszDefaultFolder[iPosColon + 1] = '\0';
		if (sDefaultName.length() == 0) {
			// Use current project name as default
			sDefaultName = sPath;
			sDefaultName.erase(0, iPosColon + 1);
		}
	}
	else
		GetDefaultProjectFolder(pszDefaultFolder);
	if (sDefaultName.length() == 0)
		sDefaultName = "Project.KSProject";
	
	tchar pszPathName[1024];
	//pDlg->DoDialog(pszPathName, pszDefaultFolder, "*.KSProject", "KS Project (*.KSProject)", msProjectFolder.c_str());
	//pDlg->DoDialog(pszPathName, pszDefaultFolder);//, "*.KSProject", "KS Project (*.KSProject)", msProjectFolder.c_str());	
	pDlg->SetBundleBehaviour(1);
	pDlg->DoDialog(pszPathName, pszDefaultFolder, "*.KSProject", "KS Project (*.KSProject)", sDefaultName.c_str());

	if (pszPathName[0] == 0) {
		// Exit with no error
		return true;
	}
	
	
	// Really do it
	if (1) {
		CAutoLock Lock(mMutex);
		//SetGUIsReady(false);
		CleanProject(0);

		std::string sPathName(pszPathName);
		SanitizeProjectFolderPath(sPathName, pszCorrectExt);
		msProjectFolder = sPathName;
		msProjectPathName = GetProjDir_Contents() + KSPROJ_FILE;

		// Try for license file
		CAutoDelete<IFile> pfLicense(IFile::Create());
		if (pfLicense) {
			if (pfLicense->Open((GetProjDir_Contents() + "license.dat").c_str(), IFile::FileRead)) {
				tuint64 uiSize = pfLicense->GetSizeWhenOpened();
				if (uiSize < 4096) {
					tchar pszLicense[4096 + 1];
					memset(pszLicense, '\0', 4096);
					pszLicense[0] = '\0';
					tuint64 uiActuallyRead = pfLicense->Read(pszLicense, uiSize);
					if (uiActuallyRead == uiSize) {
						pszLicense[uiSize] = '\0';
						for (tint32 iLicense = 0; (miPreviousLicenseNb < 0) && (iLicense < 6); iLicense++) {
							tchar* pszCode = mapszLicenseCodes[iLicense];
							if (stricmp(pszCode, pszLicense) == 0) {
								miPreviousLicenseNb = iLicense;
								msExportForWeb.iLicenseNb = iLicense;
								tchar* pszAuthor = pszLicense + (strlen(pszLicense) + 1);
								tchar* pszUrl = pszAuthor + (strlen(pszAuthor) + 1);
								tchar* pszStatus = pszUrl + (strlen(pszUrl) + 1);
								msExportForWeb.sUserName = pszAuthor;
								msExportForWeb.sUserUrl = pszUrl;
								msExportForWeb.sUserStatus = pszStatus;
								CBasePane::SMsg Msg(Msg_Init_ExportForWeb);
								Send_Msg_To_All_Panes(&Msg);
							}
						}
					}
				}
				pfLicense->Close();
			}
		}

		//CAutoDelete<ge::IWaitCursor> pWaitCursor(ge::IWaitCursor::Create());

		CAutoDelete<IChunkFile> pFile(IChunkFile::Create());
		tint32 iVersionNumber;
		if (pFile->Open(msProjectPathName.c_str(), IFile::FileRead, iVersionNumber) == false) {
			//throw IException::Create(IException::TypeFile, IException::ReasonFileCannotOpen, EXCEPTION_INFO, "Cannot open file");
			msExtendedError = "Project open failed: " + msProjectPathName;
			return false;
		}

		// Sanitize folders - This will also create project folder
		IFile::CreateDirectory(GetProjDir_ClipsDecomp().c_str());
		IFile::CreateDirectory(GetProjDir_Icons().c_str());

		// Note: We should reset parms in non-present sections to default

		tint64 iIndex = 0;
		tint32 iSection = -1;
		while (1) {
			IChunk* pChunkOrg = pFile->GetNextChunk(iIndex, giChunkPerformance);
			if (pChunkOrg == NULL) {
				// No more chunks
				break;
			}
			CAutoDelete<IChunk> pChunk = CAutoDelete<IChunk>(pChunkOrg);

			iSection++;

			// Prepare memory
			tint32 iChunkSize = pChunk->GetSize();
			tchar* p = new tchar[iChunkSize - (2 * sizeof(tint32))];

			// Read the header
			tint32 iFirstParameter;
			tint32 iParameterCount;
			pChunk->Read((tchar*)&iFirstParameter, sizeof(tint32));
			iFirstParameter = CPSwap(iFirstParameter);
			pChunk->Read((tchar*)&iParameterCount, sizeof(tint32));
			iParameterCount = CPSwap(iParameterCount);

			// Set the parameters not in chunk to default value
			tint32 iParmCountThisVersion = mpParmMan->GetGlobalDataSize(iSection);
			tint32 iParm;
			for (iParm = iParameterCount; iParm < iParmCountThisVersion; iParm++) {
				const de::IParameter* pParm = mpParmMan->Get(iParm, de::IParameterManager::TypeGlobal, iSection);
				mpParmMan->Set(true, pParm->GetDefaultValue(), iParm, de::IParameterManager::TypeGlobal, iSection, false);
			}

			// Read parameters into memory
			if (pChunk->Read((tchar*)p, iChunkSize - (2 * sizeof(tint32))) != iChunkSize - (2 * sizeof(tint32))) {
			}

			// Set the parameters
			mpParmMan->SetGlobalData(iFirstParameter, iParameterCount, (const tint32*)p, iSection, true);

			// Clean up
			delete[] p;
		}

		// Load GUI Positions of tracks
		iIndex = 0;
		if (1) {
			IChunk* pChunkOrg = pFile->GetNextChunk(iIndex, 'TPos');
			if (pChunkOrg == NULL) {
				// No chunk - project file is too old to load
				Maintain_Number_Of_Tracks(0);
				msStack.iNr_Of_Tracks = 0;
				//break;
			}
			else {
				CAutoDelete<IChunk> pChunk = CAutoDelete<IChunk>(pChunkOrg);

				if (pChunk->GetSize() == sizeof(msStack)) {
					pChunk->Read((tchar*)(&msStack), sizeof(msStack));
					// PPC backward-compatible work-around: Read projects saved without CPSwapping
					if (msStack.iNr_Of_Tracks > giNumber_Of_Tracks) {
						// Needs CPSwapping - do manually
						tint32* pi = (tint32*)&msStack;
						for (tint32 i = 0; i < (sizeof(msStack) / sizeof(tint32)); i++) {
							tint32 l = *pi;
							*pi = (((l>>24) & 0x000000FFL) | ((l>>8) & 0x0000FF00L) | ((l<<8) & 0x00FF0000L) | ((l<<24) & 0xFF000000L));
							pi++;
						}
					}
				}
			}
		}

		SetGUIsReady(true);
		GetModule()->GetHost()->ActivateWindow(giMain_Window);

		// Queue load of audio clips
		std::list<CImportAudioTask*> listImportTasks;
		if (!MenuFileLoadProject_QueueClips(pFile, &listImportTasks))
			return false;
		std::list<CImportAudioTask*>::iterator itImports = listImportTasks.begin();
		for ( ; itImports != listImportTasks.end(); itImports++) {
			CImportAudioTask* pTask = *itImports;
			mpProgressTasks->Add(pTask);
		}

		CAfterImportTask* pAfterImportTask = new CAfterImportTask();
		pAfterImportTask->Init();
		if (msStack.iNr_Of_Tracks > 0) {
			// Load track names
			iIndex = 0;
			tint32 iTrack = -1;
			while (1) {
				IChunk* pChunkOrg = pFile->GetNextChunk(iIndex, 'NAME');
				if (pChunkOrg == NULL) {
					// No more chunks
					break;
				}
				pAfterImportTask->mlistIChunkTrackNames.insert(pAfterImportTask->mlistIChunkTrackNames.end(), pChunkOrg);
			}

			// Load wave regions for tracks
			iTrack = -1;
			iIndex = 0;
			while (1) {
				IChunk* pChunkOrg = pFile->GetNextChunk(iIndex, 'REGI');
				if (pChunkOrg == NULL) {
					// No more chunks
					break;
				}
				pAfterImportTask->mlistIChunkTrackRegions.insert(pAfterImportTask->mlistIChunkTrackRegions.end(), pChunkOrg);
			}
		}

		iIndex = 0;
		while (1) {
			IChunk* pChunkOrg = pFile->GetNextChunk(iIndex, 'INSR');
			if (pChunkOrg == NULL) {
				break;
			}
			else {
				pAfterImportTask->mlistIChunkInserts.insert(pAfterImportTask->mlistIChunkInserts.end(), pChunkOrg);
			}
		}

		SetGUIsReady(true);
		GetModule()->GetHost()->ActivateWindow(giMain_Window);
		Stack_Tracks();
		Update_Zoom();

		mpProgressTasks->Add(pAfterImportTask);
	}

	// Start progress
	Playback_InProgressTask();
	*/
	// No error
	return true;
} // MenuFileLoadProject


tbool CKSApplication::MenuFileLoadProject_QueueClips(IChunkFile* pFile, std::list<CImportAudioTask*>* plistpImportTasks)
{
	/*
	tint64 iIndex = 0;
	tint32 iFileInList = -1;
	mSample_Data_List.clear();
	std::string sTemp_LeftSideOfStereoClip_Name = "";
	std::string sTemp_LeftSideOfStereoClip_OriginalPathName = "";
	std::string sTemp_LeftSideOfStereoClip_WaveName = "";
	
	tbool bAskedAboutBits = false;
	tbool bAskedIfSameBits = false;
	tbool bUseSameBits = false;
	tint32 iBitsToUse = 0;
	
	tbool bAskedAboutBroken = false;
	tbool bAskedAboutBrokenSame = false;
	tbool bBrokenSame = false;
	tint32 iBrokenSkipped = 0;

	while (1) {
		// First try new name for chunk
		tint64 iIndexSave = iIndex;
		IChunk* pChunkOrg = pFile->GetNextChunk(iIndex, 'Clip');
		if (pChunkOrg == NULL) {
			iIndex = iIndexSave;
			// Backward compatible name for chunk
			pChunkOrg = pFile->GetNextChunk(iIndex, 'WAVE');
		}
		if (pChunkOrg == NULL) {
			// No more chunks
			break;
		}
		CAutoDelete<IChunk> pChunk = CAutoDelete<IChunk>(pChunkOrg);

		iFileInList++;

		if (pChunk->GetSize() > 0) {
			tchar pszChunk[2048];
			pszChunk[0] = '0';
			pChunk->Read(pszChunk, pChunk->GetSize());
			pszChunk[pChunk->GetSize()] = '\0';

			// Check for new format chunk with the name of the comressed file as well
			std::string sName = pszChunk;
			std::string sNameL = "";
			std::string sNameR = "";
			std::string sWaveNameL = "";
			std::string sWaveNameR = "";
			std::string sOriginalName = "";
			std::string sOriginalExt = "";
			tbool bIsOriginalStereo = false;
			tbool bIsOriginalLossy = false;
			tint32 iOriginalChannelMask = 0;
			tbool bIsStereoInList = false;
			if (sName.length() == pChunk->GetSize()) {
				// Be backward compatible
				sWaveNameL = sName;
			}
			else {
				// Yes, we have extra info in chunk after the name - get it!
				tchar* pszWaveNameL = pszChunk + sName.length() + 1;
				tchar* pszWaveNameR = pszWaveNameL + strlen(pszWaveNameL) + 1;
				tchar* pszOriginalName = pszWaveNameR + strlen(pszWaveNameR) + 1;
				tchar* pszOriginalExt = pszOriginalName + strlen(pszOriginalName) + 1;
				tchar* pszBIsOriginalStereo = pszOriginalExt + strlen(pszOriginalExt) + 1;
				tchar* pszBIsOriginalLossy = pszBIsOriginalStereo + 2;
				tchar* pszIOriginalChannelMask = pszBIsOriginalLossy + 2;
				tchar* pszBIsStereoInList = pszIOriginalChannelMask + 2;
				sWaveNameL = pszWaveNameL;
				sWaveNameR = pszWaveNameR;
				sOriginalName = pszOriginalName;
				sOriginalExt = pszOriginalExt;
				bIsOriginalStereo = (*pszBIsOriginalStereo == 'T');
				bIsOriginalLossy = (*pszBIsOriginalLossy == 'T');
				iOriginalChannelMask = (*pszIOriginalChannelMask - '0');
				bIsStereoInList = (*pszBIsStereoInList == 'T');
			}

			std::string sOriginalPathName = GetProjDir_Clips() + sOriginalName + sOriginalExt;
			if ((bIsOriginalStereo) && (bIsOriginalLossy) && (sOriginalName.length())) {
				if (iOriginalChannelMask & 0x01) {
					// Left side

					// Save info for when processing right side
					sTemp_LeftSideOfStereoClip_OriginalPathName = sOriginalPathName;
					sTemp_LeftSideOfStereoClip_Name = sName;
					sTemp_LeftSideOfStereoClip_WaveName = sWaveNameL;
				}
				if (iOriginalChannelMask & 0x02) {
					// Right side

					// Verify sanity
					if (stricmp(sOriginalPathName.c_str(), sTemp_LeftSideOfStereoClip_OriginalPathName.c_str()) != 0) {
						msExtendedError = 
							std::string("Compressed stereo file trouble:\n Left side src '")
							+ sTemp_LeftSideOfStereoClip_OriginalPathName
							+ "'\n Right side src '"
							+ sOriginalPathName
							+ "'";
						CleanProject(0);
						msProjectPathName = "";
						msProjectFolder = "";
						SetProjectName(GetProjDir_Contents());
						return false;
					}

					// Retrieve saved left side info
					sNameR = sName;
					sNameL = sTemp_LeftSideOfStereoClip_Name;
					sWaveNameL = sTemp_LeftSideOfStereoClip_WaveName;

					// Clean saved info (so we don't re-use by mistake)
					sTemp_LeftSideOfStereoClip_Name = "";
					sTemp_LeftSideOfStereoClip_OriginalPathName = "";
					sTemp_LeftSideOfStereoClip_WaveName = "";
				}
			}

			// Build path(s)
			std::string sWavePathNameL = GetFromWaveName_ClipWave_Safe(sWaveNameL.c_str());
			std::string sWavePathNameR = GetFromWaveName_ClipWave_Safe(sWaveNameR.c_str());
			tbool bFoundWaveL = (sWavePathNameL.length() > 0);
			tbool bFoundWaveR = (sWavePathNameR.length() > 0);

			// Check for missing wave file(s)
			if (
				// mono missing?
				((!bIsOriginalStereo) && (!bFoundWaveL))
				||
				// stereo
				(
					(bIsOriginalStereo)
					&&
					(
						// stereo left missing?
						((iOriginalChannelMask & 0x01) && (!bFoundWaveL))
						||
						// stereo right missing?
						((iOriginalChannelMask & 0x02) && (!bFoundWaveR))
					)
				)
			)
			{
				// Missing wave file(s)

				// Do we have to guess?
				if ((!bIsOriginalLossy) || (sOriginalPathName.empty())) {
					// No chunk info about a compressed original
					// Instead guess at the name of compressed file
					std::string sOgg = GetFromWaveName_ClipDefaultOgg(sName.c_str());
					if (IFile::Exists(sOgg.c_str())) {
						sOriginalPathName = sOgg;
					}
				}

				// Can we decode a compressed original?
				if (IFile::Exists(sOriginalPathName.c_str())) {
					// We have a compressed file (must have just downloaded project)
					
					// Should we wait until we can decompress both channels of stereo file?
					if ((bIsOriginalStereo) && (iOriginalChannelMask == 1)) {
						// Left channel only - do nothing yet
					}
					else {
						// Queue decompress
						CImportAudioTask* pTask = new CImportAudioTask;
						tbool bSplit = ((bIsOriginalStereo) && (iOriginalChannelMask != 0x03));
						CImportAudioTask::EStereoBehavior eBehave = (bSplit) ? CImportAudioTask::geStereoDoSplit : CImportAudioTask::geStereoDoKeep;
						if (pTask->Init( sOriginalPathName.c_str(), false, eBehave, bIsOriginalLossy)) {
							pTask->mbCopyLossyOriginal = false;
							if (!pTask->mbSrcLossyCompressed) {
								// WAVE file
								if (pTask->meSrcQuality == ac::geQualityLossless16) {
									// No need to extrapolate details that aren't there
									pTask->miBitWidth = 16;
								}
							}
							else {
								// Ogg or mp3 file
								tbool bAbortOperation = false;

								if ((bAskedAboutBits) && (!bAskedIfSameBits)) {
									// Another ogg / mp3 file - do the same?
									tchar pszMsg[512];
									sprintf(
										pszMsg,
										"Extract all clips at %d bit depth?",
										iBitsToUse
										);
									ge::IWindow::EMsgBoxReturn eRc = ge::IWindow::ShowMessageBox(
										pszMsg, "Use same bit depth", ge::IWindow::MsgBoxYesNoCancel);
									bAskedIfSameBits = true;
									switch (eRc) {
										case ge::IWindow::MsgBoxRetYes: bUseSameBits = true; break;
										case ge::IWindow::MsgBoxRetNo: bUseSameBits = false; break;
										default: bAbortOperation = true; break;
									}
								}

								if (
									(!bAbortOperation)
									&&
									((!bAskedAboutBits) || ((bAskedIfSameBits) && (!bUseSameBits)))
								) {
									std::string sMsg =
										std::string("Do you want to extract as 24 bit Wave?\n")
										+
										sName
										+
										"\n"
										"\n"
										"- Yes:  \t24 bits = no quality loss\n"
										"- No:   \t16 bits = maybe quality loss\n"
										"- Cancel:\tAbort";

									ge::IWindow::EMsgBoxReturn eRc = ge::IWindow::ShowMessageBox(
										sMsg.c_str(), "Select bit depth", ge::IWindow::MsgBoxYesNoCancel);
									bAskedAboutBits = true;
									switch (eRc) {
										case ge::IWindow::MsgBoxRetYes: iBitsToUse = 24; break;
										case ge::IWindow::MsgBoxRetNo: iBitsToUse = 16; break;
										default: bAbortOperation = true; break;
									}
								}

								if (bAbortOperation) {
									// User cancelled a dialog box
									// Delete this task
									pTask->Destroy();
									pTask = NULL;
									// Unwind and delete any previously added tasks in list
									while (plistpImportTasks->size()) {
										pTask = *(plistpImportTasks->begin());
										pTask->Destroy();
										pTask = NULL;
										plistpImportTasks->erase(plistpImportTasks->begin());
									}
									msExtendedError = "You cancelled import";
									return false;
								}

								pTask->miBitWidth = iBitsToUse;
							}
							plistpImportTasks->insert(plistpImportTasks->end(), pTask);
						}
						else {
							msExtendedError = "Error queuing decompress: " + sName;
							msExtendedError += std::string("\n") + pTask->GetError();
							pTask->Destroy();
							CleanProject(0);
							msProjectPathName = "";
							msProjectFolder = "";
							SetProjectName(GetProjDir_Contents());
							return false;
						}
					}
				}
				else {
					// No waves and no compressed originals - no way out
					if ((bAskedAboutBroken) && (!bAskedAboutBrokenSame)) {
						ge::IWindow::EMsgBoxReturn eRc = ge::IWindow::ShowMessageBox(
							"Silently skip all missing files?",
							"Audio Clip Missing",
							ge::IWindow::MsgBoxYesNo);
						bAskedAboutBrokenSame = true;
						if (eRc == ge::IWindow::MsgBoxRetYes) {
							bBrokenSame = true;
						}
					}
					if (bBrokenSame) {
						iBrokenSkipped++;
					}
					else {
						std::string sMsg = "File missing for clip:\n  " + sName;
						sMsg += "\n\nDo you want to skip it and continue loading anyway?";
						sMsg += "\n\n(If you continue and then save, it'll be gone for good!)";
						ge::IWindow::EMsgBoxReturn eRc = ge::IWindow::ShowMessageBox(
							sMsg.c_str(), "Audio Clip Missing", ge::IWindow::MsgBoxOKCancel);
						bAskedAboutBroken = true;
						if (eRc == ge::IWindow::MsgBoxRetOK) {
							iBrokenSkipped++;
						}
						else {
							msExtendedError += "You cancelled project load";
							CleanProject(0);
							msProjectPathName = "";
							msProjectFolder = "";
							SetProjectName(GetProjDir_Contents());
							return false;
						}
					}
				}
			}
			else {
				// Wav file is already there - don't decompress

				// Queue peak file verify
				CImportAudioTask* pTask = new CImportAudioTask;
				tbool bSplit = ((bIsOriginalStereo) && (iOriginalChannelMask != 0x03));
				CImportAudioTask::EStereoBehavior eBehave = (bSplit) ? CImportAudioTask::geStereoDoSplit : CImportAudioTask::geStereoDoKeep;
				if (pTask->Init( sOriginalPathName.c_str(), true, eBehave, bIsOriginalLossy)) {
					plistpImportTasks->insert(plistpImportTasks->end(), pTask);
				}
				else {
					msExtendedError = "Error queuing peak file verify: " + sName;
					msExtendedError += std::string("\n") + pTask->GetError();
					pTask->Destroy();
					CleanProject(0);
					msProjectPathName = "";
					msProjectFolder = "";
					SetProjectName(GetProjDir_Contents());
					return false;
				}
			}
		}
	}
	
	// Verify that we have both sides of stereo
	if (sTemp_LeftSideOfStereoClip_Name.length() > 0) {
		msExtendedError = "Project contains no stereo right side for left side:\n";
		msExtendedError += sTemp_LeftSideOfStereoClip_WaveName;
		CleanProject(0);
		msProjectPathName = "";
		msProjectFolder = "";
		SetProjectName(GetProjDir_Contents());
		return false;
	}

	// - not here - UpdateGUIFileList();
	 */
	return true;
} // MenuFileLoadProject_QueueClips


tbool CKSApplication::CanWaveFilePlay(const std::string& sWaveFilePath, tbool bAllowErrorDialog, tbool bIsMissingFileAnError)
{
	std::string sError = "";

	CAutoDelete<IFile> pFileWave(IFile::Create());
	if (pFileWave) {
		if (!pFileWave->Open(sWaveFilePath.c_str(), IFile::FileRead)) {
			// No file
			if (!bIsMissingFileAnError) {
				// That's ok - just tell we don't have it
				return false;
			}
		}
		else {
			// We do have a file - now test it
			CAutoDelete<ac::IDecoder> pWaveDecoder(ac::IDecoder::Create(ac::geAudioCodecWave));
			if (pWaveDecoder) {
				tbool bSuccess = true;
				if (!pWaveDecoder->TestFile(pFileWave)) {
					tchar pszMsg[1024];
					pWaveDecoder->GetErrMsg(pszMsg, 1024);
					sError += pszMsg;
					bSuccess = false;
				}
				else {
					if (pWaveDecoder->miLastInputChannels != 1) {
						tchar pszMsg[64];
						sprintf(pszMsg, "Expected mono file, found %d channels\n", pWaveDecoder->miLastInputChannels);
						sError += pszMsg;
						bSuccess = false;
					}
					if (pWaveDecoder->miLastInputSamples <= 0) {
						tchar pszMsg[64];
						sprintf(pszMsg, "Expected > 0 samples, found %d samples\n", pWaveDecoder->miLastInputSamples);
						sError += pszMsg;
						bSuccess = false;
					}
					if ((pWaveDecoder->miLastInputBitWidth != 16) && (pWaveDecoder->miLastInputBitWidth != 24)) {
						tchar pszMsg[64];
						sprintf(pszMsg, "Expected 16 or 26 bits, found %d bits\n", pWaveDecoder->miLastInputBitWidth);
						sError += pszMsg;
						bSuccess = false;
					}
				}

				if (bSuccess) {
					return true;
				}
			}
		}
	}

	if (sError.length() == 0) {
		sError = "Unknown error.\n";
	}
	sError += "\nParsing file '" + sWaveFilePath + "'";
	if (bAllowErrorDialog) {
		ShowMessageBox_NonModal(sError.c_str(), "Wave file error");
	}
	return false;
} // CanWaveFilePlay


void CKSApplication::MenuFileDistributeMix(ac::EAudioCodec eCodec, tint32 iQuality, tint32 iChannels, tint32 iTailMS, tbool bNormalize)
{
	try {
		if (!GenerateMix(eCodec, (ac::EQuality)iQuality, iChannels, iTailMS, bNormalize)) {
			LoadSaveErrDlg("Error distributing mix");
		}
	}
	catch (IException* pEx) {
		// Display reason
		LoadSaveErrDlg(pEx->GetFullDescription());
	}
} // MenuFileDistributeMix


tbool CKSApplication::GenerateMix(ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels, tint32 iTailMS, tbool bNormalize)
{
	return ExportTracksOrMix(geStateExportingOutMix, NULL, false, eCodec, eQuality, iChannels, iTailMS, bNormalize);
} // GenerateMix


tbool CKSApplication::ExportSelectedTrack(ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels, tint32 iTailMS, tbool bNormalize)
{
	/* unused
	std::list<tint32> listiTracks;

	if (GetSelectedTrack() >= 0)
		listiTracks.insert(listiTracks.end(), GetSelectedTrack());

	return ExportTracksOrMix(
		geStateExportingTrack, &listiTracks,
		eCodec, eQuality, iChannels, iTailMS, bNormalize);
	*/
	return false;
} // ExportSelectedTrack


tbool CKSApplication::ExportAllTracks(ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels, tint32 iTailMS, tbool bNormalize)
{
	std::list<tint32> listiTracks;
	std::list<tint32> listiTracks_IncludeMuted;

	for(tint32  iTrack = 0; iTrack < giNumber_Of_Tracks; iTrack++)
	{
		if(GetGlobalParm(giParam_Ch_In_Use, iTrack + giSection_First_Track)) {
			CTrack_DSP* pTrack = gpDSPEngine->GetTrack(iTrack);
			if (pTrack->HasRegions()) {
				listiTracks_IncludeMuted.insert(listiTracks_IncludeMuted.end(), iTrack);
				if (!gpDSPEngine->IsTrackMuted(iTrack)) {
					listiTracks.insert(listiTracks.end(), iTrack);
				}
			}
		}
	}

	return ExportTracksOrMix(
		geStateExportingTrack, &listiTracks, true,
		eCodec, eQuality, iChannels, iTailMS, bNormalize);
} // ExportAllTracks


tbool CKSApplication::ExportTracksOrMix(EPlaybackState eExportState, std::list<tint32>* plistiTracks, tbool bSelectionOnly, ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels, tint32 iTailMS, tbool bNormalize)
{
	tbool bError = false;
	msExtendedError = "";

	if (msProjectFolder.length() == 0) {
		msExtendedError = "You must create or load a project first";
		return false;
	}

	tchar pszDefaultFolder[1024];
	GetDefaultProjectFolder(pszDefaultFolder);
	std::list<CExportDSPTask*> listpInfo;
	tint32 iNumberOfFullTrackExportsWithSilenceBefore = 0;

	tint64 iFirstSample = 0, iFinalSample = -1;
	if (bSelectionOnly) {
		tint32 iBlueTrack = Get_Selected_Track();
		if (iBlueTrack >= 0) {
			STrackSelectionInfo sSel = gpDSPEngine->GetTrackSelection(iBlueTrack);
			iFirstSample = sSel.uiSelection_Pos;
			iFinalSample = iFirstSample + sSel.uiSelection_Duration - 1;
		}
		if (iFinalSample < iFirstSample) {
			// Nothing selected - export everything
			bSelectionOnly = false;
		}
	}

	switch (eExportState) {
		case geStateExportingOutMix:
			{
				CExportDSPTask* pInfo = new CExportDSPTask( -1, "Mix down", eCodec, eQuality, iChannels, iTailMS);
				if (!bNormalize) {
					// Use altitude as it is - no peak search and normalization
					pInfo->mfNormalizationFactor = 1.0f;
				}
				if (bSelectionOnly) {
					pInfo->miFirstSample = iFirstSample;
					pInfo->miFinalSample = iFinalSample;
				}
				listpInfo.insert(listpInfo.end(), pInfo);
			}
			break;

		case geStateExportingTrack:
			{
				if (plistiTracks == NULL) {
					msExtendedError = "Internal error, tracks list is NULL";
					bError = true;
				}
				else if (plistiTracks->size() == 0) {
					// No track selected
					msExtendedError = "You didn't select a track";
					bError = true;
				}
				else {
					std::list<tint32>::iterator it = plistiTracks->begin();
					for ( ; it != plistiTracks->end(); it++) {
						tint32 iTrack = *it;
						std::string sTrackName = GetChannelName(iTrack);
						CExportDSPTask* pInfo = new CExportDSPTask( iTrack, sTrackName.c_str(), eCodec, eQuality, iChannels, iTailMS);
						if (!bNormalize) {
							// Use altitude as it is - no peak search and normalization
							pInfo->mfNormalizationFactor = 1.0f;
						}
						if (bSelectionOnly) {
							pInfo->miFirstSample = iFirstSample;
							pInfo->miFinalSample = iFinalSample;
						}
						else {
							// Find out range of track
							gpDSPEngine->CalcTrackDuration(iTrack, &pInfo->miFirstSample, &pInfo->miFinalSample);
							if (pInfo->miFirstSample > 0) {
								iNumberOfFullTrackExportsWithSilenceBefore++;
							}
						}
						listpInfo.insert(listpInfo.end(), pInfo);
					}
				}
			}
			break;
	}

	// Find destination
	tbool bDestinationFound = false;
	if (!bError) {
		const tchar* kpszExt = IEncoder::GetDefaultExt(eCodec);

		if (listpInfo.size() > 1) {
			// Several outputs - browse for folder

			// Let user select destination folder
			tchar pszDestinationFolder[1024];
			pszDestinationFolder[0] = '\0';
			CAutoDelete<ge::IPickDirDialog> pDlg(ge::IPickDirDialog::Create());
			pDlg->DoDialog(pszDestinationFolder, pszDefaultFolder);

			// Warn about overwritings
			tint32 iOverwritings = 0;
			std::string sOverwritten;
			tint32 iAndMore = 0;
			std::string sOverwrittenLast;
			if (*pszDestinationFolder) {
				// Se if any will be overwritten
				std::list<CExportDSPTask*>::iterator it = listpInfo.begin();
				while (it != listpInfo.end()) {
					CExportDSPTask* pInfo = *it;
					pInfo->sDestFolder = pszDestinationFolder;
					pInfo->sDestNameAndExt = pInfo->msTrackName + kpszExt;
					if (IFile::Exists((pInfo->sDestFolder + pInfo->sDestNameAndExt).c_str())) {
						iOverwritings++;
						sOverwrittenLast = std::string(" - ") + pInfo->sDestNameAndExt + "\n";
						if (iOverwritings < 10)
							sOverwritten += sOverwrittenLast;
						else {
							iAndMore++;
						}
					}
					it++;
				}

				// Overwritings?
				if (iOverwritings == 0) {
					// No overwritings - just go ahead
					bDestinationFound = true;
				}
				else {
					// Report overwritings
					if (iAndMore == 1)
						sOverwritten += sOverwrittenLast;
					else if (iAndMore > 1) {
						tchar pszAndMore[128];
						sprintf(pszAndMore, " (and %d more)\n", iAndMore);
						sOverwritten += pszAndMore;
					}

					tchar pszMsg[4096];
					sprintf(pszMsg,
						"Warning!\n"
						"\n"
						"This will result in the following file(s) being overwritten:\n"
						"%s"
						"\n"
						"Go ahead anyway?",
						sOverwritten.c_str());
					ge::IWindow::EMsgBoxReturn eRet = ge::IWindow::ShowMessageBox(
						pszMsg,
						"Overwritings",
						ge::IWindow::MsgBoxOKCancel);
					switch (eRet) {
						case ge::IWindow::MsgBoxRetOK:
							// Go ahead despite overwriting
							bDestinationFound = true;
							break;
						case ge::IWindow::MsgBoxRetCancel: 
							// Not an error
							pszDestinationFolder[0] = '\0';
							break;
					}
				}
			}
		}
		else {
			// One output only - ask user for folder and name

			tchar pszPathName[1024];
			pszPathName[0] = '\0';

			CExportDSPTask* pInfo = *(listpInfo.begin());

			std::string sDefaultName = pInfo->msTrackName + kpszExt;

			// Let user select destination + name
			CAutoDelete<ge::ISaveAsDialog> pDlg(ge::ISaveAsDialog::Create());
			pDlg->SetBundleBehaviour(1);
			pDlg->DoDialog(
				pszPathName, pszDefaultFolder,
				(std::string("*") + kpszExt).c_str(), IEncoder::GetFileTypeDesc(eCodec),
				sDefaultName.c_str());

			if (*pszPathName == '\0') {
				// User cancelled. Not an error
				//return true;
			}
			else {
				tchar* pcColon = strrchr(pszPathName, ':');
				if (pcColon != NULL) {
					tchar* pszNameAndExt = pcColon + 1;
					pInfo->sDestNameAndExt = pszNameAndExt;
					*pszNameAndExt = '\0';
					pInfo->sDestFolder = pszPathName;
					bDestinationFound = true;
				}
			}
		}
	}

	if (bDestinationFound) {
		if (iNumberOfFullTrackExportsWithSilenceBefore > 0) {
			std::string sMsg;
			if (iNumberOfFullTrackExportsWithSilenceBefore == 1) {
				if (listpInfo.size() == 1) {
					sMsg = "The track has";
				}
				else {
					sMsg = "One track has";
				}
			}
			else {
				if (iNumberOfFullTrackExportsWithSilenceBefore == listpInfo.size()) {
					sMsg = "The tracks have";
				}
				else {
					sMsg = "Some tracks have";
				}
			}
			sMsg += " a silent part before first audio.\n\nRemove initial silence?";
			ge::IWindow::EMsgBoxReturn eRc = ge::IWindow::ShowMessageBox(sMsg.c_str(), "Initial silence", ge::IWindow::MsgBoxYesNo);
			if (eRc == ge::IWindow::MsgBoxRetYes) {
				// Run through tasks and set them to remove initial silence
				std::list<CExportDSPTask*>::iterator it = listpInfo.begin();
				for ( ;it != listpInfo.end(); it++) {
					CExportDSPTask* pTask = *it;
					pTask->mbRemoveInitialSilence = true;
				}
			}
		}

		// Stop audio-card to avoid Process(...) being called
		gpDSPEngine->CloseAudioDevice_Duplex();
		gpDSPEngine->CloseAudioDevice_Input();
		gpDSPEngine->CloseAudioDevice_Output();

		// Add to queue
		std::list<CExportDSPTask*>::iterator it = listpInfo.begin();
		for ( ; it != listpInfo.end(); it++) {
			CExportDSPTask* pTask = *it;
			mpProgressTasks->Add(pTask);
		}

		// Start progress
		Playback_InProgressTask();
	}

	return !bError;
} // ExportTracksOrMix


void CKSApplication::DeleteFileThatOccupiesFolderName(std::string sFolderName)
{
	tint32 iLen = sFolderName.length();
	if (iLen > 0) {
		if (sFolderName[iLen - 1] == ':') {
			sFolderName.erase(iLen - 1);
		}
	}
	IFile::DeleteFile(sFolderName.c_str());
} // DeleteFileThatOccupiesFolderName


tbool CKSApplication::VerifyFolderCreated(std::string sFolderName)
{
	tbool bIsFolder;
	
//	printf("\n \n           +++++++++  pszDefault_Folder = %s ++++++++++++ \n \n", pszDefault_Folder);
	
	
	if ((!IFile::Exists(sFolderName.c_str(), &bIsFolder)) || (!bIsFolder)) {
		msExtendedError = "++Couldn't create folder:\n" + sFolderName;
		return false;
	}
	return true;
} // VerifyFolderCreated


tbool CKSApplication::MenuFileSaveProject(tbool bOverwrite /*= false*/)
{
	/*
	msExtendedError = "";
	
	if (msProjectPathName.length() == 0) {
		return MenuFileCreateNewProject();
	}

	std::string s = GetProjDir_Clips();
	// Delete any files that occupy the names of the folders we wish to create
	DeleteFileThatOccupiesFolderName(s);
	s = GetProjDir_Icons();
	DeleteFileThatOccupiesFolderName(s);
	s = GetProjDir_Contents();
	DeleteFileThatOccupiesFolderName(s);
	s = msProjectFolder;
	DeleteFileThatOccupiesFolderName(s);

	// This will also create project folder
	IFile::CreateDirectory(GetProjDir_ClipsDecomp().c_str());
	IFile::CreateDirectory(GetProjDir_Icons().c_str());

	// Create or rewrite project (to avoid attempting load of non-existing waves)
	if ((bOverwrite) || (!IFile::Exists(msProjectPathName.c_str()))) {
		CAutoDelete<IFile> pDummyProj(IFile::Create());
		if (pDummyProj->Open(msProjectPathName.c_str(), IFile::FileCreate)) {
			// File has been reset - just close again
			pDummyProj->Close();
		}
	}
	
	if (!VerifyFolderCreated(msProjectFolder))
		return false;
	if (!VerifyFolderCreated(GetProjDir_Clips()))
		return false;
	if (!VerifyFolderCreated(GetProjDir_Icons()))
		return false;
	
	// Overwrite existing files in bundle?
	if (bOverwrite) {
		CBaseGUI* pFirstGUI = *(GetGUIs().begin());
		ge::IResourceManager* pResMan = pFirstGUI->GetResMan();
		tchar pszOSFolder[1024];
		IFile::PathToOS2(msProjectFolder.c_str(), pszOSFolder);
		tchar pszLine[1024];
		tint32 iLineLen;

		// Windows
#ifdef _WIN32
		::PathUnmakeSystemFolder(pszOSFolder);
		tchar pszDesktopIni[1024];
		sprintf(pszDesktopIni, "%sDesktop.ini", pszOSFolder);
		DWORD dwAttribs = ::GetFileAttributes(pszDesktopIni);
		dwAttribs &= !(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY);
		::SetFileAttributes(pszDesktopIni, dwAttribs);
#endif // _WIN32
		ge::IRawResource* pIconFileWin = pResMan->GetRawResource(IDI_APP_WIN);
		CAutoDelete<IFile> fIconWin(IFile::Create());
		if (!fIconWin->Open((GetProjDir_Icons() + "win.ico").c_str(), IFile::FileCreate)) {
			msExtendedError = std::string("Error creating file: ") + GetProjDir_Icons() + "win.ico";
			return false;
		}
		fIconWin->Write((tchar*)pIconFileWin->GetData(), pIconFileWin->GetDataSize());
		pIconFileWin->DecRefCount();
		CAutoDelete<IFile> fDesktopIni(IFile::Create());
		if (!fDesktopIni->Open((msProjectFolder + "Desktop.ini").c_str(), IFile::FileCreate)) {
			msExtendedError = std::string("Error creating file: ") + GetProjDir() + "Desktop.ini";
			return false;
		}
		iLineLen = sprintf(
			pszLine,
			"[.ShellClassInfo]\r\n"
			"IconFile=Contents\\Resources\\win.ico\r\n"
			"IconIndex=0\r\n"
			"InfoTip=Koblo Studio Project\r\n"
			);
		fDesktopIni->Write(pszLine, iLineLen);
#ifdef _WIN32
		::PathMakeSystemFolder(pszOSFolder);
#endif // _WIN32

		// OS X
		ge::IRawResource* pIconFileOSX = pResMan->GetRawResource(IDI_APP_OSX);
		CAutoDelete<IFile> fIconOSX(IFile::Create());
		if (!fIconOSX->Open((GetProjDir_Icons() + "osx.icns").c_str(), IFile::FileCreate)) {
			msExtendedError = std::string("Error creating file: ") + GetProjDir_Icons() + "osx.icns";
			return false;
		}
		fIconOSX->Write((tchar*)pIconFileOSX->GetData(), pIconFileOSX->GetDataSize());
		pIconFileOSX->DecRefCount();
		CAutoDelete<IFile> fInfoPlist(IFile::Create());
		if (!fInfoPlist->Open((GetProjDir_Contents() + "Info.plist").c_str(), IFile::FileCreate)) {
			msExtendedError = std::string("Error creating file: ") + GetProjDir_Contents() + "Info.plist";
			return false;
		}
		iLineLen = sprintf(
			pszLine,
			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			"<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
			"<plist version=\"1.0\">\n"
			"<dict>\n"
			"\t<key>CFBundleDevelopmentRegion</key>\n"
			"\t<string>English</string>\n"
			//<key>CFBundleExecutable</key>
			//<string>${EXECUTABLE_NAME}</string>
			"\t<key>CFBundleIconFile</key>\n"
			"\t<string>osx</string>\n"
			"\t<key>CFBundleIdentifier</key>\n"
			"\t<string>com.Koblo.KS</string>\n"
			"\t<key>CFBundleInfoDictionaryVersion</key>\n"
			"\t<string>6.0</string>\n"
			"\t<key>CFBundleName</key>\n"
			"\t<string>Koblo Studio Project</string>\n"
			"\t<key>CFBundlePackageType</key>\n"
			"\t<string>KBLO</string>\n"
			"\t<key>CFBundleSignature</key>\n"
			"\t<string>KS  </string>\n"
			"\t<key>CFBundleVersion</key>\n"
			"\t<string>1.0</string>\n"
			"\t<key>CSResourcesFileMapped</key>\n"
			"\t<true/>\n"
			"</dict>\n"
			"</plist>\n"
			);
		fInfoPlist->Write(pszLine, iLineLen);
		CAutoDelete<IFile> fPkgInfo(IFile::Create());
		if (!fPkgInfo->Open((GetProjDir_Contents() + "PkgInfo").c_str(), IFile::FileCreate)) {
			msExtendedError = std::string("Error creating file: ") + GetProjDir_Contents() + "PkgInfo";
			return false;
		}
		iLineLen = sprintf(
			pszLine,
			"KBLO" "KS  "
			);
		fPkgInfo->Write(pszLine, iLineLen);
	}
	
#ifdef _WIN32
	// Allow write access to project file
	DWORD dwAttribs = ::GetFileAttributes(msProjectPathName.c_str());
	dwAttribs &= !(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY );
	::SetFileAttributes(msProjectPathName.c_str(), dwAttribs);
#endif // _WIN32

	// Create project file
	CAutoDelete<IChunkFile> pFile(IChunkFile::Create());
	tint32 iVersionNumber = 1;
	if (pFile->Open(msProjectPathName.c_str(), IFile::FileCreate, iVersionNumber) == false) {
		tchar pszMsg[1024];
		sprintf(pszMsg, "Cannot save project file: \"%s\"", msProjectPathName.c_str());
		//throw IException::Create(IException::TypeFile, IException::ReasonFileCannotOpen, EXCEPTION_INFO, pszMsg);
		msExtendedError = pszMsg;
		return false;
	}

	if (1) {
		CAutoDelete<ge::IWaitCursor> pWaitCursor(ge::IWaitCursor::Create());
		
		tint32 iSection;
		for (iSection = 0; iSection < giNrOfSections; iSection++) {
			CAutoDelete<IChunk> pChunk(IChunk::Create(NULL,
													  0,
													  giChunkPerformance));
			{
				tint32 iParmCount = mpParmMan->GetGlobalDataSize(iSection);
				
				// Chunk header
				tint32 iValue;
				// First parameter
				iValue = 0;
				iValue = CPSwap(iValue);
				pChunk->Write((const tchar*)&iValue, sizeof(tint32));
				// Parameter count
				iValue = iParmCount;
				iValue = CPSwap(iValue);
				pChunk->Write((const tchar*)&iValue, sizeof(tint32));
				
				// Get the parameter data
				tint32* piParmData = new tint32[iParmCount];
				mpParmMan->GetGlobalData(piParmData, 0, iSection, true);
				
				// Write parms to chunk
				pChunk->Write((const tchar*)piParmData, iParmCount * sizeof(tint32));
				
				delete[] piParmData;
			}
			
			// Add chunk to chunk file
			pFile->SetChunk(pChunk, true);
		}
		
		// Save GUI positions of tracks
		if (1) {
			CAutoDelete<IChunk> pChunk(IChunk::Create(NULL, 0, 'TPos'));
			pChunk->Write((tchar*)(&msStack), sizeof(msStack));
			pFile->SetChunk(pChunk, true);
		}
		
		std::list<CSample_Data*>::iterator itSample_List = mSample_Data_List.begin();
		
		for (; itSample_List != mSample_Data_List.end(); itSample_List++) {
			
			// Save wave info
			CAutoDelete<IChunk> pChunk(IChunk::Create(NULL, 0, 'Clip'));
			CSample_Data* pSample_Data = *itSample_List;
			CTake_Data*	pTake_Info		=	pSample_Data->Get_Take_Data();
			
			pChunk->Write(pSample_Data->sName.c_str(), pSample_Data->sName.length());
			pChunk->Write("\0", 1);
			pChunk->Write(pTake_Info->sWaveNameL.c_str(), pTake_Info->sWaveNameL.length());
			pChunk->Write("\0", 1);
			pChunk->Write(pTake_Info->sWaveNameR.c_str(), pTake_Info->sWaveNameR.length());
			pChunk->Write("\0", 1);
	//		pChunk->Write(pTake_Info->sOriginalName.c_str(), pTake_Info->sOriginalName.length());
			pChunk->Write("\0", 1);
	//		pChunk->Write(pTake_Info->sOriginalExt.c_str(), pTake_Info->sOriginalExt.length());
			pChunk->Write("\0", 1);
	//		pChunk->Write(pTake_Info->bIsOriginalStereo ? "T" : "F", 2);
	//		pChunk->Write(pTake_Info->bIsOriginalLossy ? "T" : "F", 2);
			tchar pszOriginalChannelMask[2];
			pszOriginalChannelMask[0] = '0' + pTake_Info->iOriginalChannelMask;
			pszOriginalChannelMask[1] = '\0';
			pChunk->Write(pszOriginalChannelMask, 2);
			pChunk->Write(pTake_Info->bIsStereoInList ? "T" : "F", 2);
			pFile->SetChunk(pChunk, true);
			
		}
		
		tint32 iVisibleTracks = Get_Number_Of_Tracks();
		
		// Count how many tracks to save (only save first, used part)
		tint32 iTracksToSave = 0;
		tint32 iFoundVisible = 0;
		for ( ; (iTracksToSave < giNumber_Of_Tracks) && (iFoundVisible < iVisibleTracks); iTracksToSave++) {
			tbool bTrackVisible = (GetGlobalParm(giParam_Ch_In_Use, iTracksToSave + giSection_First_Track) != 0);
			if (bTrackVisible) {
				iFoundVisible++;
			}
		}
		
		tint32 iTrack;
		
		// Save names of visible tracks
		for (iTrack = 0; iTrack < iTracksToSave; iTrack++) {
			CAutoDelete<IChunk> pChunk(IChunk::Create(NULL,
													  0,
													  'NAME'));
			{
				std::string sChannelName = GetChannelName(iTrack);
				if (sChannelName.size() != 0) {
					// Write to chunk
					pChunk->Write(sChannelName.c_str(), sChannelName.size());
				}
				
			}
			
			// Add chunk to chunk file
			pFile->SetChunk(pChunk, true);
		}
		
		CDSP* pDSP = dynamic_cast<CDSP*>(GetDSPEngine());
		
		// Save wave regions of visible tracks
		for (iTrack = 0; iTrack < iTracksToSave; iTrack++) {
			CAutoDelete<IChunk> pChunk(IChunk::Create(NULL, 0, 'REGI'));
			
			if (pDSP->SaveTrackRegionDataToChunk(iTrack, pChunk)) {
				// chunk was updated implicitly - do nothing here
			}
			
			// Add chunk to chunk file
			pFile->SetChunk(pChunk, true);
		}
		
		{
			tint32 iChannels = giNumber_Of_Tracks;
			tint32 iChannel = 0;
			for (iChannel = 0; iChannel < iChannels; iChannel++) {
				CTrack_DSP* pChannel = pDSP->GetTrack(iChannel);
				//tint32 iInserts = 4;
				//tint32 iInsert;
				for (tint32 iInsert = 0; iInsert < giNumber_Of_Inserts; iInsert++) {
					kspi::IPlugIn* pPlugIn = pChannel->GetInsert(iInsert);
					if (pPlugIn) {
						CAutoDelete<IChunk> pChunk(IChunk::Create(NULL, 0, 'INSR'));
						
						pChunk->WriteCP(&iChannel, 1);
						pChunk->WriteCP(&iInsert, 1);
						
						tint32 iSize = pPlugIn->GetChunkSize();
						tchar* p = new tchar[iSize];
						pPlugIn->GetChunk(p);
						pChunk->Write(p, iSize);
						delete[] p;
						
						pFile->SetChunk(pChunk, true);
					}
				}
			}
		}
		
#ifdef _WIN32
		// Flush project file to disk
		pFile->Close();
		
		// Deny write access to project file
		dwAttribs |= ( FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY );
		::SetFileAttributes(msProjectPathName.c_str(), dwAttribs);
#endif // _WIN32

	}
	*/
	// No error
	return true;
} // MenuFileSaveProject


tbool CKSApplication::QueueAudioFileImport(const tchar* pszPathName, tbool bAlwaysKeepStereo, tint32 iTrackID /*=-1*/, tint64 iTrackPos /*= -1*/)
{
	/*
	if (IsPlayingOrRecording()) {
		PlaybackStop();
	}

	CImportAudioTask* pImportAudioTask = new CImportAudioTask();

	CImportAudioTask::EStereoBehavior eBehave = (bAlwaysKeepStereo) ? CImportAudioTask::geStereoDoKeep : CImportAudioTask::geStereoDoAsk;
	tbool bSuccess = pImportAudioTask->Init( pszPathName, false, eBehave, false);
	
	
	if (bSuccess) {
		mpProgressTasks->Add(pImportAudioTask);
		Playback_InProgressTask();
		//bSuccess = DoAudioFileImport(pInfo);
	}
	if (!bSuccess) {
		msExtendedError = pImportAudioTask->GetError();
		pImportAudioTask->Destroy();
	}
	//delete pTask;
	 
	return bSuccess;
	*/
	return true;
} // QueueAudioFileImport

tbool CKSApplication::Take_Is_In_Use(std::string sName)
{
	/*
	std::list<CSample_Data*>::iterator itSample_Info = mSample_Data_List.begin();
	for ( ;itSample_Info != mSample_Data_List.end(); itSample_Info++) {
		
		CSample_Data* pSample_Data		=	*itSample_Info;
		CTake_Data*	pTake_Info			=	pSample_Data->Get_Take_Data();
		
		if(stricmp(pTake_Info->sOriginalName, sName) == 0)
			return true;
		
	}
	 */
	return false;

}

tbool CKSApplication::IsClipNameInUse(const tchar* pszClipName, const tchar* pszWaveNameL, const tchar* pszWaveNameR, std::string* psDescription)
{
	/*
	const tchar** ppszNames = new const tchar*[4];
	ppszNames[0] = pszClipName;
	ppszNames[1] = pszWaveNameL;
	ppszNames[2] = pszWaveNameR;
	ppszNames[3] = NULL;

	std::list<CSample_Data*>::iterator itSample_Info = mSample_Data_List.begin();
	for ( ;itSample_Info != mSample_Data_List.end(); itSample_Info++) {
		
		CSample_Data* pSample_Data = *itSample_Info;
		CTake_Data*	pTrack_Info	=	pSample_Data->Get_Take_Data();
		
		
		const tchar* pszName = pSample_Data->sName.c_str();
		const tchar* pszNameL = pTrack_Info->sWaveNameL.c_str();
		const tchar* pszNameR = pTrack_Info->sWaveNameR.c_str();
//		const tchar* pszOrgName = pTrack_Info->sOriginalName.c_str();
		tint32 iNewNameIx = 0;
		for (const tchar* pszNewName = ppszNames[iNewNameIx++]; pszNewName; pszNewName = ppszNames[iNewNameIx++]) {
			if (
				(stricmp(pszNewName, pszName) == 0)
				||
				(stricmp(pszNewName, pszNameL) == 0)
				||
				(stricmp(pszNewName, pszNameR) == 0)
				||
				(stricmp(pszNewName, pszOrgName) == 0)
			)
			{
				if (psDescription)
					*psDescription = std::string("You already imported '") + pszClipName + "'.";
				return true;
			}
		}
		if (bAlwaysStereo) {
			bSplit = false;
		}
	}
	delete[] ppszNames;
*/
	return false;
} // IsClipNameInUse


tbool CKSApplication::StartProgressTasks(EPlaybackState eState, tint64 iPos /*= -1*/, tint32 iTrack /*= -1*/)
{
	CAutoLock Lock(mMutex_Progress);
	
	return StartProgressTasks_NoLock(eState, iPos, iTrack);
} // StartProgressTasks

tbool CKSApplication::StartProgressTasks_NoLock(EPlaybackState eState, tint64 iPos /*= -1*/, tint32 iTrack /*= -1*/)
{
	if (mpProgressTasks->IsDone())
		return false;

	if (iPos >= 0) {
		miSongPos = iPos;
		gpDSPEngine->SetSongPosition_AndResetEffectsTails(miSongPos);
	}

	if (iTrack >= 0) {
		miTrackToExportNow = iTrack;
	}

	tbool bMustInitTimer = (!IsInProgressTaskState());
	mePlaybackState = eState;

#ifdef _WIN32
	// For Windows it's totally ok to ::Sleep(..) for zero time (will release time only if other tasks are pending)
	//const tint32 kiVeryShortTimeoutMS = 0;
	// ... but we wait a little anyways - if not we have weird results from time to time
	const tint32 kiVeryShortTimeoutMS = 1;
#endif // _WIN32

#ifdef _Mac
	// For OS X it seems *not* to be documented what happens if you try to ::MPWaitOnSemaphore(..) for zero time. So we better not do it...
	const tint32 kiVeryShortTimeoutMS = 1;
#endif // _Mac

	// Maybe init timer
	if (mpTimer_ProgressTasks == NULL) {
		mpTimer_ProgressTasks = ITimer::Create();
		bMustInitTimer = true;
	}
	if (bMustInitTimer) {
		mpTimer_ProgressTasks->Init(giTimerID_ProgressTasks, this, kiVeryShortTimeoutMS);
	}
	return true;
} // StartProgressTasks_NoLock


tbool CKSApplication::StopProgressTasks()
{
	CAutoLock Lock(mMutex_Progress);
	
	return StopProgressTasks_NoLock();
} // StopProgressTasks

tbool CKSApplication::StopProgressTasks_NoLock()
{
	if (!mpProgressTasks->IsDone())
		return false;

	// "Abort" - this won't panic anything since we're already done
	AbortProgressTasks_NoLock();
	return true;
} // StopProgressTasks_NoLock


void CKSApplication::AbortProgressTasks(tbool bNoDialogBoxOnAbort /*= false*/)
{
	CAutoLock Lock(mMutex_Progress);
	
	AbortProgressTasks_NoLock(bNoDialogBoxOnAbort);
} // AbortProgressTasks

void CKSApplication::AbortProgressTasks_NoLock(tbool bNoDialogBoxOnAbort /*= false*/)
{
	if (mpTimer_ProgressTasks) {

		// Do we need to interrupt an ongoing progress task?
		if (!mpProgressTasks->IsDone()) {
			// Set abort state = true
			mpProgressTasks->DoAbort(bNoDialogBoxOnAbort);

			const tint32 kiSleepTimeMS = 1;

			// Wait for abort operation to conclude
			while (IsInProgressTaskState()) {

#ifdef _WIN32
				::Sleep(kiSleepTimeMS);
#endif // _WIN32

#ifdef _Mac
				::MPSemaphoreID Semaphore;
				::MPCreateSemaphore(1, 0, &Semaphore);
				::MPWaitOnSemaphore(Semaphore, kiSleepTimeMS * kDurationMillisecond);
#endif // _Mac

			}
		}

		// Mark as stopped
		//delete mpTimer_ProgressTasks;
		//mpTimer_ProgressTasks = NULL;
	}
	mePlaybackState = geStateStopped;
	gpDSPEngine->LimitDataFromStreams(false, 0, -1);
} // AbortProgressTasks_NoLock


void CKSApplication::OnTimer(tint32 iID)
{
	// This is never called - use OnTimer_CanStop(..)
} // OnTimer


tbool CKSApplication::OnTimer_CanStop(tint32 iID)
{
	tbool bContinue = true;
	switch (iID) {
		case giTimerID_ProgressTasks:
			{
				DoProgressTasks();
				if (1) {
					CAutoLock Lock(mMutex_Progress);
					if (!IsInProgressTaskState()) {
						// We shouldn't do this anymore
						bContinue = false;

						// Clear progress bar
						Set_Progress(false, 0.0f, "");
					}
				}
			}
			break;

		case giTimerID_NonModalDialog:
			{
				SNonModalDialogInfo* pInfo = NULL;
				if (NonModalDialog_Pop(&pInfo)) {
#ifdef _WIN32
					ge::IWindow::ShowMessageBox(
						pInfo->sLongerText.c_str(),
						pInfo->sHeader.c_str()
						);
#endif // WIN32
					
#ifdef _Mac
					const tchar* pszHeader = pInfo->sHeader.c_str();
					const tchar* pszMessage = pInfo->sLongerText.c_str();
					
					CFStringRef strCaption = CFSTR("Error");
					if (pszHeader != NULL) {
						strCaption = CFStringCreateWithCStringNoCopy(NULL, pszHeader, kCFStringEncodingASCII, kCFAllocatorNull); 
					}
					CFStringRef strError = CFSTR("");
					tint iErrorLen = (pszMessage == NULL) ? 0 : strlen(pszMessage);
					if (iErrorLen > 0) {
						strError = CFStringCreateWithCStringNoCopy(NULL, pszMessage, kCFStringEncodingASCII, kCFAllocatorNull);
					}
					
					AlertStdCFStringAlertParamRec ParamRec;
					ParamRec.version = kStdCFStringAlertVersionOne;
					ParamRec.movable = 1;
					ParamRec.helpButton = 0;
					//ParamRec.filterProc = nil;
					ParamRec.defaultButton = kAlertStdAlertOKButton;
					ParamRec.cancelButton = kAlertStdAlertCancelButton;
					ParamRec.position = kWindowAlertPositionParentWindowScreen;
					ParamRec.flags = 0;
					
					ParamRec.defaultText = CFSTR("OK");
					ParamRec.cancelText = nil;
					ParamRec.otherText = nil;
					DialogRef DlgRef;
					
					//OSStatus osStat = ::CreateStandardAlert(kAlertPlainAlert, strCaption, strError, &ParamRec, &DlgRef);
					OSStatus osStat = ::CreateStandardSheet(kAlertPlainAlert, strCaption, strError, &ParamRec, NULL, &DlgRef);
					if (osStat == 0) {
						std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
						CBaseGUI* pGUI = *it;
						ge::IWindow* pWnd = pGUI->GetWindow();
						WindowRef winref = (WindowRef)(pWnd->GetParent());
						osStat = ::ShowSheetWindow(GetDialogWindow(DlgRef), winref);
						if (osStat != 0) {
							tint32 iSomethingRotten = 0;
							iSomethingRotten++;
						}
						//::ShowWindow(GetDialogWindow(DlgRef));
						//::DisposeDialog(DlgRef);
						//DialogItemIndex hit;
						//::RunStandardAlert(DlgRef, NULL, &hit);
					}
					
#endif // _Mac
					
					delete pInfo;
					pInfo = NULL;
				}
			}
			break;

	}
	return bContinue;
} // OnTimer_CanStop


void CKSApplication::NonModalDialog_Push(const tchar* pszHeader, const tchar* pszLongerText)
{
	CAutoLock lock(mMutex_NonModalDialog);

	SNonModalDialogInfo* pInfo = new SNonModalDialogInfo;
	pInfo->sHeader = pszHeader;
	pInfo->sLongerText = pszLongerText;
	mlistpNonModalDialogInfo.push_back(pInfo);
} // NonModalDialog_Push

tbool CKSApplication::NonModalDialog_Pop(SNonModalDialogInfo** ppInfo)
{
	CAutoLock lock(mMutex_NonModalDialog);

	if (mlistpNonModalDialogInfo.size()) {
		SNonModalDialogInfo* pInfo = mlistpNonModalDialogInfo.front();
		mlistpNonModalDialogInfo.pop_front();
		*ppInfo = pInfo;

		return true;
	}

	return false;
} // NonModalDialog_Pop

void CKSApplication::NonModalDialogs_ZapList()
{
	CAutoLock lock(mMutex_NonModalDialog);

	while (mlistpNonModalDialogInfo.size()) {
		SNonModalDialogInfo* pInfo = mlistpNonModalDialogInfo.front();
		delete pInfo;
		mlistpNonModalDialogInfo.pop_front();
	}
} // NonModalDialogs_ZapList

tbool CKSApplication::DoProgressTasks()
{
	CAutoLock Lock(mMutex_Progress);

	if (mpProgressTasks->IsDone()) {
		if (IsInProgressTaskState()) {
			// Return to normality
			StopProgressTasks_NoLock();

			// We may need to re-start audio device (after exporting track(s) or mix down)
			if (!gpDSPEngine->IsAudioDeviceAllocated()) {
				// Re-start audio-card to get ordinary Process(...) running again - ignore errors
				tbool bAudioDevice_LastOpenCrashed_Dummy = false;
				tbool bMidiDevice_LastOpenCrashed_Dummy = false;
				if (gpDSPEngine->OpenAudioDevice_FromPrefs_Output(
					&bAudioDevice_LastOpenCrashed_Dummy, &bMidiDevice_LastOpenCrashed_Dummy)
				) {
					gpDSPEngine->StartAudioDevices();
				}
			}
		}

		return true;
	}

	tbool bWorkSuccess = false;
	if (mpProgressTasks->DoWork()) {
		msProgress = mpProgressTasks->GetProgress(&muiProgressIx, &muiProgressAll);
		bWorkSuccess = true;
		ShowProgress(muiProgressIx, muiProgressAll, msProgress.c_str(), NULL, NULL);
	}
	else {
		msExtendedError = mpProgressTasks->GetError();
		bWorkSuccess = false;
	}

	if (!bWorkSuccess) {
		if (msExtendedError.length() == 0)
			msExtendedError = "Unknown error.";
		ShowMessageBox_NonModal(msExtendedError.c_str(), "Task failed");
		AbortProgressTasks_NoLock(true);
	}
	return bWorkSuccess;
} // DoProgressTasks

void CKSApplication::AddClipToList(CImportAudioTask* pImportAudioTask)
{
	CSample_Data	Sample_Data;
	CTake_Data*		pTake_Data			=	Sample_Data.Get_Take_Data();
	pTake_Data->Set_UUID(  pImportAudioTask->Get_UUID()  );
	
	Sample_Data.Name( pImportAudioTask->Screen_Name() );
	pTake_Data->Screen_Name(pImportAudioTask->Screen_Name());
	pTake_Data->Mode( pImportAudioTask->Stereo() ? "stereo": "mono");
	pTake_Data->Left_Wave_File_Path( pImportAudioTask->Left_Path() );
	pTake_Data->Right_Wave_File_Path( pImportAudioTask->Right_Path() );
	pTake_Data->Left_Peak_File_Path	(pImportAudioTask->Left_Peak_File_Path());
	pTake_Data->Right_Peak_File_Path(pImportAudioTask->Right_Peak_File_Path());
	

	mSample_Data_List.push_back(Sample_Data);
	
	//!!! why does this leak has to stay
	// if i delete pSample_Date I chrash!!
//	delete Sample_Data;
	
	UpdateGUIFileList();
}

void CKSApplication::AddClipToList(CTake_Data* pTake_Data_Input)
{
	
	CSample_Data	Sample_Data;
	CTake_Data*		pTake_Data			=	Sample_Data.Get_Take_Data();
	pTake_Data->Set_UUID(  pTake_Data_Input->Get_UUID()  );
	
	Sample_Data.Name( pTake_Data_Input->Screen_Name() );
	pTake_Data->Screen_Name(pTake_Data_Input->Screen_Name());
	pTake_Data->Mode( pTake_Data_Input->Mode() );
	pTake_Data->Left_Wave_File_Path( pTake_Data_Input->Left_Wave_File_Path() );
	pTake_Data->Right_Wave_File_Path( pTake_Data_Input->Right_Wave_File_Path() );
	pTake_Data->Left_Peak_File_Path	(pTake_Data_Input->Left_Peak_File_Path());
	pTake_Data->Right_Peak_File_Path(pTake_Data_Input->Right_Peak_File_Path());
	

	
	
	mSample_Data_List.push_back(Sample_Data);
	
	//!!! why does this leak has to stay
	// if i delete pSample_Date I chrash!!
//	delete pSample_Data;
	
	UpdateGUIFileList();
	 
}

tbool CKSApplication::OnAudioFileImport()
{
	Open_Close_Import_Audio_Window();
	return true;
} // OnAudioFileImport

void CKSApplication::VerifyCreatePeakFiles(CKSFile_Item* pFile_Item , tbool bForceCreate)
{
	tbool bStereo = pFile_Item->Stereo();
	
	// This may take some time
	CAutoDelete<ge::IWaitCursor> pWaitCursor(ge::IWaitCursor::Create());
	
	// Create peak files
#ifdef _Mac_PowerPC
	std::string sPeakFileL0		=	pFile_Item->Left_Peak_File_Path()	+ ".kspk1024_ppc";
	std::string sPeakFileL1		=	pFile_Item->Left_Peak_File_Path()	+ ".kspk64_ppc";
	std::string sPeakFileR0		=	pFile_Item->Right_Peak_File_Path()	+ ".kspk1024_ppc";
	std::string sPeakFileR1		=	pFile_Item->Right_Peak_File_Path()	+ ".kspk64_ppc";

#else _Mac_PowerPC
	std::string sPeakFileL0		=	pFile_Item->Left_Peak_File_Path()	+ ".kspk1024";
	std::string sPeakFileL1		=	pFile_Item->Left_Peak_File_Path()	+ ".kspk64";
	std::string sPeakFileR0		=	pFile_Item->Right_Peak_File_Path()	+ ".kspk1024";
	std::string sPeakFileR1		=	pFile_Item->Right_Peak_File_Path()	+ ".kspk64";

#endif // _Mac_PowerPC

	std::auto_ptr<CWave_File> pWaveFileL(new CWave_File());
	std::auto_ptr<CWave_File> pWaveFileR(new CWave_File());
	IFile* pSrcFileL;
	tint32 iOffsetL			= 0;
	tint32 iLengthL			= 0;
	IFile* pSrcFileR		= 0;
	tint32 iOffsetR			= 0;
	tint32 iLengthR			= 0;
	tint32 iBitWidthL		= 0, iBitWidthR = 0;
	tint32 iChannels_Dummy;
	tint32 iByteWidthL, iByteWidthR;
	
	
	if (pWaveFileL->LoadSoundStream(512, pFile_Item->Left_Path().c_str()))
		pWaveFileL->GetStreamInfo(pSrcFileL, iOffsetL, iLengthL, &iBitWidthL,&iChannels_Dummy);
	
	if (bStereo) {
		if (pWaveFileR->LoadSoundStream(512, pFile_Item->Right_Path().c_str()))
			pWaveFileR->GetStreamInfo(pSrcFileR, iOffsetR, iLengthR, &iBitWidthR, &iChannels_Dummy);
	}
	iByteWidthL = iBitWidthL / 8;
	iByteWidthR = iBitWidthR / 8;

	if (!bStereo) {
		// Mono
		tint32 iPeakFile;
		for (iPeakFile = 0; iPeakFile < 2; iPeakFile++) {
			CAutoDelete<IFile> pPeakFileL(IFile::Create());

			tint32 iSize = 1024;
			if (iPeakFile == 1) {
				iSize = 64;
			}

			std::string sPeakFileL = sPeakFileL0;

			if (iPeakFile == 1) {
				sPeakFileL = sPeakFileL1;
			}

			tint32 iPeakSize = iLengthL / iSize + 1;

			tbool bAnyMissing = bForceCreate;
			CAutoDelete<IFile> pPeakTest(IFile::Create());
			if (!IFile::Exists(sPeakFileL.c_str())) {
				bAnyMissing = true;
			}
			else {
				if (!pPeakTest->Open(sPeakFileL.c_str(), IFile::FileRead))
					bAnyMissing = true;
				else {
					if (pPeakTest->GetSizeWhenOpened() != (iPeakSize * sizeof(tfloat)))
						bAnyMissing = true;
					pPeakTest->Close();
				}
			}

			if (bAnyMissing) {
				tfloat32* pfPeak = new tfloat[iPeakSize];
				memset(pfPeak, 0, iPeakSize * sizeof(tfloat32));

				tchar* pch = new tchar[iSize * iByteWidthL];
				tint32 iReadIndex = 0;
				tint32 iToGo = iLengthL;
				tint32 iPeakIndex = 0;
				while (iToGo) {
					tint32 iThisBuffer = iSize;
					if (iThisBuffer > iToGo) {
						iThisBuffer = iToGo;
					}

					pSrcFileL->Seek(iReadIndex * iByteWidthL + iOffsetL);
					pSrcFileL->Read(pch, iThisBuffer * iByteWidthL);

					tfloat32 fMax = 0.0f;
					if (iBitWidthL == 24) {
						tint32 i;
						tchar* pcWave = pch;					
						for (i = 0; i < iThisBuffer; i++) {
							
							tint32 i32 = *((tint32*)(pcWave));
							pcWave += 3;
							Inplace24to32(i32);
							tfloat32 f = (tfloat32)(i32 / 8388607.0);

							if (fabsf(f) > fMax) {
								fMax = fabsf(f);
							}
						}
					}
					else { // if (iBitWidthL == 16) {
						tint32 i;
						tchar* pcWave = pch;					
						for (i = 0; i < iThisBuffer; i++) {
							
							tint16 i16 = CPSwap(*((tint16*)(pcWave)));
							pcWave += 2;
							tfloat32 f = (tfloat32)(i16 / 32767.0);

							if (fabsf(f) > fMax) {
								fMax = fabsf(f);
							}
						}
					}
					pfPeak[iPeakIndex] = fMax;

					iReadIndex += iThisBuffer;
					iToGo -= iThisBuffer;
					iPeakIndex++;
				}

				delete[] pch;

				pPeakFileL->Open(sPeakFileL.c_str(), IFile::FileCreate);
				pPeakFileL->Write((const tchar*)pfPeak, iPeakSize * sizeof(tfloat32));

				delete[] pfPeak;
			}
		}
	}
	else {
		// Stereo
		tint32 iPeakFile;
		for (iPeakFile = 0; iPeakFile < 2; iPeakFile++) {
			CAutoDelete<IFile> pPeakFileL(IFile::Create());
			CAutoDelete<IFile> pPeakFileR(IFile::Create());

			tint32 iSize = 1024;
			if (iPeakFile == 1) {
				iSize = 64;
			}

			std::string sPeakFileL = sPeakFileL0;
			std::string sPeakFileR = sPeakFileR0;
			if (iPeakFile == 1) {
				sPeakFileL = sPeakFileL1;
				sPeakFileR = sPeakFileR1;
			}

			tint32 iPeakSize = iLengthL / iSize + 1;

			tbool bAnyMissing = bForceCreate;
			CAutoDelete<IFile> pPeakTest(IFile::Create());
			if ((!IFile::Exists(sPeakFileL.c_str())) || (!IFile::Exists(sPeakFileR.c_str()))) {
				bAnyMissing = true;
			}
			else {
				if (!pPeakTest->Open(sPeakFileL.c_str(), IFile::FileRead))
					bAnyMissing = true;
				else {
					if (pPeakTest->GetSizeWhenOpened() != (iPeakSize * sizeof(tfloat)))
						bAnyMissing = true;
					pPeakTest->Close();
				}
				if (!pPeakTest->Open(sPeakFileR.c_str(), IFile::FileRead))
					bAnyMissing = true;
				else {
					if (pPeakTest->GetSizeWhenOpened() != (iPeakSize * sizeof(tfloat)))
						bAnyMissing = true;
					pPeakTest->Close();
				}
			}

			if (bAnyMissing) {
				tfloat32* pfPeakL = new tfloat[iPeakSize];
				memset(pfPeakL, 0, iPeakSize * sizeof(tfloat32));
				tfloat32* pfPeakR = new tfloat[iPeakSize];
				memset(pfPeakR, 0, iPeakSize * sizeof(tfloat32));

				tchar* pchL = new tchar[iSize * iByteWidthL];
				tchar* pchR = new tchar[iSize * iByteWidthR];
				tint32 iReadIndex = 0;
				tint32 iToGo = iLengthL;
				tint32 iPeakIndex = 0;
				while (iToGo) {
					tint32 iThisBuffer = iSize;
					if (iThisBuffer > iToGo) {
						iThisBuffer = iToGo;
					}

					pSrcFileL->Seek(iReadIndex * iByteWidthL + iOffsetL);
					pSrcFileL->Read(pchL, iThisBuffer * iByteWidthL);
					pSrcFileR->Seek(iReadIndex * iByteWidthR + iOffsetL);
					pSrcFileR->Read(pchR, iThisBuffer * iByteWidthR);

					tfloat32 fMax = 0.0f;
					tint32 i;
					tchar* pcWave = pchL;
					if (iBitWidthL == 24) {
						for (i = 0; i < iThisBuffer; i++) {
							
							tint32 i32 = *((tint32*)(pcWave));
							pcWave += 3;
							Inplace24to32(i32);
							tfloat32 f = (tfloat32)(i32 / 8388607.0);
							
							if (fabsf(f) > fMax) {
								fMax = fabsf(f);
							}
						}
					}
					else { // if (iBitWidthL == 16) {
						for (i = 0; i < iThisBuffer; i++) {
							
							tint16 i16 = CPSwap(*((tint16*)(pcWave)));
							pcWave += 2;
							tfloat32 f = (tfloat32)(i16 / 32767.0);

							if (fabsf(f) > fMax) {
								fMax = fabsf(f);
							}
						}
					}
					pfPeakL[iPeakIndex] = fMax;

					pcWave = pchR;
					if (iBitWidthR == 24) {
						for (i = 0; i < iThisBuffer; i++) {
							
							tint32 i32 = *((tint32*)(pcWave));
							pcWave += 3;
							Inplace24to32(i32);
							tfloat32 f = (tfloat32)(i32 / 8388607.0);
							
							if (fabsf(f) > fMax) {
								fMax = fabsf(f);
							}
						}
					}
					else { // if (iBitWidthR == 16) {
						for (i = 0; i < iThisBuffer; i++) {
							
							tint16 i16 = CPSwap(*((tint16*)(pcWave)));
							pcWave += 2;
							tfloat32 f = (tfloat32)(i16 / 32767.0);

							if (fabsf(f) > fMax) {
								fMax = fabsf(f);
							}
						}
					}
					pfPeakR[iPeakIndex] = fMax;

					iReadIndex += iThisBuffer;
					iToGo -= iThisBuffer;
					iPeakIndex++;
				}

				delete[] pchL;
				delete[] pchR;

				pPeakFileL->Open(sPeakFileL.c_str(), IFile::FileCreate);
				pPeakFileL->Write((const tchar*)pfPeakL, iPeakSize * sizeof(tfloat32));
				pPeakFileR->Open(sPeakFileR.c_str(), IFile::FileCreate);
				pPeakFileR->Write((const tchar*)pfPeakR, iPeakSize * sizeof(tfloat32));

				delete[] pfPeakL;
				delete[] pfPeakR;
			}
		}
	}
} // VerifyCreatePeakFiles

void CKSApplication::UpdateGUIFileList()
{
	// Send a message to all panes
	CBasePane::SMsg Msg(Msg_Update_File_List, &mSample_Data_List);
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		(*it)->GetPane()->SendMsg(&Msg);
	}
} // UpdateGUIFileList

void CKSApplication::PrepareMovePlayhead()
{
	if(mePlaybackState == geStatePlaying){
		mbWasPlayingWhenMovePlayheadWasStarted = true;
		PlaybackStop();
		//printf("Stop \n");
	}
	else{
		mbWasPlayingWhenMovePlayheadWasStarted = false;
	}
}

void CKSApplication::MovePlayhead(tuint64 uiSamplePos)
{
	muiMoveToPos = uiSamplePos;
	miSongPos	= uiSamplePos;
}

void CKSApplication::EndMovePlayhead()
{
	PlaybackGoToPos(muiMoveToPos);
	
	if(mbWasPlayingWhenMovePlayheadWasStarted){
		PlaybackStart();
		//printf("start \n");
	}
}

void CKSApplication::PlaybackStart()
{
	CAutoLock Lock(mMutex);
	CAutoLock Lock2(mMutex_Progress);

	// Only allowed when not doing something in progress
	if (mePlaybackState == geStateStopped) {
		if (mbRecord) {
			mePlaybackState = geStateRecording;
		}
		else {
			mePlaybackState = geStatePlaying;
		}

		mpDSPEngine->Start();
	}
} // PlaybackStart

void CKSApplication::PlaybackStop()
{
	CAutoLock Lock(mMutex);
	CAutoLock Lock2(mMutex_Progress);

	PlaybackStop_NoLock();
} // PlaybackStop

void CKSApplication::PlaybackStop_NoLock()
{
	if ((IsInProgressTaskState()) && (!mpProgressTasks->IsDone())) {
		// We are doing something in progress - queue a "stop that" signal
		mpProgressTasks->DoAbort();
	}
	else {
		// Immediately stop whatever we're doing
		tbool bWasRecording = IsRecording();
		mpDSPEngine->Stop();
		if ((!bWasRecording) || (!IsInProgressTaskState())) {
			// It's OK to really stop now
			mePlaybackState = geStateStopped;
		}
	}
} // PlaybackStop_NoLock

void CKSApplication::PlaybackFF()
{
	CAutoLock Lock(mMutex);
	CAutoLock Lock2(mMutex_Progress);
	
	// We are doing something in progress - ignore command
	if (IsInProgressTaskState()) return;
	
	tbool bGrid_On = (GetGlobalParm(giParamID_Grid, giSectionGUI) != 0);
	// Snap to grid
	if(bGrid_On){
		miSongPos = SnapToGrid(miSongPos);
		miSongPos = SnapToGridEnd(miSongPos + 16);
	}
	else{
		
		miSongPos += Float2Int(GetSampleRate()/4.41f);
	}

	/*
	dynamic_cast<CDSP*>(GetDSPEngine())->SetSongPosition(miSongPos);
	*/
	gpDSPEngine->SetSongPosition_AndResetEffectsTails(miSongPos);
} // PlaybackFF

void CKSApplication::PlaybackRewind()
{
	CAutoLock Lock(mMutex);
	CAutoLock Lock2(mMutex_Progress);
	
	// We are doing something in progress - ignore command
	if (IsInProgressTaskState()) return;
	
	tbool bGrid_On = (GetGlobalParm(giParamID_Grid, giSectionGUI) != 0);
	// Snap to grid
	if(bGrid_On){
		
		miSongPos = SnapToGrid(miSongPos + 16);
		miSongPos = miSongPos > 16 ? SnapToGridStart(miSongPos - 16): 0;
	}
	else{
		
		tint32 iTempo = Float2Int(GetSampleRate()/4.41f);
		
		miSongPos >= iTempo ? miSongPos -= iTempo : 0;
	}
	



	/*
	dynamic_cast<CDSP*>(GetDSPEngine())->SetSongPosition(miSongPos);
	*/
	gpDSPEngine->SetSongPosition_AndResetEffectsTails(miSongPos);
} // PlaybackRewind

void CKSApplication::PlaybackGoToStart(tbool bIgnorePlayState /*= false*/)
{
	CAutoLock Lock(mMutex);
	CAutoLock Lock2(mMutex_Progress);

	if ((!bIgnorePlayState) && (IsInProgressTaskState())) {
		// We are doing something in progress - ignore command
		return;
	}

	miSongPos = 0;

	/*
	dynamic_cast<CDSP*>(GetDSPEngine())->SetSongPosition(miSongPos);
	*/
	gpDSPEngine->SetSongPosition_AndResetEffectsTails(miSongPos);
} // PlaybackGoToStart

void CKSApplication::PlaybackGoToEnd()
{
	CAutoLock Lock(mMutex);
	CAutoLock Lock2(mMutex_Progress);

	if (IsInProgressTaskState()) {
		// We are doing something in progress - ignore command
		return;
	}

	miSongPos = dynamic_cast<CDSP*>(GetDSPEngine())->GetFinalSoundPos();

	/*
	dynamic_cast<CDSP*>(GetDSPEngine())->SetSongPosition(miSongPos);
	*/
	if (mePlaybackState != geStateStopped)
		gpDSPEngine->SetSongPosition(miSongPos);
	else
		gpDSPEngine->SetSongPosition_AndResetEffectsTails(miSongPos);
} // PlaybackGoToEnd

void CKSApplication::PlaybackGoToPos(tint64 iSample)
{
	CAutoLock Lock(mMutex);
	CAutoLock Lock2(mMutex_Progress);

	if (IsInProgressTaskState()) {
		// We are doing something in progress - ignore command
		return;
	}
	
	tbool bGrid_On = (GetGlobalParm(giParamID_Grid, giSectionGUI) != 0);
	// Snap to grid
	miSongPos = bGrid_On ? SnapToGrid(iSample): iSample;

	if (mePlaybackState != geStateStopped)
		gpDSPEngine->SetSongPosition(miSongPos);
	else
		gpDSPEngine->SetSongPosition_AndResetEffectsTails(miSongPos);
} // PlaybackGoToPos

void CKSApplication::Playback_InProgressTask(tint64 iNewPos /*= -1*/)
{
	StartProgressTasks_NoLock(geStateInProgressTask, iNewPos);
	gpDSPEngine->LimitDataFromStreams(false, 0, -1);
} // Playback_InProgressTask

void CKSApplication::Playback_ExportTrack(tint32 iTrack, tint64 iFirstSample, tint64 iFinalSample)
{
	StartProgressTasks_NoLock(geStateExportingTrack, iFirstSample, iTrack);
	if ((iFirstSample > 0) || (iFinalSample >= 0)) {
		gpDSPEngine->LimitDataFromStreams(true, iFirstSample, iFinalSample);
	}
} // Playback_ExportTrack

void CKSApplication::Playback_ExportOutMix(tint64 iPosToStart)
{
	StartProgressTasks_NoLock(geStateExportingOutMix, iPosToStart);
	gpDSPEngine->LimitDataFromStreams(false, 0, -1);
} // Playback_ExportOutMix

void CKSApplication::Zoom(tint32 iCmd, tint32 iVal)
{
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CKSBaseGUI*>(pGUI)->Zoom(iCmd, iVal);
	}

} // Zoom

tbool CKSApplication::OnKeyDown(ge::EKey Key)
{
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		CKSBaseGUI* pKSGUI = dynamic_cast<CKSBaseGUI*>(pGUI);
		if (IsInProgressTaskState()) {
			// We're doing something in progress bar
			if (Key != ge::KeySpace) {
				// The only key we allow is space (to abort operation)
				// This isn't it - eat it:
				return true;
			}
		}
		if (pKSGUI->DoKeyDown(Key)) {
			return true;
		}
	}
	return false;
} // OnKeyDown

void CKSApplication::SelectTrack(tint32 iID)
{
	miSelected_Track = iID;
	STrackData sData;
	sData.iTrack		= iID;
	CBasePane::SMsg Msg(Msg_Select_Track, &sData);
	Send_Msg_To_All_Panes(&Msg);
} // SelectTrack

tint32 CKSApplication::Get_Number_Of_Tracks()
{
	tint32 iTracks_Usede	=	0;
	
	for(tint32  i = 0; i < giNumber_Of_Tracks; i++)
	{
		if(GetGlobalParm(giParam_Ch_In_Use, i+giSection_First_Track))
			iTracks_Usede++;
	}
	return iTracks_Usede;
}

tint32 CKSApplication::Get_Free_Track()
{
	// Find a free track
	for( tint32 i = 0; i < giNumber_Of_Tracks; i++){
		if(GetGlobalParm(giParam_Ch_In_Use, giSection_First_Track + i) ==  false){
			return i;
		}
	}
	return -1;
}

tint32 CKSApplication::AddTrack()
{	
	// Find a free track to add
	tint32 iTrack_To_Add	=	Get_Free_Track();
	
	if(iTrack_To_Add == -1) return -1;

	// Number of tracks
	tint32 iNr_Tracks = Get_Number_Of_Tracks();
	
	// Find position for track to add
	tint32 iTrack_Position;
	
	
	// Find posittion for track to add
	if(miSelected_Track == -1)
		iTrack_Position		= iNr_Tracks;
	else 
		iTrack_Position		= GetGlobalParm(giParam_Ch_GUI_Position,  giSection_First_Track + miSelected_Track)+1;

	tint32 iIndex			=	iNr_Tracks+1;
		
	while(iIndex--){
		if(iIndex == iTrack_Position) break;
		// Move position of tracks above one posittion up
		msStack.iTrack[iIndex]	=	msStack.iTrack[iIndex-1];
		tint32 iTrack				=	msStack.iTrack[iIndex];
		tint32 iPos					=	GetGlobalParm(giParam_Ch_GUI_Position,  giSection_First_Track + iTrack);
		// Update track position
		SetGlobalParm(giParam_Ch_GUI_Position, iPos+1, giSection_First_Track + iTrack);
	}
		
	// Set position of iTrack_To_Add to add
	SetGlobalParm(giParam_Ch_GUI_Position, iTrack_Position, giSection_First_Track + iTrack_To_Add);
	SetGlobalParm(giParam_Ch_In_Use, true, giSection_First_Track + iTrack_To_Add);
	SetGlobalParm(giParam_Track_Info_SizeY, 0, giSection_First_Track + iTrack_To_Add);
	
	
	// Update Stack
	msStack.iNr_Of_Tracks			= iNr_Tracks+1;
	msStack.iTrack[iTrack_Position]	= iTrack_To_Add;
	

	char psz[32];
	sprintf(psz, "Track %d", iTrack_To_Add + 1);
	SetChannelName(iTrack_To_Add, psz);

	Stack_Tracks();
	
	return iTrack_To_Add;
}

void CKSApplication::DeleteTrack()
{
	tint32 iTrack_To_Delete;
	
	// Number of tracks
	tint32 iNr_Tracks = Get_Number_Of_Tracks()-1;
	
	// Find posittion for track to delete
	if(miSelected_Track == -1){
		// Last track
		iTrack_To_Delete	=	msStack.iTrack[iNr_Tracks];
	}
	else{
		// Selected track
		iTrack_To_Delete	=	miSelected_Track;
	}
	
	// Remove wave regions for track
	DeleteAllRegionsForTrack(iTrack_To_Delete);
	gpDSPEngine->DeleteAllRegionsForTrack(iTrack_To_Delete);

	// Possition on the GUI
	tint32 iPos	=	Find_Track_Pos(iTrack_To_Delete);
	
	if(iPos == -1) return;
	
	// Move position of all tracks above iPos one posittion down
	for(tint32 iIndex= iPos; iIndex < iNr_Tracks; iIndex++){
		msStack.iTrack[iIndex]	=	msStack.iTrack[iIndex+1];
		
		tint32 iTrack				=	msStack.iTrack[iIndex];
		tint32 iPos					=	GetGlobalParm(giParam_Ch_GUI_Position,  giSection_First_Track + iTrack);
		// Update track position
		SetGlobalParm(giParam_Ch_GUI_Position, iPos-1, giSection_First_Track + iTrack);
		
	
	}

	// Update preset data
	SetGlobalParm(giParam_Ch_In_Use, false, giSection_First_Track + iTrack_To_Delete);

	//Deselect all tracks
	SelectTrack(-1);

	// Update Stack
	msStack.iNr_Of_Tracks		= iNr_Tracks;
	Stack_Tracks();
	
}

tint32 CKSApplication::Find_Track_Pos(tint32 iTrack)
{
	for(tint32 iPos=0; iPos< giNumber_Of_Tracks; iPos++){
		if(	msStack.iTrack[iPos]==iTrack)
			return iPos;
	}
	return -1;
}

void CKSApplication::Set_All_Meters(void* pvoidMeters_All)
{
	// Don't do this during create
	if (!mbGUI_Created)
		return;
	if(!mbAll_Plugins_Created)
		return;

	// Set meters to 1.0 during load
	if (!mbAreGUIsReady)
		return;//pvoidMeters_All = NULL;

	if (1) {
		CAutoLock Lock(mMutexMeter);

		CBasePane::SMsg Msg(Msg_Set_All_Meters, pvoidMeters_All);
		Send_Msg_To_All_Panes(&Msg);
		
	}
}

void CKSApplication::Stack_Tracks()
{
	// We mustn't stack track repeatedly during create / load
	if (!mbAreGUIsReady)
		return;

	CBasePane::SMsg Msg;
	Msg = Msg_Stack_Tracks;
	Msg.pDataIn	= (void*)&msStack;
	Send_Msg_To_All_Panes(&Msg);
}

void CKSApplication::Send_Msg_To_All_Panes(CBasePane::SMsg* pMsg)
{
	// Send a message to all panes
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		(*it)->GetPane()->SendMsgFromTop(pMsg);
	}
	
}

void CKSApplication::Update_Zoom()
{
	SGUIData sData;
	sData.iID	= giParamID_Zoom;
	// Lasse, bug-fix 2008-08-25 - added this forgotten part
	sData.iVal = GetGlobalParm(giParamID_Zoom, giSectionGUI);
	// .. Lasse

	CBasePane::SMsg Msg;
	Msg = MsgGUIData;
	Msg.pDataIn	= (void*)&sData;
	Send_Msg_To_All_Panes(&Msg);
	
	// deselect all all selections
	Msg = Msg_Deselect_All;
	Msg.pDataIn	= (void*)&sData;
	Send_Msg_To_All_Panes(&Msg);
	
	gpDSPEngine->Deselect_All_Tracks();


}


void CKSApplication::DeleteAllRegionsForTrack(tint32 iTrack)
{
	STrackData sData;
	sData.iTrack = iTrack;
	CBasePane::SMsg Msg = Msg_Delete_All_Regions;
	Msg.pDataIn = (void*)&sData;
	
	// Send a message to all panes
	Send_Msg_To_All_Panes(&Msg);
}


void CKSApplication::Set_Track_Visible(tuint iID, tbool bVisible)
{
/*
	STrackData sData;
	sData.iVal	= bVisible;

	CBasePane::SMsg Msg;
	Msg = Msg_Set_Track_Visible;
	Msg.pDataIn	= (void*)&sData;

	// Send a message to all panes
	Send_Msg_To_All_Panes(&Msg);
*/
}


void CKSApplication::Set_Track_To_Default(tuint iID)
{

}
	
void CKSApplication::SetChannelName(tint32 iChannel, const std::string& sName)
{
	mpsChannelName[iChannel] = sName;
	
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CKSBaseGUI*>(pGUI)->UpdateChannelName(iChannel, sName);
	}
}









std::string CKSApplication::GetProjDir_Audio() const
{
	return GetProjDir_Contents() + "Audio:";
}

std::string CKSApplication::GetProjDir_Clips() const
{
	return GetProjDir_Audio() + "Clips:";
}

std::string CKSApplication::GetProjDir_ClipsDecomp() const
{
	return GetProjDir_Clips() + "Decompressed:";
}

std::string CKSApplication::GetFromWaveName_ClipWave(const tchar* pszWaveName) const
{
	return GetProjDir_Clips() + std::string(pszWaveName) + ".wav";
}

std::string CKSApplication::GetFromWaveName_ClipWaveOld(const tchar* pszWaveName) const
{
	return GetProjDir_Audio() + std::string(pszWaveName) + ".wav";
}

std::string CKSApplication::GetFromWaveName_ClipDefaultOgg(const tchar* pszWaveName) const
{
	return GetProjDir_Clips() + std::string(pszWaveName) + ".ogg";
}

std::string CKSApplication::GetFromWaveName_ClipWaveDecomp(const tchar* pszWaveName) const
{
	return GetProjDir_ClipsDecomp() + std::string(pszWaveName) + ".wav";
}









/*
std::string CKSApplication::GetFromWaveName_ClipWave_Safe(const tchar* pszWaveName)
{
	std::string sWavePath = GetFromWaveName_ClipWave(pszWaveName);
	if (!CanWaveFilePlay(sWavePath, true, false)) {
		sWavePath = GetFromWaveName_ClipWaveOld(pszWaveName);
	}
	if (!CanWaveFilePlay(sWavePath, true, false)) {
		sWavePath = GetFromWaveName_ClipWaveDecomp(pszWaveName);
	}
	if (!CanWaveFilePlay(sWavePath, true, false)) {
		sWavePath = "";
	}
	return sWavePath;
} // GetFromWaveName_ClipWave_Safe
 */

/*
std::string CKSApplication::GetFromWaveName_ClipComp_Safe(const tchar* pszWaveName) const
{
	CSample_Data* pSample_Data	=	gpDSPEngine->Get_Sample_Data_From_Name(pszWaveName);
	CTake_Data* pTake_Info		=	pSample_Data->Get_Take_Data();
	
	if ((pTake_Info == NULL) || (!pTake_Info->bIsOriginalLossy))
		return "";

	if (pTake_Info->bIsOriginalLossy) {
		std::string sCompressedOriginalPathName =
			GetProjDir_Clips() + pTake_Info->sOriginalName + pTake_Info->sOriginalExt;
		if (IFile::Exists(sCompressedOriginalPathName.c_str()))
			return sCompressedOriginalPathName;
	}

	std::string sDefaultOgg = GetFromWaveName_ClipDefaultOgg(pszWaveName);
	if (IFile::Exists(sDefaultOgg.c_str()))
		return sDefaultOgg;

	return "";
} // GetFromWaveName_ClipComp_Safe

*/

/*
CSample_Data* CKSApplication::Get_Sample_Data_From_Name(const tchar* pszListName) const
{
	std::list<CSample_Data*>::const_iterator it = mSample_Data_List.begin();
	for ( ; it != mSample_Data_List.end(); it++) {
		CSample_Data* pSample_Data = *it;
		if (stricmp(pSample_Data->sName.c_str(), pszListName) == 0)
			return pSample_Data;
	}
	return NULL;
} // GetFromListName_ClipEntry
*/
/*
void CKSApplication::Set_Wave_Path( CSample_Data* pSample_Data, const tchar* pszListName, std::string& rsWavePathNameL, std::string& rsWavePathNameR, tbool* pbIsDecompressed ) const
{
	CTake_Data* pTake_Info		=	pSample_Data->Get_Take_Data();
	if (pbIsDecompressed)
		*pbIsDecompressed = pTake_Info->bIsOriginalLossy;
	if (pTake_Info->bIsStereoInList) {
		// STEREO
		// Get two streams for stereo channel
		rsWavePathNameL = pTake_Info->sWavePathNameL;
		rsWavePathNameR = pTake_Info->sWavePathNameR;
	}
	else {
		// MONO
		// Get a single stream for mono channel
		if (pTake_Info->sWavePathNameL.length()) {
			// Using left (or only) side of original for mono channel
			rsWavePathNameL = pTake_Info->sWavePathNameL;
			rsWavePathNameR = "";
		}
		else if (pTake_Info->sWavePathNameR.length()) {
			// Using right side of original for mono channel
			// Note! It *IS* correct to output into *left* side always!
			rsWavePathNameL = pTake_Info->sWavePathNameR;
			rsWavePathNameR = "";
		}
	}	
}
 */
/*
tuint64 CKSApplication::Get_Sample_Duration_From_Name( const tchar* pszListName)
{
	
	CSample_Data* pSample_Data = Get_Sample_Data_From_Name(pszListName);  
	
	CTake_Data* pTake_Info		=	pSample_Data->Get_Take_Data();
	

	return 0;
	
}
*/
tint32 CKSApplication::GetFromListName_ClipWavePathNames(const tchar* pszListName, std::string& rsWavePathNameL, std::string& rsWavePathNameR, tbool* pbIsDecompressed /*= NULL*/) const
{
	// obsoleet will be replased with "Set_Wave_Path()"
	/*
	CSample_Data* pSample_Data = gpDSPEngine->Get_Sample_Data_From_Name(pszListName);
	
	
	if (pSample_Data) {
		
		CTake_Data* pTake_Info		=	pSample_Data->Get_Take_Data();
		
		if (pbIsDecompressed)
			*pbIsDecompressed = pTake_Info->bIsOriginalLossy;
		if (pTake_Info->bIsStereoInList) {
			// STEREO
			// Get two streams for stereo channel
			rsWavePathNameL = pTake_Info->sWavePathNameL;
			rsWavePathNameR = pTake_Info->sWavePathNameR;
			return 2;
		}
		else {
			// MONO
			// Get a single stream for mono channel
			if (pTake_Info->sWavePathNameL.length()) {
				// Using left (or only) side of original for mono channel
				rsWavePathNameL = pTake_Info->sWavePathNameL;
				rsWavePathNameR = "";
			}
			else if (pTake_Info->sWavePathNameR.length()) {
				// Using right side of original for mono channel
				// Note! It *IS* correct to output into *left* side always!
				rsWavePathNameL = pTake_Info->sWavePathNameR;
				rsWavePathNameR = "";
			}
			else {
				// No wave files - what's going on?
				rsWavePathNameL = "";
				rsWavePathNameR = "";
				return 0;
			}
			// 1 = using left side only for mono
			return 1;
		}
	}
*/
	// Not found
	rsWavePathNameL = "";
	rsWavePathNameR = "";
	return 0;
} // GetFromListName_ClipWavePathNames

void CKSApplication::ClearAllMeters()
{
	CAutoLock Lock(mMutexMeter);

	dynamic_cast<CDSP*>(mpDSPEngine)->ClearAllMeters();
}

void CKSApplication::GetAllMeters_MaybeClear(void* pvoid_sMeters_All, tuint32 muiTimeMS_Elapsed, tbool bClear)
{
	CAutoLock Lock(mMutexMeter);

	CDSP::SMeters_All* psMeters_All = (CDSP::SMeters_All*)pvoid_sMeters_All;
	dynamic_cast<CDSP*>(mpDSPEngine)->GetAllMeters_MaybeClear(psMeters_All, bClear);

	// Precalculate the LEDs decay value
	const tfloat32 fDecayTimeMS = 4500.0f;//1000.0f;
	// Times we're updating value within full decay time
	tfloat32 fTimes = fDecayTimeMS / muiTimeMS_Elapsed;
	// Our decay value (for -60dB)
	tfloat32 fDecay = (float)exp(-6.9077552789821370520539743640531 / fTimes);
	if (fDecay <= 0 ) {
		fDecay = 0;
	}
	// Deliver decay factor for GUI controls to use it
	psMeters_All->fDecayFactor = fDecay;
}

void CKSApplication::WindowClosed(void* pWnd)
{
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		if (dynamic_cast<CKSBaseGUI*>(pGUI)->WindowClosed(pWnd)) {
			return;
		}
	}
}

void CKSApplication::CloseWindow(void* pWnd)
{
	gpMainApplication->CloseWindow(pWnd);
}


tuint64 CKSApplication::SnapToGrid(tuint64 uiSamplePos)
{
	tbool bGrid_On = (GetGlobalParm(giParamID_Grid, giSectionGUI) != 0);
	// Snap to grid
	if (bGrid_On) {
		tfloat64 fSnap = GetGlobalParm(giParamID_KS_Snap_To, giSectionGlobal) * GetSamples_Pr_32();
		/*
		// avoid to trunchate line nr
		uiSamplePos += Float2Int(fSnap/2.0f);
		// Find gridline in samples
		tuint32 uiGridLineNr	=	Float2Int((tfloat64)uiSamplePos / fSnap);
		// Set sample pos
		uiSamplePos				=  Float2Int(uiGridLineNr * fSnap);
		 */
		// avoid to trunchate line nr
		uiSamplePos += (tint64)(fSnap/2.0f);
		// Find gridline in samples
		tuint32 uiGridLineNr	=	(tuint32)((tfloat64)uiSamplePos / fSnap);
		// Set sample pos
		uiSamplePos				=  (tint64)(uiGridLineNr * fSnap);

	}
	return uiSamplePos;
}

tuint64 CKSApplication::SnapToGridStart(tuint64 uiSamplePos)
{
	tbool bGrid_On = (GetGlobalParm(giParamID_Grid, giSectionGUI) != 0);

	
	// Snap to grid
	if (bGrid_On) {
		tfloat64 fSnap = GetGlobalParm(giParamID_KS_Snap_To, giSectionGlobal) * GetSamples_Pr_32();
		// Find gridline in samples
		tuint32 uiGridLineNr	=	(tuint32)((tfloat64)uiSamplePos / fSnap);
		// Set sample pos
		uiSamplePos				=  (tint64)(uiGridLineNr * fSnap);

		
	}
	return uiSamplePos;
}

tuint64 CKSApplication::SnapToGridEnd(tuint64 uiSamplePos)
{
	
	
	// if snap to grid is on
	if (GetGlobalParm(giParamID_Grid, giSectionGUI)) {
		
		// Find previous gridline
		uiSamplePos = SnapToGridStart(uiSamplePos);
		
		tfloat64 fSnap = GetGlobalParm(giParamID_KS_Snap_To, giSectionGlobal) * GetSamples_Pr_32();
		
		// snap to next gridline
		uiSamplePos += (tint64)(fSnap)-1;
		
		/*
		// Find gridline in samples
		tuint32 uiGridLineNr	=	(tuint32)((tfloat64)uiSamplePos / fSnap);
		
		// Set sample pos
		uiSamplePos				=  (tint64)(uiGridLineNr * fSnap);
		 */

	}
	return uiSamplePos;
}


tfloat64 CKSApplication::GetSamples_Pr_32()
{
	tfloat64 fSamplesPrBeat		=	( 240.0f / mfTempo) * GetSampleRate();
	tfloat64 fSamples_Pr_32		=	(fSamplesPrBeat / 32.0f);
	
	return fSamples_Pr_32;
}

void CKSApplication::Set_Progress(tbool bOn, tfloat32 fProgress, const tchar* pszText /*= NULL*/)
{
	SProgressData sData;
	sData.fProgress		= fProgress;
	sData.bOn			= bOn;
	sData.pszText		= (tchar*)pszText;
	
//	sData.iVal		= iValue;
	CBasePane::SMsg Msg;
	Msg = Msg_Progress;
	Msg.pDataIn	= (void*)&sData;
	Send_Msg_To_All_Panes(&Msg);
} // Set_Progress

void CKSApplication::ShowProgress(tuint64 uiIx, tuint64 uiMax, const tchar* pszText, ge::IContext* pContext, tbool* pbAbort)
{
	tbool bOn = (uiMax != 0);
	tfloat32 f = bOn ? ((tfloat32)uiIx) / uiMax : 0.0f;
	Set_Progress(bOn, f, pszText);
	if ((bOn) && (pContext)) {
		tbool bAbort = false;
		pContext->LimitedHandleEventQueue(false, &bAbort);
		if (bAbort)
			Set_Progress(false, 0.0f, NULL);
		if (pbAbort)
			*pbAbort = bAbort;
	}
} // DoProgress

void CKSApplication::DoProcess(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	CBasePlugIn::DoProcess(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	mpPreviewCallback->Accumulate(ppfSamplesOut, iNrOfSamples);
}

tint32 CKSApplication::GetNrOfInputChannels()
{
	return dynamic_cast<CDSP*>(GetDSPEngine())->GetNrOfInputChannels();
}

void CKSApplication::UpdateAudioGUI()
{
	CBasePane::SMsg Msg(MsgUpdateAudioGUI);

	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		(*it)->GetPane()->SendMsg(&Msg);
	}
}


tint64 CKSApplication::AudioInput_IntermediateBuffer_CalcExtraLatency()
{
	CAutoLock Lock(mMutex);

	tint64 iRc = 0;
	std::list<tint32>::iterator it = mlistAudioInput_IntermediateBuffer_Samples.begin();
	for ( ; it != mlistAudioInput_IntermediateBuffer_Samples.end(); it++) {
		tint32 iSamples = *it;
		iRc += iSamples;
	}
	iRc -= miAudioInput_IntermediateBuffer_FirstLinkIx;

	return iRc;
} // AudioInput_IntermediateBuffer_CalcExtraLatency


void CKSApplication::AudioInput_IntermediateBuffer_Kill()
{
	while (mlistAudioInput_IntermediateBuffer.size() > 0) {
		// Pop one ..
		tfloat** ppf = *(mlistAudioInput_IntermediateBuffer.begin());
		//tint32 iSamples;
		mlistAudioInput_IntermediateBuffer.pop_front();
		mlistAudioInput_IntermediateBuffer_Samples.pop_front();

		// .. and throw it away
		delete[] ppf[0];
		delete[] ppf[1];
		delete[] ppf;
	}
	miAudioInput_IntermediateBuffer_FirstLinkSamples = 0;
	miAudioInput_IntermediateBuffer_FirstLinkIx = 0;
	mbAudioInput_IntermediateBuffer_EverFull = false;
	miAudioInput_IntermediateBuffer_FullChainCount = kiAudioInput_IntermediateBuffer_DefaultFullChainCount;
} // AudioInput_IntermediateBuffer_Kill


void CKSApplication::AudioInput_IntermediateBuffer_Push(const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	if (iNrOfSamples <= 0)
		return;

	tfloat** ppfDst = new tfloat*[2];
	ppfDst[0] = new tfloat[iNrOfSamples];
	ppfDst[1] = new tfloat[iNrOfSamples];
	tint32 iBytes = iNrOfSamples * sizeof(tfloat);
	memcpy(ppfDst[0], ppfSamplesIn[0], iBytes);
	memcpy(ppfDst[1], ppfSamplesIn[1], iBytes);
	mlistAudioInput_IntermediateBuffer.push_back(ppfDst);
	mlistAudioInput_IntermediateBuffer_Samples.push_back(iNrOfSamples);

	tint32 iChainLinks = mlistAudioInput_IntermediateBuffer.size();

	if (iChainLinks == 1) {
		// Doublicate info about first chain link
		miAudioInput_IntermediateBuffer_FirstLinkIx = 0;
		miAudioInput_IntermediateBuffer_FirstLinkSamples = iNrOfSamples;
	}
	else if (iChainLinks >= miAudioInput_IntermediateBuffer_FullChainCount) {
		mbAudioInput_IntermediateBuffer_EverFull = true;

		// Make sure we don't get memory overflow
		tint32 iTooManyChainLinks = iChainLinks - (miAudioInput_IntermediateBuffer_FullChainCount * 2);
		if (iTooManyChainLinks > 0) {
			// More than twice as many links as we wanted in chain
			// Is input/output out of sync?
			// Be very brutal about it - throw away half of excess
			iTooManyChainLinks /= 2;
			while (iTooManyChainLinks-- > 0) {
				// Pop one ..
				tfloat** ppf = *(mlistAudioInput_IntermediateBuffer.begin());
				tint32 iSamples = *(mlistAudioInput_IntermediateBuffer_Samples.begin());
				mlistAudioInput_IntermediateBuffer.pop_front();
				mlistAudioInput_IntermediateBuffer_Samples.pop_front();

				// .. and throw it away
				delete[] ppf[0];
				delete[] ppf[1];
				delete[] ppf;
			}
		}
	}
} // AudioInput_IntermediateBuffer_Push


void CKSApplication::AudioInput_IntermediateBuffer_Pop(tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	if (iNrOfSamples <= 0)
		return;

	tfloat* pfDst0 = ppfSamplesIn[0];
	tfloat* pfDst1 = ppfSamplesIn[1];
	tbool bGotAnything = false;

	// Get data from buffer
	if (mbAudioInput_IntermediateBuffer_EverFull) {
		while ((iNrOfSamples > 0) && (mlistAudioInput_IntermediateBuffer.size() > 0)) {
			tint32 iSamplesInFirstLink = miAudioInput_IntermediateBuffer_FirstLinkSamples;
			iSamplesInFirstLink -= miAudioInput_IntermediateBuffer_FirstLinkIx;

			// Get samples from first link
			tfloat** ppfSrc = *(mlistAudioInput_IntermediateBuffer.begin());
			tfloat* pfSrc0 = ppfSrc[0];
			tfloat* pfSrc1 = ppfSrc[1];
			if (iSamplesInFirstLink > 0) {
				tint32 iToCopyNow = min(iNrOfSamples, (tuint32)iSamplesInFirstLink);
				tint32 iBytesToCopyNow = iToCopyNow * sizeof(tfloat);
				memcpy(pfDst0, pfSrc0 + miAudioInput_IntermediateBuffer_FirstLinkIx, iBytesToCopyNow);
				pfDst0 += iToCopyNow;
				memcpy(pfDst1, pfSrc1 + miAudioInput_IntermediateBuffer_FirstLinkIx, iBytesToCopyNow);
				pfDst1 += iToCopyNow;
				miAudioInput_IntermediateBuffer_FirstLinkIx += iToCopyNow;
				iNrOfSamples -= iToCopyNow;
				bGotAnything = true;
			}

			// Maybe move on to next link
			if (miAudioInput_IntermediateBuffer_FirstLinkIx >= miAudioInput_IntermediateBuffer_FirstLinkSamples) {
				// Throw first link away - it's used up
				delete[] pfSrc0;
				delete[] pfSrc1;
				delete[] ppfSrc;
				mlistAudioInput_IntermediateBuffer.pop_front();
				mlistAudioInput_IntermediateBuffer_Samples.pop_front();

				// Move on
				miAudioInput_IntermediateBuffer_FirstLinkIx = 0;
				if (mlistAudioInput_IntermediateBuffer.size() > 0) {
					miAudioInput_IntermediateBuffer_FirstLinkSamples = *(mlistAudioInput_IntermediateBuffer_Samples.begin());
				}
				else {
					miAudioInput_IntermediateBuffer_FirstLinkSamples = 0;
				}
			}
		}
	}

	// Get silence after real data
	tint32 iSilentBytes = iNrOfSamples * sizeof(tfloat);
	if (iSilentBytes > 0) {
		memset(pfDst0, '\0', iSilentBytes);
		memset(pfDst1, '\0', iSilentBytes);
	}

	if ((bGotAnything) && (miAudioInput_IntermediateBuffer_FirstLinkSamples == 0)) {
		// We just now reached end-of-buffer - force increase buffer size by 33%
		mbAudioInput_IntermediateBuffer_EverFull = false;
		miAudioInput_IntermediateBuffer_FullChainCount *= 4;
		miAudioInput_IntermediateBuffer_FullChainCount /= 3;
	}
} // AudioInput_IntermediateBuffer_Pop


void CKSApplication::ProcessNonInPlace_InputOnly(const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	CAutoLock Lock(mMutex);

	AudioInput_IntermediateBuffer_Push(ppfSamplesIn, iNrOfSamples);
} // ProcessNonInPlace_InputOnly


void CKSApplication::ProcessNonInPlace_OutputOnly(tfloat** ppfSamplesOut, tuint32 iNrOfSamples)
{
	CAutoLock Lock(mMutex);

	// Retrieve buffered input chunk
	tfloat** ppfIns = new tfloat*[2];
	ppfIns[0] = new tfloat[iNrOfSamples];
	ppfIns[1] = new tfloat[iNrOfSamples];
	AudioInput_IntermediateBuffer_Pop(ppfIns, iNrOfSamples);

	// Process output and buffered input together
	ProcessNonInPlace_NoLock(ppfSamplesOut, (const tfloat**)ppfIns, iNrOfSamples);

	// Kill intermediate buffers
	delete[] ppfIns[0];
	delete[] ppfIns[1];
	delete[] ppfIns;
} // ProcessNonInPlace_OutputOnly


void CKSApplication::ProcessNonInPlace(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	CAutoLock Lock(mMutex);

	ProcessNonInPlace_NoLock(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
} // ProcessNonInPlace

	
void CKSApplication::ProcessNonInPlace_NoLock(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	if (iNrOfSamples <= miMaxSubBufferSize) {
		// Not too many samples, we handle them all in one go
		DoProcess(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
	else {
		// Too many samples, we have to split them up into several calls
		
		// How many samples left to process?
		tuint32 uiSamplesLeft = iNrOfSamples;
		// How many samples has been processed
		tuint32 uiSamplesProcessed = 0;
		while (uiSamplesLeft) {
			// How many samples to process this iteration?
			tuint32 uiToProcess = uiSamplesLeft;
			if (uiToProcess > miMaxSubBufferSize) {
				uiToProcess = miMaxSubBufferSize;
			}
			
			// Setup audio
			tuint32 uiChannel = 0;
			tint32 iChannels = GetNrOfInputChannels();
			if (ppfSamplesIn != 0) {
				for (; uiChannel < (tuint)iChannels; uiChannel++) {
					const tfloat* p = ppfSamplesIn[uiChannel];
					p += uiSamplesProcessed;
				
					memcpy(mppfIn[uiChannel], p, uiToProcess * sizeof(tfloat));
				}
			}
			
			// Do it
			DoProcess((tfloat**)mppfOut, (const tfloat**) mppfIn, uiToProcess);
			
			// Copy output to out buffer
			for (uiChannel = 0; uiChannel < (tuint)miChannelsOut; uiChannel++) {
				tfloat* p = ppfSamplesOut[uiChannel];
				p += uiSamplesProcessed;
				
				memcpy(p, mppfOut[uiChannel], uiToProcess * sizeof(tfloat));
			}
			
			uiSamplesLeft -= uiToProcess;
			uiSamplesProcessed += uiToProcess;
		}
	}
} // ProcessNonInPlace_NoLock

void CKSApplication::Set_Project_License()
{
	SetGlobalParm(giParamID_Show_Export_For_Web_Window, true, giSectionGUI);
	
	
}

void CKSApplication::Zoom(){

	
	mbZoomFlipFlop = !mbZoomFlipFlop;
	
	if(mbZoomFlipFlop){
		miStoredZoom = GetGlobalParm(giParamID_Zoom, giSectionGUI);
		GetParmMan()->Set(true, giZoom_Levels - 10, giParamID_Zoom, de::IParameterManager::TypeGlobal, giSectionGUI);
	}
	else{
		GetParmMan()->Set(true, miStoredZoom, giParamID_Zoom, de::IParameterManager::TypeGlobal, giSectionGUI);
	}
	
}