add_library(NiCursor STATIC
    NiCursor.cpp
    NiCursor.h
    NiCursor.inl
    NiCursorLibType.h
    NiCursorPCH.h
    NiCursorRenderClick.cpp
    NiCursorRenderClick.h
    NiCursorRenderClick.inl
    NiSystemCursor.cpp
    NiSystemCursor.h
    NiSystemCursor.inl
)

target_link_libraries(NiCursor PUBLIC NiMesh)
target_include_directories(NiCursor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiCursor PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/UtilityLibs")
target_precompile_headers(NiCursor PRIVATE NiCursorPCH.h)
