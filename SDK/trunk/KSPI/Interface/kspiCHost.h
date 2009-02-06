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
// along with the Koblo SDK. If not, see <http://www.gnu.org/licenses/>.


/*
	This file contains wrapper classes (and a single function) a host can use to support kspiC
	It converts standard kspi calls into the C compatible variant
*/

#ifndef _KSPI_CHOST_INCLUDED
#define _KSPI_CHOST_INCLUDED

namespace k2s {
namespace kspi {

//! Wrapper class for kspi::IGUI
class kspiGUI : public virtual k2s::kspi::IGUI
{
friend class kspiPlugIn;

public:
	virtual void KSPI_CALL Destroy();
	virtual void KSPI_CALL GetSize(k2s::tuint32* piSizeX, k2s::tuint32* piSizeY);
	virtual void KSPI_CALL MakeWindow(void* p);
	virtual void KSPI_CALL PrepareBuffer();
	virtual void KSPI_CALL Paint();
	virtual void KSPI_CALL OnMouseDown(k2s::tuint32 iPosX, k2s::tuint32 iPosY);
	virtual void KSPI_CALL OnKeyDown(k2s::tuint32 iKeyCode);
	virtual void KSPI_CALL OnTimer();
	virtual void KSPI_CALL OnDraw(tuchar* puchBuffer, tuint32 uiInterleave, tint32 iUpdateX, tint32 iUpdateY, tint32 iUpdateCX, tint32 iUpdateCY);
	virtual void KSPI_CALL OnMouseLeftButtonDown(tint32 iPosX, tint32 iPosY);
	virtual void KSPI_CALL OnMouseLeftButtonUp(tint32 iPosX, tint32 iPosY);
	virtual void KSPI_CALL OnMouseMove(tint32 iPosX, tint32 iPosY);
	virtual void KSPI_CALL OnMouseRightButtonDown(tint32 iPosX, tint32 iPosY);
	virtual void KSPI_CALL OnMouseRightButtonUp(tint32 iPosX, tint32 iPosY);
	virtual void KSPI_CALL OnMouseLeftButtonDoubleClick(tint32 iPosX, tint32 iPosY);

protected:
	kspiGUI(k2s::kspi::kspiCDispatcherFunc DispatcherFunc, void* pGUI);

	k2s::kspi::kspiCDispatcherFunc mDispatcherFunc;
	void* mpGUI;
};

//! Wrapper class for kspi::IPlugIn
class kspiPlugIn : public virtual k2s::kspi::IPlugIn
{
friend class kspiModule;

public:
	virtual void KSPI_CALL Destroy();
	virtual k2s::kspi::IGUI* KSPI_CALL CreateGUI(tint32 iIndex);
	virtual void KSPI_CALL PreInitialize();
	virtual void KSPI_CALL Initialize();
	virtual void KSPI_CALL DeInitialize();
	virtual void KSPI_CALL Start();
	virtual void KSPI_CALL Stop();
	virtual k2s::tint32 KSPI_CALL IsSignalGenerator();
	virtual k2s::tint32 KSPI_CALL CanProcessInPlace();
	virtual k2s::tint32 KSPI_CALL WantsMIDI();
	virtual k2s::tint32 KSPI_CALL IsSynth();
	virtual k2s::tint32 KSPI_CALL GetLatency();
	virtual void KSPI_CALL GetChannelConfigurations(tuint32* puiNrOfConfigurations, tuint32* puiChannelsIn, tuint32* puiChannelsOut, tuint32* puiSideChainChannels);
	virtual void KSPI_CALL SetChannelConfiguration(tuint32 uiChannelsIn, tuint32 uiChannelsOut, tuint32 uiSideChainChannels);
	virtual tuint32 KSPI_CALL GetChannelGroups();
	virtual void KSPI_CALL SetChannelGroups(tuint32 uiGroups);
//	virtual tuint32 KSPI_CALL GetInputChannelCount();
//	virtual k2s::tuint32 KSPI_CALL GetOutputChannelCount();
//	virtual void KSPI_CALL SetChannelCount(k2s::tuint32 iChannelsIn, k2s::tuint32 iChannelsOut, k2s::tuint32 iSideChainChannels);
	virtual void KSPI_CALL SetMaxBufferSize(k2s::tuint32 iBufferSize);
	virtual k2s::tint32 KSPI_CALL SupportSampleRate(k2s::tuint32 iSampleRate);
	virtual void KSPI_CALL SetSampleRate(k2s::tuint32 iSampleRate);
	virtual void KSPI_CALL InputMIDI(const k2s::kspi::SMIDIEvent* pEvents, k2s::tuint32 iNrOfEvents);
	virtual void KSPI_CALL ProcessInPlace(k2s::tfloat32** ppfSamples, k2s::tuint32 iNrOfSamples);
	virtual void KSPI_CALL ProcessNonInPlace(k2s::tfloat32** ppfSamplesOut, const k2s::tfloat32** ppfSamplesIn, k2s::tuint32 iNrOfSamples);
	virtual k2s::tuint32 KSPI_CALL GetNrOfParameters();
	virtual void KSPI_CALL GetParameterInfo(k2s::tuint32 iIndex, k2s::kspi::SParameterInfo* pInfo);
	virtual k2s::tint32 KSPI_CALL GetParameter(k2s::tuint32 iIndex);
	virtual void KSPI_CALL SetParameter(k2s::tuint32 iIndex, k2s::tint32 iValue);
	virtual k2s::tuint32 KSPI_CALL GetChunkSize();
	virtual void KSPI_CALL GetChunk(void* p);
	virtual void KSPI_CALL SetChunk(const void* p, k2s::tuint32 iChunkSize);
	virtual void KSPI_CALL Timer();
	virtual k2s::tint32 KSPI_CALL GetProductID();
	virtual void KSPI_CALL GetProductName(tchar* pszName);

	static IPlugIn* Create(k2s::kspi::kspiCDispatcherFunc DispatcherFunc, void* hPlugIn);

protected:
	kspiPlugIn(k2s::kspi::kspiCDispatcherFunc DispatcherFunc, void* hPlugIn);

	k2s::kspi::kspiCDispatcherFunc mDispatcherFunc;
	void* mpPlugIn;
};



//! Wrapper class for kspi::IModule
class kspiModule : public virtual k2s::kspi::IModule
{
public:
	//! Main function to get kspi module through C interface. Compared to the original main function there's a single differece, in that it take a parameter for the memory location of the kspiC dispatcher function
	static k2s::kspi::IModule* KSPI_CALL Create(k2s::kspi::kspiCDispatcherFunc DispatcherFunc, /*k2s::kspi::IHost* pHost,*/ void* hModule, k2s::tuint32* pVersionNumber, bool bOSXPointerConvert);

	virtual void KSPI_CALL Destroy();
	virtual k2s::tuint32 KSPI_CALL GetNrOfPlugIns();
	virtual k2s::kspi::IPlugIn* KSPI_CALL CreateProcess(k2s::tuint32 iIndex);
	virtual k2s::tint32 KSPI_CALL GetCompanyID();
	virtual void KSPI_CALL GetCompanyName(tchar* pszName);

protected:
	kspiModule(kspi::kspiCDispatcherFunc DispatcherFunc, /*kspi::IHost* pHost,*/ void* hModule, tuint32* pVersionNumber, bool bOSXPointerConvert);

	k2s::kspi::kspiCDispatcherFunc mDispatcherFunc;
	void* mpDispatchPointer;
	void* mhModule;

//	k2s::kspi::IHost* mpHost;
};

}	// namespace kspi
}	// namespace k2s

#endif	// _KSPI_CHOST_INCLUDED





