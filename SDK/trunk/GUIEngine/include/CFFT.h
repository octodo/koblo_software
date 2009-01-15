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
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.


//! Number of queue buffers
const tuint guiFFTNrOfQueueBuffers = 32;

/*! \class CFFT
 * \brief Implementation of IFFT
*/
class CFFT : public virtual IFFT, public virtual CControl
{
public:
	//! Constructor
	CFFT();
	//! Destructor
	virtual ~CFFT();

	//! IDestructable override
	virtual void Destroy();

	//! IControl override
	virtual void OnDraw(const SRect &rUpdate);
	//! IControl override
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos);
	//! IControl override
	virtual tbool OnKeyDown(EKey Key);
	//! IControl override
	virtual void OnTimer();

	//! IFFT override
	virtual void Init(tint32 iID, const SRGB& Colour, IWindow* pParentWindow, tuint32 uiSampleRate, tuint uiChannels);
	//! IFFT override
	virtual void Input(const tfloat32** ppfSamples, tuint32 uiSampleCount);

protected:
	SRGB mColour;

	IDrawPrimitives* mpPrimitives;

	//! FFT object
	dspe::IFFT* mpFFT;

	//! Window object
	dspe::IWindow* mpWindow;

	//! Size of FFT block
	tuint muiFFTSize;

	//! Nyquist frequency
	tuint32 muiNyquist;

	//! Number of audio channels
	tuint muiChannels;

	//! Left channel queue buffers
	tfloat32* mppfQueueLeft[guiFFTNrOfQueueBuffers];

	//! Right channel queue buffers
	tfloat32* mppfQueueRight[guiFFTNrOfQueueBuffers];

	//! Current queue buffer write index
	tuint muiQueueBufferWrite;

	//! Current queue buffer read index. If its the same as muiQueueBufferWrite there's no current buffer to read
	tuint muiQueueBufferRead;

	//! Write index into queue buffer currently being written to
	tuint32 muiQueueBufferWriteIndex;

	//! Left channel processing buffer
	tfloat32* mpfLeft;

	//! Right channel processing buffer
	tfloat32* mpfRight;

	//! Holds info about the border frequencies of 1 band
	struct SBandInfo {
		tfloat64 fMinFreq;
		tfloat64 fMaxFreq;
	};
	SBandInfo* mpBandInfo;

	//! Number of bands
	tuint muiNrOfBands;

	//! History of previous magnitudes
	CAutoBuffer<tfloat32> mpfMagsHist;

	//! History of previous max. volumes
	CAutoBuffer<tfloat32> mpfMaxHist;

	//! Max. volume counters (in ms.). Determines when the max bar begins to drop
	CAutoBuffer<tfloat32> mpfMaxCounter;

	//! Process the queued input (if any) and updates screen if neccasary.
	virtual void Process();

	//! Time we were in Process() last
	tuint32 muiTimeLast;
};
