#include "stoolsInternalOS.h"


tchar* Xlat::PrepareWhateverBuff(tchar** ppcBuff, int* piCurrentSize, int iRequiredSize, tbool bPreserveDataWhenGrowing /*= true*/)
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


// ---- XLAT from single 16 bit mono PCM ----

void Xlat::PCM16Mono_PCM16Stereo(
	tchar* pcPCM16Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM16Interleaved, tint32* piAllocatedBytes)
{
	tint32 iSamples = iInputBytes / 2;
	*piOutputBytes = iSamples * 2 * 2;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM16Interleaved, piAllocatedBytes, *piOutputBytes, false);

	for ( ; iSamples > 0; iSamples--) {
		tint16 i16 = *((tint16*)pcPCM16Mono);
		pcPCM16Mono += 2;

		*((tint16*)pcOutput) = i16;
		pcOutput += 2;
		*((tint16*)pcOutput) = i16;
		pcOutput += 2;
	}
};

void Xlat::PCM16Mono_PCM24Mono(
	tchar* pcPCM16Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Mono, tint32* piAllocatedBytes)
{
	tint32 iSamples = iInputBytes / 2;
	*piOutputBytes = iSamples * 3;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM24Mono, piAllocatedBytes, *piOutputBytes, false);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32 = CPSwap(*((tint16*)pcPCM16Mono));
		pcPCM16Mono += 2;

		// Convert range -32768 => 32767 to range -8388608 => 8388607
		i32 <<= 8;
		Inplace32to24(i32);

		*((tint32*)pcOutput) = i32;
		pcOutput += 3;
	}
};

void Xlat::PCM16Mono_PCM24Stereo(
	tchar* pcPCM16Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Interleaved, tint32* piAllocatedBytes)
{
	tint32 iSamples = iInputBytes / 2;
	*piOutputBytes = iSamples * 3;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM24Interleaved, piAllocatedBytes, *piOutputBytes, false);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32 = CPSwap(*((tint16*)pcPCM16Mono));
		pcPCM16Mono += 2;

		// Convert range -32768 => 32767 to range -8388608 => 8388607
		i32 <<= 8;
		Inplace32to24(i32);

		*((tint32*)pcOutput) = i32;
		pcOutput += 3;
		*((tint32*)pcOutput) = i32;
		pcOutput += 3;
	}
};

void Xlat::PCM16Mono_AF32Mono(
	tchar* pcPCM16Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Mono, tint32* piAllocatedBytes,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 2;
	*piOutputBytes = iSamples * 4;
	tchar* pcOutput = PrepareWhateverBuff(ppcAF32Mono, piAllocatedBytes, *piOutputBytes, false);

	// We go from range -32768 => 32767
	const tint32 kiAdditor = 0x8000;
	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	tfloat32 kfDivisor = (((tfloat32)0xFFFF) / fRangeFull);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32 = CPSwap(*((tint16*)pcPCM16Mono));
		pcPCM16Mono += 2;

		// Convert range -32768 => 32767 to range -1.0 => 1.0
		i32 += kiAdditor;
		tfloat32 f32 = (tfloat32)i32;
		f32 /= kfDivisor;
		f32 += fRangeMin;

		*((tfloat32*)pcOutput) = f32;
		pcOutput += 4;
	}
};

void Xlat::PCM16Mono_DualAF32Mono(
	tchar* pcPCM16Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Left, tint32* piAllocatedLeft,
	tchar** ppcAF32Right, tint32* piAllocatedRight,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 2;
	*piOutputBytes = iSamples * 4;
	tchar* pcOutputL = PrepareWhateverBuff(ppcAF32Left, piAllocatedLeft, *piOutputBytes, false);
	tchar* pcOutputR = PrepareWhateverBuff(ppcAF32Right, piAllocatedRight, *piOutputBytes, false);

	// We go from range -32768 => 32767
	const tint32 kiAdditor = 0x8000;
	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	tfloat32 kfDivisor = (((tfloat32)0xFFFF) / fRangeFull);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32 = CPSwap(*((tint16*)pcPCM16Mono));
		pcPCM16Mono += 2;

		// Convert range -32768 => 32767 to range -1.0 => 1.0
		i32 += kiAdditor;
		tfloat32 f32 = (tfloat32)i32;
		f32 /= kfDivisor;
		f32 += fRangeMin;

		*((tfloat32*)pcOutputL) = f32;
		pcOutputL += 4;
		*((tfloat32*)pcOutputR) = f32;
		pcOutputR += 4;
	}
};


// ---- XLAT from dual 16 bit mono PCM ----

void Xlat::DualPCM16Mono_PCM16Mono_InPlace(
	tchar* pcPCM16Left, tchar* pcPCM16Right, tint32 iInputBytes)
{
	tint32 iSamples = iInputBytes / 2;
	tchar* pcOutput = pcPCM16Left;

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32 = CPSwap(*((tint16*)pcPCM16Left));
		pcPCM16Left += 2;
		i32 += CPSwap(*((tint16*)pcPCM16Right));
		pcPCM16Right += 2;

		// Downmix 16 bits dual mono to 16 bit mono
		tint16 i16 = (tint16)(i32 >> 1);
		i16 = CPSwap(i16);

		*((tint16*)pcOutput) = i16;
		pcOutput += 2;
	}
};

void Xlat::DualPCM16Mono_PCM16Stereo(
	tchar* pcPCM16Left, tchar* pcPCM16Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM16Interleaved, tint32* piAllocatedBytes)
{
	tint32 iSamples = iInputBytes / 2;
	*piOutputBytes = iSamples * 2 * 2;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM16Interleaved, piAllocatedBytes, *piOutputBytes, false);

	for ( ; iSamples > 0; iSamples--) {
		tint16 i16L = *((tint16*)pcPCM16Left);
		pcPCM16Left += 2;
		tint16 i16R = *((tint16*)pcPCM16Right);
		pcPCM16Right += 2;

		*((tint16*)pcOutput) = i16L;
		pcOutput += 2;
		*((tint16*)pcOutput) = i16R;
		pcOutput += 2;
	}
};

void Xlat::DualPCM16Mono_PCM24Mono(
	tchar* pcPCM16Left, tchar* pcPCM16Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24DownMix, tint32* piAllocatedBytes)
{
	tint32 iSamples = iInputBytes / 2;
	*piOutputBytes = iSamples * 3;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM24DownMix, piAllocatedBytes, *piOutputBytes, false);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32 = CPSwap(*((tint16*)pcPCM16Left));
		pcPCM16Left += 2;
		i32 += CPSwap(*((tint16*)pcPCM16Right));
		pcPCM16Right += 2;

		// Downmix 16 bits dual mono to 24 bit mono
		i32 <<= 7;
		Inplace32to24(i32);

		*((tint32*)pcOutput) = i32;
		pcOutput += 3;
	}
};

void Xlat::DualPCM16Mono_PCM24Stereo(
	tchar* pcPCM16Left, tchar* pcPCM16Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Interleaved, tint32* piAllocatedBytes)
{
	tint32 iSamples = iInputBytes / 2;
	*piOutputBytes = iSamples * 3 * 2;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM24Interleaved, piAllocatedBytes, *piOutputBytes, false);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = CPSwap(*((tint16*)pcPCM16Left));
		pcPCM16Left += 2;
		tint32 i32R = CPSwap(*((tint16*)pcPCM16Right));
		pcPCM16Right += 2;

		// Convert range -32768 => 32767 to range -8388608 => 8388607
		i32L <<= 8;
		Inplace32to24(i32L);
		i32R <<= 8;
		Inplace32to24(i32R);

		*((tint32*)pcOutput) = i32L;
		pcOutput += 3;
		*((tint32*)pcOutput) = i32R;
		pcOutput += 3;
	}
};

void Xlat::DualPCM16Mono_AF32Mono(
	tchar* pcPCM16Left, tchar* pcPCM16Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32DownMix, tint32* piAllocatedBytes,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 2;
	*piOutputBytes = iSamples * 4;
	tchar* pcOutput = PrepareWhateverBuff(ppcAF32DownMix, piAllocatedBytes, *piOutputBytes, false);

	// We go from range (2 x -32768) => (2 x 32767)
	const tint32 kiAdditor = 2 * 0x8000;
	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	// x 2 because we're at double volume due to downmix
	tfloat32 kfDivisor = (((tfloat32)0xFFFF) * 2 / fRangeFull);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32 = CPSwap(*((tint16*)pcPCM16Left));
		pcPCM16Left += 2;
		i32 += CPSwap(*((tint16*)pcPCM16Right));
		pcPCM16Right += 2;

		// Downmix 16 bits dual mono (range -32768 => 32767) to 32 bit float mono (range -1.0 => 1.0)
		i32 += kiAdditor;
		tfloat32 f32 = (tfloat32)i32;
		f32 /= kfDivisor;
		f32 += fRangeMin;

		*((tfloat32*)pcOutput) = f32;
		pcOutput += 4;
	}
};

