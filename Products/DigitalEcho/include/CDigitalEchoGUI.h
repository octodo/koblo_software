

class CDigitalEchoGUI : public virtual CBaseGUI, public virtual ge::IEventReceiver
{
public:
	CDigitalEchoGUI(CDigitalEchoPlugIn* pPlugIn, de::IParameterManager* pParmMan);

	virtual ~CDigitalEchoGUI();

	//! CBaseGUI override
	virtual void KSPI_CALL OnTimer();

	//! IDestructable override
	virtual void KSPI_CALL Destroy();

	//! IGUI override
	virtual void KSPI_CALL GetSize(tuint32* piSizeX, tuint32* piSizeY);

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl *pSender, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl *pSender, void *pEventData);

	//! IContextCallback override
	virtual void GetName(tchar* psz);

	//! Returns the plug-in we're connected to
	CDigitalEchoPlugIn* GetPlugIn() {return dynamic_cast<CDigitalEchoPlugIn*>(mpPlugIn);}

	//! Overload
	const CDigitalEchoPlugIn* GetPlugIn() const {return dynamic_cast<const CDigitalEchoPlugIn*>(mpPlugIn);}

	//! Called by the plug-in when a parameter has changed
	virtual void UpdateEngineData(tint iType, tint iExtra, tint32 iID, tint32 iValue);

	virtual void UpdateAll() {}

protected:
	//! CBaseGUI override
	virtual void Initialize();

	//! Parameter/controls linking
	virtual void ConnectControls();
};

