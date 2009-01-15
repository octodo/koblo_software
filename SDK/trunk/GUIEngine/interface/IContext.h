/*!	\file IContext.h
	\author Michael Olsen
	\date 09/Dec/2004
	\date 09/Dec/2004
*/

const tuint32 GE_MAX_NR_OF_MENU_ITEMS = 128;


/*! \class
 * \brief Context which the GUI runs in
 *
 * Often a context is an application, but it might as well be 1 instance of a plug-in.
 * <br>Each instance (plug-in or application) has 1 context, and 1 only.
*/
class IContext : virtual public IDestructable
{
public:
	//! Creates IContext
	static IContext* Create();

	//! Sets callback to supply info about application
	/*!
		\param pCallback [in]: Callback to set
	*/
	virtual void SetCallback(IContextCallback* pCallback) = 0;

	virtual void SetApplicationCallback(IApplicationCallback* pCallback) = 0;

	//! Windows: p points to instance handle (HINSTANCE)
	/*!
		\param p [in]: Platform dependent. Windows: Instance handle (HINSTANCE)
	*/
	virtual void SetInstanceData(void* p) = 0;

	//! Windows: returns pointer to instance handle (HINSTANCE)
	/*!
		\return void*: Platform dependent. Windows: Instance handle (HINSTANCE)
	*/
	virtual void* GetInstanceData() = 0;
	//! \overload
	virtual const void* GetInstanceData() const = 0;

	//! Initializes plug-in data. Neccasary before starting to create windows. Be sure to call SetInstanceData() before calling this
	virtual void Init() = 0;

	//! Gets full path to binary we reside in
	/*!
		\param pszPathName [out]: Path name of binary (or "application") we reside in. Must point to at least 513 bytes of data
	*/
	virtual void GetPathName(tchar* pszPathName) = 0;

	virtual void* CreateMainWindow(SSize Size = SSize(0, 0), tint32 iWindowsOnly_MenuResourceID = -1, tint32 iWindowsOnly_IconResourceID = -1) = 0;

//	virtual void* CreateExtraWindow(CFStringRef pszResName) = 0;
	virtual void* CreateExtraWindow(void* pszResName, SSize Size = SSize(0, 0), tbool bAlwaysOnTop = false) = 0;

	virtual void RunMainLoop() = 0;

	//! Will allow for redraw events
	/*!
		\param bAllowMouseAndKeyboard [in]: False => discards all mouse clicks and keyboard events
		\param pbBreak [out]: Is True if application quit or other command like that has been reached. Those will not have been removed from queue
	*/
	virtual void LimitedHandleEventQueue(tbool bAllowMouseAndKeyboard, tbool* pbBreak) = 0;

	struct SMenuItemList;

	struct SMenuItem {
		SMenuItem(const tchar* pszName = NULL, tbool bHasSubMenu = false, SMenuItemList* pSubMenu = NULL, char cmd = 0)
			: pszName(pszName), bHasSubMenu(bHasSubMenu), pSubMenu(pSubMenu), cmd(cmd) {}

		~SMenuItem() {}

		const tchar* pszName;

		tbool bHasSubMenu;

		SMenuItemList* pSubMenu;
		
		char cmd;
	};

	struct SMenuItemList {
		std::string sName;

		tuint32 uiItemCount;

		SMenuItem pItems[GE_MAX_NR_OF_MENU_ITEMS];
	};

	virtual void CreateMenu(const SMenuItemList* pList) = 0;

	virtual void ShowWindow(void* pWnd, tbool bShow = true) = 0;

	virtual tbool IsWindowVisible(void* pWnd) = 0;

	virtual void SelectWindow(void* pWnd) = 0;

	// Lasse, added 2008-04-30
	//! What closing the window really does
	enum EClosingBehaviour {
		giClosingOnlyHides = 0,
		giClosingReallyCloses,
		giClosingQuitsApp,
	};

	//! Call to set what really happens when user tries to close window. If this method is not explicitly called the window is closed normally (i.e. giClosingReallyCloses)
	/*!
		\param bQuitOnClose [in]: TRUE => Application shuts down when window closes, FALSE => only window is closed / hidden and application continues
	*/
	virtual void SetClosingBehaviour(void* pWnd, EClosingBehaviour eClosingBehaviour) = 0;

	//! Read back the current behaviour of close window
	virtual const EClosingBehaviour GetClosingBehaviour(void* pWnd) = 0;
	// .. Lasse

	// Lasse, added 2008-05-06
	//! Prevent window from getting smaller than this size
	/*!
		\param pWnd [in]: The window to set limit for
		\param sizeMin [in]: The minimum size limit. Set one or both dimensions to 0 to ignore it and allow shrinking to 0
	*/
	virtual void SetMinWindowSize(IWindow* pWnd, SSize sizeMin) = 0;

	//! Prevent window from getting larger than this size
	/*!
		\param pWnd [in]: The window to set limit for
		\param sizeMax [in]: The maximum size limit. Set one or both dimensions to 0 to ignore it and allow growing to any size
	*/
	virtual void SetMaxWindowSize(IWindow* pWnd, SSize sizeMax) = 0;
	// .. Lasse

	virtual void CloseWindow(void* pWnd) = 0;
};




