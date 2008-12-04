/*!	\file CChunkFile.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/


#include "buInternalOS.h"

IChunkFile* IChunkFile::Create()
{
	return dynamic_cast<IChunkFile*>(new CChunkFile());
}

void CChunkFile::Destroy()
{
	delete dynamic_cast<CChunkFile*>(this);
}

CChunkFile::CChunkFile()
{
}

CChunkFile::~CChunkFile()
{
}

tbool CChunkFile::Open(const tchar* pszPathName, IFile::EOpenFile AccessType, tint32& riVersionNumber, tbool bHasHeader)
{
	mAccessType = AccessType;

	mbHasHeader = bHasHeader;

	if (CFile::Open(pszPathName, AccessType) == false) {
		return false;
	}

	if (AccessType == IFile::FileRead) {
		if (GetSizeWhenOpened() < 8) {
			Close();
			return false;
		}

		if (bHasHeader) {
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
		else {
			riVersionNumber = 0;
		}
	}

	if ((AccessType == IFile::FileWrite || AccessType == IFile::FileCreate) && bHasHeader) {
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

void CChunkFile::Close()
{
	CFile::Close();
}

inline tint32 SwapB(tint32 ul)
{
	return (((ul>>24) & 0x000000FFL) | ((ul>>8) & 0x0000FF00L) | ((ul<<8) & 0x00FF0000L) | ((ul<<24) & 0xFF000000L));
}

IChunk* CChunkFile::GetChunk(tint32 iID, tbool /*bCopyMem = true*/)
{
	// Iterate until we find it, or EOF
	IChunk* pRet = NULL;
	tbool bEOF = false;
	// Initial starting position
	tint iCurPosition = 4 + sizeof(tint32);
	if (mbHasHeader == false) {
		iCurPosition = 0;
	}
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
			if (mbHasHeader == false) {
				Header.iID = SwapB(Header.iID);
				Header.iSize = SwapB(Header.iSize);
			}

			if (Header.iID == iID) {
				try {
					pRet = CChunk::Create(dynamic_cast<IFile*>(this), Header.iSize, Header.iID);
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

IChunk* CChunkFile::GetNextChunk(tint64& rIndex, tint32 iID, tbool /*bCopyMem = true*/)
{
	// Iterate until we find it, or EOF
	IChunk* pRet = NULL;
	tbool bEOF = false;
	// Initial starting position
	if (mbHasHeader != false) {
		if (rIndex < 4 + sizeof(tint32)) {
			rIndex = 4 + sizeof(tint32);
		}
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
			if (mbHasHeader == false) {
				Header.iID = SwapB(Header.iID);
				Header.iSize = SwapB(Header.iSize);
			}

			if (Header.iID == iID || iID == 0) {
				try {
					pRet = CChunk::Create(dynamic_cast<IFile*>(this), Header.iSize, Header.iID);
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

void CChunkFile::SetChunk(IChunk* pChunk, tbool bIsMemoryChunk)
{
	SChunkHeader Header;
	Header.iID = pChunk->GetID();
	Header.iID = CPSwap(Header.iID);
	Header.iSize = pChunk->GetSize();
	Header.iSize = CPSwap(Header.iSize);

	if (Write((tchar*)&Header, sizeof(Header)) != sizeof(Header)) {
	}

	// Due to a bug in XCode, we cannot use dynamic_cast here
#ifdef _Mac
	CChunkMemory* pChunk2 = (CChunkMemory*)(void*)pChunk;
	if (bIsMemoryChunk) {
#else	// _Mac
	CChunkMemory* pChunk2 = dynamic_cast<CChunkMemory*>(pChunk);
	if (pChunk2) {
#endif	// _Mac
		if (Write((const tchar*)(pChunk2->GetData()), pChunk2->GetSize()) != pChunk2->GetSize()) {
		}
	}
	else {
		// Not a memory chunk. Use regular file interface
		
		// Lasse, rewritten 2008-04-08
		/*
		tint64 iChunkSize = pChunk->GetSizeWhenOpened();
		tchar* p = new tchar[(unsigned int)iChunkSize];

		pChunk->Seek(0);
		pChunk->Read((tchar*)p, iChunkSize);

		if (Write((const tchar*)p, iChunkSize) != iChunkSize) {
		}

		delete[] p;
		*/
		
		// Copy file contents a litte at a time
		const int MAX_CHUNK_FILE_BUFF = 0x00010000;
		tchar pBuff[MAX_CHUNK_FILE_BUFF];
		tint64 iChunkSize = pChunk->GetSizeWhenOpened();
		pChunk->Seek(0);
		while (iChunkSize > 0) {
			tint32 iToRead = (iChunkSize < MAX_CHUNK_FILE_BUFF) ? (tint32)iChunkSize : MAX_CHUNK_FILE_BUFF;
			tuint64 uiActuallyRead = pChunk->Read((tchar*)pBuff, iToRead);
			if (uiActuallyRead != iToRead) {
				// Error - what to do?
				return;
			}
			tuint64 uiActuallyWritten = Write((tchar*)pBuff, uiActuallyRead);
			if (uiActuallyWritten != uiActuallyRead) {
				// Error - what to do?
				return;
			}
			iChunkSize -= (tint32)uiActuallyWritten;
		}
		// .. Lasse
	}
}

