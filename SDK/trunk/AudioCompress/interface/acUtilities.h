#ifndef _ac_utilities_h
#define _ac_utilities_h
// Created by Lasse Steen Bohnstedt, 2008-08-xx

//! Identifies codecs used for encoding audio files
enum EAudioCodec {
	//! No codec (usual meaning: invalid setting)
	geAudioCodecUndefined,
	//! 16 or 24 bit RIFF/WAVE (we don't support lower rates)
	geAudioCodecWave,
	//! Lossless FLAC (not yet supported)
	geAudioCodecFLAC,
	//! Ogg/Vorbis (AoTuV encoder is used)
	geAudioCodecVorbis,
	//! Mp3 (LAME encoder is used)
	geAudioCodecLame,

	//! Used for sanity checks. Insert new codec types before this!
	geNbOfAudioCodecs
};


//! Encoding quality. Not directly translatable to a bit-rate, so the kbps listed are approximate. I got them by encoding a "wall-of-sound" rock-tune with lots of crash-rides and dubbed distorted guitars.
enum EQuality
{
	//! This indicates an unknown or invalid quality setting
	geQualityUnknown,

	//! wave: not valid<br/>flac: not valid<br/>ogg/vorbis: mono 28 kbps, stereo 33 kbps<br/>lame/mp3: mono 40 kbps, stereo 69 kbps
	geQualityPoor,
	
	//! wave: not valid<br/>flac: not valid<br/>ogg/vorbis: mono 50 kbps, stereo 66 kbps<br/>lame/mp3: mono 64 kbps, stereo 109 kbps
	geQualityLow,
	
	//! wave: not valid<br/>flac: not valid<br/>ogg/vorbis: mono 65 kbps, stereo 99 kbps<br/>lame/mp3: mono 76 kbps, stereo 134 kbps
	geQualityMedium,
	
	//! wave: not valid<br/>flac: not valid<br/>ogg/vorbis: mono 73 kbps, stereo 127 kbps<br/>lame/mp3: mono 86 kbps, stereo 168 kbps
	geQualityGood,
	
	//! wave: not valid<br/>flac: not valid<br/>ogg/vorbis: mono 78 kbps, stereo 144 kbps<br/>lame/mp3: mono 92 kbps, stereo 183 kbps
	geQualityBetter,
	
	//! wave: not valid<br/>flac: not valid<br/>ogg/vorbis: mono 85 kbps, stereo 179 kbps<br/>lame/mp3: mono 136 kbps, stereo 248 kbps
	geQualityHigh,
	
	//! wave: not valid<br/>flac: not valid<br/>ogg/vorbis: mono 95 kbps, stereo 204 kbps<br/>lame/mp3: mono 153 kbps, stereo 258 kbps
	geQualitySuper,
	
	//! wave: not valid<br/>flac: not valid<br/>ogg/vorbis: mono 120 kbps, stereo 247 kbps<br/>lame/mp3: mono 160 kbps, stereo 320 kbps
	geQualityExtreme,
	
	//! wave: not valid<br/>flac: not valid<br/>ogg/vorbis: mono 158 kbps, stereo 336 kbps<br/>lame/mp3: not valid
	geQualityInsane,
	
	//! wave: not valid<br/>flac: not valid<br/>ogg/vorbis: mono 240 kbps, stereo 487 kbps<br/>lame/mp3: not valid
	geQualityInsaner,
	
	//! wave: mono 706 kbps, stereo 1411 kbps<br/>flac: mono 388 kbps, stereo 776 kbps<br/>ogg/vorbis: not valid<br/>lame/mp3: not valid
	geQualityLossless16,

	//! wave: mono 1058 kbps, stereo 2117 kbps<br/>flac: mono 582 kbps, stereo 1164 kbps<br/>ogg/vorbis: not valid<br/>lame/mp3: not valid
	geQualityLossless24,
	
	//! wave, flac, ogg/vorbis, lame/mp3: not valid
	geQualityLossless32,
	
	//! Used for sanity checks. Insert new quality types before this!
	geNbOfQualities,

	//! This is not an actual quality. It will be translated to the default quality for the encoder
	geQualityDefault,

};


#ifdef _Mac_PowerPC
	// Big-endian Motorola CPU needs byte swapping for 24 bit ints
	
	// Expand from 24 to 32 bits during encode: swap and expand
	#define Inplace24to32(X) { \
		X = ((X >> 24) & 0x000000FF) | ((X >> 8) & 0x0000FF00) | ((X << 8) & 0x00FF0000); \
		if (X & 0x00800000) /* Negative */ X |= 0xFF000000; \
	}
	
	// From 32 to 24 bits during decode: swap
	#define Inplace32to24(X) { \
		X = ((X >> 8) & 0x0000FF00) | ((X << 8) & 0x00FF0000) | ((X << 24) & (0xFF000000)); \
	}
#else _Mac_PowerPC
	// Little-endian Intel CPU needs expansion of bit 24 (neg) to MSB to qualify as a 32 bit int

	// Expand from 24 to 32 bits during encode: expand
	#define Inplace24to32(X) { \
		if (X & 0x00800000) /* Negative */ X |= 0xFF000000; else /* positive */ X &= 0x00FFFFFF; \
	}
	
	// From 32 to 24 bits during decode: Do nothing (no need to mask MSB bits; we just allocate one extra byte to avoid overflow)
	#define Inplace32to24(X) X
#endif // #else _Mac_PowerPC


inline tchar* PrepareWhateverBuff(tchar** ppcBuff, int* piCurrentSize, int iRequiredSize, tbool bPreserveDataWhenGrowing = true)
{
	if (ppcBuff == NULL) {
		// This sucks! internal error
		ASSERT(FALSE);
		return NULL;
	}

	if (*ppcBuff) {
		// We have a previous buffer

		if ((piCurrentSize == NULL) || (*piCurrentSize < 0)) {
			// Current buffer really isn't handled by us - leave it alone
			// Do nothing
		}
		else if ((*piCurrentSize >= iRequiredSize) && (iRequiredSize > 0)) {
			// Current buffer is big enough - leave it alone
			// Do nothing
		}
		else if (iRequiredSize <= 0) {
			// Buffer is no longer needed
			delete[] *ppcBuff;
			*ppcBuff = NULL;
			*piCurrentSize = 0;
		}
		else {
			// Must delete old buffer and create a new, larger one

			// Backup old buffer
			tchar* pcOld = *ppcBuff;
			tint32 iSizeOld = *piCurrentSize;

			// Allocate new buffer with room for an extra integer in buffer
			// This to allow for faster 24 bit operations that may ignore 32/24 bit overflows
			*ppcBuff = new tchar[iRequiredSize +  sizeof(tint32)];
			*piCurrentSize = iRequiredSize;

			if (bPreserveDataWhenGrowing) {
				// Move old buffer contents to new
				memcpy(*ppcBuff, pcOld, iSizeOld);
			}

			// Delete old buffer
			delete[] pcOld;
		}
	}
	else {
		// No previous buffer

		if (iRequiredSize > 0) {
			// Allocate buffer with room for an extra integer in buffer
			// This to allow for faster 24 bit operations that may ignore 32/24 bit overflows
			*ppcBuff = new tchar[iRequiredSize +  sizeof(tint32)];
			if (piCurrentSize != NULL) {
				// Remember the new size
				*piCurrentSize = iRequiredSize;
			}
		}
	}
	return *ppcBuff;
} // PrepareWhateverBuff


//#include "acXlat.h"

#endif // _ac_utilities_h
