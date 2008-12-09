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

/*! \class CChunk
 * \brief Implementation of IChunk
*/
class CChunk : public virtual IChunk, public virtual CFile
{
public:
	//! Create a chunk that is read from file. Chunk starts at current file position
	static IChunk* Create(IFile* pFile, tint32 iSize, tint32 iID);

	//! Constructor
	CChunk(IFile* pFile, tint32 iSize, tint32 iID);
	//! Destructor
	virtual ~CChunk();

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

protected:
	//! Size of virtual file (and thus chunk)
	tint32 miSize;
	//! ID of chunk
	tint32 miID;

	//! Original size of virtual file
	tuint64 miSizeOrg;
	//! File position (index) into "real" file
	tuint64 miCurFilePosition;

	//! Restrict access to this call by making it protected
	virtual bool Open(const tchar* pszPathName, EOpenFile eAccessType);
	//! Restrict access to this call by making it protected
	virtual void Close();
};


