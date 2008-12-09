/*!	\file CWaitCursorOSX.cpp
	\author Michael Olsen
	\date 29/Jun/2005
	\date 29/Jun/2005
*/


#include "geInternalOS.h"


CWaitCursorOSX::CWaitCursorOSX()
{
	CursHandle hCursorWatch = ::GetCursor(::watchCursor);

	::SetCursor(*hCursorWatch);}

CWaitCursorOSX::~CWaitCursorOSX()
{
	::InitCursor();
}

IWaitCursor* IWaitCursor::Create()
{
	return dynamic_cast<IWaitCursor*>(new CWaitCursorOSX());
}

void CWaitCursorOSX::Destroy()
{
	delete dynamic_cast<CWaitCursorOSX*>(this);
}







