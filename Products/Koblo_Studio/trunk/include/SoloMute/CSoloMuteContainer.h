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

class CSoloMuteState;

class CSoloMuteContainer : public virtual IDestructable
{
protected:
	CSoloMuteContainer();

public:
	//! Create CSlider_GUI
	static CSoloMuteContainer* Create();

	void Destroy();

	//! Initializer
	/*!
		\param iCtrlIDOfs [in]: Control ID - used as offset for controls
		\param iBackResID [in]: Background bitmap
	*/
	virtual void Init(CBasePane* pGUI);
	
	//! Add Channel to container
	/*!
		\param iMuteParamID [in]: Param ID used for Mute
		\param iSoloParamID [in]: Param ID used for Solo
		\param iChannel [in]: Channel ID used for the pagameters
	*/
	virtual void AddChannel(tint32 iMuteParamID, tint32 iSoloParemID, tint32 iChannel);
	
	virtual void UpdateChannelData(tint32 iID, tint32 iValue, tint32 iChannel);
	
//	virtual void SetAUXData(tint32 iID, tint32 iValue, tint32 iAUX);
	
//	virtual void UpdateAuxData(tint32 iID, tint32 iValue);
	

	
	
protected:
	// Automatic mute tracks
	virtual void MuteAll(tint32 iCaller);
	// Set the mute flag on a track
	virtual void ReMuteThis(tint32 iCaller);
	// Unmute automatic muted tracks
	virtual void UnMuteAll(tint32 iCaller);
	// Set the unmute flag on a track
	virtual void UnMuteThis(tint32 iCaller);
	
	// Set solo
//	virtual void Solo(tint32 iCaller);
	// Set solo flags
	virtual void UnSolo(tint32 iCaller);
	
	// Set solo flags
	virtual void SoloIn(tint32 iCaller, tint32 iValue);
	
	// Set solo flags
	virtual void MuteIn(tint32 iCaller, tint32 iValue);
	
	virtual tbool CheckForSolo();

	CSoloMuteState* mpSoloMuteState[128];
	CBasePane* mpPane;
	tint32 miNumChannels;
};
