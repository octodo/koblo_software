
class CDigitalEchoPane : public virtual CBasePane
{
public:
	CDigitalEchoPane(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CDigitalEchoPane();

	virtual void Init();

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl* pControl, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl* pControl, void *pEventData) {}

	virtual void ConnectControls();

protected:
};

