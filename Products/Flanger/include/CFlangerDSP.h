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

//! RAM allocated for white chorus effect (per channel). Must be a power of 2.
#define EFFECT_FLANGER_BUFFER_SIZE 1024
const tfloat	gfAnti_denormal					=	1e-10f;

class CFlangerDSP : public virtual CBaseDSPEngine
{
public:
	//! Constructor
	CFlangerDSP();

	//! Destructor
	virtual ~CFlangerDSP();

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
	
	
	virtual void Set_Time(tint32 iValue);
	virtual void Set_Depth(tint32 iValue);
	virtual void Set_Rate(tint32 iValue);
	virtual void Set_Feedback(tint32 iValue);
	virtual void Set_Mix(tint32 iValue);

protected:
	tuint32 muiChannels;
	//! CurreÊnt depth
	tfloat mfDepth;

	//! 1 / current rate
	tfloat mfOneOverRate;

	//! Tap modulatoin
	tfloat mfIndexCur;

	//! Feedback delay in samples
	tint miFeedback;

	//! Feedback
	tfloat mfFeedback;

	//! Buffer (left channel)
	tfloat* mpfBuffer1;
	//! Buffer (right channel)
	tfloat* mpfBuffer2;
	//! Current writing position into buffer
	tint32 miPos;
	//! Current delay time in samples
	tint32 miDelayTime;
	
	tfloat32 mfDry;
	tfloat32 mfWet;
	
	tuint muiSampleRate;
	
};

