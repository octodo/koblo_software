/*!	\file CFileMemory.cpp
	\author Michael Olsen
	\date 23/Apr/2005
	\date 23/Apr/2005
*/


#include "buInternalOS.h"


CFileMemory::CFileMemory() : miSizeWhenOpened(0), miCurrentFilePos(0),
	mpWriteBuffer(NULL), muiWriteBufferSize(0)
{
}

CFileMemory::~CFileMemory()
{
	Close();
}

IFileMemory* IFileMemory::Create()
{
	return dynamic_cast<IFileMemory*>(new CFileMemory());
}

void CFileMemory::Destroy()
{
	delete dynamic_cast<CFileMemory*>(this);
}

tbool CFileMemory::Open(const tchar* /*pszPathName*/, EOpenFile /*OpenFile*/)
{
	throw IException::Create(IException::TypeFile, IException::ReasonFileGeneric, EXCEPTION_INFO);
}

tbool CFileMemory::Open(EOpenFile OpenFile, const void* pReadBuffer, tuint64 uiReadBufferSize)
{
	Close();

	mpReadBuffer = (void*)pReadBuffer;

	switch(OpenFile) {
		case FileRead:
			miSizeWhenOpened = uiReadBufferSize;
			break;
		case FileWrite:
		case FileCreate:
			miSizeWhenOpened = 0;
			muiWriteBufferSize = 0;
			break;
	}

	miCurrentFilePos = 0;

	return true;
}

void CFileMemory::Close()
{
	if (mpWriteBuffer) {
		delete[] ((tchar*)mpWriteBuffer);
		mpWriteBuffer = NULL;
	}
	muiWriteBufferSize = 0;
}

tuint64 CFileMemory::Read(tchar* pch, tuint64 iSize)
{
	tuint64 iToRead = iSize;
	if (iToRead > miSizeWhenOpened - miCurrentFilePos) {
		iToRead = miSizeWhenOpened - miCurrentFilePos;
	}

	memcpy((void*)pch, &(((tchar*)mpReadBuffer)[miCurrentFilePos]), (size_t)iToRead);

	miCurrentFilePos += iToRead;

	return iToRead;
}

tuint64 CFileMemory::Write(const tchar* pch, tuint64 iSize)
{
	// Check if we need to alloc/realloc
	if (miCurrentFilePos + iSize > muiWriteBufferSize) {
		// Alloc/realloc
		if (mpWriteBuffer) {
			// Realloc
			tuint64 uiSizeNeeded = miCurrentFilePos + iSize;
			char* pTmp = new char[(unsigned int)uiSizeNeeded];

			// Copy old data
			memcpy((void*)pTmp, mpWriteBuffer, (size_t)muiWriteBufferSize);

			// Swap
			delete[] ((tchar*)mpWriteBuffer);
			mpWriteBuffer = (void*)pTmp;
			muiWriteBufferSize = uiSizeNeeded;
		}
		else {
			// Alloc
			tuint64 uiSizeNeeded = miCurrentFilePos + iSize;
			mpWriteBuffer = (void*)new char[(unsigned int)uiSizeNeeded];
			muiWriteBufferSize = uiSizeNeeded;
		}
	}

	// "Write"
	memcpy((void*)&(((char*)mpWriteBuffer)[miCurrentFilePos]), pch, (size_t)iSize);

	miCurrentFilePos += iSize;

	return iSize;
}

tuint64 CFileMemory::Seek(tuint64 iPos)
{
	miCurrentFilePos = iPos;

	return miCurrentFilePos;
}

tuint64 CFileMemory::GetSizeWhenOpened() const
{
	return miSizeWhenOpened;
}

void CFileMemory::GetPathName(tchar* pszPathName) const
{
	pszPathName[0] = 0;
}

tuint64 CFileMemory::GetCurrentFilePosition() const
{
	return miCurrentFilePos;
}

tuint64 CFileMemory::GetMemorySize()
{
	if (mpReadBuffer) {
		// We're in read mode
		return miSizeWhenOpened;
	}
	else {
		// We're in write/create mode
		return muiWriteBufferSize;
	}
}

void CFileMemory::GetMemory(void* p)
{
	if (mpReadBuffer) {
		// We're in read mode
		memcpy(p, mpReadBuffer, (size_t)miSizeWhenOpened);
	}
	else {
		// We're in write/create mode
		memcpy(p, mpWriteBuffer, (size_t)muiWriteBufferSize);
	}
}


void* CFileMemory::GetMemoryPtr()
{
	if (mpReadBuffer) {
		// We're in read mode
		return mpReadBuffer;
	}
	else {
		// We're in write/create mode
		return mpWriteBuffer;
	}
}
