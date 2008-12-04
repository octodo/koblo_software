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


class CRegion_GUI;
class CTrack_Player2;

class CTrack : public virtual CBasePane, public virtual ge::ICustomControlCallback
{
public:
	CTrack(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CTrack();

	virtual void Init();
	//! ICustomControlCallback override
	virtual void OnDraw(const ge::SRect &rUpdate);
	//! ICustomControlCallback override
	virtual tbool OnMouse(ge::EMouseMsg MouseMsg, const ge::SPos& Pos);
	//! ICustomControlCallback override
	virtual tbool OnKeyDown(ge::EKey Key);
	//! ICustomControlCallback override
	virtual void OnTimer();
	//! ICustomControlCallback override
	virtual void OnValueChange();
	//! ICustomControlCallback override
	virtual void SetSize(const ge::SSize& Size);
	//! ICustomControlCallback override
	virtual void SetPos(const ge::SPos& Pos);

	//! SetInfo
	/*!
		\param iCtrlIDOfs [in]: ID 
		\param iBackResID [in]: Vertical size of track
	*/
	virtual void SetInfo(tint32 iTrackID, tint32 iTrackSize,CTrack_Player2* pTrack_Player2);

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl* pControl, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl* pControl, void* pEventData);

	//! Connects all relevant GUI controls of the slider pane to ParamIDs
	virtual void ConnectControls();

	virtual void HandleMsg(SMsg* pMsg);
	
	virtual void Set_Track_SizeY
( tint32 iSize);
	
	virtual tint32 GetSize(){return mTrackSize.iCY+1;};
	
	virtual void InsertRegion(tuint32 uiID, tuint64 uiSample_Pos, tuint64 uiSample_Start, tuint64 uiSample_Duration, const std::string& sName);

	virtual void Refresh_Region_GUI(tuint32 uiID);

//	virtual void SetFadeInTime(tuint32 uiID, tuint64 uiFadeInTime);

//	virtual void SetFadeOutTime(tuint32 uiID, tuint64 uiFadeOutTime);

	virtual void DeleteRegion(tuint32 uiID);
	
//	virtual void DeleteSelection(tuint32 uiID);
	
	virtual void Delete_All_Regions();

	virtual void SetPos(tint32 iPosY);
	
//	virtual void Set_Samples_Pr_Pixel(tfloat32 fSamples_Pr_Pixel);
	
//	virtual void Set_Zoom_Level(tint32 iZoom_Level){miZoom_Level = iZoom_Level;};
	
	virtual void Deselect_Regions();
	
	virtual void SelectRegion(tuint32 uiRegionID);
	
//	virtual void Set_Grid(tfloat64 fGrid_In_Samples){ mfGrid_In_Samples = fGrid_In_Samples;};
	//! Called from a region passing mouse mesaages back to the track used for selection
	virtual void SetTrackSelection(const ge::SPos& Pos, tint32 iRegionID, tint32 iSelection_Type);
	//! Release mouse
	virtual void Release_Mouse(){ mbMouseCaptured = false;};
	//! Set the selection size based upon data from the DSPEngine
	virtual void Set_Selection_Size_And_Pos();
	
	virtual void SetSizeX(tint32 iSizeX);
	
	//! Dropping a region on a track
	virtual void	Handle_Mouse_Drop(ge::SPos Pos);
	//! Dragging a region around
	virtual void	Handle_Drag_Region(ge::SPos Pos);
	
	virtual void	SetTrackTopPtr(CTrack_Top* pTrack_Top){ mpTrack_Top = pTrack_Top;};
	
	tint32 Get_TrackID(){return miTrackID;};
	

protected:

	virtual tint32 Find_Tool();

	//! Select what region to show big or small
	virtual void	Show_Selection();
	virtual void	Hide_Selection();
	virtual void	Update_Region_Graphic();
	

	tfloat64		mfGrid_In_Samples;
	tint32			miTrackID;
	tint32			miTrack_Size_Y;
	tint32			miLastSelectionEndPos;
	

	ge::SSize mTrackSize;
//	ge::IInvert* mpInvert;
	
	std::vector<CRegion_GUI*> mppRegion_GUI;
	
	tint32 miLastRegion_GUI;
	tbool mbClicked;
	tbool mbMouseOver;
	tbool mbExtendSelection;

	tint32 miSelectionExtendsInYDirection;


	// Correctly typecasted pointer to plug-in class
	CKSPlugIn* mpKSPlugIn;

	
	ge::ICustomControl* mpControl;
	
	CTrack_Player2* mpTrack_Player2;
	
	std::list<CRegion_GUI*> mppRegions;
	
	ge::ILine*	mpTrack_Buttom_Line;

	ge::IBitmap*	pmBmp_Select_Big;
	ge::IBitmap*	pmBmp_Select_Small;
	tbool mbMouseCaptured;
	//! Size of region beeing dragged
	tint64 miDrag_Region_Size;
	
	CTrack_Top* mpTrack_Top;
	
	
	
};
