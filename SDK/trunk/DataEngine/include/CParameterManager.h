// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
//
// This file is part of the Koblo SDK.
//
// the Koblo SDK is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo SDK is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.



/*! \class CParameterManager
 * \brief Implementation of IParameterManager
*/
class CParameterManager : public virtual IParameterManager
{
public:
	//! Constructor
	CParameterManager();

	//! Destructor
	virtual ~CParameterManager();

	//! IDestructable override
	virtual void Destroy();

	//! IParameterManager override
	virtual void SetCallback(IParameterManagerCallback* pCallback);
	//! IParameterManager override
	virtual void SetGlobalSectionCount(tint iSections) ;
	//! IParameterManager override
	virtual void SetLayerCount(tint iLayers);
	//! IParameterManager override
	virtual void SetPartCount(tint iParts);
	//! IParameterManager override
	virtual void Add(IParameter* pParameter, EParameterType Type, tint32 iExtra);
	//! IParameterManager override
	virtual const IParameter* Get(tint32 iParameterID, EParameterType Type, tint32 iExtra);
	//! IParameterManager override
	virtual void Set(tbool bCallAutomationUpdate, tint32 iValue, tint32 iParameterID, EParameterType Type, tint32 iExtra, tbool bForceUpdate);
	//! IParameterManager override
	virtual tint32 GetParameterCount(tint32 iSection);
	//! IParameterManager override
	virtual void RegisterControl(tint32 iPaneID, tint32 iControlID, tint32 iParameterID, EParameterType Type, tint32 iExtra);
	//! IParameterManager override
	virtual void DeregisterAllControls();
	//! IParameterManager override
	virtual void DeregisterControl(tint32 iPaneID, tint32 iControlID);
	//! IParameterManager override
	virtual IParameter* GetParameterFromControl(tint32 iPaneID, tint32 iControlID);
	//! IParameterManager override
	virtual void ControlUpdate(tint32 iPaneID, tint32 iControlID, tint32 iValueNew, tbool bUpdateAll = false);
	//! IParameterManager override
	virtual void ControlUpdateAll();
	//! IParameterManager override
	virtual void ParameterUpdateAll();
	//! IParameterManager override
	virtual void GlobalParameterUpdateAll(tint iSection);
	//! IParameterManager override
	virtual void AutomationUpdateAll();
	//! IParameterManager override
	virtual tint32 GetPartDataSize();
	//! IParameterManager override
	virtual void GetPartData(tint32 iPart, tint32* piData, tint32 iParmStart, tbool bSwap);
	//! IParameterManager override
	virtual void SetPartData(tint32 iPart, tint32 iParmStart, tint32 iParmCount, const tint32* piData, tbool bSwap);
	//! IParameterManager override
	virtual tint32 GetLayerDataSize();
	//! IParameterManager override
	virtual void GetLayerData(tint32 iLayer, tint32* piData, tint32 iParmStart, tbool bSwap);
	//! IParameterManager override
	virtual void SetLayerData(tint32 iLayer, tint32 iParmStart, tint32 iParmCount, const tint32* piData, tbool bSwap);
	//! IParameterManager override
	virtual tint32 GetGlobalDataSize(tint iSection);
	//! IParameterManager override
	virtual void GetGlobalData(tint32* piData, tint32 iParmStart, tint iSection, tbool bSwap);
	//! IParameterManager override
	virtual void SetGlobalData(tint32 iParmStart, tint32 iParmCount, const tint32* piData, tint iSection, tbool bSwap);

protected:
	CMutex mMutex;

	//! Callback
	IParameterManagerCallback* mpCallback;

	//! Number of global sections in the system
	tint miSectionCount;

	//! Number of layers in the system
	tint miLayerCount;

	//! Number of parts in the system
	tint miPartCount;

	//! Struct which holds "full" control ID, i.e. both control ID and pane ID
	struct SControlID
	{
		SControlID(tint32 iPaneID, tint32 iControlID) : iPaneID(iPaneID), iControlID(iControlID) {}

		tint32 iPaneID;
		tint32 iControlID;

		tbool operator==(const SControlID& ID) const {return ((ID.iPaneID == iPaneID) && (ID.iControlID == iControlID));}

		tbool operator<(const SControlID& ID) const {if (iPaneID < ID.iPaneID) return true; else if (iPaneID > ID.iPaneID) return false;
													else if (iControlID < ID.iControlID) return true; else return false;}
	};

	//! Struct with info about 1 parameter
	struct SParameterInfo {
		//! Pointer to the parameter
		IParameter* pParameter;

		//! List of ID of controls attached to parameter
		std::list<SControlID> ControlIDs;
	};

	//! Pointer to array of pointers to global parameters
	std::vector<SParameterInfo*>* mpGlobalParameters;

	//! Pointer to array of pointers to layer parameters
	std::vector<SParameterInfo*>* mpLayerParameters;

	//! Pointer to array of pointers to part parameters
	std::vector<SParameterInfo*>* mpPartParameters;

	//! Struct used for control map
	struct SControlMapInfo {
		//! Type of parameter
		EParameterType Type;
		//! Global: Ignored. Layer: Layer index of parameter (0-based). Part: Part index of parameter (0-based)
		tint32 iIndex;
		//! Parameter ID
		tint32 iParmID;
	};

	//! Map showing what data is connected to what control. Faster than iterating when there's a lot of parameters
	std::map<SControlID, SControlMapInfo> ControlMap;

	//! Internal version of DataUpdate, with some more parameters
	/*!
		\param pParms [in]: Parameter set to use
		\param iParameterID: ID of parameter that changed
		\param iValueNew: New value of parameter
		\param iPaneExclude: If not -1, ID of pane that should not be updated (pane from which the call originated)
		\param iControlExclude: If not -1, ID of control that should not be updated (control from which the call originated)
		\param bCallSetValue: If true, SetValue is called, otherwise not
	*/
	virtual void DoParameterUpdate(std::vector<SParameterInfo*>* pParms, tint32 iParameterID, tint32 iValueNew, tint32 iPaneExclude = -1, tint32 iControlExclude = -1, tbool bCallSetValue = true);

	//! Internal version of ControlUpdateAll, with parameter for parms to use
	/*!
		\param pParms [in]: Parameter set to use
	*/
	virtual void DoControlUpdateAll(std::vector<SParameterInfo*>* pParms);

	//! Internal version of ParameterUpdateAll, with parameter for parms to use
	/*!
		\param pParms [in]: Parameter set to use
	*/
	virtual void DoParameterUpdateAll(std::vector<SParameterInfo*>* pParms);

	//! Internal version of AutomationUpdateAll, with parameter for parms to use
	/*!
		\param pParms [in]: Parameter set to use
	*/
	virtual void DoAutomationUpdateAll(IParameterManager::EParameterType Type, tint32 iExtra, std::vector<SParameterInfo*>* pParms);
};
