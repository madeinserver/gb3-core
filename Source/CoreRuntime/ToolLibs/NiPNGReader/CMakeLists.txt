add_library(NiPNGReader STATIC
    NiPNGReader.cpp
    NiPNGReader.h
    NiPNGReaderLibType.h
)

target_link_libraries(NiPNGReader PUBLIC NiMain png)
target_include_directories(NiPNGReader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiPNGReader PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/ToolLibs")
