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


class CTrack_Info;

class CTrack_Setup : public virtual CBasePane
{
public:
	CTrack_Setup(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CTrack_Setup();

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

	virtual void HandleMsg(SMsg* pMsg);

	//! Returns the scroll pane
	ge::IScrollPane* GetScrollPane();
	
	virtual void Set_Track_Info_Visible(tint32 iTrackID, tbool bVisible);

	virtual void UpdateTrackMeters_Maybe(tint32 iTrack, void* mpvoid_sMeters_All);
	
	virtual void SelectTrack(tint32 iID);
	
	virtual void Set_Track_Size(tint32 iTrack, tint32 iSize);
	
	virtual void UpdateChannelName(tint32 iTrack, const std::string& sName);
	
protected:

/*
struct STrack_Side_GUI
{
	CTrack_Info* pGUI;
	tbool	bIn_Use;
	tint32  iTrack_Id;
};
*/

	CTrack_Info*	mppTrack_Info[giNumber_Of_Tracks];
	virtual void Stack_Tracks(STrack_Stack sData);
	
	ge::IBitmap* mpBack_Track_Shaddow_Bmp;




	// Lasse, added 2008-04-16
	tint32 miSelectedTrack;
	tint32 miMouseOverTrack;
	// .. Lasse
	
	// Lasse, 2008-04-17
	ge::IScrollPane* mpScrollPane;
	// .. Lasse
};
