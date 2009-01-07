// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
//
// This file is part of the Koblo Stools.
//
// the Koblo Stools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo Stools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.


#ifndef _KSPI_C_INCLUDED
#define _KSPI_C_INCLUDED

/*
	This file contains a the C compatible dispatchers, through which all host<->plug-in communication goes.

	Note that while the interface is C compatible, the code currently isn't.
*/

namespace k2s {
namespace kspi {

//! Definition of kspiC dispatcher
/*!
	\param hObject [in]: Handle to object (module, process og gui). Ignored when iCode == kspiDispatchMain.
	\param iCode [in]: Dispatcher opcode. Defines what functionality is required
	\param ppParams [in]: Pointer to array of parameters. Parameter count differs.
	\param pReturn [out]: Pointer to return value if applicable, otherwise ignored
*/
typedef void (KSPI_CALL *kspiCDispatcherFunc)(void* hObject, tint32 iCode, void** ppParms, void* pReturn);

//! Definition of kspiC callback dispatcher
/*!
	\param hObject [in]: Handle to object.
	\param iCode [in]: Dispatcher opcode. Defines what functionality is required
	\param ppParams [in]: Pointer to array of parameters. Parameter count differs.
	\param pReturn [out]: Pointer to return value if applicable, otherwise ignored
*/
typedef void (KSPI_CALL *kspiCCallbackDispatcherFunc)(void* hObject, tint32 iCode, void** ppParms, void* pReturn);

//! kspiC dispatcher
extern "C" KSPI_EXPORT kspiCDispatcherFunc kspiCDispatcher;

// Only include if host
#ifndef KSPI_EXPORTS
extern "C" KSPI_IMPORT kspiCCallbackDispatcherFunc kspiCCallbackDispatcher;
#endif	// KSPI_EXPORTS


//! Opcodes for kspiCDispatcher
enum DispatcherOpcodes
{
	// Module opcodes
	kspiDispatchModuleDestroy = 0,
	kspiDispatchModuleGetNrOfPlugIns,
	kspiDispatchModuleCreateProcess,
	kspiDispatchModuleGetCompanyID,
	kspiDispatchModuleGetCompanyName,

	// Plugin opcodes
	kspiDispatchPlugInDestroy,
	kspiDispatchPlugInCreateGUI,
	kspiDispatchPlugInPreInitialize,
	kspiDispatchPlugInInitialize,
	kspiDispatchPlugInDeInitialize,
	kspiDispatchPlugInStart,
	kspiDispatchPlugInStop,
	kspiDispatchPlugInIsSignalGenerator,
	kspiDispatchPlugInCanProcessInPlace,
	kspiDispatchPlugInWantsMIDI,
	kspiDispatchPlugInIsSynth,
	kspiDispatchPlugInGetLatency,
	kspiDispatchPlugInGetChannelConfigurations,
	kspiDispatchPlugInSetChannelConfiguration,
	kspiDispatchPlugInGetChannelGroups,
	kspiDispatchPlugInSetChannelGroups,
//	kspiDispatchPlugInGetOutputChannelCount,
//	kspiDispatchPlugInSetChannelCount,
	kspiDispatchPlugInSetMaxBufferSize,
	kspiDispatchPlugInSupportSampleRate,
	kspiDispatchPlugInSetSampleRate,
	kspiDispatchPlugInInputMIDI,
	kspiDispatchPlugInProcessInPlace,
	kspiDispatchPlugInProcessNonInPlace,
	kspiDispatchPlugInGetNrOfParameters,
	kspiDispatchPlugInGetParameterInfo,
	kspiDispatchPlugInGetParameter,
	kspiDispatchPlugInSetParameter,
	kspiDispatchPlugInGetChunkSize,
	kspiDispatchPlugInGetChunk,
	kspiDispatchPlugInSetChunk,
	kspiDispatchPlugInTimer,
	kspiDispatchPlugInGetProductID,
	kspiDispatchPlugInGetProductName,

	// GUI opcodes
	kspiDispatchGUIDestroy,
	kspiDispatchGUIGetSize,
	kspiDispatchGUIMakeWindow,
	kspiDispatchGUIPrepareBuffer,
	kspiDispatchGUIPaint,
	kspiDispatchGUIOnMouseDown,
	kspiDispatchGUIOnKeyDown,
	kspiDispatchGUIOnTimer,
	kspiDispatchGUIOnDraw,
	kspiDispatchGUIOnMouseLeftButtonDown,
	kspiDispatchGUIOnMouseLeftButtonUp,
	kspiDispatchGUIOnMouseMove,
	kspiDispatchGUIOnMouseRightButtonDown,
	kspiDispatchGUIOnMouseRightButtonUp,
	kspiDispatchGUIOnMouseLeftButtonDoubleClick
};

//! Opcodes for kspiCCallbackDispatcher (host)
enum CallbackDispatcherOpcodes
{
	kspiDispatchHostOnAutomationUpdate = 0,
	kspiDispatchHostGetCurrentBPM,
	kspiDispatchHostGetType,
	kspiDispatchHostActivateWindow,
	kspiDispatchHostHideWindow,
	kspiDispatchHostRedrawWindow,
	kspiDispatchHostIsWindowVisible,
	kspiDispatchHostDoesWindowHaveToolbar
};


}	// namespace kspi
}	// namespace k2s

#endif	// _KSPI_C_INCLUDED


