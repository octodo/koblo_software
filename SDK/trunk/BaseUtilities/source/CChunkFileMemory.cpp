/*!	\file CChunkFileMemory.cpp
	\author Michael Olsen
	\date 23/Apr/2005
	\date 23/Apr/2005
*/


#include "buInternalOS.h"

IChunkFileMemory* IChunkFileMemory::Create()
{
	return dynamic_cast<IChunkFileMemory*>(new CChunkFileMemory());
}

void CChunkFileMemory::Destroy()
{
	delete dynamic_cast<CChunkFileMemory*>(this);
}

CChunkFileMemory::CChunkFileMemory()
{
}

CChunkFileMemory::~CChunkFileMemory()
{
}

tbool CChunkFileMemory::Open(const tchar* pszPathName, EOpenFile OpenFile)
{
	return CFileMemory::Open(pszPathName, OpenFile);
}

tuint64 CChunkFileMemory::Read(tchar* pch, tuint64 iSize)
{
	return CFileMemory::Read(pch, iSize);
}

tuint64 CChunkFileMemory::Write(const tchar* pch, tuint64 iSize)
{
	return CFileMemory::Write(pch, iSize);
}

tuint64 CChunkFileMemory::Seek(tuint64 iPos)
{
	return CFileMemory::Seek(iPos);
}

tuint64 CChunkFileMemory::GetSizeWhenOpened() const
{
	return CFileMemory::GetSizeWhenOpened();
}

void CChunkFileMemory::GetPathName(tchar* pszPathName) const
{
	CFileMemory::GetPathName(pszPathName);
}

tuint64 CChunkFileMemory::GetCurrentFilePosition() const
{
	return CFileMemory::GetCurrentFilePosition();
}

tbool CChunkFileMemory::Open(const tchar* /*pszPathName*/, IFile::EOpenFile /*AccessType*/, tint32& /*riVersionNumber*/)
{
	throw IException::Create(IException::TypeFile, IException::ReasonFileGeneric, EXCEPTION_INFO);
}

bool CChunkFileMemory::Open(IFile::EOpenFile AccessType, tint32& riVersionNumber, const void* pReadBuffer, tuint64 uiReadBufferSize)
{
	mAccessType = AccessType;

	if (CFileMemory::Open(AccessType, pReadBuffer, uiReadBufferSize) == false) {
		return false;
	}

	if (AccessType == IFile::FileRead) {
		if (GetSizeWhenOpened() < 8) {
			Close();
			return false;
		}

		tchar psz[4];
		if (Read((tchar*)psz, 4) != 4) {
			Close();
			return false;
		}

		if (psz[0] != 'K' ||
			psz[1] != '2' ||
			psz[2] != 'S' ||
			psz[3] != 'F') {
			Close();
			return false;
		}

		if (Read((tchar*)&riVersionNumber, sizeof(tint32)) != sizeof(tint32)) {
			Close();
			return false;
		}
		riVersionNumber = CPSwap(riVersionNumber);
	}

	if (AccessType == IFile::FileWrite || AccessType == IFile::FileCreate) {
		tchar psz[4];
		psz[0] = 'K';
		psz[1] = '2';
		psz[2] = 'S';
		psz[3] = 'F';
		if (Write((const tchar*)psz, 4) != 4) {
			Close();
			return false;
		}

		tint32 iVersionNumber = CPSwap(riVersionNumber);
		if (Write((const tchar*)&iVersionNumber, sizeof(tint32)) != sizeof(tint32)) {
			Close();
			return false;
		}
	}

	return true;
}

void CChunkFileMemory::Close()
{
	CFileMemory::Close();
}

IChunk* CChunkFileMemory::GetChunk(tint32 iID, tbool bCopyMem /*= true*/)
{
	// Iterate until we find it, or EOF
	IChunk* pRet = NULL;
	tbool bEOF = false;
	// Initial starting position
	tint iCurPosition = 4 + sizeof(tint32);
	while (pRet == NULL && bEOF == false) {
		Seek(iCurPosition);

		SChunkHeader Header;
		if (Read((tchar*)&Header, sizeof(Header)) != sizeof(Header)) {
			bEOF = true;
		}
		else {
			// Cross platform swapping
			Header.iID = CPSwap(Header.iID);
			Header.iSize = CPSwap(Header.iSize);

			if (Header.iID == iID) {
				try {
					pRet = CChunkMemory::Create(&(((tchar*)mpReadBuffer)[CFileMemory::miCurrentFilePos]), Header.iSize, Header.iID, bCopyMem);
				}
				catch(...) {
					delete pRet;
					throw;
				}
			}
			else {
				// Advance to next chunk
				iCurPosition += (Header.iSize + sizeof(Header));
			}
		}
	}

	return pRet;
}

IChunk* CChunkFileMemory::GetNextChunk(tint64& rIndex, tint32 iID, tbool bCopyMem /*= true*/)
{
	// Iterate until we find it, or EOF
	IChunk* pRet = NULL;
	tbool bEOF = false;
	// Initial starting position
	if (rIndex < 4 + sizeof(tint32)) {
		rIndex = 4 + sizeof(tint32);
	}
	while (pRet == NULL && bEOF == false) {
		Seek(rIndex);

		SChunkHeader Header;
		if (Read((tchar*)&Header, sizeof(Header)) != sizeof(Header)) {
			bEOF = true;
		}
		else {
			// Cross platform swapping
			Header.iID = CPSwap(Header.iID);
			Header.iSize = CPSwap(Header.iSize);

			if (Header.iID == iID || iID == 0) {
				try {
					pRet = CChunkMemory::Create(&(((tchar*)mpReadBuffer)[CFileMemory::miCurrentFilePos]), Header.iSize, Header.iID, bCopyMem);
				}
				catch(...) {
					delete pRet;
					throw;
				}
			}

			// Advance to next chunk
			rIndex += (Header.iSize + sizeof(Header));
		}
	}

	return pRet;
}

tuint64 CChunkFileMemory::GetMemorySize()
{
	return CFileMemory::GetMemorySize();
}

void CChunkFileMemory::GetMemory(void* p)
{
	CFileMemory::GetMemory(p);
}

