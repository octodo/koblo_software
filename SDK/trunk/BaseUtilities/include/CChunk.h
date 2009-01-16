/*!	\file CChunk.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

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


