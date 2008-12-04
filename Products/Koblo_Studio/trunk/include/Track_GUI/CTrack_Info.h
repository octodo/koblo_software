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

class CTrack_Info_Small;
class CTrack_Info_Big;

class CTrack_Info : public virtual CBasePane
{
public:
	CTrack_Info(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CTrack_Info();

	virtual void Init();

	//! SetInfo
	/*!
		\param iCtrlIDOfs [in]: Control ID - used as offset for controls
		\param iBackResID [in]: iSize
	*/
	virtual void SetInfo(tint32 iID, tint32 iSize, tbool bVisible, tint32 iTimerID );

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl* pControl, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl* pControl, void* pEventData);

	//! Connects all relevant GUI controls of the slider pane to ParamIDs
	virtual void ConnectControls();

	virtual void	HandleMsg(SMsg* pMsg);
	
	virtual void	SelectTrack( tbool bSelected);
	
	virtual tint32	GetSizeY();
	
	virtual void	SetVisible(tbool bVisible);
	
	virtual tbool	GetVisible(){return mbIs_Visible;};
	
	virtual void	SetPos(tint32 iPosY);
	
	virtual void	Track_Selected(tbool bSelected);
	
	virtual void	Set_Track_Size(tint32 iSize);

	virtual void UpdateTrackMeters_Maybe(void* mpvoid_sMeters_All);
	
	virtual void	UpdateChannelName( const std::string& sName);
	
	tint32			Get_ID(){ return miID;};
	
protected:
	tint32 miID;
	tint32 miTimerID;
	tint32 miSize;
	
//	ge::IBitmap* mpSmall_Back_Drop;
//	ge::IBitmap* mpSmall_Selected_Back_Drop;
//	ge::IBitmap* mpBig_Back_Drop;
//	ge::IBitmap* mpBig_SelectedBack_Drop;
	
	CTrack_Info_Small*	mpTrack_Info_Small;
	CTrack_Info_Big*	mpTrack_Info_Big;
	tbool mbIs_Visible;
	

	ge::IMouseTrap* mpMouseTrap;


	
	

};
