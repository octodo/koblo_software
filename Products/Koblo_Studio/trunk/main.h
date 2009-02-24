


class CMainApplication
{
public:
	virtual CBaseGUI* CreateExtraWindow(tint32 iGUIIndex, /*CFStringRef*/void* pszResName, tbool bAlwaysOnTop = false, tuint32 iX = 0, tuint32 iY = 0);

	virtual void CloseWindow(void* mhWnd);
	
protected:
//	tbool mbShowZoom;
	

};

extern CMainApplication* gpMainApplication;
