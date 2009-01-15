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


/*! \class CLED
 * \brief Implementation of ISlider
*/
class CLED : public virtual ILED, public virtual CControl
{
public:
	//! Constructor
	CLED();
	//! Destructor
	virtual ~CLED();

	//! IDestructable override
	virtual void Destroy();

	//! IControl override
	virtual void OnDraw(const SRect &rUpdate);
	//! IControl override
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos) {return false;}
	//! IControl override
	virtual tbool OnKeyDown(EKey Key) {return false;}	
	//! IControl override
	virtual void OnTimer();


	//! ISlider override
	virtual void Init(tint32 iID, tint32 iMin, tint32 iMax, SSize Size,IWindow* pParentWindow, SRGB RGB);

	//! Set
	/*!
		\param iValue [in]: Value to draw

	*/
	virtual void Set(tint32 iValue);

protected:
	

	tint32	miMin;
	tint32	miMax;
	tint32	miCurrent;
	SRGB	mColor;

	IDrawPrimitives* mpPrimitives;

};
