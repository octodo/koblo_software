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


/*! \class CKnob2
 * \brief Implementation of IKnob2
*/
class CKnob2 : public virtual IKnob2, public virtual CKnob
{
public:
	//! Constructor
	CKnob2();
	//! Destructor
	virtual ~CKnob2();

	//! IDestructable override
	virtual void Destroy();

	//! CKnob override
	virtual void OnDraw(const SRect &rUpdate);

	//! IKnob2 override
	virtual void Init(tint iID, IBitmapResource* pBitmap, tint32 iAngleStart, tint32 iAngleEnd);

protected:
	//! Size of bitmap
	SSize mSizeBitmap;

	//! Start angle
	tint32 miAngleStart;

	//! End angle
	tint32 miAngleEnd;
};
