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


/*! \class CDrawPrimitivesGDI
 * \brief GDI Implementation of IDrawPrimitives
*/
class CDrawPrimitivesGDI : public virtual IDrawPrimitives
{
public:
	//! Constructor
	CDrawPrimitivesGDI();
	//! Destructor
	virtual ~CDrawPrimitivesGDI();

	//! IDestructable override
	virtual void Destroy();

	//! IDrawPrimitives override
	virtual void Init(IWindow* pWnd);
	//! IDrawPrimitives override
	virtual void DrawPoint(const SRect& RectUpdate, const SPos& Pos, const SRGB& Colour);
	//! IDrawPrimitives override
	virtual void DrawLine(const SRect& RectUpdate, const SPos& PosFrom, const SPos& PosTo, const SRGB& Colour);
	//! IDrawPrimitives override
	virtual void DrawRect(const SRect& RectUpdate, const SRect& RectFill, const SRGB& Colour);
	//! IDrawPrimitives override
	virtual void InvertRect(const SRect& RectUpdate, const SRect& RectInvert);

protected:
	//! Cached pointer to window
	IWindow* mpWnd;
};