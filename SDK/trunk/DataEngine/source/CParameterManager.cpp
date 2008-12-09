/*!	\file CParameterManager.cpp
	\author Michael Olsen
	\date 07/Jan/2005
	\date 07/Jan/2005
*/

#include "deInternalOS.h"


CParameterManager::CParameterManager()
	: mpCallback(NULL),
	mpGlobalParameters(NULL),
	mpLayerParameters(NULL),
	mpPartParameters(NULL),
	miSectionCount(0), miLayerCount(0), miPartCount(0)
{
}

CParameterManager::~CParameterManager()
{
	tint iParm;

	if (mpGlobalParameters) {
		tint iSection;
		for (iSection = 0; iSection < miSectionCount; iSection++) {
			for (iParm = 0; iParm < (tint)mpGlobalParameters[iSection].size(); iParm++) {
				mpGlobalParameters[iSection][iParm]->pParameter->Destroy();
				delete mpGlobalParameters[iSection][iParm];
			}
		}
		delete[] mpGlobalParameters;
	}

	if (mpLayerParameters) {
		tint iLayer;
		for (iLayer = 0; iLayer < miLayerCount; iLayer++) {
			for (iParm = 0; iParm < (tint)mpLayerParameters[iLayer].size(); iParm++) {
				mpLayerParameters[iLayer][iParm]->pParameter->Destroy();
				delete mpLayerParameters[iLayer][iParm];
			}
		}
		delete[] mpLayerParameters;
	}

	if (mpPartParameters) {
		tint iPart;
		for (iPart = 0; iPart < miPartCount; iPart++) {
			for (iParm = 0; iParm < (tint)mpPartParameters[iPart].size(); iParm++) {
				mpPartParameters[iPart][iParm]->pParameter->Destroy();
				delete mpPartParameters[iPart][iParm];
			}
		}
		delete[] mpPartParameters;
	}
}

IParameterManager* IParameterManager::Create()
{
	return dynamic_cast<IParameterManager*>(new CParameterManager());
}

void CParameterManager::Destroy()
{
	delete dynamic_cast<CParameterManager*>(this);
}

void CParameterManager::SetCallback(IParameterManagerCallback* pCallback)
{
	mpCallback = pCallback;
}

void CParameterManager::SetGlobalSectionCount(tint iSections)
{
	CAutoLock Lock(mMutex);
	miSectionCount = iSections;

	mpGlobalParameters = new std::vector<SParameterInfo*>[miSectionCount];
}

void CParameterManager::SetLayerCount(tint iLayers)
{
	CAutoLock Lock(mMutex);
	miLayerCount = iLayers;

	mpLayerParameters = new std::vector<SParameterInfo*>[miLayerCount];
}

void CParameterManager::SetPartCount(tint iParts)
{
	CAutoLock Lock(mMutex);
	miPartCount = iParts;

	mpPartParameters = new std::vector<SParameterInfo*>[miPartCount];
}

void CParameterManager::Add(IParameter* pParameter, EParameterType Type, tint32 iExtra)
{
	CAutoLock Lock(mMutex);
	// Create new parameter info
	SParameterInfo* pNew = new SParameterInfo;
	// Set the parameter part of struct
	pNew->pParameter = pParameter;

	// Determine which parameter block to use
	std::vector<SParameterInfo*>* pParms;
	switch(Type) {
		case TypeGlobal:
			pParms = &(mpGlobalParameters[iExtra]);
			break;
		case TypeLayer:
			pParms = &(mpLayerParameters[iExtra]);
			break;
		case TypePart:
			pParms = &(mpPartParameters[iExtra]);
			break;
	}

	// Add the parameter
	// We allocate memory in 20 unit blocks, so that we don't need to reallocate too often
	if ((*pParms).capacity() == (*pParms).size()) {
		(*pParms).reserve((*pParms).capacity() + 20);
	}

	// Put new parameter info in list
	(*pParms).push_back(pNew);
}

const IParameter* CParameterManager::Get(tint32 iParameterID, EParameterType Type, tint32 iExtra)
{
	CAutoLock Lock(mMutex);
	// Determine which parameter block to use
	std::vector<SParameterInfo*>* pParms;
	switch(Type) {
		case TypeGlobal:
			pParms = &(mpGlobalParameters[iExtra]);
			break;
		case TypeLayer:
			pParms = &(mpLayerParameters[iExtra]);
			break;
		case TypePart:
			pParms = &(mpPartParameters[iExtra]);
			break;
	}

	return (*pParms)[iParameterID]->pParameter;
}

