
class IDSPTools
{
public:
	static IDSPTools* Create();
	virtual void Destroy() = 0;

	// pDest[n] = pSrc[n], assumes no overlapping
	virtual void Copy(tfloat32* pDest, const tfloat32* pSrc, tint lC) = 0;
	virtual void Copy32(tfloat32* pDest, const tfloat32* pSrc) = 0;
	// pDest[n] = sVal
	virtual void Set(tfloat32* pDest, tfloat32 Val, tint lC) = 0;
	virtual void Set32(tfloat32* pDest, tfloat32 Val) = 0;
	// pDest[n] = 0
	virtual void Clear(tfloat32* pDest, tint lC) = 0;
	virtual void Clear32(tfloat32* pDest) = 0;
	// pDest[n] = pSrc1[n] * pSrc2[n]
	virtual void Mul(tfloat32* pDest, const tfloat32* pSrc1, const tfloat32* pSrc2, tint lC) = 0;
	virtual void Mul32(tfloat32* pDest, const tfloat32* pSrc1, const tfloat32* pSrc2) = 0;
	// pDest[n] *= pSrc[n]
	virtual void Mul(tfloat32* pDest, const tfloat32* pSrc, tint lC) = 0;
	virtual void Mul32(tfloat32* pDest, const tfloat32* pSrc) = 0;
	// pDest[n] *= Src
	virtual void Mul(tfloat32* pDest, tfloat32 Src, tint lC) = 0;
	virtual void Mul32(tfloat32* pDest, tfloat32 Src) = 0;
	// pDest[n] = pSrc1[n] + pSrc2[n]
	virtual void Add(tfloat32* pDest, const tfloat32* pSrc1, const tfloat32* pSrc2, tint lC) = 0;
	virtual void Add32(tfloat32* pDest, const tfloat32* pSrc1, const tfloat32* pSrc2) = 0;
	// pDest[n] += pSrc[n]
	virtual void Add(tfloat32* pDest, const tfloat32* pSrc, tint lC) = 0;
	virtual void Add32(tfloat32* pDest, const tfloat32* pSrc) = 0;
	// pDest[n] += Src
	virtual void Add(tfloat32* pDest, tfloat32 Src, tint lC) = 0;
	virtual void Add32(tfloat32* pDest, tfloat32 Src) = 0;
	// pDest[n] += pSrc[n] * Val
	virtual void Add(tfloat32* pDest, const tfloat32* pSrc, tfloat32 Val, tint lC) = 0;
	virtual void Add32(tfloat32* pDest, const tfloat32* pSrc, tfloat32 Val) = 0;
};



