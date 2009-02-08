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


 
class CTrack;
class CTrack_Player;

class CTrack_Player2 : public virtual CBasePane, public virtual IRegionCallback
{
public:
	CTrack_Player2(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CTrack_Player2();

	virtual void Init();

	//! SetInfo
	/*!
		\param iCtrlIDOfs [in]: Control ID - used as offset for controls
		\param iBackResID [in]: Background bitmap
	*/
	virtual void SetInfo(CTrack_Player* CTrack_Player);

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl* pControl, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl* pControl, void* pEventData);

	//! Connects all relevant GUI controls of the slider pane to ParamIDs
	virtual void ConnectControls();
	//! Events
	virtual void HandleMsg(SMsg* pMsg);
	//! Returns the scroll pane
	ge::IScrollPane* GetScrollPane(){ return mpScrollPane;};
	
	// Set the track size
	virtual void Set_Track_Size(tint32 iTrack, tint32  iValue);
	
	virtual void Stack_Tracks(STrack_Stack sData);
	
	virtual void Update_Graphic();
	
	virtual void Deselect_Regions();
	
	virtual void InsertRegion(tuint32 uiRegionID, tuint32 uiTrack, tuint64 uiSample_Pos, tuint64 uiSample_Start, tuint64 uiSample_Duration, const std::string& sName);

	virtual void SelectRegion(tuint32 uiID, tuint32 uiTrack);

	virtual void RedrawSelection_For_Track(tint32 iTrack);
	
	virtual void JumpToPlayhead();
	
	
	
	virtual void Refresh_Region_GUI(tuint32 uiID, tuint32 uiTrack);

	virtual void DeleteRegion(tuint32 uiID, tuint32 uiTrack);
	//! store current mouse over track
	void SetMouse_Over_Track(tint32 iMouse_Over_Track){ miMouse_Over_Track = iMouse_Over_Track;};
	
	tint32 GetMouse_Over_Track(){ return miMouse_Over_Track;};
	//! Set selection size so it can be used by all tracks
	virtual void SetSelection( tuint64 uiSelection_Start_Sample, tuint64 uiSelection_End_Sample);
	//! Get selection size
	SSelection_Info GetSelection(){ return msSelection_Info;};
	//! Store X valus of mouse down position
	void Set_Mouse_Down_Pos_X(tuint32 uiMouse_Pos_X){ muiMouse_Down_Pos_X	= uiMouse_Pos_X;};
	//! Get mouse down position X
	tuint32 Get_Mouse_Down_Pos_X(){ return muiMouse_Down_Pos_X;};
	
	//! Store X valus of mouse up position
	void Set_Mouse_Up_Pos_X(tuint32 uiMouse_Up_Pos_X){ muiMouse_Up_Pos_X		= uiMouse_Up_Pos_X;};
	//! Get mouse up position X
	tuint32 Get_Mouse_Up_Pos_X(){ return muiMouse_Up_Pos_X;};
	
	virtual void SetTrackTopPtr(CTrack_Top* pTrack_Top);

	virtual void Set_LinePos(tint64 iSample_Pos);
	
	
protected:

	
	

	
struct STrack_GUI
{
	CTrack* pGUI;
	tbool	bIn_Use;
	tint32  iTrack_Id;
};

	// Pointers to tracks
	CTrack* mppTrack[giNumber_Of_Tracks];
		
	ge::IScrollPane* mpScrollPane;
	
	std::vector<ge::ILine*> mppLine;
	tint32		miLast_Grid_Line_Allocated;
	virtual void Allocate_Grid_Lines();
	
	
	
//	tfloat64	mfTempo;
	tfloat64	mfSamples_Pr_SubBeat;
	tfloat64	mfSamples_Pr_Beat;
	tfloat64	mfSamples_Pr_32;
	
	
	
	
//	tfloat64	mfSamplesPrSecond;
//	tfloat64	mfSamplesPrPixel;
	
//	tfloat64	mfPixel_Pr_Second;
//	tfloat64	mfPixel_Pr_Beat;
//	tint32		miZoomLevel;
	tint32		miSignature;
	tint32*		mpiPattern;
	tfloat64	mfBeats;
	tfloat64	mfSub_Beats;
//	tfloat64	mfBeats_Pr_Second;
//	tfloat64	mfPixels_Pr_32;
//	tfloat64	mfSamples_Pr_Pixel;
	
	tfloat64	mfGrid_In_Samples;
	
	ge::SRGB	msLineColor[giNumber_Of_Grid_Colors];

	
	//tuint64		muiPane_Duration_In_Samples;


	tbool		mbDraw_32;
	tbool		mbDraw_16;
	tbool		mbDraw_8;
	tbool		mbDraw_4;
	tbool		mbDraw_2;
	tbool		mbDraw_1;

	virtual void Update_Zoom();
	virtual void Draw_Grid();
	virtual void Hide_Grid();
	virtual void Show_Grid();

	
	inline void Get_Grid_Line(tint32 iLine);
	virtual void SetSignature(tint32 iSignature);
	
	virtual void Update_Empty_Array(ge::SSize sPaneSize);
	
//	virtual void Move_All_GUI_Tracks_Above_Up(tint32 iTrack);
//	virtual void Pull_Out_GUI_Track(tint32 iTrack);


	// Lasse, added 2008-05-07 and 08 - house-keeping

	//! The number of track visible to user
	tint32 miNbOfVisibleTracks;
	//! The accumulated height of all visible tracks
	tint32 miAccumTrackHeight;
	
	//! Make sure scroll-pane areasize is max the size of all visible tracks, but never less than visiblerect
	virtual void SanitizeScrollPaneAreaSize_Y();
	
	//! Make sure scroll-pane fits to zoom and audio
	virtual void SanitizeScrollPaneAreaSize_X();

	// .. Lasse
	
	CTrack_Player* mpCTrack_Player;
	
	tint32 miMouse_Over_Track;
	
//	tfloat64	mfSelection_Time_Start;
//	tfloat64	mfSelection_Time_End;
	SSelection_Info msSelection_Info;
	// Mouse down postiton X
	tuint64 muiMouse_Down_Pos_X;
	// Mouse up postiton X
	tuint64 muiMouse_Up_Pos_X;
	
	
	ge::IRect* mpEmpty_Rect;
	ge::IBitmap* mpBack_Track_Shaddow_Bmp;
	
	ge::IMouseTrap* mpMouseTrap;
	
	CTrack_Top* mpTrack_Top;
	
	
	// Play Head
	ge::ILine*		mpPlay_Line;
	tint32			miLast_Play_Line_Pos_X;

};
