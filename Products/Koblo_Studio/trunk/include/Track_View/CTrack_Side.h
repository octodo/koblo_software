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


class CTrack_Setup;
//class CTrack_Setup_ScrollPane;

class CTrack_Side : public virtual CBasePane
{
public:
	CTrack_Side(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CTrack_Side();

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
	
	virtual void Set_Track_Info_Visible(tint32 i,tbool bTrackVisible);

	virtual void Update_Tracks_Meters(void* mpvoid_sMeters_All);
	
	virtual void SelectTrack(tint32 iID);
	
	virtual void Set_Track_Size(tint32 iTrack, tint32 iSize);
	
	virtual ge::IScrollPane* GetSetupScrollPane();
	
	virtual void UpdateChannelName(tint32 iChannel, const std::string& sName);
	
	
protected:


	
	CTrack_Setup* mpTrack_Setup;
//	ge::IPopupMenu::SMenuItemList mpTime_Signature_Menu;
	
//	virtual void PreparePopups();
//	ge::IDropDownListBox* CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos);

	
};
