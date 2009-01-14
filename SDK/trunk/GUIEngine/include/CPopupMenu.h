/*!	\file CPopupMenu.h
	\author Michael Olsen
	\date 18/Feb/2005
	\date 18/Feb/2005
*/

/*! \class CPopupMenu
 * \brief Implementation of IPopupMenu
*/
class CPopupMenu : public virtual IPopupMenu, public virtual CControl, public virtual CPane, public virtual CEventReceiver
{
public:
	//! Constructor
	CPopupMenu();
	//! Destructor
	virtual ~CPopupMenu();

	//! IDestructable override
	virtual void Destroy();

	//! IControl override
	virtual void OnDraw(const SRect &rUpdate);
	//! IControl override
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos);
	//! IControl override
	virtual tbool OnKeyDown(EKey Key);
	//! IControl override
	virtual void OnTimer();
	//! IControl override
	virtual void SetVisible(tbool bVisible);

	//! CPane override
	virtual void SetValue(tint32 iNew, tbool bNotify = true);

	//! IPopupMenu override
	virtual void Init(tint32 iID, IWindow* pWnd, IResourceManager* pResMan, tint32* piIDs, IFontResource* pFont, IFontResource* pFontInv, const SMenuItemList& Items = *((SMenuItemList*)NULL));
	//! IPopupMenu override
	virtual void GetText(tint32 iValue, tchar* psz);

	//! IPopupMenu implementation
	virtual void SetInlineIconBitmap(IBitmapResource* pBitmap, tint32 iFrames);


	//! IPopupMenu implementation
	virtual SPos GetBestAbsPos(IControl* pControlThatInitiated);
	//! IPopupMenu implementation
	virtual SPos GetBestAbsPos(SPos posAbsoluteWanted, SSize sizeOfObstructiveObject = SSize(1,0) );
	//! IPopupMenu implementation
	virtual void PositionSidewaysFirst(tbool bSideways = true)
	{ mbPositionSidewaysFirst = bSideways; };

	// Lasse, added 2007-12-03 thru -06 - more flexibility
	//! IPopupMenu implementation
	virtual tint32 DestroyItems(tint32 iStartIx = 0, tint32 iCount = -1);
	//! IPopupMenu implementation
	virtual SMenuItem* CreateAndAppendItem(const tchar* pText, tint32 iValue = -1, SMenuItemList* pSubList = NULL, IBitmapResource* pBitmapRes = NULL, tint32 iBitmapFrame = -1, tint32 iNrOfBitmapFrames = 0);
	//! IPopupMenu implementation
	virtual SMenuItem* CreateAndInsertItem(tint32 iInsertIx, const tchar* pText, tint32 iValue = -1, SMenuItemList* pSubList = NULL, IBitmapResource* pBitmapRes = NULL, tint32 iBitmapFrame = -1, tint32 iNrOfBitmapFrames = 0);
	//! IPopupMenu implementation
	virtual tbool DestroySubMenuBelowItem(tint32 iItemIndex);
	//! IPopupMenu implementation
	virtual IPopupMenu* CreateSubMenuBelowItem(tint32 iItemIndex, SMenuItemList* pSubList = NULL);
	//! IPopupMenu implementation
	virtual IPopupMenu* CreateSubMenuBelowItem(tint32 iItemIndex, tchar* pszEnumList, char cDelimiter, tint32 iFirstItemValue, IBitmapResource* pBitmapRes = NULL, tint32 iNrOfBitmapFrames = 1);
	//! IPopupMenu implementation
	virtual tint32 AppendItems(const SMenuItemList& rList);
	//! IPopupMenu implementation
	virtual tint32 InsertItems(tint32 iInsertIx, const SMenuItemList& rList);
	//! IPopupMenu implementation
	virtual void SetValueUpdatesParentToo(tbool bWillSetParent = true, tbool bApplyRecursively = true);
	// .. Lasse
	virtual void* GetData(tint32 iIndex);

	//! CEventReceiver override
	virtual void EventValueChange(IControl *pSender, tint32 iValueNew);

protected:
	//! Bitmaps used for drawing menu
	IBitmapResource* mppBitmaps[IPopupMenu::BitmapCount];

	//! Bitmap used for drawing an icon to the left of text inside the popup.<br>If there are fewer frames in the bitmap than there are items in the list then only the top part of the pop-up will display an icon
	IBitmapResource* mpBitmapForInlineIcon;
	//! Nr of frames in bitmap for inlien icon
	tint32 miNrOfFramesInInlineIcon;

	//! List of items we're showing
	SMenuItemList mItems;

	//! Array of texts to show
	IText* mppTexts[GE_MAX_NR_OF_POPUP_ITEMS];

	//! Array of inverted texts to show
	IText* mppTextsInv[GE_MAX_NR_OF_POPUP_ITEMS];

	//! Array of submenus (NULL for unused)
	IPopupMenu* mppPopupMenus[GE_MAX_NR_OF_POPUP_ITEMS];

	//! Based on mouse position, determines whether the moust is above one of the items
	/*!
		\param Pos [in]: Mouse position to test for
		\return tint32: Index of item mouse is over, or -1 if none
	*/
	tint32 GetMouseOverItem(SPos Pos);

	//! Calculates the screen position of a given item
	/*!
		\param iItem [in]: Item to return rect for
		\return SRect: Position and size of item
	*/
	SRect GetItemPos(tint32 iItem);

	//! The item we drew as selected last time, or -1 if none
	tint miSelectedLast;

	//! True if a sub-menu is showing, False if this menu is the deepest level opened.
	tbool mbSubMenuShowing;

	//! True if the menu should preferably appear to the right or the left of the position entered in one of the GetBestAbsPos(...) methods. If False (which is default) the prefered positions are below or above.
	tbool mbPositionSidewaysFirst;

	//! True if we had to hide/partally hide the button that showed the pop-up
	tbool mbHadToOverlay;
	//! True if we must disregard the next mouse-up
	tbool mbDisregardFirstMouseUp;
	//! True if we shouldn't update selection until next mouse-move
	tbool mbKeepSelectionUntilFirstMove;
	
	// Lasse, added 2007-12-03 thru -06 - flexibility
	SSize mSizeCenter;
	IFontResource* mpFont;
	IFontResource* mpFontInv;
	IWindow* mpWnd;
	IResourceManager* mpResMan;
	tint32* mpiResIDs;
	tbool mbSetValueUpdatesParentToo;
	tbool mbSetValueUpdatesParentToo_ApplyRecursively;
	
	virtual void SetSizeThis();
	// .. Lasse
};