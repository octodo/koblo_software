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
// along with the Koblo SDK. If not, see <http://www.gnu.org/licenses/>.






/*! \brief the track editor pane fill up the entire track editor view 
 *	holds four sub banes
 *	passes messages on to the sub panes
 */

// Forward declarations
class CTrack_Top;
class CTrack_Side;
class CTrack_Player;
class CTrack_File_Browser;
class CSoloMuteContainer;

class CDSP;

class CTrack_Editor_Pane : public virtual CBasePane
{
public:
	CTrack_Editor_Pane(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CTrack_Editor_Pane();

	virtual void Init();

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl* pControl, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl* pControl, void *pEventData);
	
	virtual void ConnectControls();
	//! update global data
	virtual void UpdateGlobalData( tint32 iType, tint32 iExtra, tint32  iID, tint32 iValue);
	
	virtual void UpdateTrackData(tint32 iID, tint32 iValue, tint32 iChannel);
	
	virtual void UpdateBussData(tint32 iID, tint32 iValue, tint32 iBuss);
	
	virtual void UpdateMasterData(tint32 iID, tint32 iValue);
	
	virtual void UpdateGUIData(tint32 iID, tint32 iValue);
	
	virtual void UpdateCommandData(tint32 iID, tint32 iValue);

	virtual void OnTimer(tint32 iTimerID);
	
	void UpdateChannelName(tint32 iChannel, const std::string& sName);

	//! CBasePane override
	virtual void Resize(ge::SSize SizeNew);
	
	//! CBasePane override - Lasse: what?
	virtual void AddTrack(tbool bRedraw = true);
	virtual void DeleteTrack(tbool bRedraw = true);
//	virtual void SelectTrack(tint32 iID);
	
	virtual void SendMsgFromTop(SMsg* pMsg);
	
	virtual tbool DoKeyDown(ge::EKey Key);
	
	virtual tbool DoKeyUp(ge::EKey Key);
	
	virtual void SetScrollPosHorizontal(tuint64 uiSample);
	
	
	

	

	

protected:
	

	tbool mbInited;

	CTrack_Top* mpTrack_Top;
	CTrack_Side* mpTrack_Side;
	CTrack_Player* mpTrack_Player;
	CTrack_File_Browser* mpTrack_File_Browser;
//	CTrack_System_Info* mpTrack_System_Info;

	ge::IScrollBar* mpScrollBarHorizontal;
	ge::IScrollBar* mpScrollBarVertical;
	
	
	//! Create vertical scrollbar for mpTrack_Player
	virtual void Create_Track_Player_ScrollBar_Vertical();
	//! Create horizontal scrollbar for mpTrack_Player
	virtual void Create_Track_Player_ScrollBar_Horizontal();
	// React on the delete key
	virtual void Do_Delete();
	

	
	/*! Update GUI: This is called when a track is added or removed from the project. all tracks are rebuild
	*/
//	virtual void UpdateGUI();
	/*! iSelected track starts from 0 and goes to giNumberOfChannels-1 
		"-1" is interpreted as "No track selected"
	*/
	tint32 miSelectedTrack;
	tint32 miMouseOverTrack;
	tint32 miNr_Of_Tracks;

	void* mpvoid_sMeters_All;
	tuint32 miuTimeMS_Prev;
	tbool mbFirstMeterSet;
	tbool mbIsVisible;

//	tint32 miMuteState[giNumberOfChannels];
//	tbool mbMute;

	CSoloMuteContainer* mpTrackSoloMuteContainer;
	CSoloMuteContainer* mpBussesSoloMuteContainer;
	
	ge::IMouseTrap* mpMouseTrap;
	
	virtual void SetScrollPosHorizontalToStart();
	virtual void SetScrollPosToPlayHead();
	
	virtual void SetScrollPosToLoopStart();
	//tbool mbPlaying;
	tint32 miZoom;
	
	tfloat32 mfGoToPos;
	
	tbool mbLoopOn;
	tuint64 muiLoopMidt;
	
	
	
	tbool mbDisplayStutterWarning_Armed;
	
	

};

