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


/*! \class CBaseDezipper2
 * \brief ...
*/
class CBaseDezipper2 : public virtual IDestructable
{
public:
	//! Constructor
	/*!
		\param fMin [in]: Min. value of parameter
		\param fMax [in]: Max. value of parameter
		\param fCurrent [in]: Current value of parameter (determines start-up state)
		\param fMSChangeTime [in]: Time in milliseconds it takes to change parameter from min to max value. Defaults to 200.
	*/
	CBaseDezipper2(tfloat fMin, tfloat fMax, tfloat fCurrent, tfloat64 fMSChangeTime = 200.0);

	//! IDestructable override
	virtual void Destroy();

	//! Sets the target value for dezipper
	/*!
		\param fTarget [in]: Target to set. Must be within range of parmeter
	*/
	virtual void SetTarget(tfloat fTarget);
	
	//! Gets the current value for dezipper. Note! only use if dezipping isn't actually needed!
	/*!
		\return tfloat: Current value of dezipper
	*/
	virtual tfloat GetCurrent() const { return mfCurrentValue; };

	//! Call this to do the dezip action. The latest dezip value will be returned.
	/*!
		\param iSamples [in]: Time in samples. since dezipper was last called (determine from stream time, not real time).
		\param iSampleRate [in]: Sample frequency of system.
		\return tfloat: New current value of dezip
	*/
	virtual tfloat DezipSamples(tint32 iSamples, tint32 iSampleRate);

	//! Call this to do the dezip action. The buffer will be filled with the dezip values.
	/*!
		\param pfDezip [out]: Buffer to be filled with dezip values
		\param iSamples [in]: Number of samples to fill in buffer.
		\param iSampleRate [in]: Sample frequency of system.
	*/
	virtual void DezipSamples(tfloat* pfDezip, tint32 iSamples, tint32 iSampleRate);
	
	//! Tells if dezipping is needed (value changing)
	/*!
		\return tbool: True = dezipping is needed, False = value doesn't change anymore - no need to dezip
	*/
	virtual tbool IsDezipNeeded() const { return mbIsDezipNeeded; };

	//! Call this to reset dezipper. The value will be changed (fCurrentValue = fTargetValue)
	virtual void Reset();

protected:
	//! How much the parameter should change per MS when going from min to max
	tfloat64 mfChangePerMS;
	//! Current value
	tfloat mfCurrentValue;
	//! Target value (where we're "going to")
	tfloat mfTargetValue;
	//! Are we still "travelling"?
	tbool mbIsDezipNeeded;

	//! Destructor
	~CBaseDezipper2();
};


