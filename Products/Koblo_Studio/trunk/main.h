


class CMainApplication
{
public:
	virtual CBaseGUI* CreateExtraWindow(tint32 iGUIIndex, /*CFStringRef*/void* pszResName, tbool bAlwaysOnTop = false);

	virtual void CloseWindow(void* mhWnd);
	
protected:
//	tbool mbShowZoom;
	

};

extern CMainApplication* gpMainApplication;
