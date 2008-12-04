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



/*! \class
 * \brief Implementation of the part of IContext which is Windows specific
*/
class CContextWin : public virtual CContext
{
public:
	//! Constructor
	CContextWin();
	//! Destructor
	virtual ~CContextWin();

	//! IDestructable override
	virtual void Destroy();

	//! IContext override
	virtual void SetInstanceData(void* p);
	//! IContext override
	virtual void* GetInstanceData();
	//! IContext override
	virtual const void* GetInstanceData() const;
	//! IContext override
	virtual void Init();
	//! IContext override
	virtual void GetPathName(tchar* pszPathName);
	//! IContext override
	virtual void* CreateMainWindow(SSize Size = SSize(0, 0), tint32 iWindowsOnly_MenuResourceID = -1, tint32 iWindowsOnly_IconResourceID = -1);
	//! IContext override
	virtual void* CreateExtraWindow(void* pszResName, SSize Size = SSize(0, 0), tbool bAlwaysOnTop = false);
	//! IContext override
	virtual void RunMainLoop();
	//! IContext override
	virtual void LimitedHandleEventQueue(tbool bAllowMouseAndKeyboard, tbool* pbBreak);
	//! IContext override
	virtual void CreateMenu(const SMenuItemList* pList);
	//! IContext override
	virtual void ShowWindow(void* pWnd, tbool bShow = true);
	//! IContext override
	virtual tbool IsWindowVisible(void* pWnd);
	//! IContext override
	virtual void SelectWindow(void* pWnd);
	//! IContext override
	virtual void CloseWindow(void* pWnd);

	// Lasse, added 2008-04-30
	//! IContext override
	virtual void SetClosingBehaviour(void* pWnd, EClosingBehaviour eClosingBehaviour);
	//! IContext override
	virtual const EClosingBehaviour GetClosingBehaviour(void* pWnd);
	// .. Lasse

	// Lasse, added 2008-05-06
	//! IContext override
	virtual void SetMinWindowSize(IWindow* pWnd, SSize sizeMin);
	//! IContext override
	virtual void SetMaxWindowSize(IWindow* pWnd, SSize sizeMax);
	// .. Lasse

	void GetClassName(std::string& rs);

	// Internal
	virtual void PreDestroyMainWnd();

	virtual void SizeChanged(void* pWnd, SSize SizeNew);

	virtual tbool SendOnMenuEvent(tchar* pszMenEvent);

	virtual void WindowClosed(void* pWnd);

protected:
	HINSTANCE mhInstance;
	HMENU mhMenu;
};
