/*!	\file IPopupMenu.h
	\author Michael Olsen
	\date 18/Feb/2005
	\date 18/Feb/2005
*/

#define GE_MAX_NR_OF_POPUP_ITEMS 128

/*! \class IPopupMenu
 * \brief A popup menu, is a menu which pops up at a specific point
 *
 * This control is slightly different from other controls, in that it is generated when needed, and destroyed immediately following its use
 * Lasse: Is that quite right? When used in CDropDownListBox it's just SetVisible(false)
 *
 * Lasse, new functionality 2007-12-03 - more flexible creation
 * You may now re-initialize a menu or only a branch of it (sub-menu).
 * You can mix the new and old behaviour:
 * 1) You may add flexible sub-menues below old-style (static) top-levels
 * 2) You may add old-style (static) sub-menus below a new-style flexible top- or sub-menu level
 * 3) You may insert or append new-style (flexible) list items to an old-style (static) menu
 * 4) You can delete items anywhere in a list, even if it's an old-style
*/
class IPopupMenu : public virtual IControl, public virtual IDestructable
{
public:
	//! Create IPopupMenu
	static IPopupMenu* Create();

	// Forward declaration
	struct SMenuItemList;

	//! Struct to describe popup menu item
	struct SMenuItem {
		//! Items text
		tchar pszText[64];
		//! Items value. Ignored for items with submenus
		tint32 iValue;
		//! If non-NULL points to list of sub items, and thus tells us this is a popup item
		SMenuItemList* pSubItemList;
		//! Bitmap to place next to text
		IBitmapResource* pBitmap;
		//! Which frame in bitmap to show
		tint32 iBitmapFrame;
		//! How many frames total in bitmap
		tint32 iNrOfBitmapFrames;


		//! Constructor
		SMenuItem(const tchar* pText,tint32 iValue, SMenuItemList* pList, IBitmapResource* pBitmap = NULL, tint32 iBitmapFrame = -1, tint32 iNrOfBitmapFrames = 0) {
			strcpy((char*)pszText, (const char*)pText);
			this->iValue = iValue;
			pSubItemList = pList;
			this->pBitmap = pBitmap;
			this->iBitmapFrame = iBitmapFrame;
			this->iNrOfBitmapFrames = iNrOfBitmapFrames;
			
			
			// Lasse, added 2007-12-03 - more flexibility
			// Allow for correct ressource handling when bitmaps differ between items
			if (pBitmap) {
				pBitmap->IncRefCount();
			}
			// .. Lasse
		}

		//! Constructor
		SMenuItem() {
			pszText[0] = 0;
			iValue = 0;
			pSubItemList = NULL;
			pBitmap = NULL;
			iBitmapFrame = -1;
			iNrOfBitmapFrames = 0;
		}
		
		// Lasse, added 2007-12-03 - more flexibility
		//! Destructor
		virtual ~SMenuItem() {
			// De-ref bitmaps upon destroy
			if (this->pBitmap) {
				this->pBitmap->DecRefCount();
			}
		}
		// .. Lasse
	};

	//! Struct to describe list of popup menu items
	struct SMenuItemList {
		//! Number of items
		tint iItemCount;
		//! Array of items (128 max)
		SMenuItem pItems[GE_MAX_NR_OF_POPUP_ITEMS];
	};

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param pWnd [in]: Window control resides in
		\param pResMan [in]: Resource manager
		\param piIDs [in]: Array of IDs of bitmap resources to use. See EBitmapIndices
		\param pFont [in]: Font to use (will be destroyed by popup). Must be NULL if there's no items in list
		\param pFontInv [in]: Font to use when text is inverted (selected) (will be destroyed by popup). Must be NULL if there's no items in list
		\param Items [in]: Items in popup. Leave NULL if you'll be adding items dynamically afterwards (new-style)
	*/
	virtual void Init(tint32 iID, IWindow* pWnd, IResourceManager* pResMan, tint32* piIDs, IFontResource* pFont, IFontResource* pFontInv, const SMenuItemList& Items = *((SMenuItemList*)NULL)) = 0;
	
	//! Enum for bitmap indices. All bitmaps must have same width
	enum EBitmapIndices {
		//! Arrow used for folders
		BitmapFolderArrow = 0,
		//! Top line
		BitmapTop,
		//! Center line, i.e. background for 1 string
		BitmapCenter,
		//! Center line, i.e. background for 1 string, when inverted (selected). Must be same size as BitmapCenter
		BitmapCenterInv,
		//! Bottom line
		BitmapBottom,
		//! Nr of bitmaps
		BitmapCount
	};

	//! Returns the text for a specific value
	/*!
		\param iIndex [in]: Value of item to return text for
		\param psz [out]: Returned text
	*/
	virtual void GetText(tint32 iValue, tchar* psz) = 0;

