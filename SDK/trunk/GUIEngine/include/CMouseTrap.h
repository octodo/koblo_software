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


/*! \class CMouseTrap
 * \brief Implementation of IMouseTrap
*/

class CMouseTrap : public virtual IMouseTrap, public virtual CControl, public virtual CEventReceiver
{
public:
	//! Constructor
	CMouseTrap();
	//! Destructor
	virtual ~CMouseTrap();

	//! IDestructable override
	virtual void Destroy();
 

	//! IMouseTrap implementation
	virtual void PlaceOnTopOf(IControl* pControl);


	//! IMouseTrap implementation
	virtual void EnableOneTrap(const EMouseMsg msg, tbool bEnable = true, tbool bRemove = true);
	//! IMouseTrap implementation
	virtual void EnableAllTraps(tbool bEnable = true, tbool bRemove = true);
	//! IMouseTrap implementation
	virtual void EnableOneTrapOutside(const EMouseMsg msg, tbool bEnable = true, tbool bRemove = false);
	//! IMouseTrap implementation
	virtual void EnableAllTrapsOutside(tbool bEnable = true, tbool bRemove = false);

	//! IMouseTrap implementation
	virtual void AddBlindSpot(const SRect& rRect);
	//! IMouseTrap implementation
	virtual tbool RemoveBlindSpot(const SRect& rRect);
	//! IMouseTrap implementation
	virtual void ClearBlindSpots();
	//! IMouseTrap implementation
	virtual tbool IsWithinBlindSpot(const SPos& rPos);

	//! IControl override
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos);
	//! IControl override
	virtual void OnDraw(const SRect &rUpdate);
	//! IControl override
	virtual tbool OnKeyDown(EKey Key);

protected:
	tbool mbTraps[NrOfMouseMessages][2];
	tbool mbTrapsOutside[NrOfMouseMessages][2];

	//! List of blind spots
	std::list<SRect> mRectBlindSpots;

};

