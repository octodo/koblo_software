/*!	\file IParameterManager.h
	\author Michael Olsen
	\date 06/Jan/2005
	\date 04/Mar/2005
*/

// Forwards
class IParameterManagerCallback;

/*! \class IParameterManager
 * \brief Interface to describe the parameter manager
 *
 * The parameter manager keeps track of the parameters, and managed data<->GUI updates plus data streaming (presets / projects / sessions)
*/
class IParameterManager : public virtual IDestructable
{
public:
	enum EParameterType {
		//! Global data (system + performance).
		TypeGlobal = 0,
		//! Layer data
		TypeLayer,
		//! Part data (key split zone + preset)
		TypePart
	};

	//! Create IParameterManager
	static IParameterManager* Create();

	//! Sets the callback
	/*!
		\param pCallback: New callback to set. Cannot be NULL. It is imperative to set this before starting to use the manager
	*/
	virtual void SetCallback(IParameterManagerCallback* pCallback) = 0;

	//! Sets the number of global sections in the system
	/*!
		\param iSections [in]: Number of global sections in the system
	*/
	virtual void SetGlobalSectionCount(tint iSections) = 0;

	//! Sets the number of layers in the system
	/*!
		\param iLayers [in]: Number of layers in the system
	*/
	virtual void SetLayerCount(tint iLayers) = 0;

	//! Sets the number of parts in the system
	/*!
		\param iParts [in]: Number of parts in the system
	*/
	virtual void SetPartCount(tint iParts) = 0;

	/*! Adds a parameter. Note that the parameter manager will destroy parameter at destruction time. Also note that all parameters must be added is sequential order (related to their ID's), so that the first is ID 0, the next ID 1, etc.
	/*!
		\param pParameter: Parameter to add.
		\param Type: Type of parameter to add
		\param iExtra: Extra data for non-system parameters. Layer: Layer index (0-based). Part: Part index (0-based). Global: Section index (0-based)
	*/
	virtual void Add(IParameter* pParameter, EParameterType Type = TypeGlobal, tint32 iExtra = 0) = 0;

	/*! Gets a parameter.
	/*!
		\param iParameterID [in]: ID of parameter to get
		\param Type [in]: Type of parameter to get
		\param iExtra [in]: Extra data for non-system parameters. Layer: Layer index (0-based). Part: Part index (0-based). Global: Section index (0-based)
		\return const IParameter*: Parameter, or NULL if doesn't exist
	*/
	virtual const IParameter* Get(tint32 iParameterID, EParameterType Type = TypeGlobal, tint32 iExtra = 0) = 0;

	/*! Sets a new value for a parameter
	/*!
		\param bCallAutomationUpdate [in]: If true automation update is called, otherwise not
		\param iValue [in]: Value to set
		\param iParameterID [in]: ID of parameter to set value for
		\param Type [in]: Type of parameter to set value for
		\param iExtra [in]: Extra data for non-system parameters. Layer: Layer index (0-based). Part: Part index (0-based). Global: Section index (0-based)
		\param bForceUpdate [in]: If true automation update if forced, even if the value has not changed (requires bCallAutomationUpdate == true). If false, update is not forced, but is called if the value has changed (and bCallAutomationUpdate == true).
	*/
	virtual void Set(tbool bCallAutomationUpdate, tint32 iValue, tint32 iParameterID, EParameterType Type = TypeGlobal, tint32 iExtra = 0, tbool bForceUpdate = false) = 0;

	//! Gets the number of global parameters in section
	/*!
		\param iSection [in]: Section to get parameter count for
		\return tint32: Paramters in section
	*/
	virtual tint32 GetParameterCount(tint32 iSection) = 0;

	//! Link a control to a parameter. Note that only 1 parameter per control is allowed (that is, a control can only be linked to 1 parameter). However, multiple controls per parameter is allowed (for instance a knob and a text control)
	/*!
		\param iPaneID: ID of pane which holds control
		\param iControlID: ID of control to link
		\param iParameterID: ID of parameter to link
		\param Type: Type of parameter to add
		\param iExtra: Extra data for non-global parameters. Layer: Layer index (0-based). Part: Part index (0-based). Global: Section index (0-based)
	*/
	virtual void RegisterControl(tint32 iPaneID, tint32 iControlID, tint32 iParameterID, EParameterType Type = TypeGlobal, tint32 iExtra = 0) = 0;

	//! Remove existing link
	/*!
		\param iPaneID: ID of pane which holds control
		\param iControlID: ID of control to remove link for
	*/
	virtual void DeregisterControl(tint32 iPaneID, tint32 iControlID) = 0;

	//! Gets the parameter linked to a control
	/*!
		\param iPaneID: ID of pane which holds control
		\param iControlID: ID of control to get parameter for
		\return IParameter*: Parameter linked to control, or NULL if none
	*/
	virtual IParameter* GetParameterFromControl(tint32 iPaneID, tint32 iControlID) = 0;

