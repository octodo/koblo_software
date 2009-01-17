/*! \file kspiCHost.cpp
	\author Michael Olsen
	\date 09/Mar/2005
	\date 27/Jun/2007 (major revision)
*/

// KSPI
#define KSPI_RAW
#include "kspi.h"
#include "kspiC.h"
#include "kspiCHost.h"

using namespace k2s;

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

#define PARMS6(a, b, c, d, e, f) \
	void* ppParms[6]; \
	ppParms[0] = (void*)a; \
	ppParms[1] = (void*)b; \
	ppParms[2] = (void*)c; \
	ppParms[3] = (void*)d; \
	ppParms[4] = (void*)e; \
	ppParms[5] = (void*)f

using namespace kspi;


// Only include if host
#ifndef KSPI_EXPORTS
#ifdef WIN32
extern "C" KSPI_IMPORT void kspiCCallbackDispatcher(void* pObject, tint32 iCode, void** ppParms, void* pReturn)
#endif
#ifdef _Mac
extern "C" void kspiCCallbackDispatcher(void* pObject, tint32 iCode, void** ppParms, void* pReturn)
#endif
{
	switch(iCode) {
		case kspiDispatchHostOnAutomationUpdate:
			((kspi::IHost*)pObject)->OnAutomationUpdate((IPlugIn*)ppParms[0], (tint32)ppParms[1], (tint32)ppParms[2]);
			break;

		case kspiDispatchHostGetCurrentBPM:
			*((tfloat32*)pReturn) = ((kspi::IHost*)pObject)->GetCurrentBPM();
			break;

		case kspiDispatchHostGetType:
			*((kspi::IHost::EType*)pReturn) = ((kspi::IHost*)pObject)->GetType();
			break;

		case kspiDispatchHostActivateWindow:
			((kspi::IHost*)pObject)->ActivateWindow((tint32)ppParms[0]);
			break;

		case kspiDispatchHostHideWindow:
			((kspi::IHost*)pObject)->HideWindow((tint32)ppParms[0]);
			break;

		case kspiDispatchHostRedrawWindow:
			((kspi::IHost*)pObject)->RedrawWindow((kspi::IGUI*)ppParms[0], (tint32)ppParms[1], (tint32)ppParms[2], (tint32)ppParms[3], (tint32)ppParms[4]);
			break;

		case kspiDispatchHostIsWindowVisible:
			*((tint32*)pReturn) = ((kspi::IHost*)pObject)->IsWindowVisible((tint32)ppParms[0]);
			break;

		case kspiDispatchHostDoesWindowHaveToolbar:
			*((tint32*)pReturn) = ((kspi::IHost*)pObject)->DoesWindowHaveToolbar((tint32)ppParms[0]);
			break;
		}
}
#endif	// KSPI_EXPORTS









/*
	GUI
*/

kspiGUI::kspiGUI(kspi::kspiCDispatcherFunc DispatcherFunc, void* pGUI)
	: mDispatcherFunc(DispatcherFunc), mpGUI(pGUI)
{
}

void kspiGUI::Destroy()
{
	mDispatcherFunc(mpGUI, kspi::kspiDispatchGUIDestroy, NULL, NULL);
}

void kspiGUI::GetSize(tuint32* piSizeX, tuint32* piSizeY)
{
	PARMS2(piSizeX, piSizeY);

	mDispatcherFunc(mpGUI, kspi::kspiDispatchGUIGetSize, ppParms, NULL);
}

void kspiGUI::MakeWindow(void* p)
{
	PARM1(p);

	mDispatcherFunc(mpGUI, kspi::kspiDispatchGUIMakeWindow, ppParms, NULL);
}

void kspiGUI::PrepareBuffer()
{
	mDispatcherFunc(mpGUI, kspi::kspiDispatchGUIPrepareBuffer, NULL, NULL);
}

void kspiGUI::Paint()
{
	mDispatcherFunc(mpGUI, kspi::kspiDispatchGUIPaint, NULL, NULL);
}

void kspiGUI::OnMouseDown(tuint32 iPosX, tuint32 iPosY)
{
	PARMS2(iPosX, iPosY);

	mDispatcherFunc(mpGUI, kspi::kspiDispatchGUIOnMouseDown, ppParms, NULL);
}

void kspiGUI::OnKeyDown(tuint32 iKeyCode)
{
	PARM1(iKeyCode);

	mDispatcherFunc(mpGUI, kspi::kspiDispatchGUIOnKeyDown, ppParms, NULL);
}

