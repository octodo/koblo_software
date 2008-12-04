/*!	\file CWaitCursorWin.cpp
	\author Michael Olsen
	\date 29/Jun/2005
	\date 29/Jun/2005
*/


#include "geInternalOS.h"


CWaitCursorWin::CWaitCursorWin()
{
	mhCursor = ::SetCursor(LoadCursor(NULL, IDC_WAIT));
}

CWaitCursorWin::~CWaitCursorWin()
{
	::SetCursor(mhCursor);
}

IWaitCursor* IWaitCursor::Create()
{
	return dynamic_cast<IWaitCursor*>(new CWaitCursorWin());
}

void CWaitCursorWin::Destroy()
{
	delete dynamic_cast<CWaitCursorWin*>(this);
}







