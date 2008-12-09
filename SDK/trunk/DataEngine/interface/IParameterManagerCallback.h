/*!	\file IParameterManagerCallback.h
	\author Michael Olsen
	\date 06/Jan/2005
	\date 06/Jan/2005
*/


/*! \class IParameterManagerCallback
 * \brief Interface to describe the callback used by the parameter manager
 *
 * Since the parameter manager knows nothing about the GUI system, a callback system is needed. It is also used to notify when the host (for a plug-in) should be notified regarding automation
*/

class IParameterManagerCallback
{
public:
	//! Control with given ID should be updated in the GUI
	/*!
		\param iPaneID: Pane to update in GUI
		\parm iControlID: Control to update in GUI
		\parm iValue: New value for control
	*/
	virtual void GUINotify(tint32 iPaneID, tint32 iControlID, tint32 iValue) = 0;

	/*! Parameter with given ID has been changed, and automation info should be updated (or host notified)
	/*!
		\parm Type: Type of parameter
		\parm iExtra: Global: Ignored. Layer: layer number. Part: part number
		\parm iParameterID: Parameter that changed
		\parm iValue: New value for parameter
	*/
	virtual void AutomationNotify(IParameterManager::EParameterType Type, tint32 iExtra, tint32 iParameterID, tint32 iValue) = 0;
};