
set(MOD_NAME efd)
set(MOD_DIR Foundation)
set(MOD_INCLUDES ${CMAKE_CURRENT_LIST_DIR}/../)
set(MOD_LIBRARIES efdXML) # TODO: replace with tinyxml2
set(MOD_PCH "efdPCH.h")
set(MOD_DEFINES
    # Debug configuration
    $<$<CONFIG:DEBUG>:EE_CONFIG_DEBUG>;
    $<$<CONFIG:DEBUG>:EE_EFD_CONFIG_DEBUG>;
    $<$<CONFIG:DEBUG>:EE_USE_ASSERT_STACKTRACE>;
    $<$<CONFIG:DEBUG>:EE_USE_MEMORY_MANAGEMENT>;
    # Release configuration (QA)
    $<$<CONFIG:RELEASE>:EE_CONFIG_RELEASE>;
    $<$<CONFIG:RELEASE>:EE_EFD_CONFIG_RELEASE>;
    # Shipping configuration
    $<$<CONFIG:SHIPPING>:EE_CONFIG_SHIPPING>;
    $<$<CONFIG:SHIPPING>:EE_EFD_CONFIG_SHIPPING>;
    $<$<CONFIG:SHIPPING>:EE_DISABLE_LOGGING>;
    # Other configurations
    EE_USE_PCH; # Enable use of PCH
    EE_USE_NATIVE_STL; # Do not use stlport!
    EE_GAMEBRYO_VERSION=7563629; # 7563629 -> MIS
)

# architecture defines
if(GB3_X86)
    list(APPEND MOD_DEFINES -DEE_ARCH_X86=1)
endif()

if(GB3_ARCH64)
    list(APPEND MOD_DEFINES -DEE_ARCH_64=1)
else()
    list(APPEND MOD_DEFINES -DEE_ARCH_32=1)
endif()

# platform defines
if(GB3_WIN32)
    list(APPEND MOD_DEFINES -DEE_PLATFORM_WIN32=1 -DSTRICT=1)
elseif(GB3_LINUX)
    list(APPEND MOD_DEFINES -DEE_PLATFORM_LINUX=1)
elseif(GB3_MACOSX)
    list(APPEND MOD_DEFINES -DEE_PLATFORM_MACOSX=1)
endif()

# compiler defines
if (GB3_MSVC)
    list(APPEND MOD_DEFINES -DEE_COMPILER_MSVC=1)
elseif(GB3_GCC)
    list(APPEND MOD_DEFINES -DEE_COMPILER_GCC=1)
elseif(GB3_CLANG)
    list(APPEND MOD_DEFINES -DEE_COMPILER_CLANG=1)
endif()

if (GB3_WIN32)
    set(MOD_P_LIBRARIES Rpcrt4)
endif()

# generate efd project
generate_project()

# custom add of backend target defines
add_backend_target_defines(efd PUBLIC)

if (GB3_ENABLE_DLL)
    add_backend_target_defines(efdDLL PUBLIC)
endif()
