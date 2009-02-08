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



// Forward declarations

class CDSP;
class CMix_Scroll;

class CMix_Buss;
class CMix_Master;

class CKSMixerGUIPane : public virtual CBasePane
{
public:
	CKSMixerGUIPane(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CKSMixerGUIPane();

	virtual void Init();

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl* pControl, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl* pControl, void *pEventData) {}
	
	virtual void ConnectControls();
	
	virtual void UpdateEngineData( tint32 iType, tint32 iExtra, tint32  iID, tint32 iValue);
	
	virtual void UpdateTrackData(tint32 iID, tint32 iValue, tint32 iChannel);
	
	virtual void UpdateBussData(tint32 iID, tint32 iValue, tint32 iBuss);
	
	virtual void UpdateMasterData(tint32 iID, tint32 iValue);
	
	virtual void UpdateGUIData(tint32 iID, tint32 iValue);
	
	virtual void UpdateCommandData(tint32 iID, tint32 iValue);
	
	virtual void OnTimer(tint32 iTimerID);
	
	//! CBasePane override
	virtual void SendMsgFromTop(SMsg* pMsg);

	
	
	//! CBasePane override
	virtual void Resize(ge::SSize SizeNew);
	
	CMix_Scroll* mpMix_Scroll;
	
	virtual void UpdateChannelName(tint32 iTrack, const std::string& sName);

	virtual void Update_All_Meters(void* pvoid_sMeters_All);

protected:
	CDSP* mpDSP;
	
	//void* mpvoid_sMeters_All;
	//tuint32 miuTimeMS_Prev;
	tbool mbFirstMeterSet;
	tbool mbIsVisible;

	ge::IScrollBar* mpScrollBarHorizontal;

//	virtual void UpdateGUI();
	
	virtual void Create_Mix_ScrollBar_Horizontal();
	

	CMix_Buss* mpMix_Busses[giNumber_Of_Busses];
	CMix_Master* mpMix_Master;
	ge::IPopupMenu::SMenuItemList mpMix_Master_Out_Menu;
	
	
	
};

