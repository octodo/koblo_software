

#include "SToolsInternalOS.h"

// Note: Currently wrong phase, since phase is accumulated before first calc

#include "BLITTable.h"

IOscillatorBLIT* IOscillatorBLIT::Create()
{
	return dynamic_cast<IOscillatorBLIT*>(new COscillatorBLIT());
}

void COscillatorBLIT::Destroy()
{
	delete dynamic_cast<COscillatorBLIT*>(this);
}

COscillatorBLIT::COscillatorBLIT()
{
	mlBufferSize = BLIT_TABLE_SIZE / BLIT_OVERSAMPLING;
	mpdBuffer = new double[mlBufferSize];
}

COscillatorBLIT::~COscillatorBLIT()
{
	delete[] mpdBuffer;
}

void COscillatorBLIT::SetSampleRate(float fSampleRate)
{
	mfSampleRate = fSampleRate;
	mfOneDivSampleRate = 1 / mfSampleRate;
}

void COscillatorBLIT::Reset()
{
	mlBufferIndex = 0;
	memset(mpdBuffer, 0, mlBufferSize * sizeof(double));

	mfPhase			= 0;
	mdVolume		= 0;

	mfPhaseMaster	= 0;

	mdSHLeak		= 0;
}

//#define LERP(A,B,F) ((B - A) * F + A)

void COscillatorBLIT::AddBLEP(double dOffset, double dVol)
{
/*	// Because of PWM there's a chance that dOffset >= 1
	while (dOffset >= 1) {
		dOffset -= 1;
	}

	long lBLEPsToAdd = mlBufferSize;

	double dIndex = BLIT_OVERSAMPLING * dOffset;
	long lIndex = (long)dIndex;
	double dFrac = dIndex - lIndex;
	long lRound = (dFrac >= 0.5 ? 1 : 0);

	double* pdIn = gpdBLITTable + lIndex;

	long lOutIndex = mlBufferIndex;

	for (long l = 0; l < lBLEPsToAdd; l++, pdIn += BLIT_OVERSAMPLING) {
//		mpdBuffer[lOutIndex] += dVol * (1 - LERP(pdIn[0], pdIn[1], dFrac));
		mpdBuffer[lOutIndex] += dVol * (1 - pdIn[lRound]);
		if (++lOutIndex == mlBufferSize) {
			lOutIndex = 0;
		}
	}*/

	// Because of PWM there's a chance that dOffset >= 1
	while (dOffset >= 1) {
		dOffset -= 1;
	}

	long lBLEPsToAdd = mlBufferSize;

	const double *pdIn = gpdBLITTable + (int)(BLIT_OVERSAMPLING * dOffset);
	// Rounding
//	if (dOffset - (long)dOffset >= 0.5f) {
//		pdIn++;
//	}

	long lOutIndex = mlBufferIndex;

	for (long l = 0; l < lBLEPsToAdd; l++) {
		mpdBuffer[lOutIndex] += dVol * (1 - pdIn[BLIT_OVERSAMPLING * l]);
		if (++lOutIndex == mlBufferSize) {
			lOutIndex = 0;
		}
	}
}

