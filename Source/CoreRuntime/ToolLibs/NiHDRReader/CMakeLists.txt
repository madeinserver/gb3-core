add_library(NiHDRReader STATIC
    NiHDRReader.cpp
    NiHDRReader.h
    NiHDRReaderLibType.h
)

target_link_libraries(NiHDRReader PUBLIC NiDX9Renderer)
target_include_directories(NiHDRReader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiHDRReader PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/ToolLibs")
