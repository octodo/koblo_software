
#include "buInternalOS.h"

#pragma warning(disable: 4731)

ISystem* ISystem::Create()
{
	return dynamic_cast<ISystem*>(new CSystem());
}

void CSystem::Destroy()
{
	delete (dynamic_cast<CSystem*>(this));
}

#ifdef WIN32

CSystem::CSystem()
{
	mOSType = OSTypeWin32;

	DetectOS();
	DetectCPU();
}

CSystem::~CSystem()
{
}

// Detects OS and version numbers
void CSystem::DetectOS()
{
	OSVERSIONINFO osVersionInfo;
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osVersionInfo);
	switch(osVersionInfo.dwPlatformId) {
		case VER_PLATFORM_WIN32_WINDOWS:
			mOSTypeMinor = OSMinorTypeWin9x;
			break;
		case VER_PLATFORM_WIN32_NT:
			mOSTypeMinor = OSMinorTypeWinNT;
			break;
	}

	miOSVersion = osVersionInfo.dwMajorVersion;
	miOSVersionMinor = osVersionInfo.dwMinorVersion;
}

// Detected CPU Manufacturers - returned by GetCPUCaps (CPU_MFG);
typedef enum CPU_MFGS
{
    MFG_UNKNOWN,
    MFG_AMD,
    MFG_INTEL,
    MFG_CYRIX,
    MFG_CENTAUR
} CPU_MFGS;

// Detected CPU models - returned by GetCPUCaps (CPU_TYPE);
typedef enum CPU_TYPES
{
    UNKNOWN,
    AMD_Am486,
//    AMD_K5,
    AMD_K6_MMX,
    AMD_K6_2,
    AMD_K6_3,
    AMD_K7,

    INTEL_486DX,
    INTEL_486SX,
    INTEL_486DX2,
    INTEL_486SL,
    INTEL_486SX2,
    INTEL_486DX2E,
    INTEL_486DX4,
    INTEL_Pentium,
    INTEL_Pentium_MMX,
    INTEL_Pentium_Pro,
    INTEL_Pentium_II,
    INTEL_Celeron,
    INTEL_Pentium_III,
	INTEL_Pentium_IV
} CPU_TYPES;

typedef enum CPUCAPS
{
    // Synthesized values
    CPU_MFG,        // Manufacturer (returns enum CPU_MFGS)
    CPU_TYPE,       // CPU type (return enum CPU_TYPES)

    // Processor Features - returned as boolean values
    HAS_CPUID,      // Supports CPUID instruction
    HAS_FPU,        // FPU present
    HAS_VME,        // Virtual Mode Extensions
    HAS_DEBUG,      // Debug extensions
    HAS_PSE,        // Page Size Extensions
    HAS_TSC,        // Time Stamp Counter
    HAS_MSR,        // Model Specific Registers
    HAS_MCE,        // Machine Check Extensions
    HAS_CMPXCHG8,   // CMPXCHG8 instruction
    HAS_MMX,        // MMX support
    HAS_3DNOW,      // 3DNow! support
//    HAS_SSE_MMX,    // SSE MMX support
//    HAS_SSE_FP,     // SSE FP support
    HAS_SSE,         // SSE support
	HAS_SSE2		// SSE2
} CPUCAPS;

unsigned long GetCPUCaps(CPUCAPS cap);

// Detects misc cpu (model, features, count)
void CSystem::DetectCPU()
{
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	muiCPUCount = systemInfo.dwNumberOfProcessors;

	// Assume no features
	miCPUFeatures = 0;

	// Start by detecting the vendor
	CPU_MFGS vendor = (CPU_MFGS)GetCPUCaps(CPU_MFG);

	switch(vendor) {
		case MFG_INTEL:
			{
				mCPUType = CPUTypePII;
				CPU_TYPES type = (CPU_TYPES)GetCPUCaps(CPU_TYPE);
				switch(type) {
					case INTEL_Pentium_II:
						break;
					case INTEL_Celeron:
						mCPUType = CPUTypeCeleron;
						break;
					case INTEL_Pentium_III:
						mCPUType = CPUTypePIII;
						break;
					case INTEL_Pentium_IV:
						mCPUType = CPUTypePIV;
						break;
				}
				unsigned long ret;
				ret = GetCPUCaps(HAS_SSE);
				if (ret) {
					miCPUFeatures |= CPUFEATURES_SSE;
				}
				ret = GetCPUCaps(HAS_SSE2);
				if (ret) {
					miCPUFeatures |= CPUFEATURES_SSE2;
				}
			}
			return;
		case MFG_AMD:
			{
				mCPUType = CPUTypeAthlon;
				CPU_TYPES type = (CPU_TYPES)GetCPUCaps(CPU_TYPE);
				switch(type) {
					case AMD_K7:
						break;
				}
				unsigned long ret;
				ret = GetCPUCaps(HAS_3DNOW);
				if (ret) {
					miCPUFeatures |= CPUFEATURES_3DNOW;
				}
			}
			return;
		case MFG_CYRIX:
		case MFG_CENTAUR:
		case MFG_UNKNOWN:
		default:
			CPUTypeUnknownIntel;
			miCPUFeatures = 0;
			return;
	}
}