void Xlat::DualPCM16Mono_DualAF32Mono(
	tchar* pcPCM16Left, tchar* pcPCM16Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Left, tint32* piAllocatedBytesLeft, tchar** ppcAF32Right, tint32* piAllocatedBytesRight,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 2;
	*piOutputBytes = iSamples * 4;
	tchar* pcOutputLeft = PrepareWhateverBuff(ppcAF32Left, piAllocatedBytesLeft, *piOutputBytes, false);
	tchar* pcOutputRight = PrepareWhateverBuff(ppcAF32Right, piAllocatedBytesRight, *piOutputBytes, false);

	// We go from range -32768 => 32767
	const tint32 kiAdditor = 0x8000;
	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	tfloat32 kfDivisor = (((tfloat32)0xFFFF) / fRangeFull);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = CPSwap(*((tint16*)pcPCM16Left));
		pcPCM16Left += 2;
		tint32 i32R = CPSwap(*((tint16*)pcPCM16Right));
		pcPCM16Right += 2;

		// Convert range -32768 => 32767 to range -1.0 => 1.0
		i32L += kiAdditor;
		tfloat32 f32L = (tfloat32)i32L;
		f32L /= kfDivisor;
		f32L += fRangeMin;
		//
		i32R += kiAdditor;
		tfloat32 f32R = (tfloat32)i32R;
		f32R /= kfDivisor;
		f32R += fRangeMin;

		*((tfloat32*)pcOutputLeft) = f32L;
		pcOutputLeft += 4;
		*((tfloat32*)pcOutputRight) = f32R;
		pcOutputRight += 4;
	}
};


// ---- XLAT from 16 bit interleaved (stereo) PCM ----

void Xlat::PCM16Stereo_PCM16Mono_InPlace(
	tchar* pcPCM16Interleaved, tint32 iInputBytes, tint32* piOutputBytes)
{
	tint32 iSamples = iInputBytes / (2 * 2);
	*piOutputBytes = iSamples * 2;
	tchar* pcOutput = pcPCM16Interleaved;

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32 = CPSwap(*((tint16*)pcPCM16Interleaved));
		pcPCM16Interleaved += 2;
		i32 += CPSwap(*((tint16*)pcPCM16Interleaved));
		pcPCM16Interleaved += 2;

		// Downmix 16 bits stereo to 16 bit mono
		tint16 i16 = (tint16)(i32 >> 1);
		i16 = CPSwap(i16);

		*((tint16*)pcOutput) = i16;
		pcOutput += 2;
	}
};

void Xlat::PCM16Stereo_DualPCM16Mono_PartialInPlace(
	tchar* pcPCM16Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** pcPCM16Right, tint32* piAllocatedBytesRight)
{
	tint32 iSamples = iInputBytes / (2 * 2);
	*piOutputBytes = iSamples * 2;
	tchar* pcOutputLeft = pcPCM16Interleaved;
	tchar* pcOutputRight = PrepareWhateverBuff(pcPCM16Right, piAllocatedBytesRight, *piOutputBytes, false);

	for ( ; iSamples > 0; iSamples--) {
		tint16 i16L = *((tint16*)pcPCM16Interleaved);
		pcPCM16Interleaved += 2;
		tint16 i16R = *((tint16*)pcPCM16Interleaved);
		pcPCM16Interleaved += 2;

		*((tint16*)pcOutputLeft) = i16L;
		pcOutputLeft += 2;		
		*((tint16*)pcOutputRight) = i16R;
		pcOutputRight += 2;
	}
};

void Xlat::PCM16Stereo_PCM24Mono_InPlace(
	tchar* pcPCM16Interleaved, tint32 iInputBytes, tint32* piOutputBytes)
{
	tint32 iSamples = iInputBytes / (2 * 2);
	*piOutputBytes = iSamples * 3;
	tchar* pcOutput = pcPCM16Interleaved;

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32 = CPSwap(*((tint16*)pcPCM16Interleaved));
		pcPCM16Interleaved += 2;
		i32 += CPSwap(*((tint16*)pcPCM16Interleaved));
		pcPCM16Interleaved += 2;

		// Downmix 16 bits stereo to 24 bit mono
		i32 <<= 7;
		Inplace32to24(i32);

		*((tint32*)pcOutput) = i32;
		pcOutput += 3;
	}
};

void Xlat::PCM16Stereo_DualPCM24Mono_PartialInPlace(
	tchar* pcPCM16Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Right, tint32* piAllocatedBytesRight)
{
	tint32 iSamples = iInputBytes / (2 * 2);
	*piOutputBytes = iSamples * 3;
	tchar* pcOutputLeft = pcPCM16Interleaved;
	tchar* pcOutputRight = PrepareWhateverBuff(ppcPCM24Right, piAllocatedBytesRight, *piOutputBytes, false);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = CPSwap(*((tint16*)pcPCM16Interleaved));
		pcPCM16Interleaved += 2;
		tint32 i32R = CPSwap(*((tint16*)pcPCM16Interleaved));
		pcPCM16Interleaved += 2;

		// Convert range -32768 => 32767 to range -8388608 => 8388607
		i32L <<= 8;
		Inplace32to24(i32L);
		i32R <<= 8;
		Inplace32to24(i32R);

		*((tint32*)pcOutputLeft) = i32L;
		pcOutputLeft += 3;		
		*((tint32*)pcOutputRight) = i32R;
		pcOutputRight += 3;
	}
};

void Xlat::PCM16Stereo_PCM24Stereo(
	tchar* pcPCM16Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Interleaved, tint32* piAllocatedBytes)
{
	tint32 iSamples = iInputBytes / (2 * 2);
	*piOutputBytes = iSamples * 3 * 2;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM24Interleaved, piAllocatedBytes, *piOutputBytes, false);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = CPSwap(*((tint16*)pcPCM16Interleaved));
		pcPCM16Interleaved += 2;
		tint32 i32R = CPSwap(*((tint16*)pcPCM16Interleaved));
		pcPCM16Interleaved += 2;

		// Convert range -32768 => 32767 to range -8388608 => 8388607
		i32L <<= 8;
		Inplace32to24(i32L);
		i32R <<= 8;
		Inplace32to24(i32R);

		*((tint32*)pcOutput) = i32L;
		pcOutput += 3;		
		*((tint32*)pcOutput) = i32R;
		pcOutput += 3;
	}
};

void Xlat::PCM16Stereo_AF32Mono_InPlace(
	tchar* pcPCM16Interleaved, tint32 iInputBytes,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / (2 * 2);
	tchar* pcOutput = pcPCM16Interleaved;

	// We go from range (2 x -32768) => (2 x 32767)
	const tint32 kiAdditor = 2 * 0x8000;
	// x 2 because we're at double volume due to downmix
	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	tfloat32 kfDivisor = (((tfloat32)0xFFFF) * 2 / fRangeFull);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32 = CPSwap(*((tint16*)pcPCM16Interleaved));
		pcPCM16Interleaved += 2;
		i32 += CPSwap(*((tint16*)pcPCM16Interleaved));
		pcPCM16Interleaved += 2;

		// Downmix 16 bits dual mono (range -32768 => 32767) to 32 bit float mono (range -1.0 => 1.0)
		i32 += kiAdditor;
		tfloat32 f32 = (tfloat32)i32;
		f32 /= kfDivisor;
		f32 += fRangeMin;

		*((tfloat32*)pcOutput) = f32;
		pcOutput += 4;
	}
};

void Xlat::PCM16Stereo_AF32Mono(
	tchar* pcPCM16Interleaved, tint32 iInputBytes,
	tchar** ppcAF32Mono, tint32* piAllocatedBytes,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / (2 * 2);
	tchar* pcOutput = PrepareWhateverBuff(ppcAF32Mono, piAllocatedBytes, iInputBytes, false);

	// We go from range (2 x -32768) => (2 x 32767)
	const tint32 kiAdditor = 2 * 0x8000;
	// x 2 because we're at double volume due to downmix
	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	tfloat32 kfDivisor = (((tfloat32)0xFFFF) * 2 / fRangeFull);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32 = CPSwap(*((tint16*)pcPCM16Interleaved));
		pcPCM16Interleaved += 2;
		i32 += CPSwap(*((tint16*)pcPCM16Interleaved));
		pcPCM16Interleaved += 2;

		// Downmix 16 bits dual mono (range -32768 => 32767) to 32 bit float mono (range -1.0 => 1.0)
		i32 += kiAdditor;
		tfloat32 f32 = (tfloat32)i32;
		f32 /= kfDivisor;
		f32 += fRangeMin;

		*((tfloat32*)pcOutput) = f32;
		pcOutput += 4;
	}
};

void Xlat::PCM16Stereo_DualAF32Mono_PartialInPlace(
	tchar* pcPCM16Interleaved, tint32 iInputBytes,
	tchar** ppcAF32Right, tint32* piAllocatedBytesRight,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / (2 * 2);
	tchar* pcOutputL = pcPCM16Interleaved;
	tchar* pcOutputR = PrepareWhateverBuff(ppcAF32Right, piAllocatedBytesRight, iInputBytes, false);

	// We go from range -32768 => 32767
	const tint32 kiAdditor = 0x8000;
	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	tfloat32 kfDivisor = (((tfloat32)0xFFFF) / fRangeFull);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = CPSwap(*((tint16*)pcPCM16Interleaved));
		pcPCM16Interleaved += 2;
		tint32 i32R = CPSwap(*((tint16*)pcPCM16Interleaved));
		pcPCM16Interleaved += 2;

		// Convert range -32768 => 32767 to range -1.0 => 1.0
		i32L += kiAdditor;
		tfloat32 f32L = (tfloat32)i32L;
		f32L /= kfDivisor;
		f32L += fRangeMin;
		//
		i32R += kiAdditor;
		tfloat32 f32R = (tfloat32)i32R;
		f32R /= kfDivisor;
		f32R += fRangeMin;

		*((tfloat32*)pcOutputL) = f32L;
		pcOutputL += 4;
		*((tfloat32*)pcOutputR) = f32R;
		pcOutputR += 4;
	}
};


