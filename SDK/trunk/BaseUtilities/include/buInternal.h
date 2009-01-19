/*!	\file buInternal.h
	\author Michael Olsen
	\date 09/Dec/2004
	\date 09/Dec/2004
*/

#include "bu.h"


#ifdef WIN32
#include <io.h>
#endif

#ifdef WIN32
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifdef WIN32
// Inheritance via dominance
#pragma warning(disable: 4250)
// Identifier truncated to 255 characters
#pragma warning(disable: 4786)
#endif	// WIN32

namespace k2s {

// Utilities
#ifdef WIN32
#include "CTimeWin.h"
#else
#include "CTimeOSX.h"
#endif	// WIN32
#include "CSystem.h"
#ifdef WIN32
#include "CTimerWin.h"
#else
#include "CTimerOSX.h"
#endif	// WIN32

// Exceptions
#include "CException.h"

// Unique ID generator
#include "CUniqueID.h"

// File
#include "CFileCP.h"
#include "CFile.h"
#include "CFileMemory.h"
#include "CChunk.h"
#include "CChunkMemory.h"
#include "CChunkFile.h"
#include "CChunkFileMemory.h"

// File search
#ifdef WIN32
#include "CFileSearchWin.h"
#else
#include "CFileSearchOSX.h"
#endif	// WIN32

}	// namespace k2s


using namespace k2s;

