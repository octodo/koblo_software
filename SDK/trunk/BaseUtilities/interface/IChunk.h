/*!	\file IChunk.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/


/*! \class IChunk
 * \brief Interface to access a chunk from an IChunkFile
 *
 * You can create new chunks in memory using Create, and write them using IChunkFile::SetChunk
*/
class IChunk : public virtual IFile, public virtual IDestructable
{
public:
	//! Create a chunk from memory
	/*!
		\param p [in]: Pointer to data to put in chunk
		\param iSize [in]: Size of data in chunk (number of bytes)
		\param iID [in]: ID of chunk (chunk type)
		\param bCopyMem [in]: True: we must allocate a memory area and copy the chunk contents there, this will be deleted upon Destroy(). False: No memory allocation, and thus no delete is done.
	*/
	static IChunk* Create(void* p, tint32 iSize, tint32 iID, tbool bCopyMem = true);

	//! Returns chunks ID
	/*!
		\return tint32: chunks ID
	*/
	virtual tint32 GetID() = 0;

	//! Returns chunks size
	/*!
		\return tint32: chunks size
	*/
	virtual tint32 GetSize() = 0;

	//! Returns the offset of the chunk into the actual file
	/*!
		return tint32: Chunks offset into file
	*/
	virtual tint64 GetOffset() = 0;
};






