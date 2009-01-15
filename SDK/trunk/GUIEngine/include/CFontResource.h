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


/*! \class CFontResource
 * \brief Implementation of IFontResource
*/
class CFontResource : public virtual IFontResource, public virtual CBitmapResource
{
public:
	//! Constructor
	CFontResource();
	//! Destructor
	virtual ~CFontResource();

	//! IDestructable override
	virtual void Destroy();

	//! IFontResource override
	virtual tint GetLength(tint iIndex) const;
	//! IFontResource override
	virtual tint GetOffset(tint iIndex) const;
	//! IFontResource override
	virtual tint GetTextWidth(const tchar* psz) const;
	//! IFontResource override
	virtual tint GetTextHeight() const;
	//! IFontResource override
	virtual tint GetFontTopLine() const;
	//! IFontResource override
	virtual tint GetFontBaseLine() const;
	//! IFontResource override
	virtual void Load(IWindow* pWnd, IFile* pFile, tint iSize, tint32 iResID, tint iNrOfChars, const tint* piCharSizes, const SRGB& TextColour);
	//! Like Load(...) - handles deprecated fonts
	virtual void Load_Deprecated(IWindow* pWnd, IFile* pFile, tint iSize, tint32 iResID, tint iNrOfChars, const tint* piCharSizes, const SRGB& TextColour);
	//! IFontResource override
	virtual void Draw(IWindow* pWnd, const SRect& rctUpdate, const SRect& rctDraw, const std::string& sText);
	//! IBitmapResource override
	virtual void Draw(IWindow* /*pWnd*/, const SRect& /*rctUpdate*/, const SRect& /*rctDraw*/, tint /*iFrame*/) {}
	//! CBitmapResource override
	virtual void ReInit(IWindow* /*pWnd*/) {}

	//! Non-exported call to draw text onto bitmap in memory (used for drawing text onto controls). It is the responsibility of the caller to make sure array bounds are not exceeded.
	/*!
		\param pszText [in]: Text to draw
		\param Pos [in]: Position in bitmap to draw it
		\param Colour [in]: Colour to draw text with
		\param pDestBuffer [in]: Destination buffer
		\param iBytesPerRow [in]: Bytes per row in destination buffer
	*/
	virtual void DrawTextInMemory(const tchar* pszText, const SPos& Pos, const SRGB& Colour, tchar* pDestBuffer, tint iBytesPerRow);

	virtual SRGB GetColour() const {return mColour;}

protected:
	//! Number of characters in font
	tint miNrOfChars;
	//! Array of lengths of each character
	tint* mpiLength;
	//! Array of offsets of each character
	tint* mpiOffset;

	//! Top pixel of font (used when centering vertically)
	tint miTopLine;
	//! Bottom pixel of font (used when centering vertically)
	tint miBaseLine;

	//! Colour used for text
	SRGB mColour;
};