void CParameterManager::Set(tbool bCallAutomationUpdate, tint32 iValue, tint32 iParameterID, EParameterType Type, tint32 iExtra, tbool bForceUpdate)
{
	CAutoLock Lock(mMutex);
	// Determine which parameter block to use
	std::vector<SParameterInfo*>* pParms;
	switch(Type) {
		case TypeGlobal:
			pParms = &(mpGlobalParameters[iExtra]);
			break;
		case TypeLayer:
			pParms = &(mpLayerParameters[iExtra]);
			break;
		case TypePart:
			pParms = &(mpPartParameters[iExtra]);
			break;
	}

	// Check if value needs updating
	if ((bForceUpdate) || ((*pParms)[iParameterID]->pParameter->GetValue() != iValue)) {
		// Call SetValue with new value
		(*pParms)[iParameterID]->pParameter->SetValue(iValue, true);

		// Update all controls
		DoParameterUpdate(pParms, (*pParms)[iParameterID]->pParameter->GetID(), iValue, -1, -1, false);

		if (bCallAutomationUpdate) {
			// Notify data change
			mpCallback->AutomationNotify(Type,
				iExtra,
				(*pParms)[iParameterID]->pParameter->GetID(),
				(*pParms)[iParameterID]->pParameter->GetValue());
		}
	}
}

tint32 CParameterManager::GetParameterCount(tint32 iSection)
{
	CAutoLock Lock(mMutex);
	return mpGlobalParameters[iSection].size();
}

void CParameterManager::RegisterControl(tint32 iPaneID, tint32 iControlID, tint32 iParameterID, EParameterType Type, tint32 iExtra)
{
	CAutoLock Lock(mMutex);
	// Determine which parameter block to use
	std::vector<SParameterInfo*>* pParms;
	switch(Type) {
		case TypeGlobal:
			pParms = &(mpGlobalParameters[iExtra]);
			break;
		case TypeLayer:
			pParms = &(mpLayerParameters[iExtra]);
			break;
		case TypePart:
			pParms = &(mpPartParameters[iExtra]);
			break;
	}

	// Find the parameter
	SParameterInfo* pInfo = (*pParms)[iParameterID];

	// Insert control ID in list
	SControlID ID(iPaneID, iControlID);
	pInfo->ControlIDs.insert(pInfo->ControlIDs.end(), ID);

	// Update map
	SControlMapInfo Info;
	Info.Type = Type;
	Info.iIndex = iExtra;
	Info.iParmID = iParameterID;
	ControlMap[ID] = Info;
}

IParameter* CParameterManager::GetParameterFromControl(tint32 iPaneID, tint32 iControlID)
{
	CAutoLock Lock(mMutex);
	SControlID ID(iPaneID, iControlID);
	std::map<SControlID, SControlMapInfo>::iterator it = ControlMap.find(ID);
	if (it == ControlMap.end()) {
		// This means there were no parameter attached to control
		return NULL;
	}
	SControlMapInfo Info = it->second;

	// Determine which parameter block to use
	std::vector<SParameterInfo*>* pParms;
	switch(Info.Type) {
		case TypeGlobal:
			pParms = &(mpGlobalParameters[Info.iIndex]);
			break;
		case TypeLayer:
			pParms = &(mpLayerParameters[Info.iIndex]);
			break;
		case TypePart:
			pParms = &(mpPartParameters[Info.iIndex]);
			break;
	}

	return (*pParms)[Info.iParmID]->pParameter;
}

void CParameterManager::DeregisterControl(tint32 iPaneID, tint32 iControlID)
{
	CAutoLock Lock(mMutex);
	// Locate the parameter ID
	SControlID ID(iPaneID, iControlID);
	std::map<SControlID, SControlMapInfo>::iterator it = ControlMap.find(ID);
	if (it == ControlMap.end()) {
		// This means there were no parameter attached to control
		return;
	}
	SControlMapInfo Info = it->second;

	// Determine which parameter block to use
	std::vector<SParameterInfo*>* pParms;
	switch(Info.Type) {
		case TypeGlobal:
			pParms = &(mpGlobalParameters[Info.iIndex]);
			break;
		case TypeLayer:
			pParms = &(mpLayerParameters[Info.iIndex]);
			break;
		case TypePart:
			pParms = &(mpPartParameters[Info.iIndex]);
			break;
	}

	// Find the parameter and remove the control ID from its list
	(*pParms)[Info.iParmID]->ControlIDs.remove(ID);

	// Also erase control IDs map
	ControlMap.erase(ID);
}

