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


/*! \class CWindowWinDX
 * \brief DirectX specific implementation of IWindow
*/
class CWindowWinDX : public virtual CWindowWin
{
public:
	//! Constructor
	CWindowWinDX();
	//! Destructor
	virtual ~CWindowWinDX();

	//! IDestructable override
	virtual void Destroy();

	//! IWindow override
	virtual void MakeWindow(void* pParent, const SRect* pRect);
	//! IWindow override
	virtual void GetHDC(void** pp);
	//! IWindow override
	virtual void GetDrawPointer(void** pp);

protected:
	//! Drawing message action
	virtual void OnPaint();

	//! CWindow override
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos);

	//! Call to initialize DirectX
	virtual void DirectXInit();
	//! Base DirectDraw object.
	IDirectDraw3* mpDirectDraw;
	//! DirectDraw factory
	IDirectDrawFactory* mpDirectDrawFactory;
	//! DirectDraw front surface
	IDirectDrawSurface* mpDirectDrawSurface;
	//! DirectDraw back surface
	IDirectDrawSurface* mpDirectDrawSurfaceB;
};



