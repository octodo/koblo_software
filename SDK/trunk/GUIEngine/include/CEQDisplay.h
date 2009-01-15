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


/*! \class CEQDisplay
 * \brief Implementation of IEQDisplay
*/
class CEQDisplay : public virtual IEQDisplay, public virtual CPane, public virtual IEventReceiver
{
public:
	//! Constructor
	CEQDisplay();
	//! Destructor
	virtual ~CEQDisplay();

	//! IDestructable override
	virtual void Destroy();

	//! CPane override
	virtual void OnDraw(const SRect &rUpdate);

	//! IEQDisplay override
	virtual void Init(tint iID, IWindow* pWindow, IResourceManager* pResMan, tint32 iDotResID, tint32 iControlIDDot1, tint32 iControlIDDotSpan, tint iDotOffsetX, tint iDotOffsetY, tuint uiNrOfBands, SRGB Colour);
	//! IEQDisplay override
	virtual void Init2(tint iID, IWindow* pWindow, tuint uiNrOfBands, SRGB Colour);
	//! IEQDisplay override
	virtual void SetCallback(IEQDisplayCallback* pCallback, tint32 iUserDefined);
	//! IEQDisplay override
	virtual void SetdBMinAndMax(tfloat fdBMin, tfloat fdBMax);
	//! IEQDisplay override
	virtual void SetBandInfo(tuint uiBand, tbool bEnabled, tfloat32 fFreqIndex, tfloat32 fdBIndex);
	//! IEQDisplay override
	virtual void Update();

	//! CPane override
	virtual void SetSize(const SSize& Size, tbool bRedraw = false);

	//! IEventReceiver override
	virtual void EventValueChange(IControl* pSender, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(IControl* pSender, void* pEventData);

protected:
	//! Draw primitives
	IDrawPrimitives* mpDrawPrimitives;

	//! Number of bands
	tuint muiNrOfBands;

	//! Callback to get curve info from
	IEQDisplayCallback* mpCallback;
	tint32 miCallbackUserDefined;

	//! Buffer to get filled with curve info for 1 band
	tfloat32* mpfBufferCurveBand;

	//! Buffer to sum curve infos into
	tfloat32* mpfBufferCurveTotal;

	//! Offset in dot bitmaps into center position for x direction
	tint32 miDotOffsetX;

	//! Offset in dot bitmaps into center position for y direction
	tint32 miDotOffsetY;

	//! Colour of curve
	SRGB mColour;

	//! Min dB of display
	tfloat mfdBMin;

	//! Max dB of display
	tfloat mfdBMax;

	//! Control ID of first "dot" used to graphically manipulate equalizer
	tint32 miControlIDDot1;
	//! Span between control IDs of the "dots"
	tint32 miControlIDDotSpan;
};
