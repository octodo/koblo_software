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

//class CTrack_Time_Pos;

class CTrack_Fade_Handle : public virtual CBasePane, public virtual ge::ICustomControlCallback
{
public:
	CTrack_Fade_Handle(CBasePane* pParent, CBaseGUI* pGUI);
	
	//! SetInfo
	/*!
		\param iCtrlIDOfs [in]: Control ID - used as offset for controls
		\param iBackResID [in]: Background bitmap
	*/
	virtual void SetInfo(CRegion_Pane* pRegion_GUI, tuint uiHandle_Type);

	virtual ~CTrack_Fade_Handle();

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
	
	tbool Is_Moving(){return mbMouseCaptured;};
	
	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl* pControl, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl* pControl, void* pEventData);

	//! Connects all relevant GUI controls of the slider pane to ParamIDs
	virtual void ConnectControls();
	


	
	
	
protected:


	
	// Correctly typecasted pointer to plug-in class
//	CKSPlugIn* mpKSPlugIn;
	
	CRegion_Pane* mpRegion_GUI;

	tbool			mbMouseCaptured;
	tuint32			muiMouseDowPos;
	ge::ICustomControl* mpControl;
	
//	ge::IBitmap* mpHandle_Back;
	
	ge::IRect* mpFill_Rect;
	
	
	tuint muiHandle_Type;
	




	
};
