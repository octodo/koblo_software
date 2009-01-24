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


class CKS_Import_Files : public virtual CBasePane
{
public:
	CKS_Import_Files(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CKS_Import_Files();

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

	tbool AddFile(const std::string& sPathName);

	void RemoveFile();

	//! obsolete version
	void ImportFiles();

	void ClearFiles();

	tint32 GetAddedFiles() { return mFile_Items.size(); };
	
	
	
	//! new version
	void Import_Audio_Files();
	
	virtual tbool Import_Audio_File(const tchar* pszPathName, tbool bAlwaysKeepStereo, tint32 iTrackID =-1, tint64 iTrackPos = -1);

protected:
	
	CKSImportGUIPane* mpImportGUI;
	ge::IScrollPane* mpScrollPane;

	ge::IListBox* mpListBox;
	
	
	
	
	
	
/*
	struct SItemInfo {
		std::string sPathName;
		std::string sName;
	};
*/	
	
	
	
	

	std::list<CKSFile_Item> mFile_Items;

	void UpdateListBox();
};
