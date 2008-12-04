/*!	\file CStream.h
	\author Michael Olsen
	\date 19/Jun/2007
*/


// In samples
// Original synth code:
//const tint32 giBufferSize = 1024 * 32;
// Lasse; modified 2008-11-07 - that's way too much! Very slow for many tracks, especially on slow HD machine ..
// .. makes playback delay on start and sometimes sound falls out in the middle
//const tint32 giBufferSize = 1024 * 32 * 4;
const tint32 giBufferSize = 2*1024;
// .. Lasse
// Lasse modified 2008-11-07 - We'd rather up the amount of buffers
//const tint32 giNrOfBuffers = 16;
const tint32 giNrOfBuffers = 64;
// .. Lasse
// Max number of buffers we try to read in 1 go
const tint32 giNrOfBuffersToReadMax = 8;

#ifdef WIN32
#pragma pack(push, 1)
#endif	// WIN32
struct SSample24
{
	unsigned char a;
	unsigned char b;
	char c;
} PACKED;
#ifdef WIN32
#pragma pack(pop)
#endif	// WIN32

class CStream : public virtual IStream, public virtual IDestructable
{
public:
	//! Create IStream
	static IStream* Create();

	//! Constructor
	CStream();

	//! Destructor
	~CStream();

	//! IDestructable override
	virtual void Destroy();

	//! IStream override
	virtual void GetSamples(tfloat32* pfBuffer, tint32 iCount);
	//! IStream override
	virtual void Reset(IFile* pFile, tint32 iOffset, tint32 iLength, tbool bLoop, tint16* piInitialData, tint32 iBitWidth = 24, tint32 iChannels = 1);
	//! IStream override
	virtual void SetPosition(tuint64 uiPos);
	//! IStream override
	virtual tbool GetStutter();

	//! Called by the IStreamManager to fill buffers
	virtual void OnTimer();

protected:
	//! Mutex to lock access from reading/writing
	//CMutex mMutex;

	//! File to read from. Cached in Reset()
	IFile* mpFile;

	//! File offset in bytes
	tint64 miFileOffset;
	//! File length in samples
	tint64 miSamples;
	//! Current read index in samples. Next byte to read from is miCurIndex * samplelength + miFileOffset
	tint64 miCurIndex;

	tint32 miBitWidth;
	tint32 miByteWidth;

	volatile tbool mbStutter;

	//! Stream buffers
//	std::vector<tint16*> mpBuffers;
	SSample24* mpBuffers[giNrOfBuffers];

	//! Buffer currently being read
	volatile tint32 miBufferCur;
	//! Current index into buffer currently being read
	volatile tint32 miBufferIndex;

	//! Next buffer to fill.
	volatile tint32 miNextBufferToFill;

	//! Number of buffers we need to fill
	volatile tint32 miBuffersToFill;

	//! True if we're past end of file
	volatile tbool mbEmpty;

	//! True if we should loop when reaching the endpoint
	tbool mbLoop;

	//! Larger than 0 when in portion of code that we mustn't interrupt - to avoid catching up with ourselves between threads
	volatile tint32 miInCriticalCode_Level;

	//! Same as GetSamples, except it is guaranteed that we only need to read from 1 buffer
	virtual void GetSamplesFrom1Buffer(tfloat32* pfBuffer, tint32 iCount);
};