void Xlat::PCM16Stereo_DualAF32Mono(
	tchar* pcPCM16Interleaved, tint32 iInputBytes,
	tchar** ppcAF32Left, tint32* piAllocatedBytesLeft, tchar** ppcAF32Right, tint32* piAllocatedBytesRight,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / (2 * 2);
	tchar* pcOutputL = PrepareWhateverBuff(ppcAF32Left, piAllocatedBytesLeft, iInputBytes, false);
	tchar* pcOutputR = PrepareWhateverBuff(ppcAF32Right, piAllocatedBytesRight, iInputBytes, false);

	// We go from range -32768 => 32767
	const tint32 kiAdditor = 0x8000;
	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	tfloat32 kfDivisor = (((tfloat32)0xFFFF) / fRangeFull);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = CPSwap(*((tint16*)pcPCM16Interleaved));
		pcPCM16Interleaved += 2;
		tint32 i32R = CPSwap(*((tint16*)pcPCM16Interleaved));
		pcPCM16Interleaved += 2;

		// Convert range -32768 => 32767 to range -1.0 => 1.0
		i32L += kiAdditor;
		tfloat32 f32L = (tfloat32)i32L;
		f32L /= kfDivisor;
		f32L += fRangeMin;
		//
		i32R += kiAdditor;
		tfloat32 f32R = (tfloat32)i32R;
		f32R /= kfDivisor;
		f32R += fRangeMin;

		*((tfloat32*)pcOutputL) = f32L;
		pcOutputL += 4;
		*((tfloat32*)pcOutputR) = f32R;
		pcOutputR += 4;
	}
};


// ---- XLAT from single 24 bit mono PCM ----

void Xlat::PCM24Mono_PCM24Stereo(
	tchar* pcPCM24Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Interleaved, tint32* piAllocatedBytes)
{
	tint32 iSamples = iInputBytes / 3;
	*piOutputBytes = iSamples * 3 * 2;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM24Interleaved, piAllocatedBytes, *piOutputBytes, false);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32 = *((tint32*)pcPCM24Mono);
		pcPCM24Mono += 3;

		*((tint32*)pcOutput) = i32;
		pcOutput += 3;
		*((tint32*)pcOutput) = i32;
		pcOutput += 3;
	}
};

void Xlat::PCM24Mono_PCM16Mono_InPlace(
	tchar* pcPCM24Mono, tint32 iInputBytes, tint32* piOutputBytes)
{
	tint32 iSamples = iInputBytes / 3;
	*piOutputBytes = iSamples * 2;
	tchar* pcOutput = pcPCM24Mono;

	// Skip lowest byte little-endian style since PCM is Intel native (24 bits => 16 bits)
	pcPCM24Mono++;

	for ( ; iSamples > 0; iSamples--) {
		tint16 i16 = *((tint16*)pcPCM24Mono);
		pcPCM24Mono += 3;

		*((tint16*)pcOutput) = i16;
		pcOutput += 2;
	}
};

void Xlat::PCM24Mono_PCM16Stereo(
	tchar* pcPCM24Mono, tint32 iInputBytes, tint32* piOutputBytes, tchar** ppcPCM16Interleaved, tint32* piAllocatedBytes)
{
	tint32 iSamples = iInputBytes / 3;
	*piOutputBytes = iSamples * 2 * 2;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM16Interleaved, piAllocatedBytes, *piOutputBytes, false);

	// Skip lowest byte little-endian style since PCM is Intel native (24 bits => 16 bits)
	pcPCM24Mono++;

	for ( ; iSamples > 0; iSamples--) {
		tint16 i16 = *((tint16*)pcPCM24Mono);
		pcPCM24Mono += 3;

		*((tint16*)pcOutput) = i16;
		pcOutput += 2;
		*((tint16*)pcOutput) = i16;
		pcOutput += 2;
	}
};

void Xlat::PCM24Mono_AF32Mono(
	tchar* pcPCM24Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Mono, tint32* piAllocatedBytes,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 3;
	*piOutputBytes = iSamples * 4;
	tchar* pcOutput = PrepareWhateverBuff(ppcAF32Mono, piAllocatedBytes, *piOutputBytes, false);

	// We go from range -8388608 => 8388607
	const tint32 kiAdditor = 0x800000;
	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	tfloat32 kfDivisor = (((tfloat32)0xFFFFFF) / fRangeFull);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32 = *((tint32*)pcPCM24Mono);
		pcPCM24Mono += 3;

		// Convert range -8388608 => 8388607 to range -1.0 => 1.0
		Inplace24to32(i32);
		i32 += kiAdditor;
		tfloat32 f32 = (tfloat32)i32;
		f32 /= kfDivisor;
		f32 += fRangeMin;

		*((tfloat32*)pcOutput) = f32;
		pcOutput += 4;
	}
};

void Xlat::PCM24Mono_DualAF32Mono(
	tchar* pcPCM24Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Left, tint32* piAllocatedLeft,
	tchar** ppcAF32Right, tint32* piAllocatedRight,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 3;
	*piOutputBytes = iSamples * 4;
	tchar* pcOutputL = PrepareWhateverBuff(ppcAF32Left, piAllocatedLeft, *piOutputBytes, false);
	tchar* pcOutputR = PrepareWhateverBuff(ppcAF32Right, piAllocatedRight, *piOutputBytes, false);

	// We go from range -8388608 => 8388607
	const tint32 kiAdditor = 0x800000;
	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	tfloat32 kfDivisor = (((tfloat32)0xFFFFFF) / fRangeFull);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32 = *((tint32*)pcPCM24Mono);
		pcPCM24Mono += 3;

		// Convert range -8388608 => 8388607 to range -1.0 => 1.0
		Inplace24to32(i32);
		i32 += kiAdditor;
		tfloat32 f32 = (tfloat32)i32;
		f32 /= kfDivisor;
		f32 += fRangeMin;

		*((tfloat32*)pcOutputL) = f32;
		pcOutputL += 4;
		*((tfloat32*)pcOutputR) = f32;
		pcOutputR += 4;
	}
};


// ---- XLAT from dual 24 bit mono PCM ----

void Xlat::DualPCM24Mono_PCM24Mono(
	tchar* pcPCM24Left, tchar* pcPCM24Right, tint32 iInputBytes,
	tchar** ppcPCM24Mono, tint32* piAllocatedBytes)
{
	// Note! Not "in place" due to 24 bit /32 bit overflow - huh? wrong comment...
	tint32 iSamples = iInputBytes / 3;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM24Mono, piAllocatedBytes, iInputBytes, false);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = *((tint32*)pcPCM24Left);
		pcPCM24Left += 3;
		tint32 i32R = *((tint32*)pcPCM24Right);
		pcPCM24Right += 3;

		// Downmix 32 bit dual mono to 32 bit mono
		Inplace24to32(i32L);
		Inplace24to32(i32R);
		i32L += i32R;
		i32L >>= 1;
		Inplace32to24(i32L);

		*((tint32*)pcOutput) = i32L;
		pcOutput += 3;
	}
};

void Xlat::DualPCM24Mono_PCM24Stereo(
	tchar* pcPCM24Left, tchar* pcPCM24Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Interleaved, tint32* piAllocatedBytes)
{
	tint32 iSamples = iInputBytes / 3;
	*piOutputBytes = iSamples * 3 * 2;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM24Interleaved, piAllocatedBytes, *piOutputBytes, false);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = *((tint32*)pcPCM24Left);
		pcPCM24Left += 3;
		tint32 i32R = *((tint32*)pcPCM24Right);
		pcPCM24Right += 3;

		*((tint32*)pcOutput) = i32L;
		pcOutput += 3;
		*((tint32*)pcOutput) = i32R;
		pcOutput += 3;
	}
};

void Xlat::DualPCM24Mono_PCM16Mono_InPlace(
	tchar* pcPCM24Left, tchar* pcPCM24Right, tint32 iInputBytes, tint32* piOutputBytes)
{
	tint32 iSamples = iInputBytes / 3;
	*piOutputBytes = iSamples * 2;
	tchar* pcOutput = pcPCM24Left;

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = *((tint32*)pcPCM24Left);
		pcPCM24Left += 3;
		tint32 i32R = *((tint32*)pcPCM24Right);
		pcPCM24Right += 3;

		// Downmix 24 bits dual mono to 16 bit mono
		Inplace24to32(i32L);
		Inplace24to32(i32R);
		i32L += i32R;
		tint16 i16 = (tint16)(i32L >> 9);
		i16 = CPSwap(i16);

		*((tint16*)pcOutput) = i16;
		pcOutput += 2;
	}
};

void Xlat::DualPCM24Mono_PCM16Stereo(
	tchar* pcPCM24Left, tchar* pcPCM24Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM16Interleaved, tint32* piAllocatedBytes)
{
	tint32 iSamples = iInputBytes / 3;
	*piOutputBytes = iSamples * 2 * 2;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM16Interleaved, piAllocatedBytes, *piOutputBytes, false);

	// Skip lowest byte little-endian style since PCM is Intel native (24 bits => 16 bits)
	pcPCM24Left++;
	pcPCM24Right++;

	for ( ; iSamples > 0; iSamples--) {
		tint16 i16L = CPSwap(*((tint16*)pcPCM24Left));
		pcPCM24Left += 3;
		tint16 i16R = CPSwap(*((tint16*)pcPCM24Right));
		pcPCM24Right += 3;

		*((tint16*)pcOutput) = i16L;
		pcOutput += 2;
		*((tint16*)pcOutput) = i16R;
		pcOutput += 2;
	}
};

