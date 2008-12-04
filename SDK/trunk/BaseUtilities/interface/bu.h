/*!	\file bu.h
	\author Michael Olsen
	\date 09/Dec/2004
	\date 09/Dec/2004
*/

#ifndef _BU_INCLUDED
#define _BU_INCLUDED

#ifdef WIN32
// Identifier truncated
#pragma warning(disable: 4786)
// C++ exception specification ignored
#pragma warning(disable: 4290)
// Function was declared deprecateds
#pragma warning(disable: 4996)
#endif

#include <math.h>
#include <stdlib.h>

#include <string>
#include <list>
#include <vector>
#include <map>
#include <iostream>

#ifdef WIN32
#include <Windows.h>
#endif	// WIN32

/*! \namespace
 * \brief Namespace for Key2Sound classes and types
 *
 * All Key2Sound code is inside this namespaces
*/
namespace k2s {

// Base types
#include "BaseTypes.h"

// Base interfaces
#include "IInterface.h"
#include "IDestructable.h"
#include "IRefCountable.h"

// Helper implementations of interfaces
#include "CRefCountable.h"

// General utilities and macros
#include "Utilities.h"
#include "ITime.h"
#include "ISystem.h"
#include "ITimerCallback.h"
#include "ITimer.h"

// Auto delete
#include "CAutoDelete.h"

// Auto buffer
#include "CAutoBuffer.h"

// Exceptions
#include "IException.h"

// Synchronisation
#ifdef WIN32
#include "CMutexWin.h"
#endif	// WIN32
#ifdef _Mac
#include "CMutexMac.h"
#endif
#include "CAutoLock.h"
#include "CLockReleaser.h"

// Unique ID generator
#include "IUniqueID.h"

// File handling
#include "IFile.h"
#include "IFileMemory.h"
#include "IChunk.h"
#include "IChunkFile.h"
#include "IChunkFileMemory.h"

// File search
#include "IFileSearch.h"

}	// namespace k2s


using namespace k2s;


#endif	// _BU_INCLUDED


