// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__088F84D6_FB5E_4B84_9C0D_3977493CD38C__INCLUDED_)
#define AFX_STDAFX_H__088F84D6_FB5E_4B84_9C0D_3977493CD38C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <stdio.h>

#ifdef _Mac
	#include <Carbon/Carbon.h>
#endif

// Base Utilities
#include "bu.h"

#include <stdio.h>
#include <fcntl.h>

#ifdef _WIN32
	#include <io.h>
#endif // _WIN32

#include <sys/stat.h>
#include <string>
#include <list>


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__088F84D6_FB5E_4B84_9C0D_3977493CD38C__INCLUDED_)
