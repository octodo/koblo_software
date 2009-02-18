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



class CKSPlugInEffectPane : public virtual CBasePane, public virtual ITimerCallback
{
public:
	CKSPlugInEffectPane(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CKSPlugInEffectPane();

	virtual void Init();

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl* pControl, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl* pControl, void *pEventData) {}
	
	virtual void ConnectControls();

	virtual void SetGUI(kspi::IGUI* pGUI);

	// ITimerCallback override
	virtual void OnTimer(tint32 iID);

	void OnCloseGUI();

protected:
	kspi::IGUI* mpGUI;

	ITimer* mpTimer;

#ifdef _Mac
	WindowRef mWindowRef;
#endif	// _Mac

	tbool mbGUILoaded;
};

