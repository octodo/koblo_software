/*!	\file mioe.h
	\author Michael Olsen
	\date 07/Feb/2005
	\date 07/Feb/2005
*/

#ifndef _MIOE_INCLUDED
#define _MIOE_INCLUDED

// BaseUtilities
#include "bu.h"
// Data Engine
//#include "de.h"

namespace k2s {

/*! \namespace
 * \brief Namespace for MIDI I/O engine
 *
 * This namespace is for accessing MIDI input and output
*/
namespace mioe {

// Callbacks
#include "IIDeviceCallback.h"

// Devices
#include "IODevice.h"
#include "IIDevice.h"

// Manager
#include "IDeviceManager.h"


}	// namespace mioe

}	// namespace k2s

#endif	_MIOE_INCLUDED

