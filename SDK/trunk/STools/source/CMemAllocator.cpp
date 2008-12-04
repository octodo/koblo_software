

#include "SToolsInternalOS.h"


IMemAllocator* IMemAllocator::Create(tint lAlignment)
{
	return dynamic_cast<IMemAllocator*>(new CMemAllocator(lAlignment));
}

void CMemAllocator::Destroy()
{
	delete dynamic_cast<CMemAllocator*>(this);
}

CMemAllocator::CMemAllocator(tint lAlignment)
	: mlAlignment(lAlignment)
{
}

CMemAllocator::~CMemAllocator()
{
}

#ifdef WIN32
// To avoid having to include windows headers
//extern "C" void* __stdcall HeapAlloc(void*, long, long);
//extern "C" void* __stdcall GetProcessHeap();
//extern "C" long __stdcall HeapFree(void*, long, void*);

#define HEAP_ZERO_MEMORY 0x00000008      

void* CMemAllocator::Alloc(tint Bytes)
{
	// Max size we need is requested bytes + (alignment - 1) + size of header
	char* p = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Bytes + (mlAlignment - 1) + sizeof(SMemHeader));

	// First position we can use is p + size of header
	char* p2 = p + sizeof(SMemHeader);

	// From here, find the first aligned position
	char* pAligned;
	if ((unsigned long)p2 % mlAlignment == 0) {
		// This position is ok
		pAligned = p2;
	}
	else {
		pAligned = p2 + (mlAlignment - ((unsigned long)p2 % mlAlignment));
	}

	// Write header just before aligned pointer
	SMemHeader* pHeader = (SMemHeader*)(pAligned - sizeof(SMemHeader));
	pHeader->pOrgPointer = p;

	return (void*)pAligned;
}

void CMemAllocator::Free(void* pData)
{
	// Header is just before pointer
	SMemHeader* p = (SMemHeader*)(((char*)pData) - sizeof(SMemHeader));

	// Free it
	HeapFree(GetProcessHeap(), 0, p->pOrgPointer);
}

#else	// WIN32

void* CMemAllocator::Alloc(tint Bytes)
{
	// Max size we need is requested bytes + (alignment - 1) + size of header
	char* p = new char[Bytes + (mlAlignment - 1) + sizeof(SMemHeader)];

	// First position we can use is p + size of header
	char* p2 = p + sizeof(SMemHeader);

	// From here, find the first aligned position
	char* pAligned;
	if ((unsigned long)p2 % mlAlignment == 0) {
		// This position is ok
		pAligned = p2;
	}
	else {
		pAligned = p2 + (mlAlignment - ((unsigned long)p2 % mlAlignment));
	}

	// Write header just before aligned pointer
	SMemHeader* pHeader = (SMemHeader*)(pAligned - sizeof(SMemHeader));
	pHeader->pOrgPointer = p;

	return (void*)pAligned;
}

void CMemAllocator::Free(void* pData)
{
	// Header is just before pointer
	SMemHeader* p = (SMemHeader*)(((char*)pData) - sizeof(SMemHeader));

	// Free it
	delete[] (p->pOrgPointer);
}

#endif	// WIN32 / else

