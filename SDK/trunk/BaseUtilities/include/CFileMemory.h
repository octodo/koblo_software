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


/*! \class CFileMemory
 * \brief Implementation of IFileMemory
*/
class CFileMemory : public virtual IFileMemory, public virtual CFileCP
{
public:
	//! Constructor
	CFileMemory();
	//! Destructor
	virtual ~CFileMemory();

	//! IDestructable override
	virtual void Destroy();

	//! IFile override
	virtual tbool Open(const tchar* pszPathName, EOpenFile OpenFile);
	//! IFile override
	virtual void Close();
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

	//! IFileMemory override
	virtual tbool Open(EOpenFile OpenFile, const void* pReadBuffer = NULL, tuint64 uiReadBufferSize = 0);
	//! IFileMemory override
	virtual tuint64 GetMemorySize();
	//! IFileMemory override
	virtual void GetMemory(void* p);
	//! IFileMemory override
	virtual void* GetMemoryPtr();

protected:
	//! Size when file was originally opened
	tuint64 miSizeWhenOpened;
	//! Current file position
	tuint64 miCurrentFilePos;
	//! If file is opened in read mode, read buffer, NULL
	void* mpReadBuffer;
	//! If file is opened in write/create mode, write buffer, otherwise NULL. If also NULL if no data has been written yet (muiWriteBufferSize == 0).
	void* mpWriteBuffer;
	//! Current size of write buffer
	tuint64 muiWriteBufferSize;
};






