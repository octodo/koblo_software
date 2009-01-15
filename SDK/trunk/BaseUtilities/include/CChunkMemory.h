// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
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

/*! \class CChunkMemory
 * \brief Implementation of IChunk residing in memory
*/
class CChunkMemory : public virtual IChunk, public virtual CFileCP
{
public:
	//! Constructor
	CChunkMemory(void* p, tint32 iSize, tint32 iID, tbool bCopyMem = true);
	//! Destructor
	virtual ~CChunkMemory();

	//! IChunk override
	virtual void Destroy();
	//! IChunk override
	virtual tint32 GetID();
	//! IChunk override
	virtual tint32 GetSize();
	//! IChunk override
	virtual tint64 GetOffset();

	//! IFile override
	virtual tuint64 Read(tchar* pBuffer, tuint64 iSize);
	//! IFile override
	virtual tuint64 Write(const tchar* pBuffer, tuint64 iSize);
	//! IFile override
	virtual tuint64 Seek(tuint64 iOffset);
	//! IFile override
	virtual tuint64 GetSizeWhenOpened() const;
	//! IFile override
	virtual tuint64 GetCurrentFilePosition() const;
	//! IFile override
	virtual void GetPathName(tchar* pszPathName) const;

	//! Returns pointer to memory buffer
	/*!
		\return void*: Pointer to memory buffer
	*/
	virtual void* GetData();

protected:
	//! Memory buffer
	void* mpData;
	//! Size of memory buffer (and thus size of chunk)
	tint64 miSize;
	//! Chunk ID
	tint32 miID;
	//! Was a buffer created?
	tbool mbCopyMem;

	//! Current (virtual) file position
	tuint64 miCurFilePos;
	//! Original size of (virtual) file
	tuint miSizeOrg;

	//! Restrict access to this call by making it protected
	virtual tbool Open(const tchar* pszPathName, EOpenFile OpenFile);
	//! Restrict access to this call by making it protected
	virtual void Close();
};


