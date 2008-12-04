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


/*! \class CBitmap
 * \brief Implementation of IBitmap
*/
class CBitmap : public virtual IBitmap, public virtual CControl
{
public:
	//! Constructor
	CBitmap();
	//! Destructor
	virtual ~CBitmap();

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

	//! IBitmap override
	virtual void Init(tint32 iID, IBitmapResource* pBitmap, tint iFrames);
	//! IBitmap override
	virtual void SetFrame(tint iFrame);

protected:
	//! Size of bitmap
	SSize mSizeBitmap;

	//! Number of frames in bitmap
	tint miFrames;

	//! Current frame to be shown
	tint miFrameCur;
};