void Xlat::DualPCM24Mono_DualPCM16_InPlace(
	tchar* pcPCM24Left, tchar* pcPCM24Right, tint32 iInputBytes, tint32* piOutputBytes)
{
	tint32 iSamples = iInputBytes / 3;
	*piOutputBytes = iSamples * 2;
	tchar* pcOutputLeft = pcPCM24Left;
	tchar* pcOutputRight = pcPCM24Right;

	// Skip lowest byte little-endian style since PCM is Intel native (24 bits => 16 bits)
	pcPCM24Left++;
	pcPCM24Right++;

	for ( ; iSamples > 0; iSamples--) {
		tint16 i16L = *((tint16*)pcPCM24Left);
		pcPCM24Left += 3;
		tint16 i16R = *((tint16*)pcPCM24Right);
		pcPCM24Right += 3;

		*((tint16*)pcOutputLeft) = i16L;
		pcOutputLeft += 2;
		*((tint16*)pcOutputRight) = i16R;
		pcOutputRight += 2;
	}
};

void Xlat::DualPCM24Mono_AF32Mono(
	tchar* pcPCM24Left, tchar* pcPCM24Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32DownMix, tint32* piAllocatedBytes,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 3;
	*piOutputBytes = iSamples * 4;
	tchar* pcOutput = PrepareWhateverBuff(ppcAF32DownMix, piAllocatedBytes, *piOutputBytes, false);

	// We go from range (2 x -8388608) => (2 x 8388607)
	const tint32 kiAdditor = 2 * 0x800000;
	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	// x 2 because we're at double volume due to downmix
	tfloat32 kfDivisor = (((tfloat32)0xFFFFFF) * 2 / fRangeFull);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = *((tint32*)pcPCM24Left);
		pcPCM24Left += 2;
		tint32 i32R = *((tint32*)pcPCM24Right);
		pcPCM24Right += 2;

		// Downmix 24 bits dual mono (range -8388608 => 8388607) to 32 bit float mono (range -1.0 => 1.0)
		Inplace24to32(i32L);
		Inplace24to32(i32R);
		i32L += i32R;
		i32L += kiAdditor;
		tfloat32 f32 = (tfloat32)i32L;
		f32 /= kfDivisor;
		f32 += fRangeMin;

		*((tfloat32*)pcOutput) = f32;
		pcOutput += 4;
	}
};

void Xlat::DualPCM24Mono_DualAF32Mono(
	tchar* pcPCM24Left, tchar* pcPCM24Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Left, tint32* piAllocatedLeft, tchar** ppcAF32Right, tint32* piAllocatedRight,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 3;
	*piOutputBytes = iSamples * 4;
	tchar* pcOutputLeft = PrepareWhateverBuff(ppcAF32Left, piAllocatedLeft, *piOutputBytes, false);
	tchar* pcOutputRight = PrepareWhateverBuff(ppcAF32Right, piAllocatedRight, *piOutputBytes, false);

	// We go from range -8388608 => 8388607
	const tint32 kiAdditor = 0x800000;
	// " / 2 " because we go down to range 0.0 => 2.0
	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	tfloat32 kfDivisor = (((tfloat32)0xFFFFFF) / fRangeFull);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = *((tint32*)pcPCM24Left);
		pcPCM24Left += 3;
		tint32 i32R = *((tint32*)pcPCM24Right);
		pcPCM24Right += 3;

		// Convert range -8388608 => 8388607 to range -1.0 => 1.0
		Inplace24to32(i32L);
		i32L += kiAdditor;
		tfloat32 f32L = (tfloat32)i32L;
		f32L /= kfDivisor;
		f32L += fRangeMin;
		//
		Inplace24to32(i32R);
		i32R += kiAdditor;
		tfloat32 f32R = (tfloat32)i32R;
		f32R /= kfDivisor;
		f32R += fRangeMin;

		*((tfloat32*)pcOutputLeft) = f32L;
		pcOutputLeft += 4;
		*((tfloat32*)pcOutputRight) = f32R;
		pcOutputRight += 4;
	}
};


// ---- XLAT from 24 bit interleaved (stereo) PCM ----