void COscillatorBLIT::ProcessSquare(tfloat32* pfOut, const tfloat32* pfPitch, tfloat fPWM, tint lC)
{
	for (long l = 0; l < lC; l++) {
		float fFreq = pfPitch[l] * mfOneDivSampleRate;

		mfPhase += fFreq;

		if (mfPhase >= 1) {
			// Add BLEP at end of waveform
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		// Avoid narrow pulses on high notes
		float fPW	= fPWM + fFreq;

		if (!mdVolume && mfPhase > fPW) {
			// Add BLEP in middle of waveform
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}

		float fOut = (float)mdVolume;

		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		// Convert to bipolar and remove DC
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;

		pfOut[l] = (tfloat32)fOut;
	}
}

void COscillatorBLIT::ProcessSquareFM(tfloat32* pfOut, const tfloat32* pfPitch, tfloat fPWM, const tfloat32* pfFM, const tfloat32* pfFMAmt, tint lC)
{

	float fFMVol	=	(172.0f - pfFMAmt[0]) * (1.0f/172.0f);

	for (long l = 0; l < lC; l++) {
		float fFreq = pfPitch[l] * mfOneDivSampleRate;

		//  FM Amount
		float fFM		=	1.0f + (pfFMAmt[l] * pfFM[l]);


		double dFreq = pfPitch[l] * mfOneDivSampleRate;

		mfPhase += fFreq * fFM;

		// Avoid narrow pulses on high notes
		float fPW	= fPWM + fFreq;

		if (mfPhase >= 1) {
			// Add BLEP at end of waveform
			while(mfPhase >= 1)
				mfPhase -= 1;

			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		else if (!mdVolume && mfPhase > fPW) {
			// Add BLEP in middle of waveform
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		//--------------------------------
		// Negative phase
		if(mfPhase < 0) {
			// Wrap phase
			while(mfPhase < 0)
				mfPhase += 1;

			// Add BLIT
			AddBLEP(mfPhase / fFreq, -1);

			mdVolume = 1.0f;
		
		}

		float fOut = (float)mdVolume;

		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		// Convert to bipolar and remove DC
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		

		pfOut[l] = (tfloat32)fOut * fFMVol;

	}
}

void COscillatorBLIT::ProcessSquareFM(tfloat32* pfOut, const tfloat fPitch, tfloat fPWM, const tfloat32* pfFM, const tfloat pfFMAmt, tint lC)
{

	float fFMVol	=	(172.0f - pfFMAmt) * (1.0f/172.0f);
	float fFreq = fPitch * mfOneDivSampleRate;

	for (long l = 0; l < lC; l++) {
		
		//  FM Amount
		float fFM		=	1.0f + (pfFMAmt * pfFM[l]);

		mfPhase += fFreq * fFM;

		// Avoid narrow pulses on high notes
		float fPW	= fPWM + fFreq;

		if (mfPhase >= 1) {
			// Add BLEP at end of waveform
			while(mfPhase >= 1)
				mfPhase -= 1;

			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		else if (!mdVolume && mfPhase > fPW) {
			// Add BLEP in middle of waveform
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		//--------------------------------
		// Negative phase
		if(mfPhase < 0) {
			// Wrap phase
			while(mfPhase < 0)
				mfPhase += 1;

			// Add BLIT
			AddBLEP(mfPhase / fFreq, -1);

			mdVolume = 1.0f;
		
		}

		float fOut = (float)mdVolume;

		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		// Convert to bipolar and remove DC
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		

		pfOut[l] = (tfloat32)fOut * fFMVol;

	}
}

void COscillatorBLIT::ProcessSquare(tfloat32* pfOut, const tfloat32* pfPitch, const tfloat32* pfPWM, tint lC)
{
	for (long l = 0; l < lC; l++) {
		float fFreq = pfPitch[l] * mfOneDivSampleRate;

		mfPhase += fFreq;

		if (mfPhase >= 1) {
			// Add BLEP at end of waveform
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		// Avoid narrow pulses on high notes
		float fPW	= pfPWM[l] + fFreq;

		if (!mdVolume && mfPhase > fPW) {
			// Add BLEP in middle of waveform
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}

		float fOut = (float)mdVolume;

		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}

		// Convert to bipolar and remove DC
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;

		pfOut[l] = (tfloat32)fOut;
	}
}

void COscillatorBLIT::ProcessSquare(tfloat32* pfOut, const tfloat fPitch, const tfloat pfPWM, tint lC)
{

	float fFreq = fPitch * mfOneDivSampleRate;

	for (long l = 0; l < lC; l++) {
		

		mfPhase += fFreq;

		if (mfPhase >= 1) {
			// Add BLEP at end of waveform
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		// Avoid narrow pulses on high notes
		float fPW	= pfPWM + fFreq;

		if (!mdVolume && mfPhase > fPW) {
			// Add BLEP in middle of waveform
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}

		float fOut = (float)mdVolume;

		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
//		if (++mlBufferIndex == mlBufferSize) {
//			mlBufferIndex = 0;
//		}
		mlBufferIndex &= 31;

		// Convert to bipolar and remove DC
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;

		pfOut[l] = (tfloat32)fOut;
	}
}

void COscillatorBLIT::ProcessSquare32(tfloat32* pfOut, const float fPitch, const float pfPWM)
{
	float fFreq = fPitch * mfOneDivSampleRate;
	float fPW	= pfPWM + fFreq;

	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[0] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[1] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[2] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[3] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[4] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[5] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[6] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[7] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[8] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[9] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[10] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[11] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[12] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[13] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[14] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[15] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[16] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[17] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[18] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[19] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[20] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[21] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[22] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[23] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[24] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[25] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[26] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[27] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[28] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[29] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[30] = (tfloat32)fOut;
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		if (!mdVolume && mfPhase > fPW) {
			mdVolume = 1;
			AddBLEP((mfPhase - fPW) / fFreq, -1);
		}
		float fOut = (float)mdVolume;
		fOut += (float)mpdBuffer[mlBufferIndex];
		mpdBuffer[mlBufferIndex] = 0;
		mlBufferIndex &= 31;
		fOut				-=	1.0f - fPW;
		fOut				*=	2.0f;
		pfOut[31] = (tfloat32)fOut;
	}
}


void COscillatorBLIT::ProcessSquareSH(tfloat32* pfOut, const tfloat32* pfPitch, tint lC)
{
	const float fPWM1 = 0.25f;
	const float fPWM2 = 0.75f;
	const float fOffsetVolume = 0.15f;

	for (long l = 0; l < lC; l++) {
		double dFreq = pfPitch[l] * mfOneDivSampleRate;

		mfPhase += (float)dFreq;

		if (mfPhase >= 1) {
			// Add BLEP at end of waveform
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / dFreq, 1.0f);
		}

		if (!mdVolume && mfPhase > fPWM1) {
			// Add BLEP in middle of waveform
			mdVolume = fOffsetVolume;
			AddBLEP((mfPhase - fPWM1) / dFreq, -fOffsetVolume);
		}

		if ((mdVolume < 1) && mfPhase > fPWM2) {
			// Add BLEP in middle of waveform
			mdVolume = 1;
			AddBLEP((mfPhase - fPWM2) / dFreq, -(1 - fOffsetVolume));
		}

		double dOut = mdVolume;

		// Add BLEP buffer contents
		dOut += mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}

		pfOut[l] = (tfloat32)((dOut - 0.5) * 2);
	}
}

void COscillatorBLIT::ProcessSquareSync(tfloat32* pfOut, const tfloat32* pfPitchSlave, const tfloat32* pfPitchMaster, const tfloat32* pfPWM, tint lC)
{
	for (long l = 0; l < lC; l++) {
		double dFreqMaster = pfPitchMaster[l] * mfOneDivSampleRate;
		mfPhaseMaster += (float)dFreqMaster;

		double dFreqSlave = pfPitchSlave[l] * mfOneDivSampleRate;
		mfPhase += (float)dFreqSlave;

		if (mfPhaseMaster >= 1) {
			mfPhaseMaster -= 1;

			double dPhaseDelta = mfPhase;
			mfPhase = 0;
			mdVolume = 0;
			AddBLEP(0, dPhaseDelta);
		}

		if (mfPhase >= 1) {
			// Add BLEP at end of waveform
			mfPhase -= 1;
			mdVolume = 0;
			AddBLEP(mfPhase / dFreqSlave, 1.0f);
		}

		if (!mdVolume && mfPhase > pfPWM[l]) {
			// Add BLEP in middle of waveform
			mdVolume = 1;
			AddBLEP((mfPhase - pfPWM[l]) / dFreqSlave, -1);
		}

		double dOut = mdVolume;

		// Add BLEP buffer contents
		dOut += mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}

		pfOut[l] = (tfloat32)((dOut - 0.5) * 2);
	}
}

void COscillatorBLIT::ProcessSaw(tfloat32* pfOut, const tfloat32* pfPitch, tint lC)
{
	for (long l = 0; l < lC; l++) {
		float fFreq		=	pfPitch[l] * mfOneDivSampleRate;
		
		mfPhase += fFreq;

		if (mfPhase >= 1) {
			// Add BLEP at end of waveform
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}

		float fOut = mfPhase;

		// Remove DC
		fOut	-=	fFreq * 2.65f;

		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}

		pfOut[l] = (tfloat32)((fOut - 0.5f) * 2);
	}
}

void COscillatorBLIT::ProcessSaw(tfloat32* pfOut, tfloat fPitch, tint lC)
{
	float fFreq		=	fPitch * mfOneDivSampleRate;

	for (long l = 0; l < lC; l++) {
	
		
		mfPhase += fFreq;

		if (mfPhase >= 1) {
			// Add BLEP at end of waveform
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}

		float fOut = mfPhase;

		// Remove DC
		fOut	-=	fFreq * 2.65f;

		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}

		pfOut[l] = (tfloat32)((fOut - 0.5f) * 2);
	}
}

