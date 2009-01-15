/*!	\file IFileMemory.h
	\author Michael Olsen
	\date 23/Apr/2005
	\date 23/Apr/2005
*/

/*! \class IFileMemory
*/
class IFileMemory : public virtual IFile
{
public:
	//! Creates IFileMemory
	static IFileMemory* Create();

	//! Open file, given filename (full path)
	/*!
		\param OpenFile [in]: File access to open with
		\param pReadBuffer [in]: File buffer to "read" from if file access is FileRead, otherwise ignored
		\param iReadBufferSize [in]: Size of file buffer to "read" from if file access is FileRead, otherwise ignored
		\return bool: true if success, false otherwise
	*/
	virtual tbool Open(EOpenFile OpenFile, const void* pReadBuffer = NULL, tuint64 uiReadBufferSize = 0) = 0;

	//! Returns the current size of memory file.
	/*!
		\return tuint64: Current size of memory file.
	*/
	virtual tuint64 GetMemorySize() = 0;

	//! Gets the memory.
	/*!
		\param p [out]: Points to buffer to receive memory. To determine the size, call GetMemorySize()
	*/
	virtual void GetMemory(void* p) = 0;

	//! Get memory pointer. Use with extreme caution!
	/*
		\return void*: Pointer to the memory allocated to the IFileMemory
	*/
	virtual void* GetMemoryPtr() = 0;
};




