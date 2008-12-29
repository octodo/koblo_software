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


class CKSPlugInEffectPane;
//class CPlugin_Toolbar;

class CKSPlugInGUIPane : public virtual CBasePane
{
public:
	CKSPlugInGUIPane(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CKSPlugInGUIPane();

	virtual void Init();

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl* pControl, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl* pControl, void *pEventData) {}

	virtual void ConnectControls();

	virtual void SetInfo(tint32 iSectionID, tint32 iInsertIndex);

	virtual void SetGUI(kspi::IGUI* pGUI);

	void SetWidth(tuint32 uiNew);

	void OnCloseGUI();

	void UpdateChannelData(tint32 iID, tint32 iValue, tint32 iChannel);

	virtual void UpdateChannelName(tint32 iChannel, const std::string& sName);

protected:
	tint32 miSectionID;

	tint32 miInsertIndex;

	CKSPlugInEffectPane* mpEffectPane;
	
	CPlugin_Toolbar* mpToolbar;
};

