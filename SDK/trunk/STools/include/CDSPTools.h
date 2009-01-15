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

class CDSPTools : public virtual IDSPTools
{
public:
	CDSPTools();
	virtual ~CDSPTools();

	// IDSPTools overrides
	virtual void Copy(tfloat32* pDest, const tfloat32* pSrc, tint lC);
	virtual void Copy32(tfloat32* pDest, const tfloat32* pSrc);
	virtual void Set(tfloat32* pDest, tfloat32 Val, tint lC);
	virtual void Set32(tfloat32* pDest, tfloat32 Val);
	virtual void Clear(tfloat32* pDest, tint lC);
	virtual void Clear32(tfloat32* pDest);
	virtual void Mul(tfloat32* pDest, const tfloat32* pSrc1, const tfloat32* pSrc2, tint lC);
	virtual void Mul32(tfloat32* pDest, const tfloat32* pSrc1, const tfloat32* pSrc2);
	virtual void Mul(tfloat32* pDest, const tfloat32* pSrc, tint lC);
	virtual void Mul32(tfloat32* pDest, const tfloat32* pSrc);
	virtual void Mul(tfloat32* pDest, tfloat32 Src, tint lC);
	virtual void Mul32(tfloat32* pDest, tfloat32 Src);
	virtual void Add(tfloat32* pDest, const tfloat32* pSrc1, const tfloat32* pSrc2, tint lC);
	virtual void Add32(tfloat32* pDest, const tfloat32* pSrc1, const tfloat32* pSrc2);
	virtual void Add(tfloat32* pDest, const tfloat32* pSrc, tint lC);
	virtual void Add32(tfloat32* pDest, const tfloat32* pSrc);
	virtual void Add(tfloat32* pDest, tfloat32 Src, tint lC);
	virtual void Add32(tfloat32* pDest, tfloat32 Src);
	virtual void Add(tfloat32* pDest, const tfloat32* pSrc, tfloat32 Val, tint lC);
	virtual void Add32(tfloat32* pDest, const tfloat32* pSrc, tfloat32 Val);

protected:
};

