/*!	\file IChunkFileMemory.h
	\author Michael Olsen
	\date 23/Apr/2005
	\date 23/Apr/2005
*/

/*! \class IChunkFileMemory
 * \brief Interface for IChunkFiles which are in memory
 *
 * Besides supporting the IChunkFile interface, IChunkFileMemory contains calls to access the memory (read / write)
 * IChunkFile::Open should not be used, instead use the IChunkFileMemory version
*/
class IChunkFileMemory : public virtual IChunkFile
{
public:
	//! Create IChunkFileMemory
	static IChunkFileMemory* Create();

	//! Open memory chunk file
	/*!
		\param AccessType [in]: Access type (read / write / create)
		\param iVersionNumber [in/out]: For write/create [in]: Version number to write in file. For read [out]: Version number of file
		\param pReadBuffer [in]: File buffer to "read" from if file access is FileRead, otherwise ignored
		\param iReadBufferSize [in]: Size of file buffer to "read" from if file access is FileRead, otherwise ignored
	*/
	virtual tbool Open(IFile::EOpenFile AccessType, tint32& riVersionNumber, const void* pReadBuffer = NULL, tuint64 uiReadBufferSize = 0) = 0;

	//! Returns the current size of memory file, if file is opened with create or write. Otherwise returns undefined value
	/*!
		\return tuint64: Current size of memory file.
	*/
	virtual tuint64 GetMemorySize() = 0;

	//! Gets the memory, if file is opened with create ro write. If it's a read file, don't call
	/*!
		\param p [out]: Points to buffer to receive memory. To determine the size, call GetMemorySize()
	*/
	virtual void GetMemory(void* p) = 0;
};

