

#ifdef WIN32

// Base Utilities
#include "bu.h"

// Graphical Engine
#include "ge.h"

// KSPI
#include "kspi.h"
#include "kspiC.h"

// Base Classes
#include "BaseControlIDs.h"
#include "FontSizes.h"
#include "IBaseDezipperCallback.h"
#include "CBaseMIDICCEvent.h"
#include "CBaseDezipper.h"
#include "CBaseDezipper2.h"
#include "CBaseDSPEngine.h"
#include "CBaseModule.h"
#include "CBasePlugIn.h"
#include "CBasePane.h"
#include "CBasePaneStd.h"
#include "CBaseGUI.h"
#include "CBaseTopPane.h"
#endif	// WIN32


CBasePlugIn::CBasePlugIn(CBaseModule* pModule, tuint32 iMaxSubBufferSize, CBaseDSPEngine* pDSPEngine, tuint32 uiProcessIndex, std::string sCompanyName, std::string sProductName)
	: miMaxSubBufferSize(iMaxSubBufferSize), mpModule(pModule), muiProcessIndex(uiProcessIndex),
	msCompanyName(sCompanyName), msProductName(sProductName),
	/*mpGUI(NULL), */mpParmMan(NULL), mbUpdateEngineSettings(false),
	mpDSPEngine(pDSPEngine), mpDezipper(NULL),
	miStreamTime(-1),
	miChannelsIn(-1), miChannelsOut(-1), miChannelsSideChainIn(-1),
	miSampleRate(0), mbListenForCC(false),
	miMIDIChannel(-1)
{
	tint iChannel = 0;
	for (; iChannel < 2; iChannel++) {
		mppfOut[iChannel] = NULL;
	}
	for (iChannel = 0; iChannel < 32; iChannel++) {
		mppfIn[iChannel] = NULL;
	}

	mpDezipper = new CBaseDezipper();

	MIDICCReset();

	mpPresetBackground = NULL;
}

CBasePlugIn::~CBasePlugIn()
{
	if (mpPresetBackground) {
		mpPresetBackground->Destroy();
	}

	if (mpDezipper) {
		mpDezipper->Destroy();
	}

	if (mpDSPEngine) {
		mpDSPEngine->Destroy();
	}

	if (mpParmMan) {
		mpParmMan->Destroy();
	}

	tint iChannel = 0;
	for (; iChannel < 2; iChannel++) {
		if (mppfOut[iChannel]) {
			delete[] mppfOut[iChannel];
		}
	}
	for (iChannel = 0; iChannel < 32; iChannel++) {
		if (mppfIn[iChannel]) {
			delete[] mppfIn[iChannel];
		}
	}
}

void CBasePlugIn::GUIDestroyed(CBaseGUI* pGUI)
{
//	mpGUI = NULL;
	mGUIs.remove(pGUI);
}

void CBasePlugIn::PreInitialize()
{
	CAutoLock Lock(mMutex);

	// Create parameter manager
	mpParmMan = de::IParameterManager::Create();
	mpParmMan->SetCallback(dynamic_cast<de::IParameterManagerCallback*>(this));

	// Initialize parameters
	AddParameters();
	// Create automation map
	MakeAutomationMap();
}

void CBasePlugIn::SetChannelConfiguration(tuint32 uiChannelsIn, tuint32 uiChannelsOut, tuint32 uiSideChainChannels)
{
	miChannelsIn = uiChannelsIn;
	miChannelsOut = uiChannelsOut;
	miChannelsSideChainIn = uiSideChainChannels;
}

tuint32 CBasePlugIn::GetChannelGroups()
{
	return 1;
}

void CBasePlugIn::SetChannelGroups(tuint32 uiGroups)
{
	// The channel groups should be stored here
}

//void CBasePlugIn::SetChannelCount(tuint32 iChannelsIn, tuint32 iChannelsOut, tuint32 iSideChainChannels)
//{
//	miChannelsIn = (tint32)iChannelsIn;
//	miChannelsOut = (tint32)iChannelsOut;
//	miChannelsSideChainIn = (tint32)iSideChainChannels;
//}

tint32 CBasePlugIn::GetProductID()
{
	return -1;
}

void CBasePlugIn::GetProductName(tchar* pszName)
{
	strcpy(pszName, "n/a");
}

void CBasePlugIn::Initialize()
{
}

void CBasePlugIn::DeInitialize()
{
}

void CBasePlugIn::Start()
{
	mpDSPEngine->Start();

	miStreamTime = 0;
}

void CBasePlugIn::Stop()
{
	mpDSPEngine->Stop();

	mpDezipper->Reset();

	miStreamTime = -1;
}

tint32 CBasePlugIn::IsSignalGenerator()
{
	return 0;
}

tint32 CBasePlugIn::CanProcessInPlace()
{
	return 1;
}

tint32 CBasePlugIn::WantsMIDI()
{
	return 0;
}

tint32 CBasePlugIn::IsSynth()
{
	return 0;
}

tint32 CBasePlugIn::GetLatency()
{
	return 0;
}

