/*!	\file CBrowser.cpp
	\author Michael Olsen
	\date 29/Mar/2005
	\date 29/Mar/2005
*/


#include "geInternalOS.h"

//! Constant to determine how many items per folder we can handle
const tint32 giBrowserMaxItemsPerFolder = 16384;

/*
	CBrowser::SItem
*/

CBrowser::SItem::~SItem() {
	std::list<SItem*>::iterator it = SubItems.begin();
	for (; it != SubItems.end(); it++) {
		SItem* pItem = *it;
		delete pItem;
	}
}

/*
	CBrowser
*/

CBrowser::CBrowser()
	: mpFont(NULL), mpArrow(NULL), mpPrimitives(NULL), mpScrollPane(NULL), mpScrollBar(NULL), mSizeMin(0, 0),
	mpItemSelected(NULL)
{
	mItemRoot.bFolder = true;

	miMaxDepth = 3;
	miSpaceBetweenItems = 16;
	miMarginLeft = 10;
	miMarginTop = 4;
	miSpaceBetweenFolders = 16;
	mTextPosition = SPos(20, 0);
}

CBrowser::~CBrowser()
{
	if (mpFont) {
		mpFont->DecRefCount();
	}
	if (mpArrow) {
		mpArrow->DecRefCount();
	}
	if (mpPrimitives) {
		mpPrimitives->Destroy();
	}
	if (mpCallback) {
		mpCallback->Destroy();
	}
}

IBrowser* IBrowser::Create()
{
	return dynamic_cast<IBrowser*>(new CBrowser());
}

void CBrowser::Destroy()
{
	delete dynamic_cast<CBrowser*>(this);
}

void CBrowser::Init(tint32 iID, IWindow* pWnd, IFontResource* pFont, IBitmapResource* pBmp)
{
	CControl::Init(iID, NULL);

	mpFont = pFont;
	mpArrow = pBmp;

	mpPrimitives = IDrawPrimitives::Create();
	mpPrimitives->Init(pWnd);
}

void CBrowser::SetSize(const SSize& Size)
{
	CControl::SetSize(Size);

	if (mSizeMin == SSize(0, 0)) {
		mSizeMin = GetSize();
	}
}

void CBrowser::SetScrollPane(IScrollPane* pPane)
{
	mpScrollPane = pPane;
}

void CBrowser::SetScrollBar(IScrollBar* pScrollBar)
{
	mpScrollBar = pScrollBar;
}

void CBrowser::BrowseFolder(SItem* pFolder)
{
	// Clear current list
	std::list<SItem*>::iterator it = pFolder->SubItems.begin();
	for (; it != pFolder->SubItems.end(); it++) {
		SItem* pItem = *it;
		delete pItem;
	}
	pFolder->SubItems.clear();

	tint32 iItemCount = giBrowserMaxItemsPerFolder;
	IBrowser::SItem* pItems = new IBrowser::SItem[iItemCount];

	std::string sPathName(pFolder->sPathName);
	if (sPathName != std::string("")) {
#ifdef WIN32
		sPathName += std::string("\\");
#endif
#ifdef _Mac
		sPathName += std::string(":");
#endif
	}
	mpCallback->Browse((tchar*)(sPathName.c_str()), pItems, iItemCount);

	// Convert to internal structure
	tint32 iItem;
	for (iItem = 0; iItem < iItemCount; iItem++) {
		CBrowser::SItem* pItemInternal = new CBrowser::SItem();

		strcpy((char*)(pItemInternal->pszName), (char*)(pItems[iItem].pszName));
		pItemInternal->bFolder = pItems[iItem].bFolder;

		pItemInternal->sNameModified = std::string((char*)(pItemInternal->pszName));
		pItemInternal->sPathName = pFolder->sPathName;
#ifdef WIN32
		pItemInternal->sPathName += std::string("\\");
#endif
#ifdef _Mac
		pItemInternal->sPathName += std::string(":");
#endif
		pItemInternal->sPathName += std::string((char*)(pItemInternal->pszName));
		pItemInternal->bFolderIsOpen = false;

		pFolder->SubItems.push_back(pItemInternal);
	}

	delete[] pItems;

	// Get number of lines
	tint32 iLineCount = 0;
	GetItemCount(mItemRoot.SubItems, iLineCount);

	// We now know the amount of lines, and can set the size
	SSize SizeNew(GetSize());
	SizeNew.iCY = miMarginTop + (iLineCount * miSpaceBetweenItems);
	if (SizeNew.iCY < mSizeMin.iCY) {
		SizeNew.iCY = mSizeMin.iCY;
	}
	if (SizeNew != GetSize()) {
		SetSize(SizeNew);

		SScrollPos ScrollPos;
		mpScrollPane->GetScrollPos(ScrollPos);
		ScrollPos.AreaSize = SizeNew;
		mpScrollBar->SetScrollPos(ScrollPos, false);
	}
}

