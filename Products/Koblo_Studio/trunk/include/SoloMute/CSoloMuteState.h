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

class CSoloMuteContainer;

class CSoloMuteState : public virtual IDestructable
{
protected:
	CSoloMuteState();

public:
	//! Create CSlider_GUI
	static CSoloMuteState* Create();

	virtual void Destroy();

	//! Initializer
	/*!
		\param iCtrlIDOfs [in]: Control ID - used as offset for controls
		\param iBackResID [in]: Background bitmap
	*/
	virtual void Init(CBasePane* pPane, CSoloMuteContainer* pParrent, tint32 iMuteParamID, tint32 iSoloParemID, tint32 iChannel);
	
	//! Solo
	/*!
		Invoked by the parameter conected to the solo buttons
	*/
	virtual void Solo(tbool bValue);
	
	//! SoftMute
	/*!
		Invoked by the parameter conected to the mute buttons
	*/
	virtual void SoftMute();
	
	//! Soft Un Mute
	/*!
		Invoked by the parameter conected to the mute buttons
	*/
	virtual void SoftUnMute();
	
	//! Soft Un Mute
	/*!
		Invoked by the parameter conected to the mute buttons
	*/
	virtual void SoftReMute();

	virtual tbool IsSolo(){return mbSolo;};

	virtual tbool IsMute(){return mbMute;};

	virtual tbool IsSoftMute(){return mbSoftMute;};

	virtual tbool WasMuted(){return mbWasMuted;};

	virtual tbool IsLocked() {return mbLock;};

	virtual void SetMuteFlag(tbool bState){ mbMute = bState;};

	virtual void SetSoloFlag(tbool bState){ mbSolo = bState;};

	virtual void SetWasMutedFlag(tbool bState){ mbWasMuted = bState;};

protected:
	CSoloMuteContainer* mpParrent;
	CBasePane* mpPane;

	tbool mbMute;
	tbool mbSolo;
	tbool mbSoftMute;
	tbool mbSoftUnMute;
	tbool mbLock;
	tbool mbWasMuted;
	tbool mbWasSolo;
	
	tint32 miMuteParamID;
	tint32 miSoloParamID;
	tint32 miSection;
};
