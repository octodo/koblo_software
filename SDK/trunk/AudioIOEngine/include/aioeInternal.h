/*!	\file aioeInternal.h
	\author Michael Olsen
	\date 08/Feb/2005
	\date 08/Feb/2005
*/

#include "aioe.h"

// Inheritance via dominance
#pragma warning(disable: 4250)
// Identifier truncated to 255 characters
#pragma warning(disable: 4786)

// ASIO
#ifdef WIN32
#include "asiodrivers.h"
#include "common\iasiodrv.h"
#include "ASIOConvertSamples.h"
#endif	// WIN32

namespace k2s {

namespace aioe {

// Devices
#ifdef WIN32
#include "CDeviceASIO.h"
#include "CDeviceWaveIO.h"
#endif	// WIN32
#ifdef _Mac
#include "CDeviceCoreAudio.h"
#endif	// _Mac

// Managers
#ifdef WIN32
#include "CDeviceManagerASIO.h"
#include "CDeviceManagerWaveIO.h"
#endif	// WIN32
#ifdef _Mac
#include "CDeviceManagerCoreAudio.h"
#endif	// _Mac


}	// namespace aioe

}	// namespace k2s


using namespace k2s;

using namespace aioe;
