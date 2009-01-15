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


/*! \class CChunkFile
 * \brief Implementation of IChunkFile.
 *
 * Note that it uses CFile for implementation, but IChunkFile cannot be typecasted to IFile
*/
class CChunkFile : public virtual IChunkFile, public virtual CFile
{
public:
	//! Constructor
	CChunkFile();
	//! Destructor
	virtual ~CChunkFile();

	//! IChunkFile override
	virtual void Destroy();
	//! IChunkFile override
	virtual tbool Open(const tchar* pszPathName, IFile::EOpenFile AccessType, tint32& riVersionNumber, tbool bHasHeader = true);
	//! IChunkFile override
	virtual void Close();
	//! IChunkFile override
	virtual IChunk* GetChunk(tint32 iID, tbool bCopyMem = true);
	//! IChunkFile override
	virtual IChunk* GetNextChunk(tint64& rIndex, tint32 iID = 0, tbool bCopyMem = true);
	//! IChunkFile override
	virtual void SetChunk(IChunk* pChunk, tbool bIsMemoryChunk);

protected:
	//! Access type used to open file
	IFile::EOpenFile mAccessType;

#ifdef WIN32
#pragma pack(push, 1)
#endif	// WIN32
	/*! \class SChunkHeader
	 * \brief Contains info about given chunk
	*/
	struct SChunkHeader
	{
		// ID of chunk
		tint32 iID;
		// Size of chunk, not including header
		tint32 iSize;
	} PACKED;
#ifdef WIN32
#pragma pack(pop)
#endif	// WIN32

	tbool mbHasHeader;
};

