// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
//
// This file is part of the Koblo Stools.
//
// the Koblo Stools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo Stools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.

#define ST_MAX_MIDI_EVENTS 8192

// Max freq at which HQ mode can be activated
//#define ST_MAX_HQFREQ 120000
#define ST_MAX_HQFREQ 80000


#define MONO_STACK_SIZE 128

class CVoiceManager : public virtual IVoiceManager
{
public:
	CVoiceManager();
	virtual ~CVoiceManager();

	// IVoiceManager overrides
	virtual void ST_CALL Destroy();
	virtual void ST_CALL SetVoiceMode(EVoiceMode eVoiceMode);
	virtual void ST_CALL SetPolyphony(tint lPolyphony);
	virtual void ST_CALL SetFadeOutTime(tfloat fFadeOutTimeMS);
	virtual void ST_CALL SetVoiceReserve(tint lReserve);
	virtual void ST_CALL AddVoice(IVoice* pVoice);
	virtual void ST_CALL AddInitialMonoSection(IMonoSection* pMonoSection);
	virtual void ST_CALL AddFinalMonoSection(IMonoSection* pMonoSection);
	virtual void ST_CALL AddMIDIEvents(SMIDIEvent* pEvents, tint lNrOfEvents);
	virtual void ST_CALL Process(tfloat* pfOutL, tfloat* pfOutR, tint lC);
	virtual void ST_CALL ProcessMono(tfloat* pfOut, tint lC);
	virtual void ST_CALL SetHQMode(tbool bMode);
	virtual void ST_CALL SetSampleRate(tfloat fSampleRate);
	virtual void ST_CALL SetMaxBufferSize(tint lMaxBufferSize);
	virtual void ST_CALL SetMaxSubBufferSize(tint lMaxSubBufferSize);
	virtual void ST_CALL Start();
	virtual void ST_CALL Stop();
	virtual void ST_CALL Panic();
	virtual tint ST_CALL GetNrOfVoicesInNoteOn();
	virtual tint ST_CALL GetVoiceMode();

	virtual tbool GetHQMode() {return mbHQMode;}
	virtual tbool GetHQModeActive() {return mbHQModeActive;}

protected:
	struct SVoiceInfo {
		SVoiceInfo() : eStatus(StatusNotPlaying) {}

		enum EStatus {
			StatusNotPlaying = 0,
			StatusNoteOn,
			// Note off, but still playing
			StatusNoteOff
		};
		EStatus eStatus;

		// Only valid (obviously) if playing
		unsigned long ulNoteNum;
		unsigned long ulChannel;

		// If -1 -> not fading, otherwise fading
		// At reaching 0 the voice is stopped
		long lFadePosition;

		// If -1 -> not fading, otherwise fading
		// At reaching 0 is faded in
		long lFadeInPosition;

		// Tells whether voice is a legato voice
		bool bLegato;

		// Time for note on
		unsigned int uiTime;
	};
	SVoiceInfo mpVoiceInfo[MAX_POLYPHONY];

	bool mbHQMode;
	// Indicates whether we're actually running in hq mode
	bool mbHQModeActive;

	EVoiceMode meVoiceMode;
	long mlPolyphony;
	long mlReserve;
	long mlFadeTimeSamples;
	float mfSampleRate;
	float mfRealSampleRate;
	std::list<IVoice*> mVoices;
	SMIDIEvent mpMIDIEvents[ST_MAX_MIDI_EVENTS];
	SMIDIEvent mpMIDIEventsTmp[ST_MAX_MIDI_EVENTS+1];
	long mlMIDIEventIndexRead;
	long mlMIDIEventIndexWrite;
	// The delayed list is MIDI events we postpone
//	SMIDIEvent mpMIDIEventsDelayed[ST_MAX_MIDI_EVENTS];
//	long mlMIDIEventIndexDelayedRead;
//	long mlMIDIEventIndexDelayedWrite;
	IMonoSection* mpInitialMonoSection;
	IMonoSection* mpFinalMonoSection;
	unsigned int muiStreamTime;

	// If mono, pfOutR is ignored
	void ProcessFrame(float* pfOutL, float* pfOutR, long lC, long lChannels);
	// If mono, pfOutR is ignored
	void DoProcess(float* pfOutL, float* pfOutR, long lC, long lChannels);
	void DoMIDI(const SMIDIEvent& Event, bool bLegato);
	void DoNoteOn(unsigned long ulNote, unsigned long ulVelocity, unsigned long ulChannel, const SMIDIEvent& Event, bool bLegato);
	void DoNoteOff(unsigned long ulNote, unsigned long ulVelocity, unsigned long ulChannel);

	// Returns -1 if none found
	long FindFirstFreeVoice();
	// Returns -1 if none found. Only looks for NoteOn
	long FindFirstPlayingVoice();
	// Returns -1 if none found. Only looks for NoteOn (not voices in release phase)
	long FindFirstPlayingVoiceNoRelease();
	IVoice* GetVoice(long lIndex);
	// Finds voice index. Returns -1 if not found.
	long FindPlayingNote(unsigned long ulNote, unsigned long ulChannel);

	tfloat32* mpfTmpOutL;
	tfloat32* mpfTmpOutR;
	tfloat32* mpfTmpOutL2;
	tfloat32* mpfTmpOutR2;

//	IDownSampler* mpDownSamplerL;
//	IDownSampler* mpDownSamplerR;

	long mlMaxBufferSize;
	long mlMaxSubBufferSize;

	// Sort MIDI before processing
	void SortMIDI();

	// Free 1 voice, thus making room for Event. Returns -1 if there's no voice to free, otherwise fre'ed voice
	long FreeOneVoice(const SMIDIEvent& Event);
	// Called by FreeOnVoice(...), this does the actual freeing
	void ReleaseVoice(long lVoice);
	// As opposed to FreeOneVoice, this immeidately stops a voice (producing clicks). Also, it will always stop a voice (unless no voices are playing, in which case -1 is returned)
	long StopOneVoice(const SMIDIEvent& Event);
	// Called by processing to do the actual fading. Also stops the voice if neccasary
	void FadeOut(long lVoice, tfloat32* pf, long lC);
	void FadeOutStereo(long lVoice, tfloat32* pfL, tfloat32* pfR, long lC);
	// Called by processing to do fade-in (for legato)
	void FadeIn(long lVoice, tfloat32* pf, long lC);
	void FadeInStereo(long lVoice, tfloat32* pfL, tfloat32* pfR, long lC);

	// Stack for remembering previous voices in mono and mono legato mode
	SVoiceInfo mpMonoStack[MONO_STACK_SIZE];
	long mlMonoStackSize;

	// Interface for allocing/deallocing aligned memory
	IMemAllocator* mpAlignedMemAllocator;

	IDSPTools* mpDSP;

	float mfFadeOutTimeMS;

	// Keeps track of the current voice count
	int miNrOfVoicesPlaying;

	int GetNrOfVoicesPlaying();

	// Voices which are note-on or note-off, and doesn't fade
	int GetNrOfActiveVoices();

};

