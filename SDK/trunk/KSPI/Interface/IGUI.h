// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
//
// This file is part of the Koblo Stools.
//
// the Koblo Stools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo Stools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.



/*! \brief Interface for a GUI unit, attached to 1 plug-in
*
*	Each plug-in has 1 GUI.
*/
class /*KSPI_EXPORT*/ IGUI
{
public:
	//! Destroys GUI. Use this instead of delete, since it will make sure deallocation is done the right place
	virtual void KSPI_CALL Destroy() = 0;

	//! Returns the size of the plug-in window
	virtual void KSPI_CALL GetSize(tuint32* piSizeX, tuint32* piSizeY) = 0;

	//! Create a window. p is system specific. Windows: HWND to parent window
	virtual void KSPI_CALL MakeWindow(void* p) = 0;

	virtual void KSPI_CALL PrepareBuffer() = 0;
	
	//! Invalidate the window
	virtual void KSPI_CALL Paint() = 0;

	//! Notifies a mouse down event (Mac only)
	virtual void KSPI_CALL OnMouseDown(tuint32 iPosX, tuint32 iPosY) = 0;

	//! Notifies a key down event (Mac only)
	virtual void KSPI_CALL OnKeyDown(tuint32 iKeyCode) = 0;

	//! Timer call. (Mac only)
	virtual void KSPI_CALL OnTimer() = 0;

	virtual void KSPI_CALL OnDraw(tuchar* puchBuffer, tuint32 uiInterleave, tint32 iUpdateX, tint32 iUpdateY, tint32 iUpdateCX, tint32 iUpdateCY) = 0;

	virtual void KSPI_CALL OnMouseLeftButtonDown(tint32 iPosX, tint32 iPosY) = 0;

	virtual void KSPI_CALL OnMouseLeftButtonUp(tint32 iPosX, tint32 iPosY) = 0;

	virtual void KSPI_CALL OnMouseMove(tint32 iPosX, tint32 iPosY) = 0;

	virtual void KSPI_CALL OnMouseRightButtonDown(tint32 iPosX, tint32 iPosY) = 0;

	virtual void KSPI_CALL OnMouseRightButtonUp(tint32 iPosX, tint32 iPosY) = 0;

	virtual void KSPI_CALL OnMouseLeftButtonDoubleClick(tint32 iPosX, tint32 iPosY) = 0;
};

