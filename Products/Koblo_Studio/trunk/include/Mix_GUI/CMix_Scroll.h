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

class CMix_Channel;


 
class CMix_Scroll : public virtual CBasePane
{
public:
	CMix_Scroll(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CMix_Scroll();

	virtual void Init();

	//! SetInfo
	/*!
		\param iCtrlIDOfs [in]: Control ID - used as offset for controls
		\param iBackResID [in]: Background bitmap
	*/
	virtual void SetInfo();

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl* pControl, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl* pControl, void* pEventData);

	//! Connects all relevant GUI controls of the slider pane to ParamIDs
	virtual void ConnectControls();
	//! Events
	virtual void HandleMsg(SMsg* pMsg);
	//! Returns the scroll pane
	ge::IScrollPane* GetScrollPane(){return mpScrollPane;};
	
	//!Show a track on the GUI
	/*!
		\param iTrackID [in]: Unique ID for the track;
	*/
//	virtual void SetTrackVisible(tint32 iTrackID, tbool bVisible);
	
	virtual void AddTrack();
	virtual void DeleteTrack();
	virtual void SelectTrack(tint32 iID);

	virtual void Update_Tracks_Meters(void* pvoid_sMeters_All);
	
	virtual void Stack_Tracks(STrack_Stack sData);
	
	virtual void UpdateChannelName(tint32 iTrack, const std::string& sName);

protected:

	//CTrack* mpTrack[giNumber_Of_Tracks];
	CMix_Channel* mpMix_Channel[giNumber_Of_Tracks];
	//CMix_Buss* mpMix_Buss[giNumber_Of_Busses];
	//CMix_Master* mpMix_Master;
//	tint32 miNr_Of_Tracks;
	tint32 miSelectedTrack;
	
	ge::IScrollPane* mpScrollPane;
//	ge::IPopupMenu::SMenuItemList mpMix_Master_Out_Menu;
	
	virtual void PreparePopups();
	
	virtual void UpdateGUI();
	


		


};