	//! Remove all existing links
	virtual void DeregisterAllControls() = 0;

	//! When the GUI has notified a change in a controls data, this should be called
	/*!
		\param iPaneID: ID of pane which holds control
		\param iControlID: Control that changed
		\param iValueNew: New parameter value
		\param bUpdateAll: If true the control that changed will also be updated, otherwise not
	*/
	virtual void ControlUpdate(tint32 iPaneID, tint32 iControlID, tint32 iValueNew, tbool bUpdateAll = false) = 0;

	//! When this is called, the GUI is updated, as if all controls had been changed 
	virtual void ControlUpdateAll() = 0;

	//! When this is called, the parameters are updated, as if all parameters had been changed
	virtual void ParameterUpdateAll() = 0;

	//! When this is called, the parameters are updated, as if all parameters had been changed, but only for given section
	/*!
		\param iSection: Section to update parameters for
	*/
	virtual void GlobalParameterUpdateAll(tint iSection) = 0;

	//! When this is called, automation callback is send for all parameters
	virtual void AutomationUpdateAll() = 0;

	// Returns the number of part data parameters for 1 part
	/*!
		\return tint32: Number of parameters for 1 part
	*/
	virtual tint32 GetPartDataSize() = 0;

	//! Returns part data in a block, swapped for cross-platform if neccasary.
	/*!
		\param iPart [in]: Part to return data for
		\param piData [in]: Block to fill with data. To determine the size, use GetPartDataSize
		\param iParmStart [in]: Parameter to start with
		\param bSwap [in]: If true, parms are swapped for cross-platform
	*/
	virtual void GetPartData(tint32 iPart, tint32* piData, tint32 iParmStart = 0, tbool bSwap = true) = 0;

	//! Sets part data from a block, staring from a specific parameter. Bounds checking is done, so that parameters not present are not set.
	/*!
		\param iPart [in]: Part to set data with
		\param iParmStart [in]: Index of first parameter in block
		\param iParmCount [in]: Number of parameters in block
		\param piData [in]: Block of parameter data to use.
		\param bSwap [in]: If true, parms are swapped for cross-platform
	*/
	virtual void SetPartData(tint32 iPart, tint32 iParmStart, tint32 iParmCount, const tint32* piData, tbool bSwap = true) = 0;

	// Returns the number of layer data parameters for 1 layer
	/*!
		\return tint32: Number of parameters for 1 layer
	*/
	virtual tint32 GetLayerDataSize() = 0;

	//! Returns layer data in a block, swapped for cross-platform if neccasary.
	/*!
		\param iLayer [in]: Layer to return data for
		\param piData [in]: Block to fill with data. To determine the size, use GetLayerDataSize
		\param iParmStart [in]: Parameter to start with
		\param bSwap [in]: If true, parms are swapped for cross-platform
	*/
	virtual void GetLayerData(tint32 iLayer, tint32* piData, tint32 iParmStart = 0, tbool bSwap = true) = 0;

	//! Sets layer data from a block, staring from a specific parameter. Bounds checking is done, so that parameters not present are not set.
	/*!
		\param iLayer [in]: Layer to set data with
		\param iParmStart [in]: Index of first parameter in block
		\param iParmCount [in]: Number of parameters in block
		\param piData [in]: Block of parameter data to use.
		\param bSwap [in]: If true, parms are swapped for cross-platform
	*/
	virtual void SetLayerData(tint32 iLayer, tint32 iParmStart, tint32 iParmCount, const tint32* piData, tbool bSwap = true) = 0;

	// Returns the number of global data parameters for 1 section
	/*!
		\param iSection [in]: Section to return parameter count for
		\return tint32: Number of global parameters for the section
	*/
	virtual tint32 GetGlobalDataSize(tint iSection) = 0;

	//! Returns global data in a block, swapped for cross-platform if neccasary.
	/*!
		\param piData [in]: Block to fill with data. To determine the size, use GetGlobalDataSize
		\param iParmStart [in]: Parameter to start with
		\param iSection [in]: Section to get parameters from
		\param bSwap [in]: If true, parms are swapped for cross-platform
	*/
	virtual void GetGlobalData(tint32* piData, tint32 iParmStart = 0, tint iSection = 0, tbool bSwap = true) = 0;

	//! Sets global data from a block, staring from a specific parameter. Bounds checking is done, so that parameters not present are not set.
	/*!
		\param iParmStart [in]: Index of first parameter in block
		\param iParmCount [in]: Number of parameters in block
		\param piData [in]: Block of parameter data to use.
		\param iSection [in]: Section to set parameters to
		\param bSwap [in]: If true, parms are swapped for cross-platform
	*/
	virtual void SetGlobalData(tint32 iParmStart, tint32 iParmCount, const tint32* piData, tint iSection = 0, tbool bSwap = true) = 0;
};
