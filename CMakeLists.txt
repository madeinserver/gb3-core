# Copyright (C) 2022-2023 Made In Server Development team
# GameBryo3 Linux Project

cmake_minimum_required(VERSION 3.20)
project(Gamebryo3)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH};${CMAKE_CURRENT_LIST_DIR}/Source/CMake)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    if ("${CMAKE_GENERATOR}" STREQUAL "Unix Makefiles" OR "${CMAKE_GENERATOR}" STREQUAL "Ninja")
        message(FATAL_ERROR "Please specify a build type")
    endif()
endif()

set(GB3_ROOT "${CMAKE_CURRENT_LIST_DIR}")

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

if (NOT GB3_LINUX AND NOT GB3_MACOSX AND NOT WIN32)
endif()


include(FindFBXSDK)
include(Integration)
include(Swig)
include(Version)

find_package(PNG REQUIRED)
find_package(OpenEXR CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
#find_package(tinyxml2 CONFIG REQUIRED)

# for examples
set(NISETTINGDIALOG_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/Source/CoreRuntime/UtilityLibs/NiSettingsDialog/NiSettingsDialogResource.rc")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(GB3_ARCH 64)
else()
    set(GB3_ARCH 32)
endif()

bootstrap_integrations()

message(STATUS "---------------------------------")
message(STATUS "Gamebryo: ${GB3_VERSION_MAJOR}.${GB3_VERSION_MINOR}.${GB3_VERSION_PATCH}.${GB3_VERSION_BUILD} (${GB3_BUILD_DAY}/${GB3_BUILD_MONTH}/${GB3_BUILD_YEAR})")
message(STATUS "Platform: ${GB3_PLATFORM_NAME} ${GB3_ARCH}-bit")
message(STATUS "Compiler: ${GB3_COMPILER_NAME}")
message(STATUS "Integrations:")
message(STATUS "---------------------------------")

add_subdirectory(Source)
