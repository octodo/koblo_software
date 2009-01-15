#ifndef _st_xlat_h
#define _st_xlat_h
// Created by Lasse Steen Bohnstedt, 2008-09-xx


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


class Xlat {
public:

static tchar* PrepareWhateverBuff(tchar** ppcBuff, int* piCurrentSize, int iRequiredSize, tbool bPreserveDataWhenGrowing = true);
// ---- XLAT from single 16 bit mono PCM ----

static void PCM16Mono_PCM16Stereo(
	tchar* pcPCM16Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM16Interleaved, tint32* piAllocatedBytes);

static void PCM16Mono_PCM24Mono(
	tchar* pcPCM16Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Mono, tint32* piAllocatedBytes);

static void PCM16Mono_PCM24Stereo(
	tchar* pcPCM16Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Interleaved, tint32* piAllocatedBytes);

static void PCM16Mono_AF32Mono(
	tchar* pcPCM16Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Mono, tint32* piAllocatedBytes,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void PCM16Mono_DualAF32Mono(
	tchar* pcPCM16Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Left, tint32* piAllocatedLeft,
	tchar** ppcAF32Right, tint32* piAllocatedRight,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);


// ---- XLAT from dual 16 bit mono PCM ----

static void DualPCM16Mono_PCM16Mono_InPlace(
	tchar* pcPCM16Left, tchar* pcPCM16Right, tint32 iInputBytes);

static void DualPCM16Mono_PCM16Stereo(
	tchar* pcPCM16Left, tchar* pcPCM16Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM16Interleaved, tint32* piAllocatedBytes);

static void DualPCM16Mono_PCM24Mono(
	tchar* pcPCM16Left, tchar* pcPCM16Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24DownMix, tint32* piAllocatedBytes);

static void DualPCM16Mono_PCM24Stereo(
	tchar* pcPCM16Left, tchar* pcPCM16Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Interleaved, tint32* piAllocatedBytes);

static void DualPCM16Mono_AF32Mono(
	tchar* pcPCM16Left, tchar* pcPCM16Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32DownMix, tint32* piAllocatedBytes,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void DualPCM16Mono_DualAF32Mono(
	tchar* pcPCM16Left, tchar* pcPCM16Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Left, tint32* piAllocatedBytesLeft, tchar** ppcAF32Right, tint32* piAllocatedBytesRight,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);


// ---- XLAT from 16 bit interleaved (stereo) PCM ----

static void PCM16Stereo_PCM16Mono_InPlace(
	tchar* pcPCM16Interleaved, tint32 iInputBytes, tint32* piOutputBytes);

static void PCM16Stereo_DualPCM16Mono_PartialInPlace(
	tchar* pcPCM16Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** pcPCM16Right, tint32* piAllocatedBytesRight);

static void PCM16Stereo_PCM24Mono_InPlace(
	tchar* pcPCM16Interleaved, tint32 iInputBytes, tint32* piOutputBytes);

static void PCM16Stereo_DualPCM24Mono_PartialInPlace(
	tchar* pcPCM16Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Right, tint32* piAllocatedBytesRight);

static void PCM16Stereo_PCM24Stereo(
	tchar* pcPCM16Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Interleaved, tint32* piAllocatedBytes);

static void PCM16Stereo_AF32Mono_InPlace(
	tchar* pcPCM16Interleaved, tint32 iInputBytes,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void PCM16Stereo_AF32Mono(
	tchar* pcPCM16Interleaved, tint32 iInputBytes,
	tchar** ppcAF32Mono, tint32* piAllocatedBytes,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void PCM16Stereo_DualAF32Mono_PartialInPlace(
	tchar* pcPCM16Interleaved, tint32 iInputBytes,
	tchar** ppcPCM24Right, tint32* piAllocatedBytesRight,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void PCM16Stereo_DualAF32Mono(
	tchar* pcPCM16Interleaved, tint32 iInputBytes,
	tchar** ppcAF32Left, tint32* piAllocatedLeft, tchar** ppcAF32Right, tint32* piAllocatedRight,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);


// ---- XLAT from single 24 bit mono PCM ----

static void PCM24Mono_PCM24Stereo(
	tchar* pcPCM24Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Interleaved, tint32* piAllocatedBytes);

static void PCM24Mono_PCM16Mono_InPlace(
	tchar* pcPCM24Mono, tint32 iInputBytes, tint32* piOutputBytes);

static void PCM24Mono_PCM16Stereo(
	tchar* pcPCM24Mono, tint32 iInputBytes, tint32* piOutputBytes, tchar** ppcPCM16Interleaved, tint32* piAllocatedBytes);

static void PCM24Mono_AF32Mono(
	tchar* pcPCM24Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Mono, tint32* piAllocatedBytes,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void PCM24Mono_DualAF32Mono(
	tchar* pcPCM24Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Left, tint32* piAllocatedLeft,
	tchar** ppcAF32Right, tint32* piAllocatedRight,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);


// ---- XLAT from dual 24 bit mono PCM ----

static void DualPCM24Mono_PCM24Mono(
	tchar* pcPCM24Left, tchar* pcPCM24Right, tint32 iInputBytes,
	tchar** ppcPCM24Mono, tint32* piAllocatedBytes);

static void DualPCM24Mono_PCM24Stereo(
	tchar* pcPCM24Left, tchar* pcPCM24Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Interleaved, tint32* piAllocatedBytes);

static void DualPCM24Mono_PCM16Mono_InPlace(
	tchar* pcPCM24Left, tchar* pcPCM24Right, tint32 iInputBytes, tint32* piOutputBytes);

static void DualPCM24Mono_PCM16Stereo(
	tchar* pcPCM24Left, tchar* pcPCM24Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM16Interleaved, tint32* piAllocatedBytes);

static void DualPCM24Mono_DualPCM16_InPlace(
	tchar* pcPCM24Left, tchar* pcPCM24Right, tint32 iInputBytes, tint32* piOutputBytes);

static void DualPCM24Mono_AF32Mono(
	tchar* pcPCM24Left, tchar* pcPCM24Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32DownMix, tint32* piAllocatedBytes,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void DualPCM24Mono_DualAF32Mono(
	tchar* pcPCM24Left, tchar* pcPCM24Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Left, tint32* piAllocatedLeft, tchar** ppcAF32Right, tint32* piAllocatedRight,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);


// ---- XLAT from 24 bit interleaved (stereo) PCM ----

static void PCM24Stereo_PCM24Mono_InPlace(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes);

static void PCM24Stereo_DualPCM24Mono_PartialInPlace(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Right, tint32* piAllocatedRight);

static void PCM24Stereo_PCM16Mono_InPlace(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes);

static void PCM24Stereo_DualPCM16Mono_PartialInPlace(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM16Right, tint32* piAllocatedBytes);

static void PCM24Stereo_PCM16Stereo_InPlace(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes);

static void PCM24Stereo_AF32Mono_InPlace(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void PCM24Stereo_AF32Mono(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Mono, tint32* piAllocatedBytes,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void PCM24Stereo_DualAF32Mono_PartialInPlace(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Right, tint32* piAllocatedBytes,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void PCM24Stereo_DualAF32Mono(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Left, tint32* piAllocatedLeft, tchar** ppcAF32Right, tint32* piAllocatedRight,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);


// ---- XLAT from 32 bit float mono array ----

static void AF32Mono_PCM16Mono_InPlace(
	tchar* pcAF32Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void AF32Mono_PCM16Mono(
	tchar* pcAF32Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM16Mono, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void AF32Mono_PCM16Stereo_InPlace(
	tchar* pcAF32Mono, tint32 iInputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void AF32Mono_PCM16Stereo(
	tchar* pcAF32Mono, tint32 iInputBytes,
	tchar** ppcPCM16Interleaved, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void AF32Mono_PCM24Mono_InPlace(
	tchar* pcAF32Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void AF32Mono_PCM24Mono(
	tchar* pcAF32Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Mono, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void AF32Mono_PCM24Stereo(
	tchar* pcAF32Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Interleaved, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void AF32Mono_AF32Mono_InPlace(
	tchar* pcAF32Mono, tint32 iInputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeInMin = -1.0f, tfloat32 fRangeInMax = 1.0f,
	tfloat32 fRangeOutMin = -1.0f, tfloat32 fRangeOutMax = 1.0f);

static void AF32Mono_AF32Mono(
	tchar* pcAF32Mono, tint32 iInputBytes,
	tchar** ppcAF32MonoOut, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeInMin = -1.0f, tfloat32 fRangeInMax = 1.0f,
	tfloat32 fRangeOutMin = -1.0f, tfloat32 fRangeOutMax = 1.0f);


// ---- XLAT from dual 32 bit float mono array ----

static void DualAF32Mono_AF32Mono_InPlace(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void DualAF32Mono_AF32Mono(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes,
	tchar** ppcAF32Mono, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void DualAF32Mono_DualAF32Mono_InPlace(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeInMin = -1.0f, tfloat32 fRangeInMax = 1.0f,
	tfloat32 fRangeOutMin = -1.0f, tfloat32 fRangeOutMax = 1.0f);

static void DualAF32Mono_DualAF32Mono(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes,
	tchar** ppcAF32Left, tint32* piAllocatedLeft, tchar** ppcAF32Right, tint32* piAllocatedRight,
	tint64* piAccumOverflows,
	tfloat32 fRangeInMin = -1.0f, tfloat32 fRangeInMax = 1.0f,
	tfloat32 fRangeOutMin = -1.0f, tfloat32 fRangeOutMax = 1.0f);

static void DualAF32Mono_PCM16Mono_InPlace(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void DualAF32Mono_PCM16Mono(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM16Mono, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void DualAF32Mono_PCM16Stereo_InPlace(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void DualAF32Mono_PCM16Stereo(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes,
	tchar** ppcPCM16Interleaved, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void DualAF32Mono_DualPCM16Mono_InPlace(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void DualAF32Mono_DualPCM16Mono(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM16Left, tint32* piAllocatedLeft, tchar** ppcPCM16Right, tint32* piAllocatedRight,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void DualAF32Mono_PCM24Mono_InPlace(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void DualAF32Mono_PCM24Mono(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Mono, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void DualAF32Mono_PCM24Stereo(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Interleaved, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void DualAF32Mono_DualPCM24Mono_InPlace(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

static void DualAF32Mono_DualPCM24Mono(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Left, tint32* piAllocatedBytesLeft, tchar** ppcPCM24Right, tint32* piAllocatedBytesRight,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin = -1.0f, tfloat32 fRangeMax = 1.0f);

};

#endif // _st_xlat_h
