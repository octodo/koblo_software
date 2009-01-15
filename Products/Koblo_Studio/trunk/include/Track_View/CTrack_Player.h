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

class CTrack_Time_Pos;
 class CTrack_Player2;
 
//class CTrack;

class CTrack_Player : public virtual CBasePane
{
public:
	CTrack_Player(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CTrack_Player();

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
	ge::IScrollPane* GetScrollPane();
	ge::IScrollPane* GetScrollPane2();
	
	//!Show a track on the GUI
	/*!
		\param iTrackID [in]: Unique ID for the track;
	*/
//	virtual void SetTrackVisible(tint32 iTrackID, tbool bVisible);
	
	// Set the track size
	virtual void Set_Track_Size(tint32 iChannel,tint32  iValue);
	
	virtual void Set_Play_Head(tint64 iSample_Pos);

	// Lasse, test
	//! CBasePane override
	virtual void Resize(const ge::SSize& Size);
	//! Update scroll pane size
	virtual void Set_Scrol_Pane_Size_X(tint32 iSizeX);
	// .. Lasse
	
	CTrack_Time_Pos* mpTrack_Time_Pos;
	CTrack_Player2* mpTrack_Player2;
	
	virtual void SetTrackTopPtr(CTrack_Top* pTrack_Top);

protected:

	
	
	ge::IScrollPane* mpScrollPane;

//	ge::ILine* mpDragLine;
};
