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

class CAAmpDSP : public virtual CBaseDSPEngine
{
public:
	//! Constructor
	CAAmpDSP();

	//! Destructor
	virtual ~CAAmpDSP();

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
	

	virtual void Set_Gain(tfloat32 fValue);//	{ mfVol		=	fValue;};
	virtual void Set_Limiter(tfloat32 bValue)	{ mbLimiter	=	bValue;};
	virtual void Set_Harm(tbool bValue)			{ mbHarm	=	bValue;};
	virtual void Set_Tube(tbool bValue)			{ mbTube	=	bValue;};
	virtual void Set_Swap(tuint32 uiValue)		{ muiSwap	=	uiValue;};



protected:
	st::IDSPTools* mpDSPTools;
	
	tuint32 muiChannels;
	
	tuint32 muiSampleRate;

	// Volume
	tfloat32 mfVol;
	tfloat32 mfDrive;
	tfloat32 mfInvVol;
	tfloat32 mfHarch;
	//! Limiter off/on
	tbool	mbLimiter;
	// Third order harmonic distortion off/ on
	tbool	mbHarm;
	//! tube simulator off/ on
	tbool	mbTube;
	//! order of Limiter. Harm and Tube
	tuint32 muiSwap;
	
		
	st::IAmpSim* mpAmpSim;
	st::ILimiter* mpLimiter;
	st::ILimiter* mpDriveLimiter;
	//st::ISoftDistortion* mpSoftDistortion;
	st::IDrive* mpSoftDistortion;
	st::IClipper* mpClipper;
	
	st::IAmp* mpAmp;
	
	tfloat32 mfClip;
	
};

