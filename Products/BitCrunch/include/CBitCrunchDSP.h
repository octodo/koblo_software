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

class CBitCrunchDSP : public virtual CBaseDSPEngine
{
public:
	//! Constructor
	CBitCrunchDSP();

	//! Destructor
	virtual ~CBitCrunchDSP();

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
	
	virtual void Set_Bypass(tbool bBypass);
	virtual void Set_Bit(tint32 iBit);
	virtual void Set_SR(tint32 iSR);
	virtual void Set_Mix(tint32 iMix);


protected:
//	st::IDSPTools* mpDSPTools;
	
	tuint32 muiChannels;
	
	tfloat32 mfSampleRate;
	//! Feedback
	tfloat32 mfBit;
	tfloat32 mfResBit;
	//! Damping
	tfloat32 mfSR;
	// Mix
	tfloat32 mfMixWet;
	tfloat32 mfMixDry;
	
	tfloat32 mfIterator;
	
	tfloat32 mfLeft;
	tfloat32 mfRight;
	
	
};

