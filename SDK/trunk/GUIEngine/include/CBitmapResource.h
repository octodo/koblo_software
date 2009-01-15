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


/*! \class CBitmapResource
 * \brief Implementation of IBitmapResource
*/
class CBitmapResource : public virtual IBitmapResource, public virtual CResource, public virtual CRefCountable
{
public:
	//! Constructor
	CBitmapResource(tint iNrOfFrames, tint iNrOfVariants);
	//! Destructor
	virtual ~CBitmapResource();

	//! IDestructable override
	virtual void Destroy();

	//! IResource override
	virtual void Load(IWindow* pWnd, IFile* pFile, tint iSize, tint32 iResID);

	//! IBitmapResource override
	virtual void GetSize(SSize& rSize) const;
	//! IBitmapResource override
	virtual tint GetWidth() const;
	//! IBitmapResource override
	virtual tint GetHeight() const;
	//! IBitmapResource override
	virtual void GetRealSize(SSize& rSize) const;
	//! IBitmapResource override
	virtual tint GetBytesPerRow() const;
	//! IBitmapResource override
	virtual tint GetBitsPerPixel() const;
	//! IBitmapResource override
	virtual void Draw(IWindow* pWnd, const SRect& rctUpdate, const SRect& rctDraw, tint iFrame, tbool bGhost = false);

	//! Reinit is called when the original bitmap data has been altered
	/*!
		\param pWnd [in]: Window we're attached to
	*/
//	virtual void ReInit(IWindow* pWnd) = 0;

protected:
	//! Size of bitmap. This is the "physical" size of the bitmap, i.e. for a 5-frame bitmap it is 5 times as big as shown
	SSize mSize;
	//! Bytes per row in bitmap
	tint miBytesPerRow;
	//! Bits per pixel in bitmap
	tint miBitsPerPixel;
	//! Number of frames in bitmap
	tint miNrOfFrames;
	//! Number of variants in bitmap
	tint miNrOfVariants;
	//! Array of DCs for drawing, one for each variant / frame. (pDC = mppDC[Variant * NrOfFrames + Frame])
	IDC** mppDC;

	//! Call to load targa at initiazation time
	virtual void LoadTga();

	//! Returns number of frames
	/*!
		\return tint: Number of frames
	*/
	virtual tint GetNrOfFrames();

	//! Returns number of variants
	/*!
		\return tint: Nnumber of variants
	*/
	virtual tint GetNrOfVariants();
};
