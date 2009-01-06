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




/*! \brief the region  pane displays waveforms 
 *	users can cut, trim, fade in, fade out, and set the volume
 */





class CTrack_Player2;
class CTrack;
class CTrack_Fade_Handle;

class CRegion_Pane : public virtual CBasePane, public virtual ge::ICustomControlCallback
{
public:
	CRegion_Pane(CBasePane* pParent, CBaseGUI* pGUI);

	virtual ~CRegion_Pane();

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
	
//	virtual void SetFadeInTime(tint64 iSamplePoints);

	//! SetInfo
	/*!
		\param iCtrlIDOfs [in]: Control ID - used as offset for controls
		\param iBackResID [in]: Background bitmap
	*/
	virtual void SetInfo(	tuint32 uiRegionID,
							tuint64	uiSample_Pos,
							tuint64	uiSample_Start, 
							tuint64 uiSample_Duration, 
							tint32 iColor, 
							tint32 iSize_Y,
							CTrack_Player2* pTrack_Player2,
							CTrack* pTrack,
							const std::string& sName);

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl* pControl, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl* pControl, void* pEventData);

	//! Connects all relevant GUI controls of the slider pane to ParamIDs
	virtual void ConnectControls();
	// Handel messages
//	virtual void HandleMsg(SMsg* pMsg);
	
	//! called when the Y size of a track is changed
	virtual void Set_Track_SizeY(tint32 iSize_Y);
	
//	virtual void Deselect();
	
	tuint	GetID(){ return muiRegionID;};
	
	virtual void Update_Graphic();
	
	virtual void Refresh_Region_GUI();
	
	virtual void Update_Fade_In(tuint32 uiFade_In_Pixel);
	virtual void Update_Fade_Out(tuint32 uiFade_Out_Pixel);
	virtual void Update_Region_Volume(tint32 uiRegion_Volume_Pixel);
	
	
	
protected:

	virtual void Set_Size_X();
	virtual void Set_Pos_X();
	virtual void Update_Size();
	virtual tint32 Find_Tool();
	
	
	tbool Handle_LeftButtonDown( const ge::SPos& Pos);
	tbool Handle_LeftButtonUp( const ge::SPos& Pos);
	tbool Handle_MouseMove( const ge::SPos& Pos);
	
	virtual void Handel_Select_All_Tool();
	virtual void Handle_Cut_Tool(tuint32 uiPos);
	virtual void Handle_Trim_Tool(tint32 uiPos);



	virtual void SetSelection( tint64 uiSample_Selection_Start, tint64 uiSample_Selection_Duration);
	

	
	tuint32			muiRegionID;
	
	tuint64			muiTrack_Pos;
	tfloat64		mfSample_Start;
	tuint64			muiSample_Duration;
	tuint64			muiSample_End;
	tuint64			muiSample_Fade_In;
	tuint64			muiSample_Fade_Out;
	tfloat32		mfRegion_Volume;
	tuint32			muiStart_Pixel;
	tuint32			muiEnd_Pixel;
//	tuint32			muiDuration_Pixel;
	tuint32			muiFade_In_Pixel;
	tuint32			muiFade_Out_Pixel;
	tuint32			muiRegion_Volume_Pixel;
	

	tint32			miSize_X;
	//! miSize_Y holds two values 0 = small and 1 = big
	tint32			miPixel_Size_Y;
	tint32			miPos_X;
	tint32			miColor;
	tbool			mbMouseCaptured;
	tbool			mbExtendSelection;
	tuint64			muiSample_Selection_Start;
	tuint64			muiSample_Selection_Duration;


	ge::ILine*		mpLine_Start;
	ge::ILine*		mpLine_End;
	ge::IBitmap*	mpRegion_Back_Small;
	ge::IBitmap*	mpRegion_Back_Big;


	
	
	//! Current mouse position

//	ge::SPos		mMouse_Down_Pos;
	
	CTrack_Player2* mpTrack_Player2;
	CTrack*			mpTrack;
	
	//! the region pane receives mouse events so it holds a ICustomControl
	ge::ICustomControl* mpControl;

	ge::IDrawPrimitives* mpDrawPrimitives;

	tint32			miTrimStart;
	// Edit state
	tint32			miEdit_State;
	
	tint32 miSelectionType;
	
	ge::IText* mpText;
	char mPsz[256];

	IFile* mppPeakFile[4];
	tfloat32* mppfPeak[4];
	tuint64 mpiPeakFileSize[4];
	tint64 maiActuallyRead[4];
	
	
	
	
	tbool mbDrawFade;
	ge::ILine*		mpLine_Fade_In;
	ge::ILine*		mpLine_Fade_Out;
	ge::ILine*		mpLine_Region_Volume;
	
	virtual void Draw_Fade_In();
	virtual void Draw_Fade_Out();
	virtual void Draw_Region_Volume();
	

	
	CTrack_Fade_Handle* mpFade_In_Handle;
	CTrack_Fade_Handle* mpFade_Out_Handle;
	CTrack_Fade_Handle* mpRegion_Volume_Handle;
	
	
	
	
};
