

// Dette er vores "modul". Et modul svarer groft sagt til en .dll, og kan indeholde 1 eller flere plug-ins.
// Det vil sige at hvis det er det man vil kan man have en hele serie af plug-ins i en kspi.
// Module filer skal normalt kun ændres minimalt fra gang til gang.
class CVST2KSPIModule : public virtual CBaseModule
{
public:
	CVST2KSPIModule(kspi::IHost* pHost);

	virtual ~CVST2KSPIModule();

	//! IModule override
	virtual void KSPI_CALL Destroy();

	//! IModule override
	virtual kspi::IPlugIn* KSPI_CALL CreateProcess(tuint32 iIndex);

	//! CBaseModule override
	virtual tint32 KSPI_CALL GetCompanyID();
	//! CBaseModule override
	virtual void KSPI_CALL GetCompanyName(tchar* pszName);
	//! CBaseModule override
	virtual tuint32 KSPI_CALL GetNrOfPlugIns();

	virtual void Init();

protected:
	//! List of effects found
	std::list<std::string> msEffectPathNames;
};