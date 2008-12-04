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


//! RAM allocated for echo effect (per channel). Must be a power of 2.
const tfloat	gfAnti_denormal		=	1e-10f;

class CFilterDSP : public virtual CBaseDSPEngine
{
public:
	//! Constructor
	CFilterDSP();

	//! Destructor
	virtual ~CFilterDSP();

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
	
//	virtual void SetBypass(tbool bBypass);
	virtual void Set_Delay_Time(tint32 iMSec);
	virtual void Set_Feedback(tint32 iFeedback);
	virtual void Set_LF_Damping(tint32 iLF_Damping);
	virtual void Set_HF_Damping(tint32 iHP_Damping);
	virtual void Set_Sync_To_Tempo(tbool bSync);
	virtual void Set_Delay_in_Samples();
	virtual void Set_Mix(tint32 iMix);


protected:
	st::IDSPTools* mpDSPTools;
	
	tuint32 muiChannels;
	
	tuint32 muiSampleRate;

	//! Feedback
	tfloat32 mfFeedback;
	//! Damping
	tfloat32 mfDamping;
	//! High freq Damping
	tfloat32 mfHPDamping;
	//! Buffer (left channel)
	tfloat* mpfBuffer1;
	//! Buffer (right channel)
	tfloat* mpfBuffer2;
	//! Current writing position into buffer
	tint32 miPos;
	//! Current delay time in samples
	tint32 miInterFaceDelayTime;
	//! Current delay time in samples
	tint32 miDelayTime;
	//! Current Host delay time in samples
	tint32 miHostDelayTime;
	//! Synck to host time
	tbool  mbSyncToHost;
	//! Low pass history
	tfloat32 mpfLPHistory[2];
	//! High pass history
	tfloat32 mpfHPHistory[2];
	
	tfloat32 mfMixWet;
	tfloat32 mfMixDry;
	tint32 miMSec;
	
};

