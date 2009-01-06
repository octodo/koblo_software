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

/*! A hard disk stream, used for sample players.
*
*	Streams are created and managed by the IStreamManager, and should not be created directly by the application / plug-in.
*/
class IStream
{
public:
	//! Initializes and resets the stream. Can be called multiple times with different parameters
	/*!
		\param pFile [in]: File to read stream from
		\param iOffset [in]: Offset into file (in bytes) to start reading from
		\param iLength [in]: Length of sound in file (in samples)
		\param bLoop [in]: If true looping should occur, i.e. after reading iLength samples, reading is restarted from beginning
		\param pInitialData [in]: OBSOLETE! Set to NULL.<br/>Pointer to buffer with 32kb of samples which are identical to the first 32kb of sound.
		\param iBitWidth [in]: Bits per sample. Valid values or 16 or 24, default is 24.
	*/
	virtual void Reset(IFile* pFile, tint32 iOffset, tint32 iLength, tbool bLoop, tint16* piInitialData, tint32 iBitWidth = 24, tint32 iChannels = 1) = 0;

	//! Call this to get the next samples in the stream. The position in the stream will be moved after this call.
	/*!
		\param pfBuffer [out]: Buffer to be filled with data (converter to floating point)
		\param iCount [in]: Number of samples to put in buffer
	*/
	virtual void GetSamples(tfloat32* pfBuffer, tint32 iCount) = 0;

	//! Call this to see if sound playback has stuttered resently (flag will be reset after)
	virtual tbool GetStutter() = 0;

	//! Call this to change the current position
	/*!
		\param tuint32 uiPos: New position, in samples
	*/
	virtual void SetPosition(tuint64 uiPos) = 0;
};
