// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
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

class CDSPToolsOSX : public virtual CDSPTools
{
public:
	CDSPToolsOSX();
	virtual ~CDSPToolsOSX();

	//! IDestructable override
	virtual void Destroy();

	//! CDSPTools override
	virtual void Copy32(tfloat32* pDest, const tfloat32* pSrc);
	//! CDSPTools override
	virtual void Set32(tfloat32* pDest, tfloat32 Val);
	//! CDSPTools override
	virtual void Clear32(tfloat32* pDest);
	//! CDSPTools override
	virtual void Mul32(tfloat32* pDest, const tfloat32* pSrc1, const tfloat32* pSrc2);
	//! CDSPTools override
	virtual void Mul32(tfloat32* pDest, const tfloat32* pSrc);
	//! CDSPTools override
	virtual void Mul32(tfloat32* pDest, tfloat32 Src);
	//! CDSPTools override
	virtual void Add32(tfloat32* pDest, const tfloat32* pSrc1, const tfloat32* pSrc2);
	//! CDSPTools override
	virtual void Add32(tfloat32* pDest, const tfloat32* pSrc);
	//! CDSPTools override
	virtual void Add32(tfloat32* pDest, tfloat32 Src);

protected:
};

