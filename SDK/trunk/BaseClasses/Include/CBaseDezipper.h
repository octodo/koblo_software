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


/*! \class CBaseDezipper
 * \brief ...
*/
class CBaseDezipper : public virtual IDestructable
{
public:
	//! Constructor
	CBaseDezipper();

	//! IDestructable override
	virtual void Destroy();

	//! Sets the callback
	virtual void SetCallback(IBaseDezipperCallback* pCallback);

	//! Adds a parameter to the dezipper list
	/*!
		\param iID [in]: ID of parameter to add
		\param fMin [in]: Min. value of parameter
		\param fMax [in]: Max. value of parameter
		\param fCurrent [in]: Current value of parameter (determines start-up state)
		\param fMSChangeTime [in]: Time in milliseconds it takes to change parameter from min to max value. Defaults to 200.
	*/
	virtual void AddParameter(tint32 iID, tfloat fMin, tfloat fMax, tfloat fCurrent, tfloat64 fMSChangeTime = 200.0);

	//! Sets the target value for a parameter
	/*!
		\param iID [in]: ID of parameter to set target for
		\param fTarget [in]: Target to set. Must be within range of parmeter
	*/
	virtual void SetTarget(tint32 iID, tfloat fTarget);

	//! Call this to do the dezip action. The supplied callback will be called.
	/*!
		\param fMS [in]: Time in ms. since dezipper was last called (determine from stream time, not real time).
	*/
	virtual void Dezip(tfloat fMS);

	//! Call this to do the dezip action. The supplied callback will not be called, rather the latest dezip value will be returned.
	/*!
		\param iID [in]: ID of parameter to dezip
		\param iSamples [in]: Time in samples. since dezipper was last called (determine from stream time, not real time).
		\param iSampleRate [in]: Sample frequency of system.
		\return tfloat: New current value of dezip
	*/
	virtual tfloat DezipSamples(tint32 iID, tint32 iSamples, tint32 iSampleRate);

	//! Call this to do the dezip action. The supplied callback will not be called, rather buffer will be filled with the dezip values.
	/*!
		\param iID [in]: ID of parameter to dezip
		\param pfDezip [out]: Buffer to be filled with dezip values
		\param iSamples [in]: Number of samples to fill in buffer.
		\param iSampleRate [in]: Sample frequency of system.
	*/
	virtual void DezipSamples(tint32 iID, tfloat* pfDezip, tint32 iSamples, tint32 iSampleRate);

	//! Call this to reset dezipper. All values will be changed (fCurrentValue = fTargetValue)
	virtual void Reset();

protected:
	//! Callback
	IBaseDezipperCallback* mpCallback;

	//! Stores information about 1 parameter
	struct SParmInfo {
		//! Parameter ID
		tint32 iParmID;
		//! How much the parameter should change per MS when going from min to max
		tfloat64 fChangePerMS;
		//! Current value
		tfloat fCurrentValue;
		//! Target value (where we're "going to")
		tfloat fTargetValue;
	};

	//! List of parameters
	std::list<SParmInfo> mParmList;

	//! Destructor
	~CBaseDezipper();
};


