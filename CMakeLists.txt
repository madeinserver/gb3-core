# Copyright (C) 2022-2023 Made In Server Development team
# GameBryo3 Linux Project

cmake_minimum_required(VERSION 3.20)
project(Gamebryo3)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH};${CMAKE_CURRENT_LIST_DIR}/Source/CMake)

include(FindFBXSDK)
include(Integration)
include(Swig)

find_package(PNG REQUIRED)
find_package(OpenEXR CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

# for examples
set(NISETTINGDIALOG_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/Source/CoreRuntime/UtilityLibs/NiSettingsDialog/NiSettingsDialogResource.rc")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(GB3_ARCH 64)
else()
    set(GB3_ARCH 32)
endif()

bootstrap_integrations()

add_subdirectory(Source)