void CBrowser::Update()
{
	UpdateFolder(&mItemRoot);

	// Get number of lines
	tint32 iLineCount = 0;
	GetItemCount(mItemRoot.SubItems, iLineCount);

	// We now know the amount of lines, and can set the size
	SSize SizeNew(GetSize());
	SizeNew.iCY = miMarginTop + (iLineCount * miSpaceBetweenItems);
	if (SizeNew.iCY < mSizeMin.iCY) {
		SizeNew.iCY = mSizeMin.iCY;
	}
	if (SizeNew != GetSize()) {
		SetSize(SizeNew);

		SScrollPos ScrollPos;
		mpScrollPane->GetScrollPos(ScrollPos);
		ScrollPos.AreaSize = SizeNew;
		mpScrollBar->SetScrollPos(ScrollPos, true);
	}

	Redraw();
}

void CBrowser::UpdateFolder(SItem* pFolder)
{
	SItem* pFolderTmp = new SItem;

	tint32 iItemCount = giBrowserMaxItemsPerFolder;
	IBrowser::SItem* pItems = new IBrowser::SItem[iItemCount];

	std::string sPathName(pFolder->sPathName);
	if (sPathName != std::string("")) {
#ifdef WIN32
		sPathName += std::string("\\");
#endif
#ifdef _Mac
		sPathName += std::string(":");
#endif
	}
	mpCallback->Browse((tchar*)(sPathName.c_str()), pItems, iItemCount);

	// Convert to internal structure
	tint32 iItem;
	for (iItem = 0; iItem < iItemCount; iItem++) {
		CBrowser::SItem* pItemInternal = new CBrowser::SItem();

		strcpy((char*)(pItemInternal->pszName), (char*)(pItems[iItem].pszName));
		pItemInternal->bFolder = pItems[iItem].bFolder;

		pItemInternal->sNameModified = std::string((char*)(pItemInternal->pszName));
		pItemInternal->sPathName = pFolder->sPathName;
#ifdef WIN32
		pItemInternal->sPathName += std::string("\\");
#endif
#ifdef _Mac
		pItemInternal->sPathName += std::string(":");
#endif
		pItemInternal->sPathName += std::string((char*)(pItemInternal->pszName));
		pItemInternal->bFolderIsOpen = false;

		pFolderTmp->SubItems.push_back(pItemInternal);

		if (pItemInternal->bFolder) {
			// Check if folder previously was open
		}
	}

	delete[] pItems;

	// Go through the old items, deleting the ones missing, and updating the sub folders
	// These are the items we wish to delete from list. We'll do it in the end, so we don't mess up the iterator
	std::list<SItem*> DeleteList;
	tint32 iNrOfItems = pFolder->SubItems.size();
	std::list<SItem*>::iterator it = pFolder->SubItems.begin();
	for (iItem = 0; iItem < iNrOfItems; iItem++, it++) {
		SItem* pItem = *it;
		std::string sName = std::string((char*)pItem->pszName);

		// See if we can find this item in the new list
		std::list<SItem*>::iterator it2 = pFolderTmp->SubItems.begin();
		bool bFoundIt = false;
		for (; it2 != pFolderTmp->SubItems.end(); it2++) {
			SItem* pItemNew = *it2;
			std::string sNameNew = std::string((char*)pItemNew->pszName);
			if (sNameNew.compare(sName) == 0) {
				// The names match
				bFoundIt = true;
				// Update the new list
				delete pItemNew;
				pFolderTmp->SubItems.erase(it2);
				break;
			}
		}
		if (bFoundIt) {
			if (pItem->bFolder) {
				if (pItem->bFolderIsOpen) {
					UpdateFolder(pItem);
				}
			}
		}
		else {
			// We didn't find it, so put it in list of items to delete
			DeleteList.push_back(pItem);
		}
	}

	// Delete the items, which needs to be deleted
	it = DeleteList.begin();
	for (; it != DeleteList.end(); it++) {
		SItem* pItem = *it;
		delete pItem;
		pFolder->SubItems.remove(pItem);
	}

	// All remaining items in the new list needs to be put in the old list
	it = pFolderTmp->SubItems.begin();
	for (; it != pFolderTmp->SubItems.end(); it++) {
		SItem* pItem = *it;
		pFolder->SubItems.push_back(pItem);
	}
	// Clear the new list, since we don't want the items to be deleted (they've been transfered to the old list)
	pFolderTmp->SubItems.clear();

	delete pFolderTmp;
}

