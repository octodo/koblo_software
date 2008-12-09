// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
//
// This file is part of the Koblo SDK.
//
// the Koblo SDK is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo SDK is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.


/*! \class CChunkFileMemory
 * \brief Implementation of IChunkFileMemory.
*/
class CChunkFileMemory : public virtual IChunkFileMemory, public virtual CChunkFile, public virtual CFileMemory
{
public:
	//! Constructor
	CChunkFileMemory();
	//! Destructor
	virtual ~CChunkFileMemory();

	//! IChunkFile override
	virtual void Destroy();
	//! IChunkFile override
	virtual tbool Open(const tchar* pszPathName, IFile::EOpenFile AccessType, tint32& riVersionNumber);
	//! IChunkFile override
	virtual void Close();
	//! IChunkFile override
	virtual IChunk* GetChunk(tint32 iID, tbool bCopyMem = true);
	//! IChunkFile override
	virtual IChunk* GetNextChunk(tint64& rIndex, tint32 iID = 0, tbool bCopyMem = true);

	//! IChunkFileMemory
	virtual tbool Open(IFile::EOpenFile AccessType, tint32& riVersionNumber, const void* pReadBuffer = NULL, tuint64 uiReadBufferSize = 0);
	//! IChunkFileMemory
	virtual tuint64 GetMemorySize();
	//! IChunkFileMemory
	virtual void GetMemory(void* p);

	//! IFile override
	virtual tbool Open(const tchar* pszPathName, EOpenFile OpenFile);
	//! IFile override
	virtual tuint64 Read(tchar* pch, tuint64 iSize);
	//! IFile override
	virtual tuint64 Write(const tchar* pch, tuint64 iSize);
	//! IFile override
	virtual tuint64 Seek(tuint64 iPos);
	//! IFile override
	virtual tuint64 GetSizeWhenOpened() const;
	//! IFile override
	virtual void GetPathName(tchar* pszPathName) const;
	//! IFile override
	virtual tuint64 GetCurrentFilePosition() const;

protected:
	//! Access type used to open file
	IFile::EOpenFile mAccessType;
};

