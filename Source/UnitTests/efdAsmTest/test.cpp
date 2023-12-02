#include "../efd/SystemDesc.h"
#include "../efd/MemManager.h"
#include "../efd/DefaultInitializeMemoryManager.h"
#include "../efd/EEMath.h"
#undef main

EE_USE_DEFAULT_ALLOCATOR;

int main()
{
    auto sd = efd::SystemDesc();

    efd::Float32 sin, cos;
    efd::SinCos(45.0f, sin, cos);

    printf(
        "sin: %f\n"
        "cos: %f\n"
        "Little endian:%d\n"
        "Physical processor count:%d\n"
        "Physical core count:%d\n"
        "Logical processor count:%d\n"
        "Platform ID:%d\n"
        "Performance counter HZ:%f\n"
#ifdef EE_ARCH_X86
        "MMX support:%d\n"
        "SSE support:%d\n"
        "SSE2 support:%d\n"
#endif
#if 1
        "CPUID Init:%d\n"
        "CPUID CpuIDSupported:%d\n"
        "CPUID GenuineIntel:%d\n"
        "CPUID HWD_MTSupported:%d\n"
        "CPUID MMX_Supported:%d\n"
        "CPUID SSE_Supported:%d\n"
        "CPUID SSE2_Supported:%d\n"
        "CPUID MaxLogicalProcPerPhysicalProc:%d\n"
        "CPUID MaxCorePerPhysicalProc:%d\n"
        "CPUID CheckCPU_ExtendedFamilyModel:%d\n"
#endif
        ,
        sin,
        cos,
        sd.IsLittleEndian(),
        sd.GetPhysicalProcessorCount(),
        sd.GetPhysicalCoreCount(),
        sd.GetLogicalProcessorCount(),
        (int)sd.GetPlatformID(),
        sd.GetPerformanceCounterHz(),
#ifdef EE_ARCH_X86
        sd.MMX_Supported(),
        sd.SSE2_Supported(),
        sd.SSE_Supported()
#endif
#if 1
        ,sd.CPUID_Init(),
        sd.CPUID_CpuIDSupported(),
        sd.CPUID_GenuineIntel(),
        sd.CPUID_HWD_MTSupported(),
        sd.CPUID_MMX_Supported(),
        sd.CPUID_SSE_Supported(),
        sd.CPUID_SSE2_Supported(),
        sd.CPUID_MaxLogicalProcPerPhysicalProc(),
        sd.CPUID_MaxCorePerPhysicalProc(),
        sd.CPUID_CheckCPU_ExtendedFamilyModel()
#endif
    );

    return 0;
}
