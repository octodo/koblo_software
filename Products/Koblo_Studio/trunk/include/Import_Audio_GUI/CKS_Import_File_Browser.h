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


class CKS_Import_File_Browser : public virtual CBasePane
{
public:
	CKS_Import_File_Browser(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CKS_Import_File_Browser();

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
	
	//! Returns the scroll pane
	ge::IScrollPane* GetScrollPane(){return mpScrollPane;};

	//! builds the list of files for the file browser
	void Build_File_List(const std::string& sPath);

	std::string GetSelectedFile();
	
	virtual void Update_Interface(std::string sMenu_Items);

protected:
	CKSImportGUIPane* mpImportGUI;

	ge::IScrollPane* mpScrollPane;

	std::string msFolder_Path;

	ge::IListBox* mpDirectory_Popup;
	tint32 miLatestGenuineIndex;
	tint32 miLatestSelectedFile;
	
	virtual void Display_File_Info(std::string sName);


	std::list<CKSFile_Item> mFile_Items;
	
};
