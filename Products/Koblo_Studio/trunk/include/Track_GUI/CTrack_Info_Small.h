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


class CTrack_Info_Small : public virtual CBasePane
{
public:
	CTrack_Info_Small(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CTrack_Info_Small();

	virtual void Init();

	//! SetInfo
	/*!
		\param iCtrlIDOfs [in]: Control ID - used as offset for controls
		\param iBackResID [in]: iSize
	*/
	virtual void SetInfo(tint32 iID );

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl* pControl, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl* pControl, void* pEventData);

	//! Connects all relevant GUI controls of the slider pane to ParamIDs
	virtual void ConnectControls();

	virtual void HandleMsg(SMsg* pMsg);
	
	virtual void SendMsg(tint32 iMsg);
	
	virtual void SelectTrack( tbool bSelected);
	
	virtual void UpdateTrackMeters_Maybe(void* mpvoid_sMeters_All);
	
	virtual void UpdateChannelName(const std::string& sName);
	
protected:

	
	ge::IBitmap* mpSmall_Back_Drop;
	ge::IBitmap* mpSmall_Selected_Back_Drop;
	
	tint32 miID;
	tint32 miCtrl_Offset;
	tint32 miSection;
	
	ge::IPeakMeter* mpMeterOutL;
	ge::IPeakMeter* mpMeterOutR;

	ge::IMouseTrap* mpMouseTrap;
	

};
