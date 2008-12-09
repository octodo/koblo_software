/*!	\file IChunkFile.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
	\todo Make SetChunk work with non-memory chunks.
*/

// Defined chunks

//! Resources list chunk
const tint32 giChunkResourceList = 'RSLS';

//! Resources chunk
const tint32 giChunkResources = 'RSRS';

//! Part chunk (index of first parameter (tint32), parameter count(tint32), parameter data(tint32[]))
const tint32 giChunkPart = 'PART';

//! Part name chunk (size determines length of string. Not zero terminated)
const tint32 giChunkPartName = 'PRTN';

//! Part B chunk (index of first parameter (tint32), parameter count(tint32), parameter data(tint32[])). Presets contains both a 'PART' chunk and a 'PRTB' chunk
const tint32 giChunkPartB = 'PRTB';

//! Multi chunk (index of first parameter (tint32), parameter count(tint32), parameter data(tint32[]))
const tint32 giChunkMulti = 'MLTI';

//! Multi name chunk (size determines length of string. Not zero terminated)
const tint32 giChunkMultiName = 'MLTN';

//! Multi keysplit info (128 * part id)
const tint32 giChunkMultiKeySplit = 'KSPL';

//! Performance chunk (index of first parameter (tint32), parameter count(tint32), parameter data(tint32[]))
const tint32 giChunkPerformance = 'PERF';

//! B set of performance (for a/b'ing)
const tint32 giChunkBSet = 'BSET';

//! GUI settings (panes selected etc.) (index of first parameter (tint32), parameter count(tint32), parameter data(tint32[]))
const tint32 giChunkGUI = 'GUIS';

//! System settings (user name, password etc.) (index of first parameter (tint32), parameter count(tint32), parameter data(tint32[]))
const tint32 giChunkSystem = 'SYST';

//! Keymap file header (Package ID (tint32), Company ID (tint32), Sound ID (tint32), ChannelCount (tint32), ZoneCountLeft (tint32), ZoneCountRight (tint32))
const tint32 giChunkKeymap = 'KMAP';

//! Keymap file zone. (Low key (tint32), High key (tint32), Pitch enable (tint32), Sound name (tchar[65], zero terminated))
const tint32 giChunkKeymapZone = 'KZNE';

//! Keymap wave file. First comes left channel files, then right channel (if not mono). Starting with lowest key.
const tint32 giChunkKeymapWave = 'WAVE';

//! Effect chunk (index of first parameter (tint32), parameter count(tint32), parameter data(tint32[]))
const tint32 giChunkEffect = 'SEFX';

//! Info chunk (nr of packages (tint32), package id's (nr of packages * tint32)
const tint32 giChunkInfo = 'INFO';

//! AWM info chunk (nr of AWMs (tint32), (Vendor ID + Sound ID) (nr of AMWs * tint32)
const tint32 giChunkSoundInfo = 'AINF';

//! Copy protection chunk
const tint32 giChunkCopyProtection = 'CPPR';

//! MIDI CC info
const tint32 giChunkCCInfo = 'CCNF';

//! Arpeggiator (index of first parameter (tint32), parameter count(tint32), parameter data(tint32[]))
const tint32 giChunkArp = '_ARP';

//! Notes (used in Nexsyn) (string, not zero terminated, variable length)
const tint32 giChunkNotes = 'NOTE';


/*! \class IChunkFile
 * \brief Interface to open and access file containing chunks.
 *
 * Besides opening (and closing) a file, you can access it's chunks using GetChunk, and make new chunks using SetChunk
 * Currently SetChunk() only works with chunks residing in memory
*/
class IChunkFile : public virtual IDestructable
{
public:
	//! Create IChunkFile
	static IChunkFile* Create();

	//! Open chunk file
	/*!
		\param pszPathName [in]: (Full) path name of file to open
		\param AccessType [in]: Access type (read / write / create)
		\param iVersionNumber [in/out]: For write/create [in]: Version number to write in file. For read [out]: Version number of file
	*/
	virtual bool Open(const tchar* pszPathName, IFile::EOpenFile AccessType, tint32& riVersionNumber, tbool bHasHeader = true) = 0;

	//! Close is automatically called at destruction time, but if desired it can be called manually
	virtual void Close() = 0;

	//! Get a chunk (from file openened with read access)
	/*!
		\param iID [in]: Chunk ID
		\param bCopyMem [in]: Param only does any difference if the IChunkFile is a CChunkFileMemory.<br>True: A new memory area is allocated so the chunk doesn't operate on the same buffer as the CChunkFileMemory. False: The chunk shares the same memory as the CChunkFileMemory (use with caution for speed and memory optimization)
		\return IChunk*: Pointer to chunk, or NULL if not found
	*/
	virtual IChunk* GetChunk(tint32 iID, tbool bCopyMem = true) = 0;

	//! Get next chunk (from file openened with read access) from a given position.
	/*!
		\param rIndex [in / out]: Position to search from, or 0. GetNextChunk will update the position so it can be passed to the call again
		\param iID [in]: Chunk ID. If 0 the next chunk with any ID will be returned.
		\param bCopyMem [in]: Param only does any difference if the IChunkFile is a CChunkFileMemory.<br>True: A new memory area is allocated so the chunk doesn't operate on the same buffer as the CChunkFileMemory. False: The chunk shares the same memory as the CChunkFileMemory (use with caution for speed and memory optimization)
		\return IChunk*: Pointer to chunk, or NULL if not found
	*/
	virtual IChunk* GetNextChunk(tint64& rIndex, tint32 iID = 0, tbool bCopyMem = true) = 0;

	//! Set a chunk (from file openened with write / create access)
	/*!
		\param pChunk [in]: Pointer to chunk
		\param bIsMemoryChunk [in]: If true this is a memory chunk. This is neccasary due to a bug in XCode
	*/
	virtual void SetChunk(IChunk* pChunk, tbool bIsMemoryChunk) = 0;
};

