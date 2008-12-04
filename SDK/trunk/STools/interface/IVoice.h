


class IVoice
{
public:
	virtual void ST_CALL SetSampleFreq(tfloat fSampleFreq) = 0;
	virtual void ST_CALL SetMaxBufferSize(tint lMaxBufferSize) = 0;

	// Note: bLegato currently only works with VoiceStealingFade.
	// If voice mode != VoiceModeMono bLegato is always false
	// If bLegato == true, the voice can choose to continue with it's current setttings
	virtual void ST_CALL EventNoteOn(tuint ulNote, tuint ulVelocity, tuint ulChannel, tbool bLegato) = 0;
	virtual void ST_CALL EventNoteOff(tuint ulVelocity) = 0;
	virtual void ST_CALL EventVoiceDone() = 0;

	// Returns false if voice ended, true otherwise
	virtual tbool ST_CALL Process(tfloat32* pfOut, tint lC) = 0;
	virtual tbool ST_CALL ProcessStereo(tfloat32* pfOutL, tfloat32* pfOutR, tint lC) = 0;

	// Below are the vector versions. They're only called if all addresses are dividable with 16, and the length equals the max sub buffer size
	virtual tbool ST_CALL ProcessVector(tfloat32* pfOut) = 0;
	virtual tbool ST_CALL ProcessVectorStereo(tfloat32* pfOutL, tfloat32* pfOutR) = 0;
};


class IMonoSection
{
public:
	virtual void ST_CALL SetSampleFreq(tfloat fSampleFreq) = 0;
	virtual void ST_CALL SetMaxBufferSize(tint lMaxBufferSize) = 0;

	// A final mono section processes *pf data inline
	// An initial mono section does not produce output data and thus ingores pf
	virtual void ST_CALL Process(tfloat32* pf, tint lC) = 0;
	virtual void ST_CALL ProcessStereo(tfloat32* pfL, tfloat32* pfR, tint lC) = 0;

	// Below are the vector versions. They're only called if all addresses are dividable with 16, and the length equals the max sub buffer size
	virtual void ST_CALL ProcessVector(tfloat32* pf) = 0;
	virtual void ST_CALL ProcessVectorStereo(tfloat32* pfL, tfloat32* pfR) = 0;
};


