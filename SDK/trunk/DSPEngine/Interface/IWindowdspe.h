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


/*! \class IWindow
 * \brief Class to apply misc. window functions to time domain data
*/
class IWindow : public virtual IDestructable
{
public:
	//! Create IWindow object
	static IWindow* Create();

	//! Window types
	enum EWindow
	{
		//! 4th order Blackman/Harris window.
		WindowBlackmanHarris = 0,
		//! Hann (also known as Hanning) window
		WindowHann
	};

	//! Initialize. Call this before Apply. Must only be called once.
	/*!
		\param Window [in]: Type of window to create
		\param fData [in]: Some windows may have extra initialization/customization data. See EWindow for details.
		\param uiSize [in]: Size of window.
	*/
	virtual void Init(EWindow Window, tfloat32 fData, tuint32 uiSize) = 0;

	//! Apply window function. Init must have been called first. The size of the buffer applied is equal to the size originally given in Init.
	/*!
		\param pfSamples [in/out]: On input: Array to data to apply window on. On output: Contains the modified data.
	*/
	virtual void Apply(tfloat32* pfSamples) = 0;
};
