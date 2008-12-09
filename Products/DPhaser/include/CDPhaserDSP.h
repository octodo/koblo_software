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

const tuint32  guiSinTableSize = 256;
const tfloat	gfAnti_denormal					=	1e-10f;

class CDPhaserDSP : public virtual CBaseDSPEngine
{
public:
	//! Constructor
	CDPhaserDSP();

	//! Destructor
	virtual ~CDPhaserDSP();

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
	
	virtual void SetFreq(tint32 iValue);
	virtual void SetRange(tint32 iValue);
	virtual void SetRate(tint32 iValue);
	virtual void SetFeedback(tint32 iValue);
	virtual void SetMix(tint32 iValue);

protected:
	tuint32 muiChannels;
	
	//! Current sine table index
	tfloat mfCurIndex;

	//! 1 divided by the rate
	tfloat mfOneOverRate;

	//! Current frequency of the phaser (i.e. frequency to be "phased")
	tfloat mfHz;

	//! Current range
	tfloat mfRange;

	//! Current feedback
	tfloat mfFeedback;

	//! Feedback (history) left channel
	tfloat mfFeedbackL;

	//! Feedback (history) right channel
	tfloat mfFeedbackR;

	//! Phaser history left channel
	tfloat mfHistoryL[10];

	//! Phaser history right channel
	tfloat mfHistoryR[10];
	
	tuint32 muiSampleRate;
	
	tfloat32 fMix_Dry;
	tfloat32 fMix_Wet;
};

