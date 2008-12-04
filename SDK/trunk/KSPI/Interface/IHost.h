/*! \file IHost.h
	\author Michael Olsen
	\date 24/Jan/2005
	\date 24/Jan/2005
*/

/*! \brief Host callback interface
*/
class IHost
{
public:
	static IHost* IHost::Create(kspiCCallbackDispatcherFunc pCallback, void* hHost);

	//! The plug-in calls this to notify autumation update (changes caused by user interaction)
	virtual void KSPI_CALL OnAutomationUpdate(IPlugIn* pCaller, tint32 iParameterIndex, tint32 iValue) = 0;

	//! The plug-in calls this to ask about the BPM. It should not be called from inside processing or MIDI receival
	/*!
		return tfloat: Current BPM, or 0 if unknown
	*/
	virtual tfloat32 KSPI_CALL GetCurrentBPM() = 0;

	//! The host type determines what format the host wrapper is.
	enum EType {
		//! Stand-alone, i.e. application.
		HostTypeStandAlone = 0,
		//! VST
		HostTypeVST,
		//! RTAS
		HostTypeRTAS,
		//! AS
		HostTypeAS,
		//! TDM
		HostTypeTDM,
		//! AU
		HostTypeAU,

		//! Not used. Present to force enum to 32-bit
		HostTypeForce = 0xffffffff
	};

	//! The plug-in calls this to ask about the host type
	/*!
		return EType: Format of host wrapper
	*/
	virtual EType KSPI_CALL GetType() = 0;

	//! Tells the host to switch to a specific window
	virtual void KSPI_CALL ActivateWindow(tint32 iIndex) = 0;
	
	//! Tells the host to hide to a specific window
	virtual void KSPI_CALL HideWindow(tint32 iIndex) = 0;
	
	//! Asks the host whether a specific window is visible
	virtual tint32 KSPI_CALL IsWindowVisible(tint32 iIndex) = 0;

	//! Tells the host that the (buffer-drawing) window needs to be redrawn within given rectangle
	virtual void KSPI_CALL RedrawWindow(IGUI* pCaller, tint32 iX, tint32 iY, tint32 iCX, tint32 iCY) = 0;

	//! Asks the host if a given window should have a custom toolbar, of if one is already supplied by the host
	virtual tint32 KSPI_CALL DoesWindowHaveToolbar(tint32 iIndex) = 0;
};

class CHost : public virtual IHost
{
public:
	CHost(void* pCallback, void* hHost);

	~CHost();

	virtual void Destroy();

	//! IHost override
	virtual void KSPI_CALL OnAutomationUpdate(IPlugIn* pCaller, tint32 iParameterIndex, tint32 iValue);
	//! IHost override
	virtual tfloat32 KSPI_CALL GetCurrentBPM();
	//! IHost override
	virtual EType KSPI_CALL GetType();
	//! IHost override
	virtual void KSPI_CALL ActivateWindow(tint32 iIndex);
	//! IHost override
	virtual void KSPI_CALL HideWindow(tint32 iIndex);
	//! IHost override
	virtual tint32 KSPI_CALL IsWindowVisible(tint32 iIndex);
	//! IHost override
	virtual void KSPI_CALL RedrawWindow(IGUI* pCaller, tint32 iX, tint32 iY, tint32 iCX, tint32 iCY);
	//! IHost override
	virtual tint32 KSPI_CALL DoesWindowHaveToolbar(tint32 iIndex);

protected:
	kspiCCallbackDispatcherFunc mpCallback;
	void* mhHost;
};
