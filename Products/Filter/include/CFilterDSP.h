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

const tint giFreqToCutoffSize	=	16385;

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
	virtual void Set_Cutoff(tuint32 uiCutoff);
	virtual void Set_Resonance(tint32 iFeedback);
	virtual void Set_Mix(tint32 iLF_Damping);
	virtual void Set_LoPass(tint32 iLoPass);
	virtual void Set_BandPass(tint32 iBandPass);
	virtual void Set_HighPass(tint32 iHighPass);



protected:
	st::IDSPTools* mpDSPTools;
	
	tuint32 muiChannels;
	tuint32 muiSampleRate;
	
	tfloat mpfFreqToCutoff[giFreqToCutoffSize+2];

	st::IChamberlinFilter* mpFilterLeft;
	st::IChamberlinFilter* mpFilterRight;
	
	tuint32 muiCutoff;
	tfloat32 mfResonance;
	
	tfloat32 mfLP;
	tfloat32 mfBP;
	tfloat32 mfHP;
	
	tfloat32 mfDry;
	tfloat32 mfWet;
};

