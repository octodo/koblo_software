/*!	\file CChunkFile.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/


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

