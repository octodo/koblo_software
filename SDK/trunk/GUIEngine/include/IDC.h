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



/*! \class IDC
 * \brief Non-public interface which describes a bitmap device context
*/
class IDC : public virtual IDestructable
{
public:
	//! Create IDC
	static IDC* Create();

	//! Call to initialize
	/*!
		\param pWind [in]: Window which resource belongs to
		\param pRes [in]: Bitmap resource for which to init device context
		\param iFrameIndex [in]: Index of frame (in pixels) of source bitmap
		\param iVariantIndex [in]: Index of variant (in pixels) of source bitmap
	*/
	virtual void Init(IWindow* pWnd, IBitmapResource* pRes, tint iFrameIndex, tint iVariantIndex) = 0;

	//! Call to draw bitmap
	/*!
		\param pWnd [in]: Window where to draw
		\param rctUpdate [in]: Current update rectangle
		\param rctDraw [in]: Rectangle with target position and size
	*/
	virtual void Draw(IWindow* pWnd, const SRect& rctUpdate, const SRect& rctDraw) = 0;
};












