set(GB3_VERSION_MAJOR 3)
set(GB3_VERSION_MINOR 2)
set(GB3_VERSION_PATCH 0)
set(GB3_VERSION_BUILD 661)

set(GB3_BUILD_DAY 24)
set(GB3_BUILD_MONTH 10)
set(GB3_BUILD_YEAR 2010)

configure_file(
    ${GB3_ROOT}/Source/CoreRuntime/EngineLibs/NiSystem/NiVersion.h.in
    ${GB3_ROOT}/Source/CoreRuntime/EngineLibs/NiSystem/NiVersion.h
)
