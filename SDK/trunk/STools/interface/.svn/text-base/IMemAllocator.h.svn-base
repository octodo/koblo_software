

/*

Class for allocation of aligned memory

*/

class IMemAllocator
{
public:
	// lAlignment is the desired allocation alignment in bytes
	static IMemAllocator* Create(tint lAlignment);
	virtual void Destroy() = 0;

	virtual void* Alloc(tint Bytes) = 0;
	virtual void Free(void* pData) = 0;

	virtual tfloat32* AllocFloat32(tint iSize) {return (tfloat32*)Alloc(iSize * sizeof(tfloat32));}
	// This version is just here for convenience. It is ok to mix different Alloc() and Free()'s.
	virtual void Free(tfloat32* pData) {Free((void*)pData);}
};