void CBasePlugIn::SetMaxBufferSize(tuint32 iBufferSize)
{
	CAutoLock Lock(mMutex);

	tint iChannel = 0;
	for (; iChannel < 2; iChannel++) {
		if (mppfOut[iChannel]) {
			delete[] mppfOut[iChannel];
			mppfOut[iChannel] = NULL;
		}
	}
	for (iChannel = 0; iChannel < 32; iChannel++) {
		if (mppfIn[iChannel]) {
			delete[] mppfIn[iChannel];
			mppfIn[iChannel] = NULL;
		}
	}

	if (iBufferSize > miMaxSubBufferSize) {
		iBufferSize = miMaxSubBufferSize;

		for (iChannel = 0; iChannel < 2; iChannel++) {
			mppfOut[iChannel] = new tfloat[iBufferSize];
		}
		for (iChannel = 0; iChannel < 32; iChannel++) {
			mppfIn[iChannel] = new tfloat[iBufferSize];
		}
	}

	miMaxBufferSize = iBufferSize;

	mpDSPEngine->SetMaxBufferSize(miMaxBufferSize);
}

tint32 CBasePlugIn::SupportSampleRate(tuint32 iSampleRate)
{
	return (mpDSPEngine->SupportSampleRate(iSampleRate) == true) ? 1 : 0;
}

void CBasePlugIn::SetSampleRate(tuint32 uiSampleRate)
{
	CAutoLock Lock(mMutex);

	miSampleRate = uiSampleRate;

	mpDSPEngine->SetSampleRate(miSampleRate);

	// It's now safe to update the engine
	mbUpdateEngineSettings = true;

	// Make sure engine is notified about all parameter settings
	mpParmMan->ParameterUpdateAll();
}

void CBasePlugIn::Reset()
{
	tint32 iParameters = mpParmMan->GetParameterCount(0);
	for (tint i = 0; i < iParameters; i++) {
		tint32 iDefaultValue = mpParmMan->Get(i)->GetDefaultValue();
		mpParmMan->Set(true, iDefaultValue, i);
	}
}

void CBasePlugIn::ProcessInPlace(tfloat** ppfSamples, tuint32 iNrOfSamples)
{
	// We just call the non-in-place version, since we know input is read before output is written
	ProcessNonInPlace(ppfSamples, (const tfloat**)ppfSamples, iNrOfSamples);
}

