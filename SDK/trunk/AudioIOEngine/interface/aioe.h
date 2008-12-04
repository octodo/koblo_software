/*!	\file aioe.h
	\author Michael Olsen
	\date 08/Feb/2005
	\date 08/Feb/2005
*/

#ifndef _AIOE_INCLUDED
#define _AIOE_INCLUDED

// BaseUtilities
#include "bu.h"

#ifdef _WIN32
// STools
#include "STools.h"
#endif // _WIN32

namespace k2s {

/*! \namespace
 * \brief Namespace for Audio I/O engine
 *
 * This namespace is for accessing audio input and output in (potentially) various formats (ASIO, DirectX, dedicated hardware, etc. etc.)
*/
namespace aioe {

// Device
#include "IDeviceCallback.h"
#include "IDevice.h"

// Manager
#include "IDeviceManager.h"

}	// namespace aioe

}	// namespace k2s

#endif	_AIOE_INCLUDED

