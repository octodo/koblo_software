
// CPU Feature flags
//! CPU has SSIMD
#define CPUFEATURES_SSE 0x01
//! CPU has SSIMD2
#define CPUFEATURES_SSE2 0x02
//! CPU has 3DNow
#define CPUFEATURES_3DNOW 0x04
//! CPU has Altived
#define CPUFEATURES_ALTIVEC 0x08

class ISystem : public virtual IDestructable
{
public:
	static ISystem* Create();

	enum EOSType {
		OSTypeWin32 = 0,
		OSTypeMacOS
	};

	enum EOSTypeMinor {
		OSMinorTypeWin9x = 0,
		OSMinorTypeWinNT,
		OSMinorTypeMacOSClassic,
		OSMinorTypeMacOSX
	};

	enum ECPUType {
		CPUTypePII = 0,
		CPUTypeCeleron,
		CPUTypePIII,
		CPUTypePIV,
		CPUTypeAthlon,
		CPUTypeG3,
		CPUTypeG4,
		CPUTypeUnknownIntel,
		CPUTypeUnknownPowerPC
	};

	virtual EOSType GetOS() const = 0;

	virtual EOSTypeMinor GetOSMinor() const = 0;

	virtual tint32 GetOSVersionNumberMajor() const = 0;

	virtual tint32 GetOSVersionNumberMinor() const = 0;

	virtual tuint32 GetCPUCount() const = 0;

	virtual ECPUType GetCPU() const = 0;

	virtual tint32 GetCPUFeatures() const = 0;
};
