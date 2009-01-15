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


/*! \class CBrowser
 * \brief Implementation of IBrowser
*/
class CBrowser : public virtual IBrowser, public virtual CControl
{
public:
	//! Constructor
	CBrowser();
	//! Destructor
	virtual ~CBrowser();

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

	//! CControl override
	virtual void SetSize(const SSize& Size);

	//! IBrowser override
	virtual void Init(tint iID, IWindow* pWnd, IFontResource* pFont, IBitmapResource* pBmp);
	//! IBrowser override
	virtual void SetCallback(IBrowserCallback* pCallback);
	//! IBrowser override
	virtual void SetScrollPane(IScrollPane* pPane);
	//! IBrowser override
	virtual void SetScrollBar(IScrollBar* pScrollBar);
	//! IBrowser override
	virtual void Update();
	//! IBrowser override
	virtual void GetSelectedItem(tchar* pszPathName);

protected:
	//! The font we're using
	IFontResource* mpFont;

	//! The bitmap (arrow) we're using
	IBitmapResource* mpArrow;

	//! The browser callback
	IBrowserCallback* mpCallback;

	//! Drawing primitives
	IDrawPrimitives* mpPrimitives;

	//! Scroll pane we're connected to
	IScrollPane* mpScrollPane;

	//! Scroll bar we're connected to
	IScrollBar* mpScrollBar;

	//! Struct used to keep track of items we're browsing
	struct SItem : public virtual IBrowser::SItem
	{
		//! Named modified to have the proper length
		std::string sNameModified;

		//! Full path name (from root), using \ as seperator
		std::string sPathName;

		//! Sub item list for folders
		std::list<SItem*> SubItems;

		//! If folder is open true, otherwise false. Ignored if item is not folder
		tbool bFolderIsOpen;

		//! Destructor
		virtual ~SItem();
	};

	//! Root folder item
	SItem mItemRoot;

	//! Max depth (folders)
	tint miMaxDepth;

	//! Space (y) between each item
	tint miSpaceBetweenItems;

	//! Left-most margin
	tint miMarginLeft;

	//! Top-most margin
	tint miMarginTop;

	//! Space (x) for each new folder
	tint miSpaceBetweenFolders;

	//! Position of text compared to arrow
	SPos mTextPosition;

	//! Minimum size (equals original size set)
	SSize mSizeMin;

	//! Currently selected item, or NULL
	SItem* mpItemSelected;

	//! Get items to draw for specific list
	/*!
		\param Items [in]: Items to count for
		\param rLineCount [in/out]: Must be incremented with 1 for each line to draw drawn
	*/
	void GetItemCount(const std::list<SItem*>& Items, tint32& riLineCount);

	//! Draws items at specific position
	/*!
		\param RectUpdate [in]: Update rectangle
		\param Items [in]: Items to draw
		\param Pos [in/out]: Position on screen to start drawing. Will be updated to reflect new y position (not x!)
	*/
	void DrawItems(const SRect RectUpdate, const std::list<SItem*>& Items, SPos& Pos);

	//! Find item at specific position
	/*!
		\param Pos [in]: Position to test for
		\return SItem*: Item at position, or NULL if none
	*/
	SItem* GetItem(const SPos& Pos);

	//! Recursive version of GetItem, used to recursively iterate the items
	/*!
		\param Pos [in/out]: Position to test for. Will be updated to reflect position for next item after entering a folder
		\param Items [in]: Items to test
		\return SItem*: Item at position, or NULL if none
	*/
	SItem* GetItemRecur(SPos& Pos, const std::list<SItem*>& Items);

	//! Fills info about a folder
	/*!
		\param pFolder [in/out]: Folder to fill info for
	*/
	void BrowseFolder(SItem* pFolder);

	//! Updatea a folder
	/*!
		\param pFolder [in/out]: Folder to update
	*/
	void UpdateFolder(SItem* pFolder);
};

