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

class CTrack_Top : public virtual CBasePane
{
public:
	CTrack_Top(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CTrack_Top();

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
	
	// Update timt display
	virtual void Update_Time_Display(tint64 iSample_Point);
	
	// Update timt display
	virtual void SetCursorInSamples(tint64 iSample_Point);
	
	// Update timt display
	virtual void SetStartSamples(tint64 iSample_Point);
	
	// Update timt display
	virtual void SetEndSamples(tint64 iSample_Point);
	
	virtual void Show_Progress(tbool bOn);
	//! Move the progress bar 0.0 start 1.0 end
	virtual void Set_Progress(tfloat32 fProgress);
	virtual void Set_ProgressText(const tchar* pszText = NULL);
	
	tbool		mbPlaying;

protected:
	tint32 miZoomLevel;
	tfloat32	mfSamples_Pr_Pixel;
	tfloat32	mfPixel_Pr_Second;
	tfloat32	mfBeats_Pr_Second;
	tfloat32	mfPixel_Pr_Beat;
	tfloat32	mfPixels_Pr_32;
//	tfloat32	mfTempo;
	tfloat32	mfSec_Per_Pixel;
	tint32*		mpiPattern;
//	tbool		mbPlaying;
	tbool		mbFFPressed;
	tbool		mbRewindPressed;


	ge::IText* mpTextHour;
	ge::IText* mpTextMinute;
	ge::IText* mpTextSecond;
	ge::IText* mpTextMilliSecs;

	tint32 miSecond;
	tint32 miMin;
	tint32 miHour;
	
	
	
	ge::IText* mpText_Sample_Start;
	ge::IText* mpText_Sample_End;
	ge::IText* mpText_Sample_Cursor;


	virtual void Pixel_To_Sample(tint32 iPixel);
	virtual void Update_Zoom();
	
	tuint64 muiSamplePos;
	
	ge::IBitmap* mpProgress_Back;
	ge::IText*   mpProgress_Text;
	ge::IBitmap* mpProgress_Bar;
	
	ge::IPopupMenu::SMenuItemList mpTime_Signature_Menu;
	ge::IPopupMenu::SMenuItemList mpSnapTo_Menu;
	
	virtual void PreparePopups();
	ge::IDropDownListBox* CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos);
	
	CKSPlugIn* mpKSPlugIn;

};