void CBasePlugIn::ProcessNonInPlace(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	CAutoLock Lock(mMutex);

	if (iNrOfSamples <= miMaxSubBufferSize) {
		// Not too many samples, we handle them all in one go
		DoProcess(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
	else {
		// Too many samples, we have to split them up into several calls

		// How many samples left to process?
		tuint32 uiSamplesLeft = iNrOfSamples;
		// How many samples has been processed
		tuint32 uiSamplesProcessed = 0;
		while (uiSamplesLeft) {
			// How many samples to process this iteration?
			tuint32 uiToProcess = uiSamplesLeft;
			if (uiToProcess > miMaxSubBufferSize) {
				uiToProcess = miMaxSubBufferSize;
			}

			// Setup audio
			tuint32 uiChannel = 0;
			for (; uiChannel < (tuint)miChannelsIn; uiChannel++) {
				const tfloat* p = ppfSamplesIn[uiChannel];
				p += uiSamplesProcessed;

				memcpy(mppfIn[uiChannel], p, uiToProcess * sizeof(tfloat));
			}

			// Do it
			DoProcess((tfloat**)mppfOut, (const tfloat**) mppfIn, uiToProcess);

			// Copy output to out buffer
			for (uiChannel = 0; uiChannel < (tuint)miChannelsOut; uiChannel++) {
				tfloat* p = ppfSamplesOut[uiChannel];
				p += uiSamplesProcessed;

				memcpy(p, mppfOut[uiChannel], uiToProcess * sizeof(tfloat));
			}

			uiSamplesLeft -= uiToProcess;
			uiSamplesProcessed += uiToProcess;
		}
	}
}

void CBasePlugIn::DoProcess(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
/*	tint iChannels = 2;
	for (tint iChannel = 0; iChannel < iChannels; iChannel++) {
		tfloat* pfOut = ppfSamplesOut[iChannel];
		memset(pfOut, 0, iNrOfSamples * sizeof(tfloat));
	}*/

	mpDSPEngine->Process(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);

	miStreamTime += iNrOfSamples;

	float fMS = (float)(iNrOfSamples * 1000.0f / miSampleRate);
	mpDezipper->Dezip(fMS);
}

tuint32 CBasePlugIn::GetNrOfParameters()
{
	return mAutomationInfo.size();
}

void CBasePlugIn::GetParameterInfo(tuint32 iIndex, kspi::SParameterInfo* pInfo)
{
	SAutomationInfo* pParm = &(mAutomationInfo[iIndex]);

	*pInfo = pParm->Info;
}

tint32 CBasePlugIn::GetParameter(tuint32 iIndex)
{
	CAutoLock Lock(mMutex);

	SAutomationInfo* pParm = &(mAutomationInfo[iIndex]);

	return mpParmMan->Get(pParm->iIndex, pParm->Type, pParm->iExtra)->GetValue();
}

void CBasePlugIn::SetParameter(tuint32 iIndex, tint32 iValue)
{
#ifndef _DEMO_
	CAutoLock Lock(mMutex);

	SAutomationInfo* pParm = &(mAutomationInfo[iIndex]);

	mpParmMan->Set(false, iValue, pParm->iIndex, pParm->Type, pParm->iExtra);
#endif	// _DEMO_
}

tuint32 CBasePlugIn::GetChunkSize()
{
	IChunkFileMemory* pFile = CreateChunkFile();

	tuint32 uiRet = (tuint32)(pFile->GetMemorySize());

	pFile->Destroy();

	return uiRet;
}

void CBasePlugIn::GetChunk(void* p)
{
	CAutoLock Lock(mMutex);

	IChunkFileMemory* pFile = CreateChunkFile();

	pFile->GetMemory(p);

	pFile->Destroy();
}

void CBasePlugIn::SetChunk(const void* p, tuint32 iChunkSize)
{
	if (iChunkSize == 0) {
		return;
	}

	CAutoLock Lock(mMutex);

	// Open file
	CAutoDelete<IChunkFileMemory> pChunkFile(IChunkFileMemory::Create());
	tint32 iFileVersion;
	if (pChunkFile->Open(IFile::FileRead, iFileVersion, p, iChunkSize) == false) {
		// Couldn't "open" file, must be wrong format
		return;
	}

	// Performance
	{
		IChunk* pChunkOrg = pChunkFile->GetChunk(giChunkPerformance);
		if (pChunkOrg == NULL) {
			// No performance chunk
			return;
		}
		CAutoDelete<IChunk> pChunk = CAutoDelete<IChunk>(pChunkOrg);

		// Prepare memory
		tint32 iChunkSize = pChunk->GetSize();
		tchar* p = new tchar[iChunkSize - (2 * sizeof(tint32))];

		// Read the header
		tint32 iFirstParameter;
		tint32 iParameterCount;
		pChunk->Read((tchar*)&iFirstParameter, sizeof(tint32));
		iFirstParameter = CPSwap(iFirstParameter);
		pChunk->Read((tchar*)&iParameterCount, sizeof(tint32));
		iParameterCount = CPSwap(iParameterCount);

		// Set the parameters not in chunk to default value
		tint32 iParmCountThisVersion = mpParmMan->GetGlobalDataSize(0);
		tint32 iParm;
		for (iParm = iParameterCount; iParm < iParmCountThisVersion; iParm++) {
			const de::IParameter* pParm = mpParmMan->Get(iParm, de::IParameterManager::TypeGlobal, 0);
			mpParmMan->Set(true, pParm->GetDefaultValue(), iParm, de::IParameterManager::TypeGlobal, 0, false);
		}

		// Read parameters into memory
		if (pChunk->Read((tchar*)p, iChunkSize - (2 * sizeof(tint32))) != iChunkSize - (2 * sizeof(tint32))) {
		}

		// Set the parameters
		mpParmMan->SetGlobalData(iFirstParameter, iParameterCount, (const tint32*)p, 0, true);

		// Clean up
		delete[] p;
	}

	// "B" set
	{
		IChunk* pChunkOrg = pChunkFile->GetChunk(giChunkBSet);
		if (pChunkOrg == NULL) {
			// No performance chunk
		}
		else {
			CAutoDelete<IChunk> pChunk = CAutoDelete<IChunk>(pChunkOrg);

			// Prepare memory
			tint32 iChunkSize = pChunk->GetSize();
			tchar* p = new tchar[iChunkSize];
			pChunk->Read(p, iChunkSize);

			if (mpPresetBackground) {
				mpPresetBackground->Destroy();

				mpPresetBackground = IChunkFileMemory::Create();
				tint32 iVersionNumber = 1;
				mpPresetBackground->Open(IFile::FileRead, iVersionNumber, p, iChunkSize);
			}
		}
	}
}

IChunkFileMemory* CBasePlugIn::CreateChunkFile()
{
	// Performance data
	CAutoDelete<IChunk> pChunkPerf(IChunk::Create(NULL,
		0,
		giChunkPerformance));
	{
		tint32 iPerfParmCount = mpParmMan->GetGlobalDataSize(0);

		// Chunk header
		tint32 iValue;
		// First parameter
		iValue = 0;
		iValue = CPSwap(iValue);
		pChunkPerf->Write((const tchar*)&iValue, sizeof(tint32));
		// Parameter count
		iValue = iPerfParmCount;
		iValue = CPSwap(iValue);
		pChunkPerf->Write((const tchar*)&iValue, sizeof(tint32));

		// Get the parameter data
		tint32* piParmData = new tint32[iPerfParmCount];
		mpParmMan->GetGlobalData(piParmData, 0, 0, true);

		// Write parms to chunk
		pChunkPerf->Write((const tchar*)piParmData, iPerfParmCount * sizeof(tint32));

		delete[] piParmData;
	}

	// "B" set
	CAutoDelete<IChunk> pChunkB(IChunk::Create(NULL,
		0,
		giChunkBSet));
	if (mpPresetBackground) {
		// Get the data
		tint32 iDataSize = (tint32)mpPresetBackground->GetMemorySize();
		tchar* p = new tchar[iDataSize];
		mpPresetBackground->GetMemory(p);

		// Write to chunk
		pChunkB->Write(p, iDataSize);

		delete[] p;
	}

	// Create a chunk file
	IChunkFileMemory* pFile = IChunkFileMemory::Create();
	// Open it
	tint32 iFileVersion = 1;
	if (pFile->Open(IFile::FileCreate, iFileVersion) == false) {
		// Couldn't "open" file, must be wrong format
		return NULL;
	}

	// Add chunks to chunk file
#ifndef _DEMO_
	pFile->SetChunk(pChunkPerf, true);
	if (mpPresetBackground) {
		pFile->SetChunk(pChunkB, true);
	}
#endif	// _DEMO_

	return pFile;
}

void CBasePlugIn::Timer()
{
	CAutoLock Lock(mMutex);
}

tint32 CBasePlugIn::GetGlobalParm(tint32 iIndex, tint32 iSection)
{
	CAutoLock Lock(mMutex);

	return mpParmMan->Get(iIndex, de::IParameterManager::TypeGlobal, iSection)->GetValue();
}

void CBasePlugIn::SetGlobalParm(tint32 iIndex, tint32 iValue, tint iSection)
{
	CAutoLock Lock(mMutex);

	mpParmMan->Set(true, iValue, iIndex, de::IParameterManager::TypeGlobal, iSection);
}

tfloat CBasePlugIn::GetMeterSignal(tint iIndex)
{
	CAutoLock Lock(mMutex);

	return mpDSPEngine->GetMeterSignal(iIndex);
}

tfloat CBasePlugIn::GetMeterGR(tint iIndex)
{
	CAutoLock Lock(mMutex);

	return mpDSPEngine->GetMeterGR(iIndex);
}

tfloat CBasePlugIn::GetMeterSignalAndClear(tint iIndex)
{
	CAutoLock Lock(mMutex);

	return mpDSPEngine->GetMeterSignalAndClear(iIndex);
}

tfloat CBasePlugIn::GetMeterGRAndClear(tint iIndex)
{
	CAutoLock Lock(mMutex);

	return mpDSPEngine->GetMeterGRAndClear(iIndex);
}

/*
	IParameterManagerCallback
*/

void CBasePlugIn::GUINotify(tint32 iPaneID, tint32 iControlID, tint32 iValue)
{
	// This is called from the data manager when we should update a GUI control
//	if (mpGUI) {
//		mpGUI->SetControlValue(iControlID, iValue);
//	}
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		dynamic_cast<CBaseGUI*>(pGUI)->SetControlValue(iPaneID, iControlID, iValue);
	}
}

void CBasePlugIn::AutomationNotify(de::IParameterManager::EParameterType Type, tint32 iExtra, tint32 iParameterID, tint32 iValue)
{
//	CAutoLock Lock(mMutex);
	// Neccasary to avoid AU lockup in Logic
	CLockReleaser ReleaseR(mMutex);

	ASSERT(Type == de::IParameterManager::TypeGlobal);

	std::multimap<tint32, tint32>::iterator it = mAutomationMapGlobal.find(iParameterID);
	for (; it != mAutomationMapGlobal.end(); it++) {
		tint32 iAutomationIndex = it->second;

		SAutomationInfo Info = mAutomationInfo[iAutomationIndex];

		if (Info.iExtra == iExtra) {
			if (Info.iIndex == iParameterID) {
				mpModule->GetHost()->OnAutomationUpdate(dynamic_cast<kspi::IPlugIn*>(this),
					iAutomationIndex,
					iValue);
			}
		}
	}
}

void CBasePlugIn::MIDICCMapLearn(tint32 iParamId, tint32 iMinVal, tint32 iMaxVal, tint32 iStrength, kspi::SParameterInfo::ECurveType CurveType)
{
	mMIDICCParmInfo.iParamID = iParamId;
	mMIDICCParmInfo.iMinVal = iMinVal;
	mMIDICCParmInfo.iMaxVal = iMaxVal;
	mMIDICCParmInfo.iStrength = iStrength;
	mMIDICCParmInfo.CurveType = CurveType;

	// Begin listening
	mbListenForCC = true;
}

void CBasePlugIn::MIDICCMapUnlearn(tint32 iParamId)
{
	tint iIndex;
	for (iIndex = 0; iIndex < 128; iIndex++) {
		if(mpMIDICC[iIndex].iParamID == iParamId) {
			// Clear
			mpMIDICC[iIndex].iParamID = -1;

			break;
		}
	}
}

void CBasePlugIn::InputMIDI(const kspi::SMIDIEvent* pEvents, tuint32 uiNrOfEvents)
{
	CAutoLock Lock(mMutex);

	tuint32 uiEvent;
	for (uiEvent = 0; uiEvent < uiNrOfEvents; uiEvent++) {
		switch(pEvents[uiEvent].pcData[0] >> 4) {
			case 11: {
				// MIDI Control change Messages
				MIDICC(&(pEvents[uiEvent]));
				break;
			}	
		}
	}
}

void CBasePlugIn::MIDICC(const kspi::SMIDIEvent* pEvent)
{
	if (mbListenForCC) {
		// We're curently listening
		if (mMIDICCParmInfo.iParamID != -1)	{
			// Clear old controllers so only one MIDI controller can affect a parameter
			tint iIndex;
			for (iIndex = 0; iIndex < 128; iIndex++) {
				if (mpMIDICC[iIndex].iParamID == mMIDICCParmInfo.iParamID) {
					// Clearn
					mpMIDICC[iIndex].iParamID = -1;

					break;
				}
			}

			tint iCtrl = pEvent->pcData[1];
			mpMIDICC[iCtrl] = mMIDICCParmInfo;

			mMIDICCParmInfo.iParamID = -1;

			// Stop listening
			mbListenForCC = false;
		}

	}
	else{
		// Not listening, so do "automation read"
		tint iChannel = pEvent->pcData[0] & 0xf;
		if (miMIDIChannel == -1 || iChannel == miMIDIChannel) {
			// Right channel, or we're listening to all channels
			tint iCtrlNr = pEvent->pcData[1];
			tint iParamID = mpMIDICC[iCtrlNr].iParamID;

			if (iParamID != -1) {
				kspi::SParameterInfo::ECurveType CurveType = mpMIDICC[iCtrlNr].CurveType;
				tint32 iMinValue = mpMIDICC[iCtrlNr].iMinVal;
				tint32 iMaxValue = mpMIDICC[iCtrlNr].iMaxVal;
				tint32 iStrength = mpMIDICC[iCtrlNr].iStrength;

				tfloat fCtrlVal = pEvent->pcData[2] / 127.0f;

				// Convert from normalised float, either linearly or logarithmically
				de::IConverter* pConverter;
				switch(CurveType) {
					case kspi::SParameterInfo::CurveTypeLinear:
						pConverter = de::IConverter::CreateLinear(iMinValue, iMaxValue);
						break;
					case kspi::SParameterInfo::CurveTypeLog:
						pConverter = de::IConverter::CreateLog(iMinValue, iMaxValue, iStrength);
						break;
					case kspi::SParameterInfo::CurveTypeTrueLog:
						pConverter = de::IConverter::CreateTrueLog(iMinValue, iMaxValue);
						break;
					case kspi::SParameterInfo::CurveTypeX3:
						pConverter = de::IConverter::CreateX3(iMinValue, iMaxValue, 2);
						break;
				}

				tint32 iValue = pConverter->Normalised2Parm(fCtrlVal);

				pConverter->Destroy();

				mpParmMan->Set(true, iValue, iParamID, de::IParameterManager::TypeGlobal, 0, false);
			}
		}
	}	
}

void CBasePlugIn::LoadMIDICCMap(const std::string& sPathName)
{
	// Open file
	CAutoDelete<IChunkFile> pChunkFile(IChunkFile::Create());
	tint32 iFileVersion;
	if (pChunkFile->Open((const tchar*)(sPathName.c_str()),
		IFile::FileRead,
		iFileVersion) == false) {
		// Cannot open file
		return;
	}

	// MIDI CC Info
	{
		IChunk* pChunkOrg = pChunkFile->GetChunk(giChunkCCInfo);
		if (pChunkOrg != NULL) {
			// We have a  MIDICC chunk
			CAutoDelete<IChunk> pChunk = CAutoDelete<IChunk>(pChunkOrg);

			// Version
			tint32 iValue;
			pChunk->ReadCP(&iValue, 1);
			if (iValue == 0) {
				// Version 0
				// Read it directly into memory
				CAutoLock Lock(mMutex);
				pChunk->ReadCP((tint32*)mpMIDICC,
					128 * sizeof(SMIDICCInfo) / sizeof(tint32));
			}
		}
	}
}

void CBasePlugIn::SaveMIDICCMap(const std::string& sPathName)
{
	CAutoDelete<IChunk> pChunk(IChunk::Create(NULL, 0, giChunkCCInfo));
	{
		// CC Info version
		tint32 iValue = 0;
		iValue = CPSwap(iValue);
		pChunk->Write((const tchar*)&iValue, sizeof(tint32));

		// Temporary data
		tint32 iDataSize = 128 * sizeof(SMIDICCInfo) / sizeof(tint32);
		tint32* pData = new tint32[iDataSize];
		memcpy(pData, mpMIDICC, iDataSize * sizeof(tint32));

		// Write CC info to chunk
		pChunk->WriteCP(pData, iDataSize);

		// Delete temporary data
		delete[] pData;
	}

	CAutoDelete<IChunkFile> pFile(IChunkFile::Create());
	tint32 iFileVersion = 0;
	if (pFile->Open((tchar*)sPathName.c_str(), IFile::FileCreate, iFileVersion) == false) {
		// File couldn't be opened
		return;
	}

	pFile->SetChunk(pChunk, true);
}

void CBasePlugIn::MIDICCReset()
{
	tint i;
	for (i=0; i<128; i++) {
		mpMIDICC[i].iParamID = -1;	
	}
}

std::string CBasePlugIn::GetApplicationFolder()
{
	tchar psz[512];
#ifdef WIN32
	// Lasse, added 2008-07-28 - first try to look up folder in registry
	psz[0] = '\0';
	std::string sSubKey = "SOFTWARE\\KSPI\\Koblo\\";
	sSubKey += GetProductName();
	HKEY hkey;
	LONG lRes = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, sSubKey.c_str(), 0, KEY_READ, &hkey);
	if (lRes == ERROR_SUCCESS) {
		DWORD dwType = 0;
		DWORD dwSize = 512;
		lRes = ::RegQueryValueEx(hkey, "ApplicationFolder", NULL, &dwType, (BYTE*)psz, &dwSize);
		if (lRes != ERROR_SUCCESS)
			psz[0] = '\0';
		::RegCloseKey(hkey);
	}
	//::MessageBox(NULL, psz, "test", MB_OK);
	if (*psz) {
		tint iLen = strlen(psz);
		if (psz[iLen - 1] != '\\') {
			psz[iLen] = '\\';
			psz[iLen + 1] = '\0';
		}
		return psz;
	}
	// .. Lasse

	::GetModuleFileName(GetModule()->GetInstanceHandle(), (char*)psz, 512);

	// Step backwards in string until last '\' is found
	tchar* pChar = psz + strlen((char*)psz) - 1;
	while (*pChar != '\\') {
		pChar--;
	}
	// We have the last '\', so simply advance one, and set to 0 (to terminate string)
	pChar++;
	*pChar = 0;

	// Convert to internal path type
	IFile::PathFromOS((tchar*)psz);

	std::string sRet((const char*)psz);
#endif	// WIN32

#ifdef _Mac
	IFile::GetSystemDirectory(IFile::SystemDirApplications, (tchar*)psz);

	std::string sRet((const char*)psz);

	sRet += msCompanyName;
	sRet += ":";
	sRet += msProductName;
	sRet += ":";
#endif	// _Mac

	return sRet;
}