	//! Sets a bitmap used for drawing an icon to the left of text inside the popup.<br>If there are fewer frames in the bitmap than there are items in the list then only the top part of the pop-up will display an icon.
	/*!
		\param pBitmap [in]: Bitmap resource to use
		\param iFrames [in]: Frames in bitmap.
	*/
	virtual void SetInlineIconBitmap(IBitmapResource* pBitmap, tint32 iFrames) = 0;


	//! Verifies a wanted position against the size of the parent pane
	/*!
		\param pControlThatInitiated [in]: The control (probably a button) that initiated the pop-up. We will place ourselves in the vicinity of that control (preferably just below it) while trying to avoid hiding it
		\return SPos: The best bid for an absolute position
	*/
	virtual SPos GetBestAbsPos(IControl* pControlThatInitiated) = 0;

	//! Verifies a wanted absolute position against the size of the parent pane
	/*!
		\param posAbsoluteWanted [in]: The absolute position (relative to client window) that would be best if there were no physical constraints (typically the position of a mouse click, or immediately below a button)
		\param sizeOfObstructiveObject [in]: The size of a control that's "in the way" and which we should try not to hide. This could i.e. be a button that initiated the popup.<br>If no control initiated the pop-up, but rather a mouse click to a certain position, it is best to have a size of 1x1, since this will prevent the releasing of the mouse button from immediately closing the pop-up.
		\return SPos: The best bid for an absolute position
	*/
	virtual SPos GetBestAbsPos(SPos posAbsoluteWanted, SSize sizeOfObstructiveObject = SSize(1,0) ) = 0;

	//! Influences the order in which the GetBestAbsPos(...) methods will look for a suitable position.
	/*!
		\param bSideways [in]: True = the GetBestAbsPos(...) methods will first try to place the menu to the right or left. False = they will try below or above first.
	*/
	virtual void PositionSidewaysFirst(tbool bSideways = true) = 0;


	// Lasse, added 2007-12-03 thru -06 - flexibility
	
	//! Delete menu items (and any sub-menus/-items attached) to prepare for adding new ones
	/*!
	 \param iStartIx [in]: Index at which to start deletion (0 for all)
	 \param iCount [in]: Number of items to delete (-1 for all after iStartIx)
	 \return tint32: The number of items actually deteled. -1 upon error
	*/
	virtual tint32 DestroyItems(tint32 iStartIx = 0, tint32 iCount = -1) = 0;
	
	//! Append a new item to the end of the menu.<br><b>Don't call this for a visible menu!</b>
	/*!
		\param pText [in]: The text to display for the item (line) of menu
		\param iValue [in]: The value to associate with the item. This is the value assigned to the whole popup-menu when the item is clicked. Leave -1 for same as index (only use this feature for top-level menu!)
		\param pSubList [in]: This is an already prepared old-style (static) sub-menu to be inserted below the menu item we're about to create (probably rarely used feature). Leave NULL if not applicable
		\param pBitmapRes [in]: NULL or pointer to an already loaded bitmap ressource to display to the left of text. You must either add a bitmap for all or none of the items in the list. All added bitmaps must have be the same proportions.
		\param iBitmapFrame [in]: Indicates which frame to use in a multi-frame bitmap. Leave at -1 if not applicable. Must be used in conjunction with iNrOfBitmapFrames.
		\param iNrOfBitmapFrames [in]: The number of frames in the multi-frame bitmap ressource. Leave at 0 if not applicable.
		\return SMenuItem*: The newly created sub-menu item. You may discard this pointer.
	*/
	virtual SMenuItem* CreateAndAppendItem(const tchar* pText, tint32 iValue = -1, SMenuItemList* pSubList = NULL, IBitmapResource* pBitmapRes = NULL, tint32 iBitmapFrame = -1, tint32 iNrOfBitmapFrames = 0) = 0;

	//! Insert a new item somewhere in the menu. Will cause the items below to "bubble down", so use the CreateAndAppendItem method instead when possible.<br><b>Don't call this for a visible menu!</b>
	/*!
		\param iInsertIx [in]: The index at which to insert the new item. An index of -1 will append to the end of list
		\param pText [in]: The text to display for the item (line) of menu
		\param iValue [in]: The value to associate with the item. This is the value assigned to the whole popup-menu when the item is clicked. Leave -1 for same as index (only use this feature for top-level menu!)
		\param pSubList [in]: This is an already prepared old-style (static) sub-menu to be inserted below the menu item we're about to create (probably rarely used feature). Leave NULL if not applicable
		\param pBitmapRes [in]: NULL or pointer to an already loaded bitmap ressource to display to the left of text. You must either add a bitmap for all or none of the items in the list. All added bitmaps must have be the same proportions.
		\param iBitmapFrame [in]: Indicates which frame to use in a multi-frame bitmap. Leave at -1 if not applicable. Must be used in conjunction with iNrOfBitmapFrames.
		\param iNrOfBitmapFrames [in]: The number of frames in the multi-frame bitmap ressource. Leave at 0 if not applicable.
		\return SMenuItem*: The newly created sub-menu item. You may discard this pointer. Will be NULL upon error (i.e. if the index is > number of items)
	*/
	virtual SMenuItem* CreateAndInsertItem(tint32 iInsertIx, const tchar* pText, tint32 iValue, SMenuItemList* pSubList = NULL, IBitmapResource* pBitmapRes = NULL, tint32 iBitmapFrame = -1, tint32 iNrOfBitmapFrames = 0) = 0;
	
