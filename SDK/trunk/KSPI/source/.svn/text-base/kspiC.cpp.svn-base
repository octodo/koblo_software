/*! \file kspiC.cpp
	\author Michael Olsen
	\date 08/Mar/2005
	\date 08/Mar/2005
*/

/*
	This file contains a standard kspiC dispatcher, which will transform the calls into the C++ counterparts.
	This can be used by the plug-in to support kspiC with minimal efford.
*/

// KSPI
#ifndef _KSPI_INCLUDED
#include "kspi.h"
#endif
#ifndef _KSPI_C_INCLUDED
#include "kspiC.h"
#endif

//! A wrapper for the main func (fixes a bug in msvc)
kspi::IModule* MainWrap(kspi::IHost* pHost, tuint32* piVersionNumber);

#ifdef WIN32
extern "C" __declspec(dllexport) void kspiCDispatcher(void* pObject, tint32 iCode, void** ppParms, void* pReturn)
#endif
#ifdef _Mac
extern "C" void kspiCDispatcher(void* pObject, tint32 iCode, void** ppParms, void* pReturn)
#endif
{
	switch(iCode) {
/*		// Main
		case kspi::kspiDispatchMain:
			{
//				*((kspi::IModule**)pReturn) = kspi::kspiMain((kspi::IHost*)ppParms[0], (tuint32*)ppParms[1]);
				kspi::kspiCCallbackDispatcherFunc Func = (kspi::kspiCCallbackDispatcherFunc)ppParms[0];
				kspi::IHost* pHost = (kspi::IHost*)ppParms[2];
				kspi::kspiHost* pHostWrap = new kspi::kspiHost(Func, pHost);
				*((kspi::IModule**)pReturn) = MainWrap(pHostWrap, (tuint32*)ppParms[1]);
			}
			break;*/

		// Module
		case kspi::kspiDispatchModuleDestroy:
			((kspi::IModule*)pObject)->Destroy();
			break;
		case kspi::kspiDispatchModuleGetNrOfPlugIns:
			*((tuint32*)pReturn) = ((kspi::IModule*)pObject)->GetNrOfPlugIns();
			break;
		case kspi::kspiDispatchModuleCreateProcess:
			*((kspi::IPlugIn**)pReturn) = ((kspi::IModule*)pObject)->CreateProcess((tuint32)ppParms[0]);
			break;
		case kspi::kspiDispatchModuleGetCompanyID:
			*((tuint32*)pReturn) = ((kspi::IModule*)pObject)->GetCompanyID();
			break;
		case kspi::kspiDispatchModuleGetCompanyName:
			((kspi::IModule*)pObject)->GetCompanyName((tchar*)ppParms[0]);
			break;

		// PlugIn
		case kspi::kspiDispatchPlugInDestroy:
			((kspi::IPlugIn*)pObject)->Destroy();
			break;
		case kspi::kspiDispatchPlugInCreateGUI:
			*((kspi::IGUI**)pReturn) = ((kspi::IPlugIn*)pObject)->CreateGUI((tuint32)ppParms[0]);
			break;
		case kspi::kspiDispatchPlugInPreInitialize:
			((kspi::IPlugIn*)pObject)->PreInitialize();
			break;
		case kspi::kspiDispatchPlugInInitialize:
			((kspi::IPlugIn*)pObject)->Initialize();
			break;
		case kspi::kspiDispatchPlugInDeInitialize:
			((kspi::IPlugIn*)pObject)->DeInitialize();
			break;
		case kspi::kspiDispatchPlugInStart:
			((kspi::IPlugIn*)pObject)->Start();
			break;
		case kspi::kspiDispatchPlugInStop:
			((kspi::IPlugIn*)pObject)->Stop();
			break;
		case kspi::kspiDispatchPlugInIsSignalGenerator:
			*((tint32*)pReturn) = ((kspi::IPlugIn*)pObject)->IsSignalGenerator();
			break;
		case kspi::kspiDispatchPlugInCanProcessInPlace:
			*((tint32*)pReturn) = ((kspi::IPlugIn*)pObject)->CanProcessInPlace();
			break;
		case kspi::kspiDispatchPlugInWantsMIDI:
			*((tint32*)pReturn) = ((kspi::IPlugIn*)pObject)->WantsMIDI();
			break;
		case kspi::kspiDispatchPlugInIsSynth:
			*((tint32*)pReturn) = ((kspi::IPlugIn*)pObject)->IsSynth();
			break;
		case kspi::kspiDispatchPlugInGetLatency:
			*((tint32*)pReturn) = ((kspi::IPlugIn*)pObject)->GetLatency();
			break;
		case kspi::kspiDispatchPlugInGetChannelConfigurations:
			((kspi::IPlugIn*)pObject)->GetChannelConfigurations((tuint32*)ppParms[0], (tuint32*)ppParms[1], (tuint32*)ppParms[2], (tuint32*)ppParms[3]);
			break;
		case kspi::kspiDispatchPlugInSetChannelConfiguration:
			((kspi::IPlugIn*)pObject)->SetChannelConfiguration((tuint32)ppParms[0], (tuint32)ppParms[1], (tuint32)ppParms[2]);
			break;
		case kspi::kspiDispatchPlugInGetChannelGroups:
			*((tuint32*)pReturn) = ((kspi::IPlugIn*)pObject)->GetChannelGroups();
			break;
		case kspi::kspiDispatchPlugInSetMaxBufferSize:
			((kspi::IPlugIn*)pObject)->SetMaxBufferSize((tuint32)ppParms[0]);
			break;
		case kspi::kspiDispatchPlugInSetChannelGroups:
			((kspi::IPlugIn*)pObject)->SetChannelGroups((tuint32)ppParms[0]);
			break;
		case kspi::kspiDispatchPlugInSupportSampleRate:
			*((tint32*)pReturn) = ((kspi::IPlugIn*)pObject)->SupportSampleRate((tuint32)ppParms[0]);
			break;
		case kspi::kspiDispatchPlugInSetSampleRate:
			((kspi::IPlugIn*)pObject)->SetSampleRate((tuint32)ppParms[0]);
			break;
		case kspi::kspiDispatchPlugInInputMIDI:
			((kspi::IPlugIn*)pObject)->InputMIDI((const kspi::SMIDIEvent*)ppParms[0], (tuint32)ppParms[1]);
			break;
		case kspi::kspiDispatchPlugInProcessInPlace:
			((kspi::IPlugIn*)pObject)->ProcessInPlace((tfloat**)ppParms[0], (tuint32)ppParms[1]);
			break;
		case kspi::kspiDispatchPlugInProcessNonInPlace:
			((kspi::IPlugIn*)pObject)->ProcessNonInPlace((tfloat**)ppParms[0], (const tfloat**)ppParms[1], (tuint32)ppParms[2]);
			break;
		case kspi::kspiDispatchPlugInGetNrOfParameters:
			*((tuint32*)pReturn) = ((kspi::IPlugIn*)pObject)->GetNrOfParameters();
			break;
		case kspi::kspiDispatchPlugInGetParameterInfo:
			((kspi::IPlugIn*)pObject)->GetParameterInfo((tuint32)ppParms[0], (kspi::SParameterInfo*)ppParms[1]);
			break;
		case kspi::kspiDispatchPlugInGetParameter:
			*((tint32*)pReturn) = ((kspi::IPlugIn*)pObject)->GetParameter((tuint32)ppParms[0]);
			break;
		case kspi::kspiDispatchPlugInSetParameter:
			((kspi::IPlugIn*)pObject)->SetParameter((tuint32)ppParms[0], (tint32)ppParms[1]);
			break;
		case kspi::kspiDispatchPlugInGetChunkSize:
			*((tuint32*)pReturn) = ((kspi::IPlugIn*)pObject)->GetChunkSize();
			break;
		case kspi::kspiDispatchPlugInGetChunk:
			((kspi::IPlugIn*)pObject)->GetChunk((void*)ppParms[0]);
			break;
		case kspi::kspiDispatchPlugInSetChunk:
			((kspi::IPlugIn*)pObject)->SetChunk((const void*)ppParms[0], (tuint32)ppParms[1]);
			break;
		case kspi::kspiDispatchPlugInTimer:
			((kspi::IPlugIn*)pObject)->Timer();
			break;
		case kspi::kspiDispatchPlugInGetProductID:
			*((tuint32*)pReturn) = ((kspi::IPlugIn*)pObject)->GetProductID();
			break;
		case kspi::kspiDispatchPlugInGetProductName:
			((kspi::IPlugIn*)pObject)->GetProductName((tchar*)ppParms[0]);
			break;

		// GUI
		case kspi::kspiDispatchGUIDestroy:
			((kspi::IGUI*)pObject)->Destroy();
			break;
		case kspi::kspiDispatchGUIGetSize:
			((kspi::IGUI*)pObject)->GetSize((tuint32*)ppParms[0], (tuint32*)ppParms[1]);
			break;
		case kspi::kspiDispatchGUIMakeWindow:
			((kspi::IGUI*)pObject)->MakeWindow((void*)ppParms[0]);
			break;
		case kspi::kspiDispatchGUIPrepareBuffer:
			((kspi::IGUI*)pObject)->PrepareBuffer();
			break;
		case kspi::kspiDispatchGUIPaint:
			((kspi::IGUI*)pObject)->Paint();
			break;
		case kspi::kspiDispatchGUIOnMouseDown:
			((kspi::IGUI*)pObject)->OnMouseDown((tuint32)ppParms[0], (tuint32)ppParms[1]);
			break;
		case kspi::kspiDispatchGUIOnKeyDown:
			((kspi::IGUI*)pObject)->OnKeyDown((tuint32)ppParms[0]);
			break;
		case kspi::kspiDispatchGUIOnTimer:
			((kspi::IGUI*)pObject)->OnTimer();
			break;
		case kspi::kspiDispatchGUIOnDraw:
			((kspi::IGUI*)pObject)->OnDraw((tuchar*)ppParms[0], (tuint32)ppParms[1], (tint32)ppParms[2], (tint32)ppParms[3], (tint32)ppParms[4], (tint32)ppParms[5]);
			break;
		case kspi::kspiDispatchGUIOnMouseLeftButtonDown:
			((kspi::IGUI*)pObject)->OnMouseLeftButtonDown((tuint32)ppParms[0], (tuint32)ppParms[1]);
			break;
		case kspi::kspiDispatchGUIOnMouseLeftButtonUp:
			((kspi::IGUI*)pObject)->OnMouseLeftButtonUp((tuint32)ppParms[0], (tuint32)ppParms[1]);
			break;
		case kspi::kspiDispatchGUIOnMouseMove:
			((kspi::IGUI*)pObject)->OnMouseMove((tuint32)ppParms[0], (tuint32)ppParms[1]);
			break;
		case kspi::kspiDispatchGUIOnMouseRightButtonDown:
			((kspi::IGUI*)pObject)->OnMouseRightButtonDown((tuint32)ppParms[0], (tuint32)ppParms[1]);
			break;
		case kspi::kspiDispatchGUIOnMouseRightButtonUp:
			((kspi::IGUI*)pObject)->OnMouseRightButtonUp((tuint32)ppParms[0], (tuint32)ppParms[1]);
			break;
		case kspi::kspiDispatchGUIOnMouseLeftButtonDoubleClick:
			((kspi::IGUI*)pObject)->OnMouseLeftButtonDoubleClick((tuint32)ppParms[0], (tuint32)ppParms[1]);
			break;
	}
}