void Xlat::PCM24Stereo_PCM24Mono_InPlace(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes)
{
	tint32 iSamples = iInputBytes / (3 * 2);
	*piOutputBytes = iSamples * 3;
	tchar* pcOutput = pcPCM24Interleaved;

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = *((tint32*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;
		tint32 i32R = *((tint32*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;

		// Downmix 24 bit dual mono to 32 bit mono
		Inplace24to32(i32L);
		Inplace24to32(i32R);
		i32L += i32R;
		i32L >>= 1;
		Inplace32to24(i32L);

		*((tint32*)pcOutput) = i32L;
		pcOutput += 3;
	}
};

void Xlat::PCM24Stereo_DualPCM24Mono_PartialInPlace(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Right, tint32* piAllocatedRight)
{
	tint32 iSamples = iInputBytes / (3 * 2);
	*piOutputBytes = iSamples * 3;
	tchar* pcOutputLeft = pcPCM24Interleaved;
	tchar* pcOutputRight = PrepareWhateverBuff(ppcPCM24Right, piAllocatedRight, *piOutputBytes, false);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = *((tint32*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;
		tint32 i32R = *((tint32*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;

		*((tint32*)pcOutputLeft) = i32L;
		pcOutputLeft += 3;
		*((tint32*)pcOutputRight) = i32R;
		pcOutputRight += 3;
	}
};

void Xlat::PCM24Stereo_PCM16Mono_InPlace(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes)
{
	tint32 iSamples = iInputBytes / (3 * 2);
	*piOutputBytes = iSamples * 2;
	tchar* pcOutput = pcPCM24Interleaved;

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = *((tint32*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;
		tint32 i32R = *((tint32*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;

		// Downmix 24 bits dual mono to 16 bit mono
		Inplace24to32(i32L);
		Inplace24to32(i32R);
		i32L += i32R;
		tint16 i16 = (tint16)(i32L >> 9);
		i16 = CPSwap(i16);

		*((tint16*)pcOutput) = i16;
		pcOutput += 2;
	}
};

void Xlat::PCM24Stereo_DualPCM16Mono_PartialInPlace(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM16Right, tint32* piAllocatedBytes)
{
	tint32 iSamples = iInputBytes / (3 * 2);
	*piOutputBytes = iSamples * 2;
	tchar* pcOutputLeft = pcPCM24Interleaved;
	tchar* pcOutputRight = PrepareWhateverBuff(ppcPCM16Right, piAllocatedBytes, *piOutputBytes, false);

	// Skip lowest byte little-endian style since PCM is Intel native (24 bits => 16 bits)
	pcPCM24Interleaved++;

	for ( ; iSamples > 0; iSamples--) {
		tint16 i16L = *((tint16*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;
		tint16 i16R = *((tint16*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;

		*((tint16*)pcOutputLeft) = i16L;
		pcOutputLeft += 2;
		*((tint16*)pcOutputRight) = i16R;
		pcOutputRight += 2;
	}
};

void Xlat::PCM24Stereo_PCM16Stereo_InPlace(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes)
{
	tint32 iSamples = iInputBytes / (3 * 2);
	*piOutputBytes = iSamples * 2 * 2;
	tchar* pcOutput = pcPCM24Interleaved;

	// Skip lowest byte little-endian style since PCM is Intel native (24 bits => 16 bits)
	pcPCM24Interleaved++;

	for ( ; iSamples > 0; iSamples--) {
		tint16 i16L = *((tint16*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;
		tint16 i16R = *((tint16*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;

		*((tint16*)pcOutput) = i16L;
		pcOutput += 2;
		*((tint16*)pcOutput) = i16R;
		pcOutput += 2;
	}
};

void Xlat::PCM24Stereo_AF32Mono_InPlace(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / (3 * 2);
	*piOutputBytes = iSamples * 4;
	tchar* pcOutput = pcPCM24Interleaved;

	// We go from range (2 x -8388608) => (2 x 8388607)
	const tint32 kiAdditor = 2 * 0x800000;
	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	// x 2 because we're at double volume due to downmix
	tfloat32 kfDivisor = (((tfloat32)0xFFFFFF) * 2 / fRangeFull);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = *((tint32*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;
		tint32 i32R = *((tint32*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;

		// Downmix 24 bits dual mono (range -8388608 => 8388607) to 32 bit float mono (range -1.0 => 1.0)
		Inplace24to32(i32L);
		Inplace24to32(i32R);
		i32L += i32R;
		i32L += kiAdditor;
		tfloat32 f32 = (tfloat32)i32L;
		f32 /= kfDivisor;
		f32 += fRangeMin;

		*((tfloat32*)pcOutput) = f32;
		pcOutput += 4;
	}
};

void Xlat::PCM24Stereo_AF32Mono(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Mono, tint32* piAllocatedBytes,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / (3 * 2);
	*piOutputBytes = iSamples * 4;
	tchar* pcOutput = PrepareWhateverBuff(ppcAF32Mono, piAllocatedBytes, *piOutputBytes, false);

	// We go from range (2 x -8388608) => (2 x 8388607)
	const tint32 kiAdditor = 2 * 0x800000;
	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	// x 2 because we're at double volume due to downmix
	tfloat32 kfDivisor = (((tfloat32)0xFFFFFF) * 2 / fRangeFull);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = *((tint32*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;
		tint32 i32R = *((tint32*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;

		// Downmix 24 bits dual mono (range -8388608 => 8388607) to 32 bit float mono (range -1.0 => 1.0)
		Inplace24to32(i32L);
		Inplace24to32(i32R);
		i32L += i32R;
		i32L += kiAdditor;
		tfloat32 f32 = (tfloat32)i32L;
		f32 /= kfDivisor;
		f32 += fRangeMin;

		*((tfloat32*)pcOutput) = f32;
		pcOutput += 4;
	}
};

void Xlat::PCM24Stereo_DualAF32Mono_PartialInPlace(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Right, tint32* piAllocatedBytes,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / (3 * 2);
	*piOutputBytes = iSamples * 4;
	tchar* pcOutputLeft = pcPCM24Interleaved;
	tchar* pcOutputRight = PrepareWhateverBuff(ppcAF32Right, piAllocatedBytes, *piOutputBytes, false);

	// We go from range -8388608 => 8388607
	const tint32 kiAdditor = 0x800000;
	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	tfloat32 kfDivisor = (((tfloat32)0xFFFFFF) / fRangeFull);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = *((tint32*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;
		tint32 i32R = *((tint32*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;

		// Convert range -8388608 => 8388607 to range -1.0 => 1.0
		Inplace24to32(i32L);
		i32L += kiAdditor;
		tfloat32 f32L = (tfloat32)i32L;
		f32L /= kfDivisor;
		f32L += fRangeMin;
		//
		Inplace24to32(i32R);
		i32R += kiAdditor;
		tfloat32 f32R = (tfloat32)i32R;
		f32R /= kfDivisor;
		f32R += fRangeMin;

		*((tfloat32*)pcOutputLeft) = f32L;
		pcOutputLeft += 4;
		*((tfloat32*)pcOutputRight) = f32R;
		pcOutputRight += 4;
	}
};

void Xlat::PCM24Stereo_DualAF32Mono(
	tchar* pcPCM24Interleaved, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcAF32Left, tint32* piAllocatedLeft, tchar** ppcAF32Right, tint32* piAllocatedRight,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / (3 * 2);
	*piOutputBytes = iSamples * 4;
	tchar* pcOutputLeft = PrepareWhateverBuff(ppcAF32Left, piAllocatedLeft, *piOutputBytes, false);
	tchar* pcOutputRight = PrepareWhateverBuff(ppcAF32Right, piAllocatedRight, *piOutputBytes, false);

	// We go from range -8388608 => 8388607
	const tint32 kiAdditor = 0x800000;
	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	tfloat32 kfDivisor = (((tfloat32)0xFFFFFF) / fRangeFull);

	for ( ; iSamples > 0; iSamples--) {
		tint32 i32L = *((tint32*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;
		tint32 i32R = *((tint32*)pcPCM24Interleaved);
		pcPCM24Interleaved += 3;

		// Convert range -8388608 => 8388607 to range -1.0 => 1.0
		Inplace24to32(i32L);
		i32L += kiAdditor;
		tfloat32 f32L = (tfloat32)i32L;
		f32L /= kfDivisor;
		f32L += fRangeMin;
		//
		Inplace24to32(i32R);
		i32R += kiAdditor;
		tfloat32 f32R = (tfloat32)i32R;
		f32R /= kfDivisor;
		f32R += fRangeMin;

		*((tfloat32*)pcOutputLeft) = f32L;
		pcOutputLeft += 4;
		*((tfloat32*)pcOutputRight) = f32R;
		pcOutputRight += 4;
	}
};


// ---- XLAT from 32 bit float mono array ----

void Xlat::AF32Mono_PCM16Mono_InPlace(
	tchar* pcAF32Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	*piOutputBytes = iSamples * 2;
	tchar* pcOutput = pcAF32Mono;

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFF) / fRangeFull);
	// We go to range -32768 => 32767
	const tint32 kiSubtractor = 0x8000;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32 = *((tfloat32*)pcAF32Mono);
		pcAF32Mono += 4;

		// Convert from range -1.0 => 1.0 to range -32768 => 32767
		f32 -= fRangeMin;
		f32 *= kfMultiplier;
		tint32 i32 = (tint32)f32;
		i32 -= kiSubtractor;

		// Verify range
		if (i32 > 32767) {
			i32 = 32767;
			(*piAccumOverflows)++;
		}
		else if (i32 < -32768) {
			i32 = -32768;
			(*piAccumOverflows)++;
		}

		tint16 i16 = CPSwap((tint16)i32);
		*((tint16*)pcOutput) = i16;
		pcOutput += 2;
	}
};

void Xlat::AF32Mono_PCM16Mono(
	tchar* pcAF32Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM16Mono, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	*piOutputBytes = iSamples * 2;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM16Mono, piAllocatedBytes, *piOutputBytes, false);

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFF) / fRangeFull);
	// We go to range -32768 => 32767
	const tint32 kiSubtractor = 0x8000;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32 = *((tfloat32*)pcAF32Mono);
		pcAF32Mono += 4;

		// Convert from range -1.0 => 1.0 to range -32768 => 32767
		f32 -= fRangeMin;
		f32 *= kfMultiplier;
		tint32 i32 = (tint32)f32;
		i32 -= kiSubtractor;

		// Verify range
		if (i32 > 32767) {
			i32 = 32767;
			(*piAccumOverflows)++;
		}
		else if (i32 < -32768) {
			i32 = -32768;
			(*piAccumOverflows)++;
		}

		tint16 i16 = CPSwap((tint16)i32);
		*((tint16*)pcOutput) = i16;
		pcOutput += 2;
	}
};

void Xlat::AF32Mono_PCM16Stereo_InPlace(
	tchar* pcAF32Mono, tint32 iInputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	tchar* pcOutput = pcAF32Mono;

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFF) / fRangeFull);
	// We go to range -32768 => 32767
	const tint32 kiSubtractor = 0x8000;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32 = *((tfloat32*)pcAF32Mono);
		pcAF32Mono += 4;

		// Convert from range -1.0 => 1.0 to range -32768 => 32767
		f32 -= fRangeMin;
		f32 *= kfMultiplier;
		tint32 i32 = (tint32)f32;
		i32 -= kiSubtractor;

		// Verify range
		if (i32 > 32767) {
			i32 = 32767;
			(*piAccumOverflows)++;
		}
		else if (i32 < -32768) {
			i32 = -32768;
			(*piAccumOverflows)++;
		}

		tint16 i16 = CPSwap((tint16)i32);
		*((tint16*)pcOutput) = i16;
		pcOutput += 2;
		*((tint16*)pcOutput) = i16;
		pcOutput += 2;
	}
};

void Xlat::AF32Mono_PCM16Stereo(
	tchar* pcAF32Mono, tint32 iInputBytes,
	tchar** ppcPCM16Interleaved, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM16Interleaved, piAllocatedBytes, iInputBytes, false);

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFF) / fRangeFull);
	// We go to range -32768 => 32767
	const tint32 kiSubtractor = 0x8000;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32 = *((tfloat32*)pcAF32Mono);
		pcAF32Mono += 4;

		// Convert from range -1.0 => 1.0 to range -32768 => 32767
		f32 -= fRangeMin;
		f32 *= kfMultiplier;
		tint32 i32 = (tint32)f32;
		i32 -= kiSubtractor;

		// Verify range
		if (i32 > 32767) {
			i32 = 32767;
			(*piAccumOverflows)++;
		}
		else if (i32 < -32768) {
			i32 = -32768;
			(*piAccumOverflows)++;
		}

		tint16 i16 = CPSwap((tint16)i32);
		*((tint16*)pcOutput) = i16;
		pcOutput += 2;
		*((tint16*)pcOutput) = i16;
		pcOutput += 2;
	}
};

void Xlat::AF32Mono_PCM24Mono_InPlace(
	tchar* pcAF32Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	*piOutputBytes = iSamples * 3;
	tchar* pcOutput = pcAF32Mono;

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFFFF) / fRangeFull);
	// We go to range -8388608 => 8388607
	const tint32 kiSubtractor = 0x800000;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32 = *((tfloat32*)pcAF32Mono);
		pcAF32Mono += 4;

		// Convert from range -1.0 => 1.0 to range -8388608 => 8388607
		f32 -= fRangeMin;
		f32 *= kfMultiplier;
		tint32 i32 = (tint32)f32;
		i32 -= kiSubtractor;

		// Verify range
		if (i32 > 8388607) {
			i32 = 8388607;
			(*piAccumOverflows)++;
		}
		else if (i32 < -8388608) {
			i32 = -8388608;
			(*piAccumOverflows)++;
		}

		Inplace32to24(i32);
		*((tint32*)pcOutput) = i32;
		pcOutput += 3;
	}
};

void Xlat::AF32Mono_PCM24Mono(
	tchar* pcAF32Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Mono, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	*piOutputBytes = iSamples * 3;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM24Mono, piAllocatedBytes, *piOutputBytes, false);

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFFFF) / fRangeFull);
	// We go to range -8388608 => 8388607
	const tint32 kiSubtractor = 0x800000;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32 = *((tfloat32*)pcAF32Mono);
		pcAF32Mono += 4;

		// Convert from range -1.0 => 1.0 to range -8388608 => 8388607
		f32 -= fRangeMin;
		f32 *= kfMultiplier;
		tint32 i32 = (tint32)f32;
		i32 -= kiSubtractor;

		// Verify range
		if (i32 > 8388607) {
			i32 = 8388607;
			(*piAccumOverflows)++;
		}
		else if (i32 < -8388608) {
			i32 = -8388608;
			(*piAccumOverflows)++;
		}

		Inplace32to24(i32);
		*((tint32*)pcOutput) = i32;
		pcOutput += 3;
	}
};

void Xlat::AF32Mono_PCM24Stereo(
	tchar* pcAF32Mono, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Interleaved, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	*piOutputBytes = iSamples * 3 * 2;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM24Interleaved, piAllocatedBytes, *piOutputBytes, false);

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFFFF) / fRangeFull);
	// We go to range -8388608 => 8388607
	const tint32 kiSubtractor = 0x800000;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32 = *((tfloat32*)pcAF32Mono);
		pcAF32Mono += 4;

		// Convert from range -1.0 => 1.0 to range -8388608 => 8388607
		f32 -= fRangeMin;
		f32 *= kfMultiplier;
		tint32 i32 = (tint32)f32;
		i32 -= kiSubtractor;

		// Verify range
		if (i32 > 8388607) {
			i32 = 8388607;
			(*piAccumOverflows)++;
		}
		else if (i32 < -8388608) {
			i32 = -8388608;
			(*piAccumOverflows)++;
		}

		Inplace32to24(i32);
		*((tint32*)pcOutput) = i32;
		pcOutput += 3;
		*((tint32*)pcOutput) = i32;
		pcOutput += 3;
	}
};

void Xlat::AF32Mono_AF32Mono_InPlace(
	tchar* pcAF32Mono, tint32 iInputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeInMin /*= -1.0f*/, tfloat32 fRangeInMax /*= 1.0f*/,
	tfloat32 fRangeOutMin /*= -1.0f*/, tfloat32 fRangeOutMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	tchar* pcOutput = pcAF32Mono;

	tfloat32 fRangeInFull = fRangeInMax - fRangeInMin;
	tfloat32 fRangeInMiddle = (fRangeInMax + fRangeInMin) / 2;
	tfloat32 fRangeOutFull = fRangeOutMax - fRangeOutMin;
	tfloat32 fRangeOutMiddle = (fRangeOutMax + fRangeOutMin) / 2;
	const tfloat32 kfMultiplier = (fRangeOutFull / fRangeInFull);
	const tfloat32 kfSubtractor = (fRangeInMiddle * kfMultiplier) - fRangeOutMiddle;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32 = *((tfloat32*)pcAF32Mono);
		pcAF32Mono += 4;

		// Convert from range -1.0 => 1.0 to range whatever
		// This would be the straight forward way of convertion:
		//f32 -= fRangeInMin;
		//f32 *= kfMultiplier;
		//f32 += fRangeOutMin;
		// ... but this saves us an operation:
		f32 *= kfMultiplier;
		f32 -= kfSubtractor;

		// Verify range
		if (f32 > fRangeOutMax) {
			f32 = fRangeOutMax;
			(*piAccumOverflows)++;
		}
		else if (f32 < fRangeOutMin) {
			f32 = fRangeOutMin;
			(*piAccumOverflows)++;
		}

		*((tfloat32*)pcOutput) = f32;
		pcOutput += 4;
	}
};

void Xlat::AF32Mono_AF32Mono(
	tchar* pcAF32Mono, tint32 iInputBytes,
	tchar** ppcAF32MonoOut, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeInMin /*= -1.0f*/, tfloat32 fRangeInMax /*= 1.0f*/,
	tfloat32 fRangeOutMin /*= -1.0f*/, tfloat32 fRangeOutMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	tchar* pcOutput = PrepareWhateverBuff(ppcAF32MonoOut, piAllocatedBytes, iInputBytes, false);

	tfloat32 fRangeInFull = fRangeInMax - fRangeInMin;
	tfloat32 fRangeInMiddle = (fRangeInMax + fRangeInMin) / 2;
	tfloat32 fRangeOutFull = fRangeOutMax - fRangeOutMin;
	tfloat32 fRangeOutMiddle = (fRangeOutMax + fRangeOutMin) / 2;
	const tfloat32 kfMultiplier = (fRangeOutFull / fRangeInFull);
	const tfloat32 kfSubtractor = (fRangeInMiddle * kfMultiplier) - fRangeOutMiddle;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32 = *((tfloat32*)pcAF32Mono);
		pcAF32Mono += 4;

		// Convert from range -1.0 => 1.0 to range whatever
		// This would be the straight forward way of convertion:
		//f32 -= fRangeInMin;
		//f32 *= kfMultiplier;
		//f32 += fRangeOutMin;
		// ... but this saves us an operation:
		f32 *= kfMultiplier;
		f32 -= kfSubtractor;

		// Verify range
		if (f32 > fRangeOutMax) {
			f32 = fRangeOutMax;
			(*piAccumOverflows)++;
		}
		else if (f32 < fRangeOutMin) {
			f32 = fRangeOutMin;
			(*piAccumOverflows)++;
		}

		*((tfloat32*)pcOutput) = f32;
		pcOutput += 4;
	}
};



// ---- XLAT from dual 32 bit float mono array ----

void Xlat::DualAF32Mono_AF32Mono_InPlace(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	tchar* pcOutput = pcAF32Left;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32L = *((tfloat32*)pcAF32Left);
		pcAF32Left += 4;
		tfloat32 f32R = *((tfloat32*)pcAF32Right);
		pcAF32Right += 4;

		f32L += f32R;
		f32L /= 2;

		// Save while verifying range
		if (f32L > fRangeMax) {
			*((tfloat32*)pcOutput) = fRangeMax;
			(*piAccumOverflows)++;
		}
		else if (f32L < fRangeMin) {
			*((tfloat32*)pcOutput) = fRangeMin;
			(*piAccumOverflows)++;
		}
		else
			*((tfloat32*)pcOutput) = f32L;

		pcOutput += 4;
	}
};

