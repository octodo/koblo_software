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



/*! \class CTextList
 * \brief Implementation of ITextList
*/
class CTextList : public virtual ITextList, public virtual CText
{
public:
	//! Constructor
	CTextList();
	//! Destructor
	virtual ~CTextList();

	//! IDestructable override
	virtual void Destroy();

	//! IText override
	virtual void SetText(const tchar* pszText);
	//! IText override
	virtual tint32 GetValue();
	//! IText override
	virtual void SetValue(tint32 iNew, tbool bNotify = true);
	//! IText override
	virtual void SetConvertedValue(tint32 iNew, tbool bNotify);

	//! ITextList implementation
	virtual void Init(tint32 iID, IFontResource* pFont, const tchar* pszEnumList, tchar cEnumDelimiter = '@');
	//! ITextList implementation
	virtual void Init(tint iID, IFontResource* pFont, const tchar** ppszTextList);

	//! CText override
	virtual tbool OnKeyDown(EKey Key);
	//! CText override
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos);
	//! CText override
	virtual void StartEditing();

	//! CText override
	virtual void StopEditing(tbool bOK);

	//! ITextList implementation
	virtual tint32 SetTextList(const tchar* pszEnumList, tchar cEnumDelimiter = '@');
	//! ITextList implementation
	virtual tint32 SetTextList(const tchar** ppszTextList);

	//! ITextList implementation
	virtual tbool GetText(tint32 iValue, tchar* pszTextOut, tint32 iSizeOfTextOut);


protected:
	//! IText override
//	virtual tbool XlatCurEditingText(tint32& riGUIValue);

	//! Points to the list of texts to display. Contents is never changes
	std::list<std::string> mlistOfValidTexts;
/*	tchar** mppszTextList;
	//! Holds length of list; counted when SetTextList() is called
	tint32 miListLength;*/

	//! When editing and using arrow up and down to scroll through list miEditCurIndex indexes current item
	tint32 miEditCurIndex;

	//! Find match in list of psz. If not found returns -1
	virtual tint32 FindListIndex(const tchar* psz, tchar*& rpszOut);

	//! ITextList implementation
	virtual const tchar* _GetText(tint32 iValue);

};