void CBrowser::SetCallback(IBrowserCallback* pCallback)
{
	mpCallback = pCallback;

	// Do the initial browsing
	mItemRoot.sPathName = std::string("");
	BrowseFolder(&mItemRoot);
}

void CBrowser::GetSelectedItem(tchar* pszPathName)
{
	if (mpItemSelected) {
		std::string s = mpItemSelected->sPathName.substr(1);
		strcpy((char*)pszPathName, s.c_str());
	}
	else {
		pszPathName[0] = 0;
	}
}

void CBrowser::OnDraw(const SRect &rUpdate)
{
	if( !IsVisible() )
		return;

	mpPrimitives->DrawRect(rUpdate, GetRect(), SRGB(192, 192, 192));

	SPos Pos(GetPos() + SPos(miMarginLeft, miMarginTop));
	DrawItems(rUpdate, mItemRoot.SubItems, Pos);
}

void CBrowser::GetItemCount(const std::list<SItem*>& Items, tint32& riLineCount)
{
	tint32 iNrOfItems = Items.size();
	std::list<SItem*>::const_iterator it = Items.begin();
	tint32 iItem;
	for (iItem = 0; iItem < iNrOfItems; iItem++, it++) {
		SItem* pItem = *it;

		if (pItem->bFolder) {
			// Item is folder

			if (pItem->bFolderIsOpen) {
				GetItemCount(pItem->SubItems, riLineCount);
			}
		}
	}

	riLineCount += iNrOfItems;
}

CBrowser::SItem* CBrowser::GetItem(const SPos& Pos)
{
	SPos PosCur(Pos);

	PosCur -= GetPos();

	PosCur -= SPos(miMarginLeft, miMarginTop);
	if (PosCur.iX < 0 || PosCur.iY < 0) {
		return NULL;
	}

	return GetItemRecur(PosCur, mItemRoot.SubItems);
}

CBrowser::SItem* CBrowser::GetItemRecur(SPos& Pos, const std::list<SItem*>& Items)
{
	tint32 iNrOfItems = Items.size();
	std::list<SItem*>::const_iterator it = Items.begin();
	tint32 iItem;
	for (iItem = 0; iItem < iNrOfItems; iItem++, it++) {
		SItem* pItem = *it;

		if (Pos.iY <= (miSpaceBetweenItems - 1)) {
			return pItem;
		}

		Pos.iY -= miSpaceBetweenItems;

		if (pItem->bFolder) {
			// Item is folder

			if (pItem->bFolderIsOpen) {
				SItem* pRet = GetItemRecur(Pos, pItem->SubItems);
				if (pRet) {
					// Item found in sub folder
					return pRet;
				}
			}
		}
	}

	return NULL;
}

