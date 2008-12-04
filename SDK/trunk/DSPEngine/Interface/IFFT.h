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


/*! \class IFFT
 * \brief Class to do fast fourier transform on time domain audio data
*/
class IFFT : public virtual IDestructable
{
public:
	//! Create FFT object
	static IFFT* Create();

	//! Initialize
	/*!
		\param uiOrder [in]: Order of FFT. FFTSize == 2 ^ uiOrder. Must be 1 or higher
	*/
	virtual void Init(tuint uiOrder) = 0;

	//! Calculate magnitude on signal
	/*!
		\param pfSamples [in/out]: On input: Array of samples to get magnitude for. Length of array is 2 ^ uiOrder (see Init()). On output: (2 ^ uiOrder) / 2 + 1 samples magnitude values.
	*/
	virtual void GetMagnitude(sample* pfSamples) throw(IException*) = 0;
};
