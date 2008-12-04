/*!	\file IBrowser.h
	\author Michael Olsen
	\date 29/Mar/2005
	\date 29/Mar/2005
*/

// Forwards
class IBrowserCallback;
class IScrollPane;
class IScrollBar;

/*! \class IBrowser
 * \brief Browser control. Note that it must reside inside an IScrollPane
 *
 *	An IBrowser can be used to browse anything, not just files. In this case a "folder" is anything which may contain other items ("may" because the folder could be empty). However, all items within a specific folder must have unique names
*/
class IBrowser : public virtual IControl, public virtual IDestructable
{
public:
	//! Create IBrowser
	static IBrowser* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param pWnd [in]: Window control is residing in
		\param pFont [in]: Font resource to use
		\param pBmp [in]: Bitmap resource to use (arrow). Must be 2 frames
	*/
	virtual void Init(tint32 iID, IWindow* pWnd, IFontResource* pFont, IBitmapResource* pBmp) = 0;

	//! Struct which defines one browser item
	struct SItem {
		//! Name to show
		tchar pszName[256];
		//! If it's a folder true, otherwise false
		tbool bFolder;

		SItem() : bFolder(false) {pszName[0] = 0;}
	};

	//! Set the callback
	/*!
		\param pCallback [in]: Browser callback. Will be destroyed at destruction time
	*/
	virtual void SetCallback(IBrowserCallback* pCallback) = 0;

	//! Sets the scrollpane which the browser is connected to
	/*!
		\param pPane [in]: Scroll pane which the browser is connected to
	*/
	virtual void SetScrollPane(IScrollPane* pPane) = 0;

	//! Sets the scrollbar which the browser is connected to
	/*!
		\param pScrollBar [in]: Scroll bar which the browser is connected to
	*/
	virtual void SetScrollBar(IScrollBar* pScrollBar) = 0;

	//! Called when browser needs to update
	virtual void Update() = 0;

	//! Returns the path name of the currently selected item, or ""
	/*!
		\param pszPathName [out]: Pointer to string to return the pathname of selected item, or "" if none. Must be preallocated with minimum 512 characters
	*/
	virtual void GetSelectedItem(tchar* pszPathName) = 0;
};