void COscillatorBLIT::ProcessSaw32(tfloat32* pfOut, float fPitch)
{
	float fFreq		=	fPitch * mfOneDivSampleRate;
	float fOut;
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[0] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[1] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[2] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[3] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[4] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[5] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[6] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[7] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[8] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[9] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[10] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[11] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[12] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[13] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[14] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[15] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[16] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[17] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[18] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[19] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[20] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[21] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[22] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[23] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[24] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[25] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[26] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[27] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[28] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[29] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[30] = (tfloat32)((fOut - 0.5f) * 2);
	}
	{
		mfPhase += fFreq;
		if (mfPhase >= 1) {
			mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		fOut = mfPhase;
		// Remove DC
		fOut	-=	fFreq * 2.65f;
		// Add BLEP buffer contents
		fOut += (float)mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}
		pfOut[31] = (tfloat32)((fOut - 0.5f) * 2);
	}
}

void COscillatorBLIT::ProcessSawFM(tfloat32* pfOut, const tfloat32* pfPitch,const tfloat32* pfFM, const tfloat32* pfFMAmt, tint lC)
{
	for (long l = 0; l < lC; l++) {

		//  FM Amount
		float fFM		=	1.0f + (pfFMAmt[l] * pfFM[l]);
		float fFMVol	=	(160.0f - pfFMAmt[l]) * (1.0f/160.0f);


		double dFreq = pfPitch[l] * mfOneDivSampleRate;

		mfPhase += (float)(dFreq * fFM);

		if (mfPhase >= 1) {
			// Add BLEP at end of waveform
			while(mfPhase >= 1)
				mfPhase -= 1;
			AddBLEP(mfPhase / dFreq, 1.0f);
		}
		else if(mfPhase < 0) {
			// Add BLEP at end of waveform
			while(mfPhase < 0)
				mfPhase += 1;
			AddBLEP(mfPhase / dFreq, -1.0f);
		}

		double dOut = mfPhase;

		// Add BLEP buffer contents
		dOut += mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}

		dOut *= fFMVol;

		pfOut[l] = (tfloat32)((dOut - 0.5) * 2);
	}
}