std::string CBasePlugIn::GetCompanyFolder()
{
	std::string sRet = GetApplicationFolder();

	// Remove trailing ':'
	sRet = sRet.substr(0, sRet.size() - 1);
	// Search for last ':'
	tint iPos = sRet.find_last_of(':');
	// Remove trailing path
	sRet = sRet.substr(0, iPos + 1);

	return sRet;
}

std::string CBasePlugIn::GetFilesFolder()
{
	std::string sRet = GetApplicationFolder();

	sRet += msProductName;
	sRet += " Files:";

	return sRet;
}

std::string CBasePlugIn::GetPresetFolder()
{
	std::string sRet = GetFilesFolder();

	sRet += "Presets:";

	return sRet;
}

std::string CBasePlugIn::GetResourceFolder()
{
	std::string sRet = GetApplicationFolder();

	sRet += msProductName;
	sRet += " Resources:";

	return sRet;
}

std::string CBasePlugIn::GetPreferenceFolder()
{
	std::string sRet = GetCompanyPreferenceFolder();

	sRet += msProductName;
	sRet += " Preferences:";

	return sRet;
}

std::string CBasePlugIn::GetCompanyPreferenceFolder()
{
	tchar pszPathName[1024];

#ifdef WIN32
	IFile::GetSystemDirectory(IFile::SystemDirDocuments, pszPathName);
#endif	// WIN32
#ifdef _Mac
	IFile::GetSystemDirectory(IFile::SystemDirPreferences, pszPathName);
#endif	// _Mac

	std::string sRet = std::string((const char*)pszPathName);

	sRet += msCompanyName;
	sRet += " Preferences:";

	return sRet;
}