void CBrowser::DrawItems(const SRect RectUpdate, const std::list<SItem*>& Items, SPos& Pos)
{
	SSize SizeBitmap;
	mpArrow->GetRealSize(SizeBitmap);

	tint32 iNrOfItems = Items.size();
	std::list<SItem*>::const_iterator it = Items.begin();
	tint32 iItem;
	for (iItem = 0; iItem < iNrOfItems; iItem++, it++) {
		SItem* pItem = *it;

		if (pItem->bFolder) {
			// Item is folder
			DrawBitmapFrame(mpArrow, RectUpdate, SRect(Pos, SizeBitmap),
				pItem->bFolderIsOpen ? 1 : 0,
				2);
		}

		SPos PosText = Pos + mTextPosition;
		SSize SizeText;
		SizeText.iCY = mpFont->GetTextHeight();
		SizeText.iCX = mpFont->GetTextWidth((tchar*)(pItem->sNameModified.c_str()));
		DrawFont(mpFont,
			RectUpdate,
			SRect(PosText, SizeText),
			(const tchar*)(pItem->sNameModified.c_str()));

		if (pItem == mpItemSelected) {
			// This is the selected item
			SRect RectInvert(GetPos(), SizeText);
			RectInvert.iY = PosText.iY - 1;
			RectInvert.iCY += 1;
			RectInvert.iCX = GetSize().iCX;
			mpPrimitives->InvertRect(RectUpdate, RectInvert);
		}

		Pos.iY += miSpaceBetweenItems;

		if (pItem->bFolder) {
			if (pItem->bFolderIsOpen) {
				tint iX = Pos.iX;
				Pos.iX += miSpaceBetweenFolders;
				DrawItems(RectUpdate, pItem->SubItems, Pos);
				Pos.iX = iX;
			}
		}
	}
}

tbool CBrowser::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	SRect RectThis(GetRect());

	if (MouseMsg == LeftButtonDown) {
		// Left button down
		if (RectThis.Inside(Pos)) {
			// Click was inside control
			SItem* pItem = GetItem(Pos);
			if (pItem == NULL) {
				// No item clicked
				return true;
			}

			if (pItem->bFolder) {
				// Item is a folder
				if (pItem->bFolderIsOpen) {
					// Folder is open, close it
					pItem->bFolderIsOpen = false;
				}
				else {
					// Folder is closed, open it
					pItem->bFolderIsOpen = true;
				}

				BrowseFolder(pItem);

				Redraw();

				return true;
			}
			else {
				// Not folder
				mpItemSelected = pItem;

				std::string s(mpItemSelected->sPathName.substr(1));
				tint32 pData[2];
				pData[0] = 0;
				pData[1] = (tint32)(s.c_str());
				NotifyGeneric((void*)pData);
				NotifyGeneric((void*)(s.c_str()));

				return true;
			}
		}
	}

	if (MouseMsg == LeftButtonDblClk) {
		// Left button down
		if (RectThis.Inside(Pos)) {
			// Click was inside control
			SItem* pItem = GetItem(Pos);
			if (pItem == NULL) {
				// No item clicked
				return true;
			}

			if (pItem->bFolder == false) {
				// Not folder
				mpItemSelected = pItem;

				std::string s(mpItemSelected->sPathName.substr(1));
				tint32 pData[2];
				pData[0] = 1;
				pData[1] = (tint32)(s.c_str());
				NotifyGeneric((void*)pData);

				return true;
			}
		}
	}

	return false;
}

tbool CBrowser::OnKeyDown(EKey /*Key*/)
{
	return false;
}

void CBrowser::OnTimer()
{
	CControl::OnTimer();
}