void COscillatorBLIT::ProcessSawFM(tfloat32* pfOut, const tfloat fPitch, const tfloat32* pfFM, const tfloat fFMAmt, tint lC)
{
	for (long l = 0; l < lC; l++) {

		//  FM Amount
		float fFM		=	1.0f + (fFMAmt * pfFM[l]);
		float fFMVol	=	(160.0f - fFMAmt) * (1.0f/160.0f);


		float fFreq = fPitch * mfOneDivSampleRate;

		mfPhase += fFreq * fFM;

		if (mfPhase >= 1) {
			// Add BLEP at end of waveform
			while(mfPhase >= 1)
				mfPhase -= 1;
			AddBLEP(mfPhase / fFreq, 1.0f);
		}
		else if(mfPhase < 0) {
			// Add BLEP at end of waveform
			while(mfPhase < 0)
				mfPhase += 1;
			AddBLEP(mfPhase / fFreq, -1.0f);
		}

		double dOut = mfPhase;

		// Add BLEP buffer contents
		dOut += mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
//		if (++mlBufferIndex == mlBufferSize) {
//			mlBufferIndex = 0;
//		}
		mlBufferIndex &= 31;

		dOut *= fFMVol;

		pfOut[l] = (tfloat32)((dOut - 0.5) * 2);
	}
}

