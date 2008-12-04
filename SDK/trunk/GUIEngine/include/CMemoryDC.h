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


/*! \class CMemoryDC
 * \brief Partial implementation of IDC for contexts which resides in memory
*/
class CMemoryDC : public virtual IDC
{
public:
	//! Constructor
	CMemoryDC();
	//! Destructor
	virtual ~CMemoryDC();

	//! IMemoryDC override
	virtual void Init(IWindow* pWnd, IBitmapResource* pRes, tint iFrameIndex, tint iVariantIndex);

protected:
	//! Pointer to bitmap resource we're drawing
	IBitmapResource* mpBitmapRes;
	//! Index of frame (in pixels) of source bitmap
	tint miFrameIndex;
	//! Index of variant (in pixels) of source bitmap
	tint miVariantIndex;
	//! Size of bitmap resource we're drawing ("Real size", i.e. size for 1 frame)
	SSize mSize;
};












