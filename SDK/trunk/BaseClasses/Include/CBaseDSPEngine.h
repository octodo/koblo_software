// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
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
// along with the Koblo SDK. If not, see <http://www.gnu.org/licenses/>.


//! Maximum number of signal (standard) meters in a given algorithm
#define BASE_MAX_NR_OF_SIGNAL_METERS 8

//! Maximum number of GR meters in a given algorithm
#define BASE_MAX_NR_OF_GR_METERS 8

/*! \class CBaseDSPEngine
 * \brief ...
*/
class CBaseDSPEngine : public virtual IDestructable
{
public:
	//! Constructor
	CBaseDSPEngine();

	//! Destructor
	~CBaseDSPEngine();

	//! Sets the max buffer size
	virtual void SetMaxBufferSize(tint32 uiMaxBufferSize);

	//! Asks if a samplerate is supported
	virtual tbool SupportSampleRate(tuint32 uiSampleRate);

	//! Sets the sample rate. Inhering methods must call base version
	virtual void SetSampleRate(tuint32 uiSampleRate);

	//! CBasePlugIn override. Inhering class must override this.
	virtual void Process(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples) = 0;

	//! Start processing. Inhering class must override this.
	virtual void Start() = 0;

	//! Stop processing. Inhering class must override this.
	virtual void Stop() = 0;

	//! Gets signal meter
	virtual tfloat GetMeterSignal(tint iIndex);

	//! Gets GR meter
	virtual tfloat GetMeterGR(tint iIndex);

	//! Gets and clears signal meter
	virtual tfloat GetMeterSignalAndClear(tint iIndex);

	//! Gets and clears GR meter
	virtual tfloat GetMeterGRAndClear(tint iIndex);

	//! Sets the bypass status
	virtual void SetBypass(tbool bBypass);

protected:
	//! Current max. buffer size (sub-buffer)
	tuint32 muiMaxBufferSize;

	//! Current sample rate
	tuint32 muiSampleRate;

	//! Current bypass status
	tbool mbBypass;

	//! Current signal meter values
	tfloat mpfMeterSignal[BASE_MAX_NR_OF_SIGNAL_METERS];

	//! Current GR meter values
	tfloat mpfMeterGR[BASE_MAX_NR_OF_GR_METERS];
};
