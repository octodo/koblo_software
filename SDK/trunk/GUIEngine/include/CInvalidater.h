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


/*! \class CInvalidater
 * \brief Implementation of IInvalidater
*/
class CInvalidater : public virtual IInvalidater
{
public:
	//! Constructor
	CInvalidater();
	//! Destructor
	virtual ~CInvalidater();

	//! IDestructable override
	virtual void Destroy();

	//! IInvalidater override
	virtual void Reset();
	//! IInvalidater override
	virtual void InvalidateRect(const SRect &Rect);
	//! IInvalidater override
	virtual tbool IsRectInvalidated() const;
	//! IInvalidater override
	virtual void GetInvalidatedRect(SRect& rRect);// const;

protected:
	//! Is any rectangle invalidated?
	volatile tbool mbRectInvalidated;
	//! Rectangle that is invalidated (if any)
	SRect mRectInvalidated;
	//! List of rectangle that is invalidated
	std::list<SRect> mRectList;

	// Lasse
	CMutex mMutex;
	volatile tint32 miListLen_AtLastGet;
	volatile tbool mbIsAdding;
	virtual void _IncludeRect(const SRect& Rect);
	// .. Lasse
};




