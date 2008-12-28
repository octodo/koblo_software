

class CDigitalEchoModule : public virtual CBaseModule
{
public:
	CDigitalEchoModule(kspi::IHost* pHost);

	virtual ~CDigitalEchoModule();

	//! IModule override
	virtual void KSPI_CALL Destroy();

	//! IModule override
	virtual kspi::IPlugIn* KSPI_CALL CreateProcess(tuint32 iIndex);

	//! CBaseModule override
	virtual tint32 KSPI_CALL GetCompanyID();

	//! CBaseModule override
	virtual void KSPI_CALL GetCompanyName(tchar* pszName);

protected:
};