// Low-level CPU featues detector.
// Used by DetectCPU
// Modified version from AMD sample application
unsigned long GetCPUCaps(CPUCAPS cap)
{
    DWORD res = 0;

    static DWORD features       = 0;
    static DWORD ext_features   = 0;
    static DWORD processor      = 0;
    static int   init           = 0;

    // Detect CPUID presence once, since all other requests depend on it
    if (init == 0)
    {
        __asm {
            pushfd                  // save EFLAGS to stack.
            pop     eax             // store EFLAGS in EAX.
            mov     edx, eax        // save in EBX for testing later.
            xor     eax, 0x200000   // switch bit 21.
            push    eax             // copy "changed" value to stack.
            popfd                   // save "changed" EAX to EFLAGS.
            pushfd
            pop     eax
            xor     eax, edx        // See if bit changeable.
            jnz     short foundit   // if so, mark 
            mov     eax,-1
            jmp     short around

            ALIGN   4
        foundit:
            // Load up the features and (where appropriate) extended features flags
            mov     eax,1               // Check for processor features
			push ebx
            CPUID
			pop ebx
            mov     [features],edx      // Store features bits
			mov     [processor],eax     // In case we don't support extended features, this is our processor info
            mov     eax,0x80000000      // Check for support of extended functions.
			push ebx
            CPUID
			pop ebx
            cmp     eax,0x80000001      // Make sure function 0x80000001 supported.
            jb      short around
            mov     eax,0x80000001      // Select function 0x80000001
			push ebx
            CPUID
			pop ebx
            mov     [processor],eax     // Store processor family/model/step
            mov     [ext_features],edx  // Store extende features bits
            mov     eax,1               // Set "Has CPUID" flag to true

        around:
            mov     [init],eax
        }
    }
    if (init == -1)
    {
        // No CPUID, so no CPUID functions are supported
        return 0;
    }

    // Otherwise, perform the requested tests
    switch (cap)
    {
    // Synthesized Capabilities
    case HAS_CPUID:
        // Always true if this code gets executed
        res = 1;
        break;

    case CPU_MFG:
        __asm {
			push ebx
            // Query manufacturer string
            mov     eax,0           // function 0 = manufacturer string
            CPUID

            // These tests could probably just check the 'ebx' part of the string,
            // but the entire string is checked for completeness.  Plus, this function
            // should not be used in time-critical code, because the CPUID instruction
            // serializes the processor. (That is, it flushes out the instruction pipeline.)

            // Test for 'AuthenticAMD'
            cmp     ebx,'htuA'
            jne     short not_amd
            cmp     edx,'itne'
            jne     short not_amd
            cmp     ecx,'DMAc'
            jne     short not_amd
            mov     eax,MFG_AMD
            jmp     short next_test
 
            // Test for 'GenuineIntel'
         not_amd:
            cmp     ebx,'uneG'
            jne     short not_intel
            cmp     edx,'Ieni'
            jne     short not_intel
            cmp     ecx,'letn'
            jne     short not_intel
            mov     eax,MFG_INTEL
            jmp     short next_test
 
            // Test for 'CyrixInstead'
         not_intel:
            cmp     ebx,'iryC'
            jne     short not_cyrix
            cmp     edx,'snIx'
            jne     short not_cyrix
            cmp     ecx,'deat'
            jne     short not_cyrix
            mov     eax,MFG_CYRIX
            jmp     short next_test
 
            // Test for 'CentaurHauls'
         not_cyrix:
            cmp     ebx,'tneC'
            jne     short not_centaur
            cmp     edx,'Hrua'
            jne     short not_centaur
            cmp     ecx,'slua'
            jne     short not_centaur
            mov     eax,MFG_CENTAUR
            jmp     short next_test
 
         not_centaur:
            mov     eax,MFG_UNKNOWN
 
         next_test:
			pop ebx
           mov     [res],eax       // store result of previous tests
        }
        break;

    case CPU_TYPE:
        // Return a member of the CPUTYPES enumeration
        // Note: do NOT use this for determining presence of chip features, such
        // as MMX and 3DNow!  Instead, use GetCPUCaps (HAS_MMX) and GetCPUCaps (HAS_3DNOW),
        // which will accurately detect the presence of these features on all chips which
        // support them.
        switch (GetCPUCaps (CPU_MFG))
        {
        case MFG_AMD:
            switch ((processor >> 8) & 0xf) // extract family code
            {
            case 4: // Am486/AM5x86
                res = AMD_Am486;
                break;

            case 5: // K6
/*                switch ((processor >> 4) & 0xf) // extract model code
                {
                case 0: res = AMD_K5;       break;
                case 1: res = AMD_K5;       break;
                case 2: res = AMD_K5;       break;
                case 3: res = AMD_K5;       break;
                case 4: res = AMD_K6_MMX;   break;
                case 5: res = AMD_K6_MMX;   break;
                case 6: res = AMD_K6_MMX;   break;
                case 7: res = AMD_K6_MMX;   break;
                case 8: res = AMD_K6_2;     break;
                case 9: res = AMD_K6_3;     break;
                }*/
				res = 0;
                break;

            case 6: // K7 Athlon
                res = AMD_K7;
                break;
			default:
				res = AMD_K7;
				break;
            }
            break;

        case MFG_INTEL:
            switch ((processor >> 8) & 0xf) // extract family code
            {
            case 4:
                switch ((processor >> 4) & 0xf) // extract model code
                {
                case 0: res = INTEL_486DX;  break;
                case 1: res = INTEL_486DX;  break;
                case 2: res = INTEL_486SX;  break;
                case 3: res = INTEL_486DX2; break;
                case 4: res = INTEL_486SL;  break;
                case 5: res = INTEL_486SX2; break;
                case 7: res = INTEL_486DX2E;break;
                case 8: res = INTEL_486DX4; break;
                }
                break;

            case 5:
                switch ((processor >> 4) & 0xf) // extract model code
                {
                case 1: res = INTEL_Pentium;    break;
                case 2: res = INTEL_Pentium;    break;
                case 3: res = INTEL_Pentium;    break;
                case 4: res = INTEL_Pentium_MMX;break;
                }
                break;

            case 6:
                switch ((processor >> 4) & 0xf) // extract model code
                {
                case 1: res = INTEL_Pentium_Pro;break;
                case 3: res = INTEL_Pentium_II; break;
                case 5: res = INTEL_Pentium_II; break;  // actual differentiation depends on cache settings
                case 6: res = INTEL_Celeron;    break;
                case 7: res = INTEL_Pentium_III;break;  // actual differentiation depends on cache settings
                }
                break;
			default:
				res = INTEL_Pentium_IV;
				break;
            }
            break;

        case MFG_CYRIX:
            res = UNKNOWN;
            break;

        case MFG_CENTAUR:
            res = UNKNOWN;
            break;
        }
        break;

    // Feature Bit Test Capabilities
    case HAS_FPU:       res = (features >> 0) & 1;      break;  // bit 0 = FPU
    case HAS_VME:       res = (features >> 1) & 1;      break;  // bit 1 = VME
    case HAS_DEBUG:     res = (features >> 2) & 1;      break;  // bit 2 = Debugger extensions
    case HAS_PSE:       res = (features >> 3) & 1;      break;  // bit 3 = Page Size Extensions
    case HAS_TSC:       res = (features >> 4) & 1;      break;  // bit 4 = Time Stamp Counter
    case HAS_MSR:       res = (features >> 5) & 1;      break;  // bit 5 = Model Specific Registers
    case HAS_MCE:       res = (features >> 6) & 1;      break;  // bit 6 = Machine Check Extensions
    case HAS_CMPXCHG8:  res = (features >> 7) & 1;      break;  // bit 7 = CMPXCHG8 instruction
    case HAS_MMX:       res = (features >> 23) & 1;     break;  // bit 23 = MMX
    case HAS_SSE:
		{
			res = (features >> 25) & 1;
			if (res) {
				// SSIMD requires support from the operating system, so we
				// try to execute an SSIMD instruction, and check whether an
				// exception is thrown
				try {
					// orps xmm0, xmm0
					__asm _emit 0x0f
					__asm _emit 0x56
					__asm _emit 0xC0
				}
				catch(...) {
					res = 0;
				}
			}
		}
		break;
    case HAS_SSE2:
		{
			res = (features >> 26) & 1;
			if (res) {
				// SSIMD2 requires support from the operating system, so we
				// try to execute an SSIMD2 instruction, and check whether an
				// exception is thrown
				try {
/*					static __declspec(align(16)) __m128i a;
					static __declspec(align(16)) __m128i b;
					static __declspec(align(16)) __m128i c;
					c = _mm_or_si128(a, b);*/
					res = 0;
				}
				catch(...) {
					res = 0;
				}
			}
		}
		break;
    case HAS_3DNOW:     res = (ext_features >> 31) & 1; break;  // bit 31 (ext) = 3DNow!
    }

    return res;
}


#endif	// WIN32

#ifdef _Mac

#include <MacTypes.h>
#include <Errors.h>
#include <Gestalt.h>
#include <Traps.h>

CSystem::CSystem()
{
	OSErr err;
	long lProcessorAttributes;

 	mOSType = OSTypeMacOS;
	mOSTypeMinor = OSMinorTypeMacOSX;

	miOSVersion = 0;
	miOSVersionMinor = 0;

	muiCPUCount = 1;	
	mCPUType = CPUTypeG3;

	miCPUFeatures = 0;

	err = Gestalt(gestaltPowerPCProcessorFeatures, &lProcessorAttributes);

	if (err != noErr) {
		// Failed to examine hardware
	}

	if ((1 << gestaltPowerPCHasVectorInstructions) & lProcessorAttributes) {
		miCPUFeatures |= CPUFEATURES_ALTIVEC;
		mCPUType = CPUTypeG4;
	}
}

CSystem::~CSystem()
{
}

#endif	// _Mac

