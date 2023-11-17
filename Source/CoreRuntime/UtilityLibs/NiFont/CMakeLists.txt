add_library(NiFont STATIC
    Ni2DString.cpp
    Ni2DString.h
    Ni2DString.inl
    Ni2DStringRenderClick.cpp
    Ni2DStringRenderClick.h
    Ni2DStringRenderClick.inl
    NiFont.cpp
    NiFont.h
    NiFont.inl
    NiFontLibType.h
    NiFontPCH.h
    NiFontString.cpp
    NiFontString.h
    NiFontString.inl
)

target_link_libraries(NiFont PUBLIC NiMesh)
target_include_directories(NiFont PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiFont PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/UtilityLibs")
target_precompile_headers(NiFont PRIVATE NiFontPCH.h)