void CBasePlugIn::OnLoadPreset(IChunkFile* pFile, tint32 iProductID)
{
	CAutoDelete<ge::IWaitCursor> pWaitCursor(ge::IWaitCursor::Create());

	if (iProductID != -1) {
		// Info chunk
		IChunk* pChunkOrg = pFile->GetChunk('INFO');
		if (pChunkOrg == NULL) {
			// No info chunk
			return;
		}
		CAutoDelete<IChunk> pChunk = CAutoDelete<IChunk>(pChunkOrg);

		tint32 iProdID;
		pChunk->ReadCP(&iProdID, 1);
		if (iProdID != iProductID) {
			return;
		}
	}

	// Performance (i.e. global)
	{
		IChunk* pChunkOrg = pFile->GetChunk(giChunkPerformance);
		if (pChunkOrg == NULL) {
			// No performance chunk
			return;
		}
		CAutoDelete<IChunk> pChunk = CAutoDelete<IChunk>(pChunkOrg);

		// Prepare memory
		tint32 iChunkSize = pChunk->GetSize();
		tchar* p = new tchar[iChunkSize - (2 * sizeof(tint32))];

		// Read the header
		tint32 iFirstParameter;
		tint32 iParameterCount;
		pChunk->Read((tchar*)&iFirstParameter, sizeof(tint32));
		iFirstParameter = CPSwap(iFirstParameter);
		pChunk->Read((tchar*)&iParameterCount, sizeof(tint32));
		iParameterCount = CPSwap(iParameterCount);

		// Set the parameters not in chunk to default value
		tint32 iParmCountThisVersion = mpParmMan->GetGlobalDataSize(0);
		tint32 iParm;
		for (iParm = iParameterCount; iParm < iParmCountThisVersion; iParm++) {
			const de::IParameter* pParm = mpParmMan->Get(iParm, de::IParameterManager::TypeGlobal, 0);
			mpParmMan->Set(true, pParm->GetDefaultValue(), iParm, de::IParameterManager::TypeGlobal, 0, false);
		}

		// Read parameters into memory
		if (pChunk->Read((tchar*)p, iChunkSize - (2 * sizeof(tint32))) != iChunkSize - (2 * sizeof(tint32))) {
		}

		// Set the parameters
		mpParmMan->SetGlobalData(iFirstParameter, iParameterCount, (const tint32*)p, 0, true);

		// Clean up
		delete[] p;
	}
}