void kspiGUI::OnTimer()
{
	mDispatcherFunc(mpGUI, kspi::kspiDispatchGUIOnTimer, NULL, NULL);
}

void kspiGUI::OnDraw(tuchar* puchBuffer, tuint32 uiInterleave, tint32 iUpdateX, tint32 iUpdateY, tint32 iUpdateCX, tint32 iUpdateCY)
{
	PARMS6(puchBuffer, uiInterleave, iUpdateX, iUpdateY, iUpdateCX, iUpdateCY);

	mDispatcherFunc(mpGUI, kspi::kspiDispatchGUIOnDraw, ppParms, NULL);
}

void kspiGUI::OnMouseLeftButtonDown(tint32 iPosX, tint32 iPosY)
{
	PARMS2(iPosX, iPosY);

	mDispatcherFunc(mpGUI, kspi::kspiDispatchGUIOnMouseLeftButtonDown, ppParms, NULL);
}

void kspiGUI::OnMouseLeftButtonUp(tint32 iPosX, tint32 iPosY)
{
	PARMS2(iPosX, iPosY);

	mDispatcherFunc(mpGUI, kspi::kspiDispatchGUIOnMouseLeftButtonUp, ppParms, NULL);
}

void kspiGUI::OnMouseMove(tint32 iPosX, tint32 iPosY)
{
	PARMS2(iPosX, iPosY);

	mDispatcherFunc(mpGUI, kspi::kspiDispatchGUIOnMouseMove, ppParms, NULL);
}

void kspiGUI::OnMouseRightButtonDown(tint32 iPosX, tint32 iPosY)
{
	PARMS2(iPosX, iPosY);

	mDispatcherFunc(mpGUI, kspi::kspiDispatchGUIOnMouseRightButtonDown, ppParms, NULL);
}

void kspiGUI::OnMouseRightButtonUp(tint32 iPosX, tint32 iPosY)
{
	PARMS2(iPosX, iPosY);

	mDispatcherFunc(mpGUI, kspi::kspiDispatchGUIOnMouseRightButtonUp, ppParms, NULL);
}

void kspiGUI::OnMouseLeftButtonDoubleClick(tint32 iPosX, tint32 iPosY)
{
	PARMS2(iPosX, iPosY);

	mDispatcherFunc(mpGUI, kspi::kspiDispatchGUIOnMouseLeftButtonDoubleClick, ppParms, NULL);
}


/*
	PlugIn
*/

kspiPlugIn::kspiPlugIn(kspi::kspiCDispatcherFunc DispatcherFunc, void* pPlugIn)
	: mDispatcherFunc(DispatcherFunc), mpPlugIn(pPlugIn)
{
}

IPlugIn* kspiPlugIn::Create(kspi::kspiCDispatcherFunc DispatcherFunc, void* pPlugIn)
{
	return dynamic_cast<IPlugIn*>(new kspiPlugIn(DispatcherFunc, pPlugIn));
}

void KSPI_CALL kspiPlugIn::Destroy()
{
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInDestroy, NULL, NULL);
}

kspi::IGUI* KSPI_CALL kspiPlugIn:: CreateGUI(tint32 iIndex)
{
	PARM1(iIndex);

	kspi::IPlugIn* pGUI;

	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInCreateGUI, ppParms, (void*)&pGUI);

	kspiGUI* pkspiGUI = new kspiGUI(mDispatcherFunc, (void*)pGUI);

	return dynamic_cast<kspi::IGUI*>(pkspiGUI);
}

void KSPI_CALL kspiPlugIn:: PreInitialize()
{
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInPreInitialize, NULL, NULL);
}

void KSPI_CALL kspiPlugIn:: Initialize()
{
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInInitialize, NULL, NULL);
}

void KSPI_CALL kspiPlugIn:: DeInitialize()
{
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInDeInitialize, NULL, NULL);
}

void KSPI_CALL kspiPlugIn:: Start()
{
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInStart, NULL, NULL);
}

void KSPI_CALL kspiPlugIn:: Stop()
{
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInStop, NULL, NULL);
}

tint32 KSPI_CALL kspiPlugIn:: IsSignalGenerator()
{
	tint32 bRet;
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInIsSignalGenerator, NULL, (void*)&bRet);
	return bRet;
}

tint32 KSPI_CALL kspiPlugIn:: CanProcessInPlace()
{
	tint32 bRet;
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInCanProcessInPlace, NULL, (void*)&bRet);
	return bRet;
}