void kspiDispatcherWrap(void* hObject, tint32 iCode, void** ppParms, void* pReturn)
{
	kspiCDispatcher(hObject, iCode, ppParms, pReturn);
}

#define PARM1(a) \
	void* ppParms[1]; \
	ppParms[0] = (void*)a

#define PARMS2(a, b) \
	void* ppParms[2]; \
	ppParms[0] = (void*)a; \
	ppParms[1] = (void*)b

#define PARMS3(a, b, c) \
	void* ppParms[3]; \
	ppParms[0] = (void*)a; \
	ppParms[1] = (void*)b; \
	ppParms[2] = (void*)c

#define PARMS4(a, b, c, d) \
	void* ppParms[4]; \
	ppParms[0] = (void*)a; \
	ppParms[1] = (void*)b; \
	ppParms[2] = (void*)c; \
	ppParms[3] = (void*)d

#define PARMS5(a, b, c, d, e) \
	void* ppParms[5]; \
	ppParms[0] = (void*)a; \
	ppParms[1] = (void*)b; \
	ppParms[2] = (void*)c; \
	ppParms[3] = (void*)d; \
	ppParms[4] = (void*)e


kspi::IHost* kspi::IHost::Create(kspiCCallbackDispatcherFunc pCallback, void* hHost)
{
	return dynamic_cast<IHost*>(new CHost((void*)pCallback, hHost));
}

