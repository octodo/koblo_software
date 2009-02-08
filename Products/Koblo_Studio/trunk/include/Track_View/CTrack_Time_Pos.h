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




class CTrack_Time_Pos : public virtual CBasePane, public virtual ge::ICustomControlCallback
{
public:
	CTrack_Time_Pos(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CTrack_Time_Pos();

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
	
	virtual void SetSignature(tint32 iSignature);
	//! Set the playhead position in Samples
	virtual void Set_Play_Head(tint64 iSample_Pos);
	
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
	
	void SetTrackTopPtr(CTrack_Top* pTrack_Top){  mpTrack_Top = pTrack_Top;};


	
protected:

	ge::ILine*		mpLine1;
	ge::ILine*		mpLine2;
	ge::ILine*		mpLine3;
	
	tbool			mbMouseCaptured;
	ge::ICustomControl* mpControl;
	



	std::vector<ge::ILine*>		mppBar_Beat_Line;
	tint32						miLast_Bar_Beat_Line_Allocated;
	tint32						miLast_Bar_Beat_Line_Used;
	std::vector<ge::IText*>		mppBar_Beat_Text;
	tint32						miLast_Bar_Beat_Text_Allocated;
	tint32						miLast_Bar_Beat_Text_Used;
	std::vector<ge::ILine*>		mppMin_Sec_Line;
	tint32						miLast_Min_Sec_Line_Allocated;
	tint32						miLast_Min_Sec_Line_Used;
	std::vector<ge::IText*>		mppMin_Sec_Text;
	tint32						miLast_Min_Sec_Text_Allocated;
	tint32						miLast_Min_Sec_Text_Used;
	
	ge::SRGB	msLineColor[giNumber_Of_Grid_Colors];
	
	ge::IPane*	mpBar_Beat_Pane;
//	tfloat64	mfTempo;
	tfloat64	mfBeats;
	tfloat64	mfSub_Beats;
	tfloat64	mfBeats_Pr_Second;
//	tfloat64	mfPixel_Pr_Sample;

	tfloat64	mfPixel_Pr_Beat;
	tfloat64	mfSamples_Pr_32;
	tfloat64	mfSamples_Per_Line;
	tfloat64	mfPixel_Pr_Bar;

	
	tfloat64	mfSec_Per_Pixel;
	tint32		miSec_Pr_line;
	
	tfloat32	mfLine_Dist;
	tint32		miSignature;
	tint32*		mpiPattern;
	
	// Bar & Beat flags
	tbool		mbDraw_32;
	tbool		mbDraw_16;
	tbool		mbDraw_8;
	tbool		mbDraw_4;
	tbool		mbDraw_2;
	tbool		mbDraw_1;
	tbool		mbDraw_1_2;
	tbool		mbDraw_1_4;
	tbool		mbDraw_1_8;
	
	ge::IRect* mpMin_Sec_Fill_Rect;
	ge::IRect* mpBar_Beatc_Fill_Rect;
	

	
	
	virtual void Draw_Bars_Beat_Grid();
	virtual void Draw_Min_Sec_Grid();
	virtual void Update_Zoom();
	virtual void Update_Graphic();
	inline void Draw_Min_Sec_Line(tint32 iLine, ge::SPos posStart, ge::SPos posEnd, ge::SRGB col);
	inline void Draw_Min_Sec_Text( tint32 iText_ID, char* psz, ge::SPos pos, ge::SRGB color);
	inline void Draw_Bar_Beat_Line(tint32 iLine, ge::SPos posStart, ge::SPos posEnd, ge::SRGB col);
	inline void Draw_Bar_Beat_Text( tint32 iText_ID, char* psz, ge::SPos pos, ge::SRGB color);
	void Set_Loop_Selection();
	virtual void Update_Loop(tint32 iSampleStart, tint32 iSampleEnd);
	
	CTrack_Playhead* mpTrack_Playhead;
	tint32 miLast_Pos_X;
	
	ge::IBitmap*	mpLoop;
	virtual void Draw_Loop();
	void Get_Loop(tint32 &iStart, tint32 &iSize);
	
	tfloat32 mfMouse_Down_Pos_X;
	tint32 mfMouse_Down_Pos_Y;
	tint32 miEdit;
	
	SLoop_Drawing_Info mfSLoop_Drawing_Info;
	
	CTrack_Top* mpTrack_Top;
	
	
	

};
