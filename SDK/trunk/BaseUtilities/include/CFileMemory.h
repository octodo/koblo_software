/*!	\file CFileMemory.h
	\author Michael Olsen
	\date 23/Apr/2005
	\date 23/Apr/2005
*/


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






