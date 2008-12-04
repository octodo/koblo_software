
class IModulate
{
public:
	static IModulate* Create();
	virtual void Destroy() = 0;

	// pDest[n] = pSrc[n]*fModAmount+fFloor, assumes no overlapping
	virtual void SetMod(tfloat32* pDest, const tfloat32* pSrc, const tfloat fModAmount, const tfloat fFloor, tint lC) = 0;
	// pDest[n] *= pSrc[n]*fModAmount+fFloor, assumes no overlapping
	virtual void AddMod(tfloat32* pDest, const tfloat32* pSrc, const tfloat fModAmount, const tfloat fFloor, tint lC) = 0;
};