tint32 KSPI_CALL kspiPlugIn:: WantsMIDI()
{
	tint32 bRet;
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInWantsMIDI, NULL, (void*)&bRet);
	return bRet;
}

tint32 KSPI_CALL kspiPlugIn:: IsSynth()
{
	tint32 bRet;
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInIsSynth, NULL, (void*)&bRet);
	return bRet;
}

tint32 KSPI_CALL kspiPlugIn::GetLatency()
{
	tint32 bRet;
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInGetLatency, NULL, (void*)&bRet);
	return bRet;
}

void kspiPlugIn::GetChannelConfigurations(tuint32* puiNrOfConfigurations, tuint32* puiChannelsIn, tuint32* puiChannelsOut, tuint32* puiSideChainChannels)
{
	PARMS4(puiNrOfConfigurations, puiChannelsIn, puiChannelsOut, puiSideChainChannels);
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInGetChannelConfigurations, ppParms, NULL);
}

void kspiPlugIn::SetChannelConfiguration(tuint32 uiChannelsIn, tuint32 uiChannelsOut, tuint32 uiSideChainChannels)
{
	PARMS3(uiChannelsIn, uiChannelsOut, uiSideChainChannels);
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInSetChannelConfiguration, ppParms, NULL);
}

tuint32 kspiPlugIn::GetChannelGroups()
{
	tuint32 uiRet;
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInGetChannelGroups, NULL, (void*)&uiRet);
	return uiRet;
}

void kspiPlugIn::SetChannelGroups(tuint32 uiGroups)
{
	PARM1(uiGroups);
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInSetChannelGroups, ppParms, NULL);
}

/*tuint32 KSPI_CALL kspiPlugIn:: GetOutputChannelCount()
{
	tuint32 uiRet;
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInGetOutputChannelCount, NULL, (void*)&uiRet);
	return uiRet;
}

void KSPI_CALL kspiPlugIn::SetChannelCount(tuint32 iChannelsIn, tuint32 iChannelsOut, tuint32 iSideChainChannels)
{
	PARMS3(iChannelsIn, iChannelsOut, iSideChainChannels);

	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInSetChannelCount, ppParms, NULL);
}*/

void KSPI_CALL kspiPlugIn:: SetMaxBufferSize(tuint32 iBufferSize)
{
	PARM1(iBufferSize);

	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInSetMaxBufferSize, ppParms, NULL);
}

tint32 KSPI_CALL kspiPlugIn:: SupportSampleRate(tuint32 iSampleRate)
{
	PARM1(iSampleRate);

	tint32 bRet;
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInSupportSampleRate, ppParms, (void*)&bRet);
	return bRet;
}

void KSPI_CALL kspiPlugIn:: SetSampleRate(tuint32 iSampleRate)
{
	PARM1(iSampleRate);

	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInSetSampleRate, ppParms, NULL);
}

void KSPI_CALL kspiPlugIn:: InputMIDI(const kspi::SMIDIEvent* pEvents, tuint32 iNrOfEvents)
{
	PARMS2(pEvents, iNrOfEvents);

	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInInputMIDI, ppParms, NULL);
}

void KSPI_CALL kspiPlugIn:: ProcessInPlace(tfloat32** ppfSamples, tuint32 iNrOfSamples)
{
	PARMS2(ppfSamples, iNrOfSamples);

	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInProcessInPlace, ppParms, NULL);
}

void KSPI_CALL kspiPlugIn:: ProcessNonInPlace(tfloat32** ppfSamplesOut, const tfloat32** ppfSamplesIn, tuint32 iNrOfSamples)
{
	PARMS3(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);

	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInProcessNonInPlace, ppParms, NULL);
}

tuint32 KSPI_CALL kspiPlugIn:: GetNrOfParameters()
{
	tuint32 uiRet;
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInGetNrOfParameters, NULL, (void*)&uiRet);
	return uiRet;
}

void KSPI_CALL kspiPlugIn:: GetParameterInfo(tuint32 iIndex, kspi::SParameterInfo* pInfo)
{
	PARMS2(iIndex, pInfo);

	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInGetParameterInfo, ppParms, NULL);
}

tint32 KSPI_CALL kspiPlugIn:: GetParameter(tuint32 iIndex)
{
	PARM1(iIndex);

	tuint32 uiRet;
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInGetParameter, ppParms, (void*)&uiRet);
	return uiRet;
}