void CBasePlugIn::OnSavePreset(IChunkFile* pFile, tint32 iProductID)
{
	CAutoDelete<ge::IWaitCursor> pWaitCursor(ge::IWaitCursor::Create());

	// Performance data (i.e. global data)
	CAutoDelete<IChunk> pChunkPerf(IChunk::Create(NULL,
		0,
		giChunkPerformance));
	{
		tint32 iPerfParmCount = mpParmMan->GetGlobalDataSize(0);

		// Chunk header
		tint32 iValue;
		// First parameter
		iValue = 0;
		iValue = CPSwap(iValue);
		pChunkPerf->Write((const tchar*)&iValue, sizeof(tint32));
		// Parameter count
		iValue = iPerfParmCount;
		iValue = CPSwap(iValue);
		pChunkPerf->Write((const tchar*)&iValue, sizeof(tint32));

		// Get the parameter data
		tint32* piParmData = new tint32[iPerfParmCount];
		mpParmMan->GetGlobalData(piParmData, 0, 0, true);

		// Write parms to chunk
		pChunkPerf->Write((const tchar*)piParmData, iPerfParmCount * sizeof(tint32));

		delete[] piParmData;
	}

	// Add chunk to chunk file
	pFile->SetChunk(pChunkPerf, true);

	if (iProductID != -1) {
		// Info chunk
		CAutoDelete<IChunk> pChunkInfo(IChunk::Create(NULL,
			0,
			'INFO'));
		pChunkInfo->WriteCP(&iProductID, 1);

		// Add chunk to chunk file
		pFile->SetChunk(pChunkInfo, true);
	}
}

