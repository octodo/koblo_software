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


/*! \class CPeakMeter
 * \brief Implementation of IPeakMeter
*/
class CPeakMeter : public virtual IPeakMeter, public virtual CPane, public virtual CEventReceiver
{
public:
	//! Constructor
	CPeakMeter();
	//! Destructor
	virtual ~CPeakMeter();

	//! IDestructable override
	virtual void Destroy();
 
	//! IPeakMeter implementation
	virtual void Init(tint32 iID, tint32 iSteps, EDirection eDir = Up, tint32 iSpacing0 = 0, tint32 iSpacing1 = 0);

	//! IPeakMeter implementation
	virtual void SetGrowingDirection(EDirection eDir = Up);

	//! IPeakMeter implementation
	virtual void SetFloatValue(tfloat fValue, tbool bNotify = true);

	//! IPeakMeter implementation
	virtual void SetFloatValueWithDecay(tfloat fValue, tfloat fDecayMul, tbool bNotify = true);
	
	//! CPane override
	virtual void AddControl(IControl* pToAdd, const SPos& Pos);
	//! CPane override
	virtual void ControlUpdateAll();
	//! CPane override
	virtual void SetParentPane(IPane* pPane);

	//! IControl override
	virtual void SetValue(tint32 iNew, tbool bNotify = true);
	
protected:
	//! Number of steps that meter is divided into
	tint32 miSteps;

	// Lasse, added 2007-12-17
	//! Number of bitmap pixel lines to ignore for level 0.000 (silence)
	tint32 miSpacing0;
	//! Number of bitmap pixel lines to ignore for level 1.000 (full volume)
	tint32 miSpacing1;
	//! Number of pixel lines that represent the volume steps
	tint32 miPixelLines;
	// .. Lasse

	// Direction that meter grows in
	EDirection meDir;

	//! Original sizes of last bitmap
	SSize mSizeOriginalOfLastBitmap;
	
	//! Latest peak value
	tfloat32 mfPeakLatest;
};

