/*!	\file CMouseTrap.cpp
	\author Lasse Steen Bohnstedt
	\date 27/Oct/2006
*/


#include "geInternalOS.h"


CMouseTrap::CMouseTrap()
{
	EnableAllTraps(false);
	EnableAllTrapsOutside(false);
} // constructor


CMouseTrap::~CMouseTrap()
{
} // destructor


IMouseTrap* IMouseTrap::Create()
{
	return dynamic_cast<IMouseTrap*>(new CMouseTrap());
} // Create

void CMouseTrap::Destroy()
{
	delete dynamic_cast<CMouseTrap*>(this);
} // Destroy


void CMouseTrap::OnDraw(const SRect& /*rctUpdate*/)
{
} // CMouseTrap::OnDraw


tbool CMouseTrap::OnKeyDown(EKey /*Key*/)
{
	return false;
} // CMouseTrap::OnKeyDown


void CMouseTrap::PlaceOnTopOf(IControl* pControl)
{
	// Lasse, modified 2008-05-07
	/*
	SPos pos = SPos(0,0);
	SSize size = SSize(0,0);
	if (pControl) {
		pControl->GetAbsPos(pos);
		pControl->GetSize(size);
	}
	SetAbsPos(pos);
	SetSize(size);
	*/
	CloneRect(pControl);
	// .. Lasse
} // CMouseTrap::PlaceOnTopOf


void CMouseTrap::EnableOneTrap(const EMouseMsg msg, tbool bEnable /*= true*/, tbool bRemove /*= true*/)
{
	ASSERT( (msg>=0)&&(msg<NrOfMouseMessages));
	mbTraps[msg][0] = bEnable;
	mbTraps[msg][1] = bRemove;
} // CMouseTrap::EnableOneTrap

	
void CMouseTrap::EnableAllTraps(tbool bEnable /*= true*/, tbool bRemove /*= true*/)
{
	for (tint i=0; i<NrOfMouseMessages; i++) {
		mbTraps[i][0] = bEnable;
		mbTraps[i][1] = bRemove;
	}
} // CMouseTrap::EnableAllTraps


void CMouseTrap::EnableOneTrapOutside(const EMouseMsg msg, tbool bEnable /*= true*/, tbool bRemove /*= false*/)
{
	ASSERT( (msg>=0)&&(msg<NrOfMouseMessages));
	mbTrapsOutside[msg][0] = bEnable;
	mbTrapsOutside[msg][1] = bRemove;
} // CMouseTrap::EnableOneTrapOutside

	
void CMouseTrap::EnableAllTrapsOutside(tbool bEnable /*= true*/, tbool bRemove /*= false*/)
{
	for (tint i=0; i<NrOfMouseMessages; i++) {
		mbTrapsOutside[i][0] = bEnable;
		mbTrapsOutside[i][1] = bRemove;
	}
} // CMouseTrap::EnableAllTrapsOutside


void CMouseTrap::AddBlindSpot(const SRect& rRect)
{
	mRectBlindSpots.insert(mRectBlindSpots.end(), rRect);
} // CMouseTrap::AddBlindSpot


tbool CMouseTrap::RemoveBlindSpot(const SRect& rRect)
{
	// Lasse: Should be optimized
	std::list<SRect>::iterator it = mRectBlindSpots.begin();
	std::list<SRect>::iterator itBlindSpot = mRectBlindSpots.end();
	for (; it != mRectBlindSpots.end(); it++) {
		SRect& rRectInList = *it;
		if (rRectInList==rRect) {
			itBlindSpot = it;
		}
	}
	if (itBlindSpot!=mRectBlindSpots.end()) {
		mRectBlindSpots.erase(itBlindSpot);
		return true;
	}

	return false;
}


void CMouseTrap::ClearBlindSpots()
{
	mRectBlindSpots.clear();
} // CMouseTrap::ClearBlindSpots

tbool CMouseTrap::IsWithinBlindSpot(const SPos& rPos)
{
	std::list<SRect>::iterator it = mRectBlindSpots.begin();
	for (; it != mRectBlindSpots.end(); it++) {
		SRect& rRect = *it;
		if (rRect.Inside(rPos))
			return true;
	}

	return false;
} // CMouseTrap::IsWithinBlindSpot


tbool CMouseTrap::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	if( !IsVisible() )
		return false;

	if ((MouseMsg>=0)&&(MouseMsg<NrOfMouseMessages)) {
		SRect rect = GetRect();
		if (rect.Inside(Pos)) {
			// Inside
			if (mbTraps[MouseMsg][0]) {
				if (!IsWithinBlindSpot(Pos)) {
					SEventGeneric event;
					event.eType = giEventMouse;
					event.bInsideControlRect = true;
					event.iMsg = MouseMsg;
					event.pos = Pos;
					
					NotifyGeneric(&event);

					return mbTraps[MouseMsg][1];
				}
			}
		}
		else {
			// Outside
			if (mbTrapsOutside[MouseMsg][0]) {
				if (!IsWithinBlindSpot(Pos)) {
					SEventGeneric event;
					event.eType = giEventMouse;
					event.bInsideControlRect = false;
					event.iMsg = MouseMsg;
					event.pos = Pos;
					
					NotifyGeneric(&event);

					return mbTrapsOutside[MouseMsg][1];
				}
			}
		}
	}


	return false;
} // CMouseTrap::OnMouse
