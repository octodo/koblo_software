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


class CKSBaseGUI : public virtual CBaseGUI
{
public:
	CKSBaseGUI( de::IParameterManager* pParmMan);

	virtual void UpdateGUIData(tint32 iID, tint32 iValue) {}
	
	virtual void UpdateCommandData(tint32 iID, tint32 iValue) {}

	virtual void UpdateEngineData(tint iType, tint iExtra, tint32 iID, tint32 iValue) = 0;

	virtual void UpdateTrackData(tint32 iID, tint32 iValue, tint32 iChannel) {}
	
	virtual void UpdateBussData(tint32 iID, tint32 iValue, tint32 iBuss) {}
	
	virtual void UpdateMasterData(tint32 iID, tint32 iValue) {}
	
	virtual void UpdateAUX1Data(tint32 iID, tint32 iValue) {}
	
	virtual void UpdateAUX2Data(tint32 iID, tint32 iValue) {}
	
	virtual void UpdateAuxMonitorData(tint32 iID, tint32 iValue) {}

	virtual void UpdateChannelName(tint32 iChannel, const std::string& sName) {}
	
	virtual void Zoom(tint32 iCmd, tint32 iVal)  {}
	
//	virtual void AddTrack()  {}
//	virtual void DeleteTrack()  {}
//	virtual void SelectTrack(tint32 iID)  {}
	
	virtual void PlaybackStopped() {}

	virtual void UpdateAll() = 0;

	virtual tbool OnKeyDown(ge::EKey Key);

	//! Override this to implement keyboard input
	virtual tbool DoKeyDown(ge::EKey Key) {return false;}
	

	


	virtual tbool WindowClosed(void* pWnd) {return false;}
};