void CBasePlugIn::InsertParameterLinear(tint32 iParmID, const std::string& sName, tint32 iDecimals, const std::string sUnit /*= ""*/)
{
	mAutomationMapGlobal.insert(std::pair<tint32, tint32>(iParmID, mAutomationInfo.size()));

	SAutomationInfo Info;

	Info.iIndex = iParmID;
	Info.Type = de::IParameterManager::TypeGlobal;
	Info.iExtra = 0;

	strcpy((char*)(Info.Info.pszName), sName.c_str());
	Info.Info.Type = kspi::SParameterInfo::TypeLong;
	Info.Info.iDecimals = iDecimals;
	Info.Info.CurveType = kspi::SParameterInfo::CurveTypeLinear;
	strcpy((char*)Info.Info.pszUnitName, sUnit.c_str());

	const de::IParameter* pParameter = mpParmMan->Get(iParmID, de::IParameterManager::TypeGlobal, 0);
	Info.Info.iMinValue = pParameter->GetMinValue();
	Info.Info.iMaxValue = pParameter->GetMaxValue();
	Info.Info.iDefaultValue = pParameter->GetDefaultValue();

	mAutomationInfo.insert(mAutomationInfo.end(), Info);
}

void CBasePlugIn::InsertParameterEnum(tint32 iParmID, const std::string& sName, const std::string& sEnumName, const std::string sUnit /*= ""*/)
{
	mAutomationMapGlobal.insert(std::pair<tint32, tint32>(iParmID, mAutomationInfo.size()));

	SAutomationInfo Info;

	Info.iIndex = iParmID;
	Info.Type = de::IParameterManager::TypeGlobal;
	Info.iExtra = 0;

	strcpy((char*)(Info.Info.pszName), sName.c_str());
	Info.Info.Type = kspi::SParameterInfo::TypeEnum;
	Info.Info.CurveType = kspi::SParameterInfo::CurveTypeLinear;
	strcpy((char*)(Info.Info.pszEnumText), sEnumName.c_str());
	strcpy((char*)Info.Info.pszUnitName, sUnit.c_str());

	const de::IParameter* pParameter = mpParmMan->Get(iParmID, de::IParameterManager::TypeGlobal, 0);
	Info.Info.iMinValue = pParameter->GetMinValue();
	Info.Info.iMaxValue = pParameter->GetMaxValue();
	Info.Info.iDefaultValue = pParameter->GetDefaultValue();

	mAutomationInfo.insert(mAutomationInfo.end(), Info);
}