	//! Add an existing list of items to the end of the list.<br><b>Don't call this for a visible menu!</b>
	/*!
	 \param rList [in]: List of items (can have sub-menues as well) to add to the menu
	 \return tint32: Count of items added or -1 on error
	 */
	virtual tint32 AppendItems(const SMenuItemList& rList) = 0;
	
	//! Add an existing list of items to the end of the list.<br><b>Don't call this for a visible menu!</b>
	/*!
	 \param iInsertIx [in]: The index at which to insert the items. An index of -1 will append to the end of list
	 \param rList [in]: List of items (can have sub-menues as well) to add to the menu
	 \return tint32: Count of items added or -1 on error
	 */
	virtual tint32 InsertItems(tint32 iInsertIx, const SMenuItemList& rList) = 0;
	
	//! Remove any sub-menu for the item of a given index item.<br><b>Don't call this for a visible sub-menu!</b>
	/*!
	 \param iItemIndex [in]: The index of the item in the "this" menu for which to remove all subitems.
	 \return tbool: True upon success. False if index is out of range (i.e. below 0 or > number of items)
	 */
	virtual tbool DestroySubMenuBelowItem(tint32 iItemIndex) = 0;
	
	//! Create a (default empty) sub-menu below a given item index. If a sub-menu already exists it is destroyed first.<br><b>Don't call this for a visible sub-menu!</b>
	/*!
	 \param iItemIndex [in]: The index of the item in the "this" menu for which to create a sub-menu
	 \param pSubList [in]: Initialize the new sub-menu with these static (old-style) items
	 \return IPopupMenu*: The newly created sub-menu upon success. NULL if index is out of range (i.e. below 0 or > number of items)
	 */
	virtual IPopupMenu* CreateSubMenuBelowItem(tint32 iItemIndex, SMenuItemList* pSubList = NULL) = 0;

	//! Create an old-style (static) sub-menu below a given item index. If a sub-menu already exists it is destroyed first.<br><b>Don't call this for a visible sub-menu!</b>
	/*!
	 \param iItemIndex [in]: The index of the item in the "this" menu for which to create a sub-menu
	 \param pszEnumList [in]: Initialize the new sub-menu with these static (old-style) items
	 \param cDelimiter [in]: Use this char to split the items from the pszEnumList
	 \param iFirstItemValue [in]: The first item in the new sub-menu will have this value. The following will have increasing values
	 \param pBitmapRes [in]: NULL or pointer to an already loaded bitmap ressource whose frames shall display to the left of texts
	 \param iNrOfBitmapFrames [in]: The number of frames in the multi-frame bitmap ressource. Frames will be assign to items in increasing order (if there are too few frames the lowest ones just won't display any icon)
	 \return IPopupMenu*: The newly created sub-menu upon success. NULL if index is out of range (i.e. below 0 or > number of items)
	 */
	virtual IPopupMenu* CreateSubMenuBelowItem(tint32 iItemIndex, tchar* pszEnumList, char cDelimiter, tint32 iFirstItemValue, IBitmapResource* pBitmapRes = NULL, tint32 iNrOfBitmapFrames = 1) = 0;
	
	//! Controls whether a SetValue operation on a child menu will update the above-level menus as well (reason: the top-level menu is often the only one that has a parameter-id attached).<br>If the pop-up is owned by an IDropDownListBox the value of that can be autmoatically updated as well (since that is usually what's attached to a paramID)
	/*!
	 \param bWillSetParent [in]: True => parent menu is affected whenever a SetValue operation is done for this menu. False => the SetValue affects this menu only
	 \param bApplyRecursively [in]: True => the behaviour is applied recursively upon all sub-menus of the "this" menu; they may be changed individually afterwards. False => behaviour is changed for this menu only
	 */
	virtual void SetValueUpdatesParentToo(tbool bWillSetParent = true, tbool bApplyRecursively = true) = 0;
	
	// .. Lasse
	
};