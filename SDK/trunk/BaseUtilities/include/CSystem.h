
class CSystem : public virtual ISystem
{
public:
	CSystem();
	virtual ~CSystem();

	//! ISystem override
	virtual void Destroy();
	//! ISystem override
	virtual EOSType GetOS() const {return mOSType;}
	//! ISystem override
	virtual EOSTypeMinor GetOSMinor() const {return mOSTypeMinor;}
	//! ISystem override
	virtual tint32 GetOSVersionNumberMajor() const {return miOSVersion;}
	//! ISystem override
	virtual tint32 GetOSVersionNumberMinor() const {return miOSVersionMinor;}
	//! ISystem override
	virtual tuint32 GetCPUCount() const {return muiCPUCount;}
	//! ISystem override
	virtual ECPUType GetCPU() const {return mCPUType;}
	//! ISystem override
	virtual tint32 GetCPUFeatures() const {return miCPUFeatures;};

protected:
	EOSType mOSType;

	EOSTypeMinor mOSTypeMinor;

	tint32 miOSVersion;

	tint32 miOSVersionMinor;

	tuint32 muiCPUCount;

	ECPUType mCPUType;

	tint32 miCPUFeatures;

#ifdef WIN32
	virtual void DetectOS();
	virtual void DetectCPU();
#endif	// WIN32
};
