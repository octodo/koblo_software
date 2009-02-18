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

class CMix_Master_AUX_Insert;

class CMix_Master : public virtual CBasePane
{
public:
	CMix_Master(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CMix_Master();

	virtual void Init();

	//! SetInfo
	/*!
		\param iCtrlIDOfs [in]: Control ID - used as offset for controls
		\param iBackResID [in]: Background bitmap
	*/
	virtual void SetInfo(const ge::IPopupMenu::SMenuItemList& Mix_Out_Menu);

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl* pControl, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl* pControl, void* pEventData);

	//! Connects all relevant GUI controls of the slider pane to ParamIDs
	virtual void ConnectControls();

	virtual void HandleMsg(SMsg* pMsg);

	virtual void UpdateMeters(void* pvoid_sMeters_All);
	
	//! Returns the scroll pane
//	ge::IScrollPane* GetScrollPane(){return mpScrollPane;};
	
protected:

//	tint32 miID;
//	tint32 miCtrl_Offset;
//	tint32 miSection;

	CMix_Master_AUX_Insert* mpMix_Master_AUX_Insert;
	ge::IPopupMenu::SMenuItemList mpMix_Out_Menu;
	
	virtual ge::IDropDownListBox* CreatePop( tint32 iControlID, 
											tint32 iBitmapID,  
											const ge::IPopupMenu::SMenuItemList& List, 
											ge::SPos Pos);
											
	ge::IPeakMeter* mpMeterOutL;
	ge::IPeakMeter* mpMeterOutR;
	
	ge::IPanner* mpPan;


};