void Xlat::DualAF32Mono_AF32Mono(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes,
	tchar** ppcAF32Mono, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	tchar* pcOutput = PrepareWhateverBuff(ppcAF32Mono, piAllocatedBytes, iInputBytes, false);

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32L = *((tfloat32*)pcAF32Left);
		pcAF32Left += 4;
		tfloat32 f32R = *((tfloat32*)pcAF32Right);
		pcAF32Right += 4;

		f32L += f32R;
		f32L /= 2;

		// Save while verifying range
		if (f32L > fRangeMax) {
			*((tfloat32*)pcOutput) = fRangeMax;
			(*piAccumOverflows)++;
		}
		else if (f32L < fRangeMin) {
			*((tfloat32*)pcOutput) = fRangeMin;
			(*piAccumOverflows)++;
		}
		else
			*((tfloat32*)pcOutput) = f32L;

		pcOutput += 4;
	}
};

void Xlat::DualAF32Mono_DualAF32Mono_InPlace(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeInMin /*= -1.0f*/, tfloat32 fRangeInMax /*= 1.0f*/,
	tfloat32 fRangeOutMin /*= -1.0f*/, tfloat32 fRangeOutMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	tchar* pcOutputL = pcAF32Left;
	tchar* pcOutputR = pcAF32Right;

	tfloat32 fRangeInFull = fRangeInMax - fRangeInMin;
	tfloat32 fRangeInMiddle = (fRangeInMax + fRangeInMin) / 2;
	tfloat32 fRangeOutFull = fRangeOutMax - fRangeOutMin;
	tfloat32 fRangeOutMiddle = (fRangeOutMax + fRangeOutMin) / 2;
	const tfloat32 kfMultiplier = (fRangeOutFull / fRangeInFull);
	const tfloat32 kfSubtractor = (fRangeInMiddle * kfMultiplier) - fRangeOutMiddle;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32L = *((tfloat32*)pcAF32Left);
		pcAF32Left += 4;
		tfloat32 f32R = *((tfloat32*)pcAF32Right);
		pcAF32Right += 4;

		// Convert from range -1.0 => 1.0 to range whatever
		// This would be the straight forward way of convertion:
		//f32L -= fRangeInMin;
		//f32L *= kfMultiplier;
		//f32L += fRangeOutMin;
		//f32R -= fRangeInMin;
		//f32R *= kfMultiplier;
		//f32R += fRangeOutMin;
		// ... but this saves us two operations:
		f32L *= kfMultiplier;
		f32L -= kfSubtractor;
		f32R *= kfMultiplier;
		f32R -= kfSubtractor;

		// Verify range
		if (f32L > fRangeOutMax) {
			f32L = fRangeOutMax;
			(*piAccumOverflows)++;
		}
		else if (f32L < fRangeOutMin) {
			f32L = fRangeOutMin;
			(*piAccumOverflows)++;
		}
		if (f32R > fRangeOutMax) {
			f32R = fRangeOutMax;
			(*piAccumOverflows)++;
		}
		else if (f32R < fRangeOutMin) {
			f32R = fRangeOutMin;
			(*piAccumOverflows)++;
		}

		*((tfloat32*)pcOutputL) = f32L;
		pcOutputL += 4;
		*((tfloat32*)pcOutputR) = f32R;
		pcOutputR += 4;
	}
};