void CParameterManager::DeregisterAllControls()
{
	CAutoLock Lock(mMutex);
	std::map<SControlID, SControlMapInfo>::iterator it = ControlMap.begin();
	while (it != ControlMap.end()) {
		SControlMapInfo Info = it->second;

		// Determine which parameter block to use
		std::vector<SParameterInfo*>* pParms;
		switch(Info.Type) {
			case TypeGlobal:
				pParms = &(mpGlobalParameters[Info.iIndex]);
				break;
			case TypeLayer:
				pParms = &(mpLayerParameters[Info.iIndex]);
				break;
			case TypePart:
				pParms = &(mpPartParameters[Info.iIndex]);
				break;
		}

		// Find the parameter and remove the control ID from its list
		(*pParms)[Info.iParmID]->ControlIDs.remove(it->first);

		it++;
	}

	ControlMap.clear();
}

void CParameterManager::ControlUpdate(tint32 iPaneID, tint32 iControlID, tint32 iValueNew, tbool bUpdateAll)
{
	CAutoLock Lock(mMutex);
	// Locate the control in the map
	SControlID ID(iPaneID, iControlID);
	std::map<SControlID, SControlMapInfo>::iterator it = ControlMap.find(ID);

	if (it != ControlMap.end()) {
		// We found the control in the map, now get the parameter ID of the attached parameter
		SControlMapInfo Info = it->second;

		// Determine which parameter block to use
		std::vector<SParameterInfo*>* pParms;
		switch(Info.Type) {
			case TypeGlobal:
				pParms = &(mpGlobalParameters[Info.iIndex]);
				break;
			case TypeLayer:
				pParms = &(mpLayerParameters[Info.iIndex]);
				break;
			case TypePart:
				pParms = &(mpPartParameters[Info.iIndex]);
				break;
		}

		// Check if value needs updating
		if ((*pParms)[Info.iParmID]->pParameter->GetValue() != iValueNew) {
			// Call SetValue with new value
			(*pParms)[Info.iParmID]->pParameter->SetValue(iValueNew, true);

			// Check if the value was limited
			if ((*pParms)[Info.iParmID]->pParameter->GetValue() != iValueNew) {
				// Value was limited (changed), so update all controls, including the caller
				DoParameterUpdate(pParms, (*pParms)[Info.iParmID]->pParameter->GetID(), (*pParms)[Info.iParmID]->pParameter->GetValue(), -1, false);
			}
			else {
				// Update all controls but this (in case other controls were attached to this parameter)
				if (bUpdateAll) {
					DoParameterUpdate(pParms, (*pParms)[Info.iParmID]->pParameter->GetID(), iValueNew, -1, -1, false);
				}
				else {
					DoParameterUpdate(pParms, (*pParms)[Info.iParmID]->pParameter->GetID(), iValueNew, iPaneID, iControlID, false);
				}
			}

			// Notify data change
			mpCallback->AutomationNotify(
				Info.Type,
				Info.iIndex,
				(*pParms)[Info.iParmID]->pParameter->GetID(),
				(*pParms)[Info.iParmID]->pParameter->GetValue());
		}
	}
}

void CParameterManager::DoParameterUpdate(std::vector<SParameterInfo*>* pParms, tint32 iParameterID, tint32 iValueNew, tint32 iPaneExclude, tint32 iControlExclude, tbool bCallSetValue)
{
	CAutoLock Lock(mMutex);
	if (bCallSetValue) {
		if ((tuint)iParameterID < (*pParms).size()) {
			(*pParms)[iParameterID]->pParameter->SetValue(iValueNew, true);
		}
	}

	// Update each control attached to parameter
	std::list<SControlID>::iterator it = (*pParms)[iParameterID]->ControlIDs.begin();
	for (; it != (*pParms)[iParameterID]->ControlIDs.end(); it++) {
		const SControlID& ID = *it;
		if (ID.iPaneID == iPaneExclude && ID.iControlID == iControlExclude) {
		}
		else {
			mpCallback->GUINotify((*it).iPaneID, (*it).iControlID, (*pParms)[iParameterID]->pParameter->GetValue());
		}
	}
}

