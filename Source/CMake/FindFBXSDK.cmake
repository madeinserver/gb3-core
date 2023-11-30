#
# Helper function for finding the FBX SDK.
#
# sets: FBXSDK_FOUND, 
#       FBXSDK_DIR, 
#       FBXSDK_LIBRARY, 
#       FBXSDK_LIBRARY_DEBUG
#       FBXSDK_INCLUDE_DIR
#
set(_fbxsdk_version "2020.3.1")
set(_fbxsdk_vstudio_version "vs2019")

message(STATUS "Looking for FBX SDK version: ${_fbxsdk_version}")

if (APPLE)
    set(_fbxsdk_approot "/Applications/Autodesk/FBX SDK")
    set(_fbxsdk_libdir_debug "lib/clang/debug")
    set(_fbxsdk_libdir_release "lib/clang/release")
    set(_fbxsdk_libname_debug "libfbxsdk.a")
    set(_fbxsdk_libname_release "libfbxsdk.a")
elseif (WIN32)
    # the $ENV{PROGRAMFILES} variable doesn't really work since there's no 
    # 64-bit cmake version
    set(_fbxsdk_approot "C:/Program Files/Autodesk/FBX/FBX SDK")
    if (FIPS_WIN32)
        set(_fbxsdk_libdir_debug "lib/${_fbxsdk_vstudio_version}/x86/debug")
        set(_fbxsdk_libdir_release "lib/${_fbxsdk_vstudio_version}/x86/release")
        set(_fbxsdk_libname_debug "libfbxsdk-mt.lib")
        set(_fbxsdk_libname_release "libfbxsdk-mt.lib")
        set(FBXSDK_FOUND YES)
    else()
        set(_fbxsdk_libdir_debug "lib/${_fbxsdk_vstudio_version}/x64/debug")
        set(_fbxsdk_libdir_release "lib/${_fbxsdk_vstudio_version}/x64/release")
        set(_fbxsdk_libname_debug "libfbxsdk-mt.lib")
        set(_fbxsdk_libname_release "libfbxsdk-mt.lib")
        set(FBXSDK_FOUND YES)
    endif()
else()
    if (NOT DEFINED FBXSDK_DIR)
        message(STATUS "FBXSDK_DIR not set, disabling FBX functionalities...")
    endif()

    set(FBXSDK_FOUND NO)
endif()

if (FBXSDK_FOUND)
    if (NOT DEFINED FBXSDK_DIR)
        set(_fbxsdk_root "${_fbxsdk_approot}/${_fbxsdk_version}")
    else()
        set(_fbxsdk_root "${FBXSDK_DIR}")
    endif()

    # should point the the FBX SDK installation dir
    message(STATUS "_fbxsdk_root: ${_fbxsdk_root}")

    # find header dir and libs
    find_path(FBXSDK_INCLUDE_DIR "fbxsdk.h" 
            PATHS ${_fbxsdk_root} 
            PATH_SUFFIXES "include")
    message(STATUS "FBXSDK_INCLUDE_DIR: ${FBXSDK_INCLUDE_DIR}")
    find_library(FBXSDK_LIBRARY ${_fbxsdk_libname_release}
                PATHS ${_fbxsdk_root}
                PATH_SUFFIXES ${_fbxsdk_libdir_release})
    message(STATUS "FBXSDK_LIBRARY: ${FBXSDK_LIBRARY}")
    find_library(FBXSDK_LIBRARY_DEBUG ${_fbxsdk_libname_debug}
                PATHS ${_fbxsdk_root}
                PATH_SUFFIXES ${_fbxsdk_libdir_debug})
    message(STATUS "FBXSDK_LIBRARY_DEBUG: ${FBXSDK_LIBRARY_DEBUG}")

    if (FBXSDK_INCLUDE_DIR AND FBXSDK_LIBRARY AND FBXSDK_LIBRARY_DEBUG)
        set(FBXSDK_FOUND YES)
    else()
        set(FBXSDK_FOUND NO)
    endif()
endif()
