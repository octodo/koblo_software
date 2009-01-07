
class CWave_Parser : 
public virtual CLoad_Error
{
public:
	CWave_Parser();
	virtual ~CWave_Parser();


	bool Load(IFile* pFile, tbool bLoadData = true);

	tint			GetSampleRate()		const	{return miSampleRate;}
	// Returns -1 if no loop
	tint			GetLoopStart()		const	{return miLoopStart;}
	// Returns -1 if no loop
	tint			GetLoopEnd()		const	{return miLoopEnd;}
	tfloat		GetBaseFreq()		const	{return mfBaseFreq;}
	tint			GetSampleCount()	const	{return miSampleCount;}
	const tchar*	GetData()			const	{return mpData;}
	tint32 GetDataOffset() const {return miDataOffset;}

	tint32 GetBitDepth() {return miBitDepth;}
	tint32 GetSampleRate() {return miSampleRate;}
	tint32 GetChannels() {return miChannels;}

protected:
	// This is the info we want to read
	tint		miSampleRate;
	tint		miLoopStart;
	tint		miLoopEnd;
	tfloat	mfBaseFreq;
	tint		miSampleCount;
	tchar*	mpData;
	tint32 miDataOffset;

	tint32 miBitDepth;

	tint32 miChannels;
};





