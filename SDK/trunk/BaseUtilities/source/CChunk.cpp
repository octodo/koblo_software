/*!	\file CChunk.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

#include "buInternalOS.h"

IChunk* CChunk::Create(IFile* pFile, tint32 iSize, tint32 iID)
{
	return dynamic_cast<IChunk*>(new CChunk(pFile, iSize, iID));
}

void CChunk::Destroy()
{
	delete dynamic_cast<CChunk*>(this);
}

CChunk::CChunk(IFile* pFile, tint32 iSize, tint32 iID)
	: miID(iID), miSize(iSize)
{
	miCurFilePosition = pFile->GetCurrentFilePosition();

	tchar pszPathName[513];
	pFile->GetPathName((tchar*)pszPathName);
	CFile::Open(pszPathName, IFile::FileRead);
	CFile::Seek(miCurFilePosition);
}

CChunk::~CChunk()
{
}

tint32 CChunk::GetID()
{
	return miID;
}

tint32 CChunk::GetSize()
{
	return miSize;
}

tint64 CChunk::GetOffset()
{
	return miCurFilePosition;
}

tbool CChunk::Open(const tchar* /*pszPathName*/, EOpenFile /*eAccessType*/)
{
	return false;
}

void CChunk::Close()
{
}

tuint64 CChunk::Read(tchar* pBuffer, tuint64 iSize)
{
	// Current "virtual" file position, i.e. difference between current and original
	tuint64 iVirtualFilePos = GetCurrentFilePosition();

	tuint64 iToRead = iSize;
	if (iVirtualFilePos + iSize > miSize) {
		iToRead = miSize - iVirtualFilePos;
	}

	return CFile::Read((tchar*)pBuffer, iToRead);
}

tuint64 CChunk::Write(const tchar* pBuffer, tuint64 iSize)
{
	// Current "virtual" file position, i.e. difference between current and original
	tuint64 iVirtualFilePos = GetCurrentFilePosition();

	tuint64 iToWrite = iSize;
	if (iVirtualFilePos + iSize > miSize) {
		iToWrite = miSize - iVirtualFilePos;
	}

	return CFile::Write((const tchar*)pBuffer, iToWrite);
}

tuint64 CChunk::Seek(tuint64 iOffset)
{
	// Current "virtual" file position, i.e. difference between current and original
//	tuint64 iVirtualFilePos = CFile::GetCurrentFilePosition() - miCurFilePosition;

	if (iOffset > miSize) {
		iOffset = miSize;
	}

	tuint64 iRet = CFile::Seek(iOffset + miCurFilePosition);

	return iRet - miCurFilePosition;
}

tuint64 CChunk::GetSizeWhenOpened() const
{
	return miSize;
}

tuint64 CChunk::GetCurrentFilePosition() const
{
	return CFile::GetCurrentFilePosition() - miCurFilePosition;
}


