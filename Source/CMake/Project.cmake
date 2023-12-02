# Generates Gamebryo projects
# Copyright (c) 2023 Arves100/Made In Server Developers.
#
#
# Module settings:
# MOD_NAME: name of the base module to build
# MOD_DIR: name of the directory of this module
# MOD_EXTRA_SRC_PATH: extra source paths included in the build
# MOD_PCH: Precompiled header name
#
# Target configuration:
# MOD_IS_EXE: TRUE if it's an executable target
# MOD_IS_CLI: TRUE if it's a command line executable
# MOD_NO_DLL: disables generation of a shared module for this module
#
# Public defines:
# MOD_DEFINES: module definitions
# MOD_INCLUDES: module inclusions
# MOD_LIBRARIES: module libraries (do not include gamebryo libraries here!)
# MOD_LIBDIRS: module library directories
# MOD_GB_LIBS: module gamebryo libraries
#
# Private defines:
# MOD_P_DEFINES: module definitions
# MOD_P_INCLUDES: module inclusions
# MOD_P_LIBRARIES: module libraries (do not include gamebryo libraries here!)
# MOD_P_LIBDIRS: module library directories
# MOD_P_GB_LIBS: module gamebryo libraries
#
# **OUTPUT:** Generated outputs:
# MOD_SRC_PATHS: directory where all the sources are includes
# MOD_SRC_FILTERS: empty variable that used to contain all filters of the directories
# MOD_SOURCES: all loaded sources
# MOD_FULL_PATH: path of the current module (from Source/)
# MOD_NAME_UP: uppercase project name
#