void Xlat::DualAF32Mono_DualAF32Mono(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes,
	tchar** ppcAF32Left, tint32* piAllocatedLeft, tchar** ppcAF32Right, tint32* piAllocatedRight,
	tint64* piAccumOverflows,
	tfloat32 fRangeInMin /*= -1.0f*/, tfloat32 fRangeInMax /*= 1.0f*/,
	tfloat32 fRangeOutMin /*= -1.0f*/, tfloat32 fRangeOutMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	tchar* pcOutputL = PrepareWhateverBuff(ppcAF32Left, piAllocatedLeft, iInputBytes, false);
	tchar* pcOutputR = PrepareWhateverBuff(ppcAF32Right, piAllocatedRight, iInputBytes, false);

	tfloat32 fRangeInFull = fRangeInMax - fRangeInMin;
	tfloat32 fRangeInMiddle = (fRangeInMax + fRangeInMin) / 2;
	tfloat32 fRangeOutFull = fRangeOutMax - fRangeOutMin;
	tfloat32 fRangeOutMiddle = (fRangeOutMax + fRangeOutMin) / 2;
	const tfloat32 kfMultiplier = (fRangeOutFull / fRangeInFull);
	const tfloat32 kfSubtractor = (fRangeInMiddle * kfMultiplier) - fRangeOutMiddle;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32L = *((tfloat32*)pcAF32Left);
		pcAF32Left += 4;
		tfloat32 f32R = *((tfloat32*)pcAF32Right);
		pcAF32Right += 4;

		// Convert from range -1.0 => 1.0 to range whatever
		// This would be the straight forward way of convertion:
		//f32L -= fRangeInMin;
		//f32L *= kfMultiplier;
		//f32L += fRangeOutMin;
		//f32R -= fRangeInMin;
		//f32R *= kfMultiplier;
		//f32R += fRangeOutMin;
		// ... but this saves us two operations:
		f32L *= kfMultiplier;
		f32L -= kfSubtractor;
		f32R *= kfMultiplier;
		f32R -= kfSubtractor;

		// Verify range
		if (f32L > fRangeOutMax) {
			f32L = fRangeOutMax;
			(*piAccumOverflows)++;
		}
		else if (f32L < fRangeOutMin) {
			f32L = fRangeOutMin;
			(*piAccumOverflows)++;
		}
		if (f32R > fRangeOutMax) {
			f32R = fRangeOutMax;
			(*piAccumOverflows)++;
		}
		else if (f32R < fRangeOutMin) {
			f32R = fRangeOutMin;
			(*piAccumOverflows)++;
		}

		*((tfloat32*)pcOutputL) = f32L;
		pcOutputL += 4;
		*((tfloat32*)pcOutputR) = f32R;
		pcOutputR += 4;
	}
};

void Xlat::DualAF32Mono_PCM16Mono_InPlace(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	*piOutputBytes = iSamples * 2;
	tchar* pcOutput = pcAF32Left;

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	// First " / 2 " because we go from range 0.0 => 2.0
	// Second " / 2 " because we're double volume due to downmix
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFF) / fRangeFull) / 2;
	// We go to range -32768 => 32767
	const tint32 kiSubtractor = 0x8000;
	const tfloat32 fRangeMinX2 = fRangeMin * 2;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32 = *((tfloat32*)pcAF32Left);
		pcAF32Left += 4;
		f32 += *((tfloat32*)pcAF32Right);
		pcAF32Right += 4;

		// Convert from range -2.0 => 2.0 to range -32768 => 32767
		f32 -= fRangeMinX2;
		f32 *= kfMultiplier;
		tint32 i32 = (tint32)f32;
		i32 -= kiSubtractor;

		// Verify range
		if (i32 > 32767) {
			i32 = 32767;
			(*piAccumOverflows)++;
		}
		else if (i32 < -32768) {
			i32 = -32768;
			(*piAccumOverflows)++;
		}

		tint16 i16 = CPSwap((tint16)i32);
		*((tint16*)pcOutput) = i16;
		pcOutput += 2;
	}
};

void Xlat::DualAF32Mono_PCM16Mono(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM16Mono, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	*piOutputBytes = iSamples * 2;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM16Mono, piAllocatedBytes, *piOutputBytes, false);

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	// First " / 2 " because we go from range 0.0 => 2.0
	// Second " / 2 " because we're double volume due to downmix
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFF) / fRangeFull) / 2;
	// We go to range -32768 => 32767
	const tint32 kiSubtractor = 0x8000;
	const tfloat32 fRangeMinX2 = fRangeMin * 2;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32 = *((tfloat32*)pcAF32Left);
		pcAF32Left += 4;
		f32 += *((tfloat32*)pcAF32Right);
		pcAF32Right += 4;

		// Convert from range -2.0 => 2.0 to range -32768 => 32767
		f32 -= fRangeMinX2;
		f32 *= kfMultiplier;
		tint32 i32 = (tint32)f32;
		i32 -= kiSubtractor;

		// Verify range
		if (i32 > 32767) {
			i32 = 32767;
			(*piAccumOverflows)++;
		}
		else if (i32 < -32768) {
			i32 = -32768;
			(*piAccumOverflows)++;
		}

		tint16 i16 = CPSwap((tint16)i32);
		*((tint16*)pcOutput) = i16;
		pcOutput += 2;
	}
};

void Xlat::DualAF32Mono_PCM16Stereo_InPlace(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	tchar* pcOutput = pcAF32Left;

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFF) / fRangeFull);
	// We go to range -32768 => 32767
	const tint32 kiSubtractor = 0x8000;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32L = *((tfloat32*)pcAF32Left);
		pcAF32Left += 4;
		tfloat32 f32R = *((tfloat32*)pcAF32Right);
		pcAF32Right += 4;

		// Convert from range -1.0 => 1.0 to range -32768 => 32767
		f32L -= fRangeMin;
		f32L *= kfMultiplier;
		tint32 i32L = (tint32)f32L;
		i32L -= kiSubtractor;
		//
		f32R -= fRangeMin;
		f32R *= kfMultiplier;
		tint32 i32R = (tint32)f32R;
		i32R -= kiSubtractor;

		// Verify range
		if (i32L > 32767) {
			i32L = 32767;
			(*piAccumOverflows)++;
		}
		else if (i32L < -32768) {
			i32L = -32768;
			(*piAccumOverflows)++;
		}
		//
		if (i32R > 32767) {
			i32R = 32767;
			(*piAccumOverflows)++;
		}
		else if (i32R < -32768) {
			i32R = -32768;
			(*piAccumOverflows)++;
		}

		tint16 i16L = CPSwap((tint16)i32L);
		tint16 i16R = CPSwap((tint16)i32R);
		*((tint16*)pcOutput) = i16L;
		pcOutput += 2;
		*((tint16*)pcOutput) = i16R;
		pcOutput += 2;
	}
};

void Xlat::DualAF32Mono_PCM16Stereo(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes,
	tchar** ppcPCM16Stereo, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM16Stereo, piAllocatedBytes, iInputBytes, false);

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFF) / fRangeFull);
	// We go to range -32768 => 32767
	const tint32 kiSubtractor = 0x8000;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32L = *((tfloat32*)pcAF32Left);
		pcAF32Left += 4;
		tfloat32 f32R = *((tfloat32*)pcAF32Right);
		pcAF32Right += 4;

		// Convert from range -1.0 => 1.0 to range -32768 => 32767
		f32L -= fRangeMin;
		f32L *= kfMultiplier;
		tint32 i32L = (tint32)f32L;
		i32L -= kiSubtractor;
		//
		f32R -= fRangeMin;
		f32R *= kfMultiplier;
		tint32 i32R = (tint32)f32R;
		i32R -= kiSubtractor;

		// Verify range
		if (i32L > 32767) {
			i32L = 32767;
			(*piAccumOverflows)++;
		}
		else if (i32L < -32768) {
			i32L = -32768;
			(*piAccumOverflows)++;
		}
		//
		if (i32R > 32767) {
			i32R = 32767;
			(*piAccumOverflows)++;
		}
		else if (i32R < -32768) {
			i32R = -32768;
			(*piAccumOverflows)++;
		}

		tint16 i16L = CPSwap((tint16)i32L);
		tint16 i16R = CPSwap((tint16)i32R);
		*((tint16*)pcOutput) = i16L;
		pcOutput += 2;
		*((tint16*)pcOutput) = i16R;
		pcOutput += 2;
	}
};

void Xlat::DualAF32Mono_DualPCM16Mono_InPlace(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	*piOutputBytes = iSamples * 2;
	tchar* pcOutputL = pcAF32Left;
	tchar* pcOutputR = pcAF32Right;

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFF) / fRangeFull);
	// We go to range -32768 => 32767
	const tint32 kiSubtractor = 0x8000;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32L = *((tfloat32*)pcAF32Left);
		pcAF32Left += 4;
		tfloat32 f32R = *((tfloat32*)pcAF32Right);
		pcAF32Right += 4;

		// Convert from range -1.0 => 1.0 to range -32768 => 32767
		f32L -= fRangeMin;
		f32L *= kfMultiplier;
		tint32 i32L = (tint32)f32L;
		i32L -= kiSubtractor;
		//
		f32R -= fRangeMin;
		f32R *= kfMultiplier;
		tint32 i32R = (tint32)f32R;
		i32R -= kiSubtractor;

		// Verify range
		if (i32L > 32767) {
			i32L = 32767;
			(*piAccumOverflows)++;
		}
		else if (i32L < -32768) {
			i32L = -32768;
			(*piAccumOverflows)++;
		}
		//
		if (i32R > 32767) {
			i32R = 32767;
			(*piAccumOverflows)++;
		}
		else if (i32R < -32768) {
			i32R = -32768;
			(*piAccumOverflows)++;
		}

		tint16 i16L = CPSwap((tint16)i32L);
		tint16 i16R = CPSwap((tint16)i32R);
		*((tint16*)pcOutputL) = i16L;
		pcOutputL += 2;
		*((tint16*)pcOutputR) = i16R;
		pcOutputR += 2;
	}
};

