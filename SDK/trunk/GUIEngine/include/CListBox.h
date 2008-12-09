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


/*! \class CListBox
 * \brief Implementation of IListBox
*/
class CListBox : public virtual IListBox, public virtual CControl
{
public:
	//! Constructor
	CListBox();
	//! Destructor
	virtual ~CListBox();

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

	//! IListBox implementation
	virtual void Init(tint32 iID, IFontResource* pFont);
	//! IListBox implementation
	virtual void SetText(const tchar* pszEnumList, tchar cEnumDelimiter = '@');
	//! IListBox implementation
	virtual void EnableExtraMouseEvent(EMouseMsg eMsg = LeftButtonDblClk, tint32 iValueToSet = -1);

protected:
	std::list<std::string> mTexts;

	tbool mbDragging;

	tint32 miDragIndex;

	IDrawPrimitives* mpDrawPrimitives;

	tbool mabExtraMouseEvents[NrOfMouseMessages];
	tint32 maiExtraReturnValues[NrOfMouseMessages];
};
