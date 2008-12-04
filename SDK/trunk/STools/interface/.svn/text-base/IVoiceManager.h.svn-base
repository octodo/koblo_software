

// This also includes reserve voices
#define MAX_POLYPHONY 256


class IVoiceManager
{
public:
	static IVoiceManager* ST_CALL Create();
	virtual void ST_CALL Destroy() = 0;

	enum EVoiceMode {
		VoiceModePoly = 0,
		VoiceModeMono,
		VoiceModeLegato
	};
	virtual void ST_CALL SetVoiceMode(EVoiceMode eVoiceMode) = 0;
	virtual void ST_CALL SetPolyphony(tint lPolyphony) = 0;
	// Time to fade out stolen voices
	virtual void ST_CALL SetFadeOutTime(tfloat fFadeOutTimeMS) = 0;
	// Number of voices in reserve
	virtual void ST_CALL SetVoiceReserve(tint lReserve) = 0;

	// Should be called lPolyphony + lVoiceReserver times
	virtual void ST_CALL AddVoice(IVoice* pVoice) = 0;

	// You can add an initial mono section which is called just before the playing voices
	// Call with pMonoSection == NULL to remove it
	virtual void ST_CALL AddInitialMonoSection(IMonoSection* pMonoSection) = 0;

	// You can add a final mono section which is called just before output
	// Call with pMonoSection == NULL to remove it
	virtual void ST_CALL AddFinalMonoSection(IMonoSection* pMonoSection) = 0;

	// Can be called multiple times before processing
	// Events can goes beyond the boundaries of the next processing
	virtual void ST_CALL AddMIDIEvents(SMIDIEvent* pEvents, tint lNrOfEvents) = 0;

	virtual void ST_CALL Process(tfloat* pfOutL, tfloat* pfOutR, tint lC) = 0;
	virtual void ST_CALL ProcessMono(tfloat* pfOut, tint lC) = 0;

	virtual void ST_CALL SetHQMode(tbool bMode) = 0;
	virtual void ST_CALL SetSampleRate(tfloat fSampleRate) = 0;
	// Max buffer size for client process calls
	virtual void ST_CALL SetMaxBufferSize(tint lMaxBufferSize) = 0;
	// If set, the manager will split the buffers into sub-buffers of max this size.
	// lMaxSubBufferSize has to be divideable by 2
	virtual void ST_CALL SetMaxSubBufferSize(tint lMaxSubBufferSize) = 0;
	virtual void ST_CALL Start() = 0;
	virtual void ST_CALL Stop() = 0;

	virtual void ST_CALL Panic() = 0;

	virtual tint ST_CALL GetNrOfVoicesInNoteOn() = 0;

	virtual tint ST_CALL GetVoiceMode() = 0;
};



