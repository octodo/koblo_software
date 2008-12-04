/*!	\file de.h
	\author Michael Olsen
	\date 14/Dec/2004
	\date 14/Dec/2004
*/

#ifndef _DE_INCLUDED
#define _DE_INCLUDED

// BaseUtilities
#include "bu.h"


// This should be moved to something called geConstants.h
const tint32 gControlMin = -32768;
const tint32 gControlMax = 32767;


namespace k2s {

/*! \namespace
 * \brief Namespace for data engine
 *
 * This namespace is for the data engine
*/
namespace de {

// Parameters
#include "IParameter.h"
#include "IParameterManager.h"
#include "IParameterManagerCallback.h"

// Data converters
#include "IConverter.h"

// Display converters
#include "IDisplayConverter.h"

}	// namespace de

}	// namespace k2s

#endif	_DE_INCLUDED

