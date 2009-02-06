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


/*! \brief the track editor is the main window on windows
 *	from this window the user does all the audio edditing
 *	the window is organized in five panes
 */


//class CTrack_Top;
//class CTrack_Side;
//class CTrack_Player;
//class CTrack_File_Browser;
//class CTrack_System_Info;

class CSoloMuteContainer;

class CDSP;


class CTrack_Editor_View : public virtual CKSBaseGUI, public virtual ge::IEventReceiver, public virtual ITimerCallback
{
public:
	CTrack_Editor_View( de::IParameterManager* pParmMan);

	virtual ~CTrack_Editor_View();

	//! CBaseGUI override
	virtual void KSPI_CALL OnTimer();

	//! IDestructable override
	virtual void KSPI_CALL Destroy();

	//! IGUI override
	virtual void KSPI_CALL GetSize(tuint32* piSizeX, tuint32* piSizeY);

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl *pSender, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl *pSender, void *pEventData);

	//! IContextCallback override
	virtual void GetName(tchar* psz);

	//! Called by the plug-in when a parameter has changed
	virtual void UpdateEngineData(tint iType, tint iExtra, tint32 iID, tint32 iValue);

	virtual void UpdateTrackData(tint32 iID, tint32 iValue, tint32 iChannel);
	
	virtual void UpdateBussData(tint32 iID, tint32 iValue, tint32 iBuss);
	
	virtual void UpdateMasterData(tint32 iID, tint32 iValue);
	
	virtual void UpdateGUIData(tint32 iID, tint32 iValue);
	
	virtual void UpdateCommandData(tint32 iID, tint32 iValue);
	
//	virtual void UpdateAuxMonitorData(tint32 iID, tint32 iValue);
	
	virtual void UpdateAll() {}

	virtual void UpdateChannelName(tint32 iChannel, const std::string& sName);
	


	virtual tbool DoKeyDown(ge::EKey Key)  {return dynamic_cast<CTrack_Editor_Pane*>(mpPane)->DoKeyDown(Key);}
	
	virtual tbool DoKeyUp(ge::EKey Key)  {return dynamic_cast<CTrack_Editor_Pane*>(mpPane)->DoKeyUp(Key);}

	virtual tbool WindowClosed(void* pWnd);

//	kspi::IPlugIn* GetEffect(tint32 iChannel, tint32 iInsert);
	
	

protected:
	//! CBaseGUI override
	virtual void Initialize();

	//! Parameter/controls linking
	virtual void ConnectControls();

	
	
	ITimer* mpPlay_Head_Timer;
	ITimer* mpTimerMeters;
	virtual void OnTimer(tint32 iTimerID);
	
	
	
	
};

