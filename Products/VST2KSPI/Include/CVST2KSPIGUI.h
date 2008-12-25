

//! Number of frames in VU bitmap
#define VU_NUMBER_OF_FRAMES 65


class CVST2KSPIGUI : public virtual CBaseGUI, public virtual CBasePane, public virtual ge::IEventReceiver
{
public:
	CVST2KSPIGUI(CVST2KSPIPlugIn* pPlugIn, de::IParameterManager* pParmMan);

	virtual ~CVST2KSPIGUI();

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

	//! CBasePane override
	virtual void Init() {}

	//! Returns the plug-in we're connected to
	CVST2KSPIPlugIn* GetPlugIn() {return dynamic_cast<CVST2KSPIPlugIn*>(CBaseGUI::mpPlugIn);}

	//! Overload
	const CVST2KSPIPlugIn* GetPlugIn() const {return dynamic_cast<const CVST2KSPIPlugIn*>(CBaseGUI::mpPlugIn);}

	//! Called by the plug-in when a parameter has changed
	virtual void UpdateEngineData(tint iType, tint iExtra, tint32 iID, tint32 iValue) {}

	virtual void UpdateAll() {}

	virtual void CreateResourceManager() {}

	void KSPI_CALL MakeWindow(void* p);

protected:
	AEffect* mpVSTEffect;

	//! Top pane
	CBaseTopPane* mpTopPane;

	//! CBaseGUI override
	virtual void Initialize();

	//! Parameter/controls linking
	virtual void ConnectControls();

	tbool mbDemoDialogShown;

	tbool mbUnauthorizedDialogShown;

	virtual void OnAbout();

	virtual void OnManual();
};

