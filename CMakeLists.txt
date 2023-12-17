# Copyright (C) 2022-2023 Made In Server Development team
# GameBryo3 Project

cmake_minimum_required(VERSION 3.20)
project(Gamebryo3)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_LIST_DIR}/Source/CMake
)

option(GB3_ENABLE_UNIT_TESTS "Enable unit tests for migrations" ON)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    if ("${CMAKE_GENERATOR}" STREQUAL "Unix Makefiles" OR "${CMAKE_GENERATOR}" STREQUAL "Ninja")
        message(FATAL_ERROR "Please specify a build type")
    endif()
endif()

set(GB3_ROOT "${CMAKE_CURRENT_LIST_DIR}")
set(GB3_INTEGRATION_ROOT "${GB3_ROOT}/Integrations")

if (NOT DEFINED GB3_FOLDER_SUFFIX)
    set(GB3_FOLDER_SUFFIX "")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH};${GB3_INTEGRATION_ROOT})

# architecture detection
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(GB3_ARCH64 TRUE)
    set(GB3_ARCH 64)
else()
    set(GB3_ARCH32 TRUE)
    set(GB3_ARCH 32)
endif()

# platform detection
if (UNIX AND NOT APPLE)
    set(GB3_LINUX TRUE)
    set(GB3_PLATFORM_NAME "Linux")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(GB3_MACOSX TRUE)
    set(GB3_PLATFORM_NAME "MacOS X")
elseif (WIN32)
    set(GB3_WIN32 TRUE)
    set(GB3_PLATFORM_NAME "Windows")

    if (CYGWIN OR MSYS)
        message(FATAL_ERROR "Cygwin/Msys2 is not supported")
    endif()
else()
    message(FATAL_ERROR "Platform not supported")
endif()

# compiler detection
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(GB3_CLANG TRUE)
    set(GB3_COMPILER_NAME "Clang")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(GB3_GCC TRUE)
    set(GB3_COMPILER_NAME "GCC")
elseif (MSVC)
    set(GB3_MSVC TRUE)
    set(GB3_COMPILER_NAME "MSVC")
else()
    message(FATAL_ERROR "Compiler not supported")
endif()

# processor detection
if ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "AMD64" OR
    "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64" OR
    "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "i386" OR
    "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86"    
)
    set(GB3_X86 TRUE)
    set(GB3_CPU_NAME "x86")
else()
    message(FATAL_ERROR "Processor not supported (${CMAKE_SYSTEM_PROCESSOR})")
endif()

# nasm
if (GB3_X86)
    enable_language(ASM_NASM)

    if (GB3_ARCH64)
        set(GB3_ASM_SUFFIX "_x86_64.asm")
    else()
        set(GB3_ASM_SUFFIX "_i386.asm")
    endif()
else()
    # Disable assembly
    set(GB3_ASM_SUFFIX "")
endif()

# Default backend
if (NOT DEFINED GB3_TARGET_BACKEND OR "${GB3_TARGET_BACKEND}" STREQUAL "")
    set(GB3_TARGET_BACKEND "SDL2")
    set(GB3_BACKEND_ROOT "${GB3_ROOT}/Source")
    find_package(SDL2 CONFIG REQUIRED)
endif()

include(Backend)
include(Integration)
include(Swig)
include(Version)
include(Project)

# required packages
find_package(OpenEXR CONFIG REQUIRED)
find_package(PNG REQUIRED)
#find_package(tinyxml2 CONFIG REQUIRED)

# for examples in Win32
set(NISETTINGDIALOG_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/Source/CoreRuntime/UtilityLibs/NiSettingsDialog/NiSettingsDialogResource.rc")

# bootstrap extensions
bootstrap_backends()
bootstrap_integrations()

message(STATUS "---------------------------------")
message(STATUS "Gamebryo: ${GB3_VERSION_MAJOR}.${GB3_VERSION_MINOR}.${GB3_VERSION_PATCH}.${GB3_VERSION_BUILD} (${GB3_BUILD_DAY}/${GB3_BUILD_MONTH}/${GB3_BUILD_YEAR})")
message(STATUS "Platform: ${GB3_PLATFORM_NAME} ${GB3_CPU_NAME} ${GB3_ARCH}-bit")
message(STATUS "Compiler: ${GB3_COMPILER_NAME}")
message(STATUS "Backend: ${GB3_TARGET_BACKEND}")
message(STATUS "Integrations:")
print_integrations()
message(STATUS "---------------------------------")

add_subdirectory(Source)

# perform extra backend executions
post_backend_operations()