void kspi::CHost::Destroy()
{
}

kspi::CHost::CHost(void* pCallback, void* hHost)
	: mpCallback((kspiCCallbackDispatcherFunc)pCallback), mhHost(hHost)
{
}

kspi::CHost::~CHost()
{
}

void kspi::CHost::OnAutomationUpdate(IPlugIn* pCaller, tint32 iParameterIndex, tint32 iValue)
{
	PARMS3(pCaller, iParameterIndex, iValue);

	mpCallback(mhHost, kspiDispatchHostOnAutomationUpdate, ppParms, NULL);
}

tfloat32 kspi::CHost::GetCurrentBPM()
{
	float fRet;
	mpCallback(mhHost, kspiDispatchHostGetCurrentBPM, NULL, &fRet);
	return fRet;
}

kspi::IHost::EType kspi::CHost::GetType()
{
	EType Type;
	mpCallback(mhHost, kspi::kspiDispatchHostGetType, NULL, &Type);
	return Type;
}

void kspi::CHost::ActivateWindow(tint32 iIndex)
{
	PARM1(iIndex);

	mpCallback(mhHost, kspiDispatchHostActivateWindow, ppParms, NULL);
}

void kspi::CHost::HideWindow(tint32 iIndex)
{
	PARM1(iIndex);

	mpCallback(mhHost, kspiDispatchHostHideWindow, ppParms, NULL);
}

tint32 kspi::CHost::IsWindowVisible(tint32 iIndex)
{
	PARM1(iIndex);

	tint32 iRet;
	mpCallback(mhHost, kspiDispatchHostIsWindowVisible, ppParms, &iRet);
	return iRet;
}

void kspi::CHost::RedrawWindow(kspi::IGUI* pCaller, tint32 iX, tint32 iY, tint32 iCX, tint32 iCY)
{
	PARMS5(pCaller, iX, iY, iCX, iCY);

	mpCallback(mhHost, kspiDispatchHostRedrawWindow, ppParms, NULL);
}

tint32 kspi::CHost::DoesWindowHaveToolbar(tint32 iIndex)
{
	PARM1(iIndex);

	tint32 iRet;
	mpCallback(mhHost, kspiDispatchHostDoesWindowHaveToolbar, ppParms, &iRet);
	return iRet;
}