void CBasePlugIn::InsertParameterTrueLog(tint32 iParmID, const std::string& sName, tint32 iDecimals, const std::string sUnit /*= ""*/)
{
	mAutomationMapGlobal.insert(std::pair<tint32, tint32>(iParmID, mAutomationInfo.size()));

	SAutomationInfo Info;

	Info.iIndex = iParmID;
	Info.Type = de::IParameterManager::TypeGlobal;
	Info.iExtra = 0;

	strcpy((char*)(Info.Info.pszName), sName.c_str());
	Info.Info.Type = kspi::SParameterInfo::TypeLong;
	Info.Info.iDecimals = iDecimals;
	Info.Info.CurveType = kspi::SParameterInfo::CurveTypeTrueLog;
	strcpy((char*)Info.Info.pszUnitName, sUnit.c_str());

	const de::IParameter* pParameter = mpParmMan->Get(iParmID, de::IParameterManager::TypeGlobal, 0);
	Info.Info.iMinValue = pParameter->GetMinValue();
	Info.Info.iMaxValue = pParameter->GetMaxValue();
	Info.Info.iDefaultValue = pParameter->GetDefaultValue();

	mAutomationInfo.insert(mAutomationInfo.end(), Info);
}

void CBasePlugIn::InsertParameterLog(tint32 iParmID, tint32* piStrength, const std::string& sName, tint32 iDecimals, const std::string sUnit /*= ""*/)
{
	mAutomationMapGlobal.insert(std::pair<tint32, tint32>(iParmID, mAutomationInfo.size()));

	SAutomationInfo Info;

	Info.iIndex = iParmID;
	Info.Type = de::IParameterManager::TypeGlobal;
	Info.iExtra = 0;

	strcpy((char*)(Info.Info.pszName), sName.c_str());
	Info.Info.Type = kspi::SParameterInfo::TypeLong;
	Info.Info.iDecimals = iDecimals;
	Info.Info.CurveType = kspi::SParameterInfo::CurveTypeLog;
	Info.Info.pCurveData = (void*)piStrength;
	
	strcpy((char*)Info.Info.pszUnitName, sUnit.c_str());

	const de::IParameter* pParameter = mpParmMan->Get(iParmID, de::IParameterManager::TypeGlobal, 0);
	Info.Info.iMinValue = pParameter->GetMinValue();
	Info.Info.iMaxValue = pParameter->GetMaxValue();
	Info.Info.iDefaultValue = pParameter->GetDefaultValue();

	mAutomationInfo.insert(mAutomationInfo.end(), Info);
}

void CBasePlugIn::SetMIDIChannel(tint32 iChannel)
{
	miMIDIChannel = iChannel;
}

void CBasePlugIn::ActionAB(tint iIndex)
{
	if (miPresetSetCurrent != iIndex) {
		// Switch to set not current
		CopyBackgroundPresetToForeground();

		miPresetSetCurrent = iIndex;
	}
}

void CBasePlugIn::ActionAToB()
{
	if (miPresetSetCurrent == 0) {
		CopyForegroundPresetToBackground();
	}
	else {
		CopyBackgroundPresetToForeground();
	}
}

void CBasePlugIn::ActionBToA()
{
	if (miPresetSetCurrent == 0) {
		CopyBackgroundPresetToForeground();
	}
	else {
		CopyForegroundPresetToBackground();
	}
}

void CBasePlugIn::CopyBackgroundPresetToForeground()
{
	// Create new background set
	IChunkFileMemory* pPresetBackgroundNew = IChunkFileMemory::Create();
	tint32 iVersionNumber = 1;
	pPresetBackgroundNew->Open(IFile::FileCreate, iVersionNumber);

	// Read current preset into background preset set
	OnSavePreset(dynamic_cast<IChunkFile*>(pPresetBackgroundNew));

	// Get the memory from the current background set
	tint64 iMemSize = mpPresetBackground->GetMemorySize();
	void* pMem = (void*)new char[(tuint)iMemSize];
	mpPresetBackground->GetMemory(pMem);

	// Delete the current background set
	mpPresetBackground->Destroy();

	// Switch, so the current preset is the new background
	mpPresetBackground = pPresetBackgroundNew;

	// Create chunk with old background
	CAutoDelete<IChunkFileMemory> pFile(IChunkFileMemory::Create());
	pFile->Open(IFile::FileRead, iVersionNumber, pMem, iMemSize);

	// Set the preset
	OnLoadPreset(pFile);

	// Delete the memory
	delete[] pMem;
}

void CBasePlugIn::CopyForegroundPresetToBackground()
{
	// Destroy current background set
	mpPresetBackground->Destroy();

	// Create preset set
	mpPresetBackground = IChunkFileMemory::Create();
	tint32 iVersionNumber = 1;
	mpPresetBackground->Open(IFile::FileCreate, iVersionNumber);

	// Read current preset into background preset set
	OnSavePreset(dynamic_cast<IChunkFile*>(mpPresetBackground));
}

void CBasePlugIn::SetupAB()
{
	// Create preset set
	mpPresetBackground = IChunkFileMemory::Create();
	tint32 iVersionNumber = 1;
	mpPresetBackground->Open(IFile::FileCreate, iVersionNumber);

	// Read current preset into background preset set
	OnSavePreset(dynamic_cast<IChunkFile*>(mpPresetBackground));

	// Current set is A
	miPresetSetCurrent = 0;
}

void CBasePlugIn::OnGUITimer()
{
	std::list<CBaseGUI*>::const_iterator it = mGUIs.begin();
	for (; it != mGUIs.end(); it++) {
		CBaseGUI* pGUI = *it;
		pGUI->OnTimer();
	}
}




