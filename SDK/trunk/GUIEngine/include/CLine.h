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


/*! \class CLine
 * \brief Implementation of ILine
*/
class CLine : public virtual ILine, public virtual CControl
{
public:
	//! Constructor
	CLine();
	//! Destructor
	virtual ~CLine();

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

	//! ILine override
	virtual void Init(tint32 iID, const SPos& PosStart, const SPos& PosEnd, const SRGB& Colour, IWindow* pParentWindow);
	//! ILine override
	virtual void SetLinePos(const SPos& PosStart, const SPos& PosEnd);
	//! ILine override
	virtual void SetColour(const SRGB& Colour);

protected:
	SPos mPosStart;
	SPos mPosEnd;
	SRGB mColour;

	IDrawPrimitives* mpPrimitives;
};