void Xlat::DualAF32Mono_DualPCM16Mono(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM16Left, tint32* piAllocatedLeft, tchar** ppcPCM16Right, tint32* piAllocatedRight,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	*piOutputBytes = iSamples * 2;
	tchar* pcOutputL = PrepareWhateverBuff(ppcPCM16Left, piAllocatedLeft, *piOutputBytes, false);
	tchar* pcOutputR = PrepareWhateverBuff(ppcPCM16Right, piAllocatedRight, *piOutputBytes, false);

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFF) / fRangeFull);
	// We go to range -32768 => 32767
	const tint32 kiSubtractor = 0x8000;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32L = *((tfloat32*)pcAF32Left);
		pcAF32Left += 4;
		tfloat32 f32R = *((tfloat32*)pcAF32Right);
		pcAF32Right += 4;

		// Convert from range -1.0 => 1.0 to range -32768 => 32767
		f32L -= fRangeMin;
		f32L *= kfMultiplier;
		tint32 i32L = (tint32)f32L;
		i32L -= kiSubtractor;
		//
		f32R -= fRangeMin;
		f32R *= kfMultiplier;
		tint32 i32R = (tint32)f32R;
		i32R -= kiSubtractor;

		// Verify range
		if (i32L > 32767) {
			i32L = 32767;
			(*piAccumOverflows)++;
		}
		else if (i32L < -32768) {
			i32L = -32768;
			(*piAccumOverflows)++;
		}
		//
		if (i32R > 32767) {
			i32R = 32767;
			(*piAccumOverflows)++;
		}
		else if (i32R < -32768) {
			i32R = -32768;
			(*piAccumOverflows)++;
		}

		tint16 i16L = CPSwap((tint16)i32L);
		tint16 i16R = CPSwap((tint16)i32R);
		*((tint16*)pcOutputL) = i16L;
		pcOutputL += 2;
		*((tint16*)pcOutputR) = i16R;
		pcOutputR += 2;
	}
};

void Xlat::DualAF32Mono_PCM24Mono_InPlace(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	*piOutputBytes = iSamples * 3;
	tchar* pcOutput = pcAF32Left;

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	// First " / 2 " because we go from range 0.0 => 2.0
	// Second " / 2 " because we're double volume due to downmix
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFFFF) / fRangeFull) / 2;
	// We go to range -8388608 => 8388607
	const tint32 kiSubtractor = 0x800000;
	const tfloat32 fRangeMinX2 = fRangeMin * 2;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32 = *((tfloat32*)pcAF32Left);
		pcAF32Left += 4;
		f32 += *((tfloat32*)pcAF32Right);
		pcAF32Right += 4;

		// Convert from range -2.0 => 2.0 to range -32768 => 32767
		f32 -= fRangeMinX2;
		f32 *= kfMultiplier;
		tint32 i32 = (tint32)f32;
		i32 -= kiSubtractor;

		// Verify range
		if (i32 > 8388607) {
			i32 = 8388607;
			(*piAccumOverflows)++;
		}
		else if (i32 < -8388608) {
			i32 = -8388608;
			(*piAccumOverflows)++;
		}

		Inplace32to24(i32);
		*((tint32*)pcOutput) = i32;
		pcOutput += 3;
	}
};

void Xlat::DualAF32Mono_PCM24Mono(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Mono, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	*piOutputBytes = iSamples * 3;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM24Mono, piAllocatedBytes, *piOutputBytes, false);

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	// First " / 2 " because we go from range 0.0 => 2.0
	// Second " / 2 " because we're double volume due to downmix
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFFFF) / fRangeFull) / 2;
	// We go to range -8388608 => 8388607
	const tint32 kiSubtractor = 0x800000;
	const tfloat32 fRangeMinX2 = fRangeMin * 2;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32 = *((tfloat32*)pcAF32Left);
		pcAF32Left += 4;
		f32 += *((tfloat32*)pcAF32Right);
		pcAF32Right += 4;

		// Convert from range -2.0 => 2.0 to range -32768 => 32767
		f32 -= fRangeMinX2;
		f32 *= kfMultiplier;
		tint32 i32 = (tint32)f32;
		i32 -= kiSubtractor;

		// Verify range
		if (i32 > 8388607) {
			i32 = 8388607;
			(*piAccumOverflows)++;
		}
		else if (i32 < -8388608) {
			i32 = -8388608;
			(*piAccumOverflows)++;
		}

		Inplace32to24(i32);
		*((tint32*)pcOutput) = i32;
		pcOutput += 3;
	}
};

void Xlat::DualAF32Mono_PCM24Stereo(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Interleaved, tint32* piAllocatedBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	*piOutputBytes = iSamples * 3 * 2;
	tchar* pcOutput = PrepareWhateverBuff(ppcPCM24Interleaved, piAllocatedBytes, *piOutputBytes, false);

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFFFF) / fRangeFull);
	// We go to range -8388608 => 8388607
	const tint32 kiSubtractor = 0x800000;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32L = *((tfloat32*)pcAF32Left);
		pcAF32Left += 4;
		tfloat32 f32R = *((tfloat32*)pcAF32Right);
		pcAF32Right += 4;

		// Convert from range -1.0 => 1.0 to range -8388608 => 8388607
		f32L -= fRangeMin;
		f32L *= kfMultiplier;
		tint32 i32L = (tint32)f32L;
		i32L -= kiSubtractor;
		//
		f32R -= fRangeMin;
		f32R *= kfMultiplier;
		tint32 i32R = (tint32)f32R;
		i32R -= kiSubtractor;

		// Verify range
		if (i32L > 8388607) {
			i32L = 8388607;
			(*piAccumOverflows)++;
		}
		else if (i32L < -8388608) {
			i32L = -8388608;
			(*piAccumOverflows)++;
		}
		//
		if (i32R > 8388607) {
			i32R = 8388607;
			(*piAccumOverflows)++;
		}
		else if (i32R < -8388608) {
			i32R = -8388608;
			(*piAccumOverflows)++;
		}

		Inplace32to24(i32L);
		Inplace32to24(i32R);
		*((tint32*)pcOutput) = i32L;
		pcOutput += 3;
		*((tint32*)pcOutput) = i32R;
		pcOutput += 3;
	}
};

void Xlat::DualAF32Mono_DualPCM24Mono_InPlace(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	*piOutputBytes = iSamples * 3;
	tchar* pcOutputL = pcAF32Left;
	tchar* pcOutputR = pcAF32Right;

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFFFF) / fRangeFull);
	// We go to range -8388608 => 8388607
	const tint32 kiSubtractor = 0x800000;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32L = *((tfloat32*)pcAF32Left);
		pcAF32Left += 4;
		tfloat32 f32R = *((tfloat32*)pcAF32Right);
		pcAF32Right += 4;

		// Convert from range -1.0 => 1.0 to range -8388608 => 8388607
		f32L -= fRangeMin;
		f32L *= kfMultiplier;
		tint32 i32L = (tint32)f32L;
		i32L -= kiSubtractor;
		//
		f32R -= fRangeMin;
		f32R *= kfMultiplier;
		tint32 i32R = (tint32)f32R;
		i32R -= kiSubtractor;

		// Verify range
		if (i32L > 8388607) {
			i32L = 8388607;
			(*piAccumOverflows)++;
		}
		else if (i32L < -8388608) {
			i32L = -8388608;
			(*piAccumOverflows)++;
		}
		//
		if (i32R > 8388607) {
			i32R = 8388607;
			(*piAccumOverflows)++;
		}
		else if (i32R < -8388608) {
			i32R = -8388608;
			(*piAccumOverflows)++;
		}

		Inplace32to24(i32L);
		Inplace32to24(i32R);
		*((tint32*)pcOutputL) = i32L;
		pcOutputL += 3;
		*((tint32*)pcOutputR) = i32R;
		pcOutputR += 3;
	}
};

void Xlat::DualAF32Mono_DualPCM24Mono(
	tchar* pcAF32Left, tchar* pcAF32Right, tint32 iInputBytes, tint32* piOutputBytes,
	tchar** ppcPCM24Left, tint32* piAllocatedLeft, tchar** ppcPCM24Right, tint32* piAllocatedRight,
	tint64* piAccumOverflows,
	tfloat32 fRangeMin /*= -1.0f*/, tfloat32 fRangeMax /*= 1.0f*/)
{
	tint32 iSamples = iInputBytes / 4;
	*piOutputBytes = iSamples * 3;
	tchar* pcOutputL = PrepareWhateverBuff(ppcPCM24Left, piAllocatedLeft, *piOutputBytes, false);
	tchar* pcOutputR = PrepareWhateverBuff(ppcPCM24Right, piAllocatedRight, *piOutputBytes, false);

	tfloat32 fRangeFull = fRangeMax - fRangeMin;
	const tfloat32 kfMultiplier = (((tfloat32)0xFFFFFF) / fRangeFull);
	// We go to range -8388608 => 8388607
	const tint32 kiSubtractor = 0x800000;

	for ( ; iSamples > 0; iSamples--) {
		tfloat32 f32L = *((tfloat32*)pcAF32Left);
		pcAF32Left += 4;
		tfloat32 f32R = *((tfloat32*)pcAF32Right);
		pcAF32Right += 4;

		// Convert from range -1.0 => 1.0 to range -8388608 => 8388607
		f32L -= fRangeMin;
		f32L *= kfMultiplier;
		tint32 i32L = (tint32)f32L;
		i32L -= kiSubtractor;
		//
		f32R -= fRangeMin;
		f32R *= kfMultiplier;
		tint32 i32R = (tint32)f32R;
		i32R -= kiSubtractor;

		// Verify range
		if (i32L > 8388607) {
			i32L = 8388607;
			(*piAccumOverflows)++;
		}
		else if (i32L < -8388608) {
			i32L = -8388608;
			(*piAccumOverflows)++;
		}
		//
		if (i32R > 8388607) {
			i32R = 8388607;
			(*piAccumOverflows)++;
		}
		else if (i32R < -8388608) {
			i32R = -8388608;
			(*piAccumOverflows)++;
		}

		Inplace32to24(i32L);
		Inplace32to24(i32R);
		*((tint32*)pcOutputL) = i32L;
		pcOutputL += 3;
		*((tint32*)pcOutputR) = i32R;
		pcOutputR += 3;
	}
};
