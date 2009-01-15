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



/*! \class CURL
 * \brief Implementation of IURL
*/
class CURL : public virtual IURL, public virtual CControl
{
public:
	//! Constructor
	CURL();
	//! Destructor
	virtual ~CURL();

	//! IDestructable override
	virtual void Destroy();

	//! IControl override
	virtual void OnDraw(const SRect &rUpdate);
	//! IControl override
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos);
	//! IControl override
	virtual tbool OnKeyDown(EKey Key);

	//! IURL override
	virtual void Init(tint32 iID, const tchar* pszURL);
	//! IURL override
	virtual void ActivateHandCursor();

protected:
	//! URL to call when clicked
	std::string msURL;

	//! If true, the cursor is changed into a pointing hand when the mouse is over the control
	tbool mbShowHand;
};