void CParameterManager::DoControlUpdateAll(std::vector<SParameterInfo*>* pParms)
{
	CAutoLock Lock(mMutex);
	// Go through all parameters
	for (tuint i = 0; i < (*pParms).size(); i++) {
		// Update each control attached
		std::list<SControlID>::iterator it = (*pParms)[i]->ControlIDs.begin();
		for (; it != (*pParms)[i]->ControlIDs.end(); it++) {
			mpCallback->GUINotify((*it).iPaneID, (*it).iControlID, (*pParms)[i]->pParameter->GetValue());
		}
	}
}

void CParameterManager::ControlUpdateAll()
{
	CAutoLock Lock(mMutex);
	tint iSection;
	for (iSection = 0; iSection < miSectionCount; iSection++) {
		DoControlUpdateAll(&(mpGlobalParameters[iSection]));
	}

	tint iLayer;
	for (iLayer = 0; iLayer < miLayerCount; iLayer++) {
		DoControlUpdateAll(&(mpLayerParameters[iLayer]));
	}

	tint iPart;
	for (iPart = 0; iPart < miPartCount; iPart++) {
		DoControlUpdateAll(&(mpPartParameters[iPart]));
	}
}

void CParameterManager::DoParameterUpdateAll(std::vector<SParameterInfo*>* pParms)
{
	CAutoLock Lock(mMutex);
	// Go through all parameters
	for (tuint i = 0; i < (*pParms).size(); i++) {
		// Get info for this parameter
		SParameterInfo* pInfo = (*pParms)[i];
		// Update this parameter with current value
		pInfo->pParameter->SetValue(pInfo->pParameter->GetValue());
	}
}

void CParameterManager::ParameterUpdateAll()
{
	CAutoLock Lock(mMutex);
	tint iSection;
	for (iSection = 0; iSection < miSectionCount; iSection++) {
		DoParameterUpdateAll(&(mpGlobalParameters[iSection]));
	}

	tint iLayer;
	for (iLayer = 0; iLayer < miLayerCount; iLayer++) {
		DoParameterUpdateAll(&(mpLayerParameters[iLayer]));
	}

	tint iPart;
	for (iPart = 0; iPart < miPartCount; iPart++) {
		DoParameterUpdateAll(&(mpPartParameters[iPart]));
	}
}

void CParameterManager::GlobalParameterUpdateAll(tint iSection)
{
	CAutoLock Lock(mMutex);
	DoParameterUpdateAll(&(mpGlobalParameters[iSection]));
}

void CParameterManager::DoAutomationUpdateAll(IParameterManager::EParameterType Type, tint32 iExtra, std::vector<SParameterInfo*>* pParms)
{
	CAutoLock Lock(mMutex);
	// Go through all parameters
	for (tuint i = 0; i < (*pParms).size(); i++) {
		// Get info for this parameter
		SParameterInfo* pInfo = (*pParms)[i];
		// Call automation update for this parameter with current value
		mpCallback->AutomationNotify(Type,
			iExtra,
			pInfo->pParameter->GetID(),
			pInfo->pParameter->GetValue());
	}
}

void CParameterManager::AutomationUpdateAll()
{
	CAutoLock Lock(mMutex);
	tint iSection;
	for (iSection = 0; iSection < miSectionCount; iSection++) {
		DoAutomationUpdateAll(TypeGlobal,
			iSection,
			&(mpGlobalParameters[iSection]));
	}

	tint iLayer;
	for (iLayer = 0; iLayer < miLayerCount; iLayer++) {
		DoAutomationUpdateAll(TypeLayer,
			iLayer,
			&(mpLayerParameters[iLayer]));
	}

	tint iPart;
	for (iPart = 0; iPart < miPartCount; iPart++) {
		DoAutomationUpdateAll(TypePart,
			iPart,
			&(mpPartParameters[iPart]));
	}
}

tint32 CParameterManager::GetPartDataSize()
{
	CAutoLock Lock(mMutex);
	return mpPartParameters[0].size();
}

void CParameterManager::GetPartData(tint32 iPart, tint32* piData, tint32 iParmStart, tbool bSwap)
{
	CAutoLock Lock(mMutex);
	tint32 iIndex;
	tint32 iSize = mpPartParameters[iPart].size();
	for (iIndex = 0; iIndex < (iSize - iParmStart); iIndex++) {
		// Get the value
		piData[iIndex] = mpPartParameters[iPart][iParmStart + iIndex]->pParameter->GetValue();

		if (bSwap) {
			// Crossplatform swap it
			piData[iIndex] = CPSwap(piData[iIndex]);
		}
	}
}

