// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
//
// This file is part of the Koblo SDK.
//
// the Koblo SDK is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo SDK is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.


class CVChorusDSP : public virtual CBaseDSPEngine
{
public:
	//! Constructor
	CVChorusDSP();

	//! Destructor
	virtual ~CVChorusDSP();

	//! IDestructable override
	virtual void Destroy();

	// CBaseDSPEngine override
	virtual void Process(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples);
	// CBaseDSPEngine override
	virtual void Start();
	// CBaseDSPEngine override
	virtual void Stop();
	// CBaseDSPEngine override
	virtual void SetSampleRate(tuint32 uiSampleRate);
	// CBaseDSPEngine override
	virtual void SetMaxBufferSize(tint32 uiMaxBufferSize);

	void SetChannels(tuint32 uiChannels);

	void ProcessMono(float** ppfOut, const float** ppfIn, long lC);
	void ProcessStereo(float** ppfOut, const float** ppfIn, long lC);
	
	virtual void SetDelayTime(tint32 iDelayTime);
	
	virtual void SetDepth(tint32 iDepth);
	
	virtual void SetRate(tint32 iRate);
	
	virtual void SetWith(tint32 iWith);
	
	virtual void SetMix(tint32 iMix);

protected:
	tuint32 muiChannels;
	
	//! RAM allocated for vintage chorus effect (per channel). Must be a power of 2.
	#define EFFECT_CHORUSVINTAGE_BUFFER_SIZE 4096

	tuint32 muiSampleRate;
	
	
	//! CurreÊnt depth
	tfloat mfDepth;

	//! 1 / current rate
	tfloat mfOneOverRate;

	//! Current width
	tfloat mfWidth;

	//! Tap modulatoin
	tfloat mfIndexCur;

	//! Buffer (left channel)
	tfloat* mpfBuffer1;
	//! Buffer (right channel)
	tfloat* mpfBuffer2;
	//! Current writing position into buffer
	tint32 miPos;
	//! Current delay time in samples
	tint32 miDelayTime;

	//! LP Coefficient
	tfloat mfLPCoeff;
	//! LP History
	tfloat64 mfLPHistoryL;
	tfloat64 mfLPHistoryR;

	//! HP Coefficient
	tfloat mfHPCoeff;
	//! HP History
	tfloat64 mfHPHistoryL;
	tfloat64 mfHPHistoryR;
	
	tfloat32 mfMixWet;
	tfloat32 mfMixDry;
};