macro(generate_project)
    set(MOD_SRC_PATHS "${CMAKE_CURRENT_LIST_DIR}")
    set(MOD_SRC_FILTERS "Source")
    set(MOD_FULL_PATH "${MOD_DIR}/${MOD_NAME}")
    set(MOD_SOURCES "")

    string(TOUPPER ${MOD_NAME} MOD_NAME_UP)

    if (MOD_IS_EXE) 
        set(MOD_NO_DLL TRUE) # disable dll if it's an exe
    endif()

    # iterate all project sources and add them
    if (EXISTS "${GB3_BACKEND_ROOT}/${MOD_FULL_PATH}/${GB3_TARGET_BACKEND}")
        list(APPEND MOD_SRC_PATHS "${GB3_BACKEND_ROOT}/${MOD_FULL_PATH}/${GB3_TARGET_BACKEND}")
        list(APPEND MOD_SRC_FILTERS "${GB3_TARGET_BACKEND}")
    endif()

    foreach (SRC_PATH IN ITEMS ${MOD_EXTRA_SRC_PATH})
        list(APPEND MOD_SRC_PATHS "${CMAKE_CURRENT_LIST_DIR}/${SRC_PATH}")
        list(APPEND MOD_SRC_FILTERS "${SRC_PATH}")
    endforeach()

    foreach (SRC_PATH IN ITEMS ${MOD_SRC_PATHS})
        file(GLOB SRCS
            "${SRC_PATH}/*.cpp"
            "${SRC_PATH}/*.h"
            "${SRC_PATH}/*.hpp"
            "${SRC_PATH}/*.c"
            "${SRC_PATH}/*.res"
            "${SRC_PATH}/*.inl"
        )

        if (NOT "${GB3_ASM_SUFFIX}" STREQUAL "") # append assembly files if any
            file(GLOB ASM_SRCS "${SRC_PATH}/*${GB3_ASM_SUFFIX}")
            list(APPEND SRCS ${ASM_SRCS})
        endif()

        list(POP_FRONT MOD_SRC_FILTERS SRC_FILTER)
        source_group("${SRC_FILTER}" FILES ${SRCS})

        list(APPEND MOD_SOURCES ${SRCS})
    endforeach()

    source_group("Precompile Header File" FILES "${CMAKE_CURRENT_BINARY_DIR}/cmake_pch.cxx")

    if (MOD_IS_EXE)
        if (NOT MOD_IS_CLI)
            set(MOD_EXTRA_EXE_DEF WIN32) # for setting /SUBSYSTEM:WINDOWS to avoid console window spawn
        endif()

        add_executable(${MOD_NAME} ${MOD_EXTRA_EXE_DEF} ${MOD_SOURCES})
    else()
        add_library(${MOD_NAME} STATIC ${MOD_SOURCES})
    endif()
    
    set_target_properties(${MOD_NAME} PROPERTIES FOLDER "${MOD_PREFIX}${MOD_DIR}")
    target_compile_definitions(${MOD_NAME} PUBLIC -DEE_${MOD_NAME_UP}_NO_IMPORT=1) # dllspec define

    if (DEFINED MOD_PCH)
        target_precompile_headers(${MOD_NAME} PRIVATE ${MOD_PCH})
        target_compile_definitions(${MOD_NAME} PRIVATE -DNI_USE_PCH -DEE_USE_PCH)
    endif()

    # Public target
    if (DEFINED MOD_LIBRARIES)
        target_link_libraries(${MOD_NAME} PUBLIC ${MOD_LIBRARIES})
    endif()
    if (DEFINED MOD_INCLUDES)
        target_include_directories(${MOD_NAME} PUBLIC ${MOD_INCLUDES})
    endif()
    if (DEFINED MOD_LIBDIRS)
        target_link_directories(${MOD_NAME} PUBLIC ${MOD_LIBDIRS})
    endif()
    if (DEFINED MOD_DEFINES)
        target_compile_definitions(${MOD_NAME} PUBLIC ${MOD_DEFINES})
    endif()
    if (DEFINED MOD_GB_LIBS)
        target_link_libraries(${MOD_NAME} PUBLIC ${MOD_GB_LIBS})
    endif()
    # Private target
    if (DEFINED MOD_P_LIBRARIES)
        target_link_libraries(${MOD_NAME} PRIVATE ${MOD_P_LIBRARIES})
    endif()
    if (DEFINED MOD_P_INCLUDES)
        target_include_directories(${MOD_NAME} PRIVATE ${MOD_P_INCLUDES})
    endif()
    if (DEFINED MOD_P_LIBDIRS)
        target_link_directories(${MOD_NAME} PRIVATE ${MOD_P_LIBDIRS})
    endif()
    if (DEFINED MOD_P_DEFINES)
        target_compile_definitions(${MOD_NAME} PRIVATE ${MOD_P_DEFINES})
    endif()
    if (DEFINED MOD_P_GB_LIBS)
        target_link_libraries(${MOD_NAME} PRIVATE ${MOD_P_GB_LIBS})
    endif()

    if (GB3_ENABLE_DLL AND NOT MOD_NO_DLL) # for DLL modules
        add_library(${MOD_NAME}DLL SHARED ${MOD_SOURCES})
        set_target_properties(${MOD_NAME}DLL PROPERTIES FOLDER "${MOD_PREFIX}${MOD_DIR}")
        target_compile_definitions(${MOD_NAME}DLL
            PRIVATE 
                -DEE_${MOD_NAME_UP}_EXPORT=1
                -D${MOD_NAME_UP}_EXPORT=1
                -D_USRDLL
            INTERFACE
                -DEE_${MOD_NAME_UP}_IMPORT=1
                -D${MOD_NAME_UP}_IMPORT=1

        ) # dllspec define

        if (DEFINED MOD_PCH)
            target_precompile_headers(${MOD_NAME}DLL PRIVATE ${MOD_PCH})
            target_compile_definitions(${MOD_NAME}DLL PRIVATE -DNI_USE_PCH -DEE_USE_PCH)
        endif()
        # Public target
        if (DEFINED MOD_LIBRARIES)
            target_link_libraries(${MOD_NAME}DLL PUBLIC ${MOD_LIBRARIES})
        endif()
        if (DEFINED MOD_INCLUDES)
            target_include_directories(${MOD_NAME}DLL PUBLIC ${MOD_INCLUDES})
        endif()
        if (DEFINED MOD_LIBDIRS)
            target_link_directories(${MOD_NAME}DLL PUBLIC ${MOD_LIBDIRS})
        endif()
        if (DEFINED MOD_DEFINES)
            target_compile_definitions(${MOD_NAME}DLL PUBLIC ${MOD_DEFINES})
        endif()
        if (DEFINED MOD_GB_LIBS)
            foreach(lib IN ITEMS ${MOD_GB_LISTS})
                target_link_libraries(${MOD_NAME}DLL PUBLIC ${lib}DLL)
            endforeach()
        endif()
        # Private target
        if (DEFINED MOD_P_LIBRARIES)
            target_link_libraries(${MOD_NAME}DLL PRIVATE ${MOD_P_LIBRARIES})
        endif()
        if (DEFINED MOD_P_INCLUDES)
            target_include_directories(${MOD_NAME}DLL PRIVATE ${MOD_P_INCLUDES})
        endif()
        if (DEFINED MOD_P_LIBDIRS)
            target_link_directories(${MOD_NAME}DLL PRIVATE ${MOD_P_LIBDIRS})
        endif()
        if (DEFINED MOD_P_DEFINES)
            target_compile_definitions(${MOD_NAME}DLL PRIVATE ${MOD_P_DEFINES})
        endif()
        if (DEFINED MOD_P_GB_LIBS)
            foreach(lib IN ITEMS ${MOD_P_GB_LIBS})
                target_link_libraries(${MOD_NAME}DLL PRIVATE ${lib}DLL)
            endforeach()
        endif()
    endif()
endmacro()