void CParameterManager::SetPartData(tint32 iPart, tint32 iParmStart, tint32 iParmCount, const tint32* piData, tbool bSwap)
{
	CAutoLock Lock(mMutex);
	tint32 iSize = mpPartParameters[iPart].size();
	// Limit parameter count, so we don't exceed bounds
	if (iParmStart + iParmCount > iSize) {
		iParmCount = iSize - iParmStart;
	}

	tint32 iIndex;
	for (iIndex = 0; iIndex < iParmCount; iIndex++) {
		// Calculate the current parameter index
		tint32 iIndexThis = iIndex + iParmStart;

		tint32 iData = piData[iIndex];
		if (bSwap) {
			// Crossplatform swap the parameter
			iData = CPSwap(iData);
		}

		// Set the value
		Set(true, iData, iIndexThis, TypePart, iPart, false);
	}
}

tint32 CParameterManager::GetLayerDataSize()
{
	CAutoLock Lock(mMutex);
	return mpLayerParameters[0].size();
}

void CParameterManager::GetLayerData(tint32 iLayer, tint32* piData, tint32 iParmStart, tbool bSwap)
{
	CAutoLock Lock(mMutex);
	tint32 iIndex;
	tint32 iSize = mpLayerParameters[iLayer].size();
	for (iIndex = 0; iIndex < (iSize - iParmStart); iIndex++) {
		// Get the value
		piData[iIndex] = mpLayerParameters[iLayer][iParmStart + iIndex]->pParameter->GetValue();

		if (bSwap) {
			// Crossplatform swap it
			piData[iIndex] = CPSwap(piData[iIndex]);
		}
	}
}

void CParameterManager::SetLayerData(tint32 iLayer, tint32 iParmStart, tint32 iParmCount, const tint32* piData, tbool bSwap)
{
	CAutoLock Lock(mMutex);
	tint32 iSize = mpLayerParameters[iLayer].size();
	// Limit parameter count, so we don't exceed bounds
	if (iParmStart + iParmCount > iSize) {
		iParmCount = iSize - iParmStart;
	}

	tint32 iIndex;
	for (iIndex = 0; iIndex < iParmCount; iIndex++) {
		// Calculate the current parameter index
		tint32 iIndexThis = iIndex + iParmStart;

		tint32 iData = piData[iIndex];
		if (bSwap) {
			// Crossplatform swap the parameter
			iData = CPSwap(iData);
		}

		// Set the value
		Set(true, iData, iIndexThis, TypeLayer, iLayer, false);
	}
}

tint32 CParameterManager::GetGlobalDataSize(tint iSection)
{
	CAutoLock Lock(mMutex);
	return mpGlobalParameters[iSection].size();
}

void CParameterManager::GetGlobalData(tint32* piData, tint32 iParmStart, tint iSection, tbool bSwap)
{
	CAutoLock Lock(mMutex);
	tint32 iIndex;
	tint32 iSize = mpGlobalParameters[iSection].size();
	for (iIndex = 0; iIndex < (iSize - iParmStart); iIndex++) {
		// Get the value
		piData[iIndex] = mpGlobalParameters[iSection][iParmStart + iIndex]->pParameter->GetValue();

		if (bSwap) {
			// Crossplatform swap it
			piData[iIndex] = CPSwap(piData[iIndex]);
		}
	}
}

void CParameterManager::SetGlobalData(tint32 iParmStart, tint32 iParmCount, const tint32* piData, tint iSection, tbool bSwap)
{
	CAutoLock Lock(mMutex);
	tint32 iSize = mpGlobalParameters[iSection].size();
	// Limit parameter count, so we don't exceed bounds
	if (iParmStart + iParmCount > iSize) {
		iParmCount = iSize - iParmStart;
	}

	tint32 iIndex;
	for (iIndex = 0; iIndex < iParmCount; iIndex++) {
		// Calculate the current parameter index
		tint32 iIndexThis = iIndex + iParmStart;

		tint32 iData = piData[iIndex];
		if (bSwap) {
			// Crossplatform swap the parameter
			iData = CPSwap(iData);
		}

		// Set the value
		Set(true, iData, iIndexThis, TypeGlobal, iSection, false);
	}
}