void KSPI_CALL kspiPlugIn:: SetParameter(tuint32 iIndex, tint32 iValue)
{
	PARMS2(iIndex, iValue);

	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInSetParameter, ppParms, NULL);
}

tuint32 KSPI_CALL kspiPlugIn:: GetChunkSize()
{
	tuint32 uiRet;
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInGetChunkSize, NULL, (void*)&uiRet);
	return uiRet;
}

void KSPI_CALL kspiPlugIn:: GetChunk(void* p)
{
	PARM1(p);

	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInGetChunk, ppParms, NULL);
}

void KSPI_CALL kspiPlugIn:: SetChunk(const void* p, tuint32 iChunkSize)
{
	PARMS2(p, iChunkSize);

	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInSetChunk, ppParms, NULL);
}

void KSPI_CALL kspiPlugIn:: Timer()
{
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInTimer, NULL, NULL);
}

tint32 KSPI_CALL kspiPlugIn::GetProductID()
{
	tint32 bRet;
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInGetProductID, NULL, (void*)&bRet);
	return bRet;
}

void KSPI_CALL kspiPlugIn::GetProductName(tchar* pszName)
{
	PARM1(pszName);
	mDispatcherFunc(mpPlugIn, kspi::kspiDispatchPlugInGetProductName, ppParms, NULL);
}


/*
	Module
*/


void kspiModule::Destroy()
{
	mDispatcherFunc(mhModule, kspi::kspiDispatchModuleDestroy, NULL, NULL);
}

tuint32 kspiModule::GetNrOfPlugIns()
{
	tuint32 uiRet;
	mDispatcherFunc(mhModule, kspi::kspiDispatchModuleGetNrOfPlugIns, NULL, (void*)&uiRet);
	return uiRet;
}

tint32 kspiModule::GetCompanyID()
{
	tuint32 uiRet;
	mDispatcherFunc(mhModule, kspi::kspiDispatchModuleGetCompanyID, NULL, (void*)&uiRet);
	return uiRet;
}

void kspiModule::GetCompanyName(tchar* pszName)
{
	PARM1(pszName);
	mDispatcherFunc(mhModule, kspi::kspiDispatchModuleGetCompanyName, ppParms, NULL);
}

kspi::IPlugIn* kspiModule::CreateProcess(tuint32 iIndex)
{
	PARM1(iIndex);

	kspi::IPlugIn* pPlugIn;

	mDispatcherFunc(mhModule, kspi::kspiDispatchModuleCreateProcess, ppParms, (void*)&pPlugIn);

	kspiPlugIn* pkspiPlugIn = new kspiPlugIn(mDispatcherFunc, (void*)pPlugIn);

	return dynamic_cast<kspi::IPlugIn*>(pkspiPlugIn);
}

kspi::kspiModule::kspiModule(kspi::kspiCDispatcherFunc DispatcherFunc, /*kspi::IHost* pHost,*/ void* hModule, tuint32* pVersionNumber, bool bOSXPointerConvert)
{
/*	kspiModule* pkspiModule = new kspiModule();

	kspi::IModule* pModule;

	kspi::kspiCCallbackDispatcherFunc kspiCallback = DispatcherFunc;
	if (bOSXPointerConvert) {
		kspiCallback = (kspi::kspiCCallbackDispatcherFunc)(*((void**)kspiCallback));
	}*/

	mDispatcherFunc = DispatcherFunc;

/*	PARMS3(kspiCallback, pVersionNumber, pHost);

	kspi::kspiCDispatcherFunc kspiDispatch = DispatcherFunc;
	if (bOSXPointerConvert) {
//		pkspiModule->mpDispatchPointer = kspiDispatch;
//		kspiDispatch = (kspi::kspiCDispatcherFunc)(&(pkspiModule->mpDispatchPointer));
	}

	kspiDispatch(kspi::kspiDispatchMain, (kspi::kspiVoid*)&pModule, NULL, ppParms);*/

	mhModule = hModule;

/*	pkspiModule->Init(DispatcherFunc, (void*)pModule, pHost);*/
}

kspi::IModule* kspiModule::Create(kspi::kspiCDispatcherFunc DispatcherFunc, /*kspi::IHost* pHost,*/ void* hModule, tuint32* pVersionNumber, bool bOSXPointerConvert)
{
	return dynamic_cast<kspi::IModule*>(new kspiModule(DispatcherFunc, hModule, pVersionNumber, bOSXPointerConvert));
}



