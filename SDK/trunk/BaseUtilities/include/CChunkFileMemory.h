/*!	\file CChunkFileMemory.h
	\author Michael Olsen
	\date 23/Apr/2005
	\date 23/Apr/2005
*/


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

