/*!	\file CChunkMemory.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/


#include "buInternalOS.h"

IChunk* IChunk::Create(void* p, tint32 iSize, tint32 iID, tbool bCopyMem /*= true*/)
{
	return dynamic_cast<IChunk*>(new CChunkMemory(p, iSize, iID));
}

void CChunkMemory::Destroy()
{
	delete dynamic_cast<CChunkMemory*>(this);
}

CChunkMemory::CChunkMemory(void* p, tint32 iSize, tint32 iID, tbool bCopyMem /*= true*/)
	: mpData(NULL), miID(iID), miSize(iSize), miCurFilePos(0), miSizeOrg(iSize), mbCopyMem(bCopyMem)
{
	if (miSize) {
		if (bCopyMem) {
			mpData = (void*)new tchar[(unsigned int)miSize];
			memcpy(mpData, p, (size_t)miSize);
		}
		else {
			mpData = p;
		}
	}
}

CChunkMemory::~CChunkMemory()
{
	if ((mpData) && (mbCopyMem)) {
		delete[] ((tchar*)mpData);
	}
}

tint32 CChunkMemory::GetID()
{
	return miID;
}

tint32 CChunkMemory::GetSize()
{
	return (tint32)miSize;
}

tint64 CChunkMemory::GetOffset()
{
	return 0;
}

void* CChunkMemory::GetData()
{
	return mpData;
}

tbool CChunkMemory::Open(const tchar* /*pszPathName*/, EOpenFile /*eAccessType*/)
{
	return true;
}

void CChunkMemory::Close()
{
}

tuint64 CChunkMemory::Read(tchar* pBuffer, tuint64 iSize)
{
	tuint64 iToRead = iSize;
	if (miCurFilePos + iSize > (tuint64)miSize) {
		iToRead = miSize - miCurFilePos;
	}

	memcpy(pBuffer, ((tchar*)mpData) + miCurFilePos, (size_t)iToRead);

	miCurFilePos += iToRead;

	return iToRead;
}

tuint64 CChunkMemory::Write(const tchar* pBuffer, tuint64 iSize)
{
	// Check to see if we need to realloc
	if (miCurFilePos + iSize > (tuint64)miSize) {
		void* mpTmp = (void*)new tchar[(unsigned int)(miCurFilePos + iSize)];
		if (mpData) {
			memcpy(mpTmp, mpData, (size_t)miSize);
			delete[] ((tchar*)mpData);
		}
		mpData = mpTmp;
		miSize = (tuint32)(miCurFilePos + iSize);
	}

	memcpy(((char*)mpData) + miCurFilePos, pBuffer, (size_t)iSize);

	miCurFilePos += iSize;
	
	return iSize;
}

tuint64 CChunkMemory::Seek(tuint64 iOffset)
{
	miCurFilePos = iOffset;
	if (miCurFilePos > (tuint64)miSize) {
		miCurFilePos = miSize;
	}

	return miCurFilePos;
}

tuint64 CChunkMemory::GetSizeWhenOpened() const
{
	return miSizeOrg;
}

tuint64 CChunkMemory::GetCurrentFilePosition() const
{
	return miCurFilePos;
}

void CChunkMemory::GetPathName(tchar* pszPathName) const
{
	// Empty string
	pszPathName[0] = 0;
}