void COscillatorBLIT::ProcessSawSync(tfloat32* pfOut, const tfloat32* pfPitchSlave, const tfloat32* pfPitchMaster, tint lC)
{
	for (long l = 0; l < lC; l++) {
		double dFreqMaster = pfPitchMaster[l] * mfOneDivSampleRate;
		mfPhaseMaster += (float)dFreqMaster;

		double dFreqSlave = pfPitchSlave[l] * mfOneDivSampleRate;
		mfPhase += (float)dFreqSlave;

		if (mfPhaseMaster >= 1) {
			mfPhaseMaster -= 1;

			double dPhaseDelta = mfPhase;
			mfPhase = 0;
			AddBLEP(0, dPhaseDelta);
		}

		if (mfPhase >= 1) {
			// Add BLEP at end of waveform
			mfPhase -= 1;
			AddBLEP(mfPhase / dFreqSlave, 1.0f);
		}

		double dOut = mfPhase;

		// Add BLEP buffer contents
		dOut += mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}

		pfOut[l] = (tfloat32)((dOut - 0.5) * 2);
	}
}

void COscillatorBLIT::ProcessSawAndSquare(tfloat32* pfOut, const tfloat32* pfPitch, const tfloat32* pfPWM, tfloat fSawVolume, tfloat fSquareVolume, tint lC)
{
	for (long l = 0; l < lC; l++) {
		double dFreq = pfPitch[l] * mfOneDivSampleRate;

		mfPhase += (float)dFreq;

		if (mfPhase >= 1) {
			// Add BLEP at end of waveform
			mfPhase -= 1;
			mdVolume = 0;
//			AddBLEP(mfPhase / dFreq, 2.0f);
			AddBLEP(mfPhase / dFreq, fSawVolume + fSquareVolume);
		}

		if (!mdVolume && mfPhase > pfPWM[l]) {
			// Add BLEP in middle of waveform
			mdVolume = 1;
//			AddBLEP((mfPhase - pfPWM[l]) / dFreq, -1);
			AddBLEP((mfPhase - pfPWM[l]) / dFreq, -fSquareVolume);
		}

		double dOut = (mfPhase * fSawVolume) + (mdVolume * fSquareVolume);

		// Add BLEP buffer contents
		dOut += mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}

//		pfOut[l] = (tfloat32)((dOut - 1) * 2);
//		pfOut[l] = (tfloat32)((dOut - 0.75) * 2);
		pfOut[l] = (tfloat32)((dOut - ((fSawVolume + fSquareVolume) * 0.5)) * 2);
	}
}

void COscillatorBLIT::ProcessSawAndSquareSync(tfloat32* pfOut, const tfloat32* pfPitchSlave, const tfloat32* pfPitchMaster, const tfloat32* pfPWM, tfloat fSawVolume, tfloat fSquareVolume, tint lC)
{
	for (long l = 0; l < lC; l++) {
		double dFreqMaster = pfPitchMaster[l] * mfOneDivSampleRate;
		mfPhaseMaster += (float)dFreqMaster;

		double dFreqSlave = pfPitchSlave[l] * mfOneDivSampleRate;
		mfPhase += (float)dFreqSlave;

		if (mfPhaseMaster >= 1) {
			mfPhaseMaster -= 1;

			double dPhaseDelta = mfPhase;
			mfPhase = 0;
			mdVolume = 0;
//			AddBLEP(0, dPhaseDelta + mdVolume);
			AddBLEP(0, (dPhaseDelta * fSawVolume) + (mdVolume * fSquareVolume));
		}

		if (mfPhase >= 1) {
			// Add BLEP at end of waveform
			mfPhase -= 1;
			mdVolume = 0;
//			AddBLEP(mfPhase / dFreqSlave, 2.0f);
			AddBLEP(mfPhase / dFreqSlave, fSawVolume + fSquareVolume);
		}

		if (!mdVolume && mfPhase > pfPWM[l]) {
			// Add BLEP in middle of waveform
			mdVolume = 1;
//			AddBLEP((mfPhase - pfPWM[l]) / dFreqSlave, -1);
			AddBLEP((mfPhase - pfPWM[l]) / dFreqSlave, -fSquareVolume);
		}

		double dOut = (mfPhase * fSawVolume) + (mdVolume * fSquareVolume);

		// Add BLEP buffer contents
		dOut += mpdBuffer[mlBufferIndex];
		// Reset BLEP buffer content
		mpdBuffer[mlBufferIndex] = 0;
		if (++mlBufferIndex == mlBufferSize) {
			mlBufferIndex = 0;
		}

//		pfOut[l] = (tfloat32)((dOut - 1) * 2);
		pfOut[l] = (tfloat32)((dOut - ((fSawVolume + fSquareVolume) * 0.5)) * 2);
	